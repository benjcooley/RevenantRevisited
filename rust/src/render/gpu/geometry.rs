// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  geometry.rs - Batched GPU draw preparation            *
// *************************************************************************

use super::*;
use bytemuck::{Pod, Zeroable};
use std::{collections::BTreeMap, ops::Range};

#[repr(C)]
#[derive(Clone, Copy, Pod, Zeroable)]
pub(super) struct Vertex {
    pub pos: [f32; 4],
    pub normal: [f32; 4],
    pub uv: [f32; 4],
    pub rect: [f32; 4],
    pub tint: [f32; 4],
    pub tile: [f32; 4],
    pub extra: [f32; 4],
    pub info: [u32; 4],
}
impl Vertex {
    pub fn layout() -> wgpu::VertexBufferLayout<'static> {
        const ATTRS: [wgpu::VertexAttribute; 8] = wgpu::vertex_attr_array![0=>Float32x4,1=>Float32x4,2=>Float32x4,3=>Float32x4,4=>Float32x4,5=>Float32x4,6=>Float32x4,7=>Uint32x4];
        wgpu::VertexBufferLayout {
            array_stride: std::mem::size_of::<Self>() as u64,
            step_mode: wgpu::VertexStepMode::Vertex,
            attributes: &ATTRS,
        }
    }
}
#[derive(Clone)]
pub(super) struct Batch {
    pub page: usize,
    pub depth: f32,
    pub vertices: Range<u32>,
}
pub(super) struct Geometry {
    pub vertices: Vec<Vertex>,
    pub opaque: Vec<Batch>,
    pub transparent: Vec<Batch>,
    pub depth: [f32; 2],
    pub visible: usize,
}

pub(super) fn projected(p: [f64; 3]) -> [f32; 3] {
    [
        (p[0] - p[1]) as f32,
        ((p[0] + p[1]) * 0.5 - p[2] * camera::ISO_COS) as f32,
        (-(p[0] + p[1]) * camera::ISO_COS - p[2] * 0.5) as f32,
    ]
}
fn screen(p: [f32; 3], camera: &Camera, viewport: [u32; 2], focal: f32) -> [f32; 2] {
    let mut zoom = camera.zoom as f32;
    if focal > 0.0 {
        zoom *= focal / (focal + p[2] + 2.0 * camera.center[1] as f32 * 0.867).max(1.0);
    }
    [
        (p[0] - camera.center[0] as f32) * zoom + viewport[0] as f32 * 0.5,
        (p[1] - camera.center[1] as f32) * zoom + viewport[1] as f32 * 0.5,
    ]
}
fn visible(rect: [f32; 4], viewport: [u32; 2]) -> bool {
    rect[2] > 0.0 && rect[3] > 0.0 && rect[0] < viewport[0] as f32 && rect[1] < viewport[1] as f32
}
fn append(
    vertices: &mut Vec<Vertex>,
    batches: &mut Vec<Batch>,
    page: usize,
    source: Vec<Vertex>,
    depth: Option<f32>,
) {
    let start = vertices.len() as u32;
    vertices.extend(source);
    let end = vertices.len() as u32;
    if start == end {
        return;
    }
    if let Some(last) = batches
        .last_mut()
        .filter(|b| depth.is_none() && b.page == page && b.vertices.end == start)
    {
        last.vertices.end = end;
    } else {
        batches.push(Batch {
            page,
            depth: depth.unwrap_or(0.0),
            vertices: start..end,
        });
    }
}
impl Geometry {
    pub fn build(
        draws: &[Draw],
        textures: &textures::Textures,
        camera: &Camera,
        viewport: [u32; 2],
        focal: f32,
    ) -> Result<Self> {
        let mut opaque: BTreeMap<usize, Vec<Vertex>> = BTreeMap::new();
        let mut transparent: Vec<(f32, usize, Vec<Vertex>)> = Vec::new();
        let mut depth = [f32::INFINITY, f32::NEG_INFINITY];
        let mut count = 0;
        for (owner, draw) in draws.iter().enumerate() {
            ensure!(owner < u32::MAX as usize - 1, "draw owner overflow");
            let mut contributed = false;
            let mut submit = |page: usize, vertices: Vec<Vertex>, trans: bool, z: [f32; 2]| {
                depth[0] = depth[0].min(z[0]);
                depth[1] = depth[1].max(z[1]);
                contributed = true;
                if trans {
                    transparent.push(((z[0] + z[1]) * 0.5, page, vertices.clone()));
                }
                opaque.entry(page).or_default().extend(vertices);
            };
            match draw {
                Draw::Tile(draw) => {
                    let t = &draw.tile;
                    let image = textures.tile(t);
                    let b = draw.bounds();
                    let anchor = camera::world_depth(draw.position) as f32
                        - f32::from(t.state.registration[2]);
                    let z = [anchor + image.depth[0], anchor + image.depth[1]];
                    let mut screen_bounds = [
                        f32::INFINITY,
                        f32::INFINITY,
                        f32::NEG_INFINITY,
                        f32::NEG_INFINITY,
                    ];
                    for x in [b[0], b[2]] {
                        for y in [b[1], b[3]] {
                            for depth in z {
                                let p =
                                    screen([x as f32, y as f32, depth], camera, viewport, focal);
                                screen_bounds[0] = screen_bounds[0].min(p[0]);
                                screen_bounds[1] = screen_bounds[1].min(p[1]);
                                screen_bounds[2] = screen_bounds[2].max(p[0]);
                                screen_bounds[3] = screen_bounds[3].max(p[1]);
                            }
                        }
                    }
                    if !visible(screen_bounds, viewport) {
                        continue;
                    }
                    let r = image.slice;
                    let mut quad = [Vertex::zeroed(); 4];
                    for (i, [u, v]) in [[0.0, 0.0], [1.0, 0.0], [0.0, 1.0], [1.0, 1.0]]
                        .into_iter()
                        .enumerate()
                    {
                        let p = if focal > 0.0 {
                            [
                                screen_bounds[0] + (screen_bounds[2] - screen_bounds[0]) * u,
                                screen_bounds[1] + (screen_bounds[3] - screen_bounds[1]) * v,
                            ]
                        } else {
                            [
                                b[0] as f32 + t.width as f32 * u,
                                b[1] as f32 + t.height as f32 * v,
                            ]
                        };
                        quad[i] = Vertex {
                            pos: [p[0], p[1], anchor, 1.0],
                            normal: [0.0, 0.0, 1.0, 0.0],
                            uv: [u * t.width as f32, v * t.height as f32, 0.0, 0.0],
                            rect: [r.x as f32, r.y as f32, r.width as f32, r.height as f32],
                            tint: [1.0; 4],
                            tile: [b[0] as f32, b[1] as f32, anchor, z[0]],
                            extra: [z[1], 0.0, 0.0, 0.0],
                            info: [owner as u32 + 1, 1, u32::from(draw.flipped), 0],
                        };
                    }
                    submit(
                        r.page,
                        [0, 1, 2, 2, 1, 3].map(|i| quad[i]).to_vec(),
                        image.translucent,
                        z,
                    );
                }
                Draw::Mesh(_) => {}
            }
            count += usize::from(contributed);
        }
        let mut result = Self {
            vertices: Vec::new(),
            opaque: Vec::new(),
            transparent: Vec::new(),
            depth: [-1.0, 1.0],
            visible: count,
        };
        if depth[0].is_finite() {
            result.depth = [depth[0] - 1.0, depth[1] + 1.0];
        }
        for (page, vertices) in opaque {
            append(
                &mut result.vertices,
                &mut result.opaque,
                page,
                vertices,
                None,
            );
        }
        transparent.sort_by(|a, b| b.0.total_cmp(&a.0));
        for (depth, page, vertices) in transparent {
            append(
                &mut result.vertices,
                &mut result.transparent,
                page,
                vertices,
                Some(depth),
            );
        }
        ensure!(
            result.vertices.len() <= 8_000_000,
            "visible geometry exceeds vertex budget"
        );
        Ok(result)
    }
}
