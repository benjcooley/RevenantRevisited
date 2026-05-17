# TTextBar port status

**Retail vtable: `0x5a5560` (cls_0x5a5560).** Identified by Wave-2A — see [B.r4 §"Wave-2A leaf-class identification"](../../../docs/ui/briefs/B_r4_textbar_assessment.md#wave-2a-leaf-class-identification-2026-05-16--cls_0x5a5560--ttextbar-confirmed) for full evidence chain (string anchor + .CRT$ ctor stub + leaf-TPane vtable shape + Close at slot 1 matches Initialize's allocation structure).

**Global instance:** `0x65c5d0` (singleton).

**Pre-existing assumption is WRONG.** `cls_0x5a4358_likely_TTextBar.{cpp,h}` in `recon/classes_converted/` is mis-labeled — those files actually contain `TConsolePane` (editor command console). See B.r4 §"Class identity confirmation" for the decisive string-anchor evidence.

**Our source:** [src/textbar.h](../../../src/textbar.h), [src/textbar.cpp](../../../src/textbar.cpp). Pre-release implementation; awaiting retail-feature splice.

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started · ❓ unidentified. See [README.md](README.md).

## Known/extracted addresses

| Address | Role | Our source | Status | Notes |
|---|---|---|---|---|
| `cls_0x5a5560` | TTextBar leaf vtable | n/a | ✅ confirmed | 31 slots, leaf-TPane shape (slot 2 = 0x491bd0, slot 9 = 0x491a80, slot 16 = 0x491bb0). Same shape as TBottomPane/TSidePane (Wave-1A). Wired by global-ctor stub `FUN_00480725`. |
| `FUN_00480725` | TTextBar global ctor stub | n/a | ✅ confirmed | Body in `recon/discovered/cls_0x5a5560_TTextBar_ctor_globalinit_480725.cpp`. Sets `*0x65c5d0 = &PTR_FUN_005a5560`, inits rect 406×198 + padding 14. |
| `FUN_0054bf70` | **TTextBar::Initialize** (slot 0) | textbar.cpp:16 | 🟡 extracted, unstructured | 1110-byte body in `recon/discovered/FUN_0054bf70_TTextBar_init.cpp`. Allocates `0x450`-byte (1104) text buffer at +0x6c + three TMosaicSurface child surfaces at +0x84/+0x88/+0x8c + initializes fields up to `+0x120`. Confirms multi-buffered transparent rendering substrate. |
| `FUN_0054c3d0` | **TTextBar::Close** (slot 1) | textbar.cpp:27 | ✅ confirmed | Body in `recon/discovered/cls_0x5a5560_TTextBar_Close_54c3d0.cpp`. Frees text buffer at +0x6c + the three TMosaicSurfaces at +0x84/+0x88/+0x8c, then calls TPane::Close (FUN_00491970). Mirrors Initialize structure exactly — STRUCTURAL CONFIRMATION of leaf class. |
| `FUN_0054c600` | (slot 7 override) | — | ⚫ not-started | |
| `FUN_0054c9c0` | (slot 13 override) | — | ⚫ not-started | |
| `FUN_0054c460` | (slot 19 override) | — | ⚫ not-started | |
| `FUN_0054c440` | (slot 20 override) | textbar.cpp candidate | ⚫ not-started | Likely Print/Clear/Animate. |
| `FUN_0054c780` | (slot 23 override) | — | ⚫ not-started | |
| `FUN_0054d4a0` | (slot 28 override) | — | ⚫ not-started | |
| `FUN_0054cb00` | TTextBar::DrawBackground (health-bar inner block) | textbar.cpp:64-73 | ❓ likely-internal-helper | Body in `recon/discovered/cls_0x5a5320_TPlayScreen_or_TTextBar_DrawHealth_54cb00.cpp` (mis-attributed to TCharacter in recon). Constants match pre-release exactly (155/176/16/186, /4 increment). NOT in vtable directly — called by one of the overridden slots. |
| `FUN_0054cd40` | TTextBar::DrawBackground (outer, suspected) | textbar.cpp:33-85 | 🟡 extracted | Body in `recon/discovered/cls_0x5a5560_TTextBar_FUN_54cd40.cpp`. References this+0x84 (first mosaic surface) + reads this+0x7c — confirmed TTextBar member. NOT in vtable directly; called by a virtual. Likely the outer DrawBackground dispatch. |

## Pre-release API surface (likely retained in retail; needs per-method confirmation)

| Method | Pre-release source | Retail addr | Status | Notes |
|---|---|---|---|---|
| `Initialize()` | textbar.cpp:16 | `FUN_0054bf70` (vtable slot 0) | 🟡 partial (pre-release) | Retail allocates 3 mosaic surfaces + 1104-byte instance — substantially richer; will need restructure. |
| `Close()` | textbar.cpp:27 | `FUN_0054c3d0` (vtable slot 1) | ✅ confirmed | Pre-release is a no-op; retail frees the 3 mosaic surfaces + text buffer, then calls TPane::Close. |
| `DrawBackground()` | textbar.cpp:33 | `FUN_0054cd40` (suspected) + `FUN_0054cb00` (inner health) | 🟡 partial | Health-bar inner block confirmed; outer dispatch needs extraction. Pre-release direct-draw replaced by mosaic-buffered composition in retail. |
| `Print(char *, ...)` | textbar.cpp:87 | unidentified | ⚫ not-started | Pre-release: vsprintf into `text[80]`. Retail likely appends to scrollback buffer. |
| `Clear()` | textbar.h:32 (inline) | likely inlined or trivial | ⚫ not-started | |
| `SetHealthDisplay(name, level)` | textbar.cpp:97 | unidentified | ⚫ not-started | Confirmed feature SURVIVES into retail (health-bar block at `FUN_0054cb00` uses same tween/hue logic). |
| `ClearHealthDisplay()` | textbar.cpp:119 | unidentified | ⚫ not-started | |
| `SetLevels(level, targetlevel)` | textbar.cpp:111 | unidentified | ⚫ not-started | |

## NEW retail features (suspected; need extraction confirmation)

| Feature | Evidence | Status |
|---|---|---|
| Multi-line scrollback buffer | `0x450`-byte instance (vs pre-release ~256), CLASSIC_HUD_REFERENCE.md describes multi-line overlay | ❓ unconfirmed |
| Color-per-line / colored text | `{`/`}` color-escape pattern visible in TConsolePane (sibling editor class); likely shared util | ❓ unconfirmed |
| Transparent buffered rendering | Three TMosaicSurface child surfaces allocated at init (FUN_0054bf70 lines 84, 96-108, 115+) | ✅ confirmed in init body |
| Per-line fade / age-out animation | Inferred from "scrollback" + tweening pattern; not yet seen | ❓ unconfirmed |
| Health-display opponent overlay | EXACT constants match pre-release (155/176/16/186) in FUN_0054cb00 | ✅ confirmed retained |

## Next action

**Leaf vtable + slot 0/1 now done.** Next: extract and identify the six remaining overridden slots (7, 13, 19, 20, 23, 28 — all addresses in `0x54c000..0x54d800`) against pre-release `src/textbar.h`'s public API (`Print`, `Clear`, `Pulse`, `Animate`, `DrawBackground`, `SetHealthDisplay`, `SetLevels`, `ClearHealthDisplay`).

Coordinator should also retract the `cls_0x5a4358 ↔ TTextBar` mapping in:

- `recon/docs/CLASS_MAPPING.md` (line ≈ "Text Bar")
- `docs/ui/RECON_UI_COVERAGE.md` (lines 37, 44, 62, 213, 238)
- `docs/ui/BURNDOWN.md` (lines 56, 130)
- `docs/ui/RETAIL_UI_RECOVERY_PLAN.md` (line 104)

and rename `recon/classes_converted/cls_0x5a4358_likely_TTextBar.{cpp,h}` to `cls_0x5a4358_TConsolePane.{cpp,h}`.
