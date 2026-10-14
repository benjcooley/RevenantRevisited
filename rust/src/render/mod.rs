// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                mod.rs - GPU frame readback and picking                *
// *************************************************************************

//! Render submissions and GPU capture buffers. No software rasterization.
pub mod atlas;
pub mod backend;
pub mod camera;
pub mod effects;
pub mod equipment;
pub mod gpu;
pub mod lighting;
pub mod mesh;

use crate::{assets::imagery::Tile, world::sector::Position};
use anyhow::{Result, ensure};
use camera::project;
use std::{fs::File, io::BufWriter, path::Path, sync::Arc};

pub enum Draw {
    Tile(TileDraw),
    Mesh(mesh::MeshDraw),
}

impl Draw {
    pub fn bounds(&self) -> [f64; 4] {
        match self {
            Self::Tile(draw) => draw.bounds(),
            Self::Mesh(draw) => draw.bounds(),
        }
    }
    pub fn position(&self) -> Position {
        match self {
            Self::Tile(draw) => draw.position,
            Self::Mesh(draw) => draw.position,
        }
    }
}

pub struct TileDraw {
    pub tile: Arc<Tile>,
    pub position: Position,
    pub flipped: bool,
}

impl TileDraw {
    pub fn bounds(&self) -> [f64; 4] {
        let anchor = project(self.position);
        let reg = self.tile.state.registration;
        let reg_x = if self.flipped {
            self.tile.width as f64 - f64::from(reg[0])
        } else {
            f64::from(reg[0])
        };
        let left = anchor[0] - reg_x;
        let top = anchor[1] - f64::from(reg[1]);
        [
            left,
            top,
            left + self.tile.width as f64,
            top + self.tile.height as f64,
        ]
    }
}

pub struct Framebuffer {
    pub width: usize,
    pub height: usize,
    pub pixels: Vec<u32>,
    depth: Vec<f64>,
    owners: Vec<Option<usize>>,
}

#[derive(Default, Debug)]
pub struct FrameStats {
    pub visible_draws: usize,
    pub covered_pixels: usize,
}

impl Framebuffer {
    pub fn new(width: usize, height: usize) -> Result<Self> {
        ensure!(
            (64..=4096).contains(&width) && (64..=4096).contains(&height),
            "viewport must be between 64 and 4096 pixels per side"
        );
        let size = width * height;
        Ok(Self {
            width,
            height,
            pixels: vec![0; size],
            depth: vec![f64::INFINITY; size],
            owners: vec![None; size],
        })
    }

    pub fn viewport(&self) -> [usize; 2] {
        [self.width, self.height]
    }
    pub fn pick(&self, x: usize, y: usize) -> Option<usize> {
        if x >= self.width || y >= self.height {
            return None;
        }
        self.owners[y * self.width + x]
    }

    pub fn save_png(&self, path: &Path) -> Result<()> {
        if let Some(parent) = path.parent().filter(|p| !p.as_os_str().is_empty()) {
            std::fs::create_dir_all(parent)?;
        }
        let file = BufWriter::new(File::create(path)?);
        let mut encoder = png::Encoder::new(file, self.width as u32, self.height as u32);
        encoder.set_color(png::ColorType::Rgb);
        encoder.set_depth(png::BitDepth::Eight);
        let mut writer = encoder.write_header()?;
        let bytes: Vec<_> = self
            .pixels
            .iter()
            .flat_map(|p| [(p >> 16) as u8, (p >> 8) as u8, *p as u8])
            .collect();
        writer.write_image_data(&bytes)?;
        writer.finish()?;
        Ok(())
    }
}
