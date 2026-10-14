// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              exploration.rs - Controllable terrain preview            *
// *************************************************************************

use super::{
    DrawSource, Scene,
    movement::{Step, Walker, input_facing},
    sector::{MapId, Position, SECTOR_SIZE, SectorId},
};
use crate::assets::mesh::Mesh;
use crate::{
    assets::{cache::AssetCache, mesh::Clip, vfs::Vfs},
    render::{Draw, mesh::MeshDraw},
};
use anyhow::{Context, Result, ensure};
#[cfg(test)]
use std::sync::Arc;

/// Deliberately an exploration actor, separate from serialized game objects.
/// Full character actions, stats, inventory and scripts remain separate ports.
pub struct Exploration {
    pub walker: Walker,
    pub draw: usize,
    idle_state: u16,
    walk_state: u16,
    run_state: u16,
    state: u16,
    next_frame: u32,
    direction: Option<u8>,
}

#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub enum Gait {
    #[default]
    Walk,
    Run,
}

impl Exploration {
    pub const RADIUS: i32 = 16; // DEFSTAT(Character, Radius) default

    pub fn spawn(
        vfs: &mut Vfs,
        scene: &mut Scene,
        sector: SectorId,
        at: Option<[i32; 2]>,
    ) -> Result<Self> {
        Self::spawn_with_assets(vfs, scene, sector, at, &mut AssetCache::default())
    }
    pub fn spawn_with_assets(
        vfs: &mut Vfs,
        scene: &mut Scene,
        sector: SectorId,
        at: Option<[i32; 2]>,
        assets: &mut AssetCache,
    ) -> Result<Self> {
        let position = if let Some([x, y]) = at {
            let mut p = Position { x, y, z: 0 };
            p.z = i32::from(scene.walkmap.height(sector.level, p));
            ensure!(
                scene
                    .walkmap
                    .probe(sector.level, p, Self::RADIUS)
                    .permits(p.z),
                "spawn ({x}, {y}) has no safe ground for Locke's radius"
            );
            p
        } else {
            scene
                .walkmap
                .nearest_ground(
                    sector,
                    Position {
                        x: sector.x * SECTOR_SIZE + SECTOR_SIZE / 2,
                        y: sector.y * SECTOR_SIZE + SECTOR_SIZE / 2,
                        z: 0,
                    },
                    Self::RADIUS,
                )
                .context("no safe walk-preview spawn in the selected sector")?
        };
        let mesh = assets.mesh(vfs, "imagery/Chars/locke.i3d")?;
        let state = |name: &str| {
            mesh.clips
                .iter()
                .position(|clip| clip.name.eq_ignore_ascii_case(name))
                .map(|i| i as u16)
                .with_context(|| format!("Locke has no {name} clip"))
        };
        // "walk" is the relaxed stance; "walkf" is its forward movement clip.
        let idle_state = state("100:walk")?;
        let walk_state = state("walkf")?;
        let run_state = state("runf")?;
        for index in [walk_state, run_state] {
            let clip = &mesh.clips[usize::from(index)];
            ensure!(
                clip.motion.len() == clip.frames as usize
                    && clip.flags & Clip::NO_MOTION == 0
                    && clip.motion.iter().any(|m| m.distance != 0),
                "Locke's {} clip has no usable authored motion",
                clip.name
            );
            ensure!(
                clip.motion.iter().all(|m| m.vertical == 0),
                "Locke's {} clip needs vertical movement support",
                clip.name
            );
        }
        let mut draw = MeshDraw::new(mesh, position, [0; 3], idle_state, 0)?;
        draw.set_character_mode(false);
        let owner = scene.draws.len();
        scene.draws.push(Draw::Mesh(draw));
        scene.sources.push(DrawSource {
            name: "Locke (walk preview)".into(),
            sector,
            slot: 0,
            map_id: MapId(-1),
        });
        Ok(Self {
            walker: Walker::new(sector.level, position, Self::RADIUS),
            draw: owner,
            idle_state,
            walk_state,
            run_state,
            state: idle_state,
            next_frame: 0,
            direction: None,
        })
    }

    fn select(&mut self, state: u16, mesh: &Mesh) {
        if self.state == state {
            return;
        }
        self.state = state;
        self.next_frame = 0;
        if mesh.clips[usize::from(state)].flags & Clip::ROOT != 0 {
            self.walker.clear_accumulator();
        }
    }

    pub fn tick(&mut self, scene: &mut Scene, direction: [i8; 2], gait: Gait) -> Result<Step> {
        let Draw::Mesh(draw) = &mut scene.draws[self.draw] else {
            anyhow::bail!("walk preview draw is no longer a mesh");
        };
        let facing = input_facing(direction);
        if facing != self.direction {
            if let Some(angle) = facing {
                self.walker.face(angle);
            }
            self.direction = facing;
        }
        let state = match (facing, gait) {
            (None, _) => self.idle_state,
            (Some(_), Gait::Walk) => self.walk_state,
            (Some(_), Gait::Run) => self.run_state,
        };
        self.select(state, &draw.mesh);
        let clip = &draw.mesh.clips[usize::from(self.state)];
        let mut frame = self.next_frame;
        let step = if let Some(motion) = clip.motion_at(frame) {
            self.walker.apply_ground_motion(&scene.walkmap, motion)?
        } else {
            Step::default()
        };
        // Held input repeats the movement action, even when AF_LOOPING is clear.
        // Advance through zero-distance frames; only an actual collision blocks.
        self.next_frame = (frame + 1) % clip.frames;
        if step.blocked {
            // ResolveMove forces the relaxed root state when terrain blocks it.
            if let Some(angle) = facing {
                self.walker.face(angle);
            }
            self.select(self.idle_state, &draw.mesh);
            frame = 0;
        }
        draw.set_pose(
            self.walker.position(),
            self.walker.rotation(),
            self.state,
            frame,
        )?;
        let source = &mut scene.sources[self.draw];
        source.sector.x = self.walker.position().x.div_euclid(SECTOR_SIZE);
        source.sector.y = self.walker.position().y.div_euclid(SECTOR_SIZE);
        Ok(step)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::{
        assets::mesh::motion::Motion,
        world::{SceneReport, walkmap::tests::floor},
    };
    use std::collections::BTreeMap;

    fn preview() -> (Scene, Exploration) {
        let clip = |name: &str, flags, distances: &[u16]| Clip {
            name: name.into(),
            flags,
            frames: distances.len() as u32,
            motion: distances
                .iter()
                .map(|&distance| Motion {
                    distance,
                    ..Motion::default()
                })
                .collect(),
            bones: vec![],
        };
        let mesh = Arc::new(Mesh {
            morph: Vec::new(),
            vertices: vec![],
            faces: vec![],
            materials: vec![],
            textures: vec![],
            parts: vec![],
            clips: vec![
                clip("idle", Clip::ROOT | Clip::NO_MOTION, &[256, 256]),
                clip("walk", 0, &[0, 128, 256]),
                clip("run", 0, &[512, 512]),
            ],
        });
        let position = Position {
            x: 512,
            y: 512,
            z: 100,
        };
        let draw = MeshDraw::new(mesh, position, [0; 3], 0, 0).unwrap();
        let scene = Scene {
            sectors: BTreeMap::new(),
            draws: vec![Draw::Mesh(draw)],
            sources: vec![DrawSource {
                name: "test".into(),
                sector: SectorId::default(),
                slot: 0,
                map_id: MapId(-1),
            }],
            report: SceneReport::default(),
            walkmap: floor(64),
        };
        let actor = Exploration {
            walker: Walker::new(0, position, 16),
            draw: 0,
            idle_state: 0,
            walk_state: 1,
            run_state: 2,
            state: 0,
            next_frame: 0,
            direction: None,
        };
        (scene, actor)
    }

    #[test]
    fn zero_distance_frames_advance_and_clip_repeats_keep_fractional_motion() {
        let (mut scene, mut actor) = preview();
        for (tick, expected_x) in [512, 512, 513, 513, 514, 515].into_iter().enumerate() {
            actor.tick(&mut scene, [1, 0], Gait::Walk).unwrap();
            assert_eq!(actor.walker.position().x, expected_x, "tick {tick}");
            let Draw::Mesh(draw) = &scene.draws[0] else {
                unreachable!()
            };
            assert_eq!(draw.state(), 1);
            assert_eq!(draw.frame(), tick as u32 % 3);
            assert_eq!(draw.position, actor.walker.position());
        }
    }

    #[test]
    fn idle_root_stops_and_clears_fraction_and_run_switch_starts_its_first_sample() {
        let (mut scene, mut actor) = preview();
        actor.tick(&mut scene, [1, 0], Gait::Walk).unwrap();
        actor.tick(&mut scene, [1, 0], Gait::Walk).unwrap(); // half a unit retained
        for _ in 0..4 {
            assert!(!actor.tick(&mut scene, [0, 0], Gait::Walk).unwrap().moved);
        }
        actor.tick(&mut scene, [1, 0], Gait::Walk).unwrap();
        actor.tick(&mut scene, [1, 0], Gait::Walk).unwrap();
        assert_eq!(actor.walker.position().x, 512); // the old half was cleared
        actor.tick(&mut scene, [1, 0], Gait::Run).unwrap();
        assert_eq!(actor.walker.position().x, 514);
        let Draw::Mesh(draw) = &scene.draws[0] else {
            unreachable!()
        };
        assert_eq!((draw.state(), draw.frame()), (2, 0));
    }
}
