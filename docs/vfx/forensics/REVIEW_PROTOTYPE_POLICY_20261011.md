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
