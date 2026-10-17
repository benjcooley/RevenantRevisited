// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              mod.rs - Pointer capture and movement controls          *
// *************************************************************************

//! Platform-independent input edges. A press belongs to the surface on which
//! it began, and loses capture if focus or the drawable viewport is lost.

pub mod cursor;

use crate::{render::camera::Camera, world::sector::Position};

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Target {
    World,
    Ui,
}

#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub struct Button {
    pub target: Option<Target>,
    pub pressed: bool,
    pub held: bool,
    pub released: bool,
    /// A lost focus/viewport release clears interaction without activating it.
    pub cancelled: bool,
}

impl Button {
    pub fn pressed_on(self, target: Target) -> bool {
        self.pressed && self.target == Some(target)
    }

    pub fn released_on(self, target: Target) -> bool {
        self.released && !self.cancelled && self.target == Some(target)
    }
}

#[derive(Clone, Copy, Debug, Default)]
pub struct PointerFrame {
    pub position: Option<[f64; 2]>,
    pub hover: Option<Target>,
    /// Movement in framebuffer pixels, zero after focus/viewport loss.
    pub delta: [f64; 2],
    pub left: Button,
    pub right: Button,
    pub middle: Button,
}

impl PointerFrame {
    /// Held world controls pause over UI, and a UI press cannot drive them.
    pub fn held_over(self, button: Button, target: Target) -> bool {
        button.held && button.target == Some(target) && self.hover == Some(target)
    }
}

#[derive(Default)]
pub struct Pointer {
    down: [bool; 3],
    capture: [Option<Target>; 3],
    position: Option<[f64; 2]>,
}

impl Pointer {
    /// A screen transition cancels capture but retains physical button state.
    /// Holding a button across a modal boundary cannot create a world press.
    pub fn cancel_capture(&mut self) {
        self.capture = [None; 3];
        self.position = None;
    }

    /// `down` is left/right/middle. Map window coordinates into the framebuffer
    /// with `render::camera::viewport_cursor` before updating this state.
    pub fn update(
        &mut self,
        position: Option<[f64; 2]>,
        down: [bool; 3],
        active: bool,
        over_ui: bool,
    ) -> PointerFrame {
        let position = position.filter(|p| active && p.iter().all(|v| v.is_finite()));
        let hover = position.map(|_| if over_ui { Target::Ui } else { Target::World });
        let delta = match (position, self.position) {
            (Some(current), Some(previous)) => [current[0] - previous[0], current[1] - previous[1]],
            _ => [0.0; 2],
        };
        let mut buttons = [Button::default(); 3];
        for i in 0..3 {
            let previous_capture = self.capture[i];
            if hover.is_none() {
                buttons[i] = Button {
                    target: previous_capture,
                    released: previous_capture.is_some(),
                    cancelled: previous_capture.is_some(),
                    ..Button::default()
                };
                self.capture[i] = None;
            } else if down[i] && !self.down[i] {
                self.capture[i] = hover;
                buttons[i] = Button {
                    target: hover,
                    pressed: true,
                    held: true,
                    ..Button::default()
                };
            } else if !down[i] && self.down[i] {
                buttons[i] = Button {
                    target: previous_capture,
                    released: previous_capture.is_some(),
                    ..Button::default()
                };
                self.capture[i] = None;
            } else {
                buttons[i] = Button {
                    target: previous_capture,
                    held: down[i] && previous_capture.is_some(),
                    ..Button::default()
                };
            }
        }
        // Keep raw button state even while inactive. Re-entering with a held
        // button must not synthesize a new click or restart movement.
        self.down = down;
        self.position = position;
        PointerFrame {
            position,
            hover,
            delta,
            left: buttons[0],
            right: buttons[1],
            middle: buttons[2],
        }
    }
}

/// Screen-relative WASD directions, inverted through the isometric projection.
pub fn keyboard_direction([horizontal, vertical]: [i8; 2]) -> [i8; 2] {
    let horizontal = horizontal.signum();
    let vertical = vertical.signum();
    [
        (vertical + horizontal).signum(),
        (vertical - horizontal).signum(),
    ]
}

/// Right-button steering uses the native mouse plane 50 world units above the
/// actor's feet. The angle selects one of the same eight movement directions
/// used by keyboard input; it does not start a persistent pathfinding command.
pub fn pointer_direction(
    camera: &Camera,
    viewport: [usize; 2],
    actor: Position,
    pointer: [f64; 2],
    radius: i32,
) -> [i8; 2] {
    if viewport.contains(&0)
        || !camera.zoom.is_finite()
        || camera.zoom <= 0.0
        || !pointer.iter().all(|v| v.is_finite())
    {
        return [0; 2];
    }
    let projected = camera.projected(pointer, viewport);
    let height = (i64::from(actor.z) + 50) * 867 / 1000;
    let target_x = projected[1] + height as f64 + projected[0] * 0.5;
    let target_y = projected[1] + height as f64 - projected[0] * 0.5;
    let dx = target_x - f64::from(actor.x);
    let dy = target_y - f64::from(actor.y);
    let radius = f64::from(radius.max(1));
    if !dx.is_finite() || !dy.is_finite() || (dx.abs() < radius && dy.abs() < radius) {
        return [0; 2];
    }
    let facing = native_facing([dx as i64, dy as i64]);
    match facing.wrapping_add(16) / 32 {
        0 => [0, -1],
        1 => [1, -1],
        2 => [1, 0],
        3 => [1, 1],
        4 => [0, 1],
        5 => [-1, 1],
        6 => [-1, 0],
        _ => [-1, -1],
    }
}

/// ConvertToFacing reduces coordinates to the 256-square AngleTable, then
/// applies its rounded byte angle. Keep that quantization at octant edges.
fn native_facing([dx, dy]: [i64; 2]) -> u8 {
    let mut x = dx.unsigned_abs();
    let mut y = dy.unsigned_abs();
    while x > 255 || y > 255 {
        x >>= 1;
        y >>= 1;
    }
    let table_angle = if x == 0 {
        64
    } else {
        ((y as f64).atan2(x as f64) * 128.0 / std::f64::consts::PI).round() as i32
    };
    let angle = 64 - table_angle;
    let angle = if dx < 0 && dy > 0 {
        angle + 128
    } else if dx < 0 {
        256 - angle
    } else if dy > 0 {
        128 - angle
    } else {
        angle
    };
    angle as u8
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn screen_transition_requires_a_fresh_press() {
        let mut pointer = Pointer::default();
        let point = Some([10.0; 2]);
        pointer.update(point, [true; 3], true, true);
        pointer.cancel_capture();
        let held = pointer.update(point, [true; 3], true, false);
        for button in [held.left, held.right, held.middle] {
            assert!(!button.held && !button.pressed && !button.released);
            assert_eq!(button.target, None);
        }
        let release = pointer.update(point, [false; 3], true, false);
        assert!(!release.left.released);
        let fresh = pointer.update(point, [true; 3], true, false);
        assert!(fresh.left.pressed_on(Target::World));
        assert!(fresh.held_over(fresh.right, Target::World));
    }

    #[test]
    fn press_capture_prevents_click_through_and_keeps_button_edges_separate() {
        let mut pointer = Pointer::default();
        let p = Some([20.0, 30.0]);
        let ui_press = pointer.update(p, [true, false, false], true, true);
        assert!(ui_press.left.pressed_on(Target::Ui));
        let drag = pointer.update(Some([40.0, 50.0]), [true, true, false], true, false);
        assert_eq!(drag.delta, [20.0, 20.0]);
        assert!(!drag.held_over(drag.left, Target::World));
        assert!(drag.right.pressed_on(Target::World));
        let release = pointer.update(p, [false, true, false], true, false);
        assert!(release.left.released_on(Target::Ui));
        assert!(release.held_over(release.right, Target::World));
        let pause = pointer.update(p, [false, true, false], true, true);
        assert!(!pause.held_over(pause.right, Target::World));
        assert!(!pause.right.pressed);
    }

    #[test]
    fn focus_and_viewport_loss_cancel_until_a_fresh_press() {
        for (active, position) in [(false, Some([5.0; 2])), (true, None)] {
            let mut pointer = Pointer::default();
            pointer.update(Some([5.0; 2]), [true; 3], true, false);
            let lost = pointer.update(position, [true; 3], active, false);
            for button in [lost.left, lost.right, lost.middle] {
                assert!(button.cancelled && button.released);
                assert!(!button.released_on(Target::World));
            }
            let restored = pointer.update(Some([7.0; 2]), [true; 3], true, false);
            assert!(!restored.left.held && !restored.right.pressed);
            assert_eq!(restored.delta, [0.0; 2]);
            pointer.update(Some([7.0; 2]), [false; 3], true, false);
            assert!(
                pointer
                    .update(Some([7.0; 2]), [true; 3], true, false)
                    .left
                    .pressed
            );
        }
    }

    #[test]
    fn keyboard_and_mouse_directions_follow_the_same_projection_at_any_zoom() {
        assert_eq!(keyboard_direction([1, 0]), [1, -1]);
        assert_eq!(keyboard_direction([0, -1]), [-1, -1]);
        assert_eq!(keyboard_direction([-1, 1]), [0, 1]);
        let actor = Position {
            x: 2500,
            y: 26000,
            z: 192,
        };
        let viewport = [640, 480];
        for zoom in [0.125, 1.0, 4.0] {
            let mut camera = Camera::new(actor);
            camera.zoom = zoom;
            for direction in [
                [0, -1],
                [1, -1],
                [1, 0],
                [1, 1],
                [0, 1],
                [-1, 1],
                [-1, 0],
                [-1, -1],
                [0, 0],
            ] {
                let target = Position {
                    x: actor.x + i32::from(direction[0]) * 100,
                    y: actor.y + i32::from(direction[1]) * 100,
                    z: actor.z + 50,
                };
                let pixel = camera.screen(crate::render::camera::project(target), viewport);
                assert_eq!(
                    pointer_direction(&camera, viewport, actor, pixel, 16),
                    direction
                );
            }
        }
    }

    #[test]
    fn mouse_octant_boundary_keeps_native_byte_angle_rounding() {
        // AngleTable rounds the first quadrant to a byte before the movement
        // code adds 16 and masks; directly rounding atan2 to octants differs.
        assert_eq!(native_facing([100, -43]), 47);
        assert_eq!(native_facing([100, -42]), 48);
        assert_eq!(native_facing([-100, 43]), 175);
        assert_eq!(native_facing([10000, -4300]), 47);
    }

    #[test]
    fn steering_uses_the_hud_world_viewport_after_sidebar_or_scale_changes() {
        use crate::{render::camera::project, ui::Layout};

        let actor = Position {
            x: 2500,
            y: 26000,
            z: 192,
        };
        for viewport in [[640, 480], [320, 240], [1024, 768]] {
            for (visible, sidebar) in [(true, true), (true, false), (false, false)] {
                let layout = Layout::new(viewport, visible, sidebar);
                let camera = Camera::new(actor);
                let rendering_camera = layout.world_camera(camera, viewport);
                for direction in [[1, -1], [-1, 1], [0, -1], [0, 0]] {
                    let target = Position {
                        x: actor.x + i32::from(direction[0]) * 25,
                        y: actor.y + i32::from(direction[1]) * 25,
                        z: actor.z + 50,
                    };
                    let point = rendering_camera.screen(project(target), viewport);
                    assert_eq!(
                        pointer_direction(&camera, layout.world_viewport(), actor, point, 16),
                        direction
                    );
                }
            }
        }
    }
}
