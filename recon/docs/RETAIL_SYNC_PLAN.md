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

## Rendering pivot (2026-04-17)

The port is **not** rebuilding retail's shipped renderer. Instead we're
reviving the pre-release "dls" (Dynamic Lighting System) vision —
per-pixel depthmap-lit tiles — as GPU pixel shaders. See
`memory/project_rendering_direction.md`.

Consequences for retail sync:

- **No retail sync** for: `dls.cpp` (design spec → shader), most of
  `surface.cpp`/`bitmap.cpp` CPU lighting/blit machinery, `effect*.cpp`,
  `stripeffect.cpp`, `missileeffect.cpp`, `mosaicsurf.cpp`,
  `multisurface.cpp`, `colortable.cpp`, 16↔32 conversion code, and the
  retail "triangle-grid overlap" fallback wherever it's tangled into
  3dscene / imagery / mappane.
- **TSurface** survives only as a CPU UI bitmap (panels, HUD, textbar,
  cursor, inventory, mappane overlay) → uploaded as texture → drawn as a
  UI-shader quad. Don't retail-sync it as a drawing engine.
- **i3d / imagery / bitmap parsers DO need retail sync** so the texels +
  depth + (optional) normal data we upload as GPU textures are correct.

## Target sequence

**Guiding principle:** none of the higher-level game code (game logic,
scripts, AI, full PlayScreen boot, saves, multiplayer) will work until
(a) the core asset files load correctly against retail's on-disk formats
and (b) the core rendering/drawing path can actually put pixels from
those loaders on screen. Until both are proven, any bug further up could
just as easily be a data-layout mismatch or an unimplemented draw as a
real logic bug, and debugging the upper layers is a waste of time.

So we build bottom-up from the asset layer and the UI-quad draw path,
one class at a time, each with a visual `TTestScreen` mode that proves
a real retail file renders correctly. Only after that foundation holds
do we start object streaming, save-load, scene/AI, etc.

Streaming / save-load moves *after* the core asset classes are proven,
because saves are just serialized runtime objects composed of those
classes; if the underlying class layout is wrong, save load will look like
it works up to the first drift point and then corrupt silently.

1. Test harness skeleton + `blank` mode. ✅
2. **`TScreen` + `TPane`** — modernized pre-release, retail labels
   unconfirmed, no serialization → layout drift is cosmetic. ✅ (2026-04-17)
3. **Core asset classes (data-compat pass).** Each class: port retail
   parse/layout, verify against a real retail file in `data/`, prove with
   a visual `TTestScreen` mode. Render side stays CPU-bitmap (uploaded
   as a sokol texture, drawn as a UI quad) — the shipped retail renderer
   is NOT being resurrected. Order:
   - **`TFontData`** — font files in `data/Imagery/Interface/*.fnt` or
     wherever retail ships them. Test mode: `font` — load a known font,
     render the printable ASCII range into a CPU bitmap, upload, blit.
     Readable ref: `recon/classes_readable/TFontData.{h,cpp}`.
   - **`TBitmapData`** — `.rvi` bitmap resources. Test mode: `rvi` —
     load an inventory icon or cursor bitmap, upload, blit at 1:1.
     Readable refs: `recon/classes_readable/TBitmapData.{h,cpp}`,
     `TBitmapSurface.{h,cpp}`.
   - **`T3DImageryRes`** / `SOld3DImageryBody` — `.i3d` static imagery
     (tiles, props). Test mode: `i3d` — load one imagery, show the
     texels + visualize the depthmap (e.g. grayscale preview alongside).
     Readable refs: `recon/classes_readable/` — not yet extracted;
     start from `recon/classes/cls_0x5a35ac.cpp` (candidate T3DImagery,
     src-path `3DImage.cpp` x11).
   - **`TSurface`** — the data-compat scope here is *only* the CPU
     bitmap target: allocate → receive pixels from a parser → upload as
     `sg_image` → draw as UI quad. No retail blit/lock/mosaic machinery.
     Test mode: any of the above demonstrates this path.
   - **`TMultiData`** (multi-frame bitmap sets) — test mode: `multi`.
     Optional; defer until a test mode above needs multi-frame data.
4. **Object streaming layer** (retail save compatibility). Deferred to
   after step 3 — needs the core classes locked down first.
   - `TInputStream` / `TOutputStream`
   - `TObjectInstance` / `TObjectClass`
   - `TSaveGame` (retail format)
   - Test mode: `savedump` — read `data/Save/Single/New Game1/game.sav`,
     walk the stream, log each object class + field. No display needed.
5. **3D scene + characters** (retail sync for AI/animation/transform
   timing; render side is new shader pipeline):
   - `T3DScene`
   - `TCharAnimator` + `TCharacter`
6. **Game logic** — spell/script/command/inventory (behavior, not
   pixels); UIs rewire to shader-quad panels.
7. Full retail save load; back to driving PlayScreen.

## When to graduate to the main game path

Don't stay in retail-sync forever and don't bang on the main game path
prematurely. The handoff:

- **Stay in retail-sync** while any core asset class (step 3) still
  fails to round-trip real retail data through a `TTestScreen` mode.
- **Move to the main game path** once the font / bitmap / 3D-imagery
  loaders all render a retail file correctly via the CPU-bitmap →
  sokol UI-quad pipeline, and the object streaming layer (step 4)
  can walk a real save file without garbage values. At that point
  higher-up bugs (AI, scripts, scene logic) are likely real bugs,
  not data misreads, and debugging PlayScreen is no longer premature.
- **In-between (most common case):** when we hit something in the main
  game path that depends on a core class we haven't synced yet,
  prefer to go sync that class *now* (step 3-style: port, test mode,
  commit) over stubbing around it. Drop back out to the main path
  once the class is green.

## Dropped from plan (replaced by shader rewrite)

The *rendering* side of these classes is replaced by sokol_gfx pipelines
and pixel shaders. The *parsing / data-layout* side of the same classes
is still in the plan — see step 3 above. Don't confuse "retail render
machinery dropped" with "retail file format dropped" — the formats stay.

- `TSurface` blit / lock / mosaic / software-lighting paths — dropped.
  (CPU-bitmap scratch surface survives, see step 3.)
- `TBitmapSurface` retail render path — dropped. (Load-side covered by
  `TBitmapData` in step 3.)
- `TMultiSurface` retail render path — dropped. (Load-side covered by
  `TMultiData` if/when we need it.)
- `TFontData` retail render path — dropped. (Font-file parsing covered
  in step 3; rendering becomes a texture atlas + UI shader.)

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
- `recon/analysis/class_index.tsv` — grep-friendly fingerprint index (strings,
  API calls, numeric literals, method signatures). Rebuild via
  `python3 recon/scripts/build_class_index.py`. See "Class ID recipe" below.
- `recon/analysis/ghidra_fingerprints.txt` — size/field/byte-pattern
  fingerprints, sorted by pattern length.

## Class ID recipe

Before porting a class, confirm (or find) its Ghidra address. Check in this
order; stop at the first hit:

1. **Confirmed yaml mapping** —
   `ls recon/mappings/ | grep -i "^<ClassName>_"`
2. **Human-named readable decomp** —
   `ls recon/classes_readable/ | grep -i "^<ClassName>\."`
3. **Unique log string** (a `Status`/error/format literal the class emits) —
   `awk -F'\t' '$2=="str" && $4~/<slug>/' recon/analysis/class_index.tsv`
   Slug rule: Ghidra replaces `'`, `.`, spaces, etc. with `_`, so grep for
   `Can_t_clear` not `Can't clear`.
4. **API-call signature** (e.g. only the font subsystem calls `CreateFontA`) —
   `awk -F'\t' '$2=="api" && $4=="<Win32Fn>"' recon/analysis/class_index.tsv`
5. **Struct shape** — `grep ' <N>b ' recon/analysis/ghidra_fingerprints.txt`
   where `<N>` is the header's `sizeof`. Combine with method count from
   `class_index.tsv` `sum` rows to disambiguate collisions.
6. **Numeric-literal density** — matrix/3D code has many `0x3f800000` (1.0f):
   `awk -F'\t' '$2=="num" && $4=="0x3f800000" && $3>=5' recon/analysis/class_index.tsv`
7. **Method signature shape** — e.g. void taking `(T*, T*, i32, i32, i32, i32)`:
   `awk -F'\t' '$2=="meth" && $4~/:v\(p,p,d,d,d,d\):/' recon/analysis/class_index.tsv`

Signature codes in `class_index.tsv`: `v`=void `d`=dword/int `p`=pointer
`b`=byte/bool `w`=word `q`=qword/double `f`=float `s`=struct/cls_. Return type
is the first code (e.g. `d(d,p)` = `int meth(int, T*)`). Caveat: Ghidra often
lowers float args to `d` (FPU stack), so don't rely on `f` in sigs — use 1.0f
density instead.

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
