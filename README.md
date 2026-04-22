# RevenantRevisited

`RevenantRevisited` is an in-progress modernization of the original 1998 **Revenant** codebase for contemporary machines. The project keeps the original game data and gameplay intent, while replacing old platform dependencies with a modern build and rendering stack.

The current port already runs the game data, loads archived map sectors from the shipped module files, and includes an actively developed modern sector renderer with deferred lighting, shadows, ambient occlusion, and debug tooling.

## Project Goals

- Preserve the original game content and behavior wherever practical
- Make the source buildable on modern systems with CMake
- Replace obsolete DirectX-era rendering/input assumptions with modern platform code
- Expose internal systems through focused test modes so rendering and data-loading work can be verified quickly

## Current Highlights

- Cross-platform CMake-based build
- Modern GPU-backed renderer
- Deferred directional lighting
- Screen-space contact shadows
- Screen-space ambient occlusion
- Archive-backed level scanning and sector loading from the shipped Ahkuilon module
- Debug UI tabs for renderer tuning and inspection

## Screenshots

Some current captures from the modern renderer:

- [Dynamic shadows](images/revenant-dynamic-shadows.png)
- [Dynamic shadows 2](images/revenant-dynamic-shadows-2.png)
- [Lit Misthaven tower](images/revenant-lit-misthaven-tower.png)
- [Lit house scene](images/revenant-lit-house.png)
- [Dead sea monster](images/revenant-dead-seamonster.png)
- [Puppet show](images/revenant-puppet-show.png)

## Repository Layout

- `src/` — game source and current porting work
- `data/` — shipped game/module data used at runtime
- `docs/` — reverse-engineering notes, architecture notes, and subsystem docs
- `images/` — project screenshots and visual references
- `thirdparty/` — embedded dependencies used by the modern build

## Building

The project currently builds with CMake.

```bash
cmake -S . -B build
cmake --build build
```

On macOS, the project links Metal/Cocoa/AppKit frameworks through `CMakeLists.txt`.

## Running

Run the executable from the repository root so the game can find `data/` and the runtime INI/log paths land where expected:

```bash
./build/Revenant
```

You can also start directly into a save:

```bash
./build/Revenant --loadmap=SAVEFILE.SAV
```

## Test Modes

The active development workflow uses `--test=<mode>` to jump directly into focused subsystem checks.

### Basic modes

- `--test=blank` — simple animated clear color
- `--test=ticker` — logs frame/time progression
- `--test=font` — logs bitmap font diagnostics
- `--test=text` — renders bitmap font atlas text
- `--test=icon` — shows the built `SystemFont` atlas
- `--test=ttf` — exercises stb_truetype atlas rendering
- `--test=ui` — builds and previews a packed bitmap atlas from 2D imagery
- `--test=i3d` — probes a few imagery resources and logs state/header info

### Sector / map renderer mode

This is the main graphics bring-up path right now:

```bash
./build/Revenant --test=sector
```

Useful sector-renderer variants:

```bash
./build/Revenant --test=sector --sector=0_2_25
./build/Revenant --test=sector --level=0
./build/Revenant --test=sector --level=1
./build/Revenant --test=sector --level=51
```

Notes:

- `--sector=L_X_Y` centers startup on the requested sector
- `--level=L` loads every discovered sector for that level from the archived module data and picks an occupied startup area
- if a requested `--level` has no sectors, startup now fails cleanly instead of opening an empty screen
- startup logs now summarize sector count, object count, and renderable tile totals for large level loads

## Useful Files

- `docs/DEFERRED_LIGHTING.md` — current lighting/depth pipeline notes
- `docs/FILE_FORMATS.md` — data format notes, including map sectors
- `docs/PORT_PLAN.md` — broader porting roadmap
- `docs/WORLD_MATH.md` — coordinate-space and projection notes

## Status

This is an active port/recovery effort, not a finished remaster. The codebase still contains a mix of original game code, modern platform replacements, and temporary test harnesses used to verify subsystems while the port is underway.

## Authorship

Original game and source material:

- **Benjamin Cooley** and Cinematix Studios

Original game publication:

- **EIDOS Interactive** (1998)

## License

No new license text has been added to this repository yet. Treat the code and shipped game assets accordingly until an explicit license is documented.
