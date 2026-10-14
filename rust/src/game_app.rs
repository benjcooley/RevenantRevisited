// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               game_app.rs - Desktop gameflow and input                *
// *************************************************************************

use super::{
    Args,
    desktop::{DesktopWindow, Key, MouseButton},
    renderer,
};
use anyhow::{Result, ensure};
use revenant::{
    assets::vfs::Vfs,
    audio::{Audio, UiSound},
    game::{
        Action, Destination, Effect, MenuOrigin, Screen, Settings,
        session::{Game, Start},
    },
    input::{
        Pointer, Target,
        cursor::{CursorKind, CursorSet},
        keyboard_direction, pointer_direction,
    },
    render::{
        Framebuffer,
        camera::{Camera, viewport_cursor},
    },
    ui::{
        Tab,
        batch::Batch,
        screens::{MenuKeys, Screens},
    },
    world::exploration::Gait,
};
use std::{
    path::PathBuf,
    time::{Duration, Instant},
};

fn dispatch(
    game: &mut Game,
    action: Action,
    pointer: &mut Pointer,
    screens: &mut Screens,
    audio: &mut Audio,
) {
    let before = game.flow().screen();
    let effect = game.dispatch(action);
    if matches!(
        effect,
        Effect::StartSession | Effect::EndSession | Effect::Quit
    ) {
        audio.stop_all();
    }
    if before != game.flow().screen() {
        pointer.cancel_capture();
        screens.reset_input();
    }
}

pub fn run(args: &Args, vfs: &mut Vfs) -> Result<()> {
    let mut frame = Framebuffer::new(args.width, args.height)?;
    let viewport = frame.viewport();
    let mut window = DesktopWindow::new(
        "Revenant / Rust — Esc: menu | WASD/right mouse: move",
        args.width,
        args.height,
    )?;
    let mut screens = Screens::load(vfs)?;
    let cursors = match CursorSet::load(vfs) {
        Ok(cursors) => Some(cursors),
        Err(error) => {
            eprintln!("warning: cursors unavailable: {error:#}");
            None
        }
    };
    let mut audio = Audio::default();
    audio.set_volume(args.volume)?;
    audio.set_muted(args.muted);
    if let Err(error) = audio.load_ui(vfs).and_then(|()| audio.open_output()) {
        eprintln!("warning: UI audio unavailable: {error:#}");
    }
    let (mut backend, mut lighting) = renderer(args, Some(window.native().into()))?;
    let settings = Settings {
        volume: (args.volume * 100.0).round() as u8,
        muted: args.muted,
        shadows: !args.no_shadows,
    };
    let mut applied = settings;
    let mut game = Game::new(
        settings,
        Start {
            sector: args.sector,
            radius: args.radius,
            spawn: None,
            strict: args.strict,
        },
    );
    let mut pointer = Pointer::default();
    if args.quickstart {
        game.dispatch(Action::NewGame);
    }
    let mut batch = Batch::default();
    let mut smoke = args.game_smoke.then(Smoke::default);
    let mut previous = Instant::now();
    let mut frames = 0;
    while window.is_open() && game.flow().screen() != Screen::Exit {
        let now = Instant::now();
        let elapsed = if smoke.is_some() {
            Duration::from_secs_f64(1.0 / 24.0)
        } else {
            now.duration_since(previous)
        };
        previous = now;
        let before = game.flow().screen();
        let active = window.is_active();
        let mouse = window.get_mouse_pos().and_then(|(x, y)| {
            let (w, h) = window.get_size();
            viewport_cursor([x, y], [w, h], viewport)
        });
        let over_ui = before != Screen::Play
            || mouse.is_some_and(|p| {
                game.session()
                    .is_some_and(|s| s.hud.layout(viewport).hit_test(p))
            });
        let input = pointer.update(
            mouse,
            [
                window.get_mouse_down(MouseButton::Left),
                window.get_mouse_down(MouseButton::Right),
                window.get_mouse_down(MouseButton::Middle),
            ],
            active,
            over_ui,
        );
        let key = |k| active && window.is_key_pressed(k);
        let action = if before == Screen::Play {
            if key(Key::Escape) || (!active && smoke.is_none()) {
                Some(Action::Pause)
            } else {
                key(Key::F9).then_some(Action::ToggleSound)
            }
        } else {
            let response = screens.input(
                game.flow(),
                viewport,
                input,
                MenuKeys {
                    previous: key(Key::ArrowUp),
                    next: key(Key::ArrowDown),
                    accept: key(Key::Enter),
                    back: key(Key::Escape),
                },
            );
            if response.pressed {
                audio.play_ui(UiSound::Press);
            }
            if response.action.is_some() {
                audio.play_ui(UiSound::Release);
            }
            response.action
        };
        if let Some(action) = action {
            dispatch(&mut game, action, &mut pointer, &mut screens, &mut audio);
        }
        // Present the loading screen before the first phase. Cancellation is
        // handled above, before any further asset work is allowed to run.
        if before == Screen::Loading && game.flow().screen() == Screen::Loading {
            game.load_step(vfs);
            if game.flow().screen() != before {
                pointer.cancel_capture();
                screens.reset_input();
                if let Some(session) = game.session_mut() {
                    if args.no_hud {
                        session.hud.toggle_visible();
                    }
                }
            }
            previous = Instant::now();
        }
        let mut direction = [0; 2];
        let mut gait = Gait::Walk;
        let playing =
            game.flow().simulates() && before == Screen::Play && (active || smoke.is_some());
        if playing {
            let session = game.session_mut().expect("play owns a session");
            if key(Key::F1) {
                session.hud.toggle_visible();
            }
            if key(Key::KeyH) {
                session.hud.toggle_sidebar();
            }
            for (k, tab) in [
                (Key::KeyE, Tab::Equipment),
                (Key::KeyK, Tab::Stats),
                (Key::KeyB, Tab::Book),
                (Key::KeyI, Tab::Inventory),
                (Key::KeyM, Tab::Map),
                (Key::KeyP, Tab::Spells),
            ] {
                if key(k) {
                    session.hud.select(tab);
                    audio.play_ui(UiSound::Release);
                }
            }
            let response =
                session
                    .hud
                    .pointer(input, viewport, &session.scene, Some(session.player.draw));
            if response.pressed {
                audio.play_ui(UiSound::Press);
            }
            if response.activated {
                audio.play_ui(UiSound::Release);
            }
            let held = |k| active && window.is_key_down(k);
            direction = keyboard_direction([
                i8::from(held(Key::KeyD)) - i8::from(held(Key::KeyA)),
                i8::from(held(Key::KeyS)) - i8::from(held(Key::KeyW)),
            ]);
            let world_viewport = session.hud.layout(viewport).world_viewport();
            if direction == [0; 2] && input.held_over(input.right, Target::World) {
                direction = pointer_direction(
                    &session.camera,
                    world_viewport,
                    session.player.walker.position(),
                    input.position.unwrap(),
                    16,
                );
            }
            if held(Key::ShiftLeft) || held(Key::ShiftRight) || held(Key::KeyR) {
                gait = Gait::Run;
            }
            if input.hover == Some(Target::World) {
                if let Some((_, wheel)) = window.get_scroll_wheel() {
                    session.camera.zoom_at(
                        1.12f64.powf(f64::from(wheel)),
                        input.position.unwrap(),
                        world_viewport,
                    );
                }
            }
        }
        if playing || !game.flow().simulates() {
            game.tick(elapsed, direction, gait)?;
        }
        if game.flow().settings() != applied {
            applied = game.flow().settings();
            audio.set_volume(f32::from(applied.volume) / 100.0)?;
            audio.set_muted(applied.muted);
            lighting.shadows = applied.shadows;
        }
        if let Some(error) = audio.take_output_error() {
            eprintln!("warning: audio output: {error}");
        }

        batch.begin();
        let mut camera = Camera::new(Default::default());
        lighting.points.clear();
        if let Some(session) = game.session_mut() {
            session.hud.muted = applied.muted;
            session.hud.record(
                &mut frame,
                &session.scene,
                Some(session.player.draw),
                &mut batch,
                &mut backend,
            )?;
            let layout = session.hud.layout(viewport);
            lighting.points = session
                .scene
                .point_lights(&session.camera, layout.world_viewport());
            camera = layout.world_camera(session.camera, viewport);
        }
        let (progress, label) = game.loading();
        screens.record(game.flow(), viewport, progress, label, &mut batch);
        let capture_key = key(Key::F12);
        window.set_game_cursor(active && mouse.is_some() && cursors.is_some());
        if let (Some(cursors), Some(point)) = (&cursors, input.position) {
            let interactive = if game.flow().simulates() {
                game.session()
                    .is_some_and(|s| s.hud.interactive_at(viewport, point))
            } else {
                screens.interactive_at(game.flow(), viewport, point)
            };
            let kind = if interactive {
                CursorKind::Hand
            } else if playing && direction != [0; 2] {
                CursorKind::Walk(direction)
            } else {
                CursorKind::Default
            };
            let scale = (viewport[0] as f64 / 640.0)
                .min(viewport[1] as f64 / 480.0)
                .min(1.0);
            cursors.record(&mut batch, point, kind, scale);
        }
        let capture = if let Some(smoke) = &mut smoke {
            smoke.capture(game.flow().screen())
        } else if capture_key {
            Some(PathBuf::from(format!("captures/game-{frames}.png")))
        } else {
            None
        };
        if capture.is_some() {
            backend.renderer.request_capture();
        }
        backend.render(
            &mut frame,
            game.session().map_or(&[], |s| &s.scene.draws),
            &camera,
            &lighting,
            args.depth,
            &batch.effects,
        )?;
        if let Some(path) = capture {
            frame.save_png(&path)?;
            println!("Saved {}", path.display());
        }
        if let Some(smoke) = &mut smoke {
            if let Some(action) = smoke.after_frame(&game)? {
                dispatch(&mut game, action, &mut pointer, &mut screens, &mut audio);
                previous = Instant::now();
            }
        }
        window.update(&mut backend.renderer)?;
        frames += 1;
        if args.frames.is_some_and(|limit| frames >= limit) {
            break;
        }
    }
    if let Some(smoke) = smoke {
        ensure!(
            smoke.step == SMOKE.len(),
            "game smoke interrupted at step {}",
            smoke.step
        );
        println!(
            "Gameflow smoke passed: title/options, load cancellation, play/pause/resume, confirmations, session restart and exit ({frames} frames)."
        );
    }
    Ok(())
}

// Drives the same transitions as desktop input, while the normal GPU, window,
// loader and clock paths remain active. Retail-data coverage is opt-in.
const SMOKE: &[(Screen, Action)] = &[
    (Screen::Title, Action::Options),
    (Screen::Options(MenuOrigin::Title), Action::VolumeDown),
    (Screen::Options(MenuOrigin::Title), Action::Back),
    (Screen::Title, Action::NewGame),
    (Screen::Loading, Action::Back),
    (Screen::Title, Action::NewGame),
    (Screen::Play, Action::Pause),
    (Screen::Pause, Action::Options),
    (Screen::Options(MenuOrigin::Pause), Action::ToggleShadows),
    (Screen::Options(MenuOrigin::Pause), Action::ApplyOptions),
    (Screen::Pause, Action::Resume),
    (Screen::Play, Action::Pause),
    (Screen::Pause, Action::ReturnToTitle),
    (Screen::Confirm(Destination::Title), Action::Back),
    (Screen::Pause, Action::ReturnToTitle),
    (Screen::Confirm(Destination::Title), Action::Confirm),
    (Screen::Title, Action::NewGame),
    (Screen::Play, Action::Pause),
    (Screen::Pause, Action::Quit),
    (Screen::Confirm(Destination::Exit), Action::Confirm),
];
#[derive(Default)]
struct Smoke {
    step: usize,
    frames: u32,
    waiting: u32,
    ticks: u64,
    captured: Vec<Screen>,
}
impl Smoke {
    fn capture(&mut self, screen: Screen) -> Option<PathBuf> {
        if self.captured.contains(&screen) {
            return None;
        }
        self.captured.push(screen);
        Some(PathBuf::from(format!(
            "captures/game-{:02}.png",
            self.captured.len()
        )))
    }
    fn after_frame(&mut self, game: &Game) -> Result<Option<Action>> {
        ensure!(
            game.flow().error().is_none(),
            "game smoke load failed: {:?}",
            game.flow().error()
        );
        self.waiting += 1;
        ensure!(
            self.waiting < 600,
            "game smoke timed out at step {}",
            self.step
        );
        let (screen, action) = SMOKE[self.step];
        if game.flow().screen() != screen {
            return Ok(None);
        }
        self.frames += 1;
        let ticks = game.session().map_or(0, |s| s.ticks());
        if self.frames == 1 {
            self.ticks = ticks;
        }
        if matches!(
            screen,
            Screen::Pause | Screen::Options(MenuOrigin::Pause) | Screen::Confirm(_)
        ) {
            ensure!(
                ticks == self.ticks,
                "simulation advanced inside a modal screen"
            );
        }
        let dwell = if screen == Screen::Play { 20 } else { 3 };
        if self.frames < dwell {
            return Ok(None);
        }
        if screen == Screen::Play {
            ensure!(ticks > 0, "play never advanced simulation");
        }
        self.step += 1;
        self.frames = 0;
        self.waiting = 0;
        Ok(Some(action))
    }
}
