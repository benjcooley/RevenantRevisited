// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             legacy_mesh.rs - Original I3D compatibility tests          *
// *************************************************************************

//! Opt-in checks for original fixed-array bodies, including uncatalogued assets.
use anyhow::{Context, Result, ensure};
use revenant::{
    assets::{mesh::Mesh, vfs::Vfs},
    render::mesh::MeshDraw,
    world::sector::Position,
};
use std::sync::Arc;

#[test]
#[ignore = "requires a local Revenant install"]
fn all_old_bodies_load_and_form_standalone_poses() -> Result<()> {
    let mut vfs = Vfs::open(Vfs::discover(None)?, "Ahkuilon", None)?;
    let mut assets = 0;
    let mut clips = 0;
    for path in vfs
        .list("imagery")?
        .into_iter()
        .filter(|p| p.ends_with(".i3d"))
    {
        let bytes = vfs.read(&path)?;
        let u32_at = |at| -> Result<u32> {
            Ok(u32::from_le_bytes(
                bytes.get(at..at + 4).context("I3D word")?.try_into()?,
            ))
        };
        let body = 20
            + u32_at(16)? as usize
            + usize::from(u16::from_le_bytes(bytes[4..6].try_into()?)) * 4;
        if u32_at(body)? & 16 != 0 {
            continue;
        }
        let mesh = Arc::new(Mesh::decode(&bytes).with_context(|| path.clone())?);
        for (state, clip) in mesh.clips.iter().enumerate() {
            for frame in [0, clip.frames / 2, clip.frames - 1] {
                let draw = MeshDraw::new(
                    mesh.clone(),
                    Position { x: 0, y: 0, z: 0 },
                    [0; 3],
                    state as u16,
                    frame,
                )
                .with_context(|| format!("{path} state {state}"))?;
                for (index, part) in mesh.parts.iter().enumerate() {
                    // Check the transformed geometry even for non-visible zero-length states.
                    for vertex in &mesh.vertices[part.vertices.clone()] {
                        ensure!(
                            draw.part_transform(index)
                                .unwrap()
                                .point(vertex.position)
                                .iter()
                                .all(|v| v.is_finite()),
                            "{path}: non-finite pose"
                        );
                    }
                }
            }
            clips += 1;
        }
        assets += 1;
    }
    ensure!(assets > 0, "no original I3D bodies found");
    println!("Checked {assets} old I3D bodies / {clips} clips at first, middle and last frames");
    Ok(())
}

#[test]
#[ignore = "requires a local Revenant install"]
fn irisdoor_moves_only_its_disc_and_barriers_keep_root_parents() -> Result<()> {
    let mut vfs = Vfs::open(Vfs::discover(None)?, "Ahkuilon", None)?;
    let mesh = Mesh::decode(&vfs.read("imagery/misc/irisdoor.i3d")?)?;
    let closing = &mesh.clips[2];
    assert_eq!(closing.name, "closing");
    assert_eq!(closing.frames, 29);
    // Native SAniKey position units are quarters. These on-disk values also
    // distinguish object-major tracks from frame-interleaved or shared keys.
    for (frame, z) in [(0, -21.0 / 4.0), (14, -92.0 / 4.0), (28, -139.0 / 4.0)] {
        assert_eq!(
            closing.bones[0]
                .track
                .as_ref()
                .unwrap()
                .sample(frame)
                .position,
            [0.0, 0.0, z]
        );
        assert_eq!(
            closing.bones[1]
                .track
                .as_ref()
                .unwrap()
                .sample(frame)
                .position,
            [0.0, 0.0, 19.0 / 4.0]
        );
    }
    let mesh = Mesh::decode(&vfs.read("imagery/misc/barrierew.i3d")?)?;
    assert_eq!(mesh.clips[0].bones.len(), 5);
    assert!(mesh.clips[0].bones.iter().all(|bone| bone.parent.is_none()));
    Ok(())
}
