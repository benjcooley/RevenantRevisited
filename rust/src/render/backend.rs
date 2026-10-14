// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  backend.rs - Application GPU rendering              *
// *************************************************************************

use super::{
    Draw, FrameStats, Framebuffer,
    camera::Camera,
    effects::Effect,
    gpu::{Config, Renderer},
    lighting::{Lighting, View},
};
use anyhow::Result;

pub trait PreviewRenderer {
    fn preview(
        &mut self,
        frame: &mut Framebuffer,
        draws: &[Draw],
        camera: &Camera,
    ) -> Result<FrameStats>;
}
pub struct Backend {
    pub renderer: Renderer,
}
impl Backend {
    pub fn new(config: Config) -> Result<Self> {
        Ok(Self {
            renderer: Renderer::new(config)?,
        })
    }
    pub fn render(
        &mut self,
        frame: &mut Framebuffer,
        draws: &[Draw],
        camera: &Camera,
        lighting: &Lighting,
        depth: bool,
        effects: &[Effect],
    ) -> Result<FrameStats> {
        let mut lighting = lighting.clone();
        if depth {
            lighting.view = View::Depth;
        }
        self.renderer
            .render_with_effects(frame, draws, camera, &lighting, effects)
    }
}
impl PreviewRenderer for Backend {
    fn preview(
        &mut self,
        frame: &mut Framebuffer,
        draws: &[Draw],
        camera: &Camera,
    ) -> Result<FrameStats> {
        self.renderer.render_preview(frame, draws, camera)
    }
}
