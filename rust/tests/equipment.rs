// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               equipment.rs - Retail attachment compatibility          *
// *************************************************************************

use anyhow::{Result, ensure};
use revenant::{
    assets::{mesh::Mesh, vfs::Vfs},
    render::{Draw, Framebuffer, camera::Camera, equipment::Slot, mesh::MeshDraw},
    world::sector::Position,
};
use std::sync::Arc;

#[test]
#[ignore = "requires a local Revenant install"]
fn retail_gear_uses_locke_parts_across_all_character_clips() -> Result<()> {
    let mut vfs = Vfs::open(Vfs::discover(None)?, "Ahkuilon", None)?;
    let mesh = Arc::new(Mesh::decode(&vfs.read("imagery/chars/locke.i3d")?)?);
    let mut draw = MeshDraw::new(
        mesh.clone(),
        Position {
            x: 73,
            y: -29,
            z: 80,
        },
        [0, 0, 64],
        0,
        0,
    )?;
    for (path, slot) in [
        ("dsblackchest", Slot::Body),
        ("dsblackhand", Slot::OffHand),
        ("nakboot", Slot::Feet),
        ("nakchest", Slot::Body),
        ("nakhand", Slot::OffHand),
        ("ShortSword", Slot::PrimeHand),
    ] {
        let gear = Arc::new(Mesh::decode(
            &vfs.read(&format!("imagery/equip/{path}.i3d"))?,
        )?);
        if path != "ShortSword" {
            // These meshes need an attachment pose, not a standalone rig.
            ensure!(
                gear.clips[0].standalone_order().is_err(),
                "expected stored equipment parent links"
            );
        }
        let count = draw.equip(slot, gear, "normal")?;
        if path == "dsblackchest" || path == "nakchest" {
            assert_eq!(count, 5);
        }
        if path == "ShortSword" {
            assert_eq!(count, 1);
        }
        for binding in draw.equipment(slot).unwrap().bindings() {
            let name = &draw.equipment(slot).unwrap().mesh().parts[binding.equipment_part].name;
            ensure!(name != "ground", "ground geometry must not be attached");
            if path != "ShortSword" {
                ensure!(name.starts_with("*normal"), "wrong body variant {name}");
            }
        }
        for (state, clip) in mesh.clips.iter().enumerate() {
            for frame in [0, clip.frames / 2, clip.frames - 1] {
                draw.set_pose(draw.position, [0, 0, 64], state as u16, frame)?;
                ensure!(
                    draw.bounds().iter().all(|v| v.is_finite()),
                    "{path}: invalid bounds"
                );
                for binding in draw.equipment(slot).unwrap().bindings() {
                    assert!(!draw.part_visible(binding.character_part));
                }
            }
        }
        println!(
            "{path}: {count} attached parts across {} clips",
            mesh.clips.len()
        );
        draw.unequip(slot);
    }
    // A complete visible character retains one depth/pick owner, with armor
    // replacing its chest and a sword riding the original sword bone.
    for (path, slot) in [
        ("dsblackchest", Slot::Body),
        ("ShortSword", Slot::PrimeHand),
    ] {
        draw.equip(
            slot,
            Arc::new(Mesh::decode(
                &vfs.read(&format!("imagery/equip/{path}.i3d"))?,
            )?),
            "normal",
        )?;
    }
    draw.set_pose(draw.position, [0; 3], 265, 6)?;
    let mut camera = Camera::new(draw.position);
    camera.fit(draw.bounds(), [320, 320]);
    let mut framebuffer = Framebuffer::new(320, 320)?;
    assert!(
        revenant::render::gpu::Renderer::new(Default::default())?
            .render(
                &mut framebuffer,
                &[Draw::Mesh(draw)],
                &camera,
                &revenant::render::lighting::Lighting {
                    enabled: false,
                    ..Default::default()
                }
            )?
            .covered_pixels
            > 0
    );
    Ok(())
}
