// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               session.rs - Staged loading and live session             *
// *************************************************************************

use super::{Action, Effect, Flow, Screen, Settings};
use crate::{
    assets::{cache::AssetCache, catalog::Catalog, vfs::Vfs},
    render::camera::{Camera, project},
    ui::Hud,
    world::{
        Scene, SceneLoader,
        exploration::{Exploration, Gait},
        movement::TickClock,
        sector::SectorId,
        sector_ids,
    },
};
use anyhow::{Result, ensure};
use std::time::Duration;

/// Owns the transition effects as well as the flow: a cancelled loader cannot
/// later install a world, and a modal screen cannot advance simulation.
pub struct Game {
    flow: Flow,
    start: Start,
    loader: Option<Box<Loader>>,
    session: Option<Box<Session>>,
    assets: AssetCache,
}
impl Game {
    pub fn new(settings: Settings, start: Start) -> Self {
        Self {
            flow: Flow::new(settings),
            start,
            loader: None,
            session: None,
            assets: AssetCache::default(),
        }
    }
    pub fn flow(&self) -> &Flow {
        &self.flow
    }
    pub fn session(&self) -> Option<&Session> {
        self.session.as_deref()
    }
    pub fn session_mut(&mut self) -> Option<&mut Session> {
        self.session.as_deref_mut()
    }
    pub fn loading(&self) -> (u16, &'static str) {
        self.loader
            .as_ref()
            .map_or((0, "Preparing game"), |l| (l.progress(), l.label()))
    }
    fn end_session(&mut self) {
        self.loader = None;
        self.session = None;
        self.assets = AssetCache::default();
    }
    pub fn dispatch(&mut self, action: Action) -> Effect {
        if action == Action::Loaded && self.session.is_none() {
            return Effect::None;
        }
        let before = self.flow.screen();
        let effect = self.flow.dispatch(action);
        match effect {
            Effect::StartSession => {
                self.end_session();
                self.loader = Some(Box::new(Loader::new(self.start)));
            }
            Effect::EndSession | Effect::Quit => self.end_session(),
            _ => {}
        }
        if self.flow.screen() != before {
            if let Some(session) = &mut self.session {
                session.suspend();
                session.hud.cancel_capture();
            }
        }
        effect
    }
    /// A single bounded phase per frame; errors return to the title with a
    /// useful message and release every partially loaded session asset.
    pub fn load_step(&mut self, vfs: &mut Vfs) {
        if let Some(loader) = self.loader.take() {
            match loader.step(vfs, &mut self.assets) {
                Ok(LoadStep::Pending(loader)) => self.loader = Some(loader),
                Ok(LoadStep::Ready(session)) => {
                    self.session = Some(session);
                    self.dispatch(Action::Loaded);
                }
                Err(error) => {
                    let message = format!("{error:#}");
                    eprintln!("Unable to start game: {message}");
                    self.flow.load_failed(message);
                    self.end_session();
                }
            }
        }
    }
    pub fn tick(&mut self, elapsed: Duration, direction: [i8; 2], gait: Gait) -> Result<()> {
        if self.flow.screen() == Screen::Play {
            if let Some(session) = &mut self.session {
                session.tick(elapsed, direction, gait)?;
            }
        }
        Ok(())
    }
}

#[derive(Clone, Copy, Debug)]
pub struct Start {
    pub sector: SectorId,
    pub radius: u8,
    pub spawn: Option<[i32; 2]>,
    pub strict: bool,
}

/// The whole world, player and HUD leave together when a session ends.
pub struct Session {
    pub scene: Scene,
    pub player: Exploration,
    pub hud: Hud,
    pub camera: Camera,
    clock: TickClock,
    ticks: u64,
}
impl Session {
    pub fn tick(&mut self, elapsed: Duration, direction: [i8; 2], gait: Gait) -> Result<()> {
        for _ in 0..self.clock.advance(elapsed) {
            self.player.tick(&mut self.scene, direction, gait)?;
            self.ticks += 1;
        }
        self.camera.center = project(self.player.walker.position());
        Ok(())
    }
    pub fn ticks(&self) -> u64 {
        self.ticks
    }
    /// Dialogs/focus loss discard fractional time, not a pending movement tick.
    pub fn suspend(&mut self) {
        self.clock = TickClock::default();
    }
}

enum Stage {
    Catalog,
    World {
        catalog: Catalog,
        loader: SceneLoader,
    },
    Player(Scene),
    Interface {
        scene: Scene,
        player: Exploration,
    },
}

/// In-progress state is owned and disposable. Cancelling or failing a load
/// cannot expose a partially built scene to simulation or rendering.
pub struct Loader {
    start: Start,
    stage: Stage,
}
pub enum LoadStep {
    Pending(Box<Loader>),
    Ready(Box<Session>),
}
impl Loader {
    pub fn new(start: Start) -> Self {
        Self {
            start,
            stage: Stage::Catalog,
        }
    }
    pub fn progress(&self) -> u16 {
        match &self.stage {
            Stage::Catalog => 0,
            Stage::World { loader, .. } => 100 + loader.progress() * 7 / 10,
            Stage::Player(_) => 800,
            Stage::Interface { .. } => 900,
        }
    }
    pub fn label(&self) -> &'static str {
        match self.stage {
            Stage::Catalog => "Preparing game",
            Stage::World { .. } => "Loading world",
            Stage::Player(_) => "Preparing player",
            Stage::Interface { .. } => "Loading interface",
        }
    }
    pub fn step(self, vfs: &mut Vfs, assets: &mut AssetCache) -> Result<LoadStep> {
        let Self { start, stage } = self;
        let next = match stage {
            Stage::Catalog => {
                ensure!(start.radius <= 3, "session radius must be between 0 and 3");
                let catalog = Catalog::parse(&vfs.read("class.def")?)?;
                let available = sector_ids(vfs)?;
                ensure!(
                    available.contains(&start.sector),
                    "sector {} is unavailable",
                    start.sector
                );
                let radius = i32::from(start.radius);
                let ids: Vec<_> = available
                    .into_iter()
                    .filter(|id| {
                        id.level == start.sector.level
                            && (i64::from(id.x) - i64::from(start.sector.x)).abs()
                                <= i64::from(radius)
                            && (i64::from(id.y) - i64::from(start.sector.y)).abs()
                                <= i64::from(radius)
                    })
                    .collect();
                Stage::World {
                    catalog,
                    loader: SceneLoader::new(&ids),
                }
            }
            Stage::World {
                catalog,
                mut loader,
            } => {
                if loader.step(vfs, &catalog, assets)? {
                    Stage::World { catalog, loader }
                } else {
                    let scene = loader.finish()?;
                    ensure!(
                        !start.strict || scene.report.warnings.is_empty(),
                        "strict mode: {} scene warnings",
                        scene.report.warnings.len()
                    );
                    Stage::Player(scene)
                }
            }
            Stage::Player(mut scene) => {
                let player = Exploration::spawn_with_assets(
                    vfs,
                    &mut scene,
                    start.sector,
                    start.spawn,
                    assets,
                )?;
                Stage::Interface { scene, player }
            }
            Stage::Interface { scene, player } => {
                let hud = Hud::load(vfs)?;
                let camera = Camera::new(player.walker.position());
                return Ok(LoadStep::Ready(Box::new(Session {
                    scene,
                    player,
                    hud,
                    camera,
                    clock: TickClock::default(),
                    ticks: 0,
                })));
            }
        };
        Ok(LoadStep::Pending(Box::new(Self { start, stage: next })))
    }
}
