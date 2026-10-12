# fspray hardware RGB8 policy correction

Exact `fspray` `0x0c052638` (`Misc/Fspray.I3D`, SHA256
`1d883eeba4cb9ee857849fd9635084e84ae5ebf5f328c857fd4038a786fada4e`)
was using the original **software** RGB5 carrier normalization in the modern
hardware policy too. The genuine loaded hardware caller preserves packed RGB8.
Only this strict admitted profile now joins Dust's renderer-policy distinction;
other profiles and software lighting remain unchanged. Projection mode does not
select the color policy.

The reference executes cached original factory/CreateAnimator, initial Animate,
NextFrame/owner Pulse/Animate, BeginScene/PreRender/full default Render/PostRender/
EndScene. It records actual `4174fd` device `DrawIndexedPrimitive` calls, FVF1e2,
original vertices/colors/UVs/indices/world matrices and the loaded RGB565 COM
texture. Actual scene state selects MODULATE, ONE/ONE, CULL_NONE, depth test and
no depth writes. No invented emitter, colors or geometry feed the reference.
The unchanged original transform projects those exact packets with zero indices.

The qualified device explicitly declares a linear RGB8 UNORM target, rounding
per fragment, point and linear sampler controls, and modern half-integer sample
centers. Legacy integer-center and the old RGB5 interpretation are retained
controls. It is not a claim about an unchanged original software renderer or a
particular period GPU. Owner/source `(0,0,16)`, camera `(0,0,0)`, center `(320,320)`,
640x480, seed1, quality0; every update and Render runs at24Hz. The initial
10000-coordinate source capture was outside the isolated source viewport and is
retained as a rejected diagnostic; all final pairs use the declared zero origin.

Evidence is under
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-alpha/`:

- `fspray-device-origin0` and `fspray-device-repeat`: 61 complete original
  hardware frames, all packet/state/RNG/pool hashes exactly repeat. Eleven
  sampled frames retain all five device controls.
- `fspray-software-origin0`: all61 original software lifecycle/pool/RNG states
  exactly equal the hardware run.
- `fspray-device-metal/{modern-before-origin0,modern-after}`: two60-frame actual
  Metal films. Timing CSV selects phases5/10/15/20/25/30/39/45/59, without fitting.
- `fspray-device-metal/{software-before,software-after}`: all60 actual Metal
  software-policy images are byte-identical. Existing software appearance
  acceptance remains unchanged.
- `fspray-device-review-final`: nine full-size triples and three detail sheets
  (qualified original / previous source / corrected source), fixed4x nearest
  crop, no intensity adjustment. Aggregate absolute channel error against the
  qualified point-sampled original drops **12115→4066**, improving every phase.
  Before follows the RGB5 negative control; after follows original RGB8.

Frozen pair manifest SHA256:
`3fe7f62ac270cee85abd1c3d0c0ce0513f3fc4013066d818ca29580a18b0ddcd`.
Both binary hashes and all reference/source image hashes are pinned there.
Small remaining differences from texture realization, interpolation and target
quantization are explicit; no exact framebuffer or natural game caller claim.
This correction adds no effect count; its hardware appearance passed root independent review.

Validation: build passes; six focused tests pass, including26112 actual compiled
policy channels, all byte values, exact-type/admission negative controls,
unchanged other-profile/software policies, Dust's23040 compiled channels and
independent semantic/lattice controls. The generalized loaded/device fixture
retains old Dust defaults; its original25 packet/state/RNG frames are checked
against the previously frozen reference.

Run `test_fspray_device.py` and `test_dust_device.py` with the retail Python
module paths. `fspray_device_reference.py --boot-image BOOT --output DIR` emits
hardware proof; add `--software` for the original software control.
`fspray_device_review.py --help` gives all explicit film/reference arguments.
No engine shader, texture decoder, sampling mode, timing or geometry changed.

Root independent reproduction: `research/vfx-fspray-root-20261011/integrated/`
contains fresh original61-frame hardware/software runs, actual120-frame Metal
repeat, and regenerated nine-phase triples. Six tests pass; corrected-channel
error repeats12115→4066 and all60 software images remain unchanged. Root reviewed
all fixed whole/detail phases5/10/15/20/25/30/39/45/59. The additional hardware
review preserves the primary software approval and adds no headline effect count.
