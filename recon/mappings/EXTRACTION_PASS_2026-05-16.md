# VFX recon extraction pass — 2026-05-16

Phase 0.3 follow-up. The mapping pass log in
[docs/vfx/INVENTORY.md](../../docs/vfx/INVENTORY.md) tracks per-row burndown;
this file documents the *methodology* used in the 2026-05-16 extraction pass
so a future agent (or sceptical reviewer) can re-run the analysis.

## Tools used

No new Ghidra runs. All work is over the existing
`recon/classes/cls_0x*.cpp` corpus produced by an earlier `OOAnalyzer` pass.

The pre-existing scripts in `recon/scripts/` (class_fingerprinter.py,
class_matcher.py, …) were not run for this pass. They fingerprint classes by
total size + field-type sequence which we judged too coarse for the effect
family: TEffect-derived classes have a near-identical 256-byte base layout
(see "Sibling layout" below), so size+gap fingerprints alone produce many
false positives. See the "Lessons" section for what is genuinely useful.

## What was actually run

Two ad-hoc Python passes (`/tmp/find_effect_classes*.py`, kept in scratch):

1. **Extract effect-name strings + their XREF callsites** from
   `recon/classes/_data.txt`. Source-of-truth pattern in the data file is:

   ```
   s_<Name>_<saddr>      XREF[n]:    <callsite> (*) , <callsite> (*) ...
       <saddr>  XX YY ZZ ds  "<Name>"
   ```

2. **Resolve each callsite to an owning `cls_0x*` Ghidra class.** Built a
   global sorted list of `// Function at <addr>` headers across every
   `recon/classes/cls_0x*.cpp` (excluding `*__vftable_*.cpp`). For a given
   callsite address, find the largest function-start ≤ callsite; only
   attribute the class if (a) the callsite delta is ≤ ~8 KB inside that
   function and (b) the next function-start is reasonably close (i.e. not
   in "free function" territory between class methods).

This eliminates the false-positive pattern where a callsite lands in the
*static section between* two class methods (Ghidra packs registry / factory
free-functions there and they get wrongly attributed to the previous class).

## Results — effect type-name registry @ .rdata 0x5e1xxx

The pre-shipped binary has an effect-name registry (likely
`RegisterObjectClass("Tornado", TTornadoEffect::CreateInstance)` etc.) whose
type-name strings cluster at `.rdata 005e1xxx`. Each string has 2 XREFs
near each other (the registrar function "GetClass" return + the
`RegisterObjectClass(...)` callsite, typically), and a clean attribution to
one class indicates that class IS that effect.

### High-confidence (both XREFs in one class, no neighbors)

| Effect name   | Recon class       | Bytes | Vtbl | Methods |
|---------------|-------------------|------:|-----:|--------:|
| `DragonFire`  | cls_0x5aacac      |   376 |   96 |       3 |
| `IceBolt`     | cls_0x5aaf28      |   340 |  100 |       6 |
| `Streamer`    | cls_0x5abbb0      |   392 |  108 |       5 |
| `FireSwarm`   | cls_0x5abe4c      |   392 |  104 |       6 |
| `Ripple`      | cls_0x5ac340      |   396 |  100 |       6 |
| `FaultFire`   | cls_0x5ac83c      |   400 |  104 |       6 |
| `Pixie`       | cls_0x5ad4e4      |   272 |  112 |       7 |

These all derive from `cls_0x5b0074` (candidate TEffect base) per their
destructor chain, and share an identical "TEffect base" field layout at
offsets 0x4, 0x8, 0xc, 0x14, 0x3c, 0x9c, 0xac, 0xec, 0xfc (see "Sibling
layout"). Mapped via candidate yamls — see `T*_cls_*_candidate.yaml` in
this directory.

### Conflicted (multiple effect names map to one class — class file
overlaps several effects' registrar regions)

| Effect names                                 | Recon class      |
|----------------------------------------------|------------------|
| `Tornado`, `Sandswirl`, `Quicksand`          | cls_0x5ab460     |
| `Blood`, `MistFog`, `WaterFall`              | cls_0x5acaa8     |
| `FireFlash`, `FireWind`                      | cls_0x5a9194     |

Diagnosis: these `cls_0x*` files are large (1000-2000 bytes, 14+ methods)
with method ranges spanning many KB. Either:
- Ghidra merged multiple distinct classes into one file (vtable shape collision), or
- The file contains one effect + N adjacent free-function registrars that
  Ghidra absorbed by virtue of address proximity.

Disambiguation needs Ghidra rerun with stricter scoping, OR manual review
of which methods in each big file are virtual overrides vs free functions.
**Not resolved in this pass.** Left as `not-extracted` in INVENTORY with a
note pointing to this file.

### Single-XREF (plausible but weaker)

| Effect name   | Recon class       | Source candidate                          |
|---------------|-------------------|-------------------------------------------|
| `Smoke`       | cls_0x5a7e38      | (no `TSmokeEffect` in source — registrar) |
| `FLAME`       | cls_0x5a9d88      | TFlameEffect or TFlameAnimator (loader)   |
| `SymGlow`     | cls_0x5a9e50      | TSymGlowAnimator                          |
| `IrisFlare`   | cls_0x5a9f2c      | TFlareAnimator (iris variant)             |
| `Ribbon`      | cls_0x5b9fac      | TRibbonAnimator                           |
| `Fizzle`      | cls_0x5ad758 / cls_0x5ad9d4 | (no source class — likely script-side) |

Not promoted to candidate yaml — single XREFs are too weak to commit on
their own, and many of these strings are loader keywords (uppercase
"FLAME"/"SHIELD"/"RIBBON") rather than RegisterObjectClass type names.

## Re-evaluation of the 3 pre-existing mappings (from the 2026-05-16 initial pass)

| Source class           | Existing Ghidra | Verdict after inspection                  |
|------------------------|-----------------|-------------------------------------------|
| `TFireBallAnimator`    | cls_0x41cdd0    | **Wrong.** cls_0x41cdd0 is a hash/LRU cache (uses `FUN_00482130`/`40` lock/unlock around every op, walks parallel arrays sized 0x1000/0x2000, has an LRU counter at `mbr_0x14`/`mbr_0x28`). No vtable. Original yaml claims "PERFECT" based purely on size (44 bytes) + field count (11 dwords) — neither uniquely identifies an animator. Demoted in INVENTORY. |
| `TLightningAnimator`   | cls_0x5a47f0    | **Suspect.** cls_0x5a47f0 has a vtable (good) but at only 60 bytes (Ghidra-inferred; allocator allocates 0x104=260 bytes) and the only class that allocates it (cls_0x5ac340 = Ripple candidate) uses it as a **generic animator** — strongly suggesting cls_0x5a47f0 is `T3DAnimator` (the BASE), not `TLightningAnimator` specifically. Demoted in INVENTORY. |
| `TLightSource`         | cls_0x5427f0    | **Suspect.** cls_0x5427f0 is 332 bytes with 3 fields; sole method calls into TCharacter::method_0x51b580 to attach a light index. This is a "light attached to character" helper, not the TLightSource data class. Demoted in INVENTORY. |

## Sibling layout — the "TEffect base" pattern

Every confirmed effect-class candidate (cls_0x5abbb0, 5abe4c, 5ac340, 5ac5b0,
5ac83c, 5acf9c, 5ad4e4, 5aaf28, 5aacac) has its base fields at:

```
ofs   sz   field
0x00  4    vftptr
0x04  4    dword  (likely TObjectInstance::base ptr or super-vtbl)
0x08  4    dword
0x0c  4    dword
0x14  4    dword
0x3c  1    byte    (a flag field — possibly "active" or "first_time")
0x9c  4    dword
0xac  4    dword
0xec  4    dword
0xfc  4    dword
0x100 4    dword
...           (per-derived-class fields beyond ~0x110)
```

So `TEffect` itself appears to occupy ~256-272 bytes (consistent with
inheriting a *slim* TObjectInstance variant). The "fat" 1352-byte
TObjectInstance in CLASS_MAPPING.md is a different (later-version) variant.
Per-derived class fields start around offset 0x110+ which gives each effect
12-30 bytes of own state — a tight match to what `src/effect.h` declares.

This base-layout discovery is reusable for any future effect-class mapping.

## Effect3.cpp + MissileEffect.cpp (assert-string fingerprints)

The only retail effect-family source files whose names appear as `d:\revenant\*.cpp`
assertion strings in the binary:

| Source file         | Containing class      | Notes                              |
|---------------------|-----------------------|------------------------------------|
| `Effect3.cpp`       | cls_0x5b0a28          | 1400-byte concrete class, 5 methods, inherits cls_0x5b8e94 (TObjectInstance). Method at 0x503110 throws the Effect3.cpp assert. The class also references "LightningStorm" string xref at 0x503520, suggesting it IS `TLightningAnimator` or a closely related storm class. **Candidate not yet committed** — needs source-side `TStormAnimator` vs `TMeteorStormAnimator` size verification. |
| `MissileEffect.cpp` | cls_0x5b4534          | Method at 0x512740 throws the MissileEffect.cpp assert (3 hits). cls_0x5b4534 is the home of methods covering `Photon`, `FireBall`, and `FIRECOLUMN` per the XREF table. Probably `TMissileEffect` (base) or one of TPhotonEffect / TFireBallEffect / TFireColumnEffect. **Candidate not yet committed.** |

## Lessons / notes for the next agent

1. **Don't fingerprint TEffect-derived classes by total size alone.** Their
   shared 256-byte base + small per-class delta means many will have
   indistinguishable size+field-count signatures. Use vtable shape +
   inheritance + effect-name string XREFs instead.

2. **The Ghidra "cls_0xNNNNNN" file boundary is not authoritative.** A single
   file frequently contains the class's methods *plus* adjacent free-function
   registrars that have no real class membership. When looking up "which
   class does callsite X belong to", check the gap between the enclosing
   function start and the next function start; if the gap is large
   relative to typical effect-class methods (a few hundred bytes), treat
   it as a free function instead.

3. **The effect-name registry at .rdata 005e1xxx is the best single
   evidence source for type-name → class mapping.** Two XREFs near each
   other in the same class = the registration callsite. Pair this with
   inheritance from cls_0x5b0074 (TEffect base) for stronger confidence.

4. **The pre-existing class_matches.json is file-size matching** of source
   `.cpp` vs decompiled `.cpp` file sizes — useful for huge classes
   (TObjectInstance, TCharacter) but worthless at the effect level because
   `src/effect.cpp` is one 400 KB file containing 50+ classes.

5. **Source-side fingerprinting prerequisite.** Before we can promote any
   of the MEDIUM candidates to HIGH and apply `rename_classes.py`, we
   need source-side fingerprints for `TEffect`, `T3DAnimator`, and
   `TObjectInstance` that resolve their full base sizes. The existing
   `class_fingerprinter.py` does this but its `_register_known_types`
   table is missing several embedded types (`hmm_vec3`, `RTInputStream`,
   `PSDropParticle`, …) — the script reports them as 'E' (unknown).
   Fix that table first.

## Files produced in this pass

- `recon/mappings/TStreamerEffect_cls_0x5abbb0_candidate.yaml`
- `recon/mappings/TFireSwarmEffect_cls_0x5abe4c_candidate.yaml`
- `recon/mappings/TRippleEffect_cls_0x5ac340_candidate.yaml`
- `recon/mappings/TFaultFireEffect_cls_0x5ac83c_candidate.yaml`
- `recon/mappings/TPixieEffect_cls_0x5ad4e4_candidate.yaml`
- `recon/mappings/TIceBoltEffect_cls_0x5aaf28_candidate.yaml`
- `recon/mappings/TPhotonEffect_cls_0x5aacac_candidate.yaml`  (probably misidentified — needs source-side audit; commented in yaml)
- `recon/mappings/EXTRACTION_PASS_2026-05-16.md`  (this file)
- INVENTORY updates in `docs/vfx/INVENTORY.md` (mapping pass log + per-row recon-file column).

No `recon/classes_readable/T*.cpp` produced — all yamls are
`_candidate.yaml` (MEDIUM confidence). `rename_classes.py` was not run.
