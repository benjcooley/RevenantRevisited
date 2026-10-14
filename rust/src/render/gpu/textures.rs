// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 textures.rs - Renderer-owned atlas residency          *
// *************************************************************************

use super::*;
use crate::assets::{
    imagery::Tile,
    mesh::{Mesh, Texture},
};
use std::{
    collections::{BTreeMap, BTreeSet},
    sync::{Arc, Weak},
};

#[derive(Clone, Copy)]
pub(super) struct Image {
    pub slice: atlas::Slice,
    pub depth: [f32; 2],
    pub translucent: bool,
}
#[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
struct Key {
    root: usize,
    texture: usize,
    frame: usize,
    kind: u8,
}
enum Source {
    Tile(Weak<Tile>),
    Mesh(Weak<Mesh>),
    Texture(Weak<Texture>),
}
impl Source {
    fn alive(&self) -> bool {
        match self {
            Self::Tile(t) => t.strong_count() > 0,
            Self::Mesh(m) => m.strong_count() > 0,
            Self::Texture(t) => t.strong_count() > 0,
        }
    }
}
struct Page {
    _color: wgpu::Texture,
    _depth: wgpu::Texture,
    pub bind: wgpu::BindGroup,
}

pub(super) struct Textures {
    atlas: atlas::Atlas,
    images: BTreeMap<Key, Image>,
    sources: BTreeMap<(u8, usize), Source>,
    pages: Vec<Page>,
    dirty: BTreeSet<usize>,
    pub white: Image,
    budget: usize,
    pub generation: u64,
    pub uploads: u64,
}
impl Textures {
    pub fn new(budget: usize) -> Result<Self> {
        let mut atlas = atlas::Atlas::new(2048, budget)?;
        let white = Image {
            slice: atlas.insert(1, 1, &[0xffffffff], None)?,
            depth: [0.0; 2],
            translucent: false,
        };
        Ok(Self {
            atlas,
            images: BTreeMap::new(),
            sources: BTreeMap::new(),
            pages: Vec::new(),
            dirty: BTreeSet::from([0]),
            white,
            budget,
            generation: 0,
            uploads: 0,
        })
    }
    pub fn prepare(
        &mut self,
        draws: &[Draw],
        effects: &[crate::render::effects::Effect],
        device: &wgpu::Device,
        queue: &wgpu::Queue,
        layout: &wgpu::BindGroupLayout,
    ) -> Result<()> {
        // Repack only at a frame boundary. No public slice survives a generation.
        if self.sources.values().any(|source| !source.alive()) {
            let generation = self.generation + 1;
            let uploads = self.uploads;
            *self = Self::new(self.budget)?;
            self.generation = generation;
            self.uploads = uploads;
        }
        // Budget pressure can evict old animation frames while their source
        // mesh remains alive. Repack only the current frame's working set.
        if self
            .collect(draws)
            .and_then(|_| self.collect_effects(effects))
            .is_err()
        {
            let mut next = Self::new(self.budget)?;
            next.generation = self.generation + 1;
            next.uploads = self.uploads;
            next.collect(draws)?;
            next.collect_effects(effects)?;
            *self = next;
        }
        for index in std::mem::take(&mut self.dirty) {
            let p = &self.atlas.pages[index];
            if index >= self.pages.len() {
                let color = texture(
                    device,
                    "atlas color",
                    p.width,
                    p.height,
                    wgpu::TextureFormat::Rgba8Unorm,
                    wgpu::TextureUsages::TEXTURE_BINDING | wgpu::TextureUsages::COPY_DST,
                );
                let depth = texture(
                    device,
                    "atlas signed depth",
                    p.width,
                    p.height,
                    wgpu::TextureFormat::R32Float,
                    wgpu::TextureUsages::TEXTURE_BINDING | wgpu::TextureUsages::COPY_DST,
                );
                let color_view = color.create_view(&Default::default());
                let depth_view = depth.create_view(&Default::default());
                let bind = device.create_bind_group(&wgpu::BindGroupDescriptor {
                    label: Some("atlas page"),
                    layout,
                    entries: &[
                        wgpu::BindGroupEntry {
                            binding: 0,
                            resource: wgpu::BindingResource::TextureView(&color_view),
                        },
                        wgpu::BindGroupEntry {
                            binding: 1,
                            resource: wgpu::BindingResource::TextureView(&depth_view),
                        },
                    ],
                });
                self.pages.push(Page {
                    _color: color,
                    _depth: depth,
                    bind,
                });
            }
            let rgba: Vec<_> = p
                .color
                .iter()
                .flat_map(|c| [(c >> 16) as u8, (c >> 8) as u8, *c as u8, (c >> 24) as u8])
                .collect();
            upload(queue, &self.pages[index]._color, p.width, p.height, &rgba);
            upload(
                queue,
                &self.pages[index]._depth,
                p.width,
                p.height,
                bytemuck::cast_slice(&p.depth),
            );
            self.uploads += 1;
        }
        Ok(())
    }
    fn collect(&mut self, draws: &[Draw]) -> Result<()> {
        for draw in draws {
            match draw {
                Draw::Tile(draw) => {
                    let root = Arc::as_ptr(&draw.tile) as usize;
                    let key = Key {
                        root,
                        texture: 0,
                        frame: 0,
                        kind: 0,
                    };
                    self.sources
                        .entry((0, root))
                        .or_insert_with(|| Source::Tile(Arc::downgrade(&draw.tile)));
                    if self.images.contains_key(&key) {
                        continue;
                    }
                    let t = &draw.tile;
                    let slice = self.atlas.insert(
                        t.width as u32,
                        t.height as u32,
                        &t.pixels,
                        Some(&t.depth),
                    )?;
                    let (mut lo, mut hi) = (f32::INFINITY, f32::NEG_INFINITY);
                    let mut translucent = false;
                    for (&color, &depth) in t.pixels.iter().zip(&t.depth) {
                        if color >> 24 > 0 {
                            lo = lo.min(f32::from(depth));
                            hi = hi.max(f32::from(depth));
                            translucent |= color >> 24 < 255;
                        }
                    }
                    if !lo.is_finite() {
                        lo = 0.0;
                        hi = 0.0;
                    }
                    self.images.insert(
                        key,
                        Image {
                            slice,
                            depth: [lo, hi],
                            translucent,
                        },
                    );
                    self.dirty.insert(slice.page);
                }
                Draw::Mesh(draw) => {
                    for asset in draw.asset_meshes() {
                        let root = Arc::as_ptr(asset) as usize;
                        self.sources
                            .entry((1, root))
                            .or_insert_with(|| Source::Mesh(Arc::downgrade(asset)));
                    }
                    for (mesh, part, _, _, frame) in draw.posed_parts() {
                        let root = std::ptr::from_ref(mesh) as usize;
                        for batch in &mesh.parts[part].batches {
                            let Some(texture) = batch.texture else {
                                continue;
                            };
                            let frame = frame as usize % mesh.textures[texture].len();
                            let key = Key {
                                root,
                                texture,
                                frame,
                                kind: 1,
                            };
                            if self.images.contains_key(&key) {
                                continue;
                            }
                            let t = &mesh.textures[texture][frame];
                            let slice = self.atlas.insert(
                                t.width as u32,
                                t.height as u32,
                                &t.pixels,
                                None,
                            )?;
                            let translucent = t.pixels.iter().any(|c| c >> 24 > 0 && c >> 24 < 255);
                            self.images.insert(
                                key,
                                Image {
                                    slice,
                                    depth: [0.0; 2],
                                    translucent,
                                },
                            );
                            self.dirty.insert(slice.page);
                        }
                    }
                }
            }
        }
        Ok(())
    }
    fn collect_effects(&mut self, effects: &[crate::render::effects::Effect]) -> Result<()> {
        for effect in effects {
            effect.validate()?;
            let Some(t) = &effect.material.texture else {
                continue;
            };
            let root = Arc::as_ptr(t) as usize;
            let key = Key {
                root,
                texture: 0,
                frame: 0,
                kind: 2,
            };
            self.sources
                .entry((2, root))
                .or_insert_with(|| Source::Texture(Arc::downgrade(t)));
            if self.images.contains_key(&key) {
                continue;
            }
            let slice = self
                .atlas
                .insert(t.width as u32, t.height as u32, &t.pixels, None)?;
            self.images.insert(
                key,
                Image {
                    slice,
                    depth: [0.0; 2],
                    translucent: true,
                },
            );
            self.dirty.insert(slice.page);
        }
        Ok(())
    }
    pub fn effect(&self, texture: Option<&Arc<Texture>>) -> Image {
        texture.map_or(self.white, |t| {
            self.images[&Key {
                root: Arc::as_ptr(t) as usize,
                texture: 0,
                frame: 0,
                kind: 2,
            }]
        })
    }
    pub fn tile(&self, tile: &Arc<Tile>) -> Image {
        self.images[&Key {
            root: Arc::as_ptr(tile) as usize,
            texture: 0,
            frame: 0,
            kind: 0,
        }]
    }
    pub fn mesh(&self, mesh: &Mesh, texture: Option<usize>, frame: u32) -> Image {
        texture.map_or(self.white, |texture| {
            self.images[&Key {
                root: std::ptr::from_ref(mesh) as usize,
                texture,
                frame: frame as usize % mesh.textures[texture].len(),
                kind: 1,
            }]
        })
    }
    pub fn bind(&self, page: usize) -> &wgpu::BindGroup {
        &self.pages[page].bind
    }
    pub fn stats(&self) -> (usize, usize, usize) {
        (self.images.len(), self.pages.len(), self.atlas.bytes())
    }
}
