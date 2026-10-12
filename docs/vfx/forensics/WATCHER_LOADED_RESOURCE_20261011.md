# Watcher loaded-resource diagnostics

`tools/retail_runtime/watcher_loaded_resource_probe.py` observes original
class/resource/mesh loading for `ogrokwatcher1`, `ogrokwatcher2`, and
`ogrokwatcher3`. It uses the source-matched cached retail startup through
`AuthenticCharacters`, selects the original PlayScreen prefix, and calls the
original class factory and CreateAnimator. The actual primary imagery field is
owner `+0x54`; an earlier probe using `+0x4c` observed no textures and was rejected.

The qualified run observes one texture per watcher, with 4/4/6 resident frames.
All resident surfaces retain their actual ARGB1555 descriptors: bit count16,
red `0x7c00`, green `0x03e0`, blue `0x001f`, alpha `0x8000`, and pitch256.
It records every frame's surface descriptor and pixel hash without retagging or
replacing texture data.

`tools/retail_runtime/watcher_loaded_visual_reference.py` uses those genuine
loaded resources and runs original NextFrame, default Animate, BeginScene,
PreRender, Render, PostRender (`0x40eb00`), and EndScene for eight ticks each.
Its explicit scene inputs are a 640x480 RAM framebuffer, camera0, ambient38 and
directional1. PreRender can rebind the game display, so the original camera
target/bounds APIs select the isolated framebuffer afterward. The existing COM
boundary supplies material-handle GetLightState/SetLightState key1 only.
An earlier incorrect PostRender entry was rejected and retained separately.

Run either tool with `--boot-image PATH --output NEW_DIRECTORY`, using the
retail laboratory's Python environment. `AuthenticCharacters` checks that the
cache filename matches the current executable and boot sources. The rebuilt
retail executable and licensed assets are local prerequisites; they are not
included in these source tools.

Evidence is retained locally under
`/Users/benjamincooley/RevenantRetailLab/research/vfx-watchers-loaded-20261011/`:
`primary-imagery/manifest.json` records loaded resources and
`render-entry-qualified/manifest.json` records original render output.
Both reports deliberately set `accepted=false`. These diagnostics establish
loaded-resource/render execution, not port appearance acceptance. Generic
map/owner Pulse, device accuracy and the software texture-format discrepancy
remain separate questions. No watcher visual pass is added by this checkpoint.

Independent inspection of all three qualified first-frame renders confirms
that the original software output still has noisy rainbow colors rather than
the authored figure colors. The discrepancy therefore survives genuine texture
loading; the earlier fabricated texture-residency boundary was not its cause.
Keep the correct authored ARGB1555 decoder and all three visual rows open while
investigating the original software/device format handling. Do not retag the
loaded surface or tune port colors to this corrupted reference.

## Complete unchanged descriptor/upload/raster chain

`watcher_surface_flow_probe.py` now observes original GetClosestTextureFormat
4164c0, ConvertTexture416790, SetTexture56d3a0 and actual leaf raster entry.
All14 uploaded frames match exact authored ARGB1555 bytes. All source, closest
and realized Lock/GetPixelFormat masks remain7c00/03e0/001f/8000.
Scene initialized=1 and software=1 cause4164d6 to bypass hardware format
negotiation; the fixture's zero hardware-format enumeration is not involved.

Original SetTexture56d3ea tests only whether the alpha mask is nonzero. Every
one of24 draws selects54e050→54ff60. Actual texel instructions550497 and55049d
use word&0xfff and word>>12, decoding ARGB4444 regardless of the1555 masks.
A sampled authored34916(0x8864), opaque RGB5(2,3,4), is consequently read as
alpha4=8,RGB4(8,6,4). This directly explains the neon colors without tint fitting.

Actual BeginScene before NewObject/CreateAnimator does not repair the format:
all24 resulting PNGs are byte-identical to resource-first initialization.
All operations above are read-only observations of unmodified original code;
no format/texel/state substitution is used. Private evidence:
`hard-alpha/watcher-surface-flow-{first,scene-first,final}`. The final report
includes48 nonzero texel observations, exact asset/frame hashes, and assertions
for every upload and selected surface. Original software ARGB1555 handling is
therefore unsupported by this demonstrated dispatch. Correct portable1555
colors remain intact, and no primary appearance acceptance is claimed.

## Explicit adapted-device controls (not unchanged retail)

`watcher_qualified_device_probe.py` leaves the portable decoder and original
software raster untouched. Its declared COM texture allocation realizes only
exact ARGB1555 textures as ARGB4444. `--realization-only` demonstrates that this
alone does not fix the image: the original uploader uses its requested descriptor
and still copies the1555 words unchanged. That failed control is retained.

The default qualified control adds an explicit call to original ConvertTexture
416790 for every resident frame after loading, using separately declared1555
source and4444 destination descriptors. All229,376 texels across14 frames exactly
match RGB5>>1 and alpha1<<3. Thus the original converter reduces RGB to4 bits but
maps opaque alpha to8, not15. The recovered figure colors are darker and partly
transparent. All24 output PNGs match a fresh independent replay. This added
conversion is not performed by the unchanged retail software device.

`--semantic-alpha` is a second, explicitly adapted device boundary: it preserves
the RGB4 produced by that original conversion, but maps authored binary alpha
zero/one to nibble zero/15. This is a host device-format conversion rule, not an
original-native claim. All229,376 resulting words are checked against the exact
bit formula. The complete original frame selection, normal lighting, geometry,
SetTexture and software raster run afterward. The rainbow corruption disappears
and the authored figure hues return. No port color/opacity is fitted to either
qualified reference, and no watcher primary appearance pass is granted.

Frozen private reports under `hard-alpha/`:

- `watcher-qualified-device-native-final/qualified-device.json`:
  `b6b8e9c8d3ffd6be6dd345e7b5f8e52b33cda0e4333fe84e92c16aca6185474c`.
- `watcher-qualified-device-semantic-final/qualified-device.json`:
  `55e3f937c5f6ab8c0d5222f2e39d51dd1ffe7ae371e6032ff732fb2d2d174c6e`.
- `watcher-qualified-review/` retains identical fixed crops of unchanged,
  original-converted alpha8, and adapted semantic-alpha15 output for all four
  active texture phases of each watcher. Full640x480 images remain available.

The original converter and semantic-device controls both assert every stored
frame (including watcher3's two inactive frames), while the unchanged four-frame
owner animation selects only its authored active frames. Device equivalence,
RGB5-to-RGB4 quantization, modern sampling/projection/blending, generic owner
Pulse and natural map placement remain open. Reports explicitly retain
`accepted=false`, `primary_visual_pass=false`, and `device_identical=false`.

## Correct portable decoder and actual Metal diagnostic

`watcher_decode_contract.py` compiles the unchanged production texture decoder
with ASan/UBSan and the actual `retailmeshlighting.h` selector. All65,536 words
of each1555/4444/565 format pass786,432 independent channel assertions. In
particular1555 opaque alpha becomes255, and RGB5 uses full normalized expansion;
the policy selector returns2 for1555,3 for exact4444, and1 for565. This proves the
portable decoder rather than importing the original software corruption.

Fresh actual Metal captures in `hard-alpha/watcher{1,2,3}-metal-final` each
completed8/8 visible frames, four distinct authored phases, and clean exit.
They use unchanged binary
`4ab18b5620806b6b555c23235838a2168279df8c372aa9b09c14d03590a6e620`,
640x480, origin0, camera320/240, ambient32/white, configured85% directional
light (logged source ambient38/255 and direction1), and24Hz rendering. The
existing static preview advances its real owner NextFrame and selects the
registered authored frame meshes. No native projection opt-in was added.

`watcher-qualified-review/ogrokwatcher{1,2,3}-metal-detail.png` compares all four
active phases against the explicitly adapted semantic-alpha reference. A fixed
identical crop250/70/390/260 and nearest2x enlargement contains every whole
visible effect on both sides; raw640x480 frames and hashes remain retained.
Shape, phase and authored purple/gold hues agree on inspection. Metal is a few
pixels taller/higher and has brighter fine detail. Common-world projection,
RGB5 versus adaptedRGB4 precision, sampling and framebuffer arithmetic remain
explicit differences. `metal-diagnostic.json` does not grant acceptance or
claim an unchanged original device, natural world placement or exact pixels.

## Audited native owner/projector preview

The three exact watcher preview registrations now select
`SubmitNativeWorldMeshForTest_BESPOKE` only with `--vfx-native-domain`.
The existing common-world submission remains unchanged. The new path uses the
shared original owner matrix/projector, existing source-alpha helper with no
culling/depth writes, and correct1555 lighting policy2. It does not convert port
textures to4444, change the portable decoder or modify any renderer/shader.

`watcher_loaded_pose_probe.py` observes actual loaded fullRender FVF112 streams,
world matrices and render states. `watcher_pose_contract.py` compiles actual
production key decoding, mesh extraction and the new native submission with
ASan/UBSan. All24 draws match1,920 matrix/XYZ/normal/UV values with maximum error
3.3544922e-8; topology and texture-frame selection are exact. Original states are
CULL_NONE1, ZWRITE0, ZTEST1, SRCALPHA5, INVSRCALPHA6. This is explicit origin0,
facing0, eight-tick frame selection; generic owner Pulse/natural placement remain
outside the fixture.

Fresh native-projector Metal captures again pass8/8 visible/four distinct phases
per watcher and clean exit. The common watcher1 eight-frame regression is
byte-identical at pixel level to its pre-change film. Whole/detail panels and
all input hashes are frozen in `hard-alpha/watcher-native-review-final/`.
Its `pair-manifest.json` SHA256 is
`8ea17d1b9153bad91cd3d2fba504b5c4099e5da4526179ec57611dd0b2980225`.
The common projection height discrepancy is resolved. RGB5 versus adaptedRGB4
precision and raster-edge differences remain visible; the reference still
explicitly uses the qualified semantic-alpha device adaptation. Decoder, original
fullRender pose/state, and actual Metal evidence are distinct supporting proofs,
not a claim that the original software device renders1555 correctly. No approval
or primary acceptance is written by these tools.

Root independently inspected all three native-projector whole/detail pairs and verified63 frozenpins. Independent1920stream/matrix checks and786432decoder assertions pass; all24 integratedGPU PNGs byte-identical withcleanexit. Private `vfx-watcher-root-20261011/qualified-review.json` records this adapted-device comparison separately. Original1555software misdispatch and RGB5-to-RGB4 device adaptation remain explicit; primaryreview count stays159.
