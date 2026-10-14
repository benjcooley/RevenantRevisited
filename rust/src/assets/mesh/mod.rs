// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                mod.rs - I3D meshes and animation clips                *
// *************************************************************************

//! Owned I3D geometry, textures and animation. No native pointers or registries.
pub mod animation;
mod legacy;
mod morph;
pub mod motion;
mod texture;

use super::{binary::Bytes, resource::Resource};
use animation::Track;
use anyhow::{Context, Result, ensure};
use std::ops::Range;
pub use texture::{Texture, TextureLighting};

#[derive(Debug, Clone, Copy)]
pub struct Vertex {
    pub position: [f64; 3],
    pub normal: [f64; 3],
    pub uv: [f64; 2],
}

#[derive(Clone, Copy, Debug)]
pub struct Material {
    pub diffuse: [f64; 4],
    pub ambient: [f64; 4],
    pub specular: [f64; 4],
    pub emissive: [f64; 4],
    pub power: f64,
}
impl From<[f64; 4]> for Material {
    fn from(diffuse: [f64; 4]) -> Self {
        Self {
            diffuse,
            ambient: [1.0, 1.0, 1.0, diffuse[3]],
            specular: [0.0; 4],
            emissive: [0.0; 4],
            power: 1.0,
        }
    }
}

#[derive(Debug)]
pub struct Batch {
    pub faces: Range<usize>,
    pub texture: Option<usize>,
}

#[derive(Debug)]
pub struct Part {
    pub name: String,
    pub vertices: Range<usize>,
    pub material: usize,
    pub batches: Vec<Batch>,
}

#[derive(Debug)]
pub struct Bone {
    pub parent: Option<usize>,
    /// Absent keys mean this part is hidden in the clip.
    pub track: Option<Track>,
}

#[derive(Debug)]
pub struct Clip {
    pub name: String,
    pub frames: u32,
    pub flags: u16,
    /// Empty when the state has no authored motion. Never synthesize a speed.
    pub motion: Vec<motion::Motion>,
    pub bones: Vec<Bone>,
}

impl Clip {
    pub const ROOT: u16 = 0x0400;
    pub const NO_MOTION: u16 = 0x2000;

    /// Match AF_NOMOTION and GetMotion's bounds; playback decides when to repeat.
    pub fn motion_at(&self, frame: u32) -> Option<motion::Motion> {
        if self.flags & Self::NO_MOTION != 0 || frame >= self.frames {
            return None;
        }
        self.motion.get(frame as usize).copied()
    }

    /// Only standalone skeletons traverse these links. Equipped geometry uses
    /// character matrices instead (charanimator.cpp::ProcessEquipment).
    pub fn standalone_order(&self) -> Result<Vec<usize>> {
        hierarchy_order(&self.bones).with_context(|| format!(
            "clip {:?} cannot form a standalone skeleton; an equipment attachment pose may be required", self.name
        ))
    }
}

#[derive(Debug)]
pub struct Mesh {
    pub vertices: Vec<Vertex>,
    /// State/frame vertex snapshots for native morph imagery; empty for rigid meshes.
    pub morph: Vec<Vec<Vec<Vertex>>>,
    /// Each index is local to its part's vertex range.
    pub faces: Vec<[u16; 3]>,
    pub materials: Vec<Material>,
    pub textures: Vec<Vec<Texture>>,
    pub parts: Vec<Part>,
    pub clips: Vec<Clip>,
}

fn count(bytes: Bytes<'_>, at: usize, max: i32) -> Result<usize> {
    let value = bytes.i32(at)?;
    ensure!(
        (0..=max).contains(&value),
        "invalid I3D count {value} at {at:#x}"
    );
    Ok(value as usize)
}

fn pointer(bytes: Bytes<'_>, field: usize) -> Result<usize> {
    bytes
        .relative(field)?
        .with_context(|| format!("missing I3D pointer at {field:#x}"))
}

fn name(bytes: Bytes<'_>, at: usize) -> Result<String> {
    let raw = bytes.slice(at, 32)?;
    Ok(String::from_utf8_lossy(raw.split(|&v| v == 0).next().unwrap_or_default()).into_owned())
}

fn decode_vertices(b: Bytes<'_>, at: usize, count: usize) -> Result<Vec<Vertex>> {
    b.slice(at, count * 32)?;
    (0..count)
        .map(|i| {
            let at = at + i * 32;
            Ok(Vertex {
                position: [b.f32(at)?, b.f32(at + 4)?, b.f32(at + 8)?],
                normal: [b.f32(at + 12)?, b.f32(at + 16)?, b.f32(at + 20)?],
                uv: [b.f32(at + 24)?, b.f32(at + 28)?],
            })
        })
        .collect()
}

fn decode_faces(b: Bytes<'_>, at: usize, count: usize) -> Result<Vec<[u16; 3]>> {
    b.slice(at, count * 6)?;
    (0..count)
        .map(|i| {
            let at = at + i * 6;
            Ok([b.u16(at)?, b.u16(at + 2)?, b.u16(at + 4)?])
        })
        .collect()
}

fn decode_materials(b: Bytes<'_>, at: usize, count: usize) -> Result<Vec<Material>> {
    b.slice(at, count * 80)?;
    (0..count)
        .map(|i| {
            let at = at + i * 80;
            let color = |offset| -> Result<[f64; 4]> {
                Ok([
                    b.f32(at + offset)?,
                    b.f32(at + offset + 4)?,
                    b.f32(at + offset + 8)?,
                    b.f32(at + offset + 12)?,
                ])
            };
            Ok(Material {
                diffuse: color(4)?,
                ambient: color(20)?,
                specular: color(36)?,
                emissive: color(52)?,
                power: b.f32(at + 68)?,
            })
        })
        .collect()
}

impl Mesh {
    /// Frame snapshots stay immutable, allowing simultaneous instances at different poses.
    pub fn vertex_sample(&self, state: usize, frame: u32) -> usize {
        if self.morph.is_empty() || self.morph[state].is_empty() {
            return 0;
        }
        1 + self.morph[..state].iter().map(Vec::len).sum::<usize>()
            + frame as usize % self.morph[state].len()
    }
    pub fn vertex_samples(&self) -> impl Iterator<Item = &[Vertex]> {
        std::iter::once(self.vertices.as_slice())
            .chain(self.morph.iter().flatten().map(Vec::as_slice))
    }
    pub fn vertices_at(&self, state: usize, frame: u32) -> &[Vertex] {
        if self.morph.is_empty() || self.morph[state].is_empty() {
            &self.vertices
        } else {
            &self.morph[state][frame as usize % self.morph[state].len()]
        }
    }
    pub fn decode(data: &[u8]) -> Result<Self> {
        let resource = Resource::parse(data)?;
        let header = resource.header;
        ensure!(header.i32(0)? == 1, "resource is not 3D imagery");
        let states = count(header, 4, 4096)?;
        ensure!(states > 0, "I3D has no states");
        header.slice(8, states * 76)?;
        let b = resource.body;
        b.slice(0, 60)?;
        let flags = b.u32(0)?;
        if flags & 16 == 0 {
            return legacy::decode(header, b, states, flags);
        }
        let version = b.u32(4)?;
        ensure!((1..=3).contains(&version), "unsupported I3D body version");
        let num_vertices = count(b, 12, 65536)?;
        let num_faces = count(b, 20, 1_000_000)?;
        let num_materials = count(b, 28, 4096)?;
        let num_textures = count(b, 36, 256)?;
        let num_parts = count(b, 44, 4096)?;
        ensure!(
            states * num_parts <= 1_000_000,
            "mesh exceeds bone-state budget"
        );
        ensure!(
            num_parts > 0 && num_materials > 0,
            "I3D has no parts or materials"
        );
        let (vertices, morph) = morph::decode(
            header,
            b,
            pointer(b, 16)?,
            num_vertices,
            states,
            flags & 1 != 0,
        )?;
        let faces_at = if num_faces == 0 { 0 } else { pointer(b, 24)? };
        let faces = decode_faces(b, faces_at, num_faces)?;
        let materials_at = pointer(b, 32)?;
        let materials = decode_materials(b, materials_at, num_materials)?;
        let mut textures = Vec::with_capacity(num_textures);
        if num_textures != 0 {
            let at = pointer(b, 40)?;
            b.slice(at, num_textures * 120)?;
            let mut budget = 64 * 1024 * 1024;
            for i in 0..num_textures {
                textures.push(
                    texture::decode(b, at + i * 120, &mut budget)
                        .with_context(|| format!("texture {i}"))?,
                );
            }
        }
        let objects_at = pointer(b, 48)?;
        b.slice(objects_at, num_parts * 48)?;
        let mut parts = Vec::with_capacity(num_parts);
        let state_data = b.relative(8)?;
        let state_bytes = if version == 2 { 80 } else { 16 };
        if let Some(at) = state_data {
            b.slice(at, states * state_bytes)?;
        }
        let mut motion_budget = 2_000_000usize;
        let mut clips: Vec<_> = (0..states)
            .map(|i| {
                let at = 8 + i * 76;
                let frames = header.i16(at + 42)?;
                ensure!(frames >= 0, "negative clip length");
                let motion_at = state_data
                    .map(|data| b.relative(data + i * state_bytes + 12))
                    .transpose()?
                    .flatten();
                let motion = if let Some(motion_at) = motion_at {
                    motion_budget = motion_budget
                        .checked_sub(frames as usize)
                        .context("mesh exceeds motion-frame budget")?;
                    motion::decode(
                        b,
                        motion_at,
                        frames as usize,
                        motion::MotionFormat::from_flags(flags),
                    )
                    .with_context(|| format!("motion state {i}"))?
                } else {
                    Vec::new()
                };
                Ok(Clip {
                    name: name(header, at)?,
                    frames: (frames as u32).max(1),
                    flags: header.u16(at + 40)?,
                    motion,
                    bones: Vec::with_capacity(num_parts),
                })
            })
            .collect::<Result<_>>()?;
        let packed32 = flags & 128 != 0;
        let state_stride = if packed32 { 12 } else { 8 };
        // Bound expanded track storage independently of aliased resource pointers.
        let mut key_budget = 2_000_000usize;
        for i in 0..num_parts {
            let at = objects_at + i * 48;
            let material = usize::from(b.u16(at + 32)?);
            ensure!(
                material < num_materials,
                "part {i}: invalid material {material}"
            );
            let start = usize::from(b.u16(at + 34)?);
            let length = usize::from(b.u16(at + 36)?);
            ensure!(
                start + length <= num_vertices,
                "part {i}: vertex range out of bounds"
            );
            let batch_at = pointer(b, at + 40)?;
            b.slice(batch_at, (num_textures + 1) * 4)?;
            let mut batches = Vec::new();
            for slot in 0..=num_textures {
                let start = usize::from(b.u16(batch_at + slot * 4)?);
                let length_faces = usize::from(b.u16(batch_at + slot * 4 + 2)?);
                if length_faces == 0 {
                    continue;
                }
                let end = start + length_faces;
                ensure!(end <= num_faces, "part {i}: face range out of bounds");
                ensure!(
                    faces[start..end]
                        .iter()
                        .flatten()
                        .all(|&v| usize::from(v) < length),
                    "part {i}: face index exceeds local vertex count"
                );
                batches.push(Batch {
                    faces: start..end,
                    texture: slot.checked_sub(1),
                });
            }
            parts.push(Part {
                name: name(b, at)?,
                vertices: start..start + length,
                material,
                batches,
            });
            if !morph.is_empty() {
                for (state, clip) in clips.iter_mut().enumerate() {
                    clip.bones.push(Bone {
                        parent: None,
                        track: (!morph[state].is_empty()).then(Track::default),
                    });
                }
                continue;
            }
            let state_at = pointer(b, at + 44)?;
            b.slice(state_at, states * state_stride)?;
            for (state, clip) in clips.iter_mut().enumerate() {
                let at = state_at + state * state_stride;
                let raw_parent = if flags & 4 != 0 { b.i32(at)? } else { -1 };
                ensure!(
                    raw_parent >= -1 && raw_parent < num_parts as i32,
                    "part {i} state {state}: invalid parent {raw_parent}"
                );
                let parent = (raw_parent >= 0).then_some(raw_parent as usize);
                let key_at = b.relative(at + if packed32 { 8 } else { 4 })?;
                let track = if let Some(key_at) = key_at {
                    let keys = if packed32 {
                        count(b, at + 4, 1_000_000)?
                    } else {
                        clip.frames as usize
                    };
                    key_budget = key_budget
                        .checked_sub(keys)
                        .context("mesh exceeds animation key budget")?;
                    Some(
                        Track::decode(b, key_at, keys, packed32)
                            .with_context(|| format!("part {i} state {state}"))?,
                    )
                } else {
                    None
                };
                clip.bones.push(Bone { parent, track });
            }
        }
        Ok(Self {
            vertices,
            morph,
            faces,
            materials,
            textures,
            parts,
            clips,
        })
    }
}

fn hierarchy_order(bones: &[Bone]) -> Result<Vec<usize>> {
    let mut visited = vec![0u8; bones.len()];
    let mut order = Vec::with_capacity(bones.len());
    for start in 0..bones.len() {
        let mut path = Vec::new();
        let mut current = Some(start);
        while let Some(i) = current {
            ensure!(i < bones.len(), "bone parent out of range");
            ensure!(
                visited[i] != 1,
                "standalone parent traversal repeats part {i}"
            );
            if visited[i] == 2 {
                break;
            }
            visited[i] = 1;
            path.push(i);
            current = bones[i].parent;
        }
        for i in path.into_iter().rev() {
            visited[i] = 2;
            order.push(i);
        }
    }
    Ok(order)
}

#[cfg(test)]
mod tests {
    use super::*;

    fn fixture() -> Vec<u8> {
        fn u32_at(b: &mut [u8], at: usize, value: u32) {
            b[at..at + 4].copy_from_slice(&value.to_le_bytes());
        }
        fn rel(b: &mut [u8], at: usize, target: usize) {
            u32_at(b, at, (target as i32 - at as i32) as u32);
        }
        let mut file = vec![0; 104 + 320];
        file[..4].copy_from_slice(b"CGSR");
        file[7] = 1;
        for (at, v) in [(8, 320), (12, 320), (16, 84), (20, 1), (24, 1)] {
            u32_at(&mut file, at, v);
        }
        file[70..72].copy_from_slice(&1u16.to_le_bytes());
        let b = &mut file[104..];
        for (at, v) in [
            (0, 144),
            (4, 3),
            (12, 3),
            (20, 1),
            (28, 1),
            (44, 1),
            (304, u32::MAX),
            (308, 1),
            (316, 1),
        ] {
            u32_at(b, at, v);
        }
        for (at, target) in [
            (16, 60),
            (60, 64),
            (64, 68),
            (24, 164),
            (32, 172),
            (48, 252),
            (292, 300),
            (296, 304),
            (312, 316),
        ] {
            rel(b, at, target);
        }
        b[164..170].copy_from_slice(&[0, 0, 1, 0, 2, 0]);
        for at in [176, 180, 184, 188] {
            u32_at(b, at, 1.0f32.to_bits());
        }
        b[288..290].copy_from_slice(&3u16.to_le_bytes());
        b[302..304].copy_from_slice(&1u16.to_le_bytes());
        file
    }

    #[test]
    fn morph_bodies_do_not_require_rigid_animation_tables() {
        let mut data = fixture();
        data[104..108].copy_from_slice(&145u32.to_le_bytes());
        data[400..404].copy_from_slice(&0u32.to_le_bytes());
        let mesh = Mesh::decode(&data).unwrap();
        assert_eq!(mesh.morph.len(), 1);
        assert_eq!(mesh.morph[0].len(), 1);
        assert_eq!(mesh.vertex_sample(0, 17), 1);
        assert_eq!(mesh.vertices_at(0, 17).len(), 3);
        assert!(mesh.clips[0].bones[0].track.is_some());
        assert_eq!(mesh.clips[0].bones[0].parent, None);
    }

    #[test]
    fn mesh_tables_reject_truncation_invalid_indices_and_offsets() {
        let data = fixture();
        let mesh = Mesh::decode(&data).unwrap();
        assert_eq!(mesh.parts.len(), 1);
        assert_eq!(mesh.faces, [[0, 1, 2]]);
        for end in 0..data.len() {
            assert!(
                Mesh::decode(&data[..end]).is_err(),
                "accepted truncation at {end}"
            );
        }
        let mut bad = data.clone();
        bad[104 + 164] = 3;
        assert!(
            Mesh::decode(&bad)
                .unwrap_err()
                .to_string()
                .contains("face index")
        );
        let mut bad = data.clone();
        bad[104 + 16..104 + 20].copy_from_slice(&u32::MAX.to_le_bytes());
        assert!(Mesh::decode(&bad).is_err());
        let mut bad = data;
        bad[104 + 68..104 + 72].copy_from_slice(&f32::NAN.to_bits().to_le_bytes());
        assert!(Mesh::decode(&bad).is_err());
    }

    #[test]
    fn decoding_does_not_require_a_standalone_skeleton() {
        let mut bytes = fixture();
        bytes[104..108].copy_from_slice(&148u32.to_le_bytes()); // hierarchy + key32 + modern body
        bytes[104 + 304..104 + 308].copy_from_slice(&0i32.to_le_bytes());
        let mesh = Mesh::decode(&bytes).unwrap();
        assert_eq!(mesh.clips[0].bones[0].parent, Some(0));
        assert!(mesh.clips[0].standalone_order().is_err());
    }

    fn motion_fixture(version: u32, flags: u32) -> Vec<u8> {
        let base = fixture();
        let mut header = base[20..104].to_vec();
        header.resize(8 + 2 * 76, 0);
        header[4..8].copy_from_slice(&2u32.to_le_bytes());
        header[8 + 76 + 42..8 + 76 + 44].copy_from_slice(&1u16.to_le_bytes());
        let mut body = base[104..].to_vec();
        body[0..4].copy_from_slice(&(144 | flags).to_le_bytes());
        body[4..8].copy_from_slice(&version.to_le_bytes());
        let stride = if version == 2 { 80 } else { 16 };
        let table = body.len();
        body.resize(table + 2 * stride, 0);
        let data = body.len();
        let samples: &[u8] = if flags & 64 != 0 {
            &[0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 64, 0, 0, 255]
        } else if flags & 8 != 0 {
            &[0, 1, 0, 0, 0, 2, 255, 64]
        } else {
            &[0, 0, 1, 0, 0, 0, 2, 64]
        };
        body.extend(samples);
        let bones = body.len();
        body.extend(&base[104 + 304..104 + 316]);
        body.extend(&base[104 + 304..104 + 316]);
        for (field, target) in [
            (8, table),
            (table + 12, data),
            (table + stride + 12, data + samples.len() / 2),
            (296, bones),
            (bones + 8, 316),
            (bones + 20, 316),
        ] {
            body[field..field + 4].copy_from_slice(&((target as i32) - field as i32).to_le_bytes());
        }
        let mut file = base[..20].to_vec();
        file[8..12].copy_from_slice(&(body.len() as u32).to_le_bytes());
        file[12..16].copy_from_slice(&(body.len() as u32).to_le_bytes());
        file[16..20].copy_from_slice(&(header.len() as u32).to_le_bytes());
        file.extend(header);
        file.extend(body);
        file
    }

    #[test]
    fn motion_tables_use_versioned_state_stride_and_field_relative_offsets() {
        for version in 1..=3 {
            for flags in [0, 8, 64] {
                let bytes = motion_fixture(version, flags);
                let mesh = Mesh::decode(&bytes).unwrap();
                assert_eq!(mesh.clips[0].motion_at(0).unwrap().distance, 256);
                let motion = mesh.clips[1].motion_at(0).unwrap();
                assert_eq!(motion.distance, 512);
                assert_eq!(motion.angle, 64);
                assert_eq!(motion.rotation[2], if flags == 0 { 64 } else { 255 });
                assert!(mesh.clips[0].motion_at(1).is_none());
                let mut bad = bytes.clone();
                bad[20 + 8 + 76 + 40..20 + 8 + 76 + 42]
                    .copy_from_slice(&Clip::NO_MOTION.to_le_bytes());
                let disabled = Mesh::decode(&bad).unwrap();
                assert!(disabled.clips[1].motion_at(0).is_none());
                assert_eq!(disabled.clips[1].motion.len(), 1); // preserve raw data
                // Valid relative pointer to a truncated motion record.
                let body = 20 + 8 + 2 * 76;
                let field = body + 320 + 12;
                let last_byte = bytes.len() - 1;
                bad[field..field + 4].copy_from_slice(&((last_byte - field) as i32).to_le_bytes());
                assert!(
                    Mesh::decode(&bad)
                        .unwrap_err()
                        .to_string()
                        .contains("motion state 0")
                );
            }
        }
    }
    #[test]
    fn hierarchy_handles_forward_parents_and_rejects_cycles() {
        let mut bones = vec![
            Bone {
                parent: Some(1),
                track: None,
            },
            Bone {
                parent: None,
                track: None,
            },
        ];
        assert_eq!(hierarchy_order(&bones).unwrap(), [1, 0]);
        bones[1].parent = Some(0);
        assert!(hierarchy_order(&bones).is_err());
        bones[1].parent = Some(9);
        assert!(hierarchy_order(&bones).is_err());
    }
}
