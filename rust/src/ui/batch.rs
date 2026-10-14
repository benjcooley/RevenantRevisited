// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   batch.rs - Cached GPU UI draw commands             *
// *************************************************************************

use crate::{
    assets::{bitmap::Bitmap, mesh::Texture},
    render::effects::{Address, Depth, Effect, Filter, Material, Overlay, Primitive},
};
use std::{
    collections::{BTreeMap, hash_map::DefaultHasher},
    hash::{Hash, Hasher},
    sync::Arc,
};

struct Entry {
    texture: Arc<Texture>,
    touched: u64,
}
/// Bitmap content keys let transient paper-doll frames reuse textures too.
/// The cache is bounded independently of renderer-owned atlas pages.
#[derive(Default)]
pub struct Batch {
    pub effects: Vec<Effect>,
    images: BTreeMap<(u64, usize, usize, bool), Entry>,
    clock: u64,
}
impl Batch {
    pub fn begin(&mut self) {
        self.effects.clear();
        self.clock = self.clock.wrapping_add(1);
        let mut bytes: usize = self
            .images
            .values()
            .map(|e| e.texture.pixels.len() * 4)
            .sum();
        if bytes > 32 * 1024 * 1024 {
            let mut stale: Vec<_> = self.images.iter().map(|(k, e)| (e.touched, *k)).collect();
            stale.sort();
            for (_, key) in stale {
                if bytes <= 24 * 1024 * 1024 {
                    break;
                }
                if let Some(e) = self.images.remove(&key) {
                    bytes -= e.texture.pixels.len() * 4;
                }
            }
        }
    }
    pub fn texture(&mut self, bitmap: &Bitmap, mask: bool) -> Arc<Texture> {
        let mut hash = DefaultHasher::new();
        bitmap.pixels.hash(&mut hash);
        let key = (hash.finish(), bitmap.width, bitmap.height, mask);
        let entry = self.images.entry(key).or_insert_with(|| Entry {
            texture: Arc::new(Texture {
                lighting: Default::default(),
                width: bitmap.width,
                height: bitmap.height,
                pixels: bitmap
                    .pixels
                    .iter()
                    .map(|&p| {
                        if mask {
                            let coverage = (p & 255).max(p >> 8 & 255).max(p >> 16 & 255);
                            (((p >> 24) * coverage / 255) << 24) | 0xffffff
                        } else {
                            p
                        }
                    })
                    .collect(),
            }),
            touched: self.clock,
        });
        entry.touched = self.clock;
        entry.texture.clone()
    }
    pub fn overlay(&mut self, texture: Option<Arc<Texture>>, overlay: Overlay) {
        self.effects.push(Effect {
            material: Material {
                texture,
                depth: Depth::None,
                filter: Filter::Nearest,
                address: Address::Clamp,
                ..Default::default()
            },
            primitive: Primitive::Overlay(overlay),
        });
    }
    pub fn cursor(&mut self, bitmap: &Bitmap, point: [f64; 2], scale: f64) {
        if !scale.is_finite() || scale <= 0.0 || !point.iter().all(|p| p.is_finite()) {
            return;
        }
        let texture = self.texture(bitmap, false);
        self.overlay(
            Some(texture),
            Overlay {
                rect: [
                    (point[0] - f64::from(bitmap.registration[0]) * scale) as f32,
                    (point[1] - f64::from(bitmap.registration[1]) * scale) as f32,
                    (bitmap.width as f64 * scale) as f32,
                    (bitmap.height as f64 * scale) as f32,
                ],
                uv: [0.0, 0.0, 1.0, 1.0],
                color: [1.0; 4],
                clip: None,
            },
        );
    }
}
pub(super) fn color(argb: u32) -> [f32; 4] {
    [
        ((argb >> 16) & 255) as f32 / 255.0,
        ((argb >> 8) & 255) as f32 / 255.0,
        (argb & 255) as f32 / 255.0,
        (argb >> 24) as f32 / 255.0,
    ]
}
