// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              motion.rs - Authored per-frame object movement           *
// *************************************************************************

//! SMotionData and its two 32-bit predecessors (3dimagebody.h). These are
//! per-frame object deltas, separate from the skeleton's local pose tracks.
use crate::assets::binary::Bytes;
use anyhow::Result;
use std::sync::OnceLock;

pub const FIXED_ONE: i32 = 1 << 16; // revdefs.h::ROLLOVER

#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub struct Motion {
    /// Unsigned 8.8 world distance; md_dist expands this to 16.16.
    pub distance: u16,
    /// Signed 8.8 vertical delta; md_vert expands this to 16.16.
    pub vertical: i16,
    /// Relative movement angle, in native 0..255 facing units.
    pub angle: u8,
    /// Additive instance rotations, in native wrapping facing units.
    pub rotation: [u8; 3],
}

impl Motion {
    /// GetMotion + ConvertToVector. Movement uses the facing BEFORE applying
    /// this frame's rotation. Widen products to avoid native integer overflow.
    pub fn displacement(self, facing: u8) -> [i32; 3] {
        let [x, y] = direction(self.angle.wrapping_add(facing));
        let distance = i64::from(self.distance) << 8;
        [
            (i64::from(x) * distance / 256) as i32,
            (i64::from(y) * distance / 256) as i32,
            i32::from(self.vertical) * 256,
        ]
    }
}

/// The C++ DistX/DistY table is rounded once in the first quadrant and copied
/// into the others. Keep its 1/256 quantization instead of rotating with floats.
fn direction(angle: u8) -> [i16; 2] {
    static QUADRANT: OnceLock<[[i16; 2]; 64]> = OnceLock::new();
    let quadrant = QUADRANT.get_or_init(|| {
        std::array::from_fn(|i| {
            let radians = i as f64 * std::f64::consts::TAU / 256.0;
            [
                (256.0 * radians.sin()).round() as i16,
                (256.0 * radians.cos()).round() as i16,
            ]
        })
    });
    let [sin, cos] = quadrant[usize::from(angle & 63)];
    match angle / 64 {
        0 => [sin, -cos],
        1 => [cos, sin],
        2 => [-sin, cos],
        _ => [-cos, -sin],
    }
}

#[derive(Clone, Copy)]
pub(crate) enum MotionFormat {
    Legacy,
    Facing,
    Root,
}

impl MotionFormat {
    pub fn from_flags(flags: u32) -> Self {
        if flags & 64 != 0 {
            Self::Root
        } else if flags & 8 != 0 {
            Self::Facing
        } else {
            Self::Legacy
        }
    }
}

pub(crate) fn decode(
    bytes: Bytes<'_>,
    at: usize,
    frames: usize,
    format: MotionFormat,
) -> Result<Vec<Motion>> {
    let stride = if matches!(format, MotionFormat::Root) {
        8
    } else {
        4
    };
    bytes.slice(at, frames * stride)?;
    (0..frames)
        .map(|i| {
            let at = at + i * stride;
            Ok(match format {
                MotionFormat::Root => Motion {
                    distance: bytes.u16(at)?,
                    vertical: bytes.i16(at + 2)?,
                    angle: bytes.u8(at + 4)?,
                    rotation: [bytes.u8(at + 5)?, bytes.u8(at + 6)?, bytes.u8(at + 7)?],
                },
                MotionFormat::Facing => Motion {
                    distance: bytes.u16(at)?,
                    angle: bytes.u8(at + 3)?,
                    rotation: [0, 0, bytes.u8(at + 2)?],
                    ..Motion::default()
                },
                MotionFormat::Legacy => {
                    let word = bytes.u32(at)?;
                    let angle = (word >> 24) as u8;
                    // InitializeMesh discards the low eight distance bits during conversion.
                    Motion {
                        distance: (word >> 8) as u16,
                        angle,
                        rotation: [0, 0, angle],
                        ..Motion::default()
                    }
                }
            })
        })
        .collect()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn all_three_formats_preserve_native_units_and_signed_vertical_motion() {
        let modern = [0xff, 0xff, 0x00, 0x80, 64, 1, 254, 255];
        let decoded = decode(Bytes(&modern), 0, 1, MotionFormat::Root).unwrap()[0];
        assert_eq!(
            decoded,
            Motion {
                distance: 65535,
                vertical: -32768,
                angle: 64,
                rotation: [1, 254, 255]
            }
        );
        assert_eq!(decoded.displacement(0), [65535 << 8, 0, -8388608]);
        let old = decode(Bytes(&[0x34, 0x12, 0xff, 0x80]), 0, 1, MotionFormat::Facing).unwrap()[0];
        assert_eq!(
            old,
            Motion {
                distance: 0x1234,
                angle: 128,
                rotation: [0, 0, 255],
                ..Motion::default()
            }
        );
        let oldest =
            decode(Bytes(&[0xab, 0x34, 0x12, 0x80]), 0, 1, MotionFormat::Legacy).unwrap()[0];
        assert_eq!(
            oldest,
            Motion {
                rotation: [0, 0, 128],
                ..old
            }
        );
        for len in 0..8 {
            assert!(decode(Bytes(&modern[..len]), 0, 1, MotionFormat::Root).is_err());
        }
        assert!(decode(Bytes(&modern), 0, 3, MotionFormat::Facing).is_err());
    }

    #[test]
    fn facing_is_quantized_and_wraps_before_rotation_is_applied() {
        let motion = Motion {
            distance: 256,
            angle: 32,
            rotation: [0, 0, 64],
            ..Motion::default()
        };
        assert_eq!(motion.displacement(0), [181 * 256, -181 * 256, 0]);
        assert_eq!(motion.displacement(224), [0, -FIXED_ONE, 0]);
        assert_eq!(motion.displacement(64), [181 * 256, 181 * 256, 0]);
        for angle in 0..128u8 {
            let a = direction(angle);
            let b = direction(angle + 128);
            assert_eq!(a, [-b[0], -b[1]]);
        }
    }
}
