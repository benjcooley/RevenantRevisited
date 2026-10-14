// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               mesh.rs - Mesh poses and triangle rendering             *
// *************************************************************************

use super::{
    camera::ISO_COS,
    equipment::{Attachment, Slot},
};
use crate::{
    assets::mesh::{Mesh, animation::Pose},
    world::sector::Position,
};
use anyhow::{Result, ensure};
use std::{collections::BTreeMap, sync::Arc};

/// An affine transform with column-vector composition. Its representation is
/// independent of Direct3D's historical matrix storage.
#[derive(Clone, Copy)]
pub struct Transform([[f64; 4]; 3]);

impl Transform {
    const IDENTITY: Self = Self([
        [1.0, 0.0, 0.0, 0.0],
        [0.0, 1.0, 0.0, 0.0],
        [0.0, 0.0, 1.0, 0.0],
    ]);

    pub fn point(self, p: [f64; 3]) -> [f64; 3] {
        std::array::from_fn(|r| self.0[r][3] + (0..3).map(|c| self.0[r][c] * p[c]).sum::<f64>())
    }

    /// Inverse-transpose keeps normals correct under authored nonuniform scale.
    pub fn normal(self, n: [f64; 3]) -> [f64; 3] {
        let m = self.0;
        let cross = |a: [f64; 3], b: [f64; 3]| {
            [
                a[1] * b[2] - a[2] * b[1],
                a[2] * b[0] - a[0] * b[2],
                a[0] * b[1] - a[1] * b[0],
            ]
        };
        let cols = [
            [m[0][0], m[1][0], m[2][0]],
            [m[0][1], m[1][1], m[2][1]],
            [m[0][2], m[1][2], m[2][2]],
        ];
        let cofactor = [
            cross(cols[1], cols[2]),
            cross(cols[2], cols[0]),
            cross(cols[0], cols[1]),
        ];
        let determinant = (0..3).map(|i| cols[0][i] * cofactor[0][i]).sum::<f64>();
        let result: [f64; 3] = std::array::from_fn(|i| {
            (0..3).map(|j| cofactor[j][i] * n[j]).sum::<f64>() * determinant.signum()
        });
        let length = result.iter().map(|v| v * v).sum::<f64>().sqrt();
        if length < 1e-12 {
            [0.0, 0.0, 1.0]
        } else {
            result.map(|v| v / length)
        }
    }

    pub(crate) fn rows(self) -> [[f32; 4]; 3] {
        self.0.map(|row| row.map(|v| v as f32))
    }
    pub(crate) fn normal_rows(self) -> [[f32; 4]; 3] {
        let m = self.0;
        let cross = |a: [f64; 3], b: [f64; 3]| {
            [
                a[1] * b[2] - a[2] * b[1],
                a[2] * b[0] - a[0] * b[2],
                a[0] * b[1] - a[1] * b[0],
            ]
        };
        let cols = [
            [m[0][0], m[1][0], m[2][0]],
            [m[0][1], m[1][1], m[2][1]],
            [m[0][2], m[1][2], m[2][2]],
        ];
        let cofactor = [
            cross(cols[1], cols[2]),
            cross(cols[2], cols[0]),
            cross(cols[0], cols[1]),
        ];
        let determinant = (0..3).map(|i| cols[0][i] * cofactor[0][i]).sum::<f64>();
        std::array::from_fn(|i| {
            [
                (cofactor[0][i] * determinant.signum()) as f32,
                (cofactor[1][i] * determinant.signum()) as f32,
                (cofactor[2][i] * determinant.signum()) as f32,
                0.0,
            ]
        })
    }

    fn compose(self, child: Self) -> Self {
        Self(std::array::from_fn(|r| {
            std::array::from_fn(|c| {
                (0..3).map(|k| self.0[r][k] * child.0[k][c]).sum::<f64>()
                    + if c == 3 { self.0[r][3] } else { 0.0 }
            })
        }))
    }

    fn rotation(axis: usize, radians: f64) -> Self {
        let mut m = Self::IDENTITY;
        let (s, c) = radians.sin_cos();
        let a = (axis + 1) % 3;
        let b = (axis + 2) % 3;
        m.0[a][a] = c;
        m.0[b][b] = c;
        m.0[a][b] = -s;
        m.0[b][a] = s;
        m
    }

    fn pose(pose: Pose) -> Self {
        let mut m = Self::IDENTITY;
        for (axis, angle) in pose.rotation.into_iter().enumerate() {
            m = Self::rotation(axis, angle).compose(m);
        }
        // Native MakeMatrix scales the rotated basis, then adds translation.
        for r in 0..3 {
            for c in 0..3 {
                m.0[r][c] *= pose.scale[r];
            }
            m.0[r][3] = pose.position[r];
        }
        m
    }
}

#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
#[repr(u32)]
pub enum Shading {
    #[default]
    Surface = 0,
    /// Native helper material terms: ambient, diffuse, specular and emissive.
    Material = 1,
    Unlit = 2,
}
#[derive(Clone, Copy, Debug)]
pub struct Appearance {
    pub tint: [f32; 4],
    pub uv_offset: [f32; 2],
    pub shading: Shading,
}
impl Default for Appearance {
    fn default() -> Self {
        Self {
            tint: [1.0; 4],
            uv_offset: [0.0; 2],
            shading: Shading::Surface,
        }
    }
}

#[derive(Clone)]
pub struct MeshDraw {
    pub mesh: Arc<Mesh>,
    pub position: Position,
    pub rotation: [u8; 3],
    state: usize,
    frame: u32,
    matrices: Vec<Transform>,
    order: Vec<usize>,
    bounds: [f64; 4],
    equipment: BTreeMap<Slot, Attachment>,
    character: bool,
    bow_mode: bool,
    visible: Vec<bool>,
    appearance: Appearance,
}

impl MeshDraw {
    pub fn new(
        mesh: Arc<Mesh>,
        position: Position,
        rotation: [u8; 3],
        state: u16,
        frame: u32,
    ) -> Result<Self> {
        ensure!(
            usize::from(state) < mesh.clips.len(),
            "mesh state {state} out of range"
        );
        let order = mesh.clips[usize::from(state)].standalone_order()?;
        let mut draw = Self {
            order,
            matrices: vec![Transform::IDENTITY; mesh.parts.len()],
            visible: vec![false; mesh.parts.len()],
            appearance: Appearance::default(),
            equipment: BTreeMap::new(),
            character: false,
            bow_mode: false,
            mesh,
            position,
            rotation,
            state: usize::from(state),
            frame: 0,
            bounds: [0.0; 4],
        };
        draw.set_frame(frame);
        Ok(draw)
    }

    pub fn appearance(&self) -> Appearance {
        self.appearance
    }
    pub fn set_appearance(&mut self, appearance: Appearance) -> Result<()> {
        ensure!(
            appearance
                .tint
                .iter()
                .all(|v| v.is_finite() && (0.0..=1.0).contains(v))
                && appearance.uv_offset.iter().all(|v| v.is_finite()),
            "invalid mesh appearance"
        );
        self.appearance = appearance;
        Ok(())
    }
    pub fn state(&self) -> usize {
        self.state
    }
    pub fn frame(&self) -> u32 {
        self.frame
    }

    pub(crate) fn asset_meshes(&self) -> impl Iterator<Item = &Arc<Mesh>> {
        std::iter::once(&self.mesh)
            .chain(self.equipment.values().map(|attachment| &attachment.mesh))
    }

    /// Change an instance pose atomically, rebuilding parent order on clip changes.
    pub fn set_pose(
        &mut self,
        position: Position,
        rotation: [u8; 3],
        state: u16,
        frame: u32,
    ) -> Result<()> {
        let state = usize::from(state);
        ensure!(
            state < self.mesh.clips.len(),
            "mesh state {state} out of range"
        );
        if state != self.state {
            let order = self.mesh.clips[state].standalone_order()?;
            self.order = order;
            self.state = state;
        }
        self.position = position;
        self.rotation = rotation;
        self.set_frame(frame);
        Ok(())
    }
    pub fn bounds(&self) -> [f64; 4] {
        self.bounds
    }

    /// Evaluated world transform, including parts hidden by replacement gear.
    pub fn part_transform(&self, part: usize) -> Option<Transform> {
        self.matrices.get(part).copied()
    }

    /// Enable character rendering: hide baked weapon placeholders, and select
    /// prime-hand or ranged attachments. This does not select a combat action.
    pub fn set_character_mode(&mut self, bow_mode: bool) {
        self.character = true;
        self.bow_mode = bow_mode;
        self.refresh_bounds();
    }

    /// Replace one slot atomically. No mutation of shared character/gear assets.
    pub fn equip(&mut self, slot: Slot, mesh: Arc<Mesh>, body_type: &str) -> Result<usize> {
        let attachment = Attachment::bind(&self.mesh, mesh, slot, body_type)?;
        let count = attachment.bindings.len();
        self.equipment.insert(slot, attachment);
        self.character = true;
        self.refresh_bounds();
        Ok(count)
    }

    pub fn unequip(&mut self, slot: Slot) {
        self.equipment.remove(&slot);
        self.refresh_bounds();
    }

    pub fn equipment(&self, slot: Slot) -> Option<&Attachment> {
        self.equipment.get(&slot)
    }

    pub fn part_visible(&self, part: usize) -> bool {
        self.visible.get(part).copied().unwrap_or(false)
    }

    /// Both bounds and drawing consume the same parts and transforms. Equipment
    /// textures use frame zero, matching ProcessEquipment's RenderObject call.
    pub(crate) fn posed_parts(
        &self,
    ) -> impl Iterator<Item = (&Mesh, usize, Transform, usize, u32)> {
        let body = self.mesh.parts.iter().enumerate().filter_map(|(i, _)| {
            self.visible[i].then_some((&*self.mesh, i, self.matrices[i], self.state, self.frame))
        });
        let gear = self
            .equipment
            .iter()
            .filter(|(slot, _)| slot.active(self.bow_mode))
            .flat_map(move |(_, attachment)| {
                attachment.bindings.iter().map(move |binding| {
                    (
                        &*attachment.mesh,
                        binding.equipment_part,
                        self.matrices[binding.character_part],
                        attachment.state(),
                        0,
                    )
                })
            });
        body.chain(gear)
    }

    /// Pose sampling is discrete, matching native integer animation frames.
    /// Explicit preview playback loops; game state transitions are separate.
    pub fn set_frame(&mut self, frame: u32) {
        let clip = &self.mesh.clips[self.state];
        self.frame = frame % clip.frames;
        let mut root = Transform::pose(Pose {
            scale: [1.0, 1.0, 1.5],
            ..Pose::default()
        });
        for axis in [2, 0, 1] {
            root = Transform::rotation(
                axis,
                f64::from(self.rotation[axis]) * std::f64::consts::TAU / 256.0,
            )
            .compose(root);
        }
        root.0[0][3] = f64::from(self.position.x);
        root.0[1][3] = f64::from(self.position.y);
        root.0[2][3] = f64::from(self.position.z);
        for &i in &self.order {
            if !self.mesh.morph.is_empty() {
                self.matrices[i] = root;
                continue;
            }
            let bone = &clip.bones[i];
            let pose = bone
                .track
                .as_ref()
                .map_or(Pose::default(), |t| t.sample(self.frame));
            let parent = bone.parent.map_or(root, |p| self.matrices[p]);
            self.matrices[i] = parent.compose(Transform::pose(pose));
        }
        self.refresh_bounds();
    }

    fn refresh_bounds(&mut self) {
        let clip = &self.mesh.clips[self.state];
        for (i, part) in self.mesh.parts.iter().enumerate() {
            self.visible[i] = clip.bones[i].track.is_some()
                && !(self.character
                    && (part.name.eq_ignore_ascii_case("weapon")
                        || part.name.eq_ignore_ascii_case("sword")));
        }
        for (_, attachment) in self
            .equipment
            .iter()
            .filter(|(slot, _)| slot.active(self.bow_mode))
        {
            for binding in &attachment.bindings {
                self.visible[binding.character_part] = false;
            }
        }
        let mut bounds = [
            f64::INFINITY,
            f64::INFINITY,
            f64::NEG_INFINITY,
            f64::NEG_INFINITY,
        ];
        for (mesh, i, transform, state, frame) in self.posed_parts() {
            for vertex in &mesh.vertices_at(state, frame)[mesh.parts[i].vertices.clone()] {
                let p = projected(transform.point(vertex.position));
                bounds[0] = bounds[0].min(p[0]);
                bounds[1] = bounds[1].min(p[1]);
                bounds[2] = bounds[2].max(p[0]);
                bounds[3] = bounds[3].max(p[1]);
            }
        }
        self.bounds = if bounds[0].is_finite() {
            bounds
        } else {
            let p = projected([
                self.position.x.into(),
                self.position.y.into(),
                self.position.z.into(),
            ]);
            [p[0], p[1], p[0], p[1]]
        };
    }
}

fn projected(p: [f64; 3]) -> [f64; 3] {
    [
        p[0] - p[1],
        (p[0] + p[1]) * 0.5 - p[2] * ISO_COS,
        -(p[0] + p[1]) * ISO_COS - p[2] * 0.5,
    ]
}

#[cfg(test)]
mod tests {
    use super::super::equipment::tests::mesh;
    use super::*;

    #[test]
    fn attachments_follow_world_pose_once_restore_body_and_leave_other_instances_unchanged() {
        let asset = Arc::new(mesh(&["chest", "sword"]));
        let mut actor = MeshDraw::new(
            asset.clone(),
            Position {
                x: 10,
                y: 20,
                z: 30,
            },
            [0; 3],
            0,
            0,
        )
        .unwrap();
        let other = MeshDraw::new(asset, Position::default(), [0; 3], 0, 0).unwrap();
        let mut gear = mesh(&["*normalchest"]);
        gear.clips[0].bones[0].parent = Some(0);
        actor.equip(Slot::Body, Arc::new(gear), "normal").unwrap();
        assert!(!actor.part_visible(0));
        assert!(!actor.part_visible(1)); // baked sword placeholder
        assert!(other.part_visible(0));
        assert!(other.part_visible(1));
        let parts: Vec<_> = actor.posed_parts().collect();
        assert_eq!(parts.len(), 1);
        assert_eq!(parts[0].2.point([0.0, 0.0, 2.0]), [11.0, 20.0, 33.0]);
        let bounds = actor.bounds();
        assert!(
            actor
                .equip(Slot::Body, Arc::new(mesh(&["missing"])), "normal")
                .is_err()
        );
        assert_eq!(actor.bounds(), bounds);
        assert_eq!(actor.equipment(Slot::Body).unwrap().bindings().len(), 1);
        actor
            .set_pose(
                Position {
                    x: 40,
                    y: 50,
                    z: 60,
                },
                [0, 0, 64],
                0,
                1,
            )
            .unwrap();
        let (_, _, transform, _, texture_frame) = actor.posed_parts().next().unwrap();
        let point = transform.point([0.0, 0.0, 2.0]);
        assert!((point[0] - 40.0).abs() < 1e-10);
        assert!((point[1] - 52.0).abs() < 1e-10);
        assert!((point[2] - 63.0).abs() < 1e-10);
        assert_eq!(texture_frame, 0);
        assert_eq!(
            actor.bounds(),
            [
                projected(point)[0],
                projected(point)[1],
                projected(point)[0],
                projected(point)[1]
            ]
        );
        actor.unequip(Slot::Body);
        assert!(actor.part_visible(0));
        assert!(!actor.part_visible(1));
    }

    #[test]
    fn bow_mode_switches_hand_attachments_without_losing_bindings() {
        let mut actor = MeshDraw::new(
            Arc::new(mesh(&["sword"])),
            Position::default(),
            [0; 3],
            0,
            0,
        )
        .unwrap();
        actor
            .equip(Slot::PrimeHand, Arc::new(mesh(&["blade"])), "normal")
            .unwrap();
        actor
            .equip(Slot::RangedWeapon, Arc::new(mesh(&["bow"])), "normal")
            .unwrap();
        assert_eq!(actor.posed_parts().next().unwrap().0.parts[0].name, "blade");
        actor.set_character_mode(true);
        assert_eq!(actor.posed_parts().next().unwrap().0.parts[0].name, "bow");
        assert_eq!(actor.posed_parts().count(), 1);
        assert!(actor.equipment(Slot::PrimeHand).is_some());
    }
    #[test]
    fn native_scale_does_not_scale_key_translation() {
        let m = Transform::pose(Pose {
            position: [10.0, 20.0, 30.0],
            rotation: [0.0, 0.0, std::f64::consts::FRAC_PI_2],
            scale: [2.0, 3.0, 4.0],
        });
        let p = m.point([1.0, 0.0, 0.0]);
        for (a, b) in p.into_iter().zip([10.0, 23.0, 30.0]) {
            assert!((a - b).abs() < 1e-9);
        }
        let parent = Transform::pose(Pose {
            position: [1.0, 2.0, 3.0],
            ..Pose::default()
        });
        assert_eq!(parent.compose(m).point([0.0; 3]), [11.0, 22.0, 33.0]);
    }
}
