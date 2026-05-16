# UI Resurrection — Entry Point

This directory is the source of truth for bringing the **HUD** (in-game heads-up) and **OOG** (Out-Of-Game: main menu, save/load, character create, options, network) UI back online on the modernized RevenantRevisited engine.

If you're picking this up cold, read this file, then [ARCHITECTURE.md](ARCHITECTURE.md), then [BURNDOWN.md](BURNDOWN.md).

---

## What this effort is

The pre-release source snapshot in `src/` has most of the UI scaffolding — `TPane`/`TScreen`, concrete panels (`TInventory`, `TStatPane`, `TStatusBar`, `TAutoMap`, `TDialogPane`, `TMultiCtrlPane`, etc.), font system, cursor. The retail behavior is captured in [recon/classes_converted/](../../recon/classes_converted/). What's broken:
- `playscreen.cpp` has `TODO(port)` markers around `GAMECOMMAND` dispatch and pane init lifecycle.
- OOG screens have DEF assets in the GOG install but no `TScreen` subclasses constructed from them yet.
- The pane framework itself is primitive (flat lists, immediate-mode-style draw) and benefits from structural evolution.

The job is to **evolve the existing classes in place** so the UI works on the new core — using the infrastructure that's already built (overlay/mount system, `rev_fopen`, `TDialogList`, `defdoc`, INI, `EditorPrefs`, etc.), and structurally upgrading `TPane`/`TScreen` toward a retained-mode hierarchy where it pays for itself.

## Position in the master port plan

This effort is part of **[../PORT_PLAN.md](../PORT_PLAN.md) Phase 4** (feature recovery — UI is one of the WIP-at-snapshot subsystems) **and Phase 5** (rolling presentability cleanup). The phasing inside this directory is local to the UI bring-up only; it slots inside the master plan, not alongside it.

## Hard constraints

1. **Evolve, don't replace.** Modernization happens by upgrading the existing classes (`TObjectInstance` is the canonical pattern). No parallel modules. The UI work touches `TPane`, `TButtonPane`, `TInventory`, `TStatPane`, `TPlayScreen`, etc. — by name. (Memory: `feedback-evolve-dont-replace`.) The one sanctioned structural shift is moving `TPane` toward a **retained-mode hierarchy**; class identity still carries forward. (Memory: `feedback-ui-retained-mode`.)

2. **Use existing infrastructure — don't recreate.** Almost every "new module" instinct is wrong here because the codebase already has the equivalent:
   | Need | Existing facility |
   |---|---|
   | Layered resource lookup with overlay | `rev_fopen()` + `MountArchive` / `MountModule` in [src/revutils.h](../../src/revutils.h); `rev_resolve_revisited_overlay()` already implements the lookup chain |
   | Resource bundles | `LoadResource()` / `LoadResourceHeader()` / `LoadResourceBinary()` in [src/resource.h](../../src/resource.h); `TMulti` in [src/multi.h](../../src/multi.h) |
   | DEF parsing | `defdoc::Document` in [src/defdoc.h](../../src/defdoc.h) |
   | Localized strings | `TDialogList` in [src/dialog.h](../../src/dialog.h); `Language` global in [src/revenant.h:177](../../src/revenant.h#L177); strings via `DialogList.GetLine("tag")` |
   | INI settings | INI API in [src/revutils.h:116](../../src/revutils.h#L116) |
   | Editor prefs (DEF-based) | `EditorPrefs` namespace in [src/editorprefs.h](../../src/editorprefs.h) |
   | Revisited toggle namespace | `Revisited::IsEnabled("Category.Flag")` design in [revisited/README.md](../../revisited/README.md) (TBD impl) |
   | Test mode harness | `src/testmodes.cpp` Initialize/Close/Render dispatch + `src/testscreen.cpp` input forwarding; `SpawnForTest()` + data-driven registry pattern (see [docs/vfx/TEST_MODE.md](../vfx/TEST_MODE.md)) |
   (Memory: `reference-revisited-overlay-system`.)

3. **GOG data is read-only.** Deviations land in the `revisited/resources/` overlay (preferred) or as runtime mutation of `defdoc::Document` at the consumer site (the established pattern — see `effect.cpp`, `editorprefs.cpp`). No edits under `data/`.

4. **Modes are toggles, not a global mode flag.** [revisited/README.md](../../revisited/README.md) documents the established model: `[Revisited]` section in `<SavePath>/Revenant.ini`; per-feature toggles default OFF (= vanilla). "Classic vs Revisited" is the aggregate state of those toggles, not a separate enum. **Boot default = vanilla (Classic-equivalent).**

5. **Main tree stays buildable + playable.** Other agents share this repo. (Memory: `feedback-keep-main-runnable`.) Invasive work uses a worktree branch.

## Where things live

| Path | Purpose |
|---|---|
| [ARCHITECTURE.md](ARCHITECTURE.md) | Target shape — which existing classes evolve and how |
| [PLAN.md](PLAN.md) | Phased bring-up + rationale |
| [BURNDOWN.md](BURNDOWN.md) | Live task list with status |
| [CONVENTIONS.md](CONVENTIONS.md) | UI-specific do's/don'ts on top of project-wide conventions |
| [briefs/](briefs/) | Self-contained sub-agent task packets |
| [../HUD.md](../HUD.md) | Retail HUD UX spec — what we're reproducing |
| [../UI.md](../UI.md) | Older internal UI architecture notes — partly superseded by [ARCHITECTURE.md](ARCHITECTURE.md); cross-reference, don't trust blindly |
| [../PORT_PLAN.md](../PORT_PLAN.md) | Master port plan; this effort is its Phase 4/5 UI slice |
| [../ARCHITECTURE.md](../ARCHITECTURE.md) | Overall engine architecture |
| [../ASSET_SYSTEM.md](../ASSET_SYSTEM.md) | Asset / resource / TMulti reference |
| [../FILE_FORMATS.md](../FILE_FORMATS.md) | I2D / I3D / sector / .DAT formats |
| [../../revisited/README.md](../../revisited/README.md) | Overlay convention (canonical) |

## How to pick this up cold

1. Read this file, then [ARCHITECTURE.md](ARCHITECTURE.md).
2. Open [BURNDOWN.md](BURNDOWN.md), find the next `[ ]` item under the current phase in [PLAN.md](PLAN.md).
3. If the item has a brief in [briefs/](briefs/), use it. Otherwise write one first (forces clarity, helps the next agent if you don't finish).
4. Implement under [CONVENTIONS.md](CONVENTIONS.md). Update [BURNDOWN.md](BURNDOWN.md) as you go.
5. Verify in a `--test=ui-…` mode if applicable; finish with `build-asan/Revenant` (memory: `reference-debug-tooling`).
