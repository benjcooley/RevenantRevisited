// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   hud.rs - Retail interface validation                *
// *************************************************************************

use anyhow::Result;
use revenant::{
    assets::{catalog::Catalog, mesh::Mesh, vfs::Vfs},
    input::Pointer,
    render::{Draw, Framebuffer, camera::Camera, equipment::Slot},
    ui::{Hud, Tab},
    world::{Scene, exploration::Exploration, sector::SectorId},
};
use std::sync::Arc;

#[test]
#[ignore = "requires retail assets; writes captures/hud-*.png"]
fn retail_panes_equipment_scaling_and_pointer_capture() -> Result<()> {
    let mut vfs = Vfs::open(Vfs::discover(None)?, "Ahkuilon", None)?;
    let catalog = Catalog::parse(&vfs.read("class.def")?)?;
    let sector: SectorId = "0_2_25".parse()?;
    let mut scene = Scene::load(&mut vfs, &catalog, &[sector])?;
    let actor = Exploration::spawn(&mut vfs, &mut scene, sector, None)?;
    if let Draw::Mesh(draw) = &mut scene.draws[actor.draw] {
        for (slot, path) in [
            (Slot::Body, "dsblackchest"),
            (Slot::PrimeHand, "ShortSword"),
        ] {
            let mesh = Arc::new(Mesh::decode(
                &vfs.read(&format!("imagery/equip/{path}.i3d"))?,
            )?);
            draw.equip(slot, mesh, "normal")?;
        }
    }
    let camera = Camera::new(actor.walker.position());
    let mut hud = Hud::load(&mut vfs)?;
    let mut renderer = revenant::render::backend::Backend::new(Default::default())?;
    let mut batch = revenant::ui::batch::Batch::default();
    for [width, height] in [[640, 480], [1024, 768], [320, 240]] {
        let mut frame = Framebuffer::new(width, height)?;
        for (top, bottom, name) in [
            (Tab::Equipment, Tab::Inventory, "equipment"),
            (Tab::Stats, Tab::Map, "map"),
            (Tab::Book, Tab::Spells, "spells"),
        ] {
            hud.select(top);
            hud.select(bottom);
            let camera = hud
                .layout(frame.viewport())
                .world_camera(camera, frame.viewport());
            renderer.render(
                &mut frame,
                &scene.draws,
                &camera,
                &Default::default(),
                false,
                &[],
            )?;
            let before = frame.pixels.clone();
            batch.begin();
            batch.overlay(
                None,
                revenant::render::effects::Overlay {
                    rect: [0.0, 0.0, frame.width as f32, frame.height as f32],
                    uv: [0.0, 0.0, 1.0, 1.0],
                    color: [0.0, 0.0, 0.0, 1.0],
                    clip: None,
                },
            );
            hud.record(
                &mut frame,
                &scene,
                Some(actor.draw),
                &mut batch,
                &mut renderer,
            )?;
            renderer.render(
                &mut frame,
                &scene.draws,
                &camera,
                &Default::default(),
                false,
                &batch.effects,
            )?;
            assert_ne!(before, frame.pixels);
            frame.save_png(format!("captures/hud-{name}-{width}.png").as_ref())?;
            if name == "equipment" && width == 640 {
                renderer.render(
                    &mut frame,
                    &[],
                    &camera,
                    &Default::default(),
                    false,
                    &batch.effects,
                )?;
                frame.save_png("captures/hud-chrome-640.png".as_ref())?;
            }
        }
    }
    let viewport = [640, 480];
    hud.select(Tab::Equipment);
    let tab = [410.0, 200.0]; // first tab, book
    let world = [200.0, 200.0];
    let other_tab = [410.0, 240.0];
    let mut pointer = Pointer::default();
    let mut event = |hud: &mut Hud, p, down, active| {
        let input = pointer.update(
            Some(p),
            [down, false, false],
            active,
            hud.layout(viewport).hit_test(p),
        );
        hud.pointer(input, viewport, &scene, Some(actor.draw))
    };
    assert!(event(&mut hud, tab, true, true).pressed);
    assert!(!event(&mut hud, world, false, true).activated);
    assert!(!event(&mut hud, world, true, true).pressed);
    assert!(!event(&mut hud, tab, false, true).activated);
    assert!(event(&mut hud, tab, true, true).pressed);
    assert!(!event(&mut hud, other_tab, false, true).activated);
    assert!(event(&mut hud, tab, true, true).pressed);
    assert!(!event(&mut hud, tab, false, false).activated);
    assert!(!event(&mut hud, tab, false, true).activated);
    assert!(event(&mut hud, tab, true, true).pressed);
    assert!(event(&mut hud, tab, false, true).activated);
    hud.select(Tab::Equipment);
    let body_slot = [610.0, 80.0];
    assert!(event(&mut hud, body_slot, true, true).pressed);
    assert!(event(&mut hud, body_slot, false, true).activated);
    hud.toggle_sidebar();
    assert_eq!(hud.layout(viewport).world_viewport(), [640, 420]);
    hud.toggle_visible();
    assert_eq!(hud.layout(viewport).world_viewport(), viewport);
    assert!(!hud.layout(viewport).hit_test(body_slot));
    println!(
        "Verified 6 pane types at 3 resolutions, gear preview, capture cancellation and slot input"
    );
    Ok(())
}
