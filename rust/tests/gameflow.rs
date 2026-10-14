// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *            gameflow.rs - Retail screens and session lifecycle          *
// *************************************************************************

use anyhow::Result;
use revenant::{
    assets::{cache::AssetCache, catalog::Catalog, vfs::Vfs},
    game::{
        Action, Flow, Screen, Settings,
        session::{Game, Start},
    },
    input::{Pointer, PointerFrame},
    render::{Draw, Framebuffer, backend::Backend, camera::Camera},
    ui::{
        Tab,
        batch::Batch,
        screens::{MenuKeys, Screens},
    },
    world::{Scene, SceneLoader, exploration::Gait},
};
use std::{sync::Arc, time::Duration};

fn start() -> Start {
    Start {
        sector: "0_2_25".parse().unwrap(),
        radius: 0,
        spawn: None,
        strict: false,
    }
}
fn load(game: &mut Game, vfs: &mut Vfs) {
    game.dispatch(Action::NewGame);
    let mut progress = 0;
    for _ in 0..100 {
        assert!(game.loading().0 >= progress);
        progress = game.loading().0;
        game.load_step(vfs);
        if game.flow().screen() != Screen::Loading {
            break;
        }
        assert!(
            game.session().is_none(),
            "partially loaded sessions must stay private"
        );
    }
    assert_eq!(
        game.flow().screen(),
        Screen::Play,
        "{:?}",
        game.flow().error()
    );
}

#[test]
#[ignore = "requires retail assets"]
fn cancel_pause_restart_release_and_load_failure() -> Result<()> {
    let mut vfs = Vfs::open(Vfs::discover(None)?, "Ahkuilon", None)?;
    let mut game = Game::new(Settings::default(), start());
    game.dispatch(Action::NewGame);
    game.load_step(&mut vfs); // catalog
    game.load_step(&mut vfs); // sector, with cached assets
    game.dispatch(Action::Back);
    game.load_step(&mut vfs);
    game.dispatch(Action::Loaded); // stale completion
    assert_eq!(game.flow().screen(), Screen::Title);
    assert!(game.session().is_none());

    load(&mut game, &mut vfs);
    let initial = game.session().unwrap().player.walker.position();
    let mesh = match &game.session().unwrap().scene.draws[game.session().unwrap().player.draw] {
        Draw::Mesh(draw) => Arc::downgrade(&draw.mesh),
        _ => panic!("expected player mesh"),
    };
    game.tick(Duration::from_secs_f64(1.0 / 48.0), [0; 2], Gait::Walk)?;
    assert_eq!(game.session().unwrap().ticks(), 0);
    game.dispatch(Action::Pause);
    for action in [
        Action::Options,
        Action::VolumeDown,
        Action::Back,
        Action::ReturnToTitle,
        Action::Back,
    ] {
        game.dispatch(action);
        game.tick(Duration::from_secs(120), [1, 0], Gait::Run)?;
        assert_eq!(game.session().unwrap().ticks(), 0);
        assert_eq!(game.session().unwrap().player.walker.position(), initial);
    }
    game.dispatch(Action::Resume);
    game.tick(Duration::from_secs_f64(1.0 / 48.0), [0; 2], Gait::Walk)?;
    assert_eq!(
        game.session().unwrap().ticks(),
        0,
        "pause discards fractional ticks"
    );
    game.tick(Duration::from_millis(250), [1, 0], Gait::Walk)?;
    assert!(game.session().unwrap().ticks() > 0);
    game.session_mut().unwrap().hud.select(Tab::Book);
    game.dispatch(Action::Pause);
    game.dispatch(Action::ReturnToTitle);
    game.dispatch(Action::Confirm);
    assert!(game.session().is_none());
    assert!(
        mesh.upgrade().is_none(),
        "ending a session must release source assets and cache leases"
    );
    load(&mut game, &mut vfs);
    assert_eq!(game.session().unwrap().ticks(), 0);
    assert_eq!(game.session().unwrap().player.walker.position(), initial);
    game.dispatch(Action::PlayerDied);
    assert!(game.session().is_none());
    load(&mut game, &mut vfs);
    game.dispatch(Action::Pause);
    game.dispatch(Action::Quit);
    game.dispatch(Action::Confirm);
    assert_eq!(game.flow().screen(), Screen::Exit);
    assert!(game.session().is_none());

    let mut bad_start = start();
    bad_start.sector.x = 9999;
    let mut failed = Game::new(Settings::default(), bad_start);
    failed.dispatch(Action::NewGame);
    failed.load_step(&mut vfs);
    assert_eq!(failed.flow().screen(), Screen::Title);
    assert!(failed.flow().error().unwrap().contains("unavailable"));
    assert!(failed.session().is_none());
    Ok(())
}

#[test]
#[ignore = "requires retail assets"]
fn incremental_scene_matches_synchronous_assembly() -> Result<()> {
    let mut vfs = Vfs::open(Vfs::discover(None)?, "Ahkuilon", None)?;
    let catalog = Catalog::parse(&vfs.read("class.def")?)?;
    let ids = [start().sector, "0_3_25".parse()?];
    let mut loader = SceneLoader::new(&ids);
    let mut assets = AssetCache::default();
    assert_eq!(loader.progress(), 0);
    for expected in 1..=ids.len() {
        assert!(loader.step(&mut vfs, &catalog, &mut assets)?);
        assert_eq!(
            usize::from(loader.progress()),
            expected * 1000 / (ids.len() + 1)
        );
    }
    assert!(!loader.step(&mut vfs, &catalog, &mut assets)?);
    assert_eq!(loader.progress(), 1000);
    let staged = loader.finish()?;
    let bulk = Scene::load(&mut vfs, &catalog, &ids)?;
    assert_eq!(staged.draws.len(), bulk.draws.len());
    for (a, b) in staged.draws.iter().zip(&bulk.draws) {
        assert_eq!(a.position(), b.position());
    }
    for (a, b) in staged.sources.iter().zip(&bulk.sources) {
        assert_eq!(
            (&a.name, a.sector, a.slot, a.map_id),
            (&b.name, b.sector, b.slot, b.map_id)
        );
    }
    for id in ids {
        assert_eq!(staged.walkmap.cells(id), bulk.walkmap.cells(id));
    }
    assert_eq!(staged.report.warnings, bulk.report.warnings);
    Ok(())
}

#[test]
#[ignore = "requires retail assets and GPU; writes captures/screens-*.png"]
fn native_screen_art_scaling_and_mouse_keyboard_capture() -> Result<()> {
    let mut vfs = Vfs::open(Vfs::discover(None)?, "Ahkuilon", None)?;
    let mut screens = Screens::load(&mut vfs)?;
    let mut renderer = Backend::new(Default::default())?;
    let mut batch = Batch::default();
    for [w, h] in [[640, 480], [1024, 768], [320, 240]] {
        let viewport = [w, h];
        let mut flow = Flow::new(Settings::default());
        let mut pointer = Pointer::default();
        screens.reset_input();
        let new = screens
            .control_center(&flow, viewport, Action::NewGame)
            .unwrap();
        let options = screens
            .control_center(&flow, viewport, Action::Options)
            .unwrap();
        assert!(screens.interactive_at(&flow, viewport, new));
        let mut event = |screens: &mut Screens, point, down, active| {
            screens.input(
                &flow,
                viewport,
                pointer.update(Some(point), [down, false, false], active, true),
                MenuKeys::default(),
            )
        };
        assert!(event(&mut screens, new, true, true).pressed);
        assert!(event(&mut screens, options, false, true).action.is_none());
        assert!(event(&mut screens, new, true, true).pressed);
        assert!(event(&mut screens, new, false, false).action.is_none());
        assert!(event(&mut screens, new, true, true).pressed);
        assert_eq!(
            event(&mut screens, new, false, true).action,
            Some(Action::NewGame)
        );
        screens.reset_input();
        let keyboard = screens.input(
            &flow,
            viewport,
            PointerFrame::default(),
            MenuKeys {
                next: true,
                accept: true,
                ..Default::default()
            },
        );
        assert_eq!(
            keyboard.action,
            Some(Action::Options),
            "disabled Load Game must be skipped"
        );

        let mut frame = Framebuffer::new(w, h)?;
        for (name, actions) in [
            ("title", vec![]),
            ("options", vec![Action::Options]),
            ("loading", vec![Action::Back, Action::NewGame]),
            ("pause", vec![Action::Loaded, Action::Pause]),
            ("confirm", vec![Action::ReturnToTitle]),
            (
                "death",
                vec![Action::Back, Action::Resume, Action::PlayerDied],
            ),
        ] {
            for action in actions {
                flow.dispatch(action);
            }
            screens.input(
                &flow,
                viewport,
                PointerFrame::default(),
                MenuKeys::default(),
            );
            batch.begin();
            screens.record(&flow, viewport, 600, "Loading world", &mut batch);
            renderer.render(
                &mut frame,
                &[],
                &Camera::new(Default::default()),
                &Default::default(),
                false,
                &batch.effects,
            )?;
            assert!(frame.pixels.iter().any(|p| p & 0x00ffffff != 0));
            frame.save_png(format!("captures/screens-{name}-{w}.png").as_ref())?;
        }
    }
    Ok(())
}
