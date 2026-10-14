// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     font.rs - Retail bitmap fonts                     *
// *************************************************************************

use super::{bitmap::Bitmap, resource::Resource};
use anyhow::{Result, ensure};

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
}

#[cfg(test)]
mod tests {
    use super::*;

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
    }
}
