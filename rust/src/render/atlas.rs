// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  atlas.rs - Deterministic texture pages               *
// *************************************************************************

//! Paged shelf packing. Extruded borders isolate filtering; mesh UV repeat is
//! applied inside the slice, never across the surrounding page.
use anyhow::{Result, ensure};

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Slice {
    pub page: usize,
    pub x: u32,
    pub y: u32,
    pub width: u32,
    pub height: u32,
}
pub struct Page {
    pub width: u32,
    pub height: u32,
    pub color: Vec<u32>,
    pub depth: Vec<f32>,
    x: u32,
    y: u32,
    row: u32,
}
pub struct Atlas {
    pub pages: Vec<Page>,
    size: u32,
    padding: u32,
    max_bytes: usize,
}
impl Atlas {
    pub fn new(size: u32, max_bytes: usize) -> Result<Self> {
        ensure!((16..=8192).contains(&size), "invalid atlas page size");
        Ok(Self {
            pages: Vec::new(),
            size,
            padding: 2,
            max_bytes,
        })
    }
    pub fn bytes(&self) -> usize {
        self.pages.iter().map(|p| p.color.len() * 8).sum()
    }
    pub fn insert(
        &mut self,
        width: u32,
        height: u32,
        pixels: &[u32],
        depth: Option<&[i16]>,
    ) -> Result<Slice> {
        ensure!(
            width > 0 && height > 0 && width <= 8192 && height <= 8192,
            "invalid atlas image size"
        );
        let count = width as usize * height as usize;
        ensure!(
            pixels.len() == count && depth.is_none_or(|d| d.len() == count),
            "atlas plane size mismatch"
        );
        let p = self.padding;
        let w = width + 2 * p;
        let h = height + 2 * p;
        let mut location = None;
        for (index, page) in self.pages.iter_mut().enumerate() {
            let (mut x, mut y, mut row) = (page.x, page.y, page.row);
            if x + w > page.width {
                x = 0;
                y += row;
                row = 0;
            }
            if x + w <= page.width && y + h <= page.height {
                page.x = x + w;
                page.y = y;
                page.row = row.max(h);
                location = Some((index, x + p, y + p));
                break;
            }
        }
        let (page, x, y) = if let Some(found) = location {
            found
        } else {
            let size = self.size.max(w).max(h).next_power_of_two();
            ensure!(
                size <= 8192,
                "padded atlas image exceeds maximum page dimensions"
            );
            let bytes = size as usize * size as usize * 8;
            ensure!(
                self.bytes() + bytes <= self.max_bytes,
                "texture atlas budget exceeded ({} bytes)",
                self.max_bytes
            );
            let index = self.pages.len();
            self.pages.push(Page {
                width: size,
                height: size,
                color: vec![0; size as usize * size as usize],
                depth: vec![0.0; size as usize * size as usize],
                x: w,
                y: 0,
                row: h,
            });
            (index, p, p)
        };
        let target = &mut self.pages[page];
        for dy in 0..h {
            for dx in 0..w {
                let sx = dx.saturating_sub(p).min(width - 1);
                let sy = dy.saturating_sub(p).min(height - 1);
                let source = (sy * width + sx) as usize;
                let destination = ((y + dy - p) * target.width + x + dx - p) as usize;
                target.color[destination] = pixels[source];
                target.depth[destination] = depth.map_or(0.0, |d| f32::from(d[source]));
            }
        }
        Ok(Slice {
            page,
            x,
            y,
            width,
            height,
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn pages_extrude_borders_keep_signed_depth_and_reject_over_budget() {
        let mut atlas = Atlas::new(16, 16 * 16 * 8 * 2).unwrap();
        let a = atlas
            .insert(2, 1, &[0xff112233, 0x80aabbcc], Some(&[-8, 17]))
            .unwrap();
        let page = &atlas.pages[a.page];
        assert_eq!(page.color[0], 0xff112233);
        assert_eq!(page.depth[0], -8.0);
        assert_eq!(page.color[5], 0x80aabbcc);
        let b = atlas.insert(12, 12, &[0; 144], None).unwrap();
        assert_eq!(b.page, 1);
        assert!(atlas.insert(12, 12, &[0; 144], None).is_err());
        assert_eq!(atlas.pages.len(), 2);
    }
}
