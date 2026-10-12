# Sandswirl: evidenced legacy Sand restoration

This is an explicitly modified-retail configuration, not shipped-enabled Sand.
The current/CD `class.def` has Sandswirl but no `sand` child type; the original
missing-type/null-child negative remains preserved. No Sparks resource alias
is introduced, and the normal unopted source parent still fails closed.

## Positive provenance

The supplied clean legacy repository at
`83f806bb65b762b2cc3e1ea509010f05342caa12` contains the exact definition
`"Sand" "Magic\Sandswirl.i3d" 0xfecaface` in `Class.Def:2060`,
`old.old.Class.Def:1939`, `cbClass.Def:2027`, `jared.def:1943`, and
`class backup.def:2084`. `walkcode/effect.cpp:4453` actively registers `sand`
with `TParticle3DAnimator`; the corresponding registration in the other
legacy `effect.cpp:4484` is inside a block comment. The macro's factory is
the ordinary particle animator, with no special Sand simulation.

The 8,788-byte legacy asset is byte-identical to the installed and retail-CD
`Imagery/Magic/sandswirl.i3d`, SHA256
`4ffcc6ab4e0f519bbfbfebc4713691eb92d14b792f27222130c35ce5502af85f`.
It contains one `photon01` quad, two triangles, one constant frame, no tags,
and one 64x64 ARGB4444 texture. Sand therefore has one particle variant;
Sparks has four and legitimately consumes a different initialization stream.
The restored reference does not reuse the earlier Sparks pool or imagery.

`sandswirl_legacy_binding_audit.py` verifies all five definitions, active versus
commented registrations, archive negatives and all three identical assets.
Its private manifest SHA is
`b49c4defb01e3faf4ad951a2cd9fa0317bba82ad11a4b4f0207bd5a4ed346a1c`.

## Executed restoration and source adapter

The reference calls actual `TObjectClass::AddType(4746d0)` to append Sand
(type176, ID `fecaface`), then actual builder registration `40db90` creates a
separate `sand` builder whose virtual Build is original `4f5eb0`, returning
`TParticle3DAnimator`. It leaves the existing Sparks builder and asset alone.
The original Sandswirl Init literal `sand` at `4ede49` is unchanged. There is
no executable-file patch; the class/builder additions are explicit VM state.

Source `SANDSWIRL_MAP_RESTORED_SAND` restores the same entry through actual
`AddType` only when explicitly selected, without writing the registry file.
`UseRestoredLegacySand` is a per-instance admission; exact ID, one-object
geometry, state/frame count, indices and ARGB4444 format are checked. One
fixed mesh key is reference-counted and released. The previous qualified
Sparks route remains separate. Neither route is silently mapped as natural
shipped Sand behavior.

The same real map, two real Acolytes, real named Quicksand spell/public
SetSpell control, NewObject/GetInstance, live target query, particle component,
and spawned null-spell Quicksand operate as in the preceding real-map proof.
There is no named Sandswirl variant in the loaded list. The original public
Quicksand initializer is `ret 8`; no target binding or fake damage is added.
The reference explicitly makes map IDs monotonic and suppresses only the
original allocator's RNG side effects, matching the source allocator policy.
No effect-step or tick75 RNG injection occurs in the source or core proof.
Original stack-derived seekz is `236977916`; source uses declared nonzero41.
Only their Boolean nonzero semantics are claimed.

## Final evidence and limits

Evidence root: `/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra`.

- `sandswirl-restored-sand-native/manifest.json`: full310 original owner,
  controller and software-render calls; SHA
  `c7907a53c9641b9021545a318e51dac3e51d5dcf8b6517078d1d882e9363237e`.
- `sandswirl-restored-sand-core-final/manifest.json`: 283,030 independent
  pool/RNG fields and 6,042 ordered packets bit-exact, one-object RNG,
  no allocator seed injection; SHA
  `c1c1a868e4d31416caeafdf75b400991656a5740eec72a980c555dd95c723044`.
- `sandswirl-restored-sand-metal-final/manifest.json`: all310 actual GPU
  frames and pool/packet traces, clean exit; SHA
  `5230c5578ed49694f58a69df7c62305536dfecdec322880758c3353313fc0e6a`.
- `sandswirl-restored-sand-review-final/manifest.json`: 282,100 actual runtime
  pool fields, 6,042 packets, 310 phase/RNG/spell rows and 236 Quicksand
  lifetime rows exact; eighteen full-frame and fixed-ROI pairs; SHA
  `cd91b6abf126e62e32688df536385f8348ff503e230e12b96706047b0e59b1db`.
- Binary SHA256:
  `d101d17cda62fc7588fd0687df04844c1d431f26dc8eae72b5f5201367ae72bb`.

The native reference is visible through94, blank95, particle-killed96;
Quicksand spawns75, first pulses76 and kills131. Parent kills300 and completes
the real spell timer; both actors retain health102, flags and positions.
The primary fixture remains 1280x960, source3185/3185/16, target+96X,
world camera3185/3185/0, source ambient38/directional1, 24Hz, seed1.
Every requested phase is mapped through simulation logs, not image fitting.

The first GPU capture retained fractional source contributions. The final
restored/native-domain route alone enables existing
`retail_alpha_source_quantize`: all24 authored texture words plus four
negative controls pass56 original Scene2 draws with incoming RGB5 zero/31.
This performs literal RGB565 source-channel floors, not a tint/threshold.
Alpha-zero white remains nonzero. Literal proof SHA:
`9f1546e84bfca72d067a28783c16c149050f693227ccb362d9092607810cc9e0`.
All310 common-policy images remain byte-identical before/after; the earlier
qualified-Sparks310 images also remain byte-identical after restoration.
The untraced quantized film and its rejected review are retained separately;
the final repeat includes all28,210 pool rows and completes cleanly.

Residual destination rounding, approximately one-pixel sampling/edge
variation, common-world projection, natural Sandswirl spell/actor scheduling,
stack initialization, audio and device behavior remain open. No exact-pixel
or unmodified-shipped appearance claim is made. Root owns review/approval.

Root independent integrated reproduction passed under `research/vfx-sandswirl-root-20261011/restored-sand-integrated/`: all310native records/images pinned,283030 compiled core fields/6042packets, resourceguards/literal56controls, current310tracedGPUimages byteidenticaltofinalcandidate and cleanexit, full282100runtimepoolfields/6042packets/236Quicksandrows. Root reviewedall18whole/fixedphases and grantsqualifiedappearance credit for thisexplicitrestoration only. All limits above remain open.
