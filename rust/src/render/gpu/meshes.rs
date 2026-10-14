// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  meshes.rs - Persistent indexed mesh residency       *
// *************************************************************************

use super::*;
use crate::{assets::mesh::Mesh, render::mesh::Transform};
use std::{
    collections::{BTreeMap, BTreeSet},
    ops::Range,
    sync::{Arc, Weak},
};

#[repr(C)]
#[derive(Clone, Copy, Pod, Zeroable)]
struct Vertex {
    position: [f32; 3],
    normal: [f32; 3],
    uv: [f32; 2],
}
#[repr(C)]
#[derive(Clone, Copy, Pod, Zeroable)]
pub(super) struct Instance {
    world: [[f32; 4]; 3],
    normal: [[f32; 4]; 3],
    rect: [f32; 4],
    tint: [f32; 4],
    info: [u32; 4],
    uv: [f32; 4],
    ambient: [f32; 4],
    specular: [f32; 4],
    emissive: [f32; 4],
}
pub(super) fn layouts() -> [wgpu::VertexBufferLayout<'static>; 2] {
    const V: [wgpu::VertexAttribute; 3] =
        wgpu::vertex_attr_array![0=>Float32x3,1=>Float32x3,2=>Float32x2];
    const I: [wgpu::VertexAttribute; 13] = wgpu::vertex_attr_array![3=>Float32x4,4=>Float32x4,5=>Float32x4,6=>Float32x4,7=>Float32x4,8=>Float32x4,9=>Float32x4,10=>Float32x4,11=>Uint32x4,12=>Float32x4,13=>Float32x4,14=>Float32x4,15=>Float32x4];
    [
        wgpu::VertexBufferLayout {
            array_stride: 32,
            step_mode: wgpu::VertexStepMode::Vertex,
            attributes: &V,
        },
        wgpu::VertexBufferLayout {
            array_stride: 208,
            step_mode: wgpu::VertexStepMode::Instance,
            attributes: &I,
        },
    ]
}
struct Asset {
    source: Weak<Mesh>,
    vertices: wgpu::Buffer,
    indices: wgpu::Buffer,
    ranges: Vec<Vec<Range<u32>>>,
    bounds: Vec<Vec<[[f64; 3]; 2]>>,
    vertex_count: i32,
    bytes: usize,
}
pub(super) struct Meshes {
    assets: BTreeMap<usize, Asset>,
    budget: usize,
    pub uploads: u64,
}
pub(super) struct Batch {
    pub depth: f32,
    sample: usize,
    root: usize,
    page: usize,
    indices: Range<u32>,
    instances: Range<u32>,
}
pub(super) struct Frame {
    pub instances: Vec<Instance>,
    pub opaque: Vec<Batch>,
    pub transparent: Vec<Batch>,
    pub visible: usize,
}
fn bounds(mesh: &Mesh, vertices: &[crate::assets::mesh::Vertex]) -> Vec<[[f64; 3]; 2]> {
    mesh.parts
        .iter()
        .map(|part| {
            let mut b = [[f64::INFINITY; 3], [f64::NEG_INFINITY; 3]];
            for v in &vertices[part.vertices.clone()] {
                for (i, &value) in v.position.iter().enumerate() {
                    b[0][i] = b[0][i].min(value);
                    b[1][i] = b[1][i].max(value);
                }
            }
            b
        })
        .collect()
}
fn project_bounds(
    bounds: [[f64; 3]; 2],
    transform: Transform,
    camera: &Camera,
    size: [u32; 2],
    focal: f32,
) -> ([f32; 2], bool) {
    let mut z = [f32::INFINITY, f32::NEG_INFINITY];
    let mut rect = [
        f32::INFINITY,
        f32::INFINITY,
        f32::NEG_INFINITY,
        f32::NEG_INFINITY,
    ];
    for x in [bounds[0][0], bounds[1][0]] {
        for y in [bounds[0][1], bounds[1][1]] {
            for v in [bounds[0][2], bounds[1][2]] {
                let p = geometry::projected(transform.point([x, y, v]));
                z[0] = z[0].min(p[2]);
                z[1] = z[1].max(p[2]);
                let zoom = camera.zoom as f32
                    * if focal > 0.0 {
                        focal / (focal + p[2] + 2.0 * camera.center[1] as f32 * 0.867).max(1.0)
                    } else {
                        1.0
                    };
                let s = [
                    (p[0] - camera.center[0] as f32) * zoom + size[0] as f32 * 0.5,
                    (p[1] - camera.center[1] as f32) * zoom + size[1] as f32 * 0.5,
                ];
                rect[0] = rect[0].min(s[0]);
                rect[1] = rect[1].min(s[1]);
                rect[2] = rect[2].max(s[0]);
                rect[3] = rect[3].max(s[1]);
            }
        }
    }
    (
        z,
        rect[2] > 0.0 && rect[3] > 0.0 && rect[0] < size[0] as f32 && rect[1] < size[1] as f32,
    )
}
impl Meshes {
    pub fn new(budget: usize) -> Self {
        Self {
            assets: BTreeMap::new(),
            budget,
            uploads: 0,
        }
    }
    pub fn clear(&mut self) {
        self.assets.clear();
    }
    pub fn stats(&self) -> (usize, usize) {
        (
            self.assets.len(),
            self.assets.values().map(|v| v.bytes).sum(),
        )
    }
    fn prepare(&mut self, draws: &[Draw], device: &wgpu::Device) -> Result<()> {
        let required: BTreeSet<_> = draws
            .iter()
            .filter_map(|d| if let Draw::Mesh(m) = d { Some(m) } else { None })
            .flat_map(|m| m.asset_meshes())
            .map(|m| Arc::as_ptr(m) as usize)
            .collect();
        self.assets.retain(|_, a| a.source.strong_count() > 0);
        for draw in draws {
            let Draw::Mesh(draw) = draw else {
                continue;
            };
            for mesh in draw.asset_meshes() {
                let root = Arc::as_ptr(mesh) as usize;
                if self.assets.contains_key(&root) {
                    continue;
                }
                let vertices: Vec<_> = mesh
                    .vertex_samples()
                    .flatten()
                    .map(|v| Vertex {
                        position: v.position.map(|v| v as f32),
                        normal: v.normal.map(|v| v as f32),
                        uv: v.uv.map(|v| v as f32),
                    })
                    .collect();
                let mut indices = Vec::new();
                let ranges =
                    mesh.parts
                        .iter()
                        .map(|p| {
                            p.batches
                                .iter()
                                .map(|b| {
                                    let start = indices.len() as u32;
                                    for face in &mesh.faces[b.faces.clone()] {
                                        indices.extend(face.map(|index| {
                                            u32::from(index) + p.vertices.start as u32
                                        }));
                                    }
                                    start..indices.len() as u32
                                })
                                .collect()
                        })
                        .collect();
                let bytes = vertices.len() * 32 + indices.len() * 4;
                if self.stats().1 + bytes > self.budget {
                    self.assets.retain(|key, _| required.contains(key));
                }
                ensure!(
                    self.stats().1 + bytes <= self.budget,
                    "GPU mesh residency budget exceeded"
                );
                let make = |label, data: &[u8], usage| {
                    device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
                        label: Some(label),
                        contents: if data.is_empty() { &[0; 4] } else { data },
                        usage,
                    })
                };
                let asset = Asset {
                    source: Arc::downgrade(mesh),
                    vertices: make(
                        "resident mesh vertices",
                        bytemuck::cast_slice(&vertices),
                        wgpu::BufferUsages::VERTEX,
                    ),
                    indices: make(
                        "resident mesh indices",
                        bytemuck::cast_slice(&indices),
                        wgpu::BufferUsages::INDEX,
                    ),
                    ranges,
                    bounds: mesh.vertex_samples().map(|v| bounds(mesh, v)).collect(),
                    vertex_count: mesh.vertices.len() as i32,
                    bytes,
                };
                self.assets.insert(root, asset);
                self.uploads += 1;
            }
        }
        Ok(())
    }
    pub fn frame(
        &mut self,
        draws: &[Draw],
        textures: &textures::Textures,
        geometry: &mut geometry::Geometry,
        camera: &Camera,
        projection: ([u32; 2], f32),
        device: &wgpu::Device,
    ) -> Result<Frame> {
        self.prepare(draws, device)?;
        let (size, focal) = projection;
        let mut opaque: BTreeMap<(usize, usize, usize, usize, usize), Vec<Instance>> =
            BTreeMap::new();
        let mut transparent = Vec::new();
        let mut visible = BTreeSet::new();
        for (owner, draw) in draws.iter().enumerate() {
            let Draw::Mesh(draw) = draw else {
                continue;
            };
            for (mesh, part, transform, state, frame) in draw.posed_parts() {
                let root = std::ptr::from_ref(mesh) as usize;
                let asset = &self.assets[&root];
                let sample = mesh.vertex_sample(state, frame);
                let (z, show) =
                    project_bounds(asset.bounds[sample][part], transform, camera, size, focal);
                if !show {
                    continue;
                }
                let p = &mesh.parts[part];
                let material = mesh.materials[p.material];
                let appearance = draw.appearance();
                let mut tint = std::array::from_fn(|i| {
                    material.diffuse[i].clamp(0.0, 1.0) as f32 * appearance.tint[i]
                });
                if appearance.shading == crate::render::mesh::Shading::Material {
                    tint[3] = material.diffuse[3]
                        .max(material.ambient[3])
                        .max(material.specular[3])
                        .max(material.emissive[3])
                        .clamp(0.0, 1.0) as f32
                        * appearance.tint[3];
                }
                if tint[3] <= 0.0 {
                    continue;
                }
                visible.insert(owner);
                geometry.depth[0] = geometry.depth[0].min(z[0] - 1.0);
                geometry.depth[1] = geometry.depth[1].max(z[1] + 1.0);
                for (index, batch) in p.batches.iter().enumerate() {
                    if batch.faces.is_empty() {
                        continue;
                    }
                    let image = textures.mesh(mesh, batch.texture, frame);
                    let r = image.slice;
                    let instance = Instance {
                        world: transform.rows(),
                        normal: transform.normal_rows(),
                        rect: [r.x as f32, r.y as f32, r.width as f32, r.height as f32],
                        tint,
                        info: [
                            owner as u32 + 1,
                            0,
                            batch.texture.map_or(0, |t| {
                                mesh.textures[t][frame as usize % mesh.textures[t].len()].lighting
                                    as u32
                            }),
                            appearance.shading as u32,
                        ],
                        uv: [
                            appearance.uv_offset[0],
                            appearance.uv_offset[1],
                            material.power.max(1.0) as f32,
                            0.0,
                        ],
                        ambient: material.ambient.map(|v| v as f32),
                        specular: material.specular.map(|v| v as f32),
                        emissive: material.emissive.map(|v| v as f32),
                    };
                    let key = (root, sample, part, index, r.page);
                    opaque.entry(key).or_default().push(instance);
                    if image.translucent || tint[3] < 0.999 {
                        transparent.push(((z[0] + z[1]) * 0.5, key, instance));
                    }
                }
            }
        }
        let mut result = Frame {
            instances: Vec::new(),
            opaque: Vec::new(),
            transparent: Vec::new(),
            visible: visible.len(),
        };
        for ((root, sample, part, index, page), instances) in opaque {
            let start = result.instances.len() as u32;
            result.instances.extend(instances);
            result.opaque.push(Batch {
                sample,
                root,
                page,
                depth: 0.0,
                indices: self.assets[&root].ranges[part][index].clone(),
                instances: start..result.instances.len() as u32,
            });
        }
        transparent.sort_by(|a, b| b.0.total_cmp(&a.0));
        for (depth, (root, sample, part, index, page), instance) in transparent {
            let start = result.instances.len() as u32;
            result.instances.push(instance);
            result.transparent.push(Batch {
                sample,
                depth,
                root,
                page,
                indices: self.assets[&root].ranges[part][index].clone(),
                instances: start..start + 1,
            });
        }
        Ok(result)
    }
    pub fn draw<'a>(
        &'a self,
        pass: &mut wgpu::RenderPass<'a>,
        batches: &[Batch],
        textures: &'a textures::Textures,
    ) {
        for batch in batches {
            let asset = &self.assets[&batch.root];
            pass.set_bind_group(1, textures.bind(batch.page), &[]);
            pass.set_vertex_buffer(0, asset.vertices.slice(..));
            pass.set_index_buffer(asset.indices.slice(..), wgpu::IndexFormat::Uint32);
            pass.draw_indexed(
                batch.indices.clone(),
                batch.sample as i32 * asset.vertex_count,
                batch.instances.clone(),
            );
        }
    }
}
