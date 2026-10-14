// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              animation.rs - Native packed animation keys              *
// *************************************************************************

//! Native packed animation keys compiled into immutable pose spans.
use crate::assets::binary::Bytes;
use anyhow::{Result, bail, ensure};

// Preserve the authored format's conversion constant (3dimagebody.h).
#[allow(clippy::approx_constant)]
const KEY_PI: f64 = 3.1415965359;

#[derive(Clone, Copy, Debug, PartialEq)]
pub struct Pose {
    pub position: [f64; 3],
    pub rotation: [f64; 3],
    pub scale: [f64; 3],
}

impl Default for Pose {
    fn default() -> Self {
        Self {
            position: [0.0; 3],
            rotation: [0.0; 3],
            scale: [1.0; 3],
        }
    }
}

#[derive(Debug)]
struct Span {
    end: u32,
    pose: Pose,
}

#[derive(Debug, Default)]
pub struct Track {
    spans: Vec<Span>,
    last: Pose,
}

impl Track {
    pub fn sample(&self, frame: u32) -> Pose {
        let index = self.spans.partition_point(|span| span.end <= frame);
        self.spans.get(index).map_or(self.last, |span| span.pose)
    }

    pub(crate) fn decode(
        bytes: Bytes<'_>,
        at: usize,
        count: usize,
        packed32: bool,
    ) -> Result<Self> {
        bytes.slice(at, count * if packed32 { 4 } else { 8 })?;
        if !packed32 {
            let mut spans = Vec::with_capacity(count);
            for i in 0..count {
                let key = u64::from(bytes.u32(at + i * 8)?)
                    | (u64::from(bytes.u32(at + i * 8 + 4)?) << 32);
                let pose = Pose {
                    position: std::array::from_fn(|c| {
                        (((key >> (c * 12)) & 4095) as f64 - 2048.0) / 4.0
                    }),
                    rotation: std::array::from_fn(|c| {
                        (((key >> (36 + c * 9)) & 511) as f64 - 256.0) * std::f64::consts::PI
                            / 256.0
                    }),
                    ..Pose::default()
                };
                spans.push(Span {
                    end: i as u32 + 1,
                    pose,
                });
            }
            let last = spans.last().map_or(Pose::default(), |span| span.pose);
            return Ok(Self { spans, last });
        }
        let keys: Vec<_> = (0..count)
            .map(|i| bytes.u32(at + i * 4))
            .collect::<Result<_>>()?;
        let mut spans = Vec::new();
        let mut pose = Pose::default();
        let mut end = 0u32;
        let mut index = 0;
        while index < keys.len() {
            let mut changed = 0;
            let mut duration = 1;
            let start = index;
            while index < keys.len() {
                let key = keys[index];
                let kind = key & 3;
                let code = (key >> 2) & 63;
                if kind == 0 && code <= 1 {
                    duration = u32::from(changed != 0);
                    if code == 1 {
                        let skip = (key as i32) >> 8;
                        ensure!(skip >= 1, "invalid animation skip {skip}");
                        duration += skip as u32 - 1;
                    }
                    index += 1;
                    break;
                }
                let group = if kind != 0 {
                    kind
                } else {
                    match code {
                        2 => 1,
                        5 => 2,
                        8 => 3,
                        _ => bail!("invalid animation component code {code}"),
                    }
                };
                let mask = match group {
                    1 => 7,
                    2 => 6,
                    _ => 4,
                };
                if changed & mask != 0 {
                    break;
                }
                let vector = if kind == 0 {
                    ensure!(
                        index + 3 <= keys.len(),
                        "truncated animation component triplet"
                    );
                    let mut vector = [0.0; 3];
                    for (c, value) in vector.iter_mut().enumerate() {
                        let component = keys[index + c];
                        ensure!(
                            component & 3 == 0 && (component >> 2) & 63 == code + c as u32,
                            "animation component triplet is out of order"
                        );
                        *value = f64::from((component as i32) >> 8) / 256.0;
                    }
                    index += 3;
                    vector
                } else {
                    index += 1;
                    let divisor = match group {
                        1 => 4.0,
                        2 => 512.0 / KEY_PI,
                        _ => 64.0,
                    };
                    std::array::from_fn(|c| {
                        let raw = ((key >> (2 + c * 10)) & 1023) as i32;
                        f64::from((raw << 22) >> 22) / divisor
                    })
                };
                match group {
                    1 => pose.position = vector,
                    2 => pose.rotation = vector,
                    _ => pose.scale = vector,
                }
                changed |= 1 << (group - 1);
            }
            ensure!(index > start, "animation key parser made no progress");
            end = end
                .checked_add(duration)
                .ok_or_else(|| anyhow::anyhow!("animation duration overflow"))?;
            if duration != 0 {
                spans.push(Span { end, pose });
            }
        }
        Ok(Self { spans, last: pose })
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    fn track(keys: &[u32]) -> Result<Track> {
        let bytes: Vec<_> = keys.iter().flat_map(|k| k.to_le_bytes()).collect();
        Track::decode(Bytes(&bytes), 0, keys.len(), true)
    }
    #[test]
    fn packed_keys_have_half_open_durations_and_persistent_channels() {
        let t = track(&[
            1 | (8 << 2),
            3 | (64 << 2) | (64 << 12) | (64 << 22),
            (3 << 8) | 4,
            1 | (16 << 2),
        ])
        .unwrap();
        assert_eq!(t.sample(0).position[0], 2.0);
        assert_eq!(t.sample(2).position[0], 2.0);
        assert_eq!(t.sample(3).position[0], 4.0);
        assert_eq!(t.sample(999).scale, [1.0; 3]);
    }
    #[test]
    fn high_precision_triplets_and_signed_packed_values() {
        let t = track(&[
            (2 << 2) | ((-128i32 as u32) << 8),
            (3 << 2) | (256 << 8),
            4 << 2,
            1 | (1020 << 2),
        ])
        .unwrap();
        assert_eq!(t.sample(0).position, [-0.5, 1.0, 0.0]);
        assert_eq!(t.sample(1).position[0], -1.0);
        assert!(track(&[2 << 2]).is_err());
        assert!(track(&[4]).is_err());
    }

    #[test]
    fn old_keys_cross_word_boundaries_and_use_biased_components() {
        let key = 2049u64 | (2044 << 12) | (2056 << 24) | (256 << 36) | (384 << 45);
        let bytes = key.to_le_bytes();
        let track = Track::decode(Bytes(&bytes), 0, 1, false).unwrap();
        let pose = track.sample(0);
        assert_eq!(pose.position, [0.25, -1.0, 2.0]);
        assert_eq!(
            pose.rotation,
            [0.0, std::f64::consts::FRAC_PI_2, -std::f64::consts::PI]
        );
        assert_eq!(track.sample(100), pose);
    }
}
