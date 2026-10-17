// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             ui_assets.rs - Native UI pixel compatibility              *
// *************************************************************************

//! Compare the Rust readers with the actual C++ bitmap decoder and resource
//! structures. Retail bytes pass over stdin without extracting asset files.
use anyhow::{Context, Result, ensure};
use revenant::assets::{
    bitmap::{Bitmap, Decode},
    font::Font,
    multi::Multi,
    vfs::Vfs,
};
use std::{
    fs,
    io::Write,
    path::{Path, PathBuf},
    process::{Command, Stdio},
};

fn without_includes(source: &str) -> String {
    source
        .lines()
        .filter(|line| {
            let line = line.trim_start();
            !line.starts_with("#include") && !line.starts_with("#pragma once")
        })
        .collect::<Vec<_>>()
        .join("\n")
}

fn native_harness(directory: &Path) -> Result<PathBuf> {
    let src = Path::new(env!("CARGO_MANIFEST_DIR")).join("../src");
    let types = fs::read_to_string(src.join("revtypes.h"))?;
    let offset_start = types
        .find("class OFFSET\n")
        .context("native OFFSET class")?;
    let offset_end = types[offset_start..]
        .find("template <class T>\nclass TIterator;")
        .context("native OFFSET delimiter")?
        + offset_start;
    let definitions = fs::read_to_string(src.join("revdefs.h"))?;
    let definitions = definitions
        .lines()
        .filter(|line| {
            line.starts_with("#define BM_")
                || line.starts_with("#define AL_")
                || line.starts_with("#define MAXFONTCHARS")
        })
        .collect::<Vec<_>>()
        .join("\n");
    let mut harness = String::from(
        r#"
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <strings.h>
#include <cstdlib>
#include <vector>
#include <stdexcept>
#define _CLASSDEF(T) class T; typedef T* P##T; typedef TOffset<T> O##T;
#define _STRUCTDEF(T) struct T; typedef T* P##T; typedef TOffset<T> O##T;
#define stricmp strcasecmp
void FatalError(const char* message) { throw std::runtime_error(message); }
"#,
    );
    harness.push_str(&types[offset_start..offset_end]);
    harness.push_str(&definitions);
    for file in ["resourcehdr.h", "bitmapdata.h", "multidat.h", "fontdata.h"] {
        harness.push('\n');
        harness.push_str(&without_includes(&fs::read_to_string(src.join(file))?));
    }
    harness.push_str(
        r#"
using TBitmap = TBitmapData;
using PTBitmap = TBitmap*;
class TMulti : public TMultiData { public: void* Find(const char* name); void* Object(const char* name); };
static_assert(sizeof(FileResHdr) == 20);
static_assert(offsetof(TBitmapData, data8) == 72);
static_assert(sizeof(TMultiData) == 2052);
static_assert(offsetof(TFontData, chars) == 1032);
"#,
    );
    harness.push_str(&without_includes(&fs::read_to_string(
        src.join("multi.cpp"),
    )?));
    harness.push_str(&without_includes(&fs::read_to_string(
        src.join("bitmapdecode.h"),
    )?));
    harness.push_str(&without_includes(&fs::read_to_string(
        src.join("bitmapdecode.cpp"),
    )?));
    harness.push_str(r#"
void value(int32_t v) { if (std::fwrite(&v, sizeof(v), 1, stdout) != 1) throw std::runtime_error("stdout"); }
void bitmap(PTBitmap bm, EBitmapDecode decode) {
    if (!bm || bm->width <= 0 || bm->height <= 0 || bm->width > 4096 || bm->height > 4096)
        throw std::runtime_error("invalid bitmap dimensions");
    value(bm->width); value(bm->height); value(bm->regx); value(bm->regy);
    std::vector<uint8_t> rgba(size_t(bm->width) * bm->height * 4);
    if (!DecodeBitmapToRGBA(bm, rgba.data(), bm->width * 4, 0, 0, decode))
        throw std::runtime_error("native bitmap decode failed");
    for (size_t i = 0; i < rgba.size(); i += 4) {
        uint32_t argb = uint32_t(rgba[i+3]) << 24 | uint32_t(rgba[i]) << 16
            | uint32_t(rgba[i+1]) << 8 | rgba[i+2];
        value(int32_t(argb));
    }
}
int main(int argc, char** argv) {
    try {
        std::vector<uint8_t> bytes;
        uint8_t buffer[8192]; size_t count;
        // Read all input first, so a large RGBA reply cannot block stdin writes.
        while ((count = std::fread(buffer, 1, sizeof(buffer), stdin)) != 0)
            bytes.insert(bytes.end(), buffer, buffer + count);
        if (bytes.size() < sizeof(FileResHdr) || argc < 3) throw std::runtime_error("input");
        auto* header = reinterpret_cast<FileResHdr*>(bytes.data());
        if (header->resmagic != RESMAGIC || header->comptype != COMP_NONE || header->version != RESVERSION)
            throw std::runtime_error("resource header");
        size_t offset = sizeof(FileResHdr) + header->hdrsize + size_t(header->topbm) * sizeof(uint32_t);
        if (offset + header->datasize > bytes.size()) throw std::runtime_error("resource size");
        auto* body = bytes.data() + offset;
        for (int i = 2; i < argc; ++i) {
            if (std::strcmp(argv[1], "font") == 0) {
                auto* font = reinterpret_cast<TFontData*>(body);
                int index = std::atoi(argv[i]) - font->firstchar;
                if (index < 0 || index >= font->numchars) throw std::runtime_error("font range");
                value(font->height); value(-font->drawleft[index]); value(-font->startheight[index]);
                value(int(font->drawright[index]) - font->drawleft[index]);
                bitmap(reinterpret_cast<PTBitmap>(font->chars[index].ptr()), EBitmapDecode::PixelsAndAlias);
            } else {
                const char prefix = argv[i][0];
                const auto mode = prefix == '@' ? EBitmapDecode::Alias : prefix == '!' ? EBitmapDecode::Unkeyed : prefix == '%' ? EBitmapDecode::Overlay4444 : prefix == '+' ? EBitmapDecode::MagentaKeyed : EBitmapDecode::Pixels;
                auto* multi = reinterpret_cast<TMulti*>(body);
                bitmap(reinterpret_cast<PTBitmap>(multi->Object(argv[i] + (mode != EBitmapDecode::Pixels ? 1 : 0))), mode);
            }
        }
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what()); return 1;
    }
}
"#);
    let cpp = directory.join("ui_assets.cpp");
    let binary = directory.join("ui_assets");
    fs::write(&cpp, harness)?;
    let compiled = Command::new(std::env::var_os("CXX").unwrap_or_else(|| "c++".into()))
        .arg("-std=c++17")
        .arg(&cpp)
        .arg("-o")
        .arg(&binary)
        .output()?;
    ensure!(
        compiled.status.success(),
        "native UI harness: {}",
        String::from_utf8_lossy(&compiled.stderr)
    );
    Ok(binary)
}

fn reference(binary: &Path, data: &[u8], kind: &str, names: &[&str]) -> Result<Vec<u8>> {
    let mut child = Command::new(binary)
        .arg(kind)
        .args(names)
        .stdin(Stdio::piped())
        .stdout(Stdio::piped())
        .stderr(Stdio::piped())
        .spawn()?;
    child
        .stdin
        .take()
        .context("native UI stdin")?
        .write_all(data)?;
    let result = child.wait_with_output()?;
    ensure!(
        result.status.success(),
        "native UI decoder: {}",
        String::from_utf8_lossy(&result.stderr)
    );
    Ok(result.stdout)
}

fn number(data: &mut &[u8]) -> Result<i32> {
    let bytes = data.get(..4).context("truncated native UI output")?;
    let value = i32::from_le_bytes(bytes.try_into()?);
    *data = &data[4..];
    Ok(value)
}

fn compare(data: &mut &[u8], rust: &Bitmap, name: &str) -> Result<usize> {
    assert_eq!(number(data)?, rust.width as i32, "{name} width");
    assert_eq!(number(data)?, rust.height as i32, "{name} height");
    assert_eq!(
        [number(data)?, number(data)?],
        rust.registration,
        "{name} registration"
    );
    for (i, &expected) in rust.pixels.iter().enumerate() {
        assert_eq!(
            number(data)? as u32,
            expected,
            "{name} pixel ({}, {})",
            i % rust.width,
            i / rust.width
        );
    }
    Ok(rust.pixels.len())
}

#[test]
#[ignore = "requires retail assets, sibling C++ source and CXX or c++"]
fn retail_cursor_hud_and_font_pixels_match_native_cpp() -> Result<()> {
    let temporary = tempfile::tempdir()?;
    let binary = native_harness(temporary.path())?;
    let mut vfs = Vfs::open(Vfs::discover(None)?, "Ahkuilon", None)?;
    let mut pixels = 0;
    let mut bitmaps = 0;
    for (file, names) in [
        (
            "playscrn.dat",
            &[
                "cursor",
                "handcursor",
                "@cursorshadow",
                "wedge-ne",
                "wedge-e",
                "wedge-se",
                "wedge-s",
                "wedge-sw",
                "wedge-w",
                "wedge-nw",
                "wedge-n",
                "@wedge-neshadow",
                "@wedge-eshadow",
                "@wedge-seshadow",
                "@wedge-sshadow",
                "@wedge-swshadow",
                "@wedge-wshadow",
                "@wedge-nwshadow",
                "@wedge-nshadow",
            ][..],
        ),
        (
            "statusbarnotex.dat",
            &[
                "BackPanel",
                "Bars",
                "HealthIcon",
                "ManaIcon",
                "FatigueIcon",
                "Ring",
            ][..],
        ),
        (
            "statusbar.dat",
            &[
                "BackPanel",
                "Bars",
                "HealthIcon",
                "ManaIcon",
                "FatigueIcon",
                "Ring",
            ][..],
        ),
        ("sidebartabs.dat", &["Up", "Down", "Select"][..]),
        (
            "widgetsalpha.dat",
            &["!FillFrame1", "!CheckU", "!HScrollBar"][..],
        ),
        (
            "widgetstex.dat",
            &["!FillFrame1", "!CheckU", "!HScrollBar"][..],
        ),
        ("portraits.dat", &["LockeFace", "%LockeFace"][..]),
        (
            "bottombar.dat",
            &["!UtilityBar", "!BarEndCap", "!BarInvBox"][..],
        ),
        ("sidebartabsnotex.dat", &["Up", "Down", "Select"][..]),
        (
            "equippane.dat",
            &[
                "Equip", "Head", "Neck", "Chest", "Hand2", "Hand", "Ring", "Ring2", "Range",
                "Ammo", "Legs", "Feet",
            ][..],
        ),
        (
            "inventory.dat",
            &["Inventory", "Backpack", "GoldPile", "InvArwLG", "InvArwRG"][..],
        ),
        ("statspane.dat", &["Stats"][..]),
        ("spellscroll.dat", &["Scroll"][..]),
        ("spellpane.dat", &["spellconstr"][..]),
        ("automap.dat", &["Amap", "Marker"][..]),
        ("spellicons.dat", &["!RingG"][..]),
    ] {
        let bytes = vfs.read(file)?;
        let multi = Multi::parse(&bytes)?;
        let native = reference(&binary, &bytes, "bitmap", names).with_context(|| file)?;
        let mut native = native.as_slice();
        for &name in names {
            let rust = if let Some(name) = name.strip_prefix('@') {
                multi.bitmap_alias(name)?
            } else if let Some(name) = name.strip_prefix('!') {
                multi.bitmap_mode(name, Decode::Unkeyed)?
            } else if let Some(name) = name.strip_prefix('%') {
                multi.bitmap_mode(name, Decode::Overlay4444)?
            } else if let Some(name) = name.strip_prefix('+') {
                multi.bitmap_mode(name, Decode::MagentaKeyed)?
            } else {
                multi.bitmap(name)?
            };
            pixels += compare(&mut native, &rust, &format!("{file}/{name}"))?;
            bitmaps += 1;
        }
        ensure!(native.is_empty(), "extra native bitmap output for {file}");
    }
    let bytes = vfs.read("sysfont.dat")?;
    let font = Font::decode(&bytes)?;
    // Retail sysfont contains space through the cursor glyph ('z' + 1).
    let codes: Vec<_> = (32u8..=123).map(|c| c.to_string()).collect();
    let names: Vec<_> = codes.iter().map(String::as_str).collect();
    let native = reference(&binary, &bytes, "font", &names)?;
    let mut native = native.as_slice();
    for code in 32u8..=123 {
        let glyph = font
            .glyph(char::from(code))
            .context("missing system glyph")?;
        assert_eq!(number(&mut native)?, font.height);
        assert_eq!([number(&mut native)?, number(&mut native)?], glyph.offset);
        assert_eq!(number(&mut native)?, glyph.advance);
        pixels += compare(
            &mut native,
            &glyph.bitmap,
            &format!("sysfont.dat/{}", char::from(code)),
        )?;
        bitmaps += 1;
    }
    ensure!(native.is_empty(), "extra native glyph output");
    println!(
        "Compared {bitmaps} native UI bitmaps/glyphs and {pixels} RGBA pixels, including registration and font metrics"
    );
    Ok(())
}
