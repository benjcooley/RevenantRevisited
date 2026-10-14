// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              retail.rs - Retail asset compatibility tests             *
// *************************************************************************

//! Opt-in compatibility tests. No original game content is checked into Rust.
use anyhow::{Context, Result, ensure};
use revenant::{
    assets::{
        catalog::{Catalog, ClassId},
        imagery::decode_tile,
        vfs::Vfs,
    },
    world::{Scene, sector::Sector, sector_ids},
};
use std::{fs, path::PathBuf};

fn data() -> Result<Vfs> {
    Vfs::open(Vfs::discover(None)?, "Ahkuilon", None)
}

#[test]
#[ignore = "requires a local Revenant install; set REVENANT_DATA_PATH if needed"]
fn campaign_sectors_and_default_scene_load() -> Result<()> {
    let mut vfs = data()?;
    let catalog = Catalog::parse(&vfs.read("class.def")?)?;
    let ids = sector_ids(&vfs)?;
    ensure!(!ids.is_empty(), "no campaign sectors found");
    for &id in &ids {
        Sector::parse(id, &vfs.read(&id.path())?)?;
    }
    let neighborhood: Vec<_> = ids
        .into_iter()
        .filter(|id| id.level == 0 && (1..=3).contains(&id.x) && (24..=26).contains(&id.y))
        .collect();
    let scene = Scene::load(&mut vfs, &catalog, &neighborhood)?;
    ensure!(
        scene.report.warnings.is_empty(),
        "scene warnings: {:?}",
        scene.report.warnings
    );
    ensure!(!scene.draws.is_empty(), "no tile draws");
    ensure!(scene.report.mesh_objects > 0, "no mesh draws");
    ensure!(scene.walkmap.covered_cells() > 0, "no walk surfaces");
    Ok(())
}

#[test]
#[ignore = "requires a local Revenant install"]
fn locke_walk_preview_moves_and_stops_at_misthaven_terrain() -> Result<()> {
    use revenant::world::{
        exploration::{Exploration, Gait},
        sector::SectorId,
    };
    let mut vfs = data()?;
    let catalog = Catalog::parse(&vfs.read("class.def")?)?;
    let ids: Vec<_> = sector_ids(&vfs)?
        .into_iter()
        .filter(|id| id.level == 0 && (1..=3).contains(&id.x) && (24..=26).contains(&id.y))
        .collect();
    let mut scene = Scene::load(&mut vfs, &catalog, &ids)?;
    let sector: SectorId = "0_2_25".parse()?;
    let mut preview = Exploration::spawn(&mut vfs, &mut scene, sector, None)?;
    let start = preview.walker.position();
    let mut moved = 0;
    let mut blocked = 0;
    for _ in 0..48 {
        let step = preview.tick(&mut scene, [1, 0], Gait::Walk)?;
        moved += usize::from(step.moved);
        blocked += usize::from(step.blocked);
        let p = preview.walker.position();
        ensure!(
            scene.walkmap.probe(0, p, Exploration::RADIUS).permits(p.z),
            "left safe ground"
        );
        assert_eq!(scene.draws[preview.draw].position(), p);
    }
    ensure!(
        moved > 0 && blocked > 0,
        "expected movement followed by a terrain obstacle"
    );
    ensure!(preview.walker.position().x > start.x, "did not move east");
    let before = preview.walker.position();
    assert!(!preview.tick(&mut scene, [0, 0], Gait::Walk)?.moved);
    assert_eq!(preview.walker.position(), before);
    Ok(())
}

#[test]
#[ignore = "requires a local Revenant install"]
fn equipment_data_loads_without_a_standalone_skeleton() -> Result<()> {
    use revenant::assets::mesh::Mesh;
    let mut vfs = data()?;
    for name in [
        "dsblackchest",
        "dsblackhand",
        "nakboot",
        "nakchest",
        "nakhand",
    ] {
        let mesh = Mesh::decode(&vfs.read(&format!("imagery/Equip/{name}.i3d"))?)?;
        ensure!(
            mesh.parts.iter().any(|p| p.name == "ground"),
            "missing ground part in {name}"
        );
        // ProcessEquipment supplies the character's matrices, so decoding the
        // geometry must not depend on traversing these stored links as a rig.
        ensure!(!mesh.vertices.is_empty(), "missing equipment geometry");
    }
    Ok(())
}

#[test]
#[ignore = "requires REVENANT_CPP_MESH_DUMP pointing to the C++ i3d_dump_all directory"]
fn mesh_geometry_and_textures_match_cpp_exports() -> Result<()> {
    use revenant::assets::mesh::Mesh;
    let directory = PathBuf::from(
        std::env::var_os("REVENANT_CPP_MESH_DUMP").context("set REVENANT_CPP_MESH_DUMP")?,
    );
    let mut vfs = data()?;
    for (path, name) in [
        ("Chars/jason.i3d", "jason"),
        ("Chars/locke.i3d", "locke"),
        ("Equip/ShortSword.i3d", "shortsword"),
        ("Misc/box.i3d", "box"),
        ("Misc/axis.i3d", "axis"),
        ("Misc/irisdoor.i3d", "irisdoor"),
        ("Misc/barrierew.i3d", "barrierew"),
        ("Magic/flame.i3d", "flame"),
        ("Magic/vortex.i3d", "vortex"),
        ("Magic/fireflash.i3d", "fireflash"),
    ] {
        let mesh = Mesh::decode(&vfs.read(&format!("imagery/{path}"))?)?;
        let obj = fs::read_to_string(directory.join(name).join(format!("{name}.obj")))?;
        let reference: Vec<[f64; 3]> = obj
            .lines()
            .filter_map(|s| s.strip_prefix("v "))
            .map(|s| {
                let values: Vec<_> = s
                    .split_whitespace()
                    .map(str::parse)
                    .collect::<std::result::Result<_, _>>()?;
                values
                    .try_into()
                    .map_err(|_| anyhow::anyhow!("invalid OBJ position"))
            })
            .collect::<Result<_>>()?;
        // These reference assets have one nonempty texture batch per part,
        // matching the exporter's first-nonempty-slot OBJ selection.
        let mut cursor = 0;
        for part in &mesh.parts {
            for _batch in &part.batches {
                for vertex in &mesh.vertices[part.vertices.clone()] {
                    for (a, b) in vertex.position.into_iter().zip(reference[cursor]) {
                        ensure!(
                            (a - b).abs() <= 0.0001 * a.abs().max(1.0),
                            "{name}: position mismatch at {cursor}"
                        );
                    }
                    cursor += 1;
                }
            }
        }
        ensure!(cursor == reference.len(), "{name}: vertex count mismatch");
        for (texture, frames) in mesh.textures.iter().enumerate() {
            for (frame, pixels) in frames.iter().enumerate() {
                let path = directory
                    .join(name)
                    .join(format!("texture_{texture:02}_frame_{frame:02}.png"));
                let decoder = png::Decoder::new(std::io::BufReader::new(fs::File::open(path)?));
                let mut reader = decoder.read_info()?;
                let mut bytes = vec![0; reader.output_buffer_size().context("PNG size")?];
                let info = reader.next_frame(&mut bytes)?;
                ensure!(
                    info.color_type == png::ColorType::Rgba,
                    "expected RGBA C++ export"
                );
                assert_eq!(
                    (info.width as usize, info.height as usize),
                    (pixels.width, pixels.height)
                );
                // testmodes.cpp flips I3D PNGs vertically for OBJ consumers.
                // The engine's upload and our texture buffers retain native row order.
                for (index, &color) in pixels.pixels.iter().enumerate() {
                    let y = pixels.height - 1 - index / pixels.width;
                    let at = (y * pixels.width + index % pixels.width) * 4;
                    let rgba = &bytes[at..at + 4];
                    assert_eq!(
                        [
                            (color >> 16) as u8,
                            (color >> 8) as u8,
                            color as u8,
                            (color >> 24) as u8
                        ],
                        rgba,
                        "{name} texture {texture} frame {frame} pixel {index}"
                    );
                }
            }
        }
    }
    Ok(())
}

#[test]
#[ignore = "requires REVENANT_CPP_TILE_DUMP pointing to the C++ .ztile export directory"]
fn tile_decode_matches_cpp_rgba_and_signed_depth() -> Result<()> {
    let directory = PathBuf::from(
        std::env::var_os("REVENANT_CPP_TILE_DUMP").context("set REVENANT_CPP_TILE_DUMP")?,
    );
    let mut vfs = data()?;
    let catalog = Catalog::parse(&vfs.read("class.def")?)?;
    // Floors, walls, grates, and columns exercise registration, alpha holes,
    // palette data, chunk seams, and both positive and negative authored depth.
    for name in ["TwnBp1", "KepWallWin_1Ea", "KInGrateWallN", "KepColBtmSb"] {
        let ty = catalog
            .iter()
            .find(|t| t.class == ClassId::TILE && t.name.eq_ignore_ascii_case(name))
            .with_context(|| format!("missing type {name}"))?;
        let bytes = vfs.read(&format!("imagery/{}", ty.imagery))?;
        let tile = decode_tile(&bytes, 0)?.context("missing still image")?;
        let exported = fs::read(directory.join(format!("{:04}_{}.ztile", ty.ordinal, ty.name)))?;
        ensure!(
            exported.len() >= 12 && &exported[..4] == b"RTZE",
            "invalid C++ dump header"
        );
        let width = i32::from_le_bytes(exported[4..8].try_into()?);
        let height = i32::from_le_bytes(exported[8..12].try_into()?);
        assert_eq!(
            (tile.width as i32, tile.height as i32),
            (width, height),
            "{name}"
        );
        let count = tile.width * tile.height;
        ensure!(exported.len() == 12 + count * 8, "invalid C++ dump size");
        for i in 0..count {
            let rgba = &exported[12 + i * 4..16 + i * 4];
            let color = tile.pixels[i];
            assert_eq!(color >> 24, u32::from(rgba[3]), "{name}: alpha at {i}");
            if rgba[3] == 0 {
                continue;
            }
            assert_eq!(
                [(color >> 16) as u8, (color >> 8) as u8, color as u8],
                rgba[..3],
                "{name}: color at {i}"
            );
            let at = 12 + count * 4 + i * 4;
            let depth = f32::from_le_bytes(exported[at..at + 4].try_into()?);
            assert_eq!(f32::from(tile.depth[i]), depth, "{name}: depth at {i}");
        }
    }
    Ok(())
}
