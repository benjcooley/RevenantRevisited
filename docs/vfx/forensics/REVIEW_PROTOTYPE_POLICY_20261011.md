# Review-list exclusions and invisible Dust

The normal scrolling reviewer omits `Essencedrain` / `Magic/Worm.i3d`
(`0xdefeca7e`). It retains the working **Essence Drain (spell)** station, which
uses the shipped ManaDrain spell variant. Select `--vfx-review-effects=0xdefeca7e`
to inspect the separate **Worm (static prototype)** diagnostic.

The Worm exclusion is supported by these bounded shipped-inventory findings:

- No exact type-ID or name-pointer literal caller in the executable text section.
- Across 73 archived script/definition/text files, its only matching line is
  its class declaration; no spawn or attachment reference was found.
- No serialized placements in 5,307 sectors across all seven shipped modules;
  zero decoder errors.
- The genuine named Essence Drain spell selects ManaDrain instead.
- Full original default-owner rendering confirms the Worm cylinder keys reject
  after the first phase and its remaining auxiliary meshes stay static.

This supports excluding an unused prototype from normal review. It does not
prove that no computed/dynamic caller could ever reach it. The asset, runtime,
coverage row and existing scoped evidence are retained. Other entries are not
excluded merely because they are static, incomplete or invisible.

`cfire` (`0xad92bd27`, `Misc/cfire.i3d`) is also omitted from normal review.
Its only two shipped attachment commands, in `char.def` lines6886–6887, are
commented out. Its other text match is the class declaration. The two required
presets, `cfire.txt` and `csparks.txt`, are absent from all nine resource/module
archives. No exact literal executable caller or placement in the same5,307
sectors was found. Original full initialization fails both file opens and
constructs no particle controllers, leaving two base sprites.

Select `--vfx-review-effects=0xad92bd27` to inspect the installed fallback,
labeled **cfire (unused prototype)**. Its existing scoped proof is preserved;
no missing presets are invented. Computed/dynamic or externally supplied
callers remain unproven. Private evidence is in
`research/vfx-cfire-caller-root-20261011/` under the same retail lab:

- `caller.json`: SHA256 `459fc18bee5af17e3462005da50d807171b414b390207bc52a7b6156b9e5298d`.
- `placements.json`: SHA256 `361860e7efa99e4aef8d7205a9be7a838178af1ec99709d3132462e95b8c4c29`.
- `presets.json`: SHA256 `c08f3d66182dbcc4029c0fa822fc6c83f020cadf3877c079e5e00da5a7139939`.

Two further exact types are omitted on the same bounded inventory basis:

- **Vortex** `0x452dade0`: its procedural animator and registration occur
  inside the commented `Ye Old Vortex` block in the source snapshot. All93
  genuine retail registrations resolve this type to the default animator,
  with no controllers and a static one-frame cone. The shipped `VortexM`
  spell selects **Teleporter**, not this type. The archive scan finds only
  the class declaration, no literal executable caller and no map placement.
  Diagnostic label: **Vortex (unused static prototype)**.
- **Fire** `0x119f01e3`: genuine default-owner execution stays at frame0 and
  texture0, with no controller; archived animated `TFireEffect` remains
  a separate unverified implementation. The archive scan finds its class
  declaration and an unrelated spell-description sentence, no spawn/attachment
  command, literal executable caller or map placement. Diagnostic label:
  **Fire (unused retail default)**. The animated legacy diagnostic receives
  no appearance credit; Fire's reopened coverage status remains unchanged.

Both scans cover all73 archived text files and5,307 sectors across seven
modules with zero decoder errors. Named Flame, Teleporter, setvortex and
gvortex stay in normal review. No similarly named type is substituted for an
excluded exact ID. Computed/dynamic/external callers remain unproven.

Private evidence: `research/vfx-prototype-caller-root-20261011/`:

- `vortex-caller.json`: SHA256 `d1d8e5abb0170403000dbe0906eaaeb3af9e2190f0b798c807808963e739304c`.
- `vortex-placements.json`: SHA256 `acd4da3436d41003bca08651de7e83752a7c6e2bdc1731c54dc5ea22161659e0`.
- `fire-caller.json`: SHA256 `f9b984731c50c94153a730ecaeb031fdb0f88775f1132cd31b95fec7d17d3baa`.
- `fire-placements.json`: SHA256 `22b7bb01a57033ebbc21b6a81140208ef18cf80d53472ffba1108ac1681949ba`.

The existing [Vortex binding audit](VORTEX_GALLERY_CONTEXT_20261011.md) and
[Fire cardinal/caller audit](FIRE_DUST_CALLER_AUDIT_20261011.md) retain the
original execution and device limitations. All four prototype diagnostics
remain selectable together:

```sh
./build-merge/Revenant --test=vfx-review --vfx-review-first \
  --vfx-review-effects=0xdefeca7e,0xad92bd27,0x452dade0,0x119f01e3
```

Private read-only evidence is in
`/Users/benjamincooley/RevenantRetailLab/research/vfx-full-visible-review-20261011/`:
`worm-caller-audit-v2.json` (SHA256
`2636ffec1a002ea747a35f96efc6dc8741a34e17c9e8b79ef2e5da83da75ae49`)
and `worm-all-module-placements.json` (SHA256
`96ad75a589f612bbe194343c6d31fe5bc1406ebb8d36b270964ac9bca9ce33fc`).
The audit command is:

```sh
PYTHONPATH=tools/retail_runtime:tools/retail_asm "$PY" \
  tools/retail_runtime/fire_dust_caller_audit.py \
  --name Essencedrain --name Worm --type-id 0xdefeca7e --output "$OUT"
```

**Dust stays in normal review.** Its genuine software RGB565 additive colors
round down to zero, so raising ambient lighting or changing the black background
does not restore the discarded contribution. The qualified hardware-style
RGB8 path produces visible dust; it is not evidence that the effect is abandoned.
Inspect that separate policy with:

```sh
./build-merge/Revenant --test=vfx-review --vfx-review-first \
  --vfx-review-effects=Dust --vfx-lighting-mode=modern
```

The qualified device scope and software blank control remain documented in
[DUST_QUALIFIED_DEVICE_20261011.md](DUST_QUALIFIED_DEVICE_20261011.md).

Actual compiled-runtime selection checks pass: the default list has173 slots
(176 catalogue types minus four diagnostics plus the named Essence Drain spell).
Explicit selection of all four diagnostics yields four slots and clean shutdown.
Both two-frame runs used a frozen binary. `selection-check.json` SHA256:
`f422c7133e4a40c325368b2d56d61f921206d143444c7291d205a17d31889963`. This is selection/liveness evidence, not new visual acceptance.
