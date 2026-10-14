// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                effects.rs - GPU effect batching and blend states      *
// *************************************************************************

use super::{
    geometry::{self, Geometry, Vertex},
    textures, *,
};
use crate::render::effects::*;
use std::ops::Range;

pub(super) struct Batch {
    pub page: usize,
    pub pipeline: usize,
    pub vertices: Range<u32>,
    pub clip: Option<[u32; 4]>,
}
fn uv_rect(rect: [f32; 4]) -> [[f32; 2]; 4] {
    [
        [rect[0], rect[1]],
        [rect[0] + rect[2], rect[1]],
        [rect[0], rect[1] + rect[3]],
        [rect[0] + rect[2], rect[1] + rect[3]],
    ]
}
pub(super) fn build(
    effects: &[Effect],
    textures: &textures::Textures,
    geometry: &mut Geometry,
    padding: u32,
    size: [u32; 2],
) -> Result<Vec<Batch>> {
    ensure!(
        effects.len() <= 100_000,
        "effect submission budget exceeded"
    );
    let mut items = Vec::new();
    for (order, effect) in effects.iter().enumerate() {
        effect.validate()?;
        let image = textures.effect(effect.material.texture.as_ref());
        let r = image.slice;
        let (positions, uv, colors, triangle, clip, overlay) = match &effect.primitive {
            Primitive::Billboard(p) => {
                let mut positions = [[0.0; 3]; 4];
                let center = geometry::projected(p.position.map(f64::from));
                let (sin, cos) = p.rotation.sin_cos();
                for (i, [x, y]) in [[-0.5, -0.5], [0.5, -0.5], [-0.5, 0.5], [0.5, 0.5]]
                    .into_iter()
                    .enumerate()
                {
                    let (x, y) = (x * p.size[0], y * p.size[1]);
                    let (x, y) = (x * cos - y * sin, x * sin + y * cos);
                    positions[i] = match p.orientation {
                        Orientation::Screen => [center[0] + x, center[1] + y, center[2]],
                        Orientation::WorldXy => geometry::projected([
                            f64::from(p.position[0] + x),
                            f64::from(p.position[1] + y),
                            f64::from(p.position[2]),
                        ]),
                    };
                }
                (positions, uv_rect(p.uv), [p.color; 4], false, None, false)
            }
            Primitive::Strip(p) => {
                let [a, b] = p.ends.map(|p| geometry::projected(p.map(f64::from)));
                let delta = [b[0] - a[0], b[1] - a[1]];
                let length = delta[0].hypot(delta[1]);
                if length < 1e-6 {
                    continue;
                }
                let side = [-delta[1] / length, delta[0] / length];
                let positions = std::array::from_fn(|i| {
                    let end = i / 2;
                    let c = [a, b][end];
                    let sign = if i % 2 == 0 { -0.5 } else { 0.5 };
                    [
                        c[0] + side[0] * p.widths[end] * sign,
                        c[1] + side[1] * p.widths[end] * sign,
                        c[2],
                    ]
                });
                let uv = std::array::from_fn(|i| {
                    let (u, v) = (p.u[i / 2], p.v[i % 2]);
                    if p.swap_uv { [v, u] } else { [u, v] }
                });
                (
                    positions,
                    uv,
                    [p.colors[0], p.colors[0], p.colors[1], p.colors[1]],
                    false,
                    None,
                    false,
                )
            }
            Primitive::Quad(p) => (
                p.positions.map(|p| geometry::projected(p.map(f64::from))),
                p.uv,
                p.colors,
                p.triangle,
                None,
                false,
            ),
            Primitive::Overlay(p) => {
                let positions =
                    uv_rect(p.rect).map(|p| [p[0] + padding as f32, p[1] + padding as f32, 0.0]);
                let clip = p.clip.map(|[x, y, w, h]| {
                    let x = x.saturating_add(padding).min(size[0]);
                    let y = y.saturating_add(padding).min(size[1]);
                    [x, y, w.min(size[0] - x), h.min(size[1] - y)]
                });
                if clip.is_some_and(|c| c[2] == 0 || c[3] == 0) {
                    continue;
                }
                (positions, uv_rect(p.uv), [p.color; 4], false, clip, true)
            }
        };
        let material = &effect.material;
        let flags = u32::from(material.lit)
            | ((material.debug as u32) << 1)
            | ((material.filter as u32) << 4)
            | ((material.address as u32) << 5);
        let pipeline = (material.blend as usize) * 3
            + if overlay {
                Depth::None as usize
            } else {
                material.depth as usize
            };
        let vertices: Vec<_> = (if triangle {
            &[0, 1, 2][..]
        } else {
            &[0, 1, 2, 2, 1, 3][..]
        })
        .iter()
        .map(|&i| {
            let p = positions[i];
            Vertex {
                pos: [p[0], p[1], p[2], 1.0],
                normal: [(i % 2) as f32, (i / 2) as f32, 1.0, 0.0],
                uv: [uv[i][0], uv[i][1], 0.0, 0.0],
                rect: [r.x as f32, r.y as f32, r.width as f32, r.height as f32],
                tint: colors[i],
                tile: [0.0; 4],
                extra: [0.0; 4],
                info: [0, if overlay { 3 } else { 2 }, 0, flags],
            }
        })
        .collect();
        let depth = vertices.iter().map(|v| v.pos[2]).sum::<f32>() / vertices.len() as f32;
        if !overlay {
            for v in &vertices {
                geometry.depth[0] = geometry.depth[0].min(v.pos[2] - 1.0);
                geometry.depth[1] = geometry.depth[1].max(v.pos[2] + 1.0);
            }
        }
        items.push((overlay, depth, order, r.page, pipeline, clip, vertices));
    }
    // World particles sort back to front; UI keeps exact submit order.
    items.sort_by(|a, b| {
        a.0.cmp(&b.0).then_with(|| {
            if a.0 {
                a.2.cmp(&b.2)
            } else {
                b.1.total_cmp(&a.1).then(a.2.cmp(&b.2))
            }
        })
    });
    let mut batches: Vec<Batch> = Vec::new();
    for (_, _, _, page, pipeline, clip, vertices) in items {
        let start = geometry.vertices.len() as u32;
        geometry.vertices.extend(vertices);
        let end = geometry.vertices.len() as u32;
        if let Some(last) = batches
            .last_mut()
            .filter(|last| last.page == page && last.pipeline == pipeline && last.clip == clip)
        {
            last.vertices.end = end;
        } else {
            batches.push(Batch {
                page,
                pipeline,
                clip,
                vertices: start..end,
            });
        }
    }
    Ok(batches)
}
pub(super) fn pipelines(
    device: &wgpu::Device,
    shader: &wgpu::ShaderModule,
    layout: &wgpu::PipelineLayout,
) -> Vec<wgpu::RenderPipeline> {
    (0..12)
        .map(|i| {
            use wgpu::{BlendFactor as F, BlendOperation as Op};
            let (src, dst) = match i / 3 {
                0 => (F::SrcAlpha, F::OneMinusSrcAlpha),
                1 => (F::SrcAlpha, F::One),
                2 => (F::One, F::One),
                _ => (F::One, F::OneMinusSrcAlpha),
            };
            device.create_render_pipeline(&wgpu::RenderPipelineDescriptor {
                label: Some("FX blend/depth"),
                layout: Some(layout),
                vertex: wgpu::VertexState {
                    module: shader,
                    entry_point: Some("vs"),
                    compilation_options: Default::default(),
                    buffers: &[Vertex::layout()],
                },
                primitive: Default::default(),
                depth_stencil: Some(wgpu::DepthStencilState {
                    format: wgpu::TextureFormat::Depth32Float,
                    depth_write_enabled: i % 3 == 2,
                    depth_compare: if i % 3 == 1 {
                        wgpu::CompareFunction::Always
                    } else {
                        wgpu::CompareFunction::LessEqual
                    },
                    stencil: Default::default(),
                    bias: Default::default(),
                }),
                multisample: Default::default(),
                fragment: Some(wgpu::FragmentState {
                    module: shader,
                    entry_point: Some("effect"),
                    compilation_options: Default::default(),
                    targets: &[Some(wgpu::ColorTargetState {
                        format: wgpu::TextureFormat::Rgba8Unorm,
                        blend: Some(wgpu::BlendState {
                            color: wgpu::BlendComponent {
                                src_factor: src,
                                dst_factor: dst,
                                operation: Op::Add,
                            },
                            alpha: wgpu::BlendComponent::OVER,
                        }),
                        write_mask: wgpu::ColorWrites::ALL,
                    })],
                }),
                multiview: None,
                cache: None,
            })
        })
        .collect()
}
