// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                font.rs - Bitmap and named Windows fonts               *
// *************************************************************************

use super::{bitmap::Bitmap, resource::Resource};
use anyhow::{Context, Result, bail, ensure};
mod gdi;

pub struct Glyph {
    pub bitmap: Bitmap,
    pub offset: [i32; 2],
    pub advance: i32,
}

pub struct Font {
    pub height: i32,
    glyphs: Vec<Option<Glyph>>,
}

impl Font {
    /// Resolve the same named face as the DEF widget renderer. WINFONT sizes
    /// are GDI cell heights, not outline em sizes or bitmap-font names.
    pub fn named(vfs: &mut super::vfs::Vfs, name: &str) -> Result<Self> {
        let lines = super::definition::lines(&vfs.read("font.def")?)?;
        let line = lines
            .iter()
            .find(|line| line.get(1).is_some_and(|n| n.eq_ignore_ascii_case(name)))
            .with_context(|| format!("font.def has no font {name}"))?;
        if line[0] == "BMFONT" {
            let at = line
                .iter()
                .position(|s| s == "NORMAL")
                .context("bitmap font without NORMAL")?;
            return Self::decode(&vfs.read(&format!(
                "{}.dat",
                line.get(at + 1).context("missing bitmap font resource")?
            ))?);
        }
        ensure!(
            line[0] == "WINFONT" && line.get(2).is_some_and(|s| s == "FONT"),
            "unsupported font definition: {name}"
        );
        Self::windows(
            line.get(3).context("missing font face")?,
            line.get(4).context("missing font size")?.parse()?,
            line.iter().any(|s| s == "BOLD"),
        )
    }

    fn windows(name: &str, height: i32, bold: bool) -> Result<Self> {
        use ab_glyph::{Font as _, FontRef, point};
        let (cell, widths) = match (name, height, bold) {
            ("Arial", 16, true) => gdi::ARIAL_16,
            ("Arial", 12, false) => gdi::ARIAL_12,
            _ => bail!("unsupported WINFONT face/cell: {name} {height} bold={bold}"),
        };
        // Same redistributable face as C++ fonttable.cpp. Embedded so packaged
        // native binaries do not depend on a platform's installed font set.
        let face =
            FontRef::try_from_slice(include_bytes!("../../../assets/fonts/Arimo-Regular.ttf"))
                .context(
                    "loading bundled Arimo (fetch assets/fonts/Arimo-Regular.ttf with Git LFS)",
                )?;
        let scale = cell[0] as f32 * face.height_unscaled()
            / face.units_per_em().context("font has no em size")?;
        let mut glyphs: Vec<Option<Glyph>> = (0..256).map(|_| None).collect();
        for byte in 32..=255u8 {
            let glyph = face
                .glyph_id(char::from(byte))
                .with_scale_and_position(scale, point(0.0, 0.0));
            let (bitmap, offset) = if let Some(outline) = face.outline_glyph(glyph) {
                let bounds = outline.px_bounds();
                let width = bounds.width() as usize;
                let height = bounds.height() as usize;
                let mut pixels = vec![0; width * height];
                outline.draw(|x, y, coverage| {
                    // C++ draws WINFONT colour twice to match GDI's weight.
                    let alpha = ((1.0 - (1.0 - coverage).powi(2)) * 255.0).round() as u32;
                    pixels[y as usize * width + x as usize] = alpha << 24 | 0xffffff;
                });
                (
                    Bitmap {
                        width,
                        height,
                        registration: [0; 2],
                        pixels,
                    },
                    [
                        bounds.min.x as i32,
                        bounds.min.y as i32 + cell[1] - cell_height(cell),
                    ],
                )
            } else {
                (
                    Bitmap {
                        width: 1,
                        height: 1,
                        registration: [0; 2],
                        pixels: vec![0],
                    },
                    [0; 2],
                )
            };
            glyphs[usize::from(byte)] = Some(Glyph {
                bitmap,
                offset,
                advance: i32::from(widths[usize::from(byte - 32)]),
            });
        }
        Ok(Self {
            height: cell_height(cell),
            glyphs,
        })
    }
    pub fn decode(file: &[u8]) -> Result<Self> {
        let data = Resource::parse(file)?.body;
        data.slice(0, 2056)?;
        let first = data.i16(0)?;
        let count = data.i16(2)?;
        ensure!(
            first >= 0 && count >= 0 && i32::from(first) + i32::from(count) <= 256,
            "invalid font range"
        );
        let height = i32::from(data.i16(4)?);
        ensure!(height > 0, "invalid font height");
        let mut glyphs: Vec<Option<Glyph>> = (0..256).map(|_| None).collect();
        for (i, glyph) in glyphs
            .iter_mut()
            .skip(first as usize)
            .take(count as usize)
            .enumerate()
        {
            if let Some(at) = data.relative(1032 + i * 4)? {
                let left = i32::from(data.u8(518 + i)?);
                *glyph = Some(Glyph {
                    bitmap: Bitmap::decode_glyph(data, at)?,
                    offset: [-left, -i32::from(data.i16(6 + i * 2)?)],
                    advance: i32::from(data.u8(774 + i)?) - left,
                });
            }
        }
        Ok(Self { height, glyphs })
    }

    pub fn glyph(&self, c: char) -> Option<&Glyph> {
        self.glyphs
            .get(c as usize)
            .and_then(Option::as_ref)
            .or_else(|| self.glyphs[usize::from(b'?')].as_ref())
    }
    pub fn advance(&self, c: char) -> i32 {
        self.glyph(c).map_or(self.height / 2, |g| g.advance)
    }
    pub fn width(&self, text: &str) -> i32 {
        text.chars().map(|c| self.advance(c)).sum()
    }
    pub fn wrap(&self, text: &str, width: i32) -> Vec<String> {
        let mut lines = Vec::new();
        for paragraph in text.split('\n') {
            let mut line = String::new();
            for word in paragraph.split_whitespace() {
                let candidate = if line.is_empty() {
                    word.into()
                } else {
                    format!("{line} {word}")
                };
                if !line.is_empty() && self.width(&candidate) > width {
                    lines.push(std::mem::take(&mut line));
                }
                if !line.is_empty() {
                    line.push(' ');
                }
                line.push_str(word);
            }
            lines.push(line);
        }
        lines
    }
}

fn cell_height(cell: [i32; 3]) -> i32 {
    cell[1] + cell[2]
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn named_med_uses_gdi_cell_metrics_and_bundled_outlines() -> Result<()> {
        let dir = tempfile::tempdir()?;
        std::fs::write(
            dir.path().join("font.def"),
            b"WINFONT \"Med\" FONT \"Arial\" 16 BOLD COLOR 255 255 255\n",
        )?;
        let mut vfs = super::super::vfs::Vfs::open(dir.path().into(), "Ahkuilon", None)?;
        let font = Font::named(&mut vfs, "Med")?;
        assert_eq!(font.height, 16);
        assert_eq!(font.width("WW"), 26);
        let glyph = font.glyph('O').unwrap();
        assert!(glyph.bitmap.pixels.iter().any(|p| p >> 24 != 0));
        assert!(Font::named(&mut vfs, "missing").is_err());
        Ok(())
    }

    #[test]
    fn glyph_decode_includes_keyed_pixels_and_the_alias_edge() {
        let mut body = vec![0u8; 2056];
        body[0..2].copy_from_slice(&65i16.to_le_bytes());
        body[2..4].copy_from_slice(&1i16.to_le_bytes());
        body[4..6].copy_from_slice(&1i16.to_le_bytes());
        body[6..8].copy_from_slice(&1i16.to_le_bytes());
        body[774] = 2;
        body[1032..1036].copy_from_slice(&1024i32.to_le_bytes());
        let mut bitmap = vec![0u8; 72];
        bitmap[0..4].copy_from_slice(&2i32.to_le_bytes());
        bitmap[4..8].copy_from_slice(&1i32.to_le_bytes());
        bitmap[16..20].copy_from_slice(&0x82u32.to_le_bytes());
        bitmap[28..32].copy_from_slice(&6u32.to_le_bytes());
        bitmap[32..36].copy_from_slice(&44i32.to_le_bytes());
        bitmap[68..72].copy_from_slice(&4u32.to_le_bytes());
        bitmap.extend([0xff, 0x7f, 0, 0]);
        bitmap.extend([1, 1, 0xff, 0x7f, 15, 254]);
        body.extend(bitmap);
        let mut file = vec![0u8; 20];
        file[..4].copy_from_slice(b"CGSR");
        file[7] = 1;
        file[8..12].copy_from_slice(&(body.len() as u32).to_le_bytes());
        file[12..16].copy_from_slice(&(body.len() as u32).to_le_bytes());
        file.extend(body);
        let font = Font::decode(&file).unwrap();
        let glyph = font.glyph('A').unwrap();
        assert_eq!(glyph.advance, 2);
        assert_eq!(glyph.offset, [0, -1]);
        assert_eq!(glyph.bitmap.pixels, [0xfff8f8f8, 0x7bf8f8f8]);
        assert_eq!(font.wrap("A A\n\nA", 2), ["A", "A", "", "A"]);
    }
}
