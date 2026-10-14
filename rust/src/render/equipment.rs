// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             equipment.rs - Character mesh replacement bindings        *
// *************************************************************************

//! Geometry bindings, independent of inventory ownership and gameplay stats.
//! See charanimator.cpp::ProcessEquipment and uiequiptest.cpp's retail names.
use crate::assets::mesh::Mesh;
use anyhow::{Context, Result, bail, ensure};
use std::{str::FromStr, sync::Arc};

#[derive(Clone, Copy, Debug, PartialEq, Eq, PartialOrd, Ord)]
pub enum Slot {
    Head,
    Neck,
    Body,
    OffHand,
    PrimeHand,
    RightAccessory,
    LeftAccessory,
    RangedWeapon,
    Ammo,
    Legs,
    Feet,
}

impl Slot {
    pub(super) fn active(self, bow_mode: bool) -> bool {
        match self {
            Self::PrimeHand => !bow_mode,
            Self::RangedWeapon => bow_mode,
            _ => true,
        }
    }
}

impl FromStr for Slot {
    type Err = anyhow::Error;
    fn from_str(value: &str) -> Result<Self> {
        Ok(match value.to_ascii_lowercase().as_str() {
            "head" => Self::Head,
            "neck" => Self::Neck,
            "body" => Self::Body,
            "offhand" => Self::OffHand,
            "primehand" => Self::PrimeHand,
            "rightaccessory" => Self::RightAccessory,
            "leftaccessory" => Self::LeftAccessory,
            "rangedweapon" => Self::RangedWeapon,
            "ammo" => Self::Ammo,
            "legs" => Self::Legs,
            "feet" => Self::Feet,
            _ => bail!(
                "unknown equipment slot {value:?}; use head, neck, body, offhand, primehand, rightaccessory, leftaccessory, rangedweapon, ammo, legs or feet"
            ),
        })
    }
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Binding {
    pub equipment_part: usize,
    pub character_part: usize,
}

#[derive(Clone)]
pub struct Attachment {
    pub(super) mesh: Arc<Mesh>,
    state: usize,
    pub(super) bindings: Vec<Binding>,
}

fn strip_body<'a>(name: &'a str, body_type: &str) -> &'a str {
    let name = name.trim_start_matches('*');
    if name
        .get(..body_type.len())
        .is_some_and(|prefix| prefix.eq_ignore_ascii_case(body_type))
    {
        &name[body_type.len()..]
    } else {
        name
    }
}

impl Attachment {
    pub fn mesh(&self) -> &Mesh {
        &self.mesh
    }
    pub fn state(&self) -> usize {
        self.state
    }
    pub fn bindings(&self) -> &[Binding] {
        &self.bindings
    }

    pub(super) fn bind(
        character: &Mesh,
        mesh: Arc<Mesh>,
        slot: Slot,
        body_type: &str,
    ) -> Result<Self> {
        ensure!(!body_type.is_empty(), "equipment body type is empty");
        // Preserve native state order: the first still/all/body-type match wins.
        let state = mesh
            .clips
            .iter()
            .position(|clip| {
                ["still", "all", body_type]
                    .iter()
                    .any(|name| clip.name.eq_ignore_ascii_case(name))
            })
            .or_else(|| {
                mesh.clips.iter().position(|clip| {
                    mesh.parts.iter().zip(&clip.bones).any(|(part, bone)| {
                        bone.track.is_some()
                            && part
                                .name
                                .trim_start_matches('*')
                                .get(..body_type.len())
                                .is_some_and(|prefix| prefix.eq_ignore_ascii_case(body_type))
                    })
                })
            })
            .with_context(|| format!("equipment has no state for body type {body_type:?}"))?;
        let find = |name: &str| {
            character
                .parts
                .iter()
                .position(|part| part.name.eq_ignore_ascii_case(name))
        };
        let mut bindings = Vec::new();
        for (equipment_part, part) in mesh.parts.iter().enumerate() {
            if mesh.clips[state].bones[equipment_part].track.is_none() {
                continue;
            }
            let name = strip_body(&part.name, body_type);
            let character_part = if matches!(slot, Slot::PrimeHand | Slot::RangedWeapon) {
                find("weapon")
                    .or_else(|| find("sword"))
                    .or_else(|| find(name))
            } else {
                find(name)
            };
            if let Some(character_part) = character_part {
                bindings.push(Binding {
                    equipment_part,
                    character_part,
                });
            }
        }
        ensure!(
            !bindings.is_empty(),
            "equipment has no matching character parts for {slot:?} / {body_type}"
        );
        // The equipment's stored parents and local animation are deliberately
        // unused: OBJ3D_MATRIX supplies each character part's evaluated matrix.
        Ok(Self {
            mesh,
            state,
            bindings,
        })
    }
}

#[cfg(test)]
pub(crate) mod tests {
    use super::*;
    use crate::assets::{
        binary::Bytes,
        mesh::{Bone, Clip, Part, Vertex, animation::Track},
    };

    pub(crate) fn mesh(names: &[&str]) -> Mesh {
        let keys: Vec<_> = [1u32 | (4 << 2), 1 | (8 << 2)]
            .into_iter()
            .flat_map(u32::to_le_bytes)
            .collect();
        Mesh {
            morph: Vec::new(),
            vertices: vec![Vertex {
                position: [0.0, 0.0, 2.0],
                normal: [0.0; 3],
                uv: [0.0; 2],
            }],
            faces: vec![],
            materials: vec![],
            textures: vec![],
            parts: names
                .iter()
                .map(|name| Part {
                    name: (*name).into(),
                    vertices: 0..1,
                    material: 0,
                    batches: vec![],
                })
                .collect(),
            clips: vec![Clip {
                name: "STILL".into(),
                frames: 2,
                flags: 0,
                motion: vec![],
                bones: names
                    .iter()
                    .map(|_| Bone {
                        parent: None,
                        track: Some(Track::decode(Bytes(&keys), 0, 2, true).unwrap()),
                    })
                    .collect(),
            }],
        }
    }

    #[test]
    fn body_markers_hidden_parts_and_unmatched_variants_bind_without_parent_traversal() {
        let character = mesh(&["chest", "rhand", "head"]);
        let mut gear = mesh(&[
            "**NormalChest",
            "*largechest",
            "ground",
            "*normalrhand",
            "*normalhead",
        ]);
        gear.clips[0].bones[0].parent = Some(0);
        gear.clips[0].bones[4].track = None;
        let bound = Attachment::bind(&character, Arc::new(gear), Slot::Body, "normal").unwrap();
        assert_eq!(
            bound.bindings(),
            [
                Binding {
                    equipment_part: 0,
                    character_part: 0
                },
                Binding {
                    equipment_part: 3,
                    character_part: 1
                }
            ]
        );
        assert!(bound.mesh.clips[0].standalone_order().is_err());
    }

    #[test]
    fn state_selection_keeps_file_order_and_can_use_body_prefix_fallback() {
        let character = mesh(&["chest"]);
        let mut gear = mesh(&["normalchest"]);
        gear.clips.push(mesh(&["normalchest"]).clips.remove(0));
        gear.clips[1].name = "normal".into();
        assert_eq!(
            Attachment::bind(&character, Arc::new(gear), Slot::Body, "normal")
                .unwrap()
                .state(),
            0
        );
        let mut gear = mesh(&["*normalchest"]);
        gear.clips[0].name = "inventory".into();
        assert_eq!(
            Attachment::bind(&character, Arc::new(gear), Slot::Body, "normal")
                .unwrap()
                .state(),
            0
        );
        assert!(
            Attachment::bind(
                &character,
                Arc::new(mesh(&["*largechest"])),
                Slot::Body,
                "normal"
            )
            .is_err()
        );
    }

    #[test]
    fn hand_slots_prefer_weapon_then_sword_then_named_part() {
        for (parts, expected) in [
            (&["rhand", "sword", "weapon"][..], 2),
            (&["rhand", "sword"][..], 1),
            (&["rhand"][..], 0),
        ] {
            for slot in [Slot::PrimeHand, Slot::RangedWeapon] {
                let bound =
                    Attachment::bind(&mesh(parts), Arc::new(mesh(&["rhand"])), slot, "normal")
                        .unwrap();
                assert_eq!(bound.bindings()[0].character_part, expected);
            }
        }
        assert!(Slot::PrimeHand.active(false));
        assert!(!Slot::PrimeHand.active(true));
        assert!(Slot::RangedWeapon.active(true));
        assert!(!Slot::RangedWeapon.active(false));
    }
}
