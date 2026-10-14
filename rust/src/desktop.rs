// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 desktop.rs - Native events and GPU presentation      *
// *************************************************************************

use anyhow::{Context, Result};
use revenant::render::gpu::Renderer;
use std::{
    collections::BTreeSet,
    sync::Arc,
    time::{Duration, Instant},
};
use winit::{
    application::ApplicationHandler,
    dpi::LogicalSize,
    event::{ElementState, MouseScrollDelta, WindowEvent},
    event_loop::{ActiveEventLoop, EventLoop},
    keyboard::PhysicalKey,
    platform::pump_events::{EventLoopExtPumpEvents, PumpStatus},
    window::{Window, WindowAttributes, WindowId},
};
pub use winit::{event::MouseButton, keyboard::KeyCode as Key};

struct State {
    window: Option<Arc<Window>>,
    attributes: WindowAttributes,
    open: bool,
    focused: bool,
    keys: BTreeSet<Key>,
    pressed: BTreeSet<Key>,
    mouse: [bool; 3],
    position: Option<[f64; 2]>,
    scroll: [f32; 2],
    error: Option<anyhow::Error>,
}
struct Pump<'a> {
    state: &'a mut State,
    renderer: Option<&'a mut Renderer>,
}
impl ApplicationHandler for Pump<'_> {
    fn resumed(&mut self, events: &ActiveEventLoop) {
        if self.state.window.is_none() {
            match events.create_window(self.state.attributes.clone()) {
                Ok(window) => {
                    self.state.focused = window.has_focus();
                    self.state.window = Some(Arc::new(window));
                }
                Err(error) => self.state.error = Some(error.into()),
            }
        }
    }
    fn suspended(&mut self, _: &ActiveEventLoop) {
        self.state.focused = false;
        self.state.keys.clear();
        self.state.mouse = [false; 3];
    }
    fn window_event(&mut self, _: &ActiveEventLoop, _: WindowId, event: WindowEvent) {
        match event {
            WindowEvent::CloseRequested | WindowEvent::Destroyed => self.state.open = false,
            WindowEvent::Focused(focused) => {
                self.state.focused = focused;
                if !focused {
                    self.state.keys.clear();
                    self.state.pressed.clear();
                    self.state.mouse = [false; 3];
                }
            }
            WindowEvent::CursorMoved { position, .. } => {
                self.state.position = Some([position.x, position.y])
            }
            WindowEvent::CursorLeft { .. } => self.state.position = None,
            WindowEvent::KeyboardInput { event, .. } => {
                if let PhysicalKey::Code(key) = event.physical_key {
                    if event.state == ElementState::Pressed {
                        if !event.repeat {
                            self.state.pressed.insert(key);
                        }
                        self.state.keys.insert(key);
                    } else {
                        self.state.keys.remove(&key);
                    }
                }
            }
            WindowEvent::MouseInput { button, state, .. } => {
                if let Some(index) = mouse_index(button) {
                    self.state.mouse[index] = state == ElementState::Pressed;
                }
            }
            WindowEvent::MouseWheel { delta, .. } => {
                let [x, y] = match delta {
                    MouseScrollDelta::LineDelta(x, y) => [x, y],
                    MouseScrollDelta::PixelDelta(p) => [p.x as f32 / 32.0, p.y as f32 / 32.0],
                };
                self.state.scroll[0] += x;
                self.state.scroll[1] += y;
            }
            WindowEvent::RedrawRequested => {
                // Present synchronously inside the native redraw callback,
                // including AppKit's resize/drawRect path.
                if let (Some(renderer), Some(window)) = (&mut self.renderer, &self.state.window) {
                    let size = window.inner_size();
                    window.pre_present_notify();
                    if let Err(error) = renderer.present([size.width, size.height]) {
                        self.state.error = Some(error);
                    }
                }
            }
            _ => {}
        }
    }
}
fn mouse_index(button: MouseButton) -> Option<usize> {
    match button {
        MouseButton::Left => Some(0),
        MouseButton::Right => Some(1),
        MouseButton::Middle => Some(2),
        _ => None,
    }
}

pub struct DesktopWindow {
    events: EventLoop<()>,
    state: State,
    cursor_hidden: bool,
    interval: Duration,
    last: Instant,
}
impl DesktopWindow {
    pub fn new(title: &str, width: usize, height: usize) -> Result<Self> {
        let mut events = EventLoop::new().context("creating native event loop")?;
        let mut state = State {
            window: None,
            attributes: Window::default_attributes()
                .with_title(title)
                .with_inner_size(LogicalSize::new(width as f64, height as f64))
                .with_min_inner_size(LogicalSize::new(64.0, 64.0)),
            open: true,
            focused: false,
            keys: BTreeSet::new(),
            pressed: BTreeSet::new(),
            mouse: [false; 3],
            position: None,
            scroll: [0.0; 2],
            error: None,
        };
        events.pump_app_events(
            Some(Duration::ZERO),
            &mut Pump {
                state: &mut state,
                renderer: None,
            },
        );
        if let Some(error) = state.error.take() {
            return Err(error);
        }
        anyhow::ensure!(
            state.window.is_some(),
            "native event loop did not create a window"
        );
        Ok(Self {
            events,
            state,
            cursor_hidden: false,
            interval: Duration::from_secs_f64(1.0 / 60.0),
            last: Instant::now(),
        })
    }
    pub fn native(&self) -> Arc<Window> {
        self.state.window.as_ref().unwrap().clone()
    }
    pub fn get_size(&self) -> (usize, usize) {
        let size = self.native().inner_size();
        (size.width as usize, size.height as usize)
    }
    pub fn get_mouse_pos(&self) -> Option<(f64, f64)> {
        self.state.position.map(|p| (p[0], p[1]))
    }
    pub fn get_mouse_down(&self, button: MouseButton) -> bool {
        mouse_index(button).is_some_and(|i| self.state.mouse[i])
    }
    pub fn get_scroll_wheel(&self) -> Option<(f32, f32)> {
        (self.state.scroll != [0.0; 2]).then_some((self.state.scroll[0], self.state.scroll[1]))
    }
    pub fn is_key_down(&self, key: Key) -> bool {
        self.state.keys.contains(&key)
    }
    pub fn is_key_pressed(&self, key: Key) -> bool {
        self.state.pressed.contains(&key)
    }
    pub fn is_open(&self) -> bool {
        self.state.open
    }
    pub fn is_active(&self) -> bool {
        self.state.focused
    }
    pub fn set_title(&self, title: &str) {
        self.native().set_title(title);
    }
    pub fn set_game_cursor(&mut self, hidden: bool) {
        if hidden != self.cursor_hidden {
            self.native().set_cursor_visible(!hidden);
            self.cursor_hidden = hidden;
        }
    }
    pub fn update(&mut self, renderer: &mut Renderer) -> Result<()> {
        self.state.pressed.clear();
        self.state.scroll = [0.0; 2];
        self.native().request_redraw();
        if let PumpStatus::Exit(_) = self.events.pump_app_events(
            Some(Duration::ZERO),
            &mut Pump {
                state: &mut self.state,
                renderer: Some(renderer),
            },
        ) {
            self.state.open = false;
        }
        if let Some(error) = self.state.error.take() {
            return Err(error);
        }
        if let Some(remaining) = self.interval.checked_sub(self.last.elapsed()) {
            std::thread::sleep(remaining);
        }
        self.last = Instant::now();
        Ok(())
    }
}
impl Drop for DesktopWindow {
    fn drop(&mut self) {
        if self.cursor_hidden {
            if let Some(window) = &self.state.window {
                window.set_cursor_visible(true);
            }
        }
    }
}
