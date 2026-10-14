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
                    bitmap: Bitmap::decode(data, at, false)?,
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
