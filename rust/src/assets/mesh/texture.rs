// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   texture.rs - I3D texture decoding                   *
// *************************************************************************

//! I3D surface descriptors and texture frames (3dimage.cpp).
use super::{count, pointer};
use crate::assets::binary::Bytes;
use anyhow::{Result, ensure};

#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
#[repr(u32)]
pub enum TextureLighting {
    #[default]
    Surface = 0,
    RetailRgb = 1,
    RetailAlpha = 2,
}

#[derive(Debug)]
pub struct Texture {
    pub lighting: TextureLighting,
    pub width: usize,
    pub height: usize,
    pub pixels: Vec<u32>,
}

impl Texture {
    /// Authored I3D UVs may exceed [0, 1] even on ordinary character geometry
    /// (Jason's shoulder uses U=1.15..1.25). Repeat addressing matches these
    /// atlases and i3dgltf.cpp's wrapS/wrapT=10497; clamping creates false seams.
    pub fn sample(&self, uv: [f64; 2]) -> u32 {
        let x = (uv[0].rem_euclid(1.0) * self.width as f64) as usize;
        let y = (uv[1].rem_euclid(1.0) * self.height as f64) as usize;
        self.pixels[y.min(self.height - 1) * self.width + x.min(self.width - 1)]
    }
}

pub(super) fn decode(b: Bytes<'_>, at: usize, budget: &mut usize) -> Result<Vec<Texture>> {
    decode_fields(b, at, at + 108, at + 112, at + 116, budget)
}

/// Old bodies keep descriptors and frame/palette pointers in separate arrays.
pub(super) fn decode_fields(
    b: Bytes<'_>,
    at: usize,
    bits_field: usize,
    palette_field: usize,
    frames_field: usize,
    budget: &mut usize,
) -> Result<Vec<Texture>> {
    let height = count(b, at + 8, 8192)?;
    let width = count(b, at + 12, 8192)?;
    let frames = count(b, frames_field, 4096)?;
    ensure!(
        width > 0 && height > 0 && frames > 0,
        "empty texture dimensions or frames"
    );
    let pixels = width * height;
    let total = pixels
        .checked_mul(frames)
        .ok_or_else(|| anyhow::anyhow!("texture size overflow"))?;
    *budget = budget
        .checked_sub(total)
        .ok_or_else(|| anyhow::anyhow!("mesh exceeds texture pixel budget"))?;
    let bits = b.u32(at + 84)?;
    ensure!(
        [8, 16, 24, 32].contains(&bits),
        "unsupported texture bit depth {bits}"
    );
    let bytes_per_pixel = bits as usize / 8;
    let pitch = b.i32(at + 16)?;
    let pitch = if pitch <= 0 {
        width * bytes_per_pixel
    } else {
        pitch as usize
    };
    ensure!(
        pitch >= width * bytes_per_pixel,
        "texture pitch is shorter than a row"
    );
    let masks = [
        b.u32(at + 88)?,
        b.u32(at + 92)?,
        b.u32(at + 96)?,
        b.u32(at + 100)?,
    ];
    if bits != 8 {
        ensure!(
            masks[..3].iter().all(|&m| m != 0),
            "missing texture color masks"
        );
        let mut used = 0u32;
        for &mask in &masks {
            ensure!(used & mask == 0, "overlapping texture color masks");
            ensure!(
                bits == 32 || mask >> bits == 0,
                "texture mask exceeds bit depth"
            );
            if mask != 0 {
                let shifted = mask >> mask.trailing_zeros();
                ensure!(
                    shifted & shifted.wrapping_add(1) == 0,
                    "non-contiguous texture color mask"
                );
            }
            used |= mask;
        }
    }
    let data_at = pointer(b, bits_field)?;
    b.slice(data_at, frames * 4)?;
    let palette_at = if bits == 8 {
        Some(pointer(b, palette_field)?)
    } else {
        None
    };
    if let Some(at) = palette_at {
        b.slice(at, 512)?;
    }
    let mut result = Vec::with_capacity(frames);
    for frame in 0..frames {
        let source = pointer(b, data_at + frame * 4)?;
        b.slice(source, (height - 1) * pitch + width * bytes_per_pixel)?;
        let palette = palette_at;
        let mut colors = Vec::with_capacity(pixels);
        for y in 0..height {
            for x in 0..width {
                let at = source + y * pitch + x * bytes_per_pixel;
                let color = if let Some(palette) = palette {
                    let p = u32::from(b.u16(palette + usize::from(b.u8(at)?) * 2)?);
                    0xff000000 | expand(p, 0x7c00) << 16 | expand(p, 0x3e0) << 8 | expand(p, 31)
                } else {
                    let raw = b
                        .slice(at, bytes_per_pixel)?
                        .iter()
                        .enumerate()
                        .fold(0u32, |v, (i, &byte)| v | u32::from(byte) << (i * 8));
                    let alpha = if masks[3] == 0 {
                        255
                    } else {
                        expand(raw, masks[3])
                    };
                    alpha << 24
                        | expand(raw, masks[0]) << 16
                        | expand(raw, masks[1]) << 8
                        | expand(raw, masks[2])
                };
                colors.push(color);
            }
        }
        // Match the current C++ atlas heuristic, including its strict >20% threshold.
        if colors.iter().filter(|&&p| p & 0xffffff == 0).count() * 5 > pixels {
            for color in &mut colors {
                if *color & 0xffffff == 0 {
                    *color = 0;
                }
            }
        }
        result.push(Texture {
            lighting: if masks[3] == 0 {
                TextureLighting::RetailRgb
            } else {
                TextureLighting::RetailAlpha
            },
            width,
            height,
            pixels: colors,
        });
    }
    Ok(result)
}

fn expand(value: u32, mask: u32) -> u32 {
    if mask == 0 {
        return 0;
    }
    let shift = mask.trailing_zeros();
    let max = u64::from(mask >> shift);
    ((u64::from((value & mask) >> shift) * 255 + max / 2) / max) as u32
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn packed_channels_use_rounded_expansion() {
        assert_eq!(expand(0x800, 0xf800), 8);
        assert_eq!(expand(0x8000, 0xf800), 132);
        assert_eq!(expand(0xffffffff, 0xffffffff), 255);
    }

    #[test]
    fn authored_uvs_repeat_in_both_directions_including_negative_coordinates() {
        let texture = Texture {
            lighting: TextureLighting::Surface,
            width: 2,
            height: 2,
            pixels: vec![11, 22, 33, 44],
        };
        assert_eq!(texture.sample([1.25, 0.25]), 11);
        assert_eq!(texture.sample([-0.25, 1.25]), 22);
        assert_eq!(texture.sample([0.25, -0.25]), 33);
        assert_eq!(texture.sample([2.75, 3.75]), 44);
        assert_eq!(texture.sample([1.0, 1.0]), 11);
    }
}
