# Current RGB565 helper appearance recheck

Fresh actual Metal map captures recheck Immortalmight, Shadowfist, Warriorborn,
teleportation and goldeffect after the shared source-quantization changes in
70c0aa3. This is a review of their existing scoped appearances, not five new
full-effect acceptances. No renderer, effect parameters or projection policy
was changed for this batch.

The capture binary SHA256 is
`c84b20b2aa71efc9eaa9d71f0b81b4cd274043c17c3a40f0d3998ce6ac3e51e7`,
from the f0557fe branch with 70c0aa3 included. The original fixture command lines,
command files and data roots were reused. Each process had a private save root.
FontRuntimeLab camera is `(110,10000,10000,16)`, ambient32 white, seed1, warmup1,
30Hz capture with 60Hz simulation substeps. Logs confirm scene ambient32 white
and the actual helper source light ambient0.14902 / directional1 / no point
lights. Immortalmight explicitly uses quality1 and incoming blend16;
goldeffect's log confirms quality0. All other options remain as in the retained
fixture. This batch does not opt into the isolated native owner/projector mode.

All 1,770 frames and 47 command rows pass. Every capture exits normally. Each
has 50 final frames exactly equal to its own restored floor: 250 exact cleanup
frames total. The four persistent effects execute the original move/delete
sequence; rendered original and moved images differ and their bounds are
recorded. Gold follows natural destruction. Both native and current images have
zero background error outside the observation region against their own floor.

Private evidence directory:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/accepted-helper-recheck/`.

The root `manifest.json` SHA256 is
`70994cf05de4bcd4b1fb3b889e84297ddf30425c80bed32cc63d93438f52ab81`.
Each case directory (`Immortalmight`, `Shadowfist`, `Warriorborn`, `teleportation`,
`goldeffect`) contains the fresh raw frames, pinned capture manifest, command
file, timing/log files, `review.json`, `retail-current.png` and
`prior-current.png`. Preparation scripts are retained beside the manifest.
Native reference AVI and manifest hashes are checked before comparison.

The four looping comparisons use the existing fixed two-second port warmup and
240-frame native excerpt. Six samples are selected at frames
0,48,96,143,191,239. Gold uses the retained natural-onset excerpt without warmup,
with samples0,6,12,24,42,60. All contacts use the same unmodified crop and nearest
2x presentation. No phase search, RNG synchronization, color adjustment,
position/size fitting or floor replacement is performed.

Visual inspection retains the gold star/sparks of Immortalmight, purple star of
Shadowfist, rotating gold fan of Warriorborn, blue-white portal of teleportation,
and gold coin burst/star of goldeffect. Teleportation remains brighter with
rougher bright edges than the original. Gold retains trajectory/timing
residuals against the independent native excerpt. These are scoped appearance
observations for the independent root review, not strict pixel parity.

The historical port controls span other intervening source changes. The
October5 controls for Shadowfist, Warriorborn, teleportation and gold have a
different floor raster and green clear color; current clear color is black.
Immortalmight instead uses its October10 accepted capture, whose restored floor
is exactly unchanged. Whole-image historical difference counts therefore must
not be treated as an isolated measurement of the 70c0aa3 shader change.
Independent native phase/RNG, software raster/UV/color interpolation,
destination arithmetic, natural spell callers, audio and broader device scenes
remain outside this recheck.

## Frozen native-domain buff review

`accepted-helper-recheck/buff-freeze.json` pins the existing three buff panels
and their manifest, SHA256
`28d2d4ccfe105cb9e1f0135a6265306ad1a5eedb86cd3bdd88618e4b8877ddc5`.
The authoritative `domain-visual-final2/manifest.json` remains unchanged at
`ec0367f7d6155c3c10ba6da2ca2756006c62382aac873d16de5d074cb7c32e0a`.
Its `domain-capture005.png`, `domain-capture015.png` and
`domain-capture029.png` compare original software, current Metal common domain
and current Metal native domain for Speed, Quicksilver and Fmastery.

Both Metal modes use the same binary above and the same declared source light:
ambient38/255, directional1, no point lights, quality0, origin/facing/cameraZ0,
seed1, warmup1,24Hz. All18 independent native/port sample RNG counts match.
The native-domain correction improves the executed owner/projector agreement;
shared color/raster/interpolation/destination arithmetic residuals remain.
See `NATIVE_SOFTWARE_DOMAIN_20261010.md` for the causal controls and limitations.
