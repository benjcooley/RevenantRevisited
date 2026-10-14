// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  gpu-smoke.rs - Native window smoke without assets   *
// *************************************************************************

// Exercise the application's real native event/presentation adapter. Other
// input methods are intentionally unused by this unattended example.
#[allow(dead_code)]
#[path = "../src/desktop.rs"]
mod desktop;

use anyhow::{Result, ensure};
use revenant::{
    assets::imagery::{ImageryState, Tile},
    render::{
        Draw, Framebuffer, TileDraw,
        camera::Camera,
        effects::{Effect, Material, Overlay, Primitive},
        gpu::{Config, Renderer},
        lighting::Lighting,
    },
    world::sector::Position,
};
use std::sync::Arc;

fn main() -> Result<()> {
    let mut window = desktop::DesktopWindow::new("Revenant GPU smoke", 256, 192)?;
    let mut renderer = Renderer::with_surface(
        Config {
            padding: 16,
            force_fallback: std::env::var("REVENANT_TEST_GPU_FALLBACK").is_ok_and(|v| v == "1"),
            ..Default::default()
        },
        window.native(),
    )?;
    let draws = [Draw::Tile(TileDraw {
        position: Position::default(),
        flipped: false,
        tile: Arc::new(Tile {
            width: 32,
            height: 32,
            pixels: vec![0xff804020; 32 * 32],
            depth: vec![0; 32 * 32],
            state: ImageryState {
                name: "smoke".into(),
                registration: [16, 16, 0],
                world_registration: [0; 3],
                world_size: [0; 3],
                walkmap: vec![],
            },
        }),
    })];
    let ui = [Effect {
        material: Material::default(),
        primitive: Primitive::Overlay(Overlay {
            rect: [4.0, 4.0, 16.0, 16.0],
            uv: [0.0, 0.0, 1.0, 1.0],
            color: [0.0, 1.0, 0.0, 1.0],
            clip: None,
        }),
    }];
    let mut frame = Framebuffer::new(256, 192)?;
    let camera = Camera::new(Position::default());
    let lighting = Lighting {
        enabled: false,
        ..Default::default()
    };
    for i in 0..8 {
        if i == 3 {
            let _ = window
                .native()
                .request_inner_size(winit::dpi::PhysicalSize::new(320, 240));
        }
        if i == 0 || i == 7 {
            renderer.request_capture();
        }
        renderer.render_with_effects(&mut frame, &draws, &camera, &lighting, &ui)?;
        window.update(&mut renderer)?;
    }
    ensure!(
        frame.pixels[96 * 256 + 128] == 0x804020,
        "world capture changed"
    );
    ensure!(frame.pick(128, 96) == Some(0), "object picking changed");
    ensure!(
        frame.pixels[10 * 256 + 10] == 0x00ff00,
        "UI composition changed"
    );
    ensure!(
        renderer.stats.presentations > 0,
        "no native frames presented"
    );
    ensure!(
        window.get_size() == (320, 240),
        "native resize was not applied: {:?}",
        window.get_size()
    );
    ensure!(
        renderer.stats.readbacks == 2,
        "ordinary frames triggered readback"
    );
    println!(
        "Native GPU smoke: {} ({:?}), {} presents, {} readbacks; resize, world, UI and picking passed",
        renderer.adapter.name,
        renderer.adapter.backend,
        renderer.stats.presentations,
        renderer.stats.readbacks
    );
    Ok(())
}
