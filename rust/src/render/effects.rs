// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              effects.rs - Typed billboard, ribbon and quad submits    *
// *************************************************************************

//! Renderer inputs, independent of gameplay effect controllers. Immutable Arc
//! textures are leases; no raw handles or global texture registry are exposed.
use crate::assets::mesh::Texture;
use anyhow::{Result, ensure};
use std::sync::Arc;

#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub enum Blend {
    #[default]
    Alpha,
    Additive,
    AdditiveStraight,
    Premultiplied,
}
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub enum Depth {
    #[default]
    Test,
    None,
    Write,
}
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub enum Orientation {
    #[default]
    Screen,
    WorldXy,
}
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub enum Filter {
    Nearest,
    #[default]
    Linear,
}
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub enum Address {
    #[default]
    Clamp,
    Repeat,
}
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub enum DebugView {
    #[default]
    Normal,
    Solid,
    FullTexture,
    Frame,
    Wireframe,
}
#[derive(Clone, Debug, Default)]
pub struct Material {
    pub texture: Option<Arc<Texture>>,
    pub blend: Blend,
    pub depth: Depth,
    pub filter: Filter,
    pub address: Address,
    pub lit: bool,
    pub debug: DebugView,
}
#[derive(Clone, Debug)]
pub struct Billboard {
    pub position: [f32; 3],
    pub size: [f32; 2],
    pub rotation: f32,
    pub orientation: Orientation,
    pub color: [f32; 4],
    /// Normalized x,y,width,height within the source texture.
    pub uv: [f32; 4],
}
impl Default for Billboard {
    fn default() -> Self {
        Self {
            position: [0.0; 3],
            size: [1.0; 2],
            rotation: 0.0,
            orientation: Orientation::Screen,
            color: [1.0; 4],
            uv: [0.0, 0.0, 1.0, 1.0],
        }
    }
}
#[derive(Clone, Debug)]
pub struct Strip {
    pub ends: [[f32; 3]; 2],
    pub widths: [f32; 2],
    pub colors: [[f32; 4]; 2],
    pub u: [f32; 2],
    pub v: [f32; 2],
    pub swap_uv: bool,
}
#[derive(Clone, Debug)]
pub struct Quad {
    /// Top-left, top-right, bottom-left, bottom-right; triangle uses the first 3.
    pub positions: [[f32; 3]; 4],
    pub uv: [[f32; 2]; 4],
    pub colors: [[f32; 4]; 4],
    pub triangle: bool,
}
#[derive(Clone, Debug)]
pub struct Overlay {
    /// x,y,width,height in viewport pixels. Screen composition follows world FX.
    pub rect: [f32; 4],
    pub uv: [f32; 4],
    pub color: [f32; 4],
    /// Scissor x,y,width,height in viewport pixels.
    pub clip: Option<[u32; 4]>,
}
#[derive(Clone, Debug)]
pub enum Primitive {
    Billboard(Billboard),
    Strip(Strip),
    Quad(Quad),
    Overlay(Overlay),
}
#[derive(Clone, Debug)]
pub struct Effect {
    pub material: Material,
    pub primitive: Primitive,
}
impl Effect {
    pub fn validate(&self) -> Result<()> {
        if let Some(t) = &self.material.texture {
            ensure!(
                t.width > 0
                    && t.height > 0
                    && t.width <= 8188
                    && t.height <= 8188
                    && t.width.checked_mul(t.height) == Some(t.pixels.len()),
                "invalid effect texture"
            );
        }
        let finite = |values: &[f32]| values.iter().all(|v| v.is_finite() && v.abs() <= 1e8);
        let valid = match &self.primitive {
            Primitive::Billboard(p) => {
                finite(&p.position)
                    && finite(&p.size)
                    && p.size.iter().all(|v| *v >= 0.0)
                    && p.rotation.is_finite()
                    && finite(&p.color)
                    && finite(&p.uv)
            }
            Primitive::Strip(p) => {
                p.ends.iter().all(|p| finite(p))
                    && p.colors.iter().all(|p| finite(p))
                    && finite(&p.widths)
                    && p.widths.iter().all(|v| *v >= 0.0)
                    && finite(&p.u)
                    && finite(&p.v)
            }
            Primitive::Quad(p) => {
                p.positions.iter().all(|p| finite(p))
                    && p.colors.iter().all(|p| finite(p))
                    && p.uv.iter().all(|p| finite(p))
            }
            Primitive::Overlay(p) => {
                finite(&p.rect)
                    && p.rect[2] >= 0.0
                    && p.rect[3] >= 0.0
                    && finite(&p.color)
                    && finite(&p.uv)
            }
        };
        ensure!(valid, "non-finite or invalid effect geometry");
        Ok(())
    }
}
