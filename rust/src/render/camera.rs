// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              camera.rs - Isometric projection and camera              *
// *************************************************************************

use crate::world::sector::Position;

/// Retail's fixed-point approximation of cos(30 degrees).
pub const ISO_COS: f64 = 867.0 / 1000.0;

pub fn project(position: Position) -> [f64; 2] {
    let x = i64::from(position.x);
    let y = i64::from(position.y);
    [
        (x - y) as f64,
        ((x + y) / 2 - i64::from(position.z) * 867 / 1000) as f64,
    ]
}

pub fn world_depth(position: Position) -> f64 {
    -(f64::from(position.x) + f64::from(position.y)) * ISO_COS - f64::from(position.z) * 0.5
}

/// Inverse of the desktop presenter's aspect-ratio fit, including letterboxing.
pub fn viewport_cursor(
    point: [f64; 2],
    window: [usize; 2],
    buffer: [usize; 2],
) -> Option<[f64; 2]> {
    if window.contains(&0) || buffer.contains(&0) {
        return None;
    }
    let scale = (window[0] as f64 / buffer[0] as f64).min(window[1] as f64 / buffer[1] as f64);
    let x = (point[0] - (window[0] as f64 - buffer[0] as f64 * scale) * 0.5) / scale;
    let y = (point[1] - (window[1] as f64 - buffer[1] as f64 * scale) * 0.5) / scale;
    (x >= 0.0 && y >= 0.0 && x < buffer[0] as f64 && y < buffer[1] as f64).then_some([x, y])
}

#[derive(Debug, Clone, Copy)]
pub struct Camera {
    /// Center in projected, unscaled world pixels.
    pub center: [f64; 2],
    pub zoom: f64,
}

impl Camera {
    pub fn new(position: Position) -> Self {
        Self {
            center: project(position),
            zoom: 1.0,
        }
    }

    pub fn pan(&mut self, dx: f64, dy: f64) {
        self.center[0] += dx / self.zoom;
        self.center[1] += dy / self.zoom;
    }

    pub fn screen(&self, point: [f64; 2], viewport: [usize; 2]) -> [f64; 2] {
        [
            (point[0] - self.center[0]) * self.zoom + viewport[0] as f64 * 0.5,
            (point[1] - self.center[1]) * self.zoom + viewport[1] as f64 * 0.5,
        ]
    }

    pub fn projected(&self, screen: [f64; 2], viewport: [usize; 2]) -> [f64; 2] {
        [
            (screen[0] - viewport[0] as f64 * 0.5) / self.zoom + self.center[0],
            (screen[1] - viewport[1] as f64 * 0.5) / self.zoom + self.center[1],
        ]
    }

    /// Keep the world point under the cursor stable when zooming.
    pub fn zoom_at(&mut self, factor: f64, cursor: [f64; 2], viewport: [usize; 2]) {
        let before = self.projected(cursor, viewport);
        self.zoom = (self.zoom * factor).clamp(0.125, 4.0);
        let after = self.projected(cursor, viewport);
        self.center[0] += before[0] - after[0];
        self.center[1] += before[1] - after[1];
    }

    pub fn fit(&mut self, bounds: [f64; 4], viewport: [usize; 2]) {
        self.center = [(bounds[0] + bounds[2]) * 0.5, (bounds[1] + bounds[3]) * 0.5];
        self.zoom = ((viewport[0] as f64 - 48.0) / (bounds[2] - bounds[0]).max(1.0))
            .min((viewport[1] as f64 - 48.0) / (bounds[3] - bounds[1]).max(1.0))
            .clamp(0.125, 4.0);
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn resized_window_picking_excludes_letterbox_bars() {
        assert_eq!(
            viewport_cursor([400.0, 200.0], [800, 400], [200, 200]),
            Some([100.0, 100.0])
        );
        assert_eq!(viewport_cursor([50.0, 200.0], [800, 400], [200, 200]), None);
        assert_eq!(viewport_cursor([0.0, 0.0], [0, 0], [200, 200]), None);
    }
    #[test]
    fn projection_and_cursor_anchored_zoom() {
        assert_eq!(project(Position { x: -3, y: 0, z: -3 }), [-3.0, 1.0]);
        assert_eq!(
            project(Position {
                x: 100,
                y: 60,
                z: 0
            }),
            [40.0, 80.0]
        );
        let mut camera = Camera::new(Position {
            x: 2500,
            y: 26000,
            z: 340,
        });
        let viewport = [1024, 768];
        let cursor = [50.0, 620.0];
        let before = camera.projected(cursor, viewport);
        camera.zoom_at(2.0, cursor, viewport);
        let after = camera.projected(cursor, viewport);
        for i in 0..2 {
            assert!((before[i] - after[i]).abs() < 1e-9);
        }
    }
}
