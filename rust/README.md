# Revenant in Rust

An idiomatic Rust port of the engine in `../src`. Preserve the original data
formats and observable behavior while replacing the C++ ownership model,
inheritance, globals, and unchecked resource pointers with Rust structures.

The port includes a **GPU renderer, asset cache, texture atlases, and native
I3D animation**. It renders the retail map, props, characters, attached equipment,
HUD, fonts and cursors from the installed data. A controllable Locke preview
uses authored motion and terrain collision. Desktop frames present through wgpu
and winit. The software renderer, CPU UI compositor, minifb presenter and backend
selector have been removed.

Combat, scripts, save games, sector streaming, gameplay effect controllers and
gameplay-driven UI/audio remain outside this rendering implementation.
Exploration stays inside the loaded neighborhood.

## Run

From this directory, with stable Rust 1.85 or newer and a Metal, Vulkan or DX12 adapter:

```sh
cargo run --release
```

The default loads sector `0_2_25` and its available neighbors. Data is discovered
in `../data`, near the executable, or from `REVENANT_DATA_PATH`. To select an
installation or a different sector:

```sh
cargo run --release -- --data /path/to/Revenant --sector 0_3_25
cargo run --release -- --sector 10_3_17 --radius 1 --fit
```

If a package-manager Rust compiler is broken but a working stable rustup
toolchain is installed, `./scripts/cargo.sh` can replace `cargo` in every command.
It selects that toolchain for the child process without changing global settings.
This also works on the current development machine, whose Homebrew `rustc`
cannot load its LLVM library.

```sh
./scripts/cargo.sh run --release
```

## Linux and Windows

The native targets are Linux (Vulkan, X11 or Wayland), Windows (DX12 or Vulkan,
WASAPI audio), and macOS (Metal, CoreAudio). Game assets remain external.
The renderer honors `WGPU_BACKEND=vulkan`, `dx12`, or `metal` for diagnostics.
There is one renderer and one native event adapter across these platforms.

On Debian/Ubuntu, install the desktop development dependencies before building:

```sh
sudo apt-get install build-essential pkg-config libasound2-dev libxkbcommon-dev libxkbcommon-x11-0 libwayland-dev libx11-dev libxi-dev libxcursor-dev libxrandr-dev libvulkan1 mesa-vulkan-drivers
cargo build --locked --release
./target/release/revenant --data /path/to/Revenant
```

Use the GPU vendor's Vulkan driver where appropriate. Both X11 and Wayland are
compiled in; winit selects the active desktop session.

On Windows, use Rust's `x86_64-pc-windows-msvc` toolchain with the Visual Studio
C++ build tools and Windows SDK. From PowerShell in this directory:

```powershell
cargo build --locked --release
$env:WGPU_BACKEND = "dx12"
.\target\release\revenant.exe --data "C:\Games\Revenant"
```

Set `WGPU_BACKEND` to `vulkan` to test that backend instead. `cargo test --locked
--all-targets` runs the asset-free tests. The window/presentation smoke can also
run without a game installation:

```sh
cargo run --locked --example gpu-smoke
cargo test --locked --test gpu -- --ignored --test-threads=1
```

The GitHub workflow in `../.github/workflows/rust.yml` builds and tests on Linux,
Windows MSVC and macOS, including builds without desktop features. It also runs
the terrain collision comparison against the C++ engine on Linux and macOS,
Vulkan shader tests, Xvfb/X11 and headless Weston/Wayland window smokes on Linux,
and produces native executable archives without game data. Linux shader tests
use Mesa's CPU Vulkan device; Windows shader tests use DX12 WARP. These exercise
the GPU APIs and shaders. Physical drivers and Windows audio/input still require
testing on the target machines.

Linux can be exercised locally from this Mac through Docker. The source mount
is read-only, with Linux build artifacts and registry data in separate volumes:

```sh
docker build -t revenant-rust-linux:local -f ci/Dockerfile.linux ci
docker run --rm --cpus 2 --memory 4g \
  -v "$PWD:/workspace/rust:ro" \
  -v revenant-rust-linux-target:/target \
  -v revenant-rust-linux-registry:/usr/local/cargo/registry \
  revenant-rust-linux:local
```

On Apple Silicon this tests Linux ARM64; the GitHub Linux runner covers x86-64.
For a Windows cross-build on macOS with MinGW installed:

```sh
rustup target add x86_64-pc-windows-gnu x86_64-pc-windows-msvc
CARGO_TARGET_X86_64_PC_WINDOWS_GNU_LINKER=x86_64-w64-mingw32-gcc \
  ./scripts/cargo.sh build --locked --target x86_64-pc-windows-gnu
./scripts/cargo.sh check --locked --all-targets --target x86_64-pc-windows-msvc
```

The GNU build produces `target/x86_64-pc-windows-gnu/debug/revenant.exe`.
The MSVC check verifies target-specific Rust code without the Microsoft linker;
native CI supplies the actual MSVC build. Neither cross-check proves Windows
runtime behavior.

## Controls

- WASD / arrows: pan; hold Shift to move faster.
- Mouse wheel / `+` / `-`: zoom around the cursor.
- Home: reset the camera; F: fit the loaded neighborhood.
- Tab: switch between color and per-pixel depth.
- Space: start/pause mesh animation preview (24 fps, looping).
- Left click: inspect the visible object in the title bar, HUD text, and terminal.
- Middle-button drag: pan the camera.
- F1: show/hide the retail HUD; H: open/close its sidebar.
- E / K / B: equipment / stats / spell book; I / M / P: inventory / map / spells.
- F9: mute/unmute sound; `--volume 0.5` sets desktop volume.
- F12: save a PNG in `captures/`; Escape: quit.

## Walk preview

```sh
./scripts/cargo.sh run --release -- --walk
```

WASD moves Locke relative to the screen; holding the right mouse button steers
toward the pointer. Hold Shift or R to run. Arrows pan the camera. The camera follows
him until you pan or press F to fit the neighborhood; Home resumes following.
Zoom, depth view, object inspection, and captures work as before.
The HUD is enabled by default; `--no-hud` starts with it hidden. Space playback
is disabled in this mode because Locke's idle/walking poses follow simulation ticks.

This creates a separate preview actor on the nearest safe ground in the selected
sector. `--spawn X,Y` selects a specific initial world position, checked against
the walkmap. Movement runs at 24 ticks/second, uses a 16-unit collision radius,
and checks terrain every eight world units or less. Nonzero walk heights,
32-unit step limits, and the original radius/distance calculation decide whether
a move is allowed. Empty or unloaded ground stops movement.

Movement comes from each frame's authored motion record, using the C++ facing
lookup and 16.16 position accumulator. It plays `100:walk` when idle, `walkf` when
walking, and `runf` when running. On the local retail assets, one walk cycle
travels 85.617 world units over 15 frames; one run cycle travels 151.738 over
17 frames. Fractional distances survive clip repeats, so diagonal and slow motion
do not lose displacement to rounding. Zero-distance frames still advance.
`AF_NOMOTION` suppresses movement; entering a root stance clears the accumulator.

Held input repeats the selected locomotion action. Blocking returns to idle;
changing direction turns immediately. The preview snaps to the terrain surface.
Vertical motion records are decoded and retained, but applying one through this
ground controller is an explicit error. Vertical smoothing/gravity, turning transitions,
character-to-character collision, wall sliding, door interaction,
and scripts are not implemented. Equipment geometry can be attached explicitly
as described below; inventory, item stats and equip/unequip gameplay remain unported.
Loaded door states contribute their static walkmaps, but
the preview cannot operate doors or trigger level changes.

For a repeatable movement capture without opening a window:

```sh
./scripts/cargo.sh run --release --no-default-features -- \
  --walk --ticks 48 --direction east --headless captures/walk-east.png --strict
./scripts/cargo.sh run --release --no-default-features -- \
  --walk --run --ticks 6 --direction east --headless captures/run-east.png --strict
```

Directions here are world directions (north is negative Y, east is positive X),
including northeast/southeast/southwest/northwest. The terminal reports spawn,
final position, and moved/blocked tick counts. Omit `--direction` for idle ticks.
`--run` also selects running on desktop without holding Shift. Full character
action scheduling, weighted idle choices, transition tags and speed overrides
from `rules.def` remain to be ported.

## Equipment preview

Attach retail gear to a mesh preview or the walking actor with repeatable
`--equip SLOT=PATH` arguments. Paths are relative to `imagery/`:

```sh
./scripts/cargo.sh run --release -- --walk \
  --equip body=Equip/dsblackchest.i3d \
  --equip primehand=Equip/ShortSword.i3d
./scripts/cargo.sh run --release -- --mesh Chars/locke.i3d --state 265 --frame 6 \
  --equip body=Equip/dsblackchest.i3d --equip primehand=Equip/ShortSword.i3d \
  --headless captures/equipped-locke.png
```

Slots are `head`, `neck`, `body`, `offhand`, `primehand`, `rightaccessory`,
`leftaccessory`, `rangedweapon`, `ammo`, `legs`, and `feet`. Duplicate CLI slots
are rejected. `--body-type` defaults to Locke's `normal` variant. `--bow-mode`
selects ranged attachment geometry instead of prime-hand geometry; select a
matching animation separately when inspecting bow poses.

Bindings follow `TCharAnimator::ProcessEquipment`: select the first
still/all/body-type state, match armor part names, prefer weapon/sword bones for
hand slots, hide replaced body parts, and copy the character's evaluated matrix.
The retail name handling in `uiequiptest.cpp` also strips `*` markers and body
prefixes such as `normal`; generic states can fall back to visible body-prefixed
parts. Equipment parents and local animation keys are not evaluated as a separate
skeleton. Texture sampling uses frame zero, matching the native attachment call.
The character's root transform is applied once. Bounds, depth and picking include
the attached geometry, with one owner for the entire character.

`MeshDraw::equip` replaces a slot atomically; `unequip` restores uncovered body
parts. These operations do not mutate shared mesh assets. Character rendering
hides the baked weapon placeholders even without gear, as the native animator
does. Raw `--mesh` previews without equipment retain their original part visibility.
This is the attachment renderer; inventory ownership, slot eligibility, stat
bonuses, automatic loadouts and combat actions are still separate port work.

## HUD, cursors, and audio

`--walk` now opens the retail HUD. `--hud` also enables it for a scene or mesh
preview; F1 toggles it on desktop. The same composition works in headless PNGs:

```sh
./scripts/cargo.sh run --release -- --walk --width 640 --height 480 \
  --equip body=Equip/dsblackchest.i3d --equip primehand=Equip/ShortSword.i3d \
  --headless captures/hud.png
```

The status panel, portrait, dim status bars, bottom bar, empty quick-use slots,
sidebar tabs, equipment, inventory, stats, spell book and spell panes use retail
`.dat` artwork. Top and bottom pane selection are independent. The equipment
pane renders the actor's current pose and attached gear; outlined slots can be
clicked to inspect their bindings. It uses the existing isometric renderer,
not the C++ paperdoll's separate perspective camera. Equipment inventory icons,
drag/drop and gameplay equip actions are not implemented.

Health/mana/fatigue, stats and gold display `--`; inventory and spells are empty
until their gameplay components supply state. The map tab shows walkable terrain
from the loaded neighborhood, centered on the preview actor. It does not store
exploration or implement the native automap lifecycle. UI animation, menus,
dialogue and inventory icon playback remain unported.

Art keeps its native size at 640×480 and larger framebuffers; smaller buffers
scale the whole layout uniformly. The world camera centers inside the playfield.
HUD geometry drives both rendering and hit testing. Pointer presses retain their
world/UI owner until release; leaving the window or losing focus cancels them.
Dragging across panes cannot activate another button or start world movement.
Original pointer, hand, directional wedges and alias shadows render above the
HUD, with native registration and scaling independent of world zoom. Right-button
walking uses the native z+50 mouse plane and stop footprint; it is held steering,
not pathfinding. Middle drag skips pre-press and UI-crossing motion.

UI press/release actions play the retail click WAVs. `--muted` starts muted;
F9 toggles it without changing `--volume` (0–1). PCM8/PCM16 mono/stereo decoding,
shared clips, up to 32 overlapping voices, resampling, looping and stop controls
are implemented. MP3 music/dialogue, spatial attenuation, area music, animation
sound events and gameplay scheduling are not. Headless tools never open audio
hardware. Missing desktop audio hardware is a recoverable warning.

Desktop builds use CPAL and winit; Linux needs ALSA development libraries.
`--no-default-features` avoids window and audio dependencies; rendering still
requires a GPU. Inspection, decoding and asset validation do not open a device.
The event owner clears input on focus loss and restores the system cursor on
exit. Window resizing and pointer coordinates share the same letterbox transform.

## Headless tools

The core library and CLI can build without a windowing dependency:

```sh
cargo run --release --no-default-features -- --headless captures/misthaven.png --strict
cargo run --release --no-default-features -- --inspect --sector 0_2_25
cargo run --release --no-default-features -- --validate
cargo run --release --no-default-features -- --validate --level 0
cargo run --release --no-default-features -- --validate --validate-imagery
cargo run --release --no-default-features -- --validate --validate-meshes
cargo run --release -- --mesh Chars/jason.i3d --state 1 --animate
cargo run --release --no-default-features -- --mesh Chars/jason.i3d --state 1 --frame 8 --headless captures/jason.png
```

`--strict` makes scene-loading warnings fatal. Effect objects and I2D states
without still images are counted separately.
Unsupported mesh formats produce explicit decode warnings. `--mesh` previews an
asset using the same renderer as the map; `--state` and `--frame` choose its pose.
`--animate`/Space loops mesh clips as a visual preview, without gameplay ticks,
root motion, state transitions, movement, or saved animation-rate semantics.
`--frames 4` opens a native window and exits after four frames for a desktop smoke
test. `--help` lists all options.

No game assets are included, extracted to disk, or modified. Optional enhancement
assets can be mounted with `--overlay <directory-or-rvr>` or
`REVENANT_REVISITED_PATH`. The overlay is opt-in. Read priority is overlay,
loose install files (root, Resources, Imagery, module directory), module archive,
imagery archive, resource archive. Asset names are case-insensitive, including
DOS backslash paths. SavePath precedence will be added with the save subsystem.

## Structure and C++ correspondence

- `assets/vfs.rs` replaces the read side of `revutils.cpp`: an owned stack of
  directory/ZIP mounts with normalized names. `.rvi`, `.rvr`, and `.rvm` files
  are read as ZIP archives. Errors retain the asset path and archive context.
- `assets/catalog.rs` replaces the type lookup portion of `TObjectClass` in
  `object.cpp`: `Catalog`, `ClassId`, `TypeId`, and immutable definitions. Type
  stats remain ordered vectors; stat declarations and default-value evaluation
  are not implemented yet. Class IDs use the retail numeric assignments.
- `assets/binary.rs` replaces native struct casts and stream pointer arithmetic:
  checked, little-endian views and field-relative offset resolution.
- `assets/imagery.rs` and `assets/chunk.rs` port the CGSR/I2D path from
  `resource.cpp`, `imageres.h`, `bitmapdata.h`, `chunkcache.cpp`, and the C++
  tile uploader. Bitmap tables, state registration, walkmap bytes, RLE/LZ,
  RGB palettes, RGB555, transparency, and signed authored depth are decoded
  into owned arrays. LZ references address the compressed source, not output.
- `assets/mesh/` ports modern I3D bodies, owned vertex/face batches, masked and
  paletted texture frames, packed 32/64-bit keys, pose spans, and checked bone
  hierarchies from `3dimage.cpp` and `3dimagebody.h`. `render/mesh.rs` samples
  per-instance poses and bounds; GPU shaders transform persistent indexed
  geometry into the shared tile/mesh depth and picking targets. Standalone parent ordering is checked when an instance is built,
  separately from decoding assets that can be attached to a character.
- `world/sector.rs` replaces `TSector::Load` and the common object deserializer:
  sectors own their slots; empty slots retain their indexes. Retail v14/v15
  inventories are recursive values. Characters' serialized base-version prefixes
  are read without constructing a runtime inheritance hierarchy. Class extensions
  remain opaque bytes until their gameplay components are ported.
- `world/mod.rs` turns sector data into a scene. Immutable tile and mesh assets are shared
  through `Arc`; the `Draw` enum carries tile instances or independent mesh poses. No global
  registries, manual reference counts, raw pointers, or C++ FFI are involved.
- `world/walkmap.rs` assembles owned 64×64 height grids per sector, independently
  of visible draws, from shared I2D/I3D state metadata. It ports registration,
  cross-sector stamps, `OF_NOWALK`, inventory exclusion, highest-surface writes,
  exit overrides, centered lookups, and radius probes from `sector.cpp`,
  `gamemap.cpp`, and `mappane.cpp`.
- `world/movement.rs` supplies bounded fixed ticks and a terrain movement component.
  `world/exploration.rs` connects that component to a separate preview mesh with
  idle/walk/run clip changes and authored displacement. The renderer refreshes
  transforms and bounds together.
- `assets/mesh/motion.rs` decodes the three motion record layouts independently
  of pose tracks. State-table strides follow I3D body versions 1–3. Native facing
  and relative rotation use wrapping byte angles; widened arithmetic avoids
  overflow during fixed-point vector conversion.
- `assets/cache.rs` provides typed, mount-scoped source residency. `Arc` leases
  pin assets; least-recently-used unleased entries can be trimmed. The native
  UID hash remains available for interoperability without global registries.
- `render/atlas.rs` packs color and signed depth into bounded pages with extruded
  borders. `render/gpu/textures.rs` owns GPU pages, uploads, weak source identities,
  working-set eviction and frame-boundary repacking.
- `render/gpu/meshes.rs` retains indexed vertex buffers, morph snapshots, part
  bounds and independent instance transforms. Repeated geometry uploads once;
  animation changes transforms or selects resident snapshots.
- `render/gpu/` owns geometry, AO, shadow, blur, lighting, transparency, FX, UI
  and presentation passes. Renderer inputs contain no gameplay or VFS globals.
- `assets/bitmap.rs`, `multi.rs`, and `font.rs` decode checked native UI resources:
  named field-relative entries, RGB555/RGB565, indexed palettes, ARGB4444,
  separate alpha, explicit alias streams, and bitmap glyph bearings.
- `ui/` owns pane state, shared layout/hit testing, and scaled bitmap composition.
  `input/` owns pointer capture and cursor artwork; `audio/` owns PCM clips,
  mixing, and optional device output.
- `main.rs` owns startup, argument parsing, diagnostics, and the optional native
  window. The desktop backend presents the GPU target directly and supplies input.

Rust source and test files retain the classic Cinematix Revenant banner,
including the original copyright line and each module’s filename and purpose.

The crate forbids `unsafe` in its own code. The existing C++ build is independent.

## Compatibility boundaries

Sector versions present in the shipped Ahkuilon module are supported: v1 and
v9–v15. The v1 reader handles the tiles and exits present in that campaign;
unknown unframed classes fail explicitly. In v9–v13, the combined class-extension
and inventory tail is retained as `Inventory::LegacyTail`, not claimed to be a
decoded inventory. Full recursive inventory decoding starts at v14, where the
format supplies the separate tail length.

Imagery currently supports uncompressed CGSR bodies, indexed/RGB555 still
bitmaps, chunked color and depth, and unchunked alpha. Animation-only states have
no still image and are reported separately. Chunked alpha follows the current
C++ tile path's opaque-alpha treatment. Animated I2D playback remains a follow-up port.

Walkmap stamps preserve the current C++ facing lookup, including its unusual
west-facing indexing; any lookup outside the source data returns a warning
before applying the stamp. Radius probes use owned storage and match the current
C++ rounded distance table, absolute adjacent height deltas, explicit hole flag,
and radius clamp/bypass behavior.
Movement also splits equal-X/Y diagonals into collision checks (the C++ split
omits that case). These bounds fixes do not change the static height format.
The default neighborhood supplies 30,273 nonzero walk cells on the local install.

Modern I3D body versions 1–3 support static geometry with 32/64-bit animation
keys, hidden parts, per-state hierarchy, per-frame textures, and the native 1.5
mesh-local Z stretch. Original fixed-array bodies also decode geometry,
materials, animated textures, signed-byte parents, 64-bit animation tracks,
and the original motion records into the same owned types. Old state key data
stores each object's complete track in sequence; it is not a pointer table or
one shared track. The Rust decoder preserves these poses rather than copying
the current C++ old loader's zero-key fallback. Morph geometry decodes state/frame
vertex snapshots in both layouts, skips rigid key transforms and keeps per-frame
bounds. Snapshot allocation is bounded independently of file size. Inventory
icon playback and animation tags remain unported.
Equipment assets preserve their stored parent metadata without requiring it to
form a standalone skeleton. C++ `TCharAnimator::ProcessEquipment` supplies the
character's matrices instead of evaluating those links. The initial validator
incorrectly rejected five such assets (`dsblackchest`, `dsblackhand`, `nakboot`,
`nakchest`, `nakhand`); they now decode. The local catalog scan decodes 556/556
assets and 5,663 clips, including the 25 original bodies previously unsupported.
The broader legacy test checks all 44 original bodies / 75 clips exposed by the
local install, including assets absent from `class.def`.
Standalone pose construction still rejects repeating parent traversals. Equipped
pieces use character matrices through the attachment renderer; effect controllers
remain unported. Null animation tracks hide their parts; missing channels
default to identity. Texture decoding retains the C++ black-atlas alpha heuristic.

## GPU engine

The world pass writes albedo, normals, signed scene depth and object IDs, with
hardware depth testing shared by tiles and meshes. A 512-pixel border retains
offscreen shadow casters. Reconstructed tile normals and transformed mesh normals
feed AO, half-resolution sun shadows, separable blur and deferred lighting.
Up to 16 nearby serialized sector lights use the native radial attenuation curve.
Transparent tiles and meshes sort together by depth; alpha picking is resolved
after color composition. Intersecting transparent surfaces use batch sorting;
there is no order-independent transparency pass.

Lighting modes are `--lighting modern`, `retail` and `unlit`. Retail mode supports
RGB texture Gouraud lighting with source five-bit quantization, alpha-texture
lighting bypass and distance-only point lights. `RetailMeshLighting::from_native`
preserves the original integer ambient conversion and directional percentage.
Area definitions, time-of-day changes and native lighting INI settings are not
connected to the preview yet.

`MeshDraw::set_appearance` supplies independent tint, UV scrolling and surface,
full material or unlit shading. Native diffuse, ambient, specular, emissive and
power fields are decoded. Mesh UVs repeat within their texture slice. Transforms
use inverse transpose normals for nonuniform scale. World geometry uses
double-sided triangles and nearest texture sampling.

`render/effects.rs` provides screen/world billboards, tapered strips, arbitrary
quads/triangles and clipped screen overlays. Alpha, two additive modes and
premultiplied alpha combine with depth test, no depth or depth write. Filtering
and repeat/clamp addressing operate within each source slice. Submission order
is preserved for UI; world FX sort back to front. These are rendering primitives;
the map's gameplay effect controllers still need their own ports.

`--perspective <focal-distance>` enables perspective projection and depth-sprite
ray reconstruction; zero preserves orthographic projection. Diagnostic views
include albedo, depth, normals, world position, point lights, shadows, AO, depth
edges and height. `--no-ao` and `--no-shadows` isolate those passes.

Source-cache budgets and GPU texture/mesh budgets are independent. Source leases
can temporarily exceed their soft budget; GPU working sets must fit or rendering
returns an error. `--texture-budget <MiB>` adjusts atlas residency. Textures and
meshes are reclaimed when source leases expire or unused realizations must be
evicted. GPU statistics expose uploads, resident bytes, instance counts, repack
generations, presentations and readbacks.

Desktop world frames stay on the GPU. Readback is requested for screenshots and
picking; headless captures always read back. Equipment preview uses a separate
GPU target sharing asset residency, then reads its small image into a cached HUD
texture. HUD, fonts and cursor composition use GPU commands. `--frames N` reports
presentations and readbacks and fails if no frame reached the window.

In the local retail archive, two depth streams exceed the 4,096-pixel chunk
capacity under the C++ decoder's format:

- `Imagery/Forest/ForTreeC.I2D`, state 0, depth chunk (1, 2).
- `Imagery/Ruin/RunARoom39.I2D`, state 0, depth chunk (0, 1).

Rust rejects these with an error. Consequently the optional full imagery scan
returns a nonzero exit code on this installation. The default Misthaven scene
does not use these assets. No data files are patched to mask the errors.

## Verify

```sh
cargo fmt --all -- --check
cargo clippy --all-targets --all-features -- -D warnings
cargo test --all-targets
cargo test --no-default-features
cargo test --test gpu -- --ignored --test-threads=1
cargo test --test retail campaign_sectors_and_default_scene_load -- --ignored
cargo test --test retail locke_walk_preview_moves_and_stops_at_misthaven_terrain -- --ignored
cargo test --test walkmap_reference -- --ignored
cargo test --test motion_reference -- --ignored
cargo test --test legacy_mesh -- --ignored --nocapture
cargo test --test equipment -- --ignored --nocapture
cargo test --test hud -- --ignored --nocapture
cargo test --test ui_assets -- --ignored --nocapture
cargo test input::cursor::tests::retail_cursor_artwork_decodes_and_renders --lib -- --ignored
cargo test --test audio_retail -- --ignored --nocapture
REVENANT_CPP_TILE_DUMP=/path/to/i2d_dump_all \
  cargo test --test retail tile_decode_matches_cpp_rgba_and_signed_depth -- --ignored
REVENANT_CPP_MESH_DUMP=/path/to/i3d_dump_all \
  cargo test --test retail mesh_geometry_and_textures_match_cpp_exports -- --ignored
REVENANT_CPP_GLTF_DUMP=/path/to/gltf_dump_all \
  cargo test --test mesh_pose -- --ignored
```

The ordinary tests use synthetic fixtures and require no game assets. They cover
relative-offset bounds, signed values, chunk decoding and malformed input,
catalog syntax, map versions, inheritance prefixes, inventory boundaries,
source residency, atlas isolation, lighting conversion and mount precedence.
Opt-in GPU tests require an adapter but no game assets; they cover depth, alpha,
picking, morph instances, triangle seams, blend states, clipping, texture
residency, resizing and diagnostic passes. The opt-in
tests run against the user's installation and compare representative tile
pixels/depth against C++ `.ztile` exports, plus representative I3D vertex
positions and every decoded texture pixel against C++ OBJ/PNG exports. Renderer
tests check tile/mesh occlusion, picking, and triangle seams with partial alpha.
The GLB comparison samples Jason's posed vertices at the first, middle, and last
frame of all 14 clips. Texture tests cover UV repeat addressing on both axes.
Terrain tests cover hidden/undrawable imagery, inventory and `NOWALK` exclusion,
registration, sector seams, stamp priority, holes, stair gradients, radius bounds,
fixed ticks, and fast diagonal collision. The optional `walkmap_reference` test
compiles the sibling C++ radius function in a small harness using `CXX` or `c++`
and compares 630 probes, including cell edges, holes, and radius clamp/bypass
boundaries. The native function is compiled unchanged against a synthetic height
field and the rounded distance table. The retail walk test checks a move followed
by a stop at Misthaven terrain.
Motion tests cover all record layouts and state-table versions, malformed offsets,
signed vertical values, frame bounds, disabled motion, fractional accumulation,
zero-distance frames, repeated clips, and idle/run changes. The optional
`motion_reference` test compiles the sibling C++ motion structs, conversion code,
`GetMotion`, facing tables and `ConvertToVector`. It compares 12,871 samples/bounds
from Locke, Jason and ShortSword on the local install, plus all 256 facing vectors.
Legacy fixtures check separate object tracks, changing signed parents, missing
and zero-length states, original motion, fixed-array limits, invalid face indices
and truncated keys. The opt-in legacy tests build first/middle/last poses for
every old body and check Irisdoor's moving disc against its fixed surrounding
part. The OBJ/PNG comparison includes seven original-format meshes as well as
the modern character/equipment references.
Attachment tests cover variant names, state ordering, hidden/unmatched parts,
weapon/bow selection, shared-asset isolation, body restoration, bounds, alpha,
depth and picking. The retail equipment test exercises five armor meshes with
attachment-only parent metadata and ShortSword at the first/middle/last frames
of all 352 Locke clips.
HUD validation renders all six pane types at 640×480, 1024×768 and 320×240,
checks equipped slot inspection, scaling, and capture cancellation. UI asset
validation compiles the sibling native bitmap decoder and compares decoded
pixels/registration and font metrics. Audio validation compares all 1,057 retail
WAVs (38,447,664 samples) and optionally opens the native device while muted.

## Next port stages

1. Extend authored ground motion into full character actions, turning and state
   transitions and inventory-driven equipment,
   following `charanim.cpp` and `animsystem.cpp`.
2. Extend the terrain movement foundation with sector streaming, changing door
   states, character collision, gravity, and vertical motion from character actions.
3. Port stat definitions/defaults, character actions, combat, and inventory as
   focused components and systems instead of extending an object superclass.
4. Port the scripting VM and save-game lifecycle with explicit world access and
   versioned serialization. Preserve unsupported fields until their codecs exist.
5. Connect authored effect controllers, animated I2D playback, area lighting
   and animation tags to the GPU primitives; add scene streaming.
6. Connect the UI and audio foundation to gameplay state, inventory, actions,
   spell casting, dialogue, music and menus.

For each subsystem, keep the C++ source as the behavioral reference and add
small compatibility fixtures around meaningful boundaries. Structural Rust
changes should not silently change gameplay rules.
