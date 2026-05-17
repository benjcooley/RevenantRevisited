# TTextBar port status

**Retail vtable: UNKNOWN.** The retail TTextBar leaf vtable has NOT been identified yet. See [B.r4](../../../docs/ui/briefs/B_r4_textbar_assessment.md) for the full evidence trail.

**Pre-existing assumption is WRONG.** `cls_0x5a4358_likely_TTextBar.{cpp,h}` in `recon/classes_converted/` is mis-labeled — those files actually contain `TConsolePane` (editor command console). See B.r4 §"Class identity confirmation" for the decisive string-anchor evidence.

**Our source:** [src/textbar.h](../../../src/textbar.h), [src/textbar.cpp](../../../src/textbar.cpp). Pre-release implementation; awaiting retail-feature splice.

**Real retail TTextBar lives in:** `FUN_0054bf70` (init wrapper, "text bar" string anchor) and the method cluster `0x54c000`..`0x54d800`. Need a focused extraction pass before per-method porting can begin.

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started · ❓ unidentified. See [README.md](README.md).

## Known/extracted addresses

| Address | Role | Our source | Status | Notes |
|---|---|---|---|---|
| `FUN_0054bf70` | TTextBar init wrapper | n/a | 🟡 extracted, unstructured | 1110-byte body in `recon/discovered/FUN_0054bf70_TTextBar_init.cpp`. Allocates `0x450`-byte (1104) instance + three TMosaicSurface child surfaces + initializes fields up to `+0x120`. Confirms multi-buffered transparent rendering substrate. |
| `FUN_0054cb00` | TTextBar::DrawBackground (health-bar inner block) | textbar.cpp:64-73 | ❓ identified, not vetted | Body in `recon/discovered/cls_0x5a5320_TPlayScreen_or_TTextBar_DrawHealth_54cb00.cpp` (mis-attributed to TCharacter in recon). Constants match pre-release exactly (155/176/16/186, /4 increment). |
| `FUN_0054cd40` | TTextBar::DrawBackground (outer, suspected) | textbar.cpp:33-85 | ⚫ not-started | Tight caller cluster (8 callers all in 0x54c000..0x54d800); calls FUN_0054cb00. Likely the outer DrawBackground dispatch. |
| `FUN_0054c440` | unknown sibling | — | ⚫ not-started | Likely Print/Clear/Animate. |
| `FUN_0054c9c0` | unknown sibling | — | ⚫ not-started | |
| `FUN_0054ca60` | unknown sibling | — | ⚫ not-started | |
| `FUN_0054cad0` | unknown sibling | — | ⚫ not-started | |
| `FUN_0054d0c0` | unknown sibling | — | ⚫ not-started | |
| `FUN_0054d390` | unknown sibling | — | ⚫ not-started | |
| `FUN_0054d4a0` | unknown sibling | — | ⚫ not-started | |
| (leaf vtable) | TTextBar vtable address | n/a | ❓ unidentified | Constructor not yet extracted; should be reachable via the `new` site inside `FUN_0054bf70` or its caller. |

## Pre-release API surface (likely retained in retail; needs per-method confirmation)

| Method | Pre-release source | Retail addr | Status | Notes |
|---|---|---|---|---|
| `Initialize()` | textbar.cpp:16 | likely `FUN_0054bf70` body or its callee | 🟡 partial (pre-release) | Retail allocates 3 mosaic surfaces + 1104-byte instance — substantially richer; will need restructure. |
| `Close()` | textbar.cpp:27 | unidentified | ⚫ not-started | Pre-release is a no-op; retail likely frees the 3 surfaces. |
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

Block on a focused extraction wave: dump every function in `0x54c000`..`0x54d800`, find which one is the constructor (sets `*this = &vtable_XXXXXX`), then identify the leaf vtable and walk it against pre-release `src/textbar.h`'s public API.

Coordinator should also retract the `cls_0x5a4358 ↔ TTextBar` mapping in:

- `recon/docs/CLASS_MAPPING.md` (line ≈ "Text Bar")
- `docs/ui/RECON_UI_COVERAGE.md` (lines 37, 44, 62, 213, 238)
- `docs/ui/BURNDOWN.md` (lines 56, 130)
- `docs/ui/RETAIL_UI_RECOVERY_PLAN.md` (line 104)

and rename `recon/classes_converted/cls_0x5a4358_likely_TTextBar.{cpp,h}` to `cls_0x5a4358_TConsolePane.{cpp,h}`.
