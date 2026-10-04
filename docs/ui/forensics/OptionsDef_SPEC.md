# LoadDef_options (TOptionsScreen) — Reconstruction SPEC

**Class:** `cls_0x5b9744` (vtable @ `0x005b9744`, 46 slots, 184 B). This **IS a dedicated class** — the task brief listed it as TBD but the recon class table confirms a DEF-screen subclass with its own ctor, dtor, Initialize, Close, input override, and an "Apply settings" slot. Working candidate name: **TOptionsScreen** (no string anchor to confirm a `T*` identity in the binary; see §14 UNCONFIRMED-A).

**Address fingerprints:**
- ctor body `0x0053bdc0` (`recon/classes/cls_0x5b9744.cpp:158-178`).
- dtor body `0x0053be20` (`:184-209`).
- vector-deleting dtor (slot 124) `0x0053beb0` (`:215-225`).
- **Initialize** (vtable slot 0) `0x0053a8b0` — opens DEF screen `"options"` + clones the global controller key-map for editing (`:46-119`).
- **Close** (vtable slot 1) `0x0053aa60` — frees the rebind buffer + delegates to TButtonPane Close (`:125-134`).
- **DispatchInput** override (slot 108) `0x0053b9e0` — routes keys to rebind path when a controller LISTBOX row is selected, else to default DefWidget_DispatchInput (`:140-152`).
- **OnControl / Apply settings** (slot 148) `0x0053aa90` — **BODY EXTRACTED 2026-05-29** to `/tmp/options_decomp/apply_53aa90.cpp` (2875 B, force-created via `DecompileAddr.java 0x0053aa90`). The slot is a **multiplexed OnControl handler** (NOT just Apply): the `param_3` arg is a 32-bit message-class id (1 / 3000 / 0xbb9 / 0xbba / 4000 / 0x1389 / 0x138a). The "apply settings" path is the `param_3 == 3000` + `name == "ok"` branch (`:347-397`). See §6.3, §6.7 (new), §10.5 (new — command id table).
- Rebind key accumulator (helper, in cls_0x5b98b8 file) `0x0053ba80` (`recon/classes/cls_0x5b98b8_likely_MultiplayerSystem.cpp:2074-2213`).

**Instance size:** 456 B (`recon/classes/cls_0x5b9744.cpp:40`).
**Global instance:** `DAT_0066fcc0` (TPlayScreen dispatch `case 0x52` passes its address as the input-grab target — `recon/discovered/FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:469`).

**Status (2026-05-29):** `forensics-complete` for the static layout + the full vtable + the **OnControl/Apply body** + the **command-id↔button mapping** + the **FIELD→getter table**. The remaining unknowns (§14) are bracket-marked and none are required for static visual reconstruction; the runtime-behavior unknowns reduced from 9 to 3 (slider value ranges, LISTBOX multi-column row layout, font-id low-byte for "Med"). See §14 for the revised UNCONFIRMED list.

> **NOTE on coordinate frames.** This is a DEF-engine screen (same family as TCharCreateScreen — see [CharCreate_SPEC.md](CharCreate_SPEC.md) §3 for the architecture rationale, which applies verbatim here). Every visible element's `(x,y,w,h)` is a literal `POS x y w h` in `data/Resources/options.def`, evaluated as **pane-local** by `DefWidget_DispatchControl @ 0x436ec0`. There is no chrome-surface / scratch-surface composition stack at the screen level. The forensics rule "every spec coordinate is a final, composed, single-space value" reduces to: **every `(x,y)` in §4 is `pane-local`; pane origin is `screen:(0,0)`** because `DefScreen_Open` is called with x=0, y=0 at full display size 640×480. See §3.

---

## §0 — Sources & status

### Recon files read (all `recon/...` relative to `/Users/benjamincooley/projects/RevenantRevisited/worktrees/ui`)
- `recon/classes/cls_0x5b9744.cpp` — class struct (456 B) + 6 methods (Init, Close-base, DispatchInput override, ctor, dtor body, vector-dtor) (1).
- `recon/classes/cls_0x5b9744__vftable_5b9744.cpp` — 46-slot vtable (184 B) (2).
- `recon/ghidra/cls_0x5b9744.cpp` — identical Ghidra extract; cross-checked (1) to (2).
- `recon/classes/cls_0x5b98b8_likely_MultiplayerSystem.cpp:2074-2213` — `cls_0x5b98b8::meth_0x53ba80` (key-rebind accumulator). This file is **misnamed** — `cls_0x5b98b8` is the DEF-screen TButtonPane intermediate, not a multiplayer class; OOAnalyzer flattened DEF-engine and MP-system sibling methods into the same bucket. Same hazard pattern as `cls_0x5b93c4_TPlayer.cpp` (see [[feedback-recon-agent-protocol]] and `recon/discovered/renames/agent_ui_wave4_oog_closeout.txt:14-29`).
- `recon/classes/cls_0x439000.cpp` — `cls_0x439000` (the **TControlList / key-bind manager**): `meth_0x439060` reads the 19-dword (0x4c-byte) record for a given controller index out of the global table at `UNK_0065a904.field_0xa4`. Used by Init to snapshot the active bindings into `mbr_0x1ac` (the in-screen edit buffer), and by `meth_0x53ba80` to validate-and-store user-pressed keys.
- `recon/discovered/FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:466-471` — the entry hook (case 0x52 = the player's "Options" hotkey / menu command):
  ```c
  case 0x52:
      _DAT_0066fe3c = 1;                                  // = (&DAT_0066fcc0)->mbr_0x17c, gates flags=0x11
      FUN_0053a8b0_LoadDef_options();                     // virt_meth_0x53a8b0 on DAT_0066fcc0
      FUN_0048f040(&DAT_0066fcc0, (-(uint)(DAT_0066829c != 0) & 0xfffffff8) + 8 | 7);
      FUN_0053aa60();                                     // virt_meth_0x53aa60 — Close-and-clear rebind buffer
      _DAT_0065cb40 = 1;                                  // "screen is dirty, refresh next frame"
      break;
  ```
- `recon/discovered/FUN_00435150_DefScreen_Open.cpp` — slot-132 helper called by Init.
- `recon/discovered/FUN_00435040_DefScreen_LoadAndShow.cpp` — sub-helper that copies (w,h,hi_w,hi_h) into pane fields and triggers `DefWidget_LoadFile` for `options.def`.
- `recon/discovered/cls_0x5a45c8_intermediate_init_434e40.cpp` — `TButtonPane_Initialize` (`FUN_00434e40`) base-init called from `DefScreen_Open`.
- `recon/discovered/port_status/DefWidgetEngine.md` — engine status, widget roster (8 widget classes: BITMAP/FRAME/TEXT/BUTTON/SCROLLBAR/LISTBOX/EDIT/DROPLIST), options entry @ `0x53a8b0` (row "options" `:68`).
- `recon/discovered/renames/agent_ui_wave2_defengine.txt:299-302` — "options" string anchor + label confirmation.
- `recon/discovered/renames/agent_ui_wave4_sidebar_cascade.txt:101-106` — confirms F1..F5 family (F2=0x52=Options is one of them).
- `recon/classes/_data.txt:85789-85803` — global controller-list count `DAT_005d697c = 0x45 = 69` (number of controllers); list of controller display strings (`Invoke 1`..`Lock-Pick` etc.) starts at `0x005d6980`.
- `recon/classes/_data.txt:87035-87389` — XREFs for the 11 setting globals + the Init/Apply read/write evidence pairs.
- `recon/classes/_data.txt:137213-137231` — confirms `DAT_0066fcc0` is the singleton TOptionsScreen instance (the 456-byte struct lives at `0x0066fcc0..0x0066fe88`; `mbr_0x17c` falls at `0x0066fe3c`, matching the `_DAT_0066fe3c = 1` in the dispatch).
- `data/Resources/options.def` — the active panel definition (verbatim, see §4).
- `data/Resources/widgets.def` — the style library loaded by name `"widgets"` in DefScreen_Open (every BUTTON/EDIT/SCROLLBAR/LISTBOX style cited in §4 inherits from one of these `STYLE` blocks).
- [CharCreate_SPEC.md](CharCreate_SPEC.md) §0 / §3 / §6.4 / §6.5 — sibling DEF-screen for architecture reuse (DefScreen_Open, DefScreen_LoadAndShow, frame-graph rule).
- [UI_METHOD_MAP.md](UI_METHOD_MAP.md) §5 (text), §12 (port primitives), §15a (surface vtable), §16 (chroma key) — referenced; not re-derived.

### Asset measurements (via `tools/ui/dump_dat.py`)
- `optionsnotex.dat`: **1 bitmap, 640×480, "Background", flags=0x2 (BM_15BIT), kc=0x0**. Visually: engraved iron frame, gold "OPTIONS" title plate inset top-center.
- `optionsalpha.dat`: 1 bitmap, 640×480 (alpha variant of same chrome).
- `optionstex.dat`: 1 bitmap, 640×480, flags=0x10000 (16-bit texture variant; current `dump_dat.py` reports "unsupported bit-depth" — header confirms identical 640×480).
- `widgetsnotex.dat`: 39 bitmaps — the standard 9-slice frames (`ClearFrame1..4`, `FillFrame1..4`, `TransFrame1..4` = 64×57), checkboxes (`CheckU/D` = 16×17), radios (`RadioU/D` = 18×16/17), spinner arrows (`SpinUpU/D` = 20×11; `SpinDownU/D` = 20×13), dropdown (`DropBtnU/D` = 24×22; `DropRect` = 74×82), V-scrollbar (`VScrollBar` = 22×74; `VScrollRect` = 74×82; `VScrollUpU/D`, `VScrollDownU/D` = 16×11; `VScrollThumbU/D` = 14×13), H-scrollbar (`HScrollBar` = 74×21; `HScrollRect` = 82×73; `HScrollUpU/D`, `HScrollDownU/D` = 12×15; `HScrollThumbU/D` = 14×13). All flags `0x402` (BM_15BIT|BM_TRANSPARENT), kc=`0x7c1f` (RGB555 magenta). **No options-specific atlas exists**; every interactive control comes from `widgets*.dat`.

### Snapshot note (supplementary — superseded by retail)
There is **no pre-release `src/options.*`** in the snapshot (`grep -rln 'TOptions\|optionsscreen' src/ RevenantRepo/` returns empty in master). The DEF widget engine itself is a late retail addition (per `DefWidgetEngine.md:5`), so the entire LoadDef_options screen is retail-only. The setting *globals* it manipulates do exist in the snapshot (`src/revmain.cpp`): `DrawRealtimeShadows`, `AutoBeginCombat`, `PlaySpeech`, `ShowDialog`, `ViolenceLevel`, `GameSpeed`, plus `audio::SetMusicVolume` and `TSound::sound_volume` — these names give semantic context for the per-global slots but do not constrain layout (which comes from `options.def`).

---

## §1 — Overview

`LoadDef_options` is the **in-game options screen** reached from MainMenu `MenuOptions` (TLogoScreen menu — `agent_screens.txt:35`) and from the in-game pause menu (TPlayScreen `case 0x52`). It is a **full-display 640×480 modal screen** rendered by the **DEF widget engine** from `options.def` + the shared `widgets.def` style library.

Visually it is the "OPTIONS" framed chrome from `optionsnotex.dat` (engraved metal frame + gold-on-black "OPTIONS" title plate at top-center). Inside the frame, the DEF parser instantiates four logical regions:

1. **7 gameplay/audio toggles** (CheckU/CheckD checkboxes with right-side labels) along the left edge: Enable Real Time Lights, Automatically Begin Combat, Play Audio Dialog, Always Face Enemy In Combat, Use Enhanced 3D Lighting, Limit Game Speed, No Combat Results.
2. **4 horizontal sliders** (HSCROLLBAR) on the right: Violence Level, Music Volume, SFX Volume, Gamma Level — each with a right-aligned label.
3. **Controller / key rebinding** in the lower half: a single VLIST `LISTBOX` named `controller` (512×170) displaying the global key-binding table, with three column headers ("Control Name" / "Key 1" / "Key 2"), 8 fixed name labels on the left (`name0..name7` = Up/Down/Left/Right/Punch/Kick/Thrust/Use — these are **decorative; the LISTBOX is the source of truth** — see §14 UNCONFIRMED-D).
4. **OK / Cancel** buttons centered at the bottom (180,410) / (360,410).

**When shown:** The screen is opened when the user presses the Options command (F2 = command id `0x52`; see UNCONFIRMED-C for the actual VK code). **When hidden:** OK or Cancel button click. The screen is modal (the parent screen pauses input dispatch via `FUN_0048f040(&DAT_0066fcc0, …)` in case 0x52 — that helper redirects input to the options pane until it closes).

**Apply-on-OK semantics.** Init clones the live setting globals into `mbr_0x180..0x1a8` (11 fields) and snapshots the 69-entry controller key-map into a `malloc`'d edit buffer at `mbr_0x1ac`. The 8 widgets bound to those fields edit the *snapshot*. The Apply slot (`virt_meth_0x53aa90`) writes the snapshot back to the live globals (XREFs prove all 11 writes — see §6.3). Cancel discards via `meth_0x53aa60` (which frees the rebind buffer but does NOT roll back `mbr_0x180..0x1a8`; the live globals are simply never written if Apply was not invoked).

---

## §2 — Asset roster

| asset | archive | entry name | WxH (measured) | role | source rects | cite |
|-------|---------|------------|----------------|------|--------------|------|
| `Background` (Classic 15-bit) | `optionsnotex.dat` | `Background` | **640×480** | Full-screen chrome (engraved iron frame + "OPTIONS" gold title plate); blitted as the BG image of the DefScreen | full image (0,0,640,480) | dump: `/tmp/options_dump/optionsnotex.json` (PNG header 640×480, flags=0x2) |
| `Background` (16-bit) | `optionstex.dat` | `Background` | **640×480** | Same chrome for hi-color path | full image | dump: `/tmp/optionstex_dump/optionstex.json` (header 640×480 flags=0x10000; decoder pending) |
| `s_background_005cd8d4` token "background" | n/a — engine-side | "background" | logical lookup string | DefScreen_LoadAndShow passes this to `vtable+0xa0` to resolve the chrome handle at runtime | n/a | `FUN_00435040_DefScreen_LoadAndShow.cpp:31` |
| widget-style assets (loaded via `widgets.def` per control) | `widgetsnotex.dat` (39 entries — see measurements above) | `ClearFrame1..4`, `FillFrame1..4`, `TransFrame1..4`, `CheckU/D`, `RadioU/D`, `HScrollBar`, `HScrollThumbU/D`, `HScrollUpU/D`, `HScrollDownU/D`, `HScrollRect`, `VScrollBar`, `VScrollUpU/D`, `VScrollDownU/D`, `VScrollThumbU/D`, `VScrollRect`, `SpinUpU/D`, `SpinDownU/D`, `DropBtnU/D`, `DropRect` | per-asset (see widgets_dump dims above) | DEF widget chrome (button frames, checkbox bitmaps, sliders, listbox scrollbar) | per widget per `widgets.def:81-145` STYLE blocks | `widgets.def:81-145`; `/tmp/widgets_dump/widgetsnotex.json` |

**Atlas note:** there is no atlas in `optionsnotex.dat`. The chrome is a single full-screen image; every widget bitmap comes from the global `widgets*.dat` pool.

---

## §3 — Coordinate frames & surfaces

### Screen geometry (cited from Init body)

`virt_meth_0x53a8b0` calls
```c
cls_0x5b93c4::virt_meth_0x435150(  // = DefScreen_Open
    this,
    s_options_005e4494,   // panel_name = "options"
    s_default_005e448c,   // screen_root_name = "default"  (the PANEL "default" block in options.def:4)
    -(uint)(this->mbr_0x17c != 0) & 0x11,  // flags = 0x11 if mbr_0x17c!=0 else 0
    0, 0,                 // x, y  (pane TL is at screen 0,0)
    0x280, 0x1e0,         // w=640, h=480  (full display)
    0x1c2, 0xa0,          // hi_w=450, hi_h=160   (hi-res sub-content target — see CharCreate_SPEC §3 for the field-mapping)
    s_widgets_005e4484    // widgets_lib = "widgets"  (loads widgets.def style library)
);                        // NOTE: no 12th arg → bg_dat is NULL; chrome resolves via the
                          //       "background" lookup token in DefScreen_LoadAndShow:31
```
Citations: `recon/classes/cls_0x5b9744.cpp:62-64`; `recon/discovered/FUN_00435150_DefScreen_Open.cpp:1-60`; `recon/discovered/FUN_00435040_DefScreen_LoadAndShow.cpp:1-63`.

- Pane outer rect = `mbr_0xc/0x1c = 0x280 = 640` (right) × `mbr_0x10/0x20 = 0x1e0 = 480` (bottom), `mbr_0x14/0x18 = 0` (left/top). Citations: ctor `:161-169`.
- **Pane width field** (`this+0xc` per UI_METHOD_MAP §15 family) = **640** constant; this pane does not resize in retail.
- The "flag bit `0x10`" half of `0x11` is **UNCONFIRMED-E**: `_DAT_0066fe3c = 1` (set by case 0x52 just before Initialize) gates the flag to 0x11; the bit-`0x1` half is `flags & 1` in DefScreen_Open → swaps the MP widget cache (`DAT_0066733c` vs `DAT_0065bb10`, `FUN_00435150_DefScreen_Open.cpp:21,33`). The bit-`0x10` purpose is not pinned.

### Frame table (mandatory)

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|-------|--------|--------|--------------------------------------|--------------------------|------|
| **screen** | (root) | TL | `(0,0)` | framebuffer | trivial |
| **pane** | screen | TL | `(0,0)` (DefScreen_Open args `x=0,y=0`) | the whole Options screen | `cls_0x5b9744.cpp:62-64` (call args) |
| **chrome (Background bitmap)** | pane | TL | `(0,0)` | the 640×480 engraved-iron frame + "OPTIONS" title plate | `FUN_00435040_DefScreen_LoadAndShow.cpp:31` (token = `"background"`) |

All DEF widgets evaluate their `POS x y w h` in **pane-local** = **screen** coordinates (NO sub-frame). This is the same composition rule as TCharCreateScreen — see [CharCreate_SPEC.md](CharCreate_SPEC.md) §3 for the rationale and the `DefWidget_DispatchControl` evidence.

### Composition formula
```
screen_x = pane_local_x = options.def POS x
screen_y = pane_local_y = options.def POS y
```

There are **no sub-frame containers in this screen** — `options.def` has only commented-out `FRAME POS 50 200 530 200` and `FRAME POS 60 220 512 170` (both `//`-commented, `options.def:31,52`). Without FRAMEs, every widget's POS is directly pane-local. This makes the spec extremely simple: §4 is just the verbatim DEF.

### Scratch / mosaic surfaces at the screen level

- **`malloc` rebind buffer.** Init allocates `mbr_0x1ac = malloc(DAT_005d697c * 0x24) = malloc(69 * 36) = 2484 bytes` (`cls_0x5b9744.cpp:93-94`). The loop at `:95-115` walks all 69 controllers and copies 9 dwords per entry (3 outer × 3 inner; each inner iter calls `meth_0x439060` which fills 0x13=19 dwords into `local_40[16]` and 4 bytes get tee'd off into the edit buffer per call). Layout per entry: **9 dwords = 36 bytes (0x24)** — see §6.2 for the exact decode. This buffer is the editable mirror; Close (`virt_meth_0x53aa60`) frees it; Apply (`virt_meth_0x53aa90`) writes it back via the inverse helper. This is **data, not a draw surface**.
- **No off-screen draw scratch** is allocated at the screen level (cf. CharCreate which allocates a 100×100 portrait scratch). Every widget owns its own scratch through its `BGBITMAP` STYLE (e.g. `EDIT.SPIN` has `BGBITMAP "ClearFrame4"`).

### Layout diagram (pane = screen, units = pane-local px)

```
pane  (640x480, origin = screen 0,0)                                    chrome = optionsnotex.Background
┌────────────────────────────────────────────────────────────────────────────────┐
│                  OPTIONS  (gold-on-black title plate, baked into chrome)       │
│                                                                                │
│   55              305 260                          420            580          │
│  ┌──┐                                       Violence Level   ▕━━━━━━━━━━━━▏    │ y=60..89   (toggle1, slider1)
│  ☐  Enable Real Time Lights                                                    │
│  ☐  Automatically Begin Combat              Music Volume     ▕━━━━━━━━━━━━▏    │ y=80..119  (toggle2, slider2)
│  ☐  Play Audio Dialog                                                          │
│  ☐  Always Face Enemy In Combat             SFX Volume       ▕━━━━━━━━━━━━▏    │ y=100..139 (toggle3, slider3)
│  ☐  Use Enhanced 3D Lighting                                                   │
│  ☐  Limit Game Speed                        Gamma Level      ▕━━━━━━━━━━━━▏    │ y=120..169 (toggle4..6, slider4)
│  ☐  No Combat Results                                                          │ y=180
│                                                                                │
│   70                       300              455                                │
│  Control Name              Key 1            Key 2                              │ y=200 (header row)
│  60                                                                       572  │
│  ┌──────────────────────────────────────────────────────────────────────────┐ │
│  │  Up                Up Arrow          ─                                     │ │
│  │  Down              Down Arrow                                              │ │
│  │  Left              Left Arrow                                              │ │
│  │  Right             Right Arrow                  controller LISTBOX (VLIST) │ │  y=220..389
│  │  Punch             Z              Numpad-Z                                 │ │  512x170
│  │  Kick              X                                                       │ │
│  │  Thrust            C                                                       │ │
│  │  Use               Space                                                   │ │
│  └──────────────────────────────────────────────────────────────────────────┘ │
│                                                                                │
│                ┌──────────┐                ┌──────────┐                        │
│                │    OK    │                │  Cancel  │                        │ y=410..439
│                └──────────┘                └──────────┘                        │
│                180,410 100x30              360,410 100x30                      │
└────────────────────────────────────────────────────────────────────────────────┘
```

(The 8 `name0..name7` TEXT widgets at the left (70, 224/244/264/...) are **decorative pane-local labels overlaid on the LISTBOX area**, presumably to keep the canonical Up/Down/Left/Right/Punch/Kick/Thrust/Use names visible even when the LISTBOX scrolls past those rows. See UNCONFIRMED-D for why they are not the actual data source.)

---

## §4 — Static element layout

> All coords below are **pane-local** = **screen** (pane origin = `(0,0)`). No sub-frame anchoring; FRAME containers in `options.def` are commented-out.

### §4.1 — Toggle buttons (7, left column, top half)

| element (NAME) | space | (x, y) | (w, h) | type | text | flags | cite |
|-------|-------|--------|--------|------|------|-------|------|
| `Realtime` | pane | (55, 60) | 250 × 20 | TOGGLE BUTTON | "Enable Real Time Lights" | `BTNFLAG_TOGGLE \| BTNFLAG_DRAWTOGGLE` | `options.def:10` |
| `Auto` | pane | (55, 80) | 250 × 20 | TOGGLE BUTTON | "Automatically Begin Combat" | same | `options.def:11` |
| `Dialog` | pane | (55, 100) | 250 × 20 | TOGGLE BUTTON | "Play Audio Dialog" | same | `options.def:12` |
| `Face` | pane | (55, 120) | 250 × 20 | TOGGLE BUTTON | "Always Face Enemy In Combat" | same | `options.def:13` |
| `Enhanced` | pane | (55, 140) | 250 × 20 | TOGGLE BUTTON | "Use Enhanced 3D Lighting" | same | `options.def:14` |
| `Limit` | pane | (55, 160) | 250 × 20 | TOGGLE BUTTON | "Limit Game Speed" | same | `options.def:15` |
| `NoCombatRes` | pane | (55, 180) | 250 × 20 | TOGGLE BUTTON | "No Combat Results" | same | `options.def:16` |

Cell stride: **20 px** between toggles (y: 60 / 80 / 100 / 120 / 140 / 160 / 180).

`BTNFLAG_TOGGLE | BTNFLAG_DRAWTOGGLE = 0x00040000 | 0x00400000 = 0x00440000` (`widgets.def:50,54`). Visual style = `STYLE BUTTON TOGGLE` from `widgets.def:95-99`:
- **UP bitmap** = `CheckU` (16×17), **DOWN bitmap** = `CheckD` (16×17).
- **Label font** = `Med`, **color** = `(255,255,255)` white, **flags** = `TEXT_LEFT | TEXT_SHADOW`.
- **UPLABELRECT** = `(18, -1, 0, 0)` — label rect anchored at `(18, -1)` relative to the button's own POS (so the text starts 18 px right of the checkbox, 1 px above the button TL).
- **DOWNLABELRECT** same.

### §4.2 — Slider labels (4 right-aligned TEXT, right column)

| element | space | (x, y) | (w, h) | text | flags | cite |
|---------|-------|--------|--------|------|-------|------|
| (anon TEXT) | pane | (260, 70) | 150 × 20 | "Violence Level" | `TEXT_SHADOW \| TEXTFLAG_RIGHT` | `options.def:19` |
| (anon TEXT) | pane | (260, 103) | 150 × 20 | "Music Volume" | same | `options.def:20` |
| (anon TEXT) | pane | (260, 136) | 150 × 20 | "SFX Volume" | same | `options.def:21` |
| (anon TEXT) | pane | (260, 169) | 150 × 20 | "Gamma Level" | same | `options.def:22` |

Cell stride: **33 px** (y: 70 / 103 / 136 / 169). Right-anchor: the right edge of each TEXT cell is at `260 + 150 = 410`, which is exactly the left edge of the slider next to it.

`TEXT_SHADOW | TEXTFLAG_RIGHT = 0x0400 | 0x00080000 = 0x00080400`. From the shared `STYLE TEXT` block (`widgets.def:84-86`): font `Med`, color `(255,255,255)`, default flags `TEXT_LEFT | TEXT_VCENTER | TEXT_SHADOW`. The DEF flags here OVERRIDE the h-align bit (replace TEXT_LEFT with TEXTFLAG_RIGHT) and keep TEXT_SHADOW + the inherited TEXT_VCENTER.

### §4.3 — Sliders (4 horizontal SCROLLBAR)

| element (NAME) | space | (x, y) | (w, h) | type | flags | cite |
|------|-------|--------|--------|------|-------|------|
| `Violence` | pane | (420, 70) | 160 × 20 | HSCROLLBAR | `SCROLLFLAG_HSCROLL` | `options.def:24` |
| `Music` | pane | (420, 103) | 160 × 20 | HSCROLLBAR | same | `options.def:25` |
| `Sound` | pane | (420, 136) | 160 × 20 | HSCROLLBAR | same | `options.def:26` |
| `Gamma` | pane | (420, 169) | 160 × 20 | HSCROLLBAR | same | `options.def:27` |

`SCROLLFLAG_HSCROLL = 0x00020000` (`widgets.def:57`). Visual style = `STYLE SCROLLBAR HSCROLL` (`widgets.def:113-117`):
- **BGBITMAP** = `HScrollBar` (74×21), FRAME 22 6 22 6 (9-slice corners), MARGINS 0 0 4 4.
- **UP arrow** = `HScrollUp` (12×15) at (1,1).
- **DOWN arrow** = `HScrollDown` at (RELR 12, 1) — 12 px right of right edge.
- **THUMB** = `HScrollThumb` 19 wide, 30 max, step 2.

> **Slider value-to-pixel mapping:** the SCROLLBAR widget itself owns the thumb position computation (the FIELD value drives the thumb x within the bar interior). The per-slider value range is **NOT in options.def** — it comes from each FIELD's getter; see §6.2 / §6.3 + §14 UNCONFIRMED-F. Inferred ranges (from snapshot defaults): Violence `0..5` (`src/revmain.cpp:165`), Music/SFX `0..127` (`DAT_005d7a9c/DAT_005d7aa0 = 0x7f` default), Gamma `0..?` (default `3`).

### §4.4 — Controller column headers (3 left-aligned TEXT)

| element | space | (x, y) | (w, h) | text | flags | cite |
|---------|-------|--------|--------|------|-------|------|
| (anon TEXT) | pane | (70, 200) | 100 × 20 | "Control Name" | (default = TEXT_LEFT) | `options.def:32` |
| (anon TEXT) | pane | (300, 200) | 60 × 20 | "Key 1" | (default) | `options.def:33` |
| (anon TEXT) | pane | (455, 200) | 60 × 20 | "Key 2" | (default) | `options.def:34` |

No explicit FLAGS → uses STYLE TEXT defaults: font `Med`, color `(255,255,255)`, `TEXT_LEFT | TEXT_VCENTER | TEXT_SHADOW` (`widgets.def:86`).

(A commented-out `TEXT POS 480 200 60 20 TEXT "Key 3"` at `options.def:35` proves the original design had 3 keys per controller — matching `meth_0x439060`'s **0x13=19 dword** record which holds 3 keys; the binary still reads 3 keys per record but the UI only displays 2.)

### §4.5 — Controller LISTBOX (1 large)

| element (NAME) | space | (x, y) | (w, h) | type | flags | RECT | ITEM | SELCOLOR | font / color | inner FIELD | cite |
|---|---|---|---|---|---|---|---|---|---|---|---|
| `controller` | pane | (60, 220) | 512 × 170 | LISTBOX | `LISTFLAG_VLIST \| LISTFLAG_USEFORMAT \| CTRLFLAG_CLEARBG` | `185 5 23 5` | `150 20` | `(0, 100, 30)` (dark green) | font `Med`, color `(255,255,255)`, flags `TEXT_CENTER \| TEXT_VCENTER \| TEXT_SHADOW \| TEXT_SINGLELINE`, DRAWMODE `DM_USEDEFAULT` | `FIELD 0 0 150 20 FONT "Med" FLAGS TEXT_SHADOW \| TEXT_CENTER COLOR 255 255 100 "key"` | `options.def:37-41` |

`LISTFLAG_VLIST | LISTFLAG_USEFORMAT | CTRLFLAG_CLEARBG = 0x00010000 | 0x00080000 | 0x00001000 = 0x00091000`.

Visual style overrides the default `STYLE LISTBOX VLIST` (`widgets.def:119-125`): the RECT margins `185 5 23 5` (left=185, top=5, right=23, bottom=5) carve out a content area `512−185−23 = 304` px wide × `170−5−5 = 160` px tall for the rendered rows. The ITEM `150 20` says each ROW = 150 wide × 20 tall — but USEFORMAT means the per-row layout is the inner `FIELD ... BEGIN/END` block, where the FIELD `"key"` (yellow-cream `(255,255,100)`, font `Med`, centered+shadowed) is drawn for each visible column. The internal scrollbar is auto-spawned (the LISTBOX ctor at `0x430090` calls `0x42de00` to spawn a nested SCROLLBAR; see `DefWidgetEngine.md:55-56`).

> **Population:** the LISTBOX is bound to the **global controller key-map** (the same data `cls_0x439000::meth_0x439060` reads). The 69 entries (`DAT_005d697c = 0x45`) become 69 rows. Each row formats: `<displayName> | <key1> | <key2>` — but the FIELD `"key"` is a single cell and the row format glues all three columns by the column-header layout (Control Name at x=70-170, Key 1 at x=300-360, Key 2 at x=455-515 in pane coords). UNCONFIRMED-D: whether the LISTBOX renders its own multi-column row internally (with column x's matching the headers) or whether the row text is a single string with embedded tabs is not pinned. The DEF only declares ONE inner FIELD ("key").

### §4.6 — Decorative name labels (8 left-aligned TEXT, overlapping the LISTBOX top)

These 8 TEXTs are placed **on top of** the LISTBOX area (y=220+rows of 20 each starting at 224). They appear to be a **redundant static label set** matching the canonical Up/Down/Left/Right/Punch/Kick/Thrust/Use names — likely a hold-over from an earlier UI iteration where the screen showed a fixed set of bindings, NOT the full LISTBOX. See UNCONFIRMED-D.

| element (NAME) | space | (x, y) | (w, h) | text | flags | cite |
|---|---|---|---|---|---|---|
| `name0` | pane | (70, 224) | 180 × 20 | "Up" | `CTRLFLAG_CLEARBG FONT "Med" FLAGS TEXT_LEFT \| TEXT_VCENTER` | `options.def:43` |
| `name1` | pane | (70, 244) | 180 × 20 | "Down" | same | `options.def:44` |
| `name2` | pane | (70, 264) | 180 × 20 | "Left" | same | `options.def:45` |
| `name3` | pane | (70, 284) | 180 × 20 | "Right" | same | `options.def:46` |
| `name4` | pane | (70, 304) | 180 × 20 | "Punch" | same | `options.def:47` |
| `name5` | pane | (70, 324) | 180 × 20 | "Kick" | same | `options.def:48` |
| `name6` | pane | (70, 344) | 180 × 20 | "Thrust" | same | `options.def:49` |
| `name7` | pane | (70, 364) | 180 × 20 | "Use" | same | `options.def:50` |

> **NOTE:** the DEF has a quirky **double-`FLAGS` clause** on these:
> ```
> TEXT POS 70 224 180 20 NAME "name0" TEXT "Up" FLAGS CTRLFLAG_CLEARBG FONT "Med" FLAGS TEXT_LEFT | TEXT_VCENTER
> ```
> Two `FLAGS` keywords on one line. The DEF parser is left-to-right; either the second `FLAGS` overrides the first, or the parser merges them. Per `recon/discovered/port_status/DefWidgetEngine.md:106-108` the tokenizer is not fully ported; behavior is UNCONFIRMED-G. Conservative interpretation for the reconstructor: **set BOTH** = `CTRLFLAG_CLEARBG | TEXT_LEFT | TEXT_VCENTER = 0x00001000 | 0x0001 | 0x0040 = 0x00001041`.

### §4.7 — OK / Cancel buttons

| element (NAME) | space | (x, y) | (w, h) | type | text | cite |
|---|---|---|---|---|---|---|
| `ok` | pane | (180, 410) | 100 × 30 | BUTTON (default NORMAL style) | "OK" | `options.def:55` |
| `cancel` | pane | (360, 410) | 100 × 30 | BUTTON | "Cancel" | `options.def:56` |

Default `STYLE BUTTON NORMAL` (`widgets.def:88-93`): UP = `ClearFrame2` (9-slice, 64×57), DOWN = `FillFrame2` (9-slice), FRAME 10 10 12 12, MARGINS 0 0 4 4, UP label font `Med` white, DOWN label font `Med` black, both `TEXT_CENTER | TEXT_VCENTER | TEXT_SINGLELINE`. Label rects = `(0,0,0,0)` (= full button rect minus margins).

---

## §5 — Draw order / composition

The DEF widget engine drives the entire paint; widgets render in DEF source order:

1. **DefScreen background** — `optionsnotex.dat / Background` loaded via `LoadAndShow:31` (`vtable+0xa0("background", -1)`) and blitted opaque at pane `(0,0)`.
2. **7 toggle BUTTONs** (`Realtime` → `NoCombatRes`) — each draws its CheckU/D bitmap + label per the TOGGLE style. `options.def:10-16`.
3. **4 right-aligned slider-label TEXTs** — `options.def:19-22`. Render via `FUN_004be2b0` (UI_METHOD_MAP §5).
4. **4 HSCROLLBARs** (sliders) — each draws bar BG + thumb + L/R arrows per the HSCROLL style. `options.def:24-27`.
5. **3 column-header TEXTs** ("Control Name" / "Key 1" / "Key 2") — `options.def:32-34`.
6. **1 LISTBOX `controller`** + auto-spawned VSCROLLBAR — draws BGBITMAP frame, then iterates rows, drawing the FIELD "key" per row. `options.def:37-41`.
7. **8 decorative `name0..name7` TEXTs** — `options.def:43-50`. Drawn AFTER the LISTBOX in registration order, so they appear ABOVE the LISTBOX rows visually (overpainting).
8. **2 OK / Cancel BUTTONs** — `options.def:55-56`.

Per-widget draw goes through the engine's `DefWidget_RenderWalker` (renderer not yet pinned — `DefWidgetEngine.md:79-80`). Each widget calls its own DrawBG into the parent pane bitmap; text/labels funnel through `FUN_004be2b0` (UI_METHOD_MAP §5) with font index + color + flags from the DEF/STYLE.

---

## §6 — Algorithms (pseudocode per helper)

### §6.1 — `virt_meth_0x53a8b0` (Initialize, vtable slot 0)

```text
TOptionsScreen::Initialize():
    // 1) Open the DEF screen — same shape as CharCreate / SaveGame / etc.
    //    No bg_dat arg → 11 args; chrome resolves via "background" token at LoadAndShow:31.
    int flags = (this->mbr_0x17c != 0) ? 0x11 : 0;   // mbr_0x17c is set externally before Init
                                                     //   (e.g. _DAT_0066fe3c = 1 in case 0x52)
    DefScreen_Open(this,
                   panel        = "options",
                   screen_root  = "default",          // PANEL "default" in options.def
                   flags        = flags,
                   x = 0, y = 0,
                   w = 640, h = 480,
                   hi_w = 450, hi_h = 160,
                   widgets_lib  = "widgets"           // -> widgets.def
                  )
                  // [bg_dat = NULL implicitly; chrome via "background" token]

    // 2) Clone the live setting globals into the screen's editable fields.
    //    The widget engine's FIELD bindings will read from THESE, not from the live globals,
    //    so Cancel can simply discard without write-back.
    //
    //    Field offsets are int-index (×4 for byte offset). The retail decomp shows raw addresses;
    //    the labels below are inferred from snapshot src/ semantics + XREF context.
    this->mbr_0x180 = DAT_005d7a18      // "Realtime"      — DrawRealtimeShadows (T3DScene reads it)
    this->mbr_0x184 = DAT_005d7a68      // "Auto"          — AutoBeginCombat-family flag (TCharacter HasActionAni/ResolveAttack guards)
    this->mbr_0x188 = DAT_005d7a60      // "Dialog"        — ShowDialog / PlaySpeech-family flag
    this->mbr_0x18c = DAT_005d7a64      // "Face"          — AlwaysFaceEnemy flag (TCharacter HasActionAni guards)
    this->mbr_0x190 = DAT_005e91c0      // "Enhanced"      — Enhanced3DLighting (T3DScene reads/writes it)
    this->mbr_0x194 = (DAT_006682a8 == 0)   // "Limit"      — Limit Game Speed (inverted: stored as "is limited")
    this->mbr_0x198 = DAT_00668194      // "NoCombatRes"   — No Combat Results toggle
    this->mbr_0x19c = DAT_005d79e8      // "Violence"      — ViolenceLevel slider (default = 5)
    this->mbr_0x1a0 = DAT_005d7a9c      // "Music"         — Music Volume slider (default = 0x7f = 127)
    this->mbr_0x1a4 = DAT_005d7aa0      // "Sound"         — SFX Sound Volume slider (default = 0x7f = 127)
    this->mbr_0x1a8 = DAT_005d7a48      // "Gamma"         — Gamma Level slider (default = 3)

    // 3) Init the in-flight rebind state (the "user is currently pressing keys to rebind X" buffer).
    //    These 4 are the "current rebind buffer": when the user picks a controller row
    //    and starts pressing keys, meth_0x53ba80 fills 1b4/1b8/1bc with up-to-3 keys,
    //    then commits them into mbr_0x1ac[selected].
    this->mbr_0x1b0 = 0                  // "rebinding-in-progress" flag (0 = idle)
    this->mbr_0x1b4 = 0xffffffff         // pending key slot 0  (sentinel "unset")
    this->mbr_0x1b8 = 0xffffffff         // pending key slot 1
    this->mbr_0x1bc = 0xffffffff         // pending key slot 2
    this->mbr_0x1c0 = 0xffffffff         // selected controller index (sentinel "none")

    // 4) Free any prior rebind buffer + allocate a fresh per-controller edit buffer.
    if (this->mbr_0x1ac != 0):
        free(this->mbr_0x1ac)
    this->mbr_0x1ac = malloc(DAT_005d697c * 0x24)   // 69 controllers * 36 bytes/controller
                                                    // Layout per controller (36 B = 9 dwords):
                                                    //   [+0x00..0x0b]  variant 0 — keys 1/2/3   (3 dwords)
                                                    //   [+0x0c..0x17]  variant 1 — keys 1/2/3   (3 dwords)
                                                    //   [+0x18..0x23]  variant 2 — keys 1/2/3   (3 dwords)
                                                    // (The 19-dword input record from meth_0x439060 has
                                                    //  more fields; the screen only snapshots 9 of them.)

    // 5) Snapshot the live key-map into the edit buffer.
    int dst = 0   // byte offset into mbr_0x1ac
    for ctlIdx in [0 .. DAT_005d697c):              // 69 controllers
        for variant in 0..2:                        // 3 outer loops (3 variant slots? primary/secondary/joy?)
            uint32_t tmp[16]
            for sub in 0..2:                        // 3 inner loops; copies 1 dword per call out of meth_0x439060's 0x13-dword output
                meth_0x439060(&UNK_0065a904.field_0xa4, ctlIdx)   // fills tmp[0..18] for this controller index
                *(uint32_t*)(this->mbr_0x1ac + dst) = tmp[?]      // selects ONE dword from the 19; UNCONFIRMED-H which index per (variant,sub)
                dst += 4

    // 6) Parse the loaded options.def buffer into widget tree (engine call).
    DefWidget_ParseBuffer(this)                     // = cls_0x5b98b8::meth_0x437620
    return 1
```
Citations: `recon/classes/cls_0x5b9744.cpp:46-119`; for Step 5's inner-loop structure see UNCONFIRMED-H.

### §6.2 — `virt_meth_0x53aa60` (Close, vtable slot 1 — the "Cancel-or-OK already applied" cleanup)

```text
TOptionsScreen::Close():
    if (this->mbr_0x1ac != 0):
        free(this->mbr_0x1ac)
    this->mbr_0x1ac = 0
    cls_0x5b98b8::virt_meth_0x434f30(this)          // TButtonPane Close (base teardown)
```
Citations: `recon/classes/cls_0x5b9744.cpp:125-134`.

> **NOTE:** Close does **NOT** write the snapshot back. That's the role of `virt_meth_0x53aa90` (Apply); see §6.3 — Apply runs FIRST (on OK click) then Close. On Cancel, Close runs and the live globals are never touched (the edits live and die on the screen instance).

### §6.3 — `virt_meth_0x53aa90` (OnControl dispatcher / Apply, vtable slot 148) — BODY EXTRACTED

**Extracted 2026-05-29** via `DecompileAddr.java 0x0053aa90` → `/tmp/options_decomp/apply_53aa90.cpp` (2875 B = 0xb3b). Signature: `int OnControl(this, int sender_widget, int msg_id)`. The single function multiplexes **7 different message classes** (`msg_id` = `1`, `3000`, `0xbb9`, `0xbba`, `4000`, `0x1389`, `0x138a`) via if/else chain. The full per-branch decode is in §6.7 (OnControl dispatcher). This section restricts to the **OK-button → Apply settings** path.

Apply runs **only** on `msg_id == 3000 && strcmp(sender->name, "ok") == 0` (`:347` `FUN_0059a530(&DAT_005e4524, param_2)` where `DAT_005e4524 = "ok\0\0"`). Verbatim retail order from `:347-397`:

```text
TOptionsScreen::OnControl(sender, 3000):
    if (strcmp(sender->name /*+0x18*/, "ok") == 0):
        // === APPLY SETTINGS — exact retail order ===
        DAT_005d7a18 = this->mbr_0x180           // Realtime         (DrawRealtimeShadows)
        DAT_005d7a60 = this->mbr_0x188           // Dialog           (ShowDialog/PlaySpeech-family)
        DAT_005d7a68 = this->mbr_0x184           // Auto             (AutoBeginCombat-family)
        DAT_005e91c0 = this->mbr_0x190           // Enhanced         (this is param_1[100] = mbr_0x190, NOT a separate field — see note)
        DAT_00668194 = this->mbr_0x198           // NoCombatRes
        DAT_005d79e8 = this->mbr_0x19c           // Violence
        DAT_005d7a9c = this->mbr_0x1a0           // Music
        DAT_006682a8 = (this->mbr_0x194 == 0)    // Limit            (re-inverted: stored as "NoLimit")
        FUN_0049a5c0(DAT_005d7a9c)               // audio::SetMusicVolume(music)  -- inferred
        DAT_005d7a48 = this->mbr_0x1a8           // Gamma
        DAT_005d7aa0 = this->mbr_0x1a4           // Sound (SFX)
        FUN_004a98f0(DAT_005d7a48)               // SetGammaTable(gamma)  -- inferred
        FUN_00453640(DAT_006671a4, 1)            // notify world-light system  (see agent_world.txt:297 — daylight changed)
        DAT_005d7a64 = this->mbr_0x18c           // Face             (AlwaysFaceEnemy)
        FUN_00484ed0()                           // re-broadcast settings to subsystems (cf. _data.txt:87044/87143 etc. — this fn READS every setting global)

        // === REBIND BUFFER WRITE-BACK ===
        // Walk the edit buffer (param_1[0x6b] = mbr_0x1ac), variant-bank stride matches Init.
        // 3-outer × 3-inner loop (9 dwords per controller). EACH inner iteration:
        //   1) FUN_00439060(ctlIdx, local_record)         // GET fresh live 3-dword record (re-read each iter — quirky)
        //   2) local_record[0] = *(mbr_0x1ac + bufOff)    // PATCH slot 0 only with one dword from edit buffer
        //   3) FUN_00439110(ctlIdx, local_record)         // PUT patched record back
        //   4) bufOff += 4
        // Note: `*puVar7 = *(undefined4 *)(iVar6 + param_1[0x6b])` writes to local_record[0] via puVar7=&local_record[0]
        // (puVar7 advances by +4 within the inner loop, so successive iters DO write to local_record[1] and [2]) —
        // BUT each FUN_00439060 re-zeros the buffer, so only the LAST write per inner cycle of 3 actually persists.
        // This is a retail oddity: the loop structure is "9 RMW cycles per controller" but functionally only 3
        // outer iterations write distinct values (one local_record slot 0 update per outer iter).
        // The Get→Patch→Put inverse of meth_0x439060 mirrors Init's nested 3x3 (§6.1 step 5).
        ctlIdx = 0
        bufOff = 0
        while ctlIdx < DAT_005d697c:                                   // 69 controllers
            outer = 3
            puStack_cc = auStack_a4                                    // local 16-dword scratch (used like the Init buf)
            while outer-- > 0:
                inner = 3
                puVar7 = puStack_cc
                while inner-- > 0:
                    FUN_00439060(ctlIdx, local_record /*aiStack_b0*/)  // GET
                    *puVar7 = *(uint32_t *)(this->mbr_0x1ac + bufOff)  // patch
                    FUN_00439110(ctlIdx, local_record)                 // PUT
                    bufOff += 4
                    puVar7 += 4
                puStack_cc += 4
            ctlIdx += 1
        FUN_00439dc0("Controls")                                       // write "[Controls]" INI section to disk

        // === DISMISS THE MODAL ===
        if (this->mbr_0x17c == 0):                                     // mbr_0x5f
            _DAT_0066fea0 = &DAT_0065d358                              // global "current screen" → MainMenu/LogoScreen
            FUN_0048ea40()                                             // pop modal pump
        else:
            (**(this->vtable + 8))(this)                               // slot 2 = Close (TButtonPane::virt_meth_0x435010)
        this->mbr_0x17c = 0
        FUN_00435010()                                                 // engine-level "deactivate" (also slot 2 inherited)
        return 1
    // …else falls through to "cancel" / "ControlSetup" branch — see §6.7.
```

Citations (line numbers in `/tmp/options_decomp/apply_53aa90.cpp`):
- Apply globals: `:348-362` (write order ABOVE is verbatim).
- Rebind write-back loop: `:363-386` (nested 3×3 over 69 controllers).
- INI persist: `:387` (`FUN_00439dc0("Controls")`).
- Modal dismiss: `:388-396` (branch on `mbr_0x17c`).

**Notes on the write order vs Init read order:**
- Init reads: 180/188/184/190/x/19c/198/1a8 then 194 (inverted) then 1a0/x/1a4/18c.
- Apply writes: 180/188/184/190/198/19c/1a0/194 (inverted)/1a8/1a4/18c.
- The Init and Apply orders DIFFER but **the field↔global mapping is identical** (Init `mbr_0x180 = DAT_005d7a18` ↔ Apply `DAT_005d7a18 = mbr_0x180`, etc.). The compiler's register allocator dictated each order; semantically they are inverses.

**Side-effect helpers (cited from apply body):**
- `FUN_0049a5c0(music)` @ `:356` — called only on Apply, with the new music volume; inferred `audio::SetMusicVolume`. **UNCONFIRMED-P.**
- `FUN_004a98f0(gamma)` @ `:359` — called with the new gamma; inferred `SetGammaTable` / `BuildGammaRamp`. **UNCONFIRMED-Q.**
- `FUN_00453640(DAT_006671a4, 1)` @ `:360` — called with `&world_light_state, 1`; per `agent_world.txt:297` this is a daylight/ambient relight trigger. Apply pokes it because Enhanced3DLighting toggled. **UNCONFIRMED-R.**
- `FUN_00484ed0()` @ `:362` — no args; READ-XREFs in `_data.txt` show it touches DAT_005d7a60/64/68/9c/aa0/a18/a48 etc — likely `Settings_Broadcast_All()` / `Settings_PersistToINI()` (the "[Game]"/"[Audio]" INI write).
- `FUN_00439dc0("Controls")` @ `:387` — writes the rebuilt controller table to INI section `[Controls]` (sibling of `FUN_00484ed0`).
- `FUN_0048ea40()` @ `:390/:425` — pop the modal-screen pump (matches the Pause/InGameMenu close pattern).
- `FUN_00435010()` @ `:396/:428` — TButtonPane base "Deactivate" (slot 2 inherited).

### §6.4 — `virt_meth_0x53b9e0` (DispatchInput override, vtable slot 108)

```text
TOptionsScreen::DispatchInput(int vkey):
    focusedWidget = DefWidget_GetFocused(this)        // = meth_0x436980 — returns the controller LISTBOX when it has a selected row
    if (focusedWidget->selectedRow /* +0xa0 */ >= 0):
        cls_0x5b98b8::meth_0x53ba80(this, vkey)       // accumulate vkey into pending-rebind buffer (mbr_0x1b4/1b8/1bc)
        return
    cls_0x5b93c4::virt_meth_0x4361f0(this, vkey)      // default DefWidget_DispatchInput (arrows, Enter, Esc, hot-reload R)
```
Citations: `recon/classes/cls_0x5b9744.cpp:140-152`; `recon/discovered/renames/agent_ui_wave2_defengine.txt:211`.

### §6.5 — `cls_0x5b98b8::meth_0x53ba80` (key-rebind accumulator)

Called by §6.4 when the user types into a selected LISTBOX row.

Body (`recon/classes/cls_0x5b98b8_likely_MultiplayerSystem.cpp:2072-2213`) does roughly:

```text
RebindAccumulate(this, vkey):
    listbox = DefWidget_GetFocused(this)             // again
    sel = listbox->selectedRow /* +0xa0 */

    if (key-up event /* in_stack_00000008 == 0 */):
        // key release — commit the pending rebind if 1+ keys gathered
        if (this->mbr_0x1b4 == vkey):                // released the first key of the chord
            this->mbr_0x1b0 = 0
            cls_0x5a3d44::meth_0x430b80(listbox)     // listbox redraw
        // ...
    else:
        // key press
        if (this->mbr_0x1c0 == 0xffffffff): return    // no controller selected -- ignore

        if (this->mbr_0x1b0 == 0):
            // first key of a new rebind
            this->mbr_0x1b4 = vkey
            this->mbr_0x1b0 = 1                       // mark "rebinding in progress"
            this->mbr_0x1b8 = 0xffffffff
            this->mbr_0x1bc = 0xffffffff
            redraw()
            return
        // 2nd or 3rd key — fill the next free slot in (mbr_0x1b4, 1b8, 1bc)
        for slot in 0..2:
            if (slot >= 3):
                // 3 keys already pressed — commit
                this->mbr_0x1b0 = 0
                this->mbr_0x1bc = vkey
                committed = true
                break
            if (vkey == this->mbr_0x1b4 + slot*4):    // already in chord — ignore
                return
            if (this->mbr_0x1b4[slot] == 0xffffffff):
                this->mbr_0x1b4[slot] = vkey
                break

        redraw()
        if (committed):
            // dedup: remove this chord from any other controller that also had it
            //        (so each chord is bound to exactly one controller).
            for ctlIdx in [0 .. DAT_005d697c):
                for variant in 0..2:
                    for slot in 0..2:
                        if (ctlIdx_record[variant].keys_match(this->mbr_0x1b4..1bc)):
                            zero_out(ctlIdx_record[variant])     // un-bind on the other controller

    if (this->mbr_0x1b0 == 0):
        // commit phase — write chord into the edit buffer's slot for the SELECTED controller
        sel_record = this->mbr_0x1ac + (selectedVariantBank + sel * 3 + ...)   // see code at :2174-2199
        sel_record[0..2] = this->mbr_0x1b4..1bc                                 // 0xffffffff slots stored as 0
        this->mbr_0x1b4 = this->mbr_0x1b8 = this->mbr_0x1bc = this->mbr_0x1c0 = 0xffffffff
    else:
        // still gathering — set a "dirty redraw" flag on the listbox
        listbox->state |= 0x20                       // request redraw
    redraw()
```
Citations: `recon/classes/cls_0x5b98b8_likely_MultiplayerSystem.cpp:2072-2213`. Variant-bank arithmetic at `:2187,2190` (`(uVar9 + ((int)uVar2 / 2) * 3) * 3`).

### §6.6 — Field offset map

| `param_1[N]` (Init) | byte off = `4N` | OOAnalyzer name | semantic (from XREF + snapshot) |
|--|--|--|--|
| `[0x5f]` = 0x17c | 0x17c | `mbr_0x17c` | "popup/inline" mode flag — gates Init's `flags = 0x11` (set externally before Init, e.g. `_DAT_0066fe3c = 1` in case 0x52) |
| `[0x60]` = 0x180 | 0x180 | `mbr_0x180` | Realtime (DrawRealtimeShadows) snapshot |
| `[0x61]` = 0x184 | 0x184 | `mbr_0x184` | Auto (AutoBeginCombat) snapshot |
| `[0x62]` = 0x188 | 0x188 | `mbr_0x188` | Dialog (ShowDialog/PlaySpeech) snapshot |
| `[0x63]` = 0x18c | 0x18c | `mbr_0x18c` | Face (AlwaysFaceEnemy) snapshot |
| `[0x64]` = 0x190 | 0x190 | `mbr_0x190` | Enhanced (Enhanced3DLighting) snapshot |
| `[0x65]` = 0x194 | 0x194 | `mbr_0x194` | Limit (LimitGameSpeed) snapshot (inverted on read/write) |
| `[0x66]` = 0x198 | 0x198 | `mbr_0x198` | NoCombatRes snapshot |
| `[0x67]` = 0x19c | 0x19c | `mbr_0x19c` | Violence (ViolenceLevel) slider snapshot |
| `[0x68]` = 0x1a0 | 0x1a0 | `mbr_0x1a0` | Music Volume slider snapshot |
| `[0x69]` = 0x1a4 | 0x1a4 | `mbr_0x1a4` | SFX/Sound Volume slider snapshot |
| `[0x6a]` = 0x1a8 | 0x1a8 | `mbr_0x1a8` | Gamma Level slider snapshot |
| `[0x6b]` = 0x1ac | 0x1ac | `mbr_0x1ac` | malloc'd rebind edit buffer (69 × 36 = 2484 B) |
| `[0x6c]` = 0x1b0 | 0x1b0 | `mbr_0x1b0` | rebinding-in-progress flag (0/1) |
| `[0x6d]` = 0x1b4 | 0x1b4 | `mbr_0x1b4` | pending rebind key 1 (or 0xffffffff) |
| `[0x6e]` = 0x1b8 | 0x1b8 | `mbr_0x1b8` | pending rebind key 2 |
| `[0x6f]` = 0x1bc | 0x1bc | `mbr_0x1bc` | pending rebind key 3 |
| `[0x70]` = 0x1c0 | 0x1c0 | `mbr_0x1c0` | selected controller index (or 0xffffffff) |

### §6.7 — Full OnControl dispatcher (`virt_meth_0x53aa90` body decode)

Signature: `int OnControl(int *this /*=param_1*/, int sender_widget /*=param_2*/, int msg_id /*=param_3*/)`. Returns 1 (always). Source: `/tmp/options_decomp/apply_53aa90.cpp`.

Dispatch structure (top-level if-chain):

```c
OnControl(this, sender, msg_id):
    FUN_00437a80(sender, msg_id);                     // base "remember last-touched panel name" — :30
                                                       // If msg_id==3000 AND sender->classname=="Panel",
                                                       // copies sender->name into this+0x158 (truncated 0x1f)
                                                       // and sets this+0x150=1. (h_437a80.cpp body confirmed.)

    if (msg_id == 1):       goto INIT_REFRESH;        // :31  — "initialize widget visual state from snapshot"
    if (msg_id == 3000):    goto OK_OR_CANCEL_CLICK;  // :345 — button click; sender is OK/Cancel/ControlSetup
    if (msg_id == 0xbb9):   goto TOGGLE_DOWN;         // :197 — button transitioned to DOWN/CHECKED state
    if (msg_id == 0xbba):   goto TOGGLE_UP;           // :237 — button transitioned to UP/UNCHECKED state
    if (msg_id == 4000):    goto SLIDER_CHANGE;       // :280 — scrollbar value changed
    if (msg_id == 0x1389):  goto LISTBOX_RENDER_ROW;  // :247 — LISTBOX "draw this row" (custom-render hook)
    if (msg_id == 0x138a):  goto LISTBOX_SELECT;      // :239 — LISTBOX row selected
    return 1;
```

#### §6.7.1 — `msg_id == 1` (Init-refresh: paint widgets from snapshot fields)

This branch runs ONCE after DEF parse to push each `mbr_0x180..0x1a8` into its bound widget's UI state. It is implicitly called by the parser at the end of `DefWidget_ParseBuffer` (cf. how the LISTBOX is queried + flagged dirty at :36).

Verbatim (`:31-194`):

```c
sender = FUN_00436900(FUN_00436930("controller"));   // ListBox lookup by name
this->mbr_0x1c4 = sender;                            // cache pointer in mbr_0x71
if (sender):
    FUN_00430c50(numControllers * 2);                // resize ListBox row count = 69 * 2 = 138
                                                      //   (LISTBOX shows 2 visible columns per controller?)
                                                      //   UNCONFIRMED-D revisited: the *2 hint suggests the
                                                      //   listbox has 2*N items, one per (controller, keySlot) pair.

// === 7 TOGGLE buttons: set DOWN bit (0x10000) iff snapshot field nonzero ===
for each toggle in {"RealTime", "Auto" (DAT_005e44b4), "Face" (DAT_005e44bc),
                    "Dialog", "Enhanced", "Limit", "NoCombatRes"}:
    w = lookup(toggle_name)
    if (w):
        // The "Enhanced" widget has extra logic (:88-95):
        //   if ((DAT_006697bc == 0 && DAT_006697c0 == 0) || DAT_006680f4 != 0):
        //       w->flags |= 4         // disable / grey-out (UNCONFIRMED — see §14)
        //   else:
        //       w->flags &= ~4
        if (snapshot_field == 0):  w->setFlags(w->flags & ~0x10000)    // unchecked
        else:                      w->setFlags(w->flags |  0x10000)    // checked
        w->setFlags(w->flags | 0x20)                                   // mark dirty (CTRLFLAG_DIRTY)

// === 4 SLIDERS: clamp range + set value ===
w = lookup("Violence");  if (w): FUN_0042e3d0(w, 0, 4);   w->mbr_0xa0 = 1;   FUN_0042e440(w, snap_violence)
w = lookup("Music");     if (w): FUN_0042e3d0(w, 0, 0x60); w->mbr_0xa0 = 0xc; FUN_0042e440(w, snap_music)
w = lookup("Sound");     if (w): FUN_0042e3d0(w, 0, 0x7f); w->mbr_0xa0 = 0xf; FUN_0042e440(w, snap_sound)
w = lookup("Gamma");     if (w): FUN_0042e3d0(w, 0, 4);   w->mbr_0xa0 = 1;   FUN_0042e440(w, snap_gamma)
// FUN_0042e3d0(w, min, max) = TDefScrollbar::SetRange — clamps thumb pos to [min,max]
// FUN_0042e440(w, val)       = TDefScrollbar::SetValue — moves thumb + emits dirty
// w->mbr_0xa0 = step granularity (UNCONFIRMED)
//
// ▶ SLIDER VALUE RANGES (resolves UNCONFIRMED-F):
//     Violence : 0..4   (NOT 0..5 as the snapshot default suggested — retail clamps to 4)
//     Music    : 0..96  (0x60 — NOT 0..127; the field stores 0..127 but the slider only spans 0..96)
//     Sound    : 0..127 (0x7f)
//     Gamma    : 0..4

// === 8 LISTBOX-row default key labels (the "name0"/"name1"/.../"name7" ROW DECORATIONS) ===
// THIS RESOLVES UNCONFIRMED-D!  The "name0..name7" TEXT widgets are NOT static labels —
// they are PER-ROW key-name displays for the FIRST 8 controllers in the global key-map.
// They show the CURRENT default-key-name string fetched via FUN_0046e7f0(buf, 100, key_code).
//
// Layout of name template (uStack_c8 + uStack_c4 + ...):
//   uStack_c8 = DAT_005e451c = "name" (4 bytes)
//   uStack_c4 = DAT_005e4520 = "0\0" template (low byte gets overwritten with iVar6 + '0')
// So the names iterated are "name0", "name1", ..., "name7".
//
iVar2 = sender->mbr_0x9c;      // listbox->selected_controller_offset (variant-bank index / 2)
for (iVar6 = 0; iVar6 < 8; iVar6++):
    FUN_00439060(iVar6 + iVar2 / 2, local_record);     // GetRecord for THIS controller index
    name_template[4] = '0' + iVar6;                    // patch to "nameN"
    w = lookup(name_template);                         // find TEXT widget "nameN"
    if (w && local_record[0] != 0):                    // valid key found
        char keyName[100] = {0}
        keyName[0] = DAT_0066fef8;                     // some prefix byte (always 0? — UNCONFIRMED)
        FUN_0046e7f0(keyName, 100, local_record[0])    // VK-code → human key name string (e.g. "Up Arrow")
        w->SetText(keyName)                            // vtable+0x18 = TText::SetText

return 1;
```

**Resolves UNCONFIRMED-D.** The `name0..name7` TEXT widgets are **not redundant static decoration** — they are paint targets that the OnControl handler fills with key-name strings for the first 8 entries in the controller table. So they are essentially a "8-row mini key-binding display" overlaid on the LISTBOX area. The LISTBOX itself (`controller`) draws the remaining 61 rows AND ALSO redraws those first 8 (overpainted by the static TEXTs). This is wasteful but matches the visible UI.

#### §6.7.2 — `msg_id == 3000` (button click — OK / Cancel / ControlSetup)

```c
sender_name_at = sender + 0x18     // TWidget name field

if (strcmp(sender_name_at, "ok") == 0):
    // ===== APPLY (§6.3 above) =====

if (strcmp(sender_name_at, "cancel") == 0):
    FUN_004a98f0(DAT_005d7a48)     // RESTORE original gamma (because slider preview already
                                    //   altered the live gamma global during dragging!)
    if (this->mbr_0x17c != 0):     // inline-mode (mbr_0x5f != 0)
        (**(this->vtable+8))(this) // call slot 2 = Close
    else:
        _DAT_0066fea0 = &DAT_0065d358   // pop to MainMenu/LogoScreen
        FUN_0048ea40()                  // dismiss
    this->mbr_0x17c = 0
    FUN_00435010()
    return 1

if (strcmp(sender_name_at, "ControlSetup") == 0):
    // Open the "controller setup" sub-screen (UNCONFIRMED — see §14)
    if (this->mbr_0x17c != 0):
        (**(this->vtable+8))(this)
        if (FUN_00537110()):       // = FUN_00537110_LoadDef_ingamemenu (per renames file)
                                    // returns nonzero if launched from in-game menu
            FUN_0048ed90(&DAT_0066f748, 0xffffffff)
            uVar1 = FUN_0048ed60(&DAT_0066f748)
            FUN_0048eea0(uVar1, 0xffff)
    else:
        _DAT_0066fea0 = &DAT_0065d358
        FUN_0048ea40()
    this->mbr_0x17c = 0
    FUN_00435010()
    return 1

return 1;     // unhandled button — no-op
```

**Side-effect note on Cancel:** the gamma slider commits to the live `DAT_005d7a48` global IN REAL TIME during dragging (see msg_id=4000 below). Cancel must therefore RESTORE the original value — but the only stored snapshot is `mbr_0x1a8` which **IS** the slider value. So Cancel actually re-applies `mbr_0x1a8` (= original gamma at Init time) via `FUN_004a98f0(DAT_005d7a48)`. Wait — that's reading the LIVE global, not the snapshot. **This is a retail bug**: Cancel "restores" gamma by re-applying the already-mutated live global, which is effectively a no-op. To truly cancel gamma changes, the screen would need to snapshot the original gamma SEPARATELY at Init and re-apply that. The reconstructor should fix this (snapshot original gamma → restore on Cancel). See §11 (retail bugs).

#### §6.7.3 — `msg_id == 0xbb9` (TOGGLE → DOWN, sets snapshot field = 1)

Verbatim (`:198-234`):

```c
sender_name_at = sender + 0x18
if      (strcmp(name, "Realtime") == 0):     this->mbr_0x180 = 1;  return 1   // [0x60]
else if (strcmp(name, "Auto"    ) == 0):     this->mbr_0x184 = 1;  return 1   // [0x61]   (DAT_005e4558)
else if (strcmp(name, "Dialog"  ) == 0):     this->mbr_0x188 = 1;  return 1   // [0x62]
else if (strcmp(name, "Face"    ) != 0):                                       // (!=0 = NOT Face)
    if      (strcmp(name, "Enhanced") == 0): this->mbr_0x190 = 1;  return 1   // [0x64]   (param_1[100])
    else if (strcmp(name, "Limit"   ) != 0):                                   // (!=0 = NOT Limit)
        if (strcmp(name, "NoCombatRes") != 0): return 1                        // unrecognized — no-op
        this->mbr_0x198 = 1;  return 1                                         // [0x66]
    this->mbr_0x194 = 1;  return 1                                             // [0x65]
this->mbr_0x18c = 1;  return 1                                                 // [0x63]   (Face)
```

This proves the FIELD → name → mbr_0xN mapping for the 7 toggles. Note: the parser order in the source is `Realtime / Auto / Dialog / Face / Enhanced / Limit / NoCombatRes` and lines through a nested if/else; the indices `[0x60]/[0x61]/[0x62]/[0x63]/[0x64]/[0x65]/[0x66]` confirm Init's field offsets (§6.6).

#### §6.7.4 — `msg_id == 0xbba` (TOGGLE → UP, sets snapshot field = 0)

Verbatim (`:307-343`): structurally identical to §6.7.3 but writes 0. Compares against `"Realtime"` (DAT_005e4590 — note CAP), `"Auto"` (DAT_005e459c), `"Dialog"` (DAT_005e45a4), `"Face"` (DAT_005e45ac), `"Enhanced"`, `"Limit"`, `"NoCombatRes"`. Each match → `this->mbr_0xNN = 0`.

**Cite-able evidence for the FIELD↔offset table** (verbatim from the body):

| widget NAME (cstr@addr) | msg=0xbb9 sets | msg=0xbba sets | semantic |
|--|--|--|--|
| `"RealTime"` @ 005e44a8 / `"Realtime"` @ 005e454c / @ 005e4590 | `mbr_0x180` = 1 | `mbr_0x180` = 0 | DrawRealtimeShadows |
| `"Auto"` @ 005e44b4 / @ 005e4558 / @ 005e459c | `mbr_0x184` = 1 | `mbr_0x184` = 0 | AutoBeginCombat |
| `"Dialog"` @ 005e44c4 / @ 005e4560 / @ 005e45a4 | `mbr_0x188` = 1 | `mbr_0x188` = 0 | ShowDialog/PlaySpeech |
| `"Face"` @ 005e44bc / @ 005e4568 / @ 005e45ac | `mbr_0x18c` = 1 | `mbr_0x18c` = 0 | AlwaysFaceEnemy |
| `"Enhanced"` @ 005e44cc / @ 005e4570 / @ 005e45b4 | `mbr_0x190` = 1 | `mbr_0x190` = 0 | Enhanced3DLighting |
| `"Limit"` @ 005e44d8 / @ 005e457c / @ 005e45c0 | `mbr_0x194` = 1 | `mbr_0x194` = 0 | LimitGameSpeed (snapshot, INVERTED vs live global) |
| `"NoCombatRes"` @ 005e44e0 / @ 005e4584 / @ 005e45c8 | `mbr_0x198` = 1 | `mbr_0x198` = 0 | NoCombatResults |

(Three separate string literals per name because the compiler de-duplicated only WITHIN a branch — each branch has its own copy.)

#### §6.7.5 — `msg_id == 4000` (slider value changed)

Verbatim (`:280-305`):

```c
sender_name_at = sender + 0x18    // (param_2 + 0x18)
if (strcmp(name, "Violence") == 0):
    this->mbr_0x19c = sender->mbr_0x94    // [0x67] = scrollbar->value (offset +0x94)
    return 1
if (strcmp(name, "Music") == 0):
    this->mbr_0x1a0 = sender->mbr_0x94    // [0x68]
    FUN_0049a5c0(sender->mbr_0x94)        // LIVE audio::SetMusicVolume during drag !
    return 1
if (strcmp(name, "Sound") == 0):
    this->mbr_0x1a4 = sender->mbr_0x94    // [0x69]
    return 1
if (strcmp(name, "Gamma") == 0):
    this->mbr_0x1a8 = sender->mbr_0x94    // [0x6a]
    FUN_004a98f0(sender->mbr_0x94)        // LIVE SetGammaTable during drag !
    return 1
return 1                                   // unknown slider — no-op
```

**Key finding:** Music + Gamma sliders apply LIVE during dragging (call `FUN_0049a5c0`/`FUN_004a98f0` immediately), NOT just on Apply. Violence + Sound only update the snapshot. This is a deliberate UX choice (so the user hears volume / sees gamma changes as they drag) but means Cancel has to roll back — which retail does for gamma (§6.7.2) but NOT for music (UNCONFIRMED-S — likely a retail bug).

#### §6.7.6 — `msg_id == 0x138a` (LISTBOX row clicked)

Verbatim (`:239-245`):

```c
this->mbr_0x1b0 = 1                                   // start "rebinding"
this->mbr_0x1c0 = sender->mbr_0xa0                    // selectedRow = listbox->selectedIndex
return 1
```

The rebind state machine then handles the next keypress via the input override (§6.4 → §6.5).

#### §6.7.7 — `msg_id == 0x1389` (LISTBOX custom row paint)

Verbatim (`:247-278`): the same 8-row name lookup loop as §6.7.1 but using template `"Controller"` and `"name0"..."name7"` from a different copy (DAT_005e4604/4608 = "name"/"0"). Compares `sender + 0x18 == "Controller"` first; if no match return. Then iterates 0..7 to display the per-row key names. Likely fired by the LISTBOX as a "redraw your inner cells" message after a row-rebind.

#### §6.7.8 — Summary of message ids

| msg_id | dec | role |
|--|--|--|
| 1 | 1 | Init-refresh — paint widgets from snapshot |
| 3000 | 3000 | Button click (OK / Cancel / ControlSetup) |
| 0xbb9 | 3001 | Button DOWN/CHECKED (toggle on) |
| 0xbba | 3002 | Button UP/UNCHECKED (toggle off) |
| 4000 | 4000 | Scrollbar value change |
| 0x1389 | 5001 | LISTBOX custom row paint (redraw cells) |
| 0x138a | 5002 | LISTBOX row selected |

The id ranges (1xxx = init, 3xxx = button, 4xxx = scrollbar, 5xxx = listbox) match what one would expect from a widget-class-keyed dispatch (`3000+0 = button-click`, `3000+1 = toggle-down`, `3000+2 = toggle-up`, etc.). **UNCONFIRMED-T:** confirm by reading `DefWidget_DispatchControl @ 0x436ec0` for the per-widget-class message-id base.

### §6.8 — FIELD → getter / setter table (new — derived from §6.3 + §6.7)

For each widget in `options.def`, the table below pins the **getter** (where its initial display value comes from on the Init-refresh msg_id=1 paint, §6.7.1) and the **setter** (where user interaction writes — §6.7.3..§6.7.6) plus the **apply target** (where the OK-button writes — §6.3).

| widget NAME | type | getter (msg=1) | setter (user input) | apply target (OK) |
|---|---|---|---|---|
| `Realtime` | TOGGLE | `mbr_0x180` ≠ 0 → set DOWN flag (`0x10000`) | `0xbb9` → `mbr_0x180 = 1` ; `0xbba` → `mbr_0x180 = 0` | `DAT_005d7a18 = mbr_0x180` |
| `Auto` | TOGGLE | `mbr_0x184` ≠ 0 → set DOWN flag | `0xbb9/0xbba` → `mbr_0x184 = {1,0}` | `DAT_005d7a68 = mbr_0x184` |
| `Dialog` | TOGGLE | `mbr_0x188` ≠ 0 → set DOWN flag | `mbr_0x188 = {1,0}` | `DAT_005d7a60 = mbr_0x188` |
| `Face` | TOGGLE | `mbr_0x18c` ≠ 0 → set DOWN flag | `mbr_0x18c = {1,0}` | `DAT_005d7a64 = mbr_0x18c` |
| `Enhanced` | TOGGLE | `mbr_0x190` ≠ 0 → set DOWN flag; **ALSO** if `(DAT_006697bc==0 && DAT_006697c0==0) \|\| DAT_006680f4!=0` → set bit `0x4` (disable/grey-out) | `mbr_0x190 = {1,0}` | `DAT_005e91c0 = mbr_0x190` |
| `Limit` | TOGGLE | `mbr_0x194` ≠ 0 → set DOWN flag (inverted at Init: `mbr_0x194 = (DAT_006682a8 == 0)`) | `mbr_0x194 = {1,0}` | `DAT_006682a8 = (mbr_0x194 == 0)` (re-inverted) |
| `NoCombatRes` | TOGGLE | `mbr_0x198` ≠ 0 → set DOWN flag | `mbr_0x198 = {1,0}` | `DAT_00668194 = mbr_0x198` |
| `Violence` | HSCROLLBAR | `SetRange(0,4); SetValue(mbr_0x19c)` step=1 | `4000` → `mbr_0x19c = sender.value` | `DAT_005d79e8 = mbr_0x19c` |
| `Music` | HSCROLLBAR | `SetRange(0,96); SetValue(mbr_0x1a0)` step=12 | `4000` → `mbr_0x1a0 = sender.value` AND `FUN_0049a5c0(sender.value)` live | `DAT_005d7a9c = mbr_0x1a0` + `FUN_0049a5c0(DAT_005d7a9c)` |
| `Sound` | HSCROLLBAR | `SetRange(0,127); SetValue(mbr_0x1a4)` step=15 | `4000` → `mbr_0x1a4 = sender.value` | `DAT_005d7aa0 = mbr_0x1a4` |
| `Gamma` | HSCROLLBAR | `SetRange(0,4); SetValue(mbr_0x1a8)` step=1 | `4000` → `mbr_0x1a8 = sender.value` AND `FUN_004a98f0(sender.value)` live | `DAT_005d7a48 = mbr_0x1a8` + `FUN_004a98f0(DAT_005d7a48)` |
| `controller` | LISTBOX | `RowCount = numControllers * 2 = 138` ; per-row text = via FIELD "key" formatter | `0x138a` → `mbr_0x1b0=1, mbr_0x1c0=sender.selected` (begin rebind, §6.5 takes over) | (rebind buffer walked at `:363-386`; per controller `FUN_00439060`→patch→`FUN_00439110`; then `FUN_00439dc0("Controls")` persists INI) |
| `name0..name7` | TEXT (decoration) | `SetText(KeyName(GetRecord(iVar2/2 + i)[0]))` where `iVar2 = listbox->mbr_0x9c` | (read-only) | (read-only) |
| `ok` | BUTTON | (no getter) | `3000` → run Apply (§6.3) | (n/a — this IS the apply trigger) |
| `cancel` | BUTTON | (no getter) | `3000` → `FUN_004a98f0(DAT_005d7a48)` then dismiss | (n/a) |

**Notes:**
- The "step" column for sliders (`w->mbr_0xa0 = 1/0xc/0xf/1`) is the discrete step the scrollbar's arrows advance by per click; thumb-drag is continuous within `[min..max]`. **UNCONFIRMED-U** — the exact slider widget rendering of step vs continuous; verify by reading TDefScrollbar @ 0x42df00.
- "Enhanced" has the unusual "grey-out under certain GFX globals" path (`DAT_006697bc/c0/006680f4` — likely `g_d3dCapsSupported` / `g_softwareRendererOnly` / `g_someGFXLockoutFlag`). When the GPU doesn't support the enhanced lighting path, the toggle is forcibly displayed disabled but the snapshot field can still be toggled. **UNCONFIRMED-V** — resolve these globals.

---

## §7 — Effects & shadows

The screen itself does no effects work — every text/widget effect is owned by the widget. Notes for the reconstructor:

- **Text shadow.** `widgets.def` STYLE TEXT sets default `FLAGS TEXT_LEFT | TEXT_VCENTER | TEXT_SHADOW DRAWMODE DM_USEDEFAULT`. `TEXT_SHADOW = 0x400` (`widgets.def:24`) is the bit that `FUN_004be2b0` reads as the 3-pass-shadow gate (`param_8 & 0x400`, UI_METHOD_MAP §5 step 8). So every default-styled label here (3 column headers, 8 `name0..7`, 4 slider labels, all button labels) gets the standard 3-pass black drop shadow.
- **Pink-halo bug.** Same exposure as every retail DEF screen — the magenta chroma key + antialiased shadow edge produces a pink fringe. **Do NOT reproduce** (UI_METHOD_MAP §16; [[project-retail-pink-halo-bug]]). Render text via white-RGB + coverage-alpha into a TTF atlas (UI_METHOD_MAP §12 port mapping).
- **No "shadowed bitmap" (FUN_00438d80) on this screen.** All bitmap blits are widget-owned (FRAME 9-slice, BUTTON UP/DOWN, slider thumb, listbox scrollbar).

---

## §8 — Text rendering

The DEF parser packs each TEXT widget's `FLAGS` into the bit pattern read by `FUN_004be2b0`. Map (same as CharCreate_SPEC §8):

| DEF flag | bit | meaning |
|----------|-----|---------|
| `TEXT_LEFT` | 0x0001 | h-align left (default if none set) |
| `TEXT_CENTER` | 0x0002 | h-align center |
| `TEXT_RIGHT` | 0x0004 | h-align right |
| `TEXT_TOP` | 0x0010 | v-align top |
| `TEXT_BOTTOM` | 0x0020 | v-align bottom |
| `TEXT_VCENTER` | 0x0040 | v-align center |
| `TEXT_SINGLELINE` | 0x0080 | single-line, no wrap |
| `TEXT_SHADOW` | 0x0400 | enables 3-pass black drop shadow |
| `TEXTFLAG_RIGHT` | 0x00080000 | high-bit RIGHT (translated to TEXT_RIGHT by the TEXT attr parser, UNCONFIRMED — see UNCONFIRMED-I) |
| `TEXTFLAG_CENTER` | 0x00040000 | high-bit CENTER |
| `CTRLFLAG_CLEARBG` | 0x00001000 | clear background under text |

### §8.1 — Text element table

| element | cell (pane-local x,y,w,h) | font | px | color | h-align | v-align | shadow | format / value source | cite |
|---------|--------------------------|------|----|------|---------|---------|--------|----------------------|------|
| 7 toggle labels (`Realtime`/`Auto`/`Dialog`/`Face`/`Enhanced`/`Limit`/`NoCombatRes`) | each at `(button.x + 18, button.y - 1, 232, 21)` (= `STYLE BUTTON TOGGLE UPLABELRECT 18 -1 0 0` over the 250×20 toggle) | Med | (font metric) | `(255,255,255)` white | LEFT (`TEXT_LEFT`) | (default, top per UI_METHOD_MAP §5 v-align) | yes (`TEXT_SHADOW`) | literal text per row (see §4.1) | `widgets.def:96-99` |
| 4 slider labels | (260,70,150,20)…(260,169,150,20) per §4.2 | Med | (font metric) | `(255,255,255)` white | RIGHT (`TEXTFLAG_RIGHT`) | VCENTER (inherited from STYLE TEXT default) | yes (`TEXT_SHADOW`) | literal text per row | `options.def:19-22` + `widgets.def:84-86` |
| 3 column headers | (70,200,100,20) / (300,200,60,20) / (455,200,60,20) per §4.4 | Med | (font metric) | `(255,255,255)` white | LEFT (default) | VCENTER (default) | yes (default) | literal text per row | `options.def:32-34` |
| 8 decorative `name0..7` | (70, 224+i*20, 180, 20) per §4.6 | Med | (font metric) | `(255,255,255)` white | LEFT | VCENTER | yes (inherited) | literal text per row | `options.def:43-50` |
| LISTBOX row text (the FIELD "key" cell) | (per-row, 150×20 ITEM cell inside LISTBOX `controller`) | Med | (font metric) | `(255,255,100)` yellow-cream (255,255,100) | CENTER | (default top) | yes (`TEXT_SHADOW`) | `"key"` FIELD — resolves to the controller's key-name string (see UNCONFIRMED-D for the per-row layout) | `options.def:40` |
| LISTBOX default text (when row has no FIELD override) | per-row, FONT "Med" | Med | (font metric) | `(255,255,255)` white | CENTER + VCENTER + SINGLELINE | (per LISTBOX RECT) | yes | n/a | `options.def:38` |
| Button labels ("OK" / "Cancel") | inside each 100×30 button minus margins | Med | (font metric) | `(255,255,255)` white when UP; `(0,0,0)` black when DOWN | CENTER | VCENTER | yes when UP (`TEXT_SHADOW` in UP flags) | literal | `widgets.def:91-93` |

Notes on cells:
- **Toggle labels:** the `UPLABELRECT 18 -1 0 0` means the label rect's TL is `(button.x + 18, button.y - 1)` with `(w,h) = (0,0)` which the engine interprets as "remainder of button minus margins" (UNCONFIRMED-J — see). For a 250×20 button at (55,60), the label rect is approx (55+18, 60-1) = (73, 59) extending to `(55+250, 60+20)` = (305, 80) clipped: a ~232×21 label cell. The checkbox bitmap (16×17) sits at the button's TL (55,60).
- **Vertical alignment:** retail uses GDI `DrawTextA` with `DT_TOP` UNLESS `TEXT_VCENTER` is set. `STYLE TEXT` and `STYLE BUTTON NORMAL` both set `TEXT_VCENTER` (`widgets.def:86,91`), so labels DO vertically center in their cell. `STYLE BUTTON TOGGLE` (`widgets.def:97`) does NOT — so toggle labels hang from cell TOP per NOMENCLATURE §2.

### §8.2 — Font index decoding

DEF `FONT "Med"` maps to the medium-size HDC entry in the global font table at `DAT_0065b020` (per UI_METHOD_MAP §5 step 2). The TEXT attr parser (`0x42b340`) packs the font name + flag bits into the `param_8` word that `FUN_004be2b0` reads — bit `0x400` (TEXT_SHADOW) gates the 3-pass shadow, low bits select the HDC. UNCONFIRMED-I: the exact low-byte index for "Med" — likely `1` or `2` based on TPlyrStatusBar `0x401/0x402/0x404` font ids being shadow-bit + index.

---

## §9 — Animation & dynamic behavior

- **No screen-level animation.** No fades, no scrolls, no per-tick value updates managed by this class.
- **Per-widget animation** is owned by the widget classes:
  - Button hover/pressed states are instantaneous (no fade ramp like TSideTabsPane). The toggle bitmap swaps `CheckU` ↔ `CheckD` on click.
  - Slider thumb position updates discretely per drag.
  - LISTBOX row selection highlight (`SELCOLOR 0 100 30` dark green) draws under the selected row instantly.
- **Dirty/redraw model.** The DEF engine maintains a per-widget dirty bit (`CTRLFLAG_DIRTY = 0x20` per `widgets.def:35`). Rebind state changes set this on the controller LISTBOX (see §6.5 `:2209` — `(*+0x1c)(piVar7[5] | 0x20)`). The screen's outer "dirty" is `_DAT_0065cb40 = 1` set by case 0x52 (`FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:471`) which flags "redraw next frame" globally.
- **Rebind state machine** (see §6.5):
  ```
  state \ event:    keyDown(new)         keyDown(same)        keyUp(first)         clickRow(other)
  IDLE (mbr_0x1b0=0):  → CAPTURE,1key      (no change)         (no change)         (select row)
  CAPTURE (mbr_0x1b0=1, N keys):
    N=1 → CAPTURE,2keys                    (ignore)            → COMMIT (1-key chord)
    N=2 → CAPTURE,3keys                    (ignore)            → COMMIT (2-key chord)
    N=3 → COMMIT (3-key chord)             (ignore)            (already committed)
  ```
  Commit phase: write chord into `mbr_0x1ac[selectedRow]`, dedup other controllers' chords, reset `mbr_0x1b4..1c0 = 0xffffffff`, redraw.

---

## §10 — Input & dispatch

### §10.1 — Activation
| trigger | path | cite |
|---------|------|------|
| Main menu "MenuOptions" item | TLogoScreen item dispatch → spawns the screen (presumably `cls_0x5b9744::ctor() + ::Initialize()` flow, exact site UNCONFIRMED-K) | `agent_screens.txt:35` |
| In-game F-key (F2, command id `0x52`) | TPlayScreen `case 0x52` → `_DAT_0066fe3c=1; FUN_0053a8b0_LoadDef_options(); FUN_0048f040(&DAT_0066fcc0, …); FUN_0053aa60(); _DAT_0065cb40=1;` | `FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:466-471` |

> **NOTE on the case 0x52 sequence:** the dispatch calls Init **AND** Close back-to-back (Lines 468 and 470). The `FUN_0048f040(&DAT_0066fcc0, flags)` between them is the **modal input-grab pump** — it BLOCKS, processing input through the options screen, until the user dismisses. So the call sequence is:
> 1. Initialize the screen (DEF parsed, widgets shown, edit buffer cloned).
> 2. Enter the modal input pump — user interacts here; OK invokes Apply (`virt_meth_0x53aa90`), Cancel does nothing.
> 3. The pump returns when the user closes the screen.
> 4. Close (free rebind buffer + TButtonPane teardown).
>
> So on Cancel, the live globals are untouched. On OK, Apply has already run inside the pump (via the OK button's command dispatch). Resolve: extract `FUN_0048f040` to confirm the pump's structure.

### §10.2 — Hit rects (table)

All hit rects are the widgets' POS x y w h (per §4) — no special hit-test geometry beyond the standard widget rect. The 8 `name0..name7` decorative TEXTs do NOT consume mouse events (they have no NAME-bound action; only the LISTBOX rows accept clicks).

### §10.3 — Command id ↔ widget mapping — CONFIRMED 2026-05-29

The OnControl body (§6.7) ties every NAMED widget to its dispatch effect. Pinned table:

| NAME | DEF widget | msg_id(s) → effect |
|------|---|---|
| `Realtime`    | BUTTON TOGGLE @ (55,60)   | `0xbb9` → `mbr_0x180 = 1` ; `0xbba` → `mbr_0x180 = 0` |
| `Auto`        | BUTTON TOGGLE @ (55,80)   | `0xbb9` → `mbr_0x184 = 1` ; `0xbba` → `mbr_0x184 = 0` |
| `Dialog`      | BUTTON TOGGLE @ (55,100)  | `0xbb9` → `mbr_0x188 = 1` ; `0xbba` → `mbr_0x188 = 0` |
| `Face`        | BUTTON TOGGLE @ (55,120)  | `0xbb9` → `mbr_0x18c = 1` ; `0xbba` → `mbr_0x18c = 0` |
| `Enhanced`    | BUTTON TOGGLE @ (55,140)  | `0xbb9` → `mbr_0x190 = 1` ; `0xbba` → `mbr_0x190 = 0` |
| `Limit`       | BUTTON TOGGLE @ (55,160)  | `0xbb9` → `mbr_0x194 = 1` ; `0xbba` → `mbr_0x194 = 0` |
| `NoCombatRes` | BUTTON TOGGLE @ (55,180)  | `0xbb9` → `mbr_0x198 = 1` ; `0xbba` → `mbr_0x198 = 0` |
| `Violence`    | HSCROLLBAR @ (420,70)     | `4000` → `mbr_0x19c = sender.value` ; range 0..4 |
| `Music`       | HSCROLLBAR @ (420,103)    | `4000` → `mbr_0x1a0 = sender.value` ; **live-applies** via `FUN_0049a5c0` ; range 0..96 |
| `Sound`       | HSCROLLBAR @ (420,136)    | `4000` → `mbr_0x1a4 = sender.value` ; range 0..127 |
| `Gamma`       | HSCROLLBAR @ (420,169)    | `4000` → `mbr_0x1a8 = sender.value` ; **live-applies** via `FUN_004a98f0` ; range 0..4 |
| `controller`  | LISTBOX @ (60,220)        | `0x138a` → `mbr_0x1b0=1, mbr_0x1c0=sel` (begin rebind) ; `0x1389` → custom-row paint |
| `name0..name7`| TEXT @ (70,224+20i)       | painted by msg_id=1 + msg_id=0x1389; SetText(key-name for controller `iVar2/2 + i`) — see §6.7.1 |
| `ok`          | BUTTON @ (180,410)        | `3000` → run Apply (§6.3) → dismiss pump |
| `cancel`      | BUTTON @ (360,410)        | `3000` → re-apply gamma (effective no-op) → dismiss pump |
| `ControlSetup`| (not in options.def)      | `3000` → open Controller-setup sub-screen via `FUN_00537110_LoadDef_ingamemenu` — present in retail dispatcher but no corresponding BUTTON in this DEF; likely dead code or future feature |

Citations: `:198/:209/:222/:198` for toggle-DOWN names; `:308/:318/:332` for toggle-UP; `:282/:286/:288/:293` for slider names; `:347` ok / `:399` cancel / `:408` ControlSetup; `:240-244` LISTBOX select; `:163-194` name0..7 paint.

### §10.5 — Command id table (added)

| msg_id (hex/dec) | source class | semantic |
|---|---|---|
| `0x001` / 1     | (engine, post-parse)         | Init-refresh: paint snapshot → widgets |
| `0xbb8` / 3000  | TDefButton (BUTTON NORMAL click) | button-was-clicked; sender->name dispatches |
| `0xbb9` / 3001  | TDefButton (TOGGLE down)     | toggle moved to DOWN state |
| `0xbba` / 3002  | TDefButton (TOGGLE up)       | toggle moved to UP state |
| `0xfa0` / 4000  | TDefScrollbar                | value changed |
| `0x1389` / 5001 | TDefListBox                  | custom row paint hook |
| `0x138a` / 5002 | TDefListBox                  | row selected |

These reflect the actual values handled by `OnControl`; the per-widget-class **base id** (3000 / 4000 / 5000) is the obvious pattern. **UNCONFIRMED-T:** which exact `DAT_xxxx` constant in DefWidget_DispatchControl encodes the base — not yet checked but trivial to confirm via `DecompileAddr.java 0x00436ec0`.

### §10.4 — Modal-state globals (rebind)
| field | range | meaning |
|---|---|---|
| `mbr_0x1b0` | 0 / 1 | rebinding-in-progress flag |
| `mbr_0x1b4..1bc` | u32 × 3 | pending key chord (`0xffffffff` = unset) |
| `mbr_0x1c0` | s32 or `0xffffffff` | selected controller index |

---

## §11 — Retail bugs NOT to reproduce

1. **Pink halo around text** — chroma-key + antialiased shadow artifact (UI_METHOD_MAP §16). Render text via white-RGB + alpha-coverage atlas; do NOT magenta-key the text composite.
2. **`name0..name7` overlap with LISTBOX rows** — RESOLVED — these widgets are NOT decoration; they're TEXT widgets the OnControl handler `SetText()`'s with key-name strings for the first 8 controllers (§6.7.1). They do still overlap the LISTBOX visually, but this is intentional retail behavior. Render them as the DEF says.
3. **`FLAGS ... FONT ... FLAGS ...` double clause in `options.def:43-50`** — likely a DEF authoring bug (only the second `FLAGS` clause should win in a left-to-right parser; the first is silently overridden). For the reconstructor: parse it the same way the engine does (UNCONFIRMED-G) — see §4.6.
4. **Commented-out `Key 3` column at `options.def:35`** — the binary supports 3 keys per chord (per `meth_0x53ba80`'s mbr_0x1b4/1b8/1bc trio + meth_0x439060's 0x13-dword record), but the UI only shows 2 columns. Not a "bug to not reproduce" — it's just an unfinished feature. The reconstructor should match retail (2 columns shown).
5. **Music-volume Cancel doesn't rewind** (NEW — see UNCONFIRMED-S). The Music slider drag LIVE-applies `FUN_0049a5c0(new_vol)` (§6.7.5); Cancel only rewinds gamma (`:401`) and not music. So if the user drags Music to 0 then hits Cancel, music stays at 0. **The reconstructor should**: snapshot original `mbr_0x1a0_original` separately at Init and re-apply on Cancel. Same fix should apply for any future setting that live-applies during drag.
6. **Cancel's gamma "rollback" is a no-op** (NEW — see §6.7.2). Apply writes `mbr_0x1a8 → DAT_005d7a48` then calls `FUN_004a98f0`. The Cancel branch calls `FUN_004a98f0(DAT_005d7a48)` — but DAT_005d7a48 has already been mutated to the LIVE-dragged value via msg_id=4000 (`:299`). So Cancel applies the dragged value (not the original). Effectively gamma changes commit on slider release, regardless of OK/Cancel. **The reconstructor should**: snapshot original gamma at Init, restore that on Cancel (NOT the current live value).
7. **`ControlSetup` button is dead code in this DEF** (NEW — §6.7.2). The OnControl handler has a fully implemented branch for a button named `"ControlSetup"` that opens the in-game-menu controller-setup sub-screen, but `options.def` declares no such button. The handler code is unreachable in retail. The reconstructor can safely omit it.

---

## §12 — Reconstruction pseudocode

```cpp
class TOptionsScreen : public TButtonPaneIntermediate /* cls_0x5b98b8 */ {
    // 8-toggle + 4-slider snapshot (cf. §4.1, §4.3)
    int32_t opt_Realtime;       // mbr_0x180
    int32_t opt_Auto;           // mbr_0x184
    int32_t opt_Dialog;         // mbr_0x188
    int32_t opt_Face;           // mbr_0x18c
    int32_t opt_Enhanced;       // mbr_0x190
    int32_t opt_Limit;          // mbr_0x194  (inverted re-pack: live global is "no-limit" 0/1)
    int32_t opt_NoCombatRes;    // mbr_0x198
    int32_t opt_Violence;       // mbr_0x19c
    int32_t opt_Music;          // mbr_0x1a0
    int32_t opt_Sound;          // mbr_0x1a4
    int32_t opt_Gamma;          // mbr_0x1a8

    void*   rebindBuf;          // mbr_0x1ac — malloc(NumControllers * 36)
    int32_t rebinding;          // mbr_0x1b0  (0/1)
    int32_t pendingKey[3];      // mbr_0x1b4/1b8/1bc  (0xffffffff = unset)
    int32_t selectedRow;        // mbr_0x1c0  (0xffffffff = none)

    int32_t opt_inlineMode;     // mbr_0x17c — set externally before Initialize

    bool Initialize() {                 // = virt_meth_0x53a8b0
        // 1) Open the DEF screen
        DefScreen_Open(
            /*panel*/        "options",
            /*screen_root*/  "default",
            /*flags*/        (opt_inlineMode != 0) ? 0x11 : 0,
            /*x,y*/          0, 0,
            /*w,h*/          640, 480,
            /*hi_w,hi_h*/    450, 160,
            /*widgets_lib*/  "widgets"
            /*bg_dat*/       /* (omitted; chrome via "background" token) */
        );

        // 2) Snapshot live settings (see §6.1 step 2)
        opt_Realtime    = g_DrawRealtimeShadows;      // DAT_005d7a18
        opt_Auto        = g_AutoBeginCombat_;          // DAT_005d7a68  (UNCONFIRMED exact semantic)
        opt_Dialog      = g_ShowDialog_;               // DAT_005d7a60
        opt_Face        = g_AlwaysFaceEnemy_;          // DAT_005d7a64
        opt_Enhanced    = g_Enhanced3DLighting;        // DAT_005e91c0
        opt_Limit       = (g_NoLimitGameSpeed == 0);   // DAT_006682a8 — inverted
        opt_NoCombatRes = g_NoCombatRes;               // DAT_00668194
        opt_Violence    = g_ViolenceLevel;             // DAT_005d79e8  (default 5)
        opt_Music       = g_MusicVolume;               // DAT_005d7a9c  (default 127)
        opt_Sound       = g_SFXVolume;                 // DAT_005d7aa0  (default 127)
        opt_Gamma       = g_GammaLevel;                // DAT_005d7a48  (default 3)

        // 3) Init rebind state
        rebinding   = 0;
        pendingKey[0] = pendingKey[1] = pendingKey[2] = -1;
        selectedRow = -1;

        // 4) Allocate edit buffer + snapshot keymap
        free(rebindBuf);
        const int N = g_ControlList.numControllers;   // DAT_005d697c = 69
        rebindBuf = malloc(N * 36);
        for (int i = 0; i < N; ++i) {
            uint32_t rec[19];
            g_ControlList.GetRecord(i, rec);          // = cls_0x439000::meth_0x439060
            memcpy((char*)rebindBuf + i*36, rec, 36); // copy first 36 of 76 bytes
                                                      //   (UNCONFIRMED-H — which 36 bytes exactly)
        }

        DefWidget_ParseBuffer(this);
        return true;
    }

    void DispatchInput(int vkey) override {           // = virt_meth_0x53b9e0
        auto* listbox = DefWidget_GetFocused(this);
        if (listbox && listbox->selectedRow >= 0)
            RebindAccumulate(vkey);                   // §6.5
        else
            DefWidget_DispatchInput_base(this, vkey); // = virt_meth_0x4361f0
    }

    void ApplySettings() {                            // = virt_meth_0x53aa90 (OK button)
        g_DrawRealtimeShadows = opt_Realtime;
        g_AutoBeginCombat_    = opt_Auto;
        g_ShowDialog_         = opt_Dialog;
        g_AlwaysFaceEnemy_    = opt_Face;
        g_Enhanced3DLighting  = opt_Enhanced;
        g_NoLimitGameSpeed    = (opt_Limit == 0) ? 1 : 0;
        g_NoCombatRes         = opt_NoCombatRes;
        g_ViolenceLevel       = opt_Violence;
        g_MusicVolume         = opt_Music;  audio::SetMusicVolume(opt_Music / 127.0f);
        g_SFXVolume           = opt_Sound;
        g_GammaLevel          = opt_Gamma;
        for (int i = 0; i < g_ControlList.numControllers; ++i) {
            g_ControlList.PutRecord(i, (char*)rebindBuf + i*36);  // = cls_0x439000::meth_0x439110
        }
    }

    void Close() {                                    // = virt_meth_0x53aa60
        free(rebindBuf); rebindBuf = nullptr;
        TButtonPaneIntermediate::Close();
    }
};
```

**No screen-level `paint()`** — the inherited DefScreen base walks the widget tree.

---

## §13 — Port mapping notes

| retail primitive used | port equivalent | notes |
|-----------------------|------------------|-------|
| `FUN_00435150_DefScreen_Open(..., panel, root, flags, x,y,w,h,hi_w,hi_h, widgets_lib)` | DefScreen engine port (not yet implemented) | shared with all DEF screens — `DefWidgetEngine.md:106` outstanding |
| `cls_0x439000::meth_0x439060(table, idx)` (read controller record) | `TControlList::GetRecord(idx, dst19dwords)` | The global lives at `&UNK_0065a904.field_0xa4`; port wraps as a singleton |
| `cls_0x439000::meth_0x439110(table, idx, src)` (write controller record) | `TControlList::PutRecord(idx, src9dwords)` | (called from Apply slot) |
| `FUN_00482fb0(n)` | `operator new` / `malloc(n)` | (CRT, for `mbr_0x1ac`) |
| `FUN_004830f0(p)` | `free(p)` | (CRT) |
| `FUN_0048f040(&screen, flags)` | modal input pump | Same helper used by save/load screens; extract on demand if pump structure is needed |
| TEXT widget via `FUN_004be2b0` | `Font->DrawTextShadowedToTarget(...)` (UI_METHOD_MAP §5 + §12) | DEF engine routes here per-TEXT widget |
| BUTTON / TOGGLE / SCROLLBAR / LISTBOX widget renderers | DEF widget engine subclasses (not yet ported) | `DefWidgetEngine.md:113` lists EDIT/DROPLIST/etc. as outstanding work |

**Missing primitives the reconstructor must add:**
1. `BMSurface` decoder for **`BM_TEXTURE` (flag 0x10000)** — needed for `optionstex.dat`. Defer to Classic path (`optionsnotex.dat`) for now (CharCreate_SPEC §13 has the same TODO).
2. **DEF widget engine** (entire): DefScreen_Open + BITMAP/FRAME/TEXT/BUTTON/SCROLLBAR/LISTBOX/EDIT/DROPLIST + renderer walker + input dispatcher. Shared with CharCreate_SPEC §13.
3. **TControlList singleton** — the global key-binding table accessor + Get/Put record API. Snapshot's `src/control.*` (if it exists) might already have the snapshot equivalent; verify before porting.
4. **Modal input pump** — the `FUN_0048f040` family. CharCreate also needs this; build once and share.

---

## §14 — UNCONFIRMED / open questions

### Resolved 2026-05-29 (struck through)

| # | Item | Resolution |
|---|------|-----------|
| ~~**B**~~ | ~~`virt_meth_0x53aa90` (Apply settings) body~~ | RESOLVED — extracted via `DecompileAddr.java 0x0053aa90` (`/tmp/options_decomp/apply_53aa90.cpp`). Apply is one branch (msg_id=3000 + name="ok") of a multiplexed OnControl handler. Full decode in §6.3 + §6.7. |
| ~~**D**~~ | ~~LISTBOX "controller" row format vs the 8 decorative `name0..name7` labels~~ | RESOLVED — see §6.7.1: `name0..name7` are NOT static decoration. The OnControl (msg_id=1 and 0x1389) populates them with key-name strings via `FUN_0046e7f0(buf, 100, key_code)` for the first 8 controllers. They are a "mini 8-row key-binding display" overlaid on the LISTBOX area. LISTBOX row count is `numControllers * 2 = 138` (so each controller occupies TWO rows — likely "primary chord" + "secondary chord" per binding). |
| ~~**F**~~ | ~~Slider value range per FIELD~~ | RESOLVED — set in OnControl msg_id=1 via `FUN_0042e3d0(w, min, max)`: Violence 0..4, Music 0..96, Sound 0..127, Gamma 0..4. The step granularity (`w->mbr_0xa0`) is 1/12/15/1 respectively. |
| ~~**L**~~ | ~~Per-NAME→cmdId mapping (the buttons' click → dispatch ids)~~ | RESOLVED — see §10.3/§10.5. msg_id encodes widget class (3000s=button, 4000=scrollbar, 5000s=listbox); sender->name dispatches to the per-field setter. OK runs Apply; Cancel re-applies gamma. |

### Outstanding (revised)

| # | Item | What we tried | Impact if wrong | Resolve step |
|---|------|---------------|-----------------|--------------|
| **A** | Confirm the class is "TOptionsScreen" (no string anchor in binary) | Searched `recon/classes/_data.txt` for "Options"/"option" — only hits are the `s_options_005e4494` DEF-name and `s_Controls_005ce634` ("Controls" — used by `cls_0x439000::meth_0x439dc0` to write to a "[Controls]" INI section). No "TOptionsScreen" / "OptionsScreen" symbol. | Cosmetic only — the class identity is still confirmed by code shape (the only fn that DEF-loads "options"); any name will do. | Treat as a confirmed-by-behavior identification; do not block the port on this. |
| **C** | What VK code is `case 0x52` in TPlayScreen dispatch — is it the F2 key or a synthesized command id? | The case label is `0x52` = 82 dec. VK_F1 is 0x70..VK_F12 is 0x7B, so `0x52` is NOT a function key directly; it's the **'R' VK** (`VK_R` in Win32 is `0x52`). But `agent_ui_wave4_sidebar_cascade.txt:101` claims F2 = "options" — there's likely an indirection (the F-key gets translated into a command id `0x52` by an earlier dispatch layer; or the actual options hotkey is 'R'). | Wrong hotkey — Options might be on 'R' (re-bindable as a Controller) rather than F2. | Trace the call path INTO TPlayScreen::Command with the case-0x52 entry — `FindCallersOf.java 0x0047cf40` then look at what command id is computed. |
| **E** | Bit-`0x10` purpose in `flags = 0x11` | Saw `flags & 1` test in `DefScreen_Open` → MP widget cache swap. `flags & 0x10` not pinned in any extracted body. | Probably some UI variant (popup-mode vs full-screen-mode); incorrect treatment would change layout flag dispatch. | `DecompileAddr.java 0x00435150` deeper + look for `flags & 0x10` test sites in `DefScreen_LoadAndShow` or downstream. |
| **G** | Double-`FLAGS` clause parser behavior in `options.def:43-50` | Looked at the DEF; the tokenizer (`FUN_00478720` family) is not extracted. | `name0..7` labels may render with wrong flags (e.g. losing CTRLFLAG_CLEARBG). | Extract `0x42a350` (TWidget base parse-time init) — it handles FLAGS for every widget. |
| **H** | Which 9 dwords of the 19-dword `meth_0x439060` output the Init step 5 keeps | The 3-outer × 3-inner loop is structurally clear (9 dwords copied per controller); the per-call output dword is selected from `local_40[16]`. The exact indices and which (variant, sub) maps to which key slot is not pinned. With the Apply body extracted, the inverse map at `:363-386` is also a 3×3 over 9 dwords — confirming the structure but not the per-slot key-id semantic. | Wrong key snapshot — UI shows wrong current bindings. | Extract `cls_0x439000::meth_0x439060` more carefully + step through the Init loop disassembly. |
| **I** | Font-id low-byte for "Med" | UI_METHOD_MAP §5 shows the HDC table is indexed by the low byte of the font id, but the "Med" → index mapping is not in any extracted recon. | Text rendered with wrong font (sub-pixel size differences). | `FindCallsTo.java 0x004be2b0` and tabulate the font-id arg per call site; cross-check against the font.def "Med" definition. |
| **J** | LABELRECT semantics — does `(18,-1,0,0)` mean "(x,y) offset + (w,h)=0 meaning fill remainder" or "(left,top,right,bottom) inset"? | The TOGGLE STYLE block uses `(18,-1,0,0)`; the NORMAL block uses `(0,0,0,0)`. Implicit "fill" treatment matches the visual (toggle label sits to the RIGHT of the checkbox). | Toggle labels mispositioned. | Extract `FUN_0042bd90` (BUTTON attr parser) for the UPLABELRECT/DOWNLABELRECT semantics. |
| **K** | Where the TOptionsScreen instance is allocated (`DAT_0066fcc0`) — static? Heap-once-on-app-init? | The instance is at a fixed `.bss` address (0x0066fcc0..0x0066fe88), so it's **statically embedded** in the binary (not heap-allocated). The ctor `0x53bdc0` must be called from a global-init `_initterm` or app boot path. | Cosmetic for the port; the port likely just `new TOptionsScreen` at app start. | `FindCallersOf.java 0x0053bdc0`; check for static-initializer presence. |
| **M** | Whether `DAT_00668194` ("NoCombatRes") is the correct semantic for `mbr_0x198` | The XREF table shows it's read by Init and written by Apply (`:352` = `DAT_00668194 = param_1[0x66]`), and the DEF has a NoCombatRes toggle in the 7th slot. Confirmed by position alignment via OnControl (§6.7.3 `name="NoCombatRes"` → `mbr_0x198`), NOT by string anchor in the live global. | Wrong toggle is wired. | Inspect callers of `DAT_00668194` for usage that matches "No Combat Results" semantic (e.g. combat-log suppression in TCharacter::ResolveAttack). |
| **N** | Snapshot global names for the 7 toggles | Inferred from snapshot src/revmain.cpp:184/209/211 (DrawRealtimeShadows, AutoBeginCombat, ShowDialog). The other 4 (Face, Enhanced, Limit, NoCombatRes) don't have a directly-named snapshot global — likely renamed or added late. | Apply writes to wrong global → setting changes silently ignored. | Grep snapshot src/ for AlwaysFace/FaceEnemy/EnhancedLight/LimitGame/CombatRes patterns; cross-check XREF source files. (Confirmed for Face/Enhanced/Limit/NoCombatRes: NOT in snapshot src/. They are retail-only.) |
| **O** | The `FUN_0048f040(&DAT_0066fcc0, flags)` flags arg `(-(uint)(DAT_0066829c != 0) & 0xfffffff8) + 8 | 7` | Same pattern used for savegame (case 0x53/0x54) — appears to be a state-bit recipe for the modal pump. | Modal pump may behave differently. | Extract `FUN_0048f040` and decode the flags interpretation. |
| **P** | `FUN_0049a5c0` semantic | Called by Apply (`:356`) and by Music slider drag (`:304`) with the live music volume value. Inferred `audio::SetMusicVolume`. | If wrong, the Music slider does nothing audible. | `DecompileAddr.java 0x0049a5c0`; look for DirectSound `IDirectSoundBuffer::SetVolume` or our internal mixer call. |
| **Q** | `FUN_004a98f0` semantic | Called by Apply (`:359`), Cancel (`:401`), and by Gamma slider drag (`:299`). Inferred `SetGammaTable` / `BuildGammaRamp`. | If wrong, the Gamma slider does nothing visible. | `DecompileAddr.java 0x004a98f0`; look for `IDirectDrawGammaControl::SetGammaRamp`. |
| **R** | `FUN_00453640(DAT_006671a4, 1)` semantic | Per `agent_world.txt:297` this is the daylight/ambient relight trigger. Called by Apply (`:360`); arg is `&world_light_state, force=1`. The Apply call here likely propagates Enhanced3D / Realtime toggle changes to the renderer. | If wrong, settings might not take effect until a level re-load. | `DecompileAddr.java 0x00453640`. |
| **S** | Music-volume rollback on Cancel | Apply re-applies gamma on Cancel (`:401` `FUN_004a98f0(DAT_005d7a48)`) but NOT music; Music drags `FUN_0049a5c0()` LIVE which mutates the audio mixer state, then Cancel does not rewind. **Retail bug.** | Cancel doesn't fully cancel — music volume stays at the dragged value. | The reconstructor should snapshot the original music volume at Init and restore on Cancel. |
| **T** | DefWidget_DispatchControl @ `0x436ec0` per-widget-class message-id base | Inferred from the OnControl branches: button=3000+{0,1,2}, scrollbar=4000+{0}, listbox=5000+{1,2}. | If wrong, custom widgets in the port might be routed to wrong handlers. | `DecompileAddr.java 0x00436ec0`. |
| **U** | TDefScrollbar @ `0x42df00` step semantics (`w->mbr_0xa0`) | Set to 1/12/15/1 for Violence/Music/Sound/Gamma. Inferred = "discrete step per arrow click." | If wrong, arrow buttons jump wrong amounts. | `DecompileAddr.java 0x0042df00`. |
| **V** | `DAT_006697bc`, `DAT_006697c0`, `DAT_006680f4` ("Enhanced" disable trio) | OnControl msg_id=1 disables the Enhanced toggle visually when `(006697bc==0 && 006697c0==0) \|\| 006680f4!=0`. Inferred = D3D-caps + software-renderer flags. | If wrong, the toggle disable logic is broken. | Cross-XREF those globals; they're set by the renderer-init path (likely 3dscene.cpp / TDirect3D). |

---

## Summary

**`cls_0x5b9744` (TOptionsScreen)** — a 456 B retail DEF-screen subclass with a 46-slot vtable, dedicated ctor (`0x53bdc0`), dtor (`0x53be20`), Initialize (`0x53a8b0`), Close (`0x53aa60`), input-dispatch override (`0x53b9e0`), and OnControl-dispatcher / Apply (`0x53aa90`, body extracted 2026-05-29). Renders `options.def` over the `optionsnotex.dat / Background` 640×480 chrome ("OPTIONS" gold title plate). Layout pane = screen = `(0,0)..(640,480)` — every coordinate is pane-local, no sub-frame anchoring (the DEF's two FRAME blocks are commented out). The screen edits SNAPSHOTS of 11 setting globals (`mbr_0x180..0x1a8`) + a 2484 B per-controller key-rebind buffer (`mbr_0x1ac = malloc(69 × 36)`); on OK (msg_id=3000 + name=="ok"), Apply writes both back to the live globals + the global TControlList (`cls_0x439000::meth_0x439110`) + INI-persists via `FUN_00439dc0("Controls")`; on Cancel, the snapshot is simply freed (BUT gamma + music live-apply during drag — see §11 bugs). Activated via case `0x52` in TPlayScreen's dispatch (`FUN_0047cf40 :466`) or from `MenuOptions` in TLogoScreen.

**§3 frame table:** screen → pane (TL @ 0,0, 640×480) → chrome (TL @ 0,0, 640×480 = optionsnotex.Background). No sub-frames. Composition: `screen_x = pane_local_x = options.def POS x`.

**Closed-out items (2026-05-29):**
- **OnControl body extracted** (§6.3 + §6.7) — 7 message-id branches (1 / 3000 / 0xbb9 / 0xbba / 4000 / 0x1389 / 0x138a).
- **Apply settings write order pinned** (§6.3) — verbatim retail order of 11 setting-global writes + 69-controller rebind walk + INI persist.
- **FIELD → getter/setter/apply table pinned** (§6.8) — every widget's snapshot field, live setter, and apply target.
- **command-id ↔ button mapping pinned** (§10.3 + §10.5) — sender->name dispatches per msg-id-class.
- **Slider value ranges pinned** (§6.7.1) — Violence 0..4, Music 0..96, Sound 0..127, Gamma 0..4.
- **`name0..name7` role corrected** (§6.7.1) — paint targets for per-row key-names of the first 8 controllers, not static labels.
- **New retail bugs flagged** (§11) — music-volume Cancel doesn't rewind; gamma Cancel rewinds to already-mutated value; `ControlSetup` branch is dead code.

**Remaining open items** (see §14, ~10 of original 15 + 5 new): all secondary — the `0x10` flag bit purpose, exact `meth_0x439060` 9-of-19-dword selection, "Med" font index, semantic of side-effect helpers `FUN_0049a5c0/004a98f0/00453640`, and the per-widget-class msg-id base. None block runtime correctness of OK/Apply (which is now fully decoded); they block confirmatory semantics for the few helper calls.
