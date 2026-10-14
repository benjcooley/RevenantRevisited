// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  canvas.rs - Scaled bitmap composition                *
// *************************************************************************

use crate::assets::{bitmap::Bitmap, font::Font};

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Rect {
    pub x: i32,
    pub y: i32,
    pub w: i32,
    pub h: i32,
}
impl Rect {
    pub const fn new(x: i32, y: i32, w: i32, h: i32) -> Self {
        Self { x, y, w, h }
    }
    pub fn contains(self, [x, y]: [f64; 2]) -> bool {
        x >= f64::from(self.x)
            && y >= f64::from(self.y)
            && x < f64::from(self.x + self.w)
            && y < f64::from(self.y + self.h)
    }
}

pub struct Canvas<'a> {
    pub viewport: [usize; 2],
    pub scale: f64,
    pub batch: &'a mut super::batch::Batch,
}
impl Canvas<'_> {
    pub fn fill(&mut self, rect: Rect, color: u32) {
        let [x0, y0, x1, y1] = self.bounds(rect);
        let batch = &mut self.batch;

        batch.overlay(
            None,
            crate::render::effects::Overlay {
                rect: [x0 as f32, y0 as f32, (x1 - x0) as f32, (y1 - y0) as f32],
                uv: [0.0, 0.0, 1.0, 1.0],
                color: super::batch::color(color),
                clip: None,
            },
        );
    }
    fn bounds(&self, r: Rect) -> [usize; 4] {
        let x = |n: i32| {
            (f64::from(n) * self.scale)
                .round()
                .clamp(0.0, self.viewport[0] as f64) as usize
        };
        let y = |n: i32| {
            (f64::from(n) * self.scale)
                .round()
                .clamp(0.0, self.viewport[1] as f64) as usize
        };
        [x(r.x), y(r.y), x(r.x + r.w), y(r.y + r.h)]
    }
    pub fn bitmap(&mut self, bitmap: &Bitmap, x: i32, y: i32) {
        let src = Rect::new(0, 0, bitmap.width as i32, bitmap.height as i32);
        self.blit(bitmap, src, Rect::new(x, y, src.w, src.h));
    }
    pub fn blit(&mut self, bitmap: &Bitmap, src: Rect, dst: Rect) {
        self.blit_mask(bitmap, src, dst, None);
    }
    fn blit_mask(&mut self, bitmap: &Bitmap, src: Rect, dst: Rect, tint: Option<u32>) {
        if dst.w <= 0 || dst.h <= 0 || src.w <= 0 || src.h <= 0 {
            return;
        }
        let [x0, y0, x1, y1] = self.bounds(dst);
        let batch = &mut self.batch;

        let texture = batch.texture(bitmap, tint.is_some());
        // Keep the continuous source mapping used by the reference blitter;
        // scissors provide its rounded destination bounds.
        batch.overlay(
            Some(texture),
            crate::render::effects::Overlay {
                rect: [
                    (f64::from(dst.x) * self.scale) as f32,
                    (f64::from(dst.y) * self.scale) as f32,
                    (f64::from(dst.w) * self.scale) as f32,
                    (f64::from(dst.h) * self.scale) as f32,
                ],
                uv: [
                    src.x as f32 / bitmap.width as f32,
                    src.y as f32 / bitmap.height as f32,
                    src.w as f32 / bitmap.width as f32,
                    src.h as f32 / bitmap.height as f32,
                ],
                color: super::batch::color(tint.map_or(0xffffffff, |t| 0xff000000 | t)),
                clip: Some([x0 as u32, y0 as u32, (x1 - x0) as u32, (y1 - y0) as u32]),
            },
        );
    }
    pub fn text(&mut self, font: &Font, text: &str, x: i32, top: i32, max_width: i32) {
        let mut pen = x;
        for ch in text.chars() {
            if pen + font.advance(ch) > x + max_width {
                break;
            }
            if let Some(glyph) = font.glyph(ch) {
                let src = Rect::new(0, 0, glyph.bitmap.width as i32, glyph.bitmap.height as i32);
                self.blit_mask(
                    &glyph.bitmap,
                    src,
                    Rect::new(
                        pen + glyph.offset[0],
                        top + font.height + glyph.offset[1],
                        src.w,
                        src.h,
                    ),
                    Some(0xddcead),
                );
            }
            pen += font.advance(ch);
        }
    }
    pub fn border(&mut self, r: Rect, color: u32) {
        for edge in [
            Rect::new(r.x, r.y, r.w, 1),
            Rect::new(r.x, r.y + r.h - 1, r.w, 1),
            Rect::new(r.x, r.y, 1, r.h),
            Rect::new(r.x + r.w - 1, r.y, 1, r.h),
        ] {
            self.fill(edge, color);
        }
    }
}
