// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     mod.rs - World scene assembly                     *
// *************************************************************************

pub mod exploration;
pub mod movement;
pub mod sector;
pub mod walkmap;

use crate::{
    assets::{
        cache::AssetCache,
        catalog::Catalog,
        imagery::Tile,
        mesh::Mesh,
        vfs::{Vfs, normalize},
    },
    render::{Draw, TileDraw, mesh::MeshDraw},
};
use anyhow::{Context, Result, ensure};
use sector::{MapId, ObjectFlags, Sector, SectorId};
use std::{
    collections::{BTreeMap, BTreeSet},
    sync::Arc,
};

pub struct DrawSource {
    pub name: String,
    pub sector: SectorId,
    pub slot: usize,
    pub map_id: MapId,
}

#[derive(Default, Debug)]
pub struct SceneReport {
    pub sectors: usize,
    pub objects: usize,
    pub tile_assets: usize,
    pub mesh_objects: usize,
    pub mesh_assets: usize,
    pub effect_objects: usize,
    pub hidden_objects: usize,
    pub empty_states: usize,
    pub unknown_types: usize,
    pub warnings: Vec<String>,
}

pub struct Scene {
    pub sectors: BTreeMap<SectorId, Sector>,
    pub draws: Vec<Draw>,
    pub sources: Vec<DrawSource>,
    pub walkmap: walkmap::WalkMap,
    pub report: SceneReport,
}

pub fn sector_ids(vfs: &Vfs) -> Result<BTreeSet<SectorId>> {
    Ok(vfs
        .list("map")?
        .iter()
        .filter_map(|name| SectorId::from_path(name))
        .collect())
}

/// Incremental scene assembly: one sector per step, then the walkmap. The
/// synchronous loader below uses the same path, so validation and gameplay agree.
pub struct SceneLoader {
    scene: Scene,
    tiles: BTreeMap<(String, u16), Option<Arc<Tile>>>,
    meshes: BTreeMap<String, Option<Arc<Mesh>>>,
    pending: std::vec::IntoIter<SectorId>,
    total: usize,
    finished: bool,
}
impl SceneLoader {
    pub fn new(ids: &[SectorId]) -> Self {
        let scene = Scene {
            sectors: BTreeMap::new(),
            draws: Vec::new(),
            sources: Vec::new(),
            walkmap: walkmap::WalkMap::default(),
            report: SceneReport::default(),
        };
        Self {
            scene,
            tiles: BTreeMap::new(),
            meshes: BTreeMap::new(),
            pending: Vec::from(ids).into_iter(),
            total: ids.len() + 1,
            finished: false,
        }
    }
    pub fn progress(&self) -> u16 {
        if self.finished {
            1000
        } else {
            ((self.total - self.pending.len() - 1) * 1000 / self.total) as u16
        }
    }
    /// True means another step remains. Errors leave this loader disposable.
    pub fn step(
        &mut self,
        vfs: &mut Vfs,
        catalog: &Catalog,
        assets: &mut AssetCache,
    ) -> Result<bool> {
        if self.finished {
            return Ok(false);
        }
        let scene = &mut self.scene;
        let cache = &mut self.tiles;
        let meshes = &mut self.meshes;
        if let Some(id) = self.pending.next() {
            let bytes = vfs.read(&id.path())?;
            let sector = Sector::parse(id, &bytes)?;
            scene.report.sectors += 1;
            for (slot, object) in sector.objects.iter().enumerate() {
                let Some(object) = object else {
                    continue;
                };
                scene.report.objects += 1;
                if object
                    .flags
                    .contains(ObjectFlags::INVISIBLE | ObjectFlags::EDITOR | ObjectFlags::NON_MAP)
                {
                    scene.report.hidden_objects += 1;
                    continue;
                }
                let Some(ty) = catalog.get(object.class, object.type_id) else {
                    scene.report.unknown_types += 1;
                    scene.report.warnings.push(format!(
                        "{id} slot {slot}: unknown type {:#x}",
                        object.type_id.0
                    ));
                    continue;
                };
                let name = normalize(&format!("imagery/{}", ty.imagery))?;
                let source = DrawSource {
                    name: if object.name.is_empty() {
                        ty.name.clone()
                    } else {
                        object.name.clone()
                    },
                    sector: id,
                    slot,
                    map_id: object.map_id,
                };
                if name.ends_with(".i3d") {
                    // Effect classes require their own runtime controllers.
                    if object.class.0 == 25 {
                        scene.report.effect_objects += 1;
                        continue;
                    }
                    if !meshes.contains_key(&name) {
                        match assets.mesh(vfs, &name) {
                            Ok(mesh) => {
                                meshes.insert(name.clone(), Some(mesh));
                            }
                            Err(error) => {
                                scene.report.warnings.push(format!("{name}: {error:#}"));
                                meshes.insert(name.clone(), None);
                            }
                        }
                    }
                    let Some(mesh) = &meshes[&name] else {
                        continue;
                    };
                    let frame = object.animation.map_or(0, |(frame, _)| frame.max(0) as u32);
                    match MeshDraw::new(
                        Arc::clone(mesh),
                        object.position,
                        object.rotation,
                        object.state,
                        frame,
                    ) {
                        Ok(draw) => {
                            scene.draws.push(Draw::Mesh(draw));
                            scene.sources.push(source);
                            scene.report.mesh_objects += 1;
                        }
                        Err(error) => scene
                            .report
                            .warnings
                            .push(format!("{id} slot {slot}: {error:#}")),
                    }
                    continue;
                }
                if !name.ends_with(".i2d") {
                    scene
                        .report
                        .warnings
                        .push(format!("unsupported imagery {name}"));
                    continue;
                }
                let key = (name.clone(), object.state);
                if !cache.contains_key(&key) {
                    let decoded = assets.tile(vfs, &name, object.state);
                    match decoded {
                        Ok(tile) => {
                            cache.insert(key.clone(), tile);
                        }
                        Err(error) => {
                            scene
                                .report
                                .warnings
                                .push(format!("{} state {}: {error:#}", ty.imagery, object.state));
                            cache.insert(key.clone(), None);
                        }
                    }
                }
                let Some(tile) = cache[&key].as_ref() else {
                    scene.report.empty_states += 1;
                    continue;
                };
                scene.draws.push(Draw::Tile(TileDraw {
                    tile: Arc::clone(tile),
                    position: object.position,
                    flipped: object.flags.contains(ObjectFlags::DRAW_FLIP),
                }));
                scene.sources.push(source);
            }
            scene.sectors.insert(id, sector);
            return Ok(true);
        }
        scene.report.mesh_assets = meshes.values().flatten().count();
        scene.report.tile_assets = cache.values().flatten().count();
        let (walkmap, warnings) = walkmap::WalkMap::load(vfs, catalog, &scene.sectors);
        scene.walkmap = walkmap;
        scene.report.warnings.extend(warnings);
        ensure!(
            !scene.draws.is_empty(),
            "selected sectors contain no renderable imagery; {} decode warnings",
            scene.report.warnings.len()
        );
        self.finished = true;
        Ok(false)
    }
    pub fn finish(self) -> Result<Scene> {
        ensure!(self.finished, "scene loading is incomplete");
        Ok(self.scene)
    }
}

impl Scene {
    pub fn load(vfs: &mut Vfs, catalog: &Catalog, ids: &[SectorId]) -> Result<Self> {
        Self::load_with_assets(vfs, catalog, ids, &mut AssetCache::default())
    }
    pub fn load_with_assets(
        vfs: &mut Vfs,
        catalog: &Catalog,
        ids: &[SectorId],
        assets: &mut AssetCache,
    ) -> Result<Self> {
        let mut loader = SceneLoader::new(ids);
        while loader.step(vfs, catalog, assets)? {}
        loader.finish()
    }

    /// Match native map lighting: cull by projected radius, retain the nearest 16.
    pub fn point_lights(
        &self,
        camera: &crate::render::camera::Camera,
        viewport: [usize; 2],
    ) -> Vec<crate::render::lighting::PointLight> {
        use crate::render::{camera::project, lighting::PointLight};
        use sector::Position;
        let center = [
            camera.center[1] + camera.center[0] * 0.5,
            camera.center[1] - camera.center[0] * 0.5,
        ];
        let mut lights = Vec::new();
        for object in self.sectors.values().flat_map(Sector::iter) {
            let Some(light) = object.light else {
                continue;
            };
            if light.radius == 0 || light.multiplier <= 0 || object.inventory_slot >= 0 {
                continue;
            }
            let p = Position {
                x: object.position.x.saturating_add(light.offset.x),
                y: object.position.y.saturating_add(light.offset.y),
                z: object.position.z.saturating_add(light.offset.z),
            };
            let s = camera.screen(project(p), viewport);
            let r = f64::from(light.radius) * camera.zoom;
            if s[0] + r < 0.0
                || s[1] + r < 0.0
                || s[0] - r >= viewport[0] as f64
                || s[1] - r >= viewport[1] as f64
            {
                continue;
            }
            let distance = (f64::from(p.x) - center[0]).powi(2)
                + (f64::from(p.y) - center[1]).powi(2)
                + f64::from(p.z).powi(2);
            lights.push((
                distance,
                PointLight {
                    position: [p.x as f32, p.y as f32, p.z as f32],
                    radius: f32::from(light.radius),
                    color: light.color.map(|c| f32::from(c) / 255.0),
                    intensity: f32::from(light.multiplier) / 100.0 * 2.5,
                },
            ));
        }
        lights.sort_by(|a, b| a.0.total_cmp(&b.0));
        lights
            .into_iter()
            .take(16)
            .map(|(_, light)| light)
            .collect()
    }

    pub fn bounds(&self, focus: Option<SectorId>) -> Result<[f64; 4]> {
        let mut result: Option<[f64; 4]> = None;
        for (draw, source) in self.draws.iter().zip(&self.sources) {
            if focus.is_some_and(|id| id != source.sector) {
                continue;
            }
            let b = draw.bounds();
            result = Some(match result {
                None => b,
                Some(old) => [
                    old[0].min(b[0]),
                    old[1].min(b[1]),
                    old[2].max(b[2]),
                    old[3].max(b[3]),
                ],
            });
        }
        result.context("no imagery bounds in the selected sector")
    }
}
