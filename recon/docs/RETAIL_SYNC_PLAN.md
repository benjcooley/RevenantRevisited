# Retail Sync Plan

Status: **active** — started 2026-04-17.

## Why

The pre-release source in `src/` has drifted from the retail binary. Symptoms:

- Retail save files (`data/Save/Single/New Game1/game.sav`) don't parse with our
  `TSaveGame::ReadGame` — format mismatch (no 32-slot DATA_SLOTS header where
  we expect it, retail embeds map name as a fixed-width string, etc.).
- Any class the retail object stream dispatches into is a potential divergence
  point. Late bug-fixes and feature work shipped in retail but aren't in our
  pre-release source.

Trying to bring the full retail boot chain up against pre-release classes will
hit divergence after divergence in undefined order. We need to synchronize core
classes back to retail first, then worry about retail save data.

## Strategy

Three phases, interleaved:

1. **Test harness.** `--test=<mode>` cmdline flag (argh already wired), a
   `TTestScreen : TScreen` that dispatches to named modes, routed ahead of
   `LogoScreen`/`PlayScreen` at boot. One test mode per synced subsystem.
2. **Sync core classes from Ghidra.** Diff `src/X.cpp` against
   `recon/classes_readable/X.cpp`, port retail semantics in, keep modern C++
   surface syntax (override/=default/nullptr/etc. conventions already
   established — see `memory/feedback_modern_cpp.md`). One class per commit.
3. **Validate via test mode.** Every sync lands with a corresponding test mode
   (or extends an existing one) that exercises the class. Regressions show up
   immediately.

## Target sequence

Roughly bottom-up; small classes first to shake out the workflow.

1. Test harness skeleton + `blank` mode.
2. **`TScreen` + `TPane`** — the substrate the harness itself runs on.
3. **Drawing primitives**, each with its own test mode:
   - `TSurface` → test: `blit` (lock/fill/rect)
   - `TBitmapData` / `TBitmapSurface` → test: `bitmap` (load+draw a .BM)
   - `TMultiSurface` → test: `multisurf`
   - `TFontData` → test: `font` (render a string)
4. **3D / imagery**:
   - `SOld3DImageryBody` / `T3DImageryRes` → test: `i3d` (static imagery)
   - `T3DScene` → test: `scene` (scene w/ one imagery, no animation)
   - `TCharAnimator` + `TCharacter` → test: `char3d`
5. **Object streaming layer**:
   - `TObjectInstance` / `TObjectClass`
   - `TInputStream` / `TOutputStream`
   - `TSaveGame` (retail format)
6. Full retail save load; back to driving PlayScreen.

## Method per class

1. Read `recon/classes_readable/<ClassName>.cpp` and `.h`.
2. Diff against our `src/<file>.cpp`/`.h` (names don't always match 1:1).
3. Port retail layout and semantics:
   - Member offsets / struct layout match retail exactly (required for
     serialization and inheritance to work).
   - Method signatures match retail unless trivially equivalent.
   - Keep our modern-C++ conventions on the surface (override, =default,
     nullptr, [[nodiscard]], const-correctness) — retail's decompiled style
     is not a style target.
4. Build. Run test mode. Commit.
5. Note any unresolved divergences in a short comment at the class declaration
   site (e.g. "// retail layout as of 5a5320; VFT slot 0x14 unmapped").

## Files and layout

- `recon/classes_readable/` — Ghidra decompilations, human-readable. Primary
  reference.
- `recon/classes/` / `recon/classes_converted/` — raw Ghidra output (address-
  keyed). Fall back to these for classes not yet in `classes_readable`.
- `recon/docs/CLASS_MAPPING.md` — initial size-based mapping between
  `Classes/cls_0x*` and `src/*`. Partially stale but useful for address lookup.
- `recon/docs/GHIDRA_CLASS_IDENTIFICATIONS.md` — Ghidra class identifications
  with base-class sizes (TObjectInstance=112, TScreen=276, etc.).

## Cmdline shape

```
Revenant                         # retail boot path (currently broken; deferred)
Revenant --test=<mode>           # test harness
Revenant --loadmap=<path>        # load a save file (deferred until retail save
                                 # format is implemented)
```

Flags all parsed via `thirdparty/argh/argh.h`, case-insensitive. Flag plumbing
lives in `src/revmain.cpp::GetParameters`.

## Deferred / parked

- Retail save loading — needs whole streaming subsystem first.
- Full retail boot via `LogoScreen` → `PlayScreen` — same reason.
- `rev_fopen` data-root fallback (done 2026-04-17) stays, it's a real fix.
