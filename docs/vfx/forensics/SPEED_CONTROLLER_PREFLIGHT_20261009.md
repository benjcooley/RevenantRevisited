# Speed controller preflight and shared curve destination correction

The exact shipped `speed` row is `0xad92bd36`, `magic\Speed.i3d`, member
`Imagery/Magic/speed.I3D`, SHA256
`65c45c03bcf3cd5cfe549232f88fb18550a30720c5b9500d0cfd5ef8154fce42`.
It has eight vertices/four faces, three independent objects, two textures and
one looping state. `speed` has no vertices; `#speedflare` is the animated base
quad and the sole emitter; `#speedo` is the actual particle quad. The two tags
are state0 `blendcont=litaddz` at frame1 and the literal `partsys` at frame5.

The [preflight probe](../../../tools/retail_runtime/speed_controller_preflight.py)
executes all 92 original animator registrations and the original name lookup.
`speed` and `EFFECT` resolve to the default builder `0x5e8508`; `FLAME` is the
custom-builder positive control. Full factory `40dc00`, base constructor
`445940` and dynamic-array constructors `41c7f0` execute. Only resource lookup
`46e8a0` returns the explicitly decoded imagery provider.

Whole native `RefreshControllers40df90`, controller constructors, literal
parser `40d750`, `ParseItem4042e0`, expression parser `4010d0` and both
Initialize bodies execute. The real `blendcont` sets mode80 on every object;
the real particle Initialize resolves exactly `#speedflare` to `#speedo`,
stores PPS30/capacity31 at quality0 and obtains initial object-zero origin
`(0,0,-10)`. Tag frames do not defer this initialization. Prototype vertex
duplication `40a0c0` is the declared resource interface: it copies the exact
shipped quad, never substitute geometry.

During 90 ticks, original asset key decode/`CalcObjectMatrix40a420` supplies
the emitter; full native Pulse, particle Init/Update and live render sampling
execute. Ground height0, RNG seed1, a zero-position owner and MOVE `(16,-8,4)`
at tick60 are explicit fixture inputs. Sampling stops at `4028e2`, before the
owner's animator virtual and mesh/device handoff. This is not full generic
animator Initialize/object construction, punctuation/billboard, renderer
carrier, geometry, UV, material or pixel acceptance.

The initial comparison exposed a production parser error. The full native
parser gives a scale curve a scalar template of zero, while the production
reader retained its absent-field default1. After the checker age offset was
corrected to `particle+d0`, exactly 224 differences remained: scale and render
scale at each of 112 births. Later updates agreed. This failure is retained in
[the original comparison](</Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261009/speed-state/manifest.json>).

The causal instruction is unmodified `4010f3: mov [ebx],0`, before choosing
literal versus curve parsing. Only the first destination is cleared. The
[native/compiled destination probe](../../../tools/retail_runtime/curve_destination_probe.py)
executes seven complete native parser/Initialize diagnostics and compiles the
actual production reader under ASan/UBSan. All 28 comparisons pass: absent
scale/alpha/bounce stay `1/1/(100,100)`; scalar literals preserve their values;
scale and alpha curves have scalar template0; a bounce curve has `(0,100)`.
Its authored first keys deliberately differ from zero, so this does not fit
defaults to a first curve value. [Evidence](</Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261009/curve-destination/manifest.json>).

The production correction in [partsysdefinition.cpp](../../../src/partsysdefinition.cpp)
clears only `expression.constant[0]` after accepting `[`. An absent expression
and a literal expression retain their existing behavior. Atomic parse failure
still leaves the caller's output unchanged. The particle state implementation,
controller integration and rendering code are unchanged.

The corrected Speed comparison passes 85,614 meaningful fields across all31
slots/90 ticks, with maximum absolute error below `5e-7`.
[The comparison probe](../../../tools/retail_runtime/speed_state_compare.py)
compiles the real production parser/State under ASan/UBSan. Native-decoded
authored emitter poses are common inputs; independent compiled pose decode is
not claimed. Never-born alpha/scale defaults are recorded and excluded, as
native Initialize has not initialized those inactive fields; no live or
previously born slot is excluded.

The admitted profiles affected by this parser correction are Might and
Immortalmight (scale/alpha), Fmastery and CombatFlash start1 (scale), and all four
WFall/WCap variants (scale). Gold uses literal scale1.25 and is unaffected.
Previous synthetic controller packets that supplied template1 do not prove
the corrected newborn fields. Their numerical records remain historical;
fresh full-parser comparisons and renewed runtime/appearance review are
required before carrying those claims forward.

Fresh native traces now pass against the corrected compiled parser/State:
Might and Immortalmight each pass 84,926 fields at quality0 and 19,334 at
quality1; Fmastery passes 49,354/11,074. Each case repeats twice in a fresh
Runtime with identical complete rows/poses. [Buff results](</Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261009/curve-destination-buff-final/>).
Gold's unchanged literal-scale case passes 1,526,112 fields. CombatFlash
start1 passes 1,227,242 fields; the fixture records original matrix rejection
of unused state0 tracks without substituting an emitter or transform.
[Combat result](</Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261009/curve-destination-combat-final/comparison/manifest.json>).

The four actual six-emitter water variants pass 1,037,244 meaningful fields
over 34,560 complete-pool rows, 90 ticks each, with two equal fresh native
replays. WFall/WFall2 have capacity140 and WCap/WCap2 capacity52. Maximum
absolute error is below `0.00013`; discrete fields and RNG are exact.
[Water final manifest](</Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261009/curve-destination-water-final/manifest.json>)
pins source/probes before and after. The intermediate gravity-field checker
mistake and subsequent changed-probe hash rejection are retained as
superseded diagnostics. The corrected native gravity address is particle+0,
independently confirmed by unmodified `402c26: fsub [ebx]`.

These restore the bounded numerical newborn/update claims through the actual
parser. They do not renew historical map/device/appearance evidence after a
production change. The synthetic 19,800-field controller regression with two
replays and all 25 controller API tests also pass; their deliberately synthetic
scope is preserved. A reproducible selected-family entry point is
[admitted_full_parser_probe.py](../../../tools/retail_runtime/admitted_full_parser_probe.py).

Strict Speed state0 runtime admission is now implemented, with an exact
asset/path/key/tag/texture descriptor/material/geometry guard. The independent
[profile/integer decoder contract](SPEED_PROFILE_INTEGER_CONTRACT_20261009.md)
passes 258 mutation rejects and all 810 original authored TRS channels.
Modified assets/states fail closed. Unrelated profile paths retain their
existing policies.

The [native render contract](../../../tools/retail_runtime/speed_render_contract.py)
executes actual NewObject punctuation, whole `#` CalcObjectMatrix and the
RenderObject blend carrier followed by complete software SetBlendMode. The
cold base and post-particle base explicitly select mode80 (ONE/ONE, no Z test
or writes); live particle geometry selects mode16 (ONE/ONE, Z test/no writes).
The particle prototype's own mode never replaces the base's explicit80.
No incoming scene blend is guessed. [Evidence](</Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261009/speed-native-render-contract/manifest.json>).

The emitter is itself `#speedflare`. Its native matrix uses Scale then
Rx/Ry/Rz then fixed camera orientation, preserving authored translation. The
production [Speed matrix helper](../../../src/speedauthoredmatrix.h) matches all
30 original matrices/120 authored corners within `1.3e-6`. FillInputs uses
that exact owner-local matrix at the animator's cached integer frame; base
render uses the owner frame and actual root matrix. This avoids fractional
pose interpolation or an ordinary emitter matrix replacing the `#` branch.

Native CalcObjectMatrix rejects scale at or below the literal `5a36d4`
threshold `9.999999747e-6`: `40a591..40a5bf` reaches return0 at `40a5c1`.
Whole RenderObject `40a9e7..40a9f3` returns success1 before blend/material/draw.
The [executed cold rejection](</Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261009/speed-native-render-contract/zero-scale-native-return.json>)
visits the rejection/early return and never reaches its blend carrier.
Speed SubmitPartSys now applies this gate after SampleRender, retaining all
render-time RNG calls. Other profiles' newborn draw admission and inherited
blend gates require separate rechecks, particularly Immortalmight's quads
counter policy; this change does not broaden their renderer behavior.

The [particle packet probe](../../../tools/retail_runtime/speed_packet_probe.py)
compiles actual production parser/State/SubmitPartSys under ASan/UBSan. Over
90 ticks, 2,221 live samples produce exactly 2,109 native/source geometry
draws and 112 matched zero-scale rejections. All quad coordinates/UVs,
own RGB5, mode16, depth and actual prototype texture agree: maximum corner
error `3e-6`, maximum UV error `3e-10`. It executes native `#`
CalcObjectMatrix and x87 transforms, using the exact authored prototype.

Five particle-only shared original software pairs at ticks5/15/29/60/89 have
zero differing RGB565 pixels and equal depth buffers, with 316–410 visible
native pixels. The port's raw-world Z packet is compared through an explicit
MODELZ common domain; this does not prove the actual map/Metal projector.
Canonical ADD tables are built by original `56c730` and its actual returned
pointers are supplied to the fixture; no raster or pixel equation is replaced.
[Final packet evidence](</Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261009/speed-packet-centre-hook-final/manifest.json>).
An independent 4,218 centre checks compare compiled raw particle Z with native
raw Z, then execute original x87 `4027e8..40281f` on the compiled raw input
and compare its output with the separately retained native RenderSample
MODELZ. Raw error is below `4.8e-8`; the original FIX result is exactly equal.
The common-domain normalization therefore cannot hide a wrong centre.
The second fresh native/compiled execution reproduces all draw admissions,
coordinates and five image/depth pair rows exactly.
[Repeat validation](</Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261009/speed-packet-centre-hook-final/repeat-validation.json>)
and the timed [checked-in regression](../../../tools/retail_runtime/test_speed_packets.py)
retain this bounded repeatability, without native scene or full-effect credit.

The first packet attempt stopped at missing ADD fixture tables. Its successor
incorrectly transformed native zero-scale matrix failures as identity; that
failed report is retained. The corrected native/source draw gate replaces
that interpretation. Base normal lighting/material/software raster, actual
map/Metal projection, native scene appearance, caster/spell context and full
acceptance remain open. Ledger credit is owned by integration review.

Asset triage ranks Speed first, Ogrestrength next (three emitters, supported
grammar), then Regeneration (32 emitters) and trollblood (two states, three
emitters). Dexterity/Antimagic/Restorelife have unsupported `scljitter`;
stoneskin/ironskin combine malformed localrotation syntax with unsupported
`numtrails`. These families remain deferred without parser fitting.
