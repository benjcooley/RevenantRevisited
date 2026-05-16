# UI Conventions — Do's and Don'ts

Read [README.md](README.md) for context. These are the project-wide conventions ([../../CLAUDE.md](../../CLAUDE.md), memory, [../PORT_PLAN.md](../PORT_PLAN.md)) applied specifically to UI work.

## Do

- **Evolve existing classes; don't introduce parallel modules.** `TObjectInstance` is the canonical evolution pattern (memory: `feedback-evolve-dont-replace`). For the UI specifically, the sanctioned structural shift is moving `TPane` toward a retained-mode hierarchy — class identity (`TPane`, `TButtonPane`, etc.) carries forward (memory: `feedback-ui-retained-mode`).
- **Use existing infrastructure** rather than recreating it. See the table in [README.md](README.md#hard-constraints) — every facility we need (overlay, mount, DEF parse, localized strings, INI, EditorPrefs, font path, test harness) already exists.
- **Hold T/S/P prefix and PascalCase methods** per [../PORT_PLAN.md §2](../PORT_PLAN.md#2-style-conventions-enforce-for-newtouched-code).
- **Store sokol handles (`sg_image`, `sg_pipeline`, …) directly as members** — no wrapper, no smart pointer, no adapter (memory and PORT_PLAN). Matches `TSurface`.
- **For new code / rewritten subsystems**, prefer RAII / value semantics / `std::unique_ptr`. For untouched files, don't retrofit smart pointers.
- **Route all UI strings through `DialogList.GetLine("tag")`** (existing `TDialogList`). No hardcoded English in pane code, button labels, or anything player-visible. If a tag doesn't exist yet, add it to the appropriate `LANGUAGE.DEF` (English source of truth is in the GOG install; extensions ship as a `revisited/resources/.../english.def` overlay file per `feedback-evolve-dont-replace` and `project-distribution-model`).
- **For Revisited-only behaviors, gate on `Revisited::IsEnabled("Category.Flag")`** per [revisited/README.md](../../revisited/README.md). Toggles live in `[Revisited]` section of `<SavePath>/Revenant.ini`. Default off (= vanilla).
- **Verify at both resolutions** before marking any UI pane done — vanilla 640×480 (must match retail pixel-faithful) and Revisited 1920×1080 (anchor-positioned).
- **For settings, use the existing INI API** in [src/revutils.h:116](../../src/revutils.h#L116). For internal pref-like state (UI-side, not user-facing), follow the `EditorPrefs` `.def`-via-`defdoc` model in [src/editorprefs.h](../../src/editorprefs.h).
- **Use `TSafeRef<T>` for object references** held by panes (memory: `project-object-ref`). Hold mapindex, not raw `TObjectInstance*`.
- **Use `kPrimaryMod` for keyboard chords** — Cmd on macOS, Ctrl elsewhere (memory: `project-imgui-mac-modifiers`).
- **Log with bracketed subsystem tags via the logging facade** — `log_info("[ui] inventory pane initialized")` (memory: `project-logging`). Tags: `[ui]`, `[hud]`, `[oog]`, `[revisited]`, `[loc]` as appropriate.
- **Default to a `--test=<mode>` example** for every non-trivial UI subsystem (memory: `feedback-test-modes-for-ui`). Three categories — subsystem / game panel mockup / game screen mockup. Follow the established pattern in [src/testmodes.cpp](../../src/testmodes.cpp): add cases to `Initialize`/`Close`/`Render` dispatch; implementation in `src/<name>test.{h,cpp}`; data-driven registry where applicable (see [src/vfxtest.cpp](../../src/vfxtest.cpp)).
- **Keep the main tree buildable and runnable.** Every commit on main must keep `cmake --build build` clean AND `./build/Revenant` launching and playing. (Memory: `feedback-keep-main-runnable`.)
- **Use a git worktree branch for invasive changes.** A.2 (TPane retained-mode evolution) is the standout — touches the base class every screen uses. Spawn sub-agents with `isolation: "worktree"` for non-trivial work.
- **For final pre-handoff validation, use the ASan build.** `cmake -S . -B build-asan -DREVENANT_ASAN=ON`; `cmake --build build-asan -j`; `./build-asan/Revenant [--test=<mode>]`. Grep stderr for `Sanitizer` / `ASAN` / `runtime error`. (Memory: `reference-debug-tooling`.)
- **Apply Tier-1 modern C++ to files you touch** — `override`, `=default`, `constexpr`, `nullptr`, `[[nodiscard]]`, default-initialized members, `const` where appropriate. Drop `PT*`/`PS*` typedefs. Trim umbrella includes. **Bundled with work in flight**, not as a sweep (memory: `feedback-code-style`).

## Don't

- **Don't introduce parallel new modules** for things the codebase already does. If you find yourself proposing `src/rvroverlay.cpp`, `src/uiloader.cpp`, `src/loc.cpp`, `src/defpatch.cpp` — stop. Those concerns have owners.
- **Don't edit any file under the GOG install** (`data/Resources/`, `data/resources_unzipped/`, etc.). Read-only. Deviations live in `revisited/resources/` overlay or as runtime mutation at the consumer site. (Memory: `project-distribution-model`.)
- **Don't redesign player-visible UI** for "looking better." Modernization is structural / perf / specifically requested features only. (Memory: `feedback-modernization-stance`.)
- **Don't add per-feature mode flags outside `[Revisited]`.** Toggles live in one namespace per [revisited/README.md](../../revisited/README.md). Don't invent CLI flags or env vars for individual features.
- **Don't collapse "Classic" and "640×480" into one concept** in code. The Revisited toggle namespace is the single source; resolution is one toggle, anchored layout is another, etc. (Memory: `project-resolution-modes`.)
- **Don't bypass `TRenderer` to draw directly.** UI panes hold data; the renderer owns submission. (Memory: `feedback-centralized-rendering`, `project-renderer-layering`.)
- **Don't spawn raw `std::thread`** for UI background work. Use the worker abstractions (memory: `feedback-threading`).
- **Don't bodge.** No "make it work for now." Land code where it belongs (memory: `feedback-no-bodges`).
- **Don't delete legacy UI code we plan to revive.** Move to `attic/` per [../PORT_PLAN.md §2](../PORT_PLAN.md#2-style-conventions-enforce-for-newtouched-code) or wrap in `#ifdef UNUSED` (memory: `feedback-preserve-old-code`).
- **Don't trust the `TScreen` Ghidra labels.** They fail sanity checks (memory: `project-tscreen-unconfirmed`). Verify via decomp source-path fingerprints (memory: `reference-revsync-convention`).
- **Don't refactor on a sweep.** Cleanup bundles into work in flight only. (memory: `feedback-code-style`.)
- **Don't introduce removed macros.** `_CLASSDEF`/`_STRUCTDEF`/`_PTRDEF`/`min`/`max` are flagged for retirement; leave alone unless already in the file (PORT_PLAN).
- **Don't hardcode pixel widths sized to English strings.** Translated text varies (German +30%, CJK width-different, RTL eventually). Size content-driven or with explicit ellipsis policy.
- **Don't add anchor metadata in C++ code.** Anchors are data — in DEF anchor blocks shipped through the overlay.
- **Don't push WIP commits to main that break the build.** Use a worktree, or sequence the work so each commit is independently buildable.
- **Don't write multi-paragraph comments** or docstrings. One short line max if anything. CLAUDE.md rule applies in UI files too.

## When in doubt

- About to add a new module? → Stop. Find the existing class that owns this concern.
- About to edit retail data? → Stop. Use overlay or runtime mutation at consumer.
- About to add a CLI flag for a Revisited feature? → Stop. Use `Revisited::IsEnabled("...")` and a key under `[Revisited]`.
- About to hardcode a string in a pane? → Stop. Use `DialogList.GetLine("tag")`.
- About to start a sub-task without a brief? → Write one in [briefs/](briefs/) first.
- About to make a multi-file invasive change in main? → Move to a worktree.

## Sub-agent brief template

When delegating, drop a file in [briefs/](briefs/) following this skeleton:

```markdown
# Brief: <short title> (burndown item <id>)

## Goal
One paragraph. What "done" looks like.

## Context
- Phase: <A/B/C/D> per [PLAN.md](../PLAN.md)
- Depends on: <other burndown items>
- Retail reference: <recon/ files relevant>
- Existing classes to evolve: <TPane, TInventory, ...> at <file:line>
- Existing facilities to use: <rev_fopen, TDialogList, INI API, defdoc, ...>

## Constraints (non-negotiable)
- Evolve existing classes; no parallel new modules.
- GOG data is read-only. Deviations via overlay or consumer-site mutation.
- Render via TRenderer. UI strings via DialogList.GetLine. Toggles via Revisited::IsEnabled.
- Main tree stays buildable + playable; invasive work in a worktree.

## Scope
- In scope: <bulleted>
- Out of scope: <bulleted — the temptations to refuse>

## Definition of done
- <Concrete, externally verifiable checks.>
- BURNDOWN.md updated.
- Test mode (if applicable) passes; verified in build-asan.
```
