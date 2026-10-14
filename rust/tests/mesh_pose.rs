// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             mesh_pose.rs - C++ posed mesh compatibility tests          *
// *************************************************************************

use anyhow::{Context, Result, ensure};
use revenant::{
    assets::{mesh::Mesh, vfs::Vfs},
    render::mesh::MeshDraw,
    world::sector::Position,
};
use serde_json::Value;
use std::{fs, path::PathBuf, sync::Arc};

fn index(v: &Value) -> Result<usize> {
    Ok(v.as_u64().context("expected JSON index")? as usize)
}

fn sample(
    json: &Value,
    bin: &[u8],
    accessor: usize,
    frame: usize,
    size: usize,
) -> Result<Vec<f64>> {
    let a = &json["accessors"][accessor];
    ensure!(a["componentType"] == 5126, "expected float accessor");
    ensure!(frame < index(&a["count"])?, "frame out of range");
    let view = &json["bufferViews"][index(&a["bufferView"])?];
    let stride = view["byteStride"].as_u64().unwrap_or((size * 4) as u64) as usize;
    let at = view["byteOffset"].as_u64().unwrap_or(0) as usize
        + a["byteOffset"].as_u64().unwrap_or(0) as usize
        + frame * stride;
    (0..size)
        .map(|i| {
            Ok(f64::from(f32::from_le_bytes(
                bin.get(at + i * 4..at + i * 4 + 4)
                    .context("truncated GLB accessor")?
                    .try_into()?,
            )))
        })
        .collect()
}

fn cross(a: [f64; 3], b: [f64; 3]) -> [f64; 3] {
    [
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0],
    ]
}

fn gltf_point(p: [f64; 3], t: &[f64], q: &[f64], s: &[f64]) -> [f64; 3] {
    let p = [p[0] * s[0], p[2] * s[1], -p[1] * s[2]];
    let qv = [q[0], q[1], q[2]];
    let u = cross(qv, p);
    let v = cross(qv, u);
    let world: [f64; 3] = std::array::from_fn(|c| t[c] + p[c] + 2.0 * (q[3] * u[c] + v[c]));
    [world[0], -world[2], world[1] * 1.5]
}

#[test]
#[ignore = "requires REVENANT_CPP_GLTF_DUMP pointing to C++ gltf_dump_all"]
fn jason_vertices_match_cpp_animated_poses() -> Result<()> {
    let root = PathBuf::from(
        std::env::var_os("REVENANT_CPP_GLTF_DUMP").context("set REVENANT_CPP_GLTF_DUMP")?,
    );
    let glb = fs::read(root.join("jason.glb"))?;
    ensure!(glb.get(..4) == Some(b"glTF"), "invalid GLB header");
    let json_len = u32::from_le_bytes(glb[12..16].try_into()?) as usize;
    let json: Value = serde_json::from_slice(&glb[20..20 + json_len])?;
    let bin = &glb[28 + json_len..];
    let mut vfs = Vfs::open(Vfs::discover(None)?, "Ahkuilon", None)?;
    let mesh = Arc::new(Mesh::decode(&vfs.read("imagery/Chars/jason.i3d")?)?);
    let mut max_error = 0.0f64;
    for (state, clip) in mesh.clips.iter().enumerate() {
        for frame in [0, clip.frames / 2, clip.frames - 1] {
            let draw = MeshDraw::new(
                Arc::clone(&mesh),
                Position::default(),
                [0; 3],
                state as u16,
                frame,
            )?;
            let animation = &json["animations"][state];
            for (part_index, part) in mesh.parts.iter().enumerate() {
                let (node_index, _node) = json["nodes"]
                    .as_array()
                    .context("missing nodes")?
                    .iter()
                    .enumerate()
                    .find(|(_, n)| n["name"] == part.name)
                    .context("missing part node")?;
                let mut t = vec![0.0; 3];
                let mut q = vec![0.0, 0.0, 0.0, 1.0];
                let mut s = vec![1.0; 3];
                for channel in animation["channels"]
                    .as_array()
                    .context("missing channels")?
                {
                    if index(&channel["target"]["node"])? != node_index {
                        continue;
                    }
                    let accessor =
                        index(&animation["samplers"][index(&channel["sampler"])?]["output"])?;
                    match channel["target"]["path"].as_str() {
                        Some("translation") => t = sample(&json, bin, accessor, frame as usize, 3)?,
                        Some("rotation") => q = sample(&json, bin, accessor, frame as usize, 4)?,
                        Some("scale") => s = sample(&json, bin, accessor, frame as usize, 3)?,
                        _ => {}
                    }
                }
                for vertex in &mesh.vertices[part.vertices.clone()] {
                    let actual = draw
                        .part_transform(part_index)
                        .context("missing part transform")?
                        .point(vertex.position);
                    let expected = gltf_point(vertex.position, &t, &q, &s);
                    let error = actual
                        .into_iter()
                        .zip(expected)
                        .map(|(a, b)| (a - b).abs())
                        .fold(0.0f64, f64::max);
                    max_error = max_error.max(error);
                    ensure!(
                        error < 0.002,
                        "{} frame {frame} part {}: world vertex differs by {error}: Rust {actual:?}, C++ {expected:?}",
                        clip.name,
                        part.name
                    );
                }
            }
        }
    }
    println!("Jason: maximum posed vertex error {max_error}");
    Ok(())
}
