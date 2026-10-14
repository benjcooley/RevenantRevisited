// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  bitmap.rs - Native UI bitmap decoder                 *
// *************************************************************************

//! Owned UI pixels, matching bitmapdecode.cpp rather than I2D depth tiles.
use super::binary::{Bytes, Reader};
use anyhow::{Context, Result, ensure};

#[derive(Clone, Debug)]
pub struct Bitmap {
    pub width: usize,
    pub height: usize,
    pub registration: [i32; 2],
    /// Straight-alpha AARRGGBB pixels.
    pub pixels: Vec<u32>,
}

impl Bitmap {
    pub(crate) fn decode(data: Bytes<'_>, at: usize, alias: bool) -> Result<Self> {
        data.slice(at, 72)?;
        let width = data.i32(at)?;
        let height = data.i32(at + 4)?;
        ensure!(width > 0 && height > 0, "empty bitmap");
        let (width, height) = (width as usize, height as usize);
        let count = width.checked_mul(height).context("bitmap size overflow")?;
        ensure!(count <= 16 * 1024 * 1024, "bitmap exceeds pixel budget");
        let flags = data.u32(at + 16)?;
        ensure!(
            flags & 0xc000 == 0,
            "compressed/chunked UI bitmap is unsupported"
        );
        let mut bitmap = Self {
            width,
            height,
            registration: [data.i32(at + 8)?, data.i32(at + 12)?],
            pixels: vec![0; count],
        };
        let rgb565 = flags & 4 != 0;
        if alias && flags & 128 != 0 {
            let start = data.relative(at + 32)?.context("missing alias stream")?;
            let mut stream = Reader::new(data.slice(start, data.u32(at + 28)? as usize)?);
            for y in 0..height {
                let mut code = stream.u8()?;
                if code == 255 {
                    break;
                }
                let mut x = 0;
                while code != 254 {
                    ensure!(code != 255, "alias end-of-data inside row");
                    x += usize::from(code);
                    let run = usize::from(stream.u8()?);
                    ensure!(x + run <= width, "alias run exceeds bitmap width");
                    for _ in 0..run {
                        let color = stream.u16()?;
                        let coverage = u32::from(stream.u8()?);
                        ensure!(coverage <= 31, "invalid alias coverage");
                        bitmap.pixels[y * width + x] =
                            (coverage * 255 / 31) << 24 | decode_color(color, rgb565);
                        x += 1;
                    }
                    code = stream.u8()?;
                }
            }
            return Ok(bitmap);
        }
        let bits = flags & 7;
        ensure!(
            bits != 0 || flags & 0x10000 != 0,
            "unsupported UI bitmap format {flags:#x}"
        );
        let stride = if bits & 1 != 0 { 1 } else { 2 };
        let pixels = Bytes(data.slice(at + 72, data.u32(at + 68)? as usize)?);
        pixels.slice(0, count * stride)?;
        let palette = if bits & 1 != 0 {
            let start = data.relative(at + 64)?.context("missing bitmap palette")?;
            Some(Bytes(data.slice(start + 512, 1024)?))
        } else {
            None
        };
        let alpha = if flags & 256 != 0 {
            let start = data.relative(at + 40)?.context("missing bitmap alpha")?;
            ensure!(data.u32(at + 36)? as usize >= count, "short bitmap alpha");
            Some(data.slice(start, count)?)
        } else {
            None
        };
        let key = data.u32(at + 24)? as u16;
        for (i, pixel) in bitmap.pixels.iter_mut().enumerate() {
            if let Some(palette) = palette {
                let index = pixels.u8(i)?;
                if index != key as u8 {
                    let c = palette.u32(usize::from(index) * 4)?;
                    *pixel = 0xff000000 | (c & 255) << 16 | (c & 0xff00) | (c >> 16 & 255);
                }
                continue;
            }
            let color = pixels.u16(i * 2)?;
            if flags & 0x10000 != 0 {
                let c = u32::from(color);
                *pixel = ((c >> 12) * 17) << 24
                    | ((c >> 8 & 15) * 17) << 16
                    | ((c >> 4 & 15) * 17) << 8
                    | ((c & 15) * 17);
                continue;
            }
            let a = alpha.map_or(255, |alpha| {
                let a = u32::from(alpha[i] & 31);
                a << 3 | a >> 2
            });
            if a != 0 && color != key && color != if rgb565 { 0xf81f } else { 0x7c1f } {
                *pixel = a << 24 | decode_color(color, rgb565);
            }
        }
        Ok(bitmap)
    }
}

fn decode_color(color: u16, rgb565: bool) -> u32 {
    let c = u32::from(color);
    if rgb565 {
        let (r, g, b) = (c >> 11, c >> 5 & 63, c & 31);
        (r << 3 | r >> 2) << 16 | (g << 2 | g >> 4) << 8 | (b << 3 | b >> 2)
    } else {
        (c >> 10 & 31) << 19 | (c >> 5 & 31) << 11 | (c & 31) << 3
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    fn fixture(flags: u32, colors: &[u16]) -> Vec<u8> {
        let mut data = vec![0; 72];
        data[0..4].copy_from_slice(&(colors.len() as i32).to_le_bytes());
        data[4..8].copy_from_slice(&1i32.to_le_bytes());
        data[16..20].copy_from_slice(&flags.to_le_bytes());
        data[68..72].copy_from_slice(&((colors.len() * 2) as u32).to_le_bytes());
        data.extend(colors.iter().flat_map(|c| c.to_le_bytes()));
        data
    }
    #[test]
    fn native_color_formats_keys_and_truncation() {
        let data = fixture(2, &[0x7fff, 0x7c1f, 0]);
        assert_eq!(
            Bitmap::decode(Bytes(&data), 0, false).unwrap().pixels,
            [0xfff8f8f8, 0, 0]
        );
        let data = fixture(4, &[0xffff, 0xf81f, 0x07e0]);
        assert_eq!(
            Bitmap::decode(Bytes(&data), 0, false).unwrap().pixels,
            [0xffffffff, 0, 0xff00ff00]
        );
        let data = fixture(0x10000, &[0xfda5, 0x8000]);
        assert_eq!(
            Bitmap::decode(Bytes(&data), 0, false).unwrap().pixels,
            [0xffddaa55, 0x88000000]
        );
        assert!(Bitmap::decode(Bytes(&data[..data.len() - 1]), 0, false).is_err());
    }
    #[test]
    fn separate_alpha_and_explicit_alias_path() {
        let mut data = fixture(0x182, &[0x7fff, 0x7fff]);
        data[36..40].copy_from_slice(&2u32.to_le_bytes());
        data[40..44].copy_from_slice(&36i32.to_le_bytes());
        data.extend([15, 31]);
        assert_eq!(
            Bitmap::decode(Bytes(&data), 0, false).unwrap().pixels,
            [0x7bf8f8f8, 0xfff8f8f8]
        );
        data[28..32].copy_from_slice(&6u32.to_le_bytes());
        data[32..36].copy_from_slice(&46i32.to_le_bytes());
        data.extend([1, 1, 0, 0x7c, 15, 254]);
        assert_eq!(
            Bitmap::decode(Bytes(&data), 0, true).unwrap().pixels,
            [0, 0x7bf80000]
        );
        data[78] = 2;
        assert!(Bitmap::decode(Bytes(&data), 0, true).is_err());
    }
}
