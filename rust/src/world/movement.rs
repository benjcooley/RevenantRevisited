// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              movement.rs - Fixed ticks and terrain movement           *
// *************************************************************************

use super::{sector::Position, walkmap::WalkMap};
use crate::assets::mesh::motion::{FIXED_ONE, Motion};
use anyhow::{Result, ensure};
use std::time::Duration;

pub const TICK_RATE: u32 = 24;
pub const MOVE_CHECK_DISTANCE: i32 = 8;

/// A stalled window cannot build an unbounded queue of simulation updates.
#[derive(Default)]
pub struct TickClock {
    scaled_nanos: u128,
}

impl TickClock {
    pub fn advance(&mut self, elapsed: Duration) -> u32 {
        self.scaled_nanos +=
            elapsed.min(Duration::from_millis(250)).as_nanos() * u128::from(TICK_RATE);
        let ticks = self.scaled_nanos / 1_000_000_000;
        self.scaled_nanos %= 1_000_000_000;
        ticks as u32
    }
}

#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub struct Step {
    pub moved: bool,
    pub blocked: bool,
}

/// Terrain-only movement component consuming authored 16.16 motion deltas.
/// Flying, falling, character collisions, and wall sliding are not simulated.
pub struct Walker {
    pub level: i32,
    position: Position,
    rotation: [u8; 3],
    radius: i32,
    fraction: [i32; 2],
}

impl Walker {
    pub fn new(level: i32, position: Position, radius: i32) -> Self {
        Self {
            level,
            position,
            rotation: [0; 3],
            radius,
            fraction: [0; 2],
        }
    }

    pub fn position(&self) -> Position {
        self.position
    }
    pub fn facing(&self) -> u8 {
        self.rotation[2]
    }
    pub fn rotation(&self) -> [u8; 3] {
        self.rotation
    }
    pub fn face(&mut self, angle: u8) {
        self.rotation[2] = angle;
    }
    pub fn clear_accumulator(&mut self) {
        self.fraction = [0; 2];
    }

    /// Preserve sub-unit translation between frames, including clip repeats.
    /// Vertical deltas are decoded by the asset loader, but require a future
    /// flying/falling controller; reject them instead of silently discarding them.
    pub fn apply_ground_motion(&mut self, map: &WalkMap, motion: Motion) -> Result<Step> {
        ensure!(
            motion.vertical == 0,
            "vertical authored motion requires a flying/falling controller"
        );
        let movement = motion.displacement(self.facing());
        for (angle, delta) in self.rotation.iter_mut().zip(motion.rotation) {
            *angle = angle.wrapping_add(delta);
        }
        let mut delta = [0; 2];
        for i in 0..2 {
            self.fraction[i] += movement[i];
            delta[i] = self.fraction[i] / FIXED_ONE;
            self.fraction[i] %= FIXED_ONE;
        }
        // Sweep even equal X/Y diagonals, avoiding the native split's equality gap.
        let count = ((delta[0].abs().max(delta[1].abs()) + MOVE_CHECK_DISTANCE - 1)
            / MOVE_CHECK_DISTANCE)
            .max(1);
        let start = self.position;
        let mut result = Step::default();
        for step in 1..=count {
            let Some(x) = start.x.checked_add(delta[0] * step / count) else {
                result.blocked = true;
                break;
            };
            let Some(y) = start.y.checked_add(delta[1] * step / count) else {
                result.blocked = true;
                break;
            };
            let mut next = Position {
                x,
                y,
                z: self.position.z,
            };
            if next == self.position {
                continue;
            }
            let ground = map.probe(self.level, next, self.radius);
            if !ground.permits(self.position.z) {
                result.blocked = true;
                break;
            }
            // This preview stays on the surface. Gravity and the character's
            // vertical smoothing belong to the forthcoming action runtime.
            next.z = i32::from(ground.height);
            self.position = next;
            result.moved = true;
        }
        if result.blocked {
            self.fraction = [0; 2];
        }
        Ok(result)
    }
}

pub fn input_facing(direction: [i8; 2]) -> Option<u8> {
    Some(match direction.map(i8::signum) {
        [0, -1] => 0,
        [1, -1] => 32,
        [1, 0] => 64,
        [1, 1] => 96,
        [0, 1] => 128,
        [-1, 1] => 160,
        [-1, 0] => 192,
        [-1, -1] => 224,
        _ => return None,
    })
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::world::walkmap::tests::{floor, state};

    fn tick(walker: &mut Walker, map: &WalkMap, direction: [i8; 2], speed: u8) -> Step {
        if let Some(angle) = input_facing(direction) {
            walker.face(angle);
            walker
                .apply_ground_motion(
                    map,
                    Motion {
                        distance: u16::from(speed) * 256,
                        ..Motion::default()
                    },
                )
                .unwrap()
        } else {
            walker.clear_accumulator();
            Step::default()
        }
    }

    #[test]
    fn fixed_ticks_do_not_depend_on_presentation_rate() {
        let mut fast = TickClock::default();
        let mut slow = TickClock::default();
        assert_eq!(
            (0..100)
                .map(|_| fast.advance(Duration::from_millis(10)))
                .sum::<u32>(),
            24
        );
        assert_eq!(
            (0..10)
                .map(|_| slow.advance(Duration::from_millis(100)))
                .sum::<u32>(),
            24
        );
        assert_eq!(fast.advance(Duration::from_secs(10)), 6);
        assert_eq!(fast.advance(Duration::ZERO), 0);
    }

    #[test]
    fn diagonal_fractions_and_facing_are_preserved() {
        let map = floor(64);
        let start = Position {
            x: 512,
            y: 512,
            z: 100,
        };
        for (direction, facing) in [([1, 1], 96), ([-1, -1], 224)] {
            let mut walker = Walker::new(0, start, 16);
            for _ in 0..24 {
                assert!(tick(&mut walker, &map, direction, 6).moved);
            }
            let distance = 24 * 6 * 181 / 256;
            assert_eq!(
                walker.position.x,
                start.x + i32::from(direction[0]) * distance
            );
            assert_eq!(
                walker.position.y,
                start.y + i32::from(direction[1]) * distance
            );
            assert_eq!(walker.facing(), facing);
            assert_eq!(tick(&mut walker, &map, [0, 0], 6), Step::default());
        }
    }

    #[test]
    fn fast_equal_diagonals_cannot_tunnel_through_a_one_cell_wall() {
        let mut map = floor(64);
        map.stamp(
            0,
            Position { x: 128, y: 0, z: 0 },
            0,
            &state(1, 64, vec![255; 64]),
            false,
        )
        .unwrap();
        let mut walker = Walker::new(
            0,
            Position {
                x: 64,
                y: 64,
                z: 100,
            },
            16,
        );
        let step = tick(&mut walker, &map, [1, 1], 255);
        assert!(step.moved && step.blocked);
        assert!(walker.position.x > 64 && walker.position.x < 120);
        assert_eq!(walker.position.x, walker.position.y);
        for _ in 0..100 {
            assert!(tick(&mut walker, &map, [1, 0], 6).blocked);
        }
        assert!(walker.position.x < 120);
    }

    #[test]
    fn crosses_loaded_sector_seams_and_stops_before_unloaded_ground() {
        let map = floor(128);
        let mut walker = Walker::new(
            0,
            Position {
                x: 1000,
                y: 512,
                z: 100,
            },
            16,
        );
        for _ in 0..8 {
            assert!(!tick(&mut walker, &map, [1, 0], 6).blocked);
        }
        assert_eq!(walker.position.x, 1048);
        for _ in 0..300 {
            tick(&mut walker, &map, [1, 0], 6);
        }
        assert!(walker.position.x > 1900 && walker.position.x < 2040);
        assert!(tick(&mut walker, &map, [1, 0], 6).blocked);
        assert!(tick(&mut walker, &map, [-1, 0], 6).moved);
    }

    #[test]
    fn subunit_motion_accumulates_and_clears_without_latent_translation() {
        let map = floor(64);
        let start = Position {
            x: 512,
            y: 512,
            z: 100,
        };
        let mut walker = Walker::new(0, start, 16);
        let quarter = Motion {
            distance: 64,
            ..Motion::default()
        };
        for _ in 0..3 {
            assert!(!walker.apply_ground_motion(&map, quarter).unwrap().moved);
        }
        assert!(walker.apply_ground_motion(&map, quarter).unwrap().moved);
        assert_eq!(walker.position.y, 511);
        for _ in 0..3 {
            walker.apply_ground_motion(&map, quarter).unwrap();
        }
        walker.clear_accumulator();
        assert!(!walker.apply_ground_motion(&map, quarter).unwrap().moved);
        assert_eq!(walker.position.y, 511);
    }

    #[test]
    fn relative_motion_uses_old_facing_then_wraps_all_rotations() {
        let map = floor(64);
        let mut walker = Walker::new(
            0,
            Position {
                x: 512,
                y: 512,
                z: 100,
            },
            16,
        );
        walker.face(64);
        walker
            .apply_ground_motion(
                &map,
                Motion {
                    distance: 256,
                    angle: 64,
                    rotation: [255, 1, 255],
                    ..Motion::default()
                },
            )
            .unwrap();
        assert_eq!(
            walker.position(),
            Position {
                x: 512,
                y: 513,
                z: 100
            }
        );
        assert_eq!(walker.rotation(), [255, 1, 63]);
        let before = walker.position();
        assert!(
            walker
                .apply_ground_motion(
                    &map,
                    Motion {
                        vertical: 1,
                        ..Motion::default()
                    }
                )
                .is_err()
        );
        assert_eq!(walker.position(), before);
        assert_eq!(walker.rotation(), [255, 1, 63]);
    }
}
