// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             motion_reference.rs - Native motion compatibility         *
// *************************************************************************

//! Compile native motion structs, conversion, GetMotion and ConvertToVector in
//! a standalone harness. Retail bytes travel over stdin and never get extracted.
use anyhow::{Context, Result, ensure};
use revenant::assets::{
    mesh::{Mesh, motion::Motion},
    vfs::Vfs,
};
use std::{
    fs,
    io::Write,
    path::Path,
    process::{Command, Stdio},
};

fn section<'a>(source: &'a str, start: &str, end: &str) -> Result<&'a str> {
    let at = source
        .find(start)
        .with_context(|| format!("missing native section {start}"))?;
    let length = source[at..]
        .find(end)
        .with_context(|| format!("missing native delimiter {end}"))?;
    Ok(&source[at..at + length])
}

#[test]
#[ignore = "requires local retail assets, sibling C++ source and CXX or c++"]
fn retail_motion_and_all_facings_match_native_cpp() -> Result<()> {
    let src = Path::new(env!("CARGO_MANIFEST_DIR")).join("../src");
    let body = fs::read_to_string(src.join("3dimagebody.h"))?;
    let image = fs::read_to_string(src.join("3dimage.cpp"))?;
    let object = fs::read_to_string(src.join("object.cpp"))?;
    let color = fs::read_to_string(src.join("colortable.cpp"))?;
    let structs = section(&body, "struct SOldOldMotionData", "#define md_setdist")?
        .lines()
        .filter(|line| !line.trim_start().starts_with("_STRUCTDEF"))
        .collect::<Vec<_>>()
        .join("\n");
    let get_motion = section(
        &image,
        "bool T3DImagery::GetMotion(",
        "void T3DImagery::SetObjectMotion(",
    )?
    .replace("T3DImagery::", "");
    let convert = section(&object, "void ConvertToVector(", "int32_t ConvertToFacing(")?;
    let table = section(
        &color,
        "for (loop = 0; loop < 64; loop++)",
        "signed char *coscosidx",
    )?;
    let initialization = section(
        &image,
        "if (mesh->flags & I3D_ROOTMOTION)",
        "\n            }\n        }\n    }\n\n    // Parse immutable controller metadata",
    )?;
    let prefix = r#"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>
#include <stdexcept>
#define I3D_ROOTMOTION 64
#define I3D_FACINGMOTION 8
constexpr double M_2PI = 6.28318530717958647692;
int32_t Round(double value) { return int32_t(std::floor(value + 0.5)); }
short DistX[256], DistY[256];
struct S3DPoint { int32_t x, y, z; };
struct State { int32_t frames; };
struct Header { int32_t numstates; std::vector<State> states; } header;
Header* GetHeader() { return &header; }
int32_t GetAniLength(int32_t state) { return header.states.at(state).frames; }
struct Pointer { const uint8_t* value; const uint8_t* ptr() { return value; } };
struct ImageState { Pointer motion; };
struct ImageBody { uint32_t flags; };
std::vector<uint8_t> bytes;
uint32_t u32(size_t at) { uint32_t v; for (size_t i=0;i<4;i++) (void)bytes.at(at+i); std::memcpy(&v, &bytes.at(at),4); return v; }
uint16_t u16(size_t at) { uint16_t v; (void)bytes.at(at+1); std::memcpy(&v, &bytes.at(at),2); return v; }
size_t relative(size_t at) { int32_t d = int32_t(u32(at)); return d ? size_t(int64_t(at)+d) : 0; }
"#;
    let main = r#"
int main() {
    uint8_t buffer[8192]; size_t count;
    while ((count=std::fread(buffer,1,sizeof(buffer),stdin)) != 0) bytes.insert(bytes.end(),buffer,buffer+count);
    setup_vectors();
    for (int angle=0;angle<256;angle++) {
        S3DPoint p; ConvertToVector(angle,1480<<8,p,0);
        std::printf("V %d %d %d %d\n",angle,p.x,p.y,p.z);
    }
    size_t h=20, b=h+u32(16)+u16(4)*4;
    ImageBody image{u32(b)}; ImageBody* mesh=&image;
    uint32_t version=u32(b+4); size_t stride=version==2?80:16;
    header.numstates=u32(h+4); header.states.resize(header.numstates);
    motion.resize(header.numstates,nullptr);
    std::vector<std::vector<SMotionData>> storage(header.numstates);
    size_t states=relative(b+8);
    for (int c=0;c<header.numstates;c++) {
        header.states[c].frames=u16(h+8+c*76+42);
        size_t at=states ? relative(states+c*stride+12) : 0;
        if (at) {
            storage[c].resize(GetAniLength(c)); motion[c]=storage[c].data();
            ImageState state{{&bytes.at(at)}}; ImageState* ist=&state;
            initialize_state(c,mesh,ist);
        }
        for (int frame=0;frame<=GetAniLength(c);frame++) {
            int d,v,a,x,y,z; bool ok=GetMotion(c,frame,d,v,a,x,y,z);
            std::printf("M %d %d %d %d %d %d %d %d %d\n",c,frame,int(ok),d,v,a,x,y,z);
        }
    }
}
"#;
    let harness = format!(
        "{prefix}\n{structs}\nstd::vector<SMotionData*> motion;\n{get_motion}\n{convert}\nvoid setup_vectors() {{ int32_t loop; {table} }}\nvoid initialize_state(int c, ImageBody* mesh, ImageState* ist) {{ {initialization} }}\n{main}"
    );
    let temp = tempfile::tempdir()?;
    let cpp = temp.path().join("motion.cpp");
    let binary = temp.path().join("motion");
    fs::write(&cpp, harness)?;
    let result = Command::new(std::env::var_os("CXX").unwrap_or_else(|| "c++".into()))
        .arg("-std=c++17")
        .arg(&cpp)
        .arg("-o")
        .arg(&binary)
        .output()?;
    ensure!(
        result.status.success(),
        "native motion harness: {}",
        String::from_utf8_lossy(&result.stderr)
    );
    let mut vfs = Vfs::open(Vfs::discover(None)?, "Ahkuilon", None)?;
    let mut checked = 0;
    for path in ["Chars/locke.i3d", "Chars/jason.i3d", "Equip/ShortSword.i3d"] {
        let data = vfs.read(&format!("imagery/{path}"))?;
        let mesh = Mesh::decode(&data)?;
        let mut child = Command::new(&binary)
            .stdin(Stdio::piped())
            .stdout(Stdio::piped())
            .stderr(Stdio::piped())
            .spawn()?;
        child
            .stdin
            .take()
            .context("native stdin")?
            .write_all(&data)?;
        let output = child.wait_with_output()?;
        ensure!(
            output.status.success(),
            "native harness failed: {}",
            String::from_utf8_lossy(&output.stderr)
        );
        let output = String::from_utf8(output.stdout)?;
        let mut vectors = 0;
        let mut records = 0;
        for line in output.lines() {
            let (kind, values) = line.split_once(' ').context("invalid harness row")?;
            let values: Vec<i32> = values
                .split_whitespace()
                .map(str::parse)
                .collect::<std::result::Result<_, _>>()?;
            match kind {
                "V" => {
                    let delta = Motion {
                        distance: 1480,
                        ..Motion::default()
                    }
                    .displacement(values[0] as u8);
                    assert_eq!(delta, values[1..], "facing {}", values[0]);
                    vectors += 1;
                }
                "M" => {
                    ensure!(values.len() == 9, "bad motion row");
                    let sample = mesh.clips[values[0] as usize]
                        .motion
                        .get(values[1] as usize);
                    assert_eq!(
                        i32::from(sample.is_some()),
                        values[2],
                        "{path}: motion presence"
                    );
                    let m = sample.copied().unwrap_or_default();
                    assert_eq!(
                        [
                            i32::from(m.distance) * 256,
                            i32::from(m.vertical) * 256,
                            i32::from(m.angle),
                            i32::from(m.rotation[0]),
                            i32::from(m.rotation[1]),
                            i32::from(m.rotation[2])
                        ],
                        values[3..],
                        "{path}: state {} frame {}",
                        values[0],
                        values[1]
                    );
                    records += 1;
                }
                _ => anyhow::bail!("unknown native row {kind}"),
            }
        }
        assert_eq!(vectors, 256);
        ensure!(records > 0, "no native motion records");
        checked += records;
    }
    println!("Compared {checked} native motion samples/bounds and all 256 facing vectors");
    Ok(())
}
