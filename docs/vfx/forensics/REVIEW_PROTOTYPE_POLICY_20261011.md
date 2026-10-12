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
