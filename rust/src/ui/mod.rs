// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     mod.rs - Retail HUD composition                   *
// *************************************************************************

//! Native pane artwork and layout; gameplay values arrive separately.
pub mod batch;
mod canvas;

use crate::{
    assets::{bitmap::Bitmap, font::Font, multi::Multi, vfs::Vfs},
    input::{PointerFrame, Target},
    render::{Draw, Framebuffer, camera::Camera, equipment::Slot},
    world::{Scene, sector::Position},
};
use anyhow::{Context, Result};
use canvas::{Canvas, Rect};
use std::collections::BTreeMap;

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Tab {
    Book,
    Stats,
    Equipment,
    Spells,
    Inventory,
    Map,
}
impl Tab {
    fn index(self) -> usize {
        match self {
            Self::Book => 0,
            Self::Stats => 1,
            Self::Equipment => 2,
            Self::Spells => 3,
            Self::Inventory => 4,
            Self::Map => 5,
        }
    }
}
const TABS: [Tab; 6] = [
    Tab::Book,
    Tab::Stats,
    Tab::Equipment,
    Tab::Spells,
    Tab::Inventory,
    Tab::Map,
];
const SLOTS: [(Slot, &str, i32, i32); 11] = [
    (Slot::Head, "Head", 74, 37),
    (Slot::Neck, "Neck", 7, 58),
    (Slot::Body, "Chest", 141, 58),
    (Slot::OffHand, "Hand2", 141, 106),
    (Slot::PrimeHand, "Hand", 7, 106),
    (Slot::RightAccessory, "Ring", 7, 154),
    (Slot::LeftAccessory, "Ring2", 141, 154),
    (Slot::RangedWeapon, "Range", 24, 11),
    (Slot::Ammo, "Ammo", 124, 11),
    (Slot::Legs, "Legs", 7, 202),
    (Slot::Feet, "Feet", 141, 202),
];

#[derive(Clone, Copy, Debug)]
pub struct Layout {
    pub scale: f64,
    pub width: i32,
    pub height: i32,
    pub sidebar: bool,
    pub visible: bool,
}
impl Layout {
    pub fn new(viewport: [usize; 2], visible: bool, sidebar: bool) -> Self {
        let scale = (viewport[0] as f64 / 640.0)
            .min(viewport[1] as f64 / 480.0)
            .min(1.0);
        Self {
            scale,
            width: (viewport[0] as f64 / scale).round() as i32,
            height: (viewport[1] as f64 / scale).round() as i32,
            sidebar,
            visible,
        }
    }
    fn play_width(self) -> i32 {
        self.width - if self.visible && self.sidebar { 188 } else { 0 }
    }
    fn play_height(self) -> i32 {
        self.height - if self.visible { 60 } else { 0 }
    }
    pub fn world_viewport(self) -> [usize; 2] {
        [
            (f64::from(self.play_width()) * self.scale).round() as usize,
            (f64::from(self.play_height()) * self.scale).round() as usize,
        ]
    }
    fn tab_strip(self) -> Rect {
        Rect::new(self.play_width() - 56, self.play_height() - 234, 52, 232)
    }
    fn tab(self, index: usize) -> Rect {
        let r = self.tab_strip();
        Rect::new(r.x + 3, r.y + 2 + index as i32 * 37, 36, 35)
    }
    fn logical(self, p: [f64; 2]) -> [f64; 2] {
        [p[0] / self.scale, p[1] / self.scale]
    }
    pub fn hit_test(self, p: [f64; 2]) -> bool {
        if !self.visible {
            return false;
        }
        let p = self.logical(p);
        Rect::new(0, self.height - 60, self.play_width(), 60).contains(p)
            || Rect::new(0, self.height - 76, self.play_width() - 60, 16).contains(p)
            || (self.sidebar && Rect::new(self.width - 188, 0, 188, self.height).contains(p))
            || self.tab_strip().contains(p)
            || Rect::new(6, 0, 190, 64).contains(p)
    }
    pub fn world_camera(self, mut camera: Camera, viewport: [usize; 2]) -> Camera {
        let world = self.world_viewport();
        for i in 0..2 {
            camera.center[i] += (viewport[i] as f64 - world[i] as f64) / (2.0 * camera.zoom);
        }
        camera
    }
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
enum Control {
    Tab(Tab),
    Slot(Slot),
}
#[derive(Default, Debug, PartialEq, Eq)]
pub struct Response {
    pub pressed: bool,
    pub activated: bool,
}

pub struct Hud {
    art: BTreeMap<String, Bitmap>,
    font: Font,
    paperdoll: Framebuffer,
    pub visible: bool,
    pub sidebar: bool,
    top: Tab,
    bottom: Tab,
    pressed: Option<Control>,
    hover: Option<Control>,
    pub message: String,
    pub muted: bool,
}
impl Hud {
    pub fn load(vfs: &mut Vfs) -> Result<Self> {
        let mut art = BTreeMap::new();
        for (file, names) in [
            (
                "statusbarnotex.dat",
                &[
                    "BackPanel",
                    "Bars",
                    "HealthIcon",
                    "ManaIcon",
                    "FatigueIcon",
                    "Ring",
                ][..],
            ),
            ("portraits.dat", &["LockeFace"][..]),
            (
                "bottombar.dat",
                &["UtilityBar", "BarEndCap", "BarInvBox"][..],
            ),
            ("sidebartabsnotex.dat", &["Up", "Down", "Select"][..]),
            (
                "equippane.dat",
                &[
                    "Equip", "Head", "Neck", "Chest", "Hand2", "Hand", "Ring", "Ring2", "Range",
                    "Ammo", "Legs", "Feet",
                ][..],
            ),
            (
                "inventory.dat",
                &["Inventory", "Backpack", "GoldPile", "InvArwLG", "InvArwRG"][..],
            ),
            ("statspane.dat", &["Stats"][..]),
            ("spellscroll.dat", &["Scroll"][..]),
            ("spellpane.dat", &["spellconstr"][..]),
            ("automap.dat", &["Amap", "Marker"][..]),
            ("spellicons.dat", &["RingG"][..]),
        ] {
            let data = vfs.read(file)?;
            let multi = Multi::parse(&data).with_context(|| file)?;
            for name in names {
                art.insert(
                    format!("{file}/{name}"),
                    multi.bitmap(name).with_context(|| file)?,
                );
            }
        }
        Ok(Self {
            art,
            font: Font::decode(&vfs.read("sysfont.dat")?)?,
            paperdoll: Framebuffer::new(92, 164)?,
            visible: true,
            sidebar: true,
            top: Tab::Equipment,
            bottom: Tab::Inventory,
            pressed: None,
            hover: None,
            message: "WASD / RIGHT MOUSE: WALK    F1: HUD    H: SIDEBAR    F9: SOUND".into(),
            muted: false,
        })
    }
    pub fn layout(&self, viewport: [usize; 2]) -> Layout {
        Layout::new(viewport, self.visible, self.sidebar)
    }
    pub fn select(&mut self, tab: Tab) {
        self.visible = true;
        self.sidebar = true;
        if tab.index() < 3 {
            self.top = tab;
        } else {
            self.bottom = tab;
        }
        self.pressed = None;
    }
    pub fn toggle_visible(&mut self) {
        self.visible = !self.visible;
        self.pressed = None;
        self.hover = None;
    }
    pub fn toggle_sidebar(&mut self) {
        self.sidebar = !self.sidebar;
        self.pressed = None;
        self.hover = None;
    }
    fn control(&self, layout: Layout, p: [f64; 2]) -> Option<Control> {
        if !layout.visible {
            return None;
        }
        let p = layout.logical(p);
        for (i, tab) in TABS.into_iter().enumerate() {
            if layout.tab(i).contains(p) {
                return Some(Control::Tab(tab));
            }
        }
        if self.sidebar && self.top == Tab::Equipment {
            for (slot, _, x, y) in SLOTS {
                if Rect::new(layout.width - 188 + x, y, 40, 40).contains(p) {
                    return Some(Control::Slot(slot));
                }
            }
        }
        None
    }
    pub fn interactive_at(&self, viewport: [usize; 2], p: [f64; 2]) -> bool {
        self.control(self.layout(viewport), p).is_some()
    }
    pub fn pointer(
        &mut self,
        event: PointerFrame,
        viewport: [usize; 2],
        scene: &Scene,
        actor: Option<usize>,
    ) -> Response {
        let layout = self.layout(viewport);
        self.hover = event.position.and_then(|p| self.control(layout, p));
        if event.left.cancelled {
            self.pressed = None;
            return Response::default();
        }
        let mut response = Response::default();
        if event.left.pressed_on(Target::Ui) {
            self.pressed = self.hover;
            response.pressed = self.pressed.is_some();
        }
        if event.left.released {
            let control = self.pressed.take();
            if let Some(control) = control.filter(|control| {
                event.left.released_on(Target::Ui) && Some(*control) == self.hover
            }) {
                response.activated = true;
                match control {
                    Control::Tab(tab) => self.select(tab),
                    Control::Slot(slot) => {
                        let gear = actor
                            .and_then(|i| scene.draws.get(i))
                            .and_then(|d| match d {
                                Draw::Mesh(m) => m.equipment(slot),
                                _ => None,
                            });
                        self.message = match gear {
                            Some(gear) => {
                                format!("{slot:?}: {} attached parts", gear.bindings().len())
                            }
                            None => format!("{slot:?}: empty"),
                        };
                    }
                }
            }
        }
        response
    }
    pub fn record(
        &mut self,
        frame: &mut Framebuffer,
        scene: &Scene,
        actor: Option<usize>,
        batch: &mut batch::Batch,
        renderer: &mut dyn crate::render::backend::PreviewRenderer,
    ) -> Result<()> {
        self.render_commands(frame, scene, actor, batch, renderer)
    }
    fn render_commands(
        &mut self,
        frame: &mut Framebuffer,
        scene: &Scene,
        actor: Option<usize>,
        batch: &mut batch::Batch,
        renderer: &mut dyn crate::render::backend::PreviewRenderer,
    ) -> Result<()> {
        if !self.visible {
            return Ok(());
        }
        let layout = self.layout(frame.viewport());
        let source = actor.and_then(|i| scene.sources.get(i));
        let locke = source.is_some_and(|source| source.name.to_ascii_lowercase().contains("locke"));
        let level = source.map_or(0, |source| source.sector.level);
        let actor = actor
            .and_then(|i| scene.draws.get(i))
            .and_then(|d| match d {
                Draw::Mesh(m) => Some(m),
                _ => None,
            });
        let art = &self.art;
        let image = |file: &str, name: &str| &art[&format!("{file}.dat/{name}")];
        let mut c = Canvas {
            viewport: frame.viewport(),
            scale: layout.scale,
            batch,
        };
        let sx = layout.width - 188;
        let by = layout.height - 60;
        let pw = layout.play_width();
        c.blit(
            image("bottombar", "UtilityBar"),
            Rect::new(0, 0, 640, 60),
            Rect::new(0, by, pw, 60),
        );
        c.bitmap(image("bottombar", "BarEndCap"), pw - 10, by);
        for x in [10, 60, 110, 160] {
            c.bitmap(image("spellicons", "RingG"), x, by + 10);
        }
        for i in 0..((pw - 220) / 45).max(0) {
            c.bitmap(image("bottombar", "BarInvBox"), 220 + i * 45, by + 10);
        }
        c.fill(Rect::new(0, by - 16, pw - 60, 16), 0xb0101010);
        c.text(&self.font, &self.message, 8, by - 13, pw - 76);
        if self.muted {
            c.text(&self.font, "MUTED", 8, 70, 80);
        }

        c.bitmap(image("statusbarnotex", "BackPanel"), 6, 0);
        if locke {
            c.bitmap(image("portraits", "LockeFace"), 17, 16);
        }
        c.bitmap(image("statusbarnotex", "Ring"), 10, 9);
        for (name, y) in [("HealthIcon", 3), ("ManaIcon", 17), ("FatigueIcon", 32)] {
            c.bitmap(image("statusbarnotex", name), 49, y);
        }
        for (y, src_y, w, h) in [(11, 49, 125, 17), (27, 66, 77, 12), (40, 81, 53, 12)] {
            c.blit(
                image("statusbarnotex", "Bars"),
                Rect::new(2, src_y, w, h),
                Rect::new(70, y, w, h),
            );
            c.text(&self.font, "--", 80, y + 1, w - 4);
        }
        c.text(
            &self.font,
            if locke { "LOCKE" } else { "PREVIEW" },
            15,
            53,
            70,
        );

        if self.sidebar {
            c.fill(Rect::new(sx, 0, 188, layout.height), 0xff171410);
            let top = match self.top {
                Tab::Equipment => ("equippane", "Equip"),
                Tab::Stats => ("statspane", "Stats"),
                _ => ("spellscroll", "Scroll"),
            };
            c.bitmap(image(top.0, top.1), sx, 0);
            if self.top == Tab::Equipment {
                for (slot, name, x, y) in SLOTS {
                    c.bitmap(image("equippane", name), sx + x, y);
                    if actor.is_some_and(|actor| actor.equipment(slot).is_some()) {
                        c.border(Rect::new(sx + x, y, 40, 40), 0xffc4a65a);
                    }
                }
                if let Some(actor) = actor {
                    let mut doll = actor.clone();
                    doll.set_pose(
                        Position::default(),
                        [0, 0, 128],
                        actor.state() as u16,
                        actor.frame(),
                    )?;
                    let bounds = doll.bounds();
                    let mut camera = Camera::new(Position::default());
                    camera.center = [(bounds[0] + bounds[2]) * 0.5, (bounds[1] + bounds[3]) * 0.5];
                    camera.zoom = (84.0 / (bounds[2] - bounds[0]).max(1.0))
                        .min(156.0 / (bounds[3] - bounds[1]).max(1.0))
                        .clamp(0.125, 4.0);
                    renderer.preview(&mut self.paperdoll, &[Draw::Mesh(doll)], &camera)?;
                    let mut bitmap = Bitmap {
                        width: 92,
                        height: 164,
                        registration: [0; 2],
                        pixels: vec![0; 92 * 164],
                    };
                    for (i, pixel) in bitmap.pixels.iter_mut().enumerate() {
                        if self.paperdoll.pick(i % 92, i / 92).is_some() {
                            *pixel = 0xff000000 | self.paperdoll.pixels[i];
                        }
                    }
                    c.bitmap(&bitmap, sx + 48, 78);
                }
                c.text(&self.font, "EQUIPMENT PREVIEW", sx + 24, 253, 152);
                c.text(&self.font, "Select a slot to inspect", sx + 14, 268, 162);
            } else if self.top == Tab::Stats {
                c.text(&self.font, "CHARACTER STATS", sx + 28, 52, 135);
                for (i, name) in [
                    "Level    --",
                    "Health   --",
                    "Mana     --",
                    "Fatigue  --",
                    "Strength --",
                    "Agility  --",
                ]
                .iter()
                .enumerate()
                {
                    c.text(&self.font, name, sx + 28, 75 + i as i32 * 17, 140);
                }
            } else {
                c.text(&self.font, "SPELL BOOK", sx + 35, 58, 130);
                c.text(&self.font, "No spells loaded", sx + 27, 86, 140);
            }
            match self.bottom {
                Tab::Inventory => {
                    c.bitmap(image("inventory", "Inventory"), sx, 306);
                    for (name, x, y) in [
                        ("Backpack", 10, 10),
                        ("GoldPile", 40, 5),
                        ("InvArwLG", 140, 11),
                        ("InvArwRG", 161, 12),
                    ] {
                        c.bitmap(image("inventory", name), sx + x, 306 + y);
                    }
                    c.text(&self.font, "--", sx + 80, 319, 45);
                }
                Tab::Map => {
                    // A terrain preview from loaded walk cells, not explored-map state.
                    if let Some(actor) = actor {
                        for y in 0..110 {
                            for x in 0..148 {
                                let dx = (x - 74) * 16;
                                let dy = (y - 55) * 16;
                                let p = Position {
                                    x: actor.position.x + dy + dx / 2,
                                    y: actor.position.y + dy - dx / 2,
                                    z: 0,
                                };
                                if scene.walkmap.height(level, p) > 0 {
                                    c.fill(Rect::new(sx + 20 + x, 338 + y, 1, 1), 0xff796b52);
                                }
                            }
                        }
                    }
                    c.bitmap(image("automap", "Amap"), sx, 306);
                    if actor.is_some() {
                        // Marker is a 4x4 grid of blip variants, not one sprite.
                        // Match the blue player cell selected in uimaptest.cpp.
                        c.blit(
                            image("automap", "Marker"),
                            Rect::new(8, 12, 4, 4),
                            Rect::new(sx + 92, 391, 4, 4),
                        );
                    }
                    c.text(&self.font, "LOADED TERRAIN", sx + 30, 453, 135);
                }
                _ => {
                    c.bitmap(image("spellpane", "spellconstr"), sx, 306);
                    c.text(&self.font, "No talismans loaded", sx + 20, 450, 148);
                }
            }
        }
        let strip = layout.tab_strip();
        c.bitmap(image("sidebartabsnotex", "Up"), strip.x, strip.y);
        for (i, tab) in TABS.into_iter().enumerate() {
            let selected = self.sidebar && (self.top == tab || self.bottom == tab);
            if selected || self.pressed == Some(Control::Tab(tab)) {
                let r = layout.tab(i);
                c.blit(
                    image("sidebartabsnotex", "Down"),
                    Rect::new(r.x - strip.x, r.y - strip.y, r.w, r.h),
                    r,
                );
            }
            if self.hover == Some(Control::Tab(tab)) {
                let r = layout.tab(i);
                c.blit(
                    image("sidebartabsnotex", "Select"),
                    Rect::new(r.x - strip.x, r.y - strip.y, r.w, r.h),
                    r,
                );
            }
        }
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn layout_capture_and_camera_share_scaled_geometry() {
        for viewport in [[640, 480], [1024, 768], [320, 240]] {
            let layout = Layout::new(viewport, true, true);
            let world = layout.world_viewport();
            assert!(layout.hit_test([viewport[0] as f64 - 1.0, 1.0]));
            assert!(layout.hit_test([1.0, viewport[1] as f64 - 1.0]));
            assert!(!layout.hit_test([world[0] as f64 * 0.5, world[1] as f64 * 0.5]));
            let camera = Camera {
                center: [123.0, 456.0],
                zoom: 1.5,
            };
            let screen = layout
                .world_camera(camera, viewport)
                .screen(camera.center, viewport);
            for i in 0..2 {
                assert!((screen[i] - world[i] as f64 * 0.5).abs() < 1e-9);
            }
        }
    }
}
