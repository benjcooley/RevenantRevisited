// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   morph.rs - Immutable vertex animation frames       *
// *************************************************************************

use super::{Vertex, decode_vertices, pointer};
use crate::assets::binary::Bytes;
use anyhow::{Context, Result, ensure};

type Frames = Vec<Vec<Vec<Vertex>>>;

/// Modern bodies point at a state table; old bodies embed it at byte eight.
/// In both layouts each OFFSET is relative to its own field, including frames.
pub(super) fn decode(
    header: Bytes<'_>,
    b: Bytes<'_>,
    states_at: usize,
    vertices: usize,
    states: usize,
    morph: bool,
) -> Result<(Vec<Vertex>, Frames)> {
    if !morph {
        return Ok((
            decode_vertices(b, pointer(b, pointer(b, states_at)?)?, vertices)?,
            Vec::new(),
        ));
    }
    b.slice(states_at, states * 4)?;
    let mut budget = 2_000_000usize;
    let mut snapshots = Vec::with_capacity(states);
    for state in 0..states {
        let count = header.i16(8 + state * 76 + 42)?;
        ensure!(count >= 0, "negative morph frame count");
        budget = budget
            .checked_sub(count as usize * vertices)
            .context("mesh exceeds morph vertex budget")?;
        let mut frames = Vec::with_capacity(count as usize);
        if count != 0 {
            let at = pointer(b, states_at + state * 4)?;
            b.slice(at, count as usize * 4)?;
            for frame in 0..count as usize {
                frames.push(
                    decode_vertices(b, pointer(b, at + frame * 4)?, vertices)
                        .with_context(|| format!("morph state {state}, frame {frame}"))?,
                );
            }
        }
        snapshots.push(frames);
    }
    let base = snapshots
        .iter()
        .flatten()
        .next()
        .context("morph mesh has no frames")?
        .clone();
    Ok((base, snapshots))
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn state_and_frame_offsets_are_relative_and_bounded() {
        let mut header = vec![0; 8 + 3 * 76];
        header[50..52].copy_from_slice(&2i16.to_le_bytes());
        header[202..204].copy_from_slice(&1i16.to_le_bytes());
        let mut body = vec![0; 180];
        for (field, target) in [(8usize, 24usize), (16, 32), (24, 40), (28, 72), (32, 104)] {
            body[field..field + 4]
                .copy_from_slice(&((target as i32 - field as i32) as u32).to_le_bytes());
        }
        for (at, value) in [(40usize, 1f32), (72, 2.), (104, 3.)] {
            body[at..at + 4].copy_from_slice(&value.to_le_bytes());
        }
        let (base, frames) = decode(Bytes(&header), Bytes(&body), 8, 1, 3, true).unwrap();
        assert_eq!(base[0].position[0], 1.);
        assert_eq!(frames[0][1][0].position[0], 2.);
        assert!(frames[1].is_empty());
        assert_eq!(frames[2][0][0].position[0], 3.);
        assert!(decode(Bytes(&header), Bytes(&body[..120]), 8, 1, 3, true).is_err());
        body[28..32].copy_from_slice(&u32::MAX.to_le_bytes());
        assert!(decode(Bytes(&header), Bytes(&body), 8, 1, 3, true).is_err());
    }
}
