// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              walkmap.rs - Sector heights and terrain collision        *
// *************************************************************************

//! Static walk heights: gamemap.cpp::StampTileWalkmap, sector.cpp::WalkmapHandler,
//! and mappane.cpp::GetWalkHeightRadius. Zero denotes a hole, not ground at Z=0.
use super::sector::{ObjectFlags, Position, SECTOR_SIZE, Sector, SectorId};
use crate::assets::{
    catalog::{Catalog, ClassId},
    imagery::{ImageryState, decode_state},
    vfs::{Vfs, normalize},
};
use anyhow::{Result, ensure};
use std::collections::BTreeMap;

pub const GRID: i32 = 16;
pub const SIDE: usize = (SECTOR_SIZE / GRID) as usize;
pub const MAX_STEP: i32 = 32;

#[derive(Debug, Default)]
pub struct WalkMap {
    sectors: BTreeMap<SectorId, Box<[u16]>>,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct GroundProbe {
    pub height: u16,
    /// Largest absolute difference between adjacent cells within the radius.
    pub max_delta: i32,
    /// At least one cell within the radius has no walk surface.
    pub hole: bool,
}

impl GroundProbe {
    pub fn permits(self, from_z: i32) -> bool {
        self.height != 0
            && (i64::from(from_z) - i64::from(self.height)).abs() <= i64::from(MAX_STEP)
            && !self.hole
            && self.max_delta <= MAX_STEP
    }
}

impl WalkMap {
    pub fn new(ids: impl IntoIterator<Item = SectorId>) -> Self {
        Self {
            sectors: ids
                .into_iter()
                .map(|id| (id, vec![0; SIDE * SIDE].into()))
                .collect(),
        }
    }

    /// Top-level objects only, as in TransferAllWalkmaps(CHECK_NOINVENT).
    /// Visibility and still-image availability do not decide collision.
    pub fn load(
        vfs: &mut Vfs,
        catalog: &Catalog,
        sectors: &BTreeMap<SectorId, Sector>,
    ) -> (Self, Vec<String>) {
        let mut map = Self::new(sectors.keys().copied());
        let mut warnings = Vec::new();
        let mut cache: BTreeMap<(String, u16), Option<ImageryState>> = BTreeMap::new();
        for (id, sector) in sectors {
            for (slot, object) in sector.objects.iter().enumerate() {
                let Some(object) = object else { continue };
                if object.flags.contains(ObjectFlags::NO_WALK)
                    || object.inventory_slot >= 0
                    || object.map_id.0 < 0
                {
                    continue;
                }
                let Some(ty) = catalog.get(object.class, object.type_id) else {
                    continue;
                };
                let path = match normalize(&format!("imagery/{}", ty.imagery)) {
                    Ok(path) => path,
                    Err(error) => {
                        warnings.push(format!("walkmap {id} slot {slot}: {error:#}"));
                        continue;
                    }
                };
                let key = (path, object.state);
                let state = cache.entry(key.clone()).or_insert_with(|| {
                    match vfs
                        .read(&key.0)
                        .and_then(|bytes| decode_state(&bytes, key.1))
                    {
                        Ok(state) => Some(state),
                        Err(error) => {
                            warnings.push(format!("walkmap {} state {}: {error:#}", key.0, key.1));
                            None
                        }
                    }
                });
                if let Some(state) = state {
                    if let Err(error) = map.stamp(
                        id.level,
                        object.position,
                        object.rotation[2],
                        state,
                        object.class == ClassId::EXIT,
                    ) {
                        warnings.push(format!("walkmap {id} slot {slot}: {error:#}"));
                    }
                }
            }
        }
        (map, warnings)
    }

    /// Retain the C++ facing lookup, including its unusual west-facing source
    /// indexing. Validate the whole stamp before applying it; native out-of-bounds
    /// reads become an error, never a partially applied collision surface.
    pub fn stamp(
        &mut self,
        level: i32,
        position: Position,
        facing: u8,
        state: &ImageryState,
        override_height: bool,
    ) -> Result<()> {
        if state.walkmap.is_empty() {
            return Ok(());
        }
        let [w, h, _] = state.world_size.map(i32::from);
        ensure!(
            w > 0 && h > 0 && state.walkmap.len() == (w * h) as usize,
            "walkmap dimensions do not match data"
        );
        let [mut rx, mut ry, _] = state.world_registration.map(i32::from);
        let (mut width, mut height) = (w, h);
        match facing {
            32..=95 => {
                std::mem::swap(&mut rx, &mut ry);
                (width, height) = (h, w);
            }
            96..=159 => rx = w - rx - 1,
            160..=223 => {
                ry = w - rx - 1;
                (width, height) = (h, w);
            }
            _ => {}
        }
        let mut applied = Vec::with_capacity(state.walkmap.len());
        for y in 0..height {
            for x in 0..width {
                let source = match facing {
                    0 => y * w + x,
                    1..=127 => (h - x - 1) * w + y,
                    128..=191 => y * w + w - x - 1,
                    _ => (h - x - 1) * w + w - x - 1,
                };
                ensure!(
                    source >= 0 && (source as usize) < state.walkmap.len(),
                    "C++ facing {facing} indexes outside {w}x{h} walkmap"
                );
                applied.push(state.walkmap[source as usize]);
            }
        }
        let origin_x = i64::from(position.x).div_euclid(i64::from(GRID)) - i64::from(rx);
        let origin_y = i64::from(position.y).div_euclid(i64::from(GRID)) - i64::from(ry);
        for y in 0..height {
            for x in 0..width {
                let value = applied[(y * width + x) as usize];
                if value == 0 {
                    continue;
                }
                let gx = origin_x + i64::from(x);
                let gy = origin_y + i64::from(y);
                let id = SectorId {
                    level,
                    x: gx.div_euclid(SIDE as i64) as i32,
                    y: gy.div_euclid(SIDE as i64) as i32,
                };
                let Some(cells) = self.sectors.get_mut(&id) else {
                    continue;
                };
                let index = gy.rem_euclid(SIDE as i64) as usize * SIDE
                    + gx.rem_euclid(SIDE as i64) as usize;
                let height = (i64::from(position.z) + i64::from(value)).clamp(1, 65535) as u16;
                if override_height || height > cells[index] {
                    cells[index] = height;
                }
            }
        }
        Ok(())
    }

    pub fn cells(&self, sector: SectorId) -> Option<&[u16]> {
        self.sectors.get(&sector).map(AsRef::as_ref)
    }

    pub fn covered_cells(&self) -> usize {
        self.sectors
            .values()
            .map(|cells| cells.iter().filter(|&&h| h != 0).count())
            .sum()
    }

    /// +8 centers the grid on the authored world coordinate, matching C++.
    /// Absent sectors are holes: the preview cannot walk out of its loaded map.
    pub fn height(&self, level: i32, position: Position) -> u16 {
        self.height_xy(level, i64::from(position.x), i64::from(position.y))
    }

    fn height_xy(&self, level: i32, x: i64, y: i64) -> u16 {
        let gx = (x + i64::from(GRID / 2)).div_euclid(i64::from(GRID));
        let gy = (y + i64::from(GRID / 2)).div_euclid(i64::from(GRID));
        let id = SectorId {
            level,
            x: gx.div_euclid(SIDE as i64) as i32,
            y: gy.div_euclid(SIDE as i64) as i32,
        };
        self.sectors.get(&id).map_or(0, |cells| {
            cells[gy.rem_euclid(SIDE as i64) as usize * SIDE + gx.rem_euclid(SIDE as i64) as usize]
        })
    }

    pub fn probe(&self, level: i32, position: Position, radius: i32) -> GroundProbe {
        let mut result = GroundProbe {
            height: self.height(level, position),
            max_delta: 0,
            hole: false,
        };
        // Retail bypasses the footprint for these radii. Clamp before any
        // coordinate arithmetic; widened coordinates also handle map extremes.
        if radius <= 0 || radius >= 240 {
            return result;
        }
        let radius = i64::from(if (radius + 15) & !15 >= 128 {
            111
        } else {
            radius
        });
        let px = i64::from(position.x);
        let py = i64::from(position.y);
        let start_x = (px - radius) & !15;
        let start_y = (py - radius) & !15;
        let end_x = (px + radius + 16) & !15;
        let end_y = (py + radius + 16) & !15;
        let width = ((end_x - start_x) / 16 + 1) as usize;
        let height = ((end_y - start_y) / 16 + 1) as usize;
        let mut heights = vec![None; width * height];
        for y in 0..height {
            for x in 0..width {
                let wx = start_x + x as i64 * 16;
                let wy = start_y + y as i64 * 16;
                let dx = (px.clamp(wx - 8, wx + 7) - px).abs();
                let dy = (py.clamp(wy - 8, wy + 7) - py).abs();
                // DistTable rounds Euclidean distance to the nearest integer.
                // For these bounded radii, round(sqrt(d2)) <= r is exactly
                // 4*d2 < (2*r+1)^2; no table allocation or floating point needed.
                if 4 * (dx * dx + dy * dy) >= (2 * radius + 1).pow(2) {
                    continue;
                }
                let at = y * width + x;
                let h = i32::from(self.height_xy(level, wx, wy));
                heights[at] = Some(h);
                result.hole |= h == 0;
                let left = x.checked_sub(1).and_then(|_| heights[at - 1]);
                let above = y.checked_sub(1).and_then(|_| heights[at - width]);
                for neighbor in [left, above].into_iter().flatten() {
                    result.max_delta = result.max_delta.max((h - neighbor).abs());
                }
            }
        }
        result
    }

    /// Preview spawn only. Search this sector for the nearest safe grid center.
    pub fn nearest_ground(
        &self,
        sector: SectorId,
        around: Position,
        radius: i32,
    ) -> Option<Position> {
        let cells = self.cells(sector)?;
        let mut best: Option<(i64, Position)> = None;
        for (i, &height) in cells.iter().enumerate().filter(|(_, h)| **h != 0) {
            let p = Position {
                x: sector.x * SECTOR_SIZE + (i % SIDE) as i32 * GRID,
                y: sector.y * SECTOR_SIZE + (i / SIDE) as i32 * GRID,
                z: i32::from(height),
            };
            let d = (i64::from(p.x) - i64::from(around.x)).pow(2)
                + (i64::from(p.y) - i64::from(around.y)).pow(2);
            if best.is_none_or(|(old, _)| d < old)
                && self.probe(sector.level, p, radius).permits(p.z)
            {
                best = Some((d, p));
            }
        }
        best.map(|(_, p)| p)
    }
}

#[cfg(test)]
pub(super) mod tests {
    use super::*;

    pub fn state(width: i16, height: i16, values: Vec<u8>) -> ImageryState {
        ImageryState {
            name: "terrain".into(),
            registration: [0; 3],
            world_registration: [0; 3],
            world_size: [width, height, 0],
            walkmap: values,
        }
    }

    pub fn floor(width: i16) -> WalkMap {
        let mut map =
            WalkMap::new((0..(i32::from(width) + 63) / 64).map(|x| SectorId { level: 0, x, y: 0 }));
        map.stamp(
            0,
            Position::default(),
            0,
            &state(width, 64, vec![100; width as usize * 64]),
            false,
        )
        .unwrap();
        map
    }

    #[test]
    fn hidden_mesh_metadata_stamps_but_nowalk_inventory_and_unmapped_objects_do_not() {
        use super::super::sector::{Inventory, MapId, Object};
        use crate::assets::catalog::TypeId;
        let directory = tempfile::tempdir().unwrap();
        // An I3D common header with a walkmap and no mesh body whatsoever.
        let mut file = vec![0; 105];
        file[..4].copy_from_slice(b"CGSR");
        file[7] = 1;
        file[16..20].copy_from_slice(&85u32.to_le_bytes());
        file[20..24].copy_from_slice(&1i32.to_le_bytes());
        file[24..28].copy_from_slice(&1i32.to_le_bytes());
        file[60..64].copy_from_slice(&44i32.to_le_bytes());
        file[94..96].copy_from_slice(&1i16.to_le_bytes());
        file[96..98].copy_from_slice(&1i16.to_le_bytes());
        file[104] = 7;
        std::fs::create_dir(directory.path().join("imagery")).unwrap();
        std::fs::write(directory.path().join("imagery/terrain.i3d"), file).unwrap();
        let mut vfs = Vfs::open(directory.path().to_owned(), "test", None).unwrap();
        let catalog =
            Catalog::parse(b"CLASS TILE\nTYPES\nBEGIN\n\"floor\" \"terrain.i3d\" 1\nEND").unwrap();
        let make = |x, flags, inventory_slot, map_id| {
            Some(Object {
                version: 0,
                base_versions: vec![],
                class: ClassId::TILE,
                type_id: TypeId(1),
                name: String::new(),
                flags: ObjectFlags(flags),
                position: Position { x, y: 0, z: 0 },
                velocity: None,
                state: 0,
                level: None,
                inventory_slot,
                inventory_index: -1,
                shadow: -1,
                rotation: [0; 3],
                map_id: MapId(map_id),
                animation: None,
                group: 0,
                stats: vec![],
                light: None,
                extension: vec![],
                inventory: Inventory::Decoded(vec![]),
            })
        };
        let id = SectorId::default();
        let sectors = BTreeMap::from([(
            id,
            Sector {
                id,
                version: 15,
                state_hash: 0,
                objects: vec![
                    make(0, ObjectFlags::INVISIBLE, -1, 0),
                    make(16, ObjectFlags::NO_WALK, -1, 1),
                    make(32, 0, 0, 2),
                    make(48, 0, -1, -1),
                ],
            },
        )]);
        let (map, warnings) = WalkMap::load(&mut vfs, &catalog, &sectors);
        assert!(warnings.is_empty(), "{warnings:?}");
        assert_eq!(map.covered_cells(), 1);
        assert_eq!(map.height(0, Position::default()), 7);
    }

    #[test]
    fn registration_crosses_sectors_and_lookup_rounds_at_half_cell() {
        let mut map = WalkMap::new([
            SectorId::default(),
            SectorId {
                x: 1,
                ..SectorId::default()
            },
        ]);
        let mut stamp = state(2, 2, vec![1, 2, 3, 4]);
        stamp.world_registration = [1, 1, 0];
        map.stamp(
            0,
            Position {
                x: 1024,
                y: 16,
                z: 100,
            },
            0,
            &stamp,
            false,
        )
        .unwrap();
        assert_eq!(map.covered_cells(), 4);
        for (x, y, h) in [
            (1008, 0, 101),
            (1015, 0, 101),
            (1016, 0, 102),
            (1024, 16, 104),
            (1008, 16, 103),
        ] {
            assert_eq!(map.height(0, Position { x, y, z: 0 }), h);
        }
        assert_eq!(
            map.height(
                1,
                Position {
                    x: 1024,
                    y: 0,
                    z: 0
                }
            ),
            0
        );
        assert_eq!(map.height(0, Position { x: -9, y: 0, z: 0 }), 0);
    }

    #[test]
    fn highest_surface_wins_but_exit_override_replaces_nonzero_cells() {
        let mut map = floor(64);
        let shape = state(2, 1, vec![20, 0]);
        map.stamp(0, Position::default(), 0, &shape, false).unwrap();
        assert_eq!(&map.cells(SectorId::default()).unwrap()[..2], &[100, 100]);
        map.stamp(0, Position::default(), 0, &shape, true).unwrap();
        assert_eq!(&map.cells(SectorId::default()).unwrap()[..2], &[20, 100]);
        map.stamp(
            0,
            Position {
                z: -1000,
                ..Position::default()
            },
            0,
            &shape,
            true,
        )
        .unwrap();
        assert_eq!(map.height(0, Position::default()), 1);
        map.stamp(
            0,
            Position {
                z: i32::MAX,
                ..Position::default()
            },
            0,
            &shape,
            false,
        )
        .unwrap();
        assert_eq!(map.height(0, Position::default()), u16::MAX);
    }

    #[test]
    fn facing_lookups_match_cpp_and_bad_lookup_does_not_partially_stamp() {
        let shape = state(2, 2, vec![1, 2, 3, 4]);
        for (facing, expected) in [
            (0, [1, 2, 3, 4]),
            (64, [3, 1, 4, 2]),
            (128, [2, 1, 4, 3]),
            (192, [4, 1, 4, 1]),
        ] {
            let mut map = WalkMap::new([SectorId::default()]);
            map.stamp(0, Position { x: 32, y: 32, z: 0 }, facing, &shape, false)
                .unwrap();
            // Native facing bounds shift the origin for south/west.
            let x = if facing == 128 { 16 } else { 32 };
            let y = if facing == 192 { 16 } else { 32 };
            let mut actual = Vec::new();
            for dy in [0, 16] {
                for dx in [0, 16] {
                    actual.push(map.height(
                        0,
                        Position {
                            x: x + dx,
                            y: y + dy,
                            z: 0,
                        },
                    ));
                }
            }
            assert_eq!(actual, expected, "facing {facing}");
        }
        let mut map = floor(64);
        let before = map.cells(SectorId::default()).unwrap().to_vec();
        assert!(
            map.stamp(
                0,
                Position::default(),
                192,
                &state(2, 3, vec![255; 6]),
                true
            )
            .is_err()
        );
        assert_eq!(map.cells(SectorId::default()).unwrap(), before);
    }

    #[test]
    fn stairs_use_adjacent_deltas_not_total_height_range() {
        let mut map = WalkMap::new([SectorId::default()]);
        let stairs: Vec<_> = (0..16 * 16).map(|i| 1 + (i % 16) as u8 * 16).collect();
        map.stamp(0, Position::default(), 0, &state(16, 16, stairs), false)
            .unwrap();
        let p = Position {
            x: 112,
            y: 112,
            z: 113,
        };
        let probe = map.probe(0, p, 32);
        assert_eq!(probe.height, 113);
        assert_eq!(probe.max_delta, 16);
        assert!(!probe.hole);
        assert!(probe.permits(p.z));
        assert!(probe.permits(p.z + MAX_STEP));
        assert!(!probe.permits(p.z + MAX_STEP + 1));
        map.stamp(
            0,
            Position { x: 128, y: 0, z: 0 },
            0,
            &state(1, 16, vec![250; 16]),
            true,
        )
        .unwrap();
        assert!(!map.probe(0, p, 32).permits(p.z));
    }

    #[test]
    fn footprint_holes_block_even_when_the_drop_is_below_the_step_limit() {
        let mut map = WalkMap::new([SectorId::default()]);
        map.stamp(
            0,
            Position::default(),
            0,
            &state(64, 64, vec![8; 64 * 64]),
            false,
        )
        .unwrap();
        let edge = Position { x: 0, y: 512, z: 8 };
        let probe = map.probe(0, edge, 16);
        assert_eq!(probe.height, 8);
        assert_eq!(probe.max_delta, 8);
        assert!(probe.hole);
        assert!(!probe.permits(edge.z));
        assert!(map.probe(0, edge, 240).permits(edge.z));
        assert_eq!(map.probe(0, edge, 111), map.probe(0, edge, 239));
    }

    #[test]
    fn absent_ground_and_large_radii_remain_bounded() {
        let map = floor(64);
        assert!(
            !map.probe(
                0,
                Position {
                    x: 0,
                    y: 512,
                    z: 100
                },
                16
            )
            .permits(100)
        );
        let p = Position {
            x: 512,
            y: 512,
            z: 100,
        };
        assert!(map.probe(0, p, i32::MAX).permits(100));
        assert_eq!(map.probe(0, p, 111), map.probe(0, p, 239));
        assert_eq!(map.probe(0, p, 0), map.probe(0, p, 240));
        assert_eq!(
            map.height(
                0,
                Position {
                    x: i32::MAX,
                    y: i32::MIN,
                    z: 0
                }
            ),
            0
        );
    }
}
