// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                screens.rs - Title, loading and modal menus            *
// *************************************************************************

use super::{
    batch::Batch,
    canvas::{Canvas, Rect},
};
use crate::{
    assets::{bitmap::Bitmap, font::Font, multi::Multi, vfs::Vfs},
    game::{Action, Destination, Flow, Screen},
    input::{PointerFrame, Target},
};
use anyhow::{Context, Result};
use std::collections::BTreeMap;

#[derive(Clone, Copy, Default)]
pub struct MenuKeys {
    pub previous: bool,
    pub next: bool,
    pub accept: bool,
    pub back: bool,
}
#[derive(Default)]
pub struct MenuResponse {
    pub action: Option<Action>,
    pub pressed: bool,
}
struct Control {
    action: Option<Action>,
    label: String,
    rect: Rect,
    sprite: Option<&'static str>,
}
impl Control {
    fn text(action: Action, label: impl Into<String>, rect: Rect) -> Self {
        Self {
            action: Some(action),
            label: label.into(),
            rect,
            sprite: None,
        }
    }
}

/// A centered 640x480 canvas, scaled down for smaller windows. Hit testing and
/// composition share this transform, including the surrounding letterbox.
#[derive(Clone, Copy)]
struct Layout {
    scale: f64,
    origin: [i32; 2],
}
impl Layout {
    fn new([w, h]: [usize; 2]) -> Self {
        let scale = (w as f64 / 640.0).min(h as f64 / 480.0).clamp(0.001, 1.0);
        Self {
            scale,
            origin: [
                ((w as f64 / scale - 640.0) / 2.0).round() as i32,
                ((h as f64 / scale - 480.0) / 2.0).round() as i32,
            ],
        }
    }
    fn point(self, p: [f64; 2]) -> [f64; 2] {
        [
            p[0] / self.scale - f64::from(self.origin[0]),
            p[1] / self.scale - f64::from(self.origin[1]),
        ]
    }
    fn rect(self, r: Rect) -> Rect {
        Rect::new(r.x + self.origin[0], r.y + self.origin[1], r.w, r.h)
    }
}

pub struct Screens {
    art: BTreeMap<String, Bitmap>,
    font: Font,
    small: Font,
    screen: Option<Screen>,
    selected: Option<Action>,
    pressed: Option<Action>,
}
impl Screens {
    pub fn load(vfs: &mut Vfs) -> Result<Self> {
        let mut art = BTreeMap::new();
        for (file, names) in [
            (
                "menus.dat",
                vec![
                    "MainMenu",
                    "MenuNewGameU",
                    "MenuNewGameD",
                    "MenuNewGameS",
                    "MenuLoadGameU",
                    "MenuMultiU",
                    "MenuOptionsU",
                    "MenuOptionsD",
                    "MenuOptionsS",
                    "MenuExitU",
                    "MenuExitD",
                    "MenuExitS",
                ],
            ),
            ("loadbar.dat", vec!["background", "bar"]),
        ] {
            let bytes = vfs.read(file)?;
            let multi = Multi::parse(&bytes)?;
            for name in names {
                art.insert(name.into(), multi.bitmap(name).with_context(|| file)?);
            }
        }
        Ok(Self {
            art,
            font: Font::decode(&vfs.read("dlgfont.dat")?)?,
            small: Font::decode(&vfs.read("sysfont.dat")?)?,
            screen: None,
            selected: None,
            pressed: None,
        })
    }
    pub fn reset_input(&mut self) {
        self.screen = None;
        self.selected = None;
        self.pressed = None;
    }

    fn controls(&self, flow: &Flow) -> Vec<Control> {
        let row = |y| Rect::new(205, y, 230, 34);
        match flow.screen() {
            Screen::Title => [
                (Some(Action::NewGame), "MenuNewGame"),
                (None, "MenuLoadGame"),
                (None, "MenuMulti"),
                (Some(Action::Options), "MenuOptions"),
                (Some(Action::Quit), "MenuExit"),
            ]
            .into_iter()
            .map(|(action, sprite)| {
                let bitmap = &self.art[&format!("{sprite}U")];
                Control {
                    action,
                    label: String::new(),
                    sprite: Some(sprite),
                    rect: Rect::new(
                        -bitmap.registration[0],
                        -bitmap.registration[1],
                        bitmap.width as i32,
                        bitmap.height as i32,
                    ),
                }
            })
            .collect(),
            Screen::Pause => vec![
                Control::text(Action::Resume, "Resume", row(165)),
                Control::text(Action::Options, "Options", row(210)),
                Control::text(Action::ReturnToTitle, "Quit Module", row(255)),
                Control::text(Action::Quit, "Exit Program", row(300)),
            ],
            Screen::Options(_) => {
                let options = flow.options();
                vec![
                    Control::text(Action::VolumeDown, "Volume -", Rect::new(205, 165, 105, 34)),
                    Control::text(Action::VolumeUp, "Volume +", Rect::new(330, 165, 105, 34)),
                    Control::text(
                        Action::ToggleSound,
                        if options.muted {
                            "Sound: Off"
                        } else {
                            "Sound: On"
                        },
                        row(235),
                    ),
                    Control::text(
                        Action::ToggleShadows,
                        if options.shadows {
                            "Shadows: On"
                        } else {
                            "Shadows: Off"
                        },
                        row(280),
                    ),
                    Control::text(Action::ApplyOptions, "Apply", Rect::new(205, 340, 105, 34)),
                    Control::text(Action::Back, "Cancel", Rect::new(330, 340, 105, 34)),
                ]
            }
            Screen::Confirm(_) => vec![
                Control::text(Action::Back, "No", Rect::new(205, 270, 105, 34)),
                Control::text(Action::Confirm, "Yes", Rect::new(330, 270, 105, 34)),
            ],
            Screen::Death => vec![
                Control::text(Action::NewGame, "Restart", row(220)),
                Control::text(Action::ReturnToTitle, "Return to Title", row(265)),
            ],
            _ => Vec::new(),
        }
    }
    pub fn control_center(
        &self,
        flow: &Flow,
        viewport: [usize; 2],
        action: Action,
    ) -> Option<[f64; 2]> {
        let layout = Layout::new(viewport);
        self.controls(flow)
            .into_iter()
            .find(|c| c.action == Some(action))
            .map(|c| {
                let r = layout.rect(c.rect);
                [
                    (f64::from(r.x) + f64::from(r.w) * 0.5) * layout.scale,
                    (f64::from(r.y) + f64::from(r.h) * 0.5) * layout.scale,
                ]
            })
    }
    pub fn interactive_at(&self, flow: &Flow, viewport: [usize; 2], position: [f64; 2]) -> bool {
        let point = Layout::new(viewport).point(position);
        self.controls(flow)
            .iter()
            .any(|c| c.action.is_some() && c.rect.contains(point))
    }
    pub fn input(
        &mut self,
        flow: &Flow,
        viewport: [usize; 2],
        pointer: PointerFrame,
        keys: MenuKeys,
    ) -> MenuResponse {
        let controls = self.controls(flow);
        let enabled: Vec<_> = controls.iter().filter_map(|c| c.action).collect();
        if self.screen != Some(flow.screen()) {
            self.screen = Some(flow.screen());
            self.selected = enabled.first().copied();
            self.pressed = None;
        }
        let hover = pointer.position.and_then(|p| {
            let point = Layout::new(viewport).point(p);
            controls
                .iter()
                .find(|c| c.rect.contains(point))
                .and_then(|c| c.action)
        });
        if pointer.left.cancelled {
            self.pressed = None;
        }
        if pointer.delta != [0.0; 2] && hover.is_some() {
            self.selected = hover;
        }
        if keys.back {
            self.pressed = None;
            return MenuResponse {
                action: Some(Action::Back),
                pressed: false,
            };
        }
        if !enabled.is_empty() && (keys.previous || keys.next) {
            let at = enabled
                .iter()
                .position(|&a| Some(a) == self.selected)
                .unwrap_or(0);
            let next = if keys.previous {
                (at + enabled.len() - 1) % enabled.len()
            } else {
                (at + 1) % enabled.len()
            };
            self.selected = Some(enabled[next]);
        }
        if keys.accept {
            self.pressed = None;
            return MenuResponse {
                action: self.selected,
                pressed: false,
            };
        }
        let mut response = MenuResponse::default();
        if pointer.left.pressed_on(Target::Ui) {
            self.pressed = hover;
            self.selected = hover.or(self.selected);
            response.pressed = hover.is_some();
        }
        if pointer.left.released {
            if pointer.left.released_on(Target::Ui)
                && self.pressed.is_some()
                && self.pressed == hover
            {
                response.action = self.pressed;
            }
            self.pressed = None;
        }
        response
    }

    pub fn record(
        &self,
        flow: &Flow,
        viewport: [usize; 2],
        progress: u16,
        label: &str,
        batch: &mut Batch,
    ) {
        if matches!(flow.screen(), Screen::Play | Screen::Exit) {
            return;
        }
        let layout = Layout::new(viewport);
        let mut canvas = Canvas {
            viewport,
            scale: layout.scale,
            batch,
        };
        let full = Rect::new(
            0,
            0,
            (viewport[0] as f64 / layout.scale).ceil() as i32,
            (viewport[1] as f64 / layout.scale).ceil() as i32,
        );
        let modal = matches!(
            flow.screen(),
            Screen::Pause | Screen::Options(crate::game::MenuOrigin::Pause) | Screen::Confirm(_)
        );
        canvas.fill(full, if modal { 0xbb000000 } else { 0xff000000 });
        let font = if matches!(flow.screen(), Screen::Title | Screen::Loading) {
            &self.small
        } else {
            &self.font
        };
        let text = |canvas: &mut Canvas<'_>, text: &str, y| {
            let x = 320 - font.width(text) / 2;
            canvas.text(font, text, layout.origin[0] + x, layout.origin[1] + y, 610);
        };
        match flow.screen() {
            Screen::Title => {
                canvas.bitmap(&self.art["MainMenu"], layout.origin[0], layout.origin[1]);
                canvas.text(
                    font,
                    concat!("Revenant / Rust  ", env!("CARGO_PKG_VERSION")),
                    layout.origin[0] + 15,
                    layout.origin[1] + 450,
                    610,
                );
                if let Some(error) = flow.error() {
                    canvas.fill(layout.rect(Rect::new(10, 10, 620, 60)), 0xee171310);
                    text(&mut canvas, "Unable to start the game", 15);
                    canvas.text(
                        font,
                        error,
                        layout.origin[0] + 20,
                        layout.origin[1] + 40,
                        600,
                    );
                }
            }
            Screen::Loading => {
                canvas.bitmap(&self.art["background"], layout.origin[0], layout.origin[1]);
                let bar = &self.art["bar"];
                let width = (bar.width * usize::from(progress.min(1000)) / 1000) as i32;
                canvas.blit(
                    bar,
                    Rect::new(0, 0, width, bar.height as i32),
                    layout.rect(Rect::new(
                        bar.registration[0].abs(),
                        bar.registration[1].abs(),
                        width,
                        bar.height as i32,
                    )),
                );
                text(&mut canvas, label, 435);
            }
            screen => {
                canvas.fill(layout.rect(Rect::new(165, 95, 310, 310)), 0xfa211c18);
                canvas.border(layout.rect(Rect::new(165, 95, 310, 310)), 0xffa68b59);
                let title = match screen {
                    Screen::Pause => "Game Paused",
                    Screen::Options(_) => "Options",
                    Screen::Confirm(Destination::Title) => "Return to Title?",
                    Screen::Confirm(Destination::Exit) => "Exit Program?",
                    Screen::Death => "You have died",
                    _ => "",
                };
                text(&mut canvas, title, 115);
                if matches!(screen, Screen::Confirm(_)) {
                    text(&mut canvas, "Your current game will end.", 205);
                }
                if matches!(screen, Screen::Options(_)) {
                    text(
                        &mut canvas,
                        &format!("Volume: {}%", flow.options().volume),
                        205,
                    );
                }
            }
        }
        for control in self.controls(flow) {
            let r = layout.rect(control.rect);
            if let Some(sprite) = control.sprite {
                let state = if control.action.is_some() && self.pressed == control.action {
                    'D'
                } else if control.action.is_some() && self.selected == control.action {
                    'S'
                } else {
                    'U'
                };
                canvas.bitmap(&self.art[&format!("{sprite}{state}")], r.x, r.y);
                if control.action.is_none() {
                    canvas.fill(r, 0x99000000);
                }
            } else {
                let color = if self.pressed == control.action {
                    0xff635039
                } else if self.selected == control.action {
                    0xff4c3d28
                } else {
                    0xff302a22
                };
                canvas.fill(r, color);
                canvas.border(r, 0xff8e7955);
                canvas.text(
                    font,
                    &control.label,
                    r.x + (r.w - font.width(&control.label)) / 2,
                    r.y + (r.h - font.height) / 2,
                    r.w,
                );
            }
        }
        if flow.screen() == Screen::Title {
            text(
                &mut canvas,
                "Load Game and Multiplayer are unavailable.",
                425,
            );
        }
    }
}
