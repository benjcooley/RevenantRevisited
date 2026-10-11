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
