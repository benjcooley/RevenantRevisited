// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                screens.rs - Native title and modal menus              *
// *************************************************************************

use super::{
    batch::Batch,
    canvas::{Canvas, Rect},
    definition::{Pane, Widget, WidgetState},
};
use crate::{
    assets::{bitmap::Bitmap, definition, font::Font, multi::Multi, vfs::Vfs},
    game::{Action, Destination, Flow, MenuOrigin, Screen},
    input::{PointerFrame, Target},
};
use anyhow::Result;
use std::collections::BTreeMap;

#[derive(Clone, Copy, Default)]
pub struct MenuKeys {
    pub previous: bool,
    pub next: bool,
    pub accept: bool,
    pub back: bool,
    pub left: bool,
    pub right: bool,
}
#[derive(Default)]
pub struct MenuResponse {
    pub action: Option<Action>,
    pub pressed: bool,
}
struct Control {
    action: Option<Action>,
    rect: Rect,
    sprite: Option<&'static str>,
    slider: bool,
}
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
    pause: Pane,
    options_title: Pane,
    options_game: Pane,
    confirm: Pane,
    questions: [String; 2],
    screen: Option<Screen>,
    selected: Option<Action>,
    pressed: Option<Action>,
    dragging: bool,
}
impl Screens {
    pub fn load(vfs: &mut Vfs) -> Result<Self> {
        let mut art = BTreeMap::new();
        for (file, names, prefix) in [
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
                "",
            ),
            ("loadbar.dat", vec!["background", "bar"], "Loading"),
            (
                "death.dat",
                vec![
                    "background",
                    "RestartU",
                    "RestartD",
                    "RestartS",
                    "LoadU",
                    "ExitU",
                    "ExitD",
                    "ExitS",
                ],
                "Death",
            ),
        ] {
            let bytes = vfs.read(file)?;
            let multi = Multi::parse(&bytes)?;
            for name in names {
                art.insert(format!("{prefix}{name}"), multi.bitmap_native(name)?);
            }
        }
        let english = definition::lines(&vfs.read("english.def")?)?;
        let question = |key: &str, fallback: &str| {
            english
                .iter()
                .find(|line| line[0].eq_ignore_ascii_case(key))
                .and_then(|line| line.get(1))
                .map_or_else(|| fallback.into(), |s| s.replace("\\n", "\n"))
        };
        Ok(Self {
            art,
            font: Font::named(vfs, "Med")?,
            pause: Pane::load(
                vfs,
                "ingamemenu",
                "ingamemenu",
                "tex",
                Rect::new(126, 65, 394, 316),
            )?,
            options_title: Pane::load(
                vfs,
                "options",
                "default",
                "alpha",
                Rect::new(0, 0, 640, 480),
            )?,
            options_game: Pane::load(vfs, "options", "default", "tex", Rect::new(0, 0, 640, 480))?,
            confirm: Pane::load(
                vfs,
                "popup",
                "yesno",
                "alpha",
                Rect::new(129, 122, 398, 212),
            )?,
            questions: [
                question(
                    "QUITGAMEYN",
                    "\nAre you sure you want to quit the current module?",
                ),
                question(
                    "EXITGAMEYN",
                    "\nAre you sure you want to exit the game and return to Windows?",
                ),
            ],
            screen: None,
            selected: None,
            pressed: None,
            dragging: false,
        })
    }
    pub fn reset_input(&mut self) {
        self.screen = None;
        self.selected = None;
        self.pressed = None;
        self.dragging = false;
    }
    fn pane(&self, screen: Screen) -> Option<&Pane> {
        match screen {
            Screen::Pause => Some(&self.pause),
            Screen::Options(MenuOrigin::Title) => Some(&self.options_title),
            Screen::Options(MenuOrigin::Pause) => Some(&self.options_game),
            Screen::Confirm(_) => Some(&self.confirm),
            _ => None,
        }
    }
    fn binding(screen: Screen, widget: &Widget) -> Option<Action> {
        match (screen, widget.name.to_ascii_lowercase().as_str()) {
            (Screen::Pause, "options") => Some(Action::Options),
            (Screen::Pause, "quit") => Some(Action::ReturnToTitle),
            (Screen::Pause, "exit") => Some(Action::Quit),
            (Screen::Pause, "resume") => Some(Action::Resume),
            (Screen::Options(_), "realtime") => Some(Action::ToggleRealTime),
            (Screen::Options(_), "enhanced") => Some(Action::ToggleEnhanced),
            (Screen::Options(_), "sound") => Some(Action::VolumeUp),
            (Screen::Options(_), "ok") => Some(Action::ApplyOptions),
            (Screen::Options(_), "cancel") => Some(Action::Back),
            (Screen::Confirm(_), "yes") => Some(Action::Confirm),
            (Screen::Confirm(_), "no") => Some(Action::Back),
            _ => None,
        }
    }
    fn controls(&self, flow: &Flow) -> Vec<Control> {
        let sprites: &[(Option<Action>, &str)] = match flow.screen() {
            Screen::Title => &[
                (Some(Action::NewGame), "MenuNewGame"),
                (None, "MenuLoadGame"),
                (None, "MenuMulti"),
                (Some(Action::Options), "MenuOptions"),
                (Some(Action::Quit), "MenuExit"),
            ],
            Screen::Death => &[
                (Some(Action::NewGame), "DeathRestart"),
                (None, "DeathLoad"),
                (Some(Action::ReturnToTitle), "DeathExit"),
            ],
            _ => &[],
        };
        if !sprites.is_empty() {
            return sprites
                .iter()
                .map(|&(action, sprite)| {
                    let bitmap = &self.art[&format!("{sprite}U")];
                    Control {
                        action,
                        rect: Rect::new(
                            -bitmap.registration[0],
                            -bitmap.registration[1],
                            bitmap.width as i32,
                            bitmap.height as i32,
                        ),
                        sprite: Some(sprite),
                        slider: false,
                    }
                })
                .collect();
        }
        self.pane(flow.screen()).map_or_else(Vec::new, |pane| {
            pane.widgets
                .iter()
                .filter(|w| matches!(w.kind.as_str(), "BUTTON" | "SCROLLBAR"))
                .map(|w| Control {
                    action: Self::binding(flow.screen(), w),
                    rect: w.rect,
                    sprite: None,
                    slider: w.kind == "SCROLLBAR",
                })
                .collect()
        })
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
        let point = pointer.position.map(|p| Layout::new(viewport).point(p));
        let hover = point
            .and_then(|p| controls.iter().find(|c| c.rect.contains(p)))
            .and_then(|c| c.action);
        if pointer.left.cancelled {
            self.pressed = None;
            self.dragging = false;
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
            self.selected = Some(
                enabled[if keys.previous {
                    (at + enabled.len() - 1) % enabled.len()
                } else {
                    (at + 1) % enabled.len()
                }],
            );
        }
        if self.selected == Some(Action::VolumeUp) && (keys.left || keys.right) {
            return MenuResponse {
                action: Some(if keys.left {
                    Action::VolumeDown
                } else {
                    Action::VolumeUp
                }),
                pressed: false,
            };
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
            self.dragging = false;
        }
        if pointer.left.held && pointer.left.target == Some(Target::Ui) {
            if let (Some(p), Some(slider)) = (
                point,
                controls
                    .iter()
                    .find(|c| c.slider && c.action.is_some() && c.action == self.pressed),
            ) {
                let x = p[0] - f64::from(slider.rect.x);
                if pointer.left.pressed {
                    self.dragging = x >= 19.0 && x <= f64::from(slider.rect.w - 12);
                    if !self.dragging {
                        response.action = Some(if x < 19.0 {
                            Action::VolumeDown
                        } else {
                            Action::VolumeUp
                        });
                    }
                }
                if self.dragging {
                    response.action = Some(Action::SetVolume(
                        ((x - 19.0) * 100.0 / f64::from(slider.rect.w - 49))
                            .round()
                            .clamp(0.0, 100.0) as u8,
                    ));
                }
            }
        }
        if pointer.left.released {
            if pointer.left.released_on(Target::Ui)
                && self.pressed.is_some()
                && self.pressed == hover
                && self.pressed != Some(Action::VolumeUp)
            {
                response.action = self.pressed;
            }
            self.pressed = None;
            self.dragging = false;
        }
        response
    }
    fn draw_pane(
        &self,
        pane: &Pane,
        flow: &Flow,
        screen: Screen,
        canvas: &mut Canvas<'_>,
        origin: [i32; 2],
    ) {
        pane.background(canvas, origin);
        for widget in &pane.widgets {
            let binding = Self::binding(screen, widget);
            let options = flow.options();
            let down = match binding {
                Some(Action::ToggleRealTime) => options.real_time,
                Some(Action::ToggleEnhanced) => options.enhanced,
                _ => binding.is_some() && binding == self.pressed,
            };
            let state = WidgetState {
                down,
                focused: screen == flow.screen() && binding.is_some() && binding == self.selected,
                disabled: matches!(widget.kind.as_str(), "BUTTON" | "SCROLLBAR")
                    && binding.is_none(),
                value: if widget.name.eq_ignore_ascii_case("Sound") {
                    options.volume
                } else {
                    0
                },
            };
            if widget.name == "message" {
                let Screen::Confirm(destination) = screen else {
                    continue;
                };
                let rect = Rect::new(
                    widget.rect.x + origin[0],
                    widget.rect.y + origin[1],
                    widget.rect.w,
                    widget.rect.h,
                );
                let message = &self.questions[usize::from(destination == Destination::Exit)];
                for (i, line) in self.font.wrap(message, rect.w).iter().enumerate() {
                    if (i as i32 + 1) * self.font.height > rect.h {
                        break;
                    }
                    Pane::label(
                        canvas,
                        &self.font,
                        line,
                        Rect::new(
                            rect.x,
                            rect.y + i as i32 * self.font.height,
                            rect.w,
                            self.font.height,
                        ),
                        0,
                        0xffffff,
                        true,
                    );
                }
            } else {
                pane.widget(canvas, &self.font, widget, origin, state);
            }
        }
    }
    pub fn record(
        &self,
        flow: &Flow,
        viewport: [usize; 2],
        progress: u16,
        _label: &str,
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
        if matches!(
            flow.screen(),
            Screen::Title | Screen::Loading | Screen::Death | Screen::Options(MenuOrigin::Title)
        ) {
            canvas.fill(
                Rect::new(
                    0,
                    0,
                    (viewport[0] as f64 / layout.scale).ceil() as i32,
                    (viewport[1] as f64 / layout.scale).ceil() as i32,
                ),
                0xff000000,
            );
        }
        match flow.screen() {
            Screen::Title => {
                canvas.bitmap(&self.art["MainMenu"], layout.origin[0], layout.origin[1]);
                if let Some(error) = flow.error() {
                    canvas.fill(layout.rect(Rect::new(10, 10, 620, 55)), 0xee171310);
                    Pane::label(
                        &mut canvas,
                        &self.font,
                        error,
                        layout.rect(Rect::new(20, 20, 600, 35)),
                        -1,
                        0xffffff,
                        true,
                    );
                }
            }
            Screen::Loading => {
                canvas.bitmap(
                    &self.art["Loadingbackground"],
                    layout.origin[0],
                    layout.origin[1],
                );
                let bar = &self.art["Loadingbar"];
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
            }
            Screen::Death => canvas.bitmap(
                &self.art["Deathbackground"],
                layout.origin[0],
                layout.origin[1],
            ),
            Screen::Confirm(_) => {
                self.draw_pane(&self.pause, flow, Screen::Pause, &mut canvas, layout.origin);
                self.draw_pane(
                    &self.confirm,
                    flow,
                    flow.screen(),
                    &mut canvas,
                    layout.origin,
                );
            }
            screen => {
                if let Some(pane) = self.pane(screen) {
                    self.draw_pane(pane, flow, screen, &mut canvas, layout.origin);
                }
            }
        }
        for control in self.controls(flow) {
            if let Some(sprite) = control.sprite {
                let state = if control.action.is_some() && self.pressed == control.action {
                    'D'
                } else if control.action.is_some() && self.selected == control.action {
                    'S'
                } else {
                    'U'
                };
                let rect = layout.rect(control.rect);
                canvas.bitmap(&self.art[&format!("{sprite}{state}")], rect.x, rect.y);
                if control.action.is_none() {
                    canvas.fill(rect, 0x99000000);
                }
            }
        }
    }
}
