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
