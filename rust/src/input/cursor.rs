// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 cursor.rs - Native Revenant mouse sprites             *
// *************************************************************************

use crate::{
    assets::{bitmap::Bitmap, multi::Multi, vfs::Vfs},
    world::movement::input_facing,
};
use anyhow::{Context, Result};

#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub enum CursorKind {
    #[default]
    Default,
    Hand,
    /// World direction, matching the walk preview's movement input.
    Walk([i8; 2]),
}

/// Native pixels and registration points. These are owned independently of the
/// archive so pointer rendering performs no decoding or disk access per frame.
pub struct CursorSet {
    pointer: Bitmap,
    pointer_shadow: Bitmap,
    hand: Bitmap,
    wedges: Vec<Bitmap>,
    wedge_shadows: Vec<Bitmap>,
}

impl CursorSet {
    pub fn load(vfs: &mut Vfs) -> Result<Self> {
        let bytes = vfs.read("playscrn.dat").context("loading game cursors")?;
        let multi = Multi::parse(&bytes)?;
        let pointer = multi.bitmap("cursor")?;
        let pointer_shadow = multi.bitmap_alias("cursorshadow")?;
        let hand = multi.bitmap("handcursor")?;
        let wedges = ["ne", "e", "se", "s", "sw", "w", "nw", "n"]
            .into_iter()
            .map(|direction| multi.bitmap(&format!("wedge-{direction}")))
            .collect::<Result<Vec<_>>>()?;
        let wedge_shadows = ["ne", "e", "se", "s", "sw", "w", "nw", "n"]
            .into_iter()
            .map(|direction| multi.bitmap_alias(&format!("wedge-{direction}shadow")))
            .collect::<Result<Vec<_>>>()?;
        Ok(Self {
            pointer,
            pointer_shadow,
            hand,
            wedges,
            wedge_shadows,
        })
    }

    pub fn record(
        &self,
        batch: &mut crate::ui::batch::Batch,
        point: [f64; 2],
        kind: CursorKind,
        scale: f64,
    ) {
        let wedge = match kind {
            CursorKind::Walk(d) => input_facing(d).map(|f| usize::from(f / 32)),
            _ => None,
        };
        let bitmap = if let Some(wedge) = wedge {
            batch.cursor(
                &self.wedge_shadows[wedge],
                [point[0], point[1] + 43.0 * scale],
                scale,
            );
            &self.wedges[wedge]
        } else if kind == CursorKind::Hand {
            &self.hand
        } else {
            batch.cursor(
                &self.pointer_shadow,
                [point[0] + scale, point[1] + 3.0 * scale],
                scale,
            );
            &self.pointer
        };
        batch.cursor(bitmap, point, scale);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    #[ignore = "requires retail assets; writes captures/cursors.png"]
    fn retail_cursor_artwork_decodes_and_renders() -> Result<()> {
        let root = Vfs::discover(None)?;
        let mut vfs = Vfs::open(root, "Ahkuilon", None)?;
        let cursors = CursorSet::load(&mut vfs)?;
        for bitmap in [&cursors.pointer, &cursors.hand]
            .into_iter()
            .chain(&cursors.wedges)
        {
            assert!(bitmap.width > 0 && bitmap.height > 0);
            assert!(bitmap.pixels.iter().any(|p| p >> 24 != 0));
            assert!(bitmap.pixels.iter().any(|p| p >> 24 == 0));
        }
        for shadow in [&cursors.pointer_shadow]
            .into_iter()
            .chain(&cursors.wedge_shadows)
        {
            assert!(shadow.pixels.iter().any(|p| (1..255).contains(&(p >> 24))));
        }
        use crate::render::{Framebuffer, camera::Camera, effects::Overlay, gpu::Renderer};
        let mut frame = Framebuffer::new(640, 240)?;
        let mut batch = crate::ui::batch::Batch::default();
        batch.overlay(
            None,
            Overlay {
                rect: [0.0, 0.0, 640.0, 240.0],
                uv: [0.0, 0.0, 1.0, 1.0],
                color: [0.204, 0.22, 0.235, 1.0],
                clip: None,
            },
        );
        cursors.record(&mut batch, [40.0, 50.0], CursorKind::Default, 1.0);
        cursors.record(&mut batch, [100.0, 50.0], CursorKind::Hand, 1.0);
        for (i, direction) in [
            [0, -1],
            [1, -1],
            [1, 0],
            [1, 1],
            [0, 1],
            [-1, 1],
            [-1, 0],
            [-1, -1],
        ]
        .into_iter()
        .enumerate()
        {
            cursors.record(
                &mut batch,
                [40.0 + i as f64 * 75.0, 170.0],
                CursorKind::Walk(direction),
                1.0,
            );
        }
        Renderer::new(Default::default())?.render_with_effects(
            &mut frame,
            &[],
            &Camera::new(Default::default()),
            &Default::default(),
            &batch.effects,
        )?;
        frame.save_png(std::path::Path::new("captures/cursors.png"))?;
        Ok(())
    }
}
