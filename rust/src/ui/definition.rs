// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             definition.rs - Native menu layouts and widget art        *
// *************************************************************************

use super::canvas::{Canvas, Rect};
use crate::assets::{bitmap::Bitmap, definition::lines, font::Font, multi::Multi, vfs::Vfs};
use anyhow::{Context, Result, ensure};
use std::collections::BTreeMap;

#[derive(Clone, Debug)]
pub struct Widget {
    pub kind: String,
    pub name: String,
    pub text: String,
    pub field: String,
    pub rect: Rect,
    pub toggle: bool,
    pub right: bool,
}
#[derive(Default)]
struct Style {
    up: String,
    down: String,
    frame: Option<[i32; 4]>,
    margins: [i32; 4],
    center: bool,
}
pub struct Pane {
    pub rect: Rect,
    pub widgets: Vec<Widget>,
    art: BTreeMap<String, Bitmap>,
    styles: BTreeMap<String, Style>,
}
fn value<'a>(line: &'a [String], key: &str) -> Option<&'a str> {
    let start = if line.first().is_some_and(|s| s == "STYLE") {
        2
    } else {
        1
    };
    line.iter()
        .enumerate()
        .skip(start)
        .find(|(_, s)| *s == key)
        .and_then(|(i, _)| line.get(i + 1))
        .map(String::as_str)
}
fn four(line: &[String], key: &str) -> Result<Option<[i32; 4]>> {
    let start = if line.first().is_some_and(|s| s == "STYLE") {
        2
    } else {
        1
    };
    let Some((at, _)) = line.iter().enumerate().skip(start).find(|(_, s)| *s == key) else {
        return Ok(None);
    };
    let nums = line
        .get(at + 1..at + 5)
        .with_context(|| format!("short {key}"))?;
    let mut out = [0; 4];
    for (n, s) in out.iter_mut().zip(nums) {
        *n = s.parse()?;
    }
    Ok(Some(out))
}
fn panel(lines: &[Vec<String>], name: &str, origin: Rect) -> Result<Vec<Widget>> {
    let mut active = false;
    let mut depth = 0;
    let mut widgets = Vec::new();
    for line in lines {
        match line[0].as_str() {
            "PANEL" => {
                active = line.get(1).is_some_and(|s| s.eq_ignore_ascii_case(name));
                depth = 0;
            }
            "BEGIN" if active => depth += 1,
            "END" if active => {
                depth -= 1;
                if depth == 0 {
                    break;
                }
            }
            "BUTTON" | "TEXT" | "FRAME" | "BITMAP" | "SCROLLBAR" | "LISTBOX"
                if active && depth == 1 =>
            {
                let [x, y, w, h] = four(line, "POS")?.context("widget without POS")?;
                ensure!(
                    w > 0 && h > 0 && w <= 4096 && h <= 4096,
                    "invalid widget extent"
                );
                widgets.push(Widget {
                    kind: line[0].clone(),
                    name: value(line, "NAME").unwrap_or("").into(),
                    text: value(line, "TEXT").unwrap_or("").into(),
                    field: value(line, "FIELD").unwrap_or("").into(),
                    rect: Rect::new(origin.x + x, origin.y + y, w, h),
                    toggle: line.iter().any(|s| s == "BTNFLAG_TOGGLE"),
                    right: line
                        .iter()
                        .any(|s| matches!(s.as_str(), "TEXTFLAG_RIGHT" | "TEXT_RIGHT")),
                });
            }
            _ => {}
        }
    }
    ensure!(!widgets.is_empty(), "missing/empty DEF panel {name}");
    Ok(widgets)
}
impl Pane {
    pub fn load(
        vfs: &mut Vfs,
        definition: &str,
        name: &str,
        variant: &str,
        rect: Rect,
    ) -> Result<Self> {
        let mut source = lines(&vfs.read("widgets.def")?)?;
        let own = lines(&vfs.read(&format!("{definition}.def"))?)?;
        let mut widgets = panel(&own, name, rect)?;
        source.extend(own);
        let mut styles = BTreeMap::new();
        for line in source.iter().filter(|line| line[0] == "STYLE") {
            let kind = line.get(1).context("STYLE without type")?;
            let variant = match kind.as_str() {
                "BUTTON" | "SCROLLBAR" | "LISTBOX" => line.get(2).map_or("", String::as_str),
                _ => "",
            };
            styles.insert(
                format!("{kind}:{variant}"),
                Style {
                    up: if kind == "BUTTON" {
                        value(line, "UP")
                    } else {
                        value(line, "BITMAP").or_else(|| value(line, "BGBITMAP"))
                    }
                    .unwrap_or("")
                    .into(),
                    down: value(line, "DOWN").unwrap_or("").into(),
                    frame: four(line, "FRAME")?,
                    margins: four(line, "MARGINS")?.unwrap_or([0; 4]),
                    center: !line.iter().any(|s| s == "NOCENTER"),
                },
            );
        }
        let mut art = BTreeMap::new();
        for file in [
            format!("widgets{variant}.dat"),
            format!("{definition}{variant}.dat"),
        ] {
            let bytes = vfs.read(&file)?;
            let multi = Multi::parse(&bytes)?;
            for entry in multi.names() {
                art.insert(entry.to_ascii_lowercase(), multi.bitmap_native(entry)?);
            }
        }
        // The scrollbar constructor takes its thickness from the artwork,
        // less its margins, rather than the DEF's requested thickness.
        if let (Some(style), Some(track)) = (styles.get("SCROLLBAR:HSCROLL"), art.get("hscrollbar"))
        {
            for widget in &mut widgets {
                if widget.kind == "SCROLLBAR" {
                    widget.rect.h = track.height as i32 - style.margins[1] - style.margins[3];
                }
            }
        }
        Ok(Self {
            rect,
            widgets,
            art,
            styles,
        })
    }
    fn art(&self, name: &str) -> Option<&Bitmap> {
        self.art.get(&name.to_ascii_lowercase())
    }
    fn frame(&self, canvas: &mut Canvas<'_>, style: &str, down: bool, rect: Rect) {
        let Some(style) = self.styles.get(style) else {
            return;
        };
        let Some(bitmap) = self.art(if down && !style.down.is_empty() {
            &style.down
        } else {
            &style.up
        }) else {
            return;
        };
        if let Some(insets) = style.frame {
            canvas.tiled_frame(bitmap, rect, insets, style.margins, style.center);
        } else {
            canvas.bitmap(bitmap, rect.x, rect.y);
        }
    }
    pub fn background(&self, canvas: &mut Canvas<'_>, origin: [i32; 2]) {
        if let Some(bitmap) = self.art("Background") {
            canvas.bitmap(bitmap, self.rect.x + origin[0], self.rect.y + origin[1]);
        }
    }
    pub fn label(
        canvas: &mut Canvas<'_>,
        font: &Font,
        text: &str,
        rect: Rect,
        align: i8,
        color: u32,
        shadow: bool,
    ) {
        let x = match align {
            0 => rect.x + (rect.w - font.width(text)) / 2,
            1 => rect.x + rect.w - font.width(text),
            _ => rect.x,
        };
        let y = rect.y + (rect.h - font.height) / 2;
        if shadow {
            for [dx, dy] in [[2, 1], [3, 1], [2, 2]] {
                canvas.text_color(font, text, Rect::new(x + dx, y + dy, rect.w, rect.h), 0);
            }
        }
        canvas.text_color(font, text, Rect::new(x, y, rect.w, rect.h), color);
    }
    pub fn widget(
        &self,
        canvas: &mut Canvas<'_>,
        font: &Font,
        widget: &Widget,
        origin: [i32; 2],
        state: WidgetState,
    ) {
        let rect = Rect::new(
            widget.rect.x + origin[0],
            widget.rect.y + origin[1],
            widget.rect.w,
            widget.rect.h,
        );
        let color = if state.disabled {
            0x888888
        } else if state.down && !widget.toggle {
            0
        } else if state.focused {
            0xffff00
        } else {
            0xffffff
        };
        match widget.kind.as_str() {
            "BITMAP" => {
                if let Some(bitmap) = self.art(&widget.field) {
                    canvas.bitmap(bitmap, rect.x, rect.y);
                }
            }
            "FRAME" => self.frame(canvas, "FRAME:", false, rect),
            "TEXT" => Self::label(
                canvas,
                font,
                &widget.text,
                Rect::new(rect.x + 3, rect.y, rect.w - 6, rect.h),
                if widget.right { 1 } else { -1 },
                color,
                true,
            ),
            "BUTTON" => {
                self.frame(
                    canvas,
                    if widget.toggle {
                        "BUTTON:TOGGLE"
                    } else {
                        "BUTTON:NORMAL"
                    },
                    state.down,
                    rect,
                );
                let text_rect = if widget.toggle {
                    Rect::new(rect.x + 18, rect.y - 1, rect.w - 18, font.height)
                } else {
                    rect
                };
                Self::label(
                    canvas,
                    font,
                    &widget.text,
                    text_rect,
                    if widget.toggle { -1 } else { 0 },
                    color,
                    !state.down || widget.toggle,
                );
            }
            "SCROLLBAR" => {
                // widgets.def's horizontal track, arrows and native thumb.
                // The track's UP/DOWN sprites are not its BGBITMAP.
                if let Some(track) = self.art("HScrollBar") {
                    canvas.tiled_frame(track, rect, [22, 6, 22, 6], [0, 0, 4, 4], true);
                }
                for (name, x) in [
                    ("HScrollUpU", rect.x + 1),
                    ("HScrollDownU", rect.x + rect.w - 12),
                ] {
                    if let Some(bitmap) = self.art(name) {
                        canvas.bitmap(bitmap, x, rect.y + 1);
                    }
                }
                if let Some(thumb) = self.art("HScrollThumbU") {
                    let travel = (rect.w - 19 - 30).max(0);
                    canvas.bitmap(
                        thumb,
                        rect.x + 19 + travel * i32::from(state.value) / 100,
                        rect.y + 2,
                    );
                }
            }
            "LISTBOX" => {
                // Read-only binding display until rebinding is ported.
                self.frame(canvas, "LISTBOX:VLIST", false, rect);
                for (name, x, y) in [
                    ("VScrollUpU", rect.x + rect.w - 20, rect.y + 5),
                    ("VScrollDownU", rect.x + rect.w - 20, rect.y + rect.h - 16),
                    ("VScrollThumbU", rect.x + rect.w - 19, rect.y + 23),
                ] {
                    if let Some(bitmap) = self.art(name) {
                        canvas.bitmap(bitmap, x, y);
                    }
                }
                for (row, key) in ["W", "S", "A", "D", "-", "-", "-", "-"].iter().enumerate() {
                    for (x, text) in [(rect.x + 185, *key), (rect.x + 335, "-")] {
                        Self::label(
                            canvas,
                            font,
                            text,
                            Rect::new(x, rect.y + 5 + row as i32 * 20, 150, 20),
                            0,
                            0xffff64,
                            true,
                        );
                    }
                }
            }
            _ => {}
        }
    }
}
#[derive(Default)]
pub struct WidgetState {
    pub down: bool,
    pub focused: bool,
    pub disabled: bool,
    pub value: u8,
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn panel_ignores_other_panels_and_nested_list_fields() -> Result<()> {
        let source = lines(
            br#"PANEL "other"
BEGIN
BUTTON POS 0 0 1 1
END
PANEL "wanted"
BEGIN
BUTTON POS 1 2 30 20 NAME "ok" TEXT "OK"
TEXT POS 1 2 30 20 TEXT "Title"
LISTBOX POS 0 0 200 100
BEGIN
FIELD 0 0 100 20 "key"
END
END"#,
        )?;
        let widgets = panel(&source, "wanted", Rect::new(126, 65, 394, 316))?;
        assert_eq!(widgets.len(), 3);
        assert_eq!(widgets[0].rect, Rect::new(127, 67, 30, 20));
        assert_eq!(widgets[0].name, "ok");
        assert_eq!(widgets[1].text, "Title");
        let frame_style = lines(br#"STYLE FRAME BITMAP "ClearFrame1" FRAME 14 14 16 16"#)?;
        assert_eq!(four(&frame_style[0], "FRAME")?, Some([14, 14, 16, 16]));
        Ok(())
    }
}
