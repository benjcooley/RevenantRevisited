// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              legacy.rs - Original fixed-array I3D bodies              *
// *************************************************************************

//! SOld3DImageryBody's 32-bit disk layout. Convert into the same owned mesh as
//! modern bodies; do not reproduce its fixed limits in runtime storage.
use super::{
    Batch, Bone, Clip, Mesh, Part, animation::Track, count, decode_faces, decode_materials, morph,
    motion, name, pointer, texture,
};
use crate::assets::binary::Bytes;
use anyhow::{Context, Result, ensure};

const MAX_STATES: usize = 64;
const MAX_PARTS: i32 = 64;
const MAX_MATERIALS: i32 = 8;
const TEXTURE_SLOTS: usize = 8;
const NAMES: usize = 1884;
const MATERIALS: usize = 3932;
const VERTEX_STARTS: usize = 4060;
const VERTEX_COUNTS: usize = 4188;
const FACE_STARTS: usize = 4316;
const FACE_COUNTS: usize = 5340;
const MOTION: usize = 6364;
const KEYS: usize = 6620;
const PARENTS: usize = 7132;

pub(super) fn decode(header: Bytes<'_>, b: Bytes<'_>, states: usize, flags: u32) -> Result<Mesh> {
    // Files without icons/hierarchy can end the fixed header after anikeys.
    // Reading sizeof(SOld3DImageryBody) would consume their vertex data.
    b.slice(0, KEYS + MAX_STATES * 4)?;
    ensure!(states <= MAX_STATES, "old I3D exceeds 64 states");
    ensure!(flags & 128 == 0, "old I3D body has unsupported 32-bit keys");
    let num_vertices = count(b, 4, 65536)?;
    let num_faces = count(b, 264, 1_000_000)?;
    let num_materials = count(b, 272, MAX_MATERIALS)?;
    // Slot zero is untextured. The old face arrays have only eight slots,
    // leaving seven usable textures even though the descriptor array has eight.
    let num_textures = count(b, 916, (TEXTURE_SLOTS - 1) as i32)?;
    let num_parts = count(b, 1880, MAX_PARTS)?;
    ensure!(
        num_parts > 0 && num_materials > 0,
        "I3D has no parts or materials"
    );
    let (vertices, morph) = morph::decode(header, b, 8, num_vertices, states, flags & 1 != 0)?;
    let faces_at = if num_faces == 0 { 0 } else { pointer(b, 268)? };
    let faces = decode_faces(b, faces_at, num_faces)?;
    let materials = decode_materials(b, 276, num_materials)?;
    let mut budget = 64 * 1024 * 1024;
    let textures = (0..num_textures)
        .map(|i| {
            texture::decode_fields(
                b,
                920 + i * 108,
                1784 + i * 4,
                1816 + i * 4,
                1848 + i * 4,
                &mut budget,
            )
            .with_context(|| format!("texture {i}"))
        })
        .collect::<Result<_>>()?;
    let mut parts = Vec::with_capacity(num_parts);
    for i in 0..num_parts {
        let material = usize::from(b.u16(MATERIALS + i * 2)?);
        ensure!(
            material < num_materials,
            "part {i}: invalid material {material}"
        );
        let start = usize::from(b.u16(VERTEX_STARTS + i * 2)?);
        let length = usize::from(b.u16(VERTEX_COUNTS + i * 2)?);
        ensure!(
            start + length <= num_vertices,
            "part {i}: vertex range out of bounds"
        );
        let mut batches = Vec::new();
        for slot in 0..=num_textures {
            let index = (i * TEXTURE_SLOTS + slot) * 2;
            let first = usize::from(b.u16(FACE_STARTS + index)?);
            let count = usize::from(b.u16(FACE_COUNTS + index)?);
            if count == 0 {
                continue;
            }
            let end = first + count;
            ensure!(end <= num_faces, "part {i}: face range out of bounds");
            ensure!(
                faces[first..end]
                    .iter()
                    .flatten()
                    .all(|&v| usize::from(v) < length),
                "part {i}: face index exceeds local vertex count"
            );
            batches.push(Batch {
                faces: first..end,
                texture: slot.checked_sub(1),
            });
        }
        parts.push(Part {
            name: name(b, NAMES + i * 32)?,
            vertices: start..start + length,
            material,
            batches,
        });
    }
    let mut parents = Vec::with_capacity(num_parts);
    for i in 0..num_parts {
        let at = if flags & 4 != 0 && morph.is_empty() {
            let at = pointer(b, PARENTS + i * 4)?;
            b.slice(at, states)?;
            Some(at)
        } else {
            None
        };
        parents.push(at);
    }
    let mut clips = Vec::with_capacity(states);
    let mut key_budget = 2_000_000usize;
    let mut motion_budget = 2_000_000usize;
    for state in 0..states {
        let at = 8 + state * 76;
        let frames = header.i16(at + 42)?;
        ensure!(frames >= 0, "negative clip length");
        let frames = frames as usize;
        let keys = if morph.is_empty() {
            b.relative(KEYS + state * 4)?
        } else {
            None
        };
        if let Some(keys) = keys {
            key_budget = key_budget
                .checked_sub(frames * num_parts)
                .context("mesh exceeds animation key budget")?;
            b.slice(keys, frames * num_parts * 8)
                .with_context(|| format!("animation state {state}"))?;
        }
        let mut bones = Vec::with_capacity(num_parts);
        for (i, &parents) in parents.iter().enumerate() {
            let raw_parent = match parents {
                Some(at) => i32::from(b.u8(at + state)? as i8),
                None => -1,
            };
            ensure!(
                raw_parent >= -1 && raw_parent < num_parts as i32,
                "part {i} state {state}: invalid parent {raw_parent}"
            );
            // The state points to [object][frame] SAniKey records, not a
            // pointer table or one shared track. Retail irisdoor/astrolabe
            // retain distinct, continuous tracks with this ordering.
            let track = if !morph.is_empty() {
                (frames != 0).then(Track::default)
            } else {
                keys.filter(|_| frames != 0)
                    .map(|keys| {
                        Track::decode(b, keys + i * frames * 8, frames, false)
                            .with_context(|| format!("part {i} state {state}"))
                    })
                    .transpose()?
            };
            bones.push(Bone {
                parent: (raw_parent >= 0).then_some(raw_parent as usize),
                track,
            });
        }
        let motion_at = b.relative(MOTION + state * 4)?;
        let motion = if let Some(at) = motion_at {
            motion_budget = motion_budget
                .checked_sub(frames)
                .context("mesh exceeds motion-frame budget")?;
            // OldInitializeMesh always uses the original 24-bit-distance
            // record. The later format flags belong to modern bodies.
            motion::decode(b, at, frames, motion::MotionFormat::Legacy)
                .with_context(|| format!("motion state {state}"))?
        } else {
            Vec::new()
        };
        clips.push(Clip {
            name: name(header, at)?,
            frames: (frames as u32).max(1),
            flags: header.u16(at + 40)?
                | if motion_at.is_none() {
                    Clip::NO_MOTION
                } else {
                    0
                },
            motion,
            bones,
        });
    }
    Ok(Mesh {
        vertices,
        morph,
        faces,
        materials,
        textures,
        parts,
        clips,
    })
}

#[cfg(test)]
mod tests {
    use super::*;

    fn put32(b: &mut [u8], at: usize, value: u32) {
        b[at..at + 4].copy_from_slice(&value.to_le_bytes());
    }
    fn put16(b: &mut [u8], at: usize, value: u16) {
        b[at..at + 2].copy_from_slice(&value.to_le_bytes());
    }
    fn rel(b: &mut [u8], at: usize, target: usize) {
        put32(b, at, (target as i32 - at as i32) as u32);
    }

    fn fixture() -> (Vec<u8>, Vec<u8>) {
        let mut header = vec![0; 8 + 2 * 76];
        put32(&mut header, 0, 1);
        put32(&mut header, 4, 2);
        put16(&mut header, 8 + 42, 2);
        put16(&mut header, 8 + 76 + 42, 3);
        let mut b = vec![0; 7700];
        for (at, v) in [(0, 4), (4, 6), (264, 2), (272, 1), (1880, 2)] {
            put32(&mut b, at, v);
        }
        for (at, target) in [
            (8, 7388),
            (7388, 7392),
            (268, 7584),
            (6620, 7596),
            (6624, 7628),
            (7132, 7676),
            (7136, 7680),
            (6364, 7684),
        ] {
            rel(&mut b, at, target);
        }
        for i in 0..2 {
            put16(&mut b, VERTEX_STARTS + i * 2, (i * 3) as u16);
            put16(&mut b, VERTEX_COUNTS + i * 2, 3);
            put16(&mut b, FACE_STARTS + i * 16, i as u16);
            put16(&mut b, FACE_COUNTS + i * 16, 1);
            for j in 0..3 {
                put16(&mut b, 7584 + i * 6 + j * 2, j as u16);
            }
        }
        // Each state stores a whole object's track before the next object's.
        for (i, x) in [1, 2, 11, 12, 3, 4, 5, 13, 14, 15].into_iter().enumerate() {
            let key =
                (2048u64 + x * 4) | 2048 << 12 | 2048 << 24 | 256 << 36 | 256 << 45 | 256 << 54;
            b[7596 + i * 8..7604 + i * 8].copy_from_slice(&key.to_le_bytes());
        }
        b[7676..7678].copy_from_slice(&[255, 1]); // root in state 0; forward parent in state 1
        b[7680..7682].copy_from_slice(&[0, 255]);
        b[7684..7692].copy_from_slice(&[0, 0, 1, 64, 0, 0, 2, 128]);
        (header, b)
    }

    #[test]
    fn old_morph_states_use_embedded_offsets_without_rigid_keys() {
        let (mut header, mut b) = fixture();
        for state in 0..2 {
            put16(&mut header, 8 + state * 76 + 42, 1);
            rel(&mut b, 8 + state * 4, 7388);
            put32(&mut b, KEYS + state * 4, u32::MAX);
        }
        let mesh = decode(Bytes(&header), Bytes(&b), 2, 5).unwrap();
        assert_eq!(mesh.morph.len(), 2);
        assert_eq!(mesh.vertices_at(1, 0).len(), 6);
        assert!(mesh.clips.iter().all(|c| {
            c.bones
                .iter()
                .all(|b| b.parent.is_none() && b.track.is_some())
        }));
    }

    #[test]
    fn fixed_tables_keep_object_tracks_signed_parents_and_original_motion() {
        let (header, b) = fixture();
        let mesh = decode(Bytes(&header), Bytes(&b), 2, 4).unwrap();
        assert_eq!(mesh.parts[1].vertices, 3..6);
        assert_eq!(mesh.parts[1].batches[0].faces, 1..2);
        assert_eq!(mesh.clips[0].standalone_order().unwrap(), [0, 1]);
        assert_eq!(mesh.clips[1].standalone_order().unwrap(), [1, 0]);
        for (state, expected) in [(0, [2.0, 12.0]), (1, [5.0, 15.0])] {
            let clip = &mesh.clips[state];
            for (bone, x) in clip.bones.iter().zip(expected) {
                let pose = bone.track.as_ref().unwrap().sample(clip.frames - 1);
                assert_eq!(pose.position, [x, 0.0, 0.0]);
                assert_eq!(pose.rotation, [0.0; 3]);
            }
        }
        let motion = mesh.clips[0].motion_at(1).unwrap();
        assert_eq!(
            (motion.distance, motion.angle, motion.rotation),
            (512, 128, [0, 0, 128])
        );
        assert_ne!(mesh.clips[1].flags & Clip::NO_MOTION, 0);
    }

    #[test]
    fn absent_or_zero_length_keys_hide_parts_without_reading_a_phantom_frame() {
        let (mut header, mut b) = fixture();
        put32(&mut b, KEYS, 0);
        put16(&mut header, 8 + 76 + 42, 0);
        let end = b.len() - 1;
        rel(&mut b, KEYS + 4, end);
        let mesh = decode(Bytes(&header), Bytes(&b), 2, 4).unwrap();
        assert!(
            mesh.clips
                .iter()
                .flat_map(|clip| &clip.bones)
                .all(|bone| bone.track.is_none())
        );
        assert_eq!(mesh.clips[1].frames, 1);
    }

    #[test]
    fn old_tables_reject_bad_counts_ranges_parents_and_truncated_tracks() {
        let (header, b) = fixture();
        for (at, value) in [(272, 9), (916, 8), (1880, 65), (4, 5)] {
            let mut bad = b.clone();
            put32(&mut bad, at, value);
            assert!(
                decode(Bytes(&header), Bytes(&bad), 2, 4).is_err(),
                "field {at}"
            );
        }
        for (at, value) in [(7676, 2), (7676, 254), (7584, 3), (FACE_COUNTS, 3)] {
            let mut bad = b.clone();
            bad[at] = value;
            assert!(
                decode(Bytes(&header), Bytes(&bad), 2, 4).is_err(),
                "field {at}"
            );
        }
        let mut bad = b.clone();
        rel(&mut bad, KEYS + 4, b.len() - 8);
        assert!(decode(Bytes(&header), Bytes(&bad), 2, 4).is_err());
        assert!(decode(Bytes(&header), Bytes(&b), 65, 4).is_err());
    }
}
