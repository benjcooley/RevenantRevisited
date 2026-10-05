# Data layout: install, user data, overlay, engine assets

Where the engine's files live, how a file name becomes bytes, and what is
in the repo's `data/` tree. Companion to the root [README](../README.md)
("Where things live at runtime", "Distribution model") and
[revisited/README.md](../revisited/README.md) (the overlay contract).

Retail fidelity: §2 is **retail-confirmed** from the Ghidra decomp of the
GOG `Revenant.exe` (v1.22, MD5 `e9a42fe6baef4e7bbf2ac610d7a3e313`, the
binary in `data/`) unless marked otherwise.

## 1. Four roots

| Root | What | Writable | Found by |
|---|---|---|---|
| **Install** (`RunPath`) | The player's stock Revenant install: `resources.rvr`, `imagery.rvi`, `Modules/*.rvm`, loose patch files. The repo's `data/` stands in for it in dev. | never | `$REVENANT_DATA_PATH`, else cwd / exe-relative probes (`rev_resolve_program_paths`) |
| **User data** (`SavePath`) | `Revenant.ini`, `curmap/`, `Save/`, `cache/` | yes, the only writable root | `$REVENANT_SAVE_PATH`, else Application Support / XDG / LOCALAPPDATA |
| **Overlay** | Revisited enhancements; mirrors the install tree. Opt-in (`--revisited`). | no | `rev_resolve_revisited_overlay()` |
| **Engine assets** | Data the port itself authors or supplies and needs in every mode: `effects.def`, `render_metadata.def`, `editor/icons/`, `fonts/` (the TrueType faces: Arimo and Tinos draw retail's Arial and Times New Roman, plus the editor's). Repo: `assets/`. | no | `rev_engine_asset()`: `$REVENANT_ASSETS_PATH`, `<exe-dir>/assets`, `<exe-dir>/../Resources/assets`, `<repo>/assets` |

The install is the player's property and stays stock: nothing the port
authors goes into it, and nothing writes to it. The engine must run on a
stock install with no overlay (revisited/README.md, "Hard invariant"),
which is why engine-required data cannot live in the overlay either.

## 2. Retail path behavior

### 2.1 The `[Paths]` section — `GetINISettings @ 0x00484500`

Each value is read with the quoted-text getter (`FUN_00482330`) and gets a
trailing `\`.

| Key | Retail default | Used for |
|---|---|---|
| `ExileRCPath` | `.` | editor resource compiler |
| `ClassDefPath` | `.\Resources` | shared game data (`rules.def`, `spell.def`, scripts and module-file fallbacks) |
| `ResourcePath` | `.\Resources` | resources and DEF screens; **also names the resource pack** (§2.3) |
| `ImageryPath` | `.\Imagery` | class / roster defs; **names the imagery pack** |
| `CurMapPath` | `.` | sector working set |
| `BaseMapPath` | `.` | pristine sectors (overwritten by `SetCurModule`) |
| `MoviePath` | `.\Resources\FMV` | movies; the value `[CDROM]` triggers a CD-drive search (`FUN_00484400`) |
| `SaveGamePath` | `.\Save` | save slots |
| `ModulesPath` | `.\Modules` | modules; **names each module pack** |

`[Modules] MainModule` (default `Ahkuilon`) and `[Language] Language`
(default `English`) are read with the same quoted-text getter. The GOG
`revenant.ini` carries the retail defaults, with `MoviePath = ".\Disk2"`.

### 2.2 `makepath @ 0x00483120` and the pack-aware open

- `makepath(name)`: an absolute name (`\…`, `X:…`, `..…`) is kept; otherwise
  a leading `.` and the separators after it are stripped and the rest is
  appended to **SavePath**. On a hard-drive install SavePath is the install
  directory.
- `FUN_004a13f0(name, mode, looseFirst)` is retail's `rev_fopen`: it tries
  `SavePath + name`, then `RunPath + name` when the two differ, each through
  `FUN_004a1240`.
- `FUN_004a1240(path, mode, looseFirst)`: if `looseFirst`, `fopen` the loose
  file first; otherwise ask every mounted pack (`FUN_004a0380`), and only if
  none answers `fopen` the loose file. So a pack **shadows a loose file at
  the same path** unless the caller asks for loose-first.
- `FUN_004a1c00` / `FUN_004a1c30` is the matching "exists" test: makepath,
  packs, then `stat`.

### 2.3 Packs stand in for directories

- WinMain (`FUN_004865a0`) mounts `makepath(ResourcePath)` minus its trailing
  `\` plus `.rvr`, so `.\Resources` → `<install>\Resources.rvr`, and
  `makepath(ImageryPath)` + `.rvi` → `<install>\Imagery.rvi`. A missing pack
  (error 2) is tolerated; a pack holding compressed entries is fatal.
- `SetCurModule @ 0x004609f0` mounts `makepath(ModulesPath) + <module> +
  .rvm`, then sets `BaseMapPath` under the module.
- `TPackFile::Open @ 0x0049ee20` stores the pack's directory and file name.
  The lookup `FUN_004a0380` matches a request case-insensitively against
  `<directory><file name up to '.'>\`, then binary-searches the remainder
  (sub-directory + name) among the pack's entries. So `Resources.rvr`
  answers `<install>\Resources\rules.def` (entry `rules.def`) and
  `<install>\Resources\Sound\…` (entry `Sound\…`), and nothing outside
  that directory. There is no lookup by bare file name.

The packs' contents are rooted accordingly:

| Pack | Answers for | Root entries |
|---|---|---|
| `resources.rvr` | `Resources\` | `*.dat` UI art, `rules.def`, `spell.def`, `font.def`, `master.s`, `state.def`, `exit.def`, `english.def`, DEF screens, …; `Sound\` |
| `imagery.rvi` | `Imagery\` | `class.def`, `char.def`, `weapon.def`, `armor.def`, `quickload.dat`; `Imagery\…`, `Thumbnails\…` |
| `Modules\<m>.rvm` | `Modules\<m>\` | `module.def`, `area.def`, `exit.def`, `location.def`, `state.def`, `english.def`, `newgame.sav`, `*.s`; `Map\`, `Automaps\`, `Sound\` |

### 2.4 Which path each loader asks for

| File | Retail composition | Order | On a stock install, read from |
|---|---|---|---|
| `rules.def` | `ClassDefPath` (`TRules::Load @ 0x0048b990`) | pack first | `resources.rvr` |
| `stats.def` | `ClassDefPath`, if it exists | pack first | `resources.rvr` |
| `char.def`, `weapon.def`, `armor.def`, `equip.def` | `ImageryPath` if it exists, else `ClassDefPath` | pack first | `imagery.rvi` (`equip.def`: none) |
| `class.def` | `ImageryPath` if it exists, else `ClassDefPath`; per module `ModulesPath\<m>\class.def` if it exists (`LoadClasses @ 0x00476140`) | pack first | `imagery.rvi` |
| `spell.def` | `ClassDefPath` (`0x0053ead0`) | pack first | `resources.rvr` |
| `statpane.def` | `ResourcePath` (`0x00546b50`) | pack first | `resources.rvr` |
| `font.def` | `ResourcePath` | pack first | `resources.rvr` |
| `area.def`, `exit.def`, `location.def` | `ModulesPath\<m>\` if it exists, else `ClassDefPath` (`0x0041c000`, `0x0050c8f0`, `0x00424e10`) | pack first | the module |
| `state.def` | same (`TGameState::Load @ 0x00495cf0`) | pack first | the module |
| scripts (`master.s`, `keep.s`, …) | `ModulesPath\<m>\` if it opens, else `ClassDefPath` (`TScriptManager::Load @ 0x00496490`) | pack first | the module; `master.s` from `resources.rvr` (Ahkuilon has none) |
| dialog | `ModulesPath\<m>\<Language>dialog.def`, `…\<Language>.def`, `…\english.def` (`0x0049d2a0`) | pack first | the module's `english.def` |
| `widgets.def`, `<screen>.def` | `ResourcePath` (`FUN_004377c0`, from `DefScreen_Open @ 0x00435040`) | **loose first** | loose `Resources\options.def`, `joingame.def`, `mpingame.def`; the rest from `resources.rvr` |

Consequence for the GOG install's loose files: the loose
`Resources\{rules,spell,statpane}.def`, `Resources\master.s` and
`Imagery\char.def` differ from the packed copies (they look like 1.22-era
revisions: different class modifiers, a spell light colour, `USER.*` in
`master.s`), but retail never reads them: the packs answer first. Only
the DEF-screen loader asks loose-first, so only `options.def` ("No Combat
Results" toggle), `joingame.def` and `mpingame.def` take effect.

## 3. How the port resolves files

`rev_fopen(name, flags, order = EOpenOrder::PackFirst)` in
`src/revutils.cpp`:

1. Absolute names are opened as given (packs, then loose).
2. Writes go to `SavePath + name` only.
3. Reads walk the roots **SavePath → overlay → RunPath**; under each, the
   pack whose directory holds `root + name`, then the loose file (reversed
   for `EOpenOrder::LooseFirst`). This is `FUN_004a13f0` / `FUN_004a1240`.
4. Legacy fallbacks: an unpacked pre-release module directory by file name,
   a `data/`-relative path under the data root, then a lookup by bare file
   name across the mounted packs (module first).

`rev_file_exists` is the strict exists test (no step 4), and
`rev_first_existing(preferred, fallback, file)` the either/or lookup the
loaders use. `rev_find_files(dir, ext)` lists a directory the same way
(retail's pack-aware findfirst `FUN_004a19d0`): under each root, the pack
whose directory holds it if any entry matches, else the loose directory.
The sound list is built with it. `TModuleManager::DataFilePath(file)` is the module-or-shared
rule of §2.4; `ModuleFilePath(file)` is the module's own path.
`GetINISettings` uses retail's defaults and reads `ImageryPath` and
`ModulesPath`; the loaders in §2.4 compose retail's paths (REVSYNC tags
at each).

Deliberate divergences (`// REVSYNC-DIVERGENCE:` in `revutils.cpp`):

- **SavePath is loose-first.** Retail asks the packs first within SavePath
  too, because its packs live there on a hard-drive install. The port's
  packs live under RunPath, so a loose file in SavePath (editor saves, user
  data) is found before the install's packs.
- **By-name fallback.** Retail has none. The port keeps it as the last
  resort because older port INIs (`ClassDefPath = "."`, written by builds
  that defaulted to `.`) and call sites not yet moved to retail paths name
  files outside any pack directory. Both INI flavours now resolve every
  data file to the same copy (§6). `rev_find_files` has the directory
  counterpart: a directory no root answers is looked up as a path inside
  the base packs (not the module's), so `.\sound\effects\` under an old
  INI lists `resources.rvr`'s `Sound/effects/`.
- **Fixed pack names.** Retail derives the pack file names from the INI
  values (`ResourcePath` → `Resources.rvr`); the port mounts the stock
  names `resources.rvr`, `imagery.rvi` and `Modules/<m>.rvm` under RunPath.
  They differ only for non-default INI values.

The overlay mirrors the install tree, so an override of a module file
lives at the module's path: `revisited/resources/Modules/Ahkuilon/area.def`.

## 4. Inventory of `data/`

`data/` is a git-LFS copy of the author's GOG install (imported in
`5b0d1b5` as `RevenantBin/`, moved to `data/` in `bf5d840`). Evidence used:
the GOG Galaxy manifest `goggame-galaxyFileList.ini` (the install's file
list), GOG's integrity DB `goggame-1207665803.hashdb` (MD5s for 171 files),
`git log` per file, and comparison with the packed archives.

### 4.1 Stock

Everything named in the Galaxy manifest: the executables and DLLs it
lists, `imagery.rvi`, `resources.rvr`, `Disk2/`, `EULA/`, `Music/`,
`Imagery/char.def`, `Modules/*.rvm`, `Modules/Demo/` (323 files, an
unpacked module), `Modules/Deathmatch6/area.def`,
`Resources/{joingame,master.s,mpingame,options,rules,spell,statpane}`,
`revenant.ini` (installed from GOG's support folder), and the text files.
All 20 hash-DB entries present in the tree match: `resources.rvr`, the 7
`.rvm`, `Launcher.exe`, `Manual.pdf`, `cmdline.txt`, `control.txt`, two
`Disk2` movies and `Resources\{joingame,mpingame,options,rules,spell,statpane}.def`.

### 4.2 Port-modified — restored (commit `data: restore …`)

| File | What the port did | Now |
|---|---|---|
| `Resources/rules.def` | `b1bd2be` replaced it with a merge of the legacy pre-release file; with the retail INI the port read it and died at line 1002 | stock bytes, MD5 `99dd4124…` = hash DB; merged copy in history at `ec46f52` |
| `revenant.ini` | an old port build rewrote it through simpleini (spacing, LF); committed in `5e93976` | stock bytes from the import |

### 4.3 Port-authored — moved to `assets/` (commit `assets: …`)

| Was | Now | Introduced |
|---|---|---|
| `Resources/effects.def` | `assets/effects.def` | `5215967` |
| `Resources/render_metadata.def` | `assets/render_metadata.def` | `a37daf6` |
| `editor/icons/**` (20 PNG, LFS) | `assets/editor/icons/**` | `526fe82` |

### 4.4 Not stock, not port data — left in place

None of these are read by the engine.

| Files | What |
|---|---|
| `D3D8.dll`, `D3DImm.dll`, `DDraw.dll`, `dgVoodooCpl.exe` | dgVoodoo2 DirectX wrapper (`revboot.log` shows retail running on it); not in the manifest |
| `dxgi.dll`, `ReShade.ini`, `ReShade.log`, `ReShadePreset.ini`, `reshade-shaders/` (143 files) | ReShade, user-installed (the log names it) |
| `Revenant.gpr`/`.rep`, `RevenantBC*.gpr`/`.rep`, `RevenantDev.gpr`/`.rep` (29 files) | Ghidra projects. `RevenantDev` is the one recon tooling copies; `Revenant.rep` was modified in `470b3fb` |
| `revboot.log`, `ss.bmp` | retail runtime output (boot log, last save thumbnail) |
| `revenant.old_ini` | an earlier `revenant.ini` (`Software3D=Yes`, `EnhancedLighting=No`); origin unknown |
| `!Downloads/*.zip` | GOG extras (concept art, manual, renders) |
| `unins000.*`, `Launch Revenant.lnk`, `gog.ico`, `goggame-*.ico`, `support.ico`, `goglog.ini`, `goggame-galaxyFileList.ini` | written by the GOG installer, so present in a stock install but not in its own file list |

### 4.5 Stock files not tracked

- `Curmap/2_*.DAT` (151 files) and `Save/{Chars,Multi,Single}`: in the
  manifest (GOG ships a pre-populated level-2 working set). Untracked in
  `6b69ce9` as runtime state; the port keeps its working set in
  `<SavePath>/curmap`.
- `__redist/` (DirectX redistributables): never imported.

### 4.6 Local extractions (gitignored)

`data/resources_unzipped/` and `data/Modules/Ahkuilon_unzipped/` exist only
in the main checkout, as hand-extracted copies for grepping. The engine
reads neither as data, but the module manager enumerates every directory
under `Modules/`, so `Ahkuilon_unzipped` shows up there as an extra module.

## 5. Engine assets (`assets/`)

Port-authored data the engine needs whether or not the overlay is present.
The fonts are here rather than in the overlay because Classic mode needs
them too: retail's WINFONT faces (Arial, Times New Roman) are drawn with
metric-compatible Arimo and Tinos, found through `TTFFilePath` (`font.h`).
Dev builds find `<repo>/assets/`; a shipped build puts `assets/` beside the
binary (or in the `.app`'s `Resources/`). Unit tests read the source tree's
copy through `REV_ASSETS_DIR`. PNGs are LFS-tracked. This is distinct from:

- the **overlay**: optional, shadows install files, mirrors the install tree;
- `<SavePath>/cache/`: regenerable conversions of the player's own data.

## 6. Checking which copy a file came from

`REVENANT_VFS_TRACE=1` logs every read as `pack`, `loose` or `by-name`:

```
mkdir -p /tmp/rev_fresh
REVENANT_VFS_TRACE=1 REVENANT_SAVE_PATH=/tmp/rev_fresh \
  ./build/Revenant --quickstart --headless --max-runtime=30 | grep '\[vfs\]'
```

With a fresh SavePath (seeded from the install's `revenant.ini`) and with
an older port INI (`ClassDefPath = "."`), the data files resolve
identically: `rules.def`, `font.def`, `master.s` from `resources.rvr`;
`class.def`, `char.def` from `imagery.rvi`; `area.def`, `english.def`,
`state.def`, `keep.s`, `newgame.sav` from `Ahkuilon.rvm`.

## 7. Open items

- Imagery (`ResourcePath + "Imagery\…"` in `resource.cpp`), sector maps
  (`.\map\…`) and `module.def` probing still resolve through the by-name
  fallback; retail's composition for these has not been traced.
- `TRules::Load` reads `rules.def` and `char.def`; retail also reads
  `stats.def`, `weapon.def`, `armor.def` and `equip.def` in that loop.
- Overlay deploy mode (`RevenantRevisited.rvr`) is resolved but never
  mounted; only the loose-folder overlay works.
- The editor's saves (`exit.def`, `class.def`, scripts) still write to
  `ClassDefPath`; retail writes `exit.def` to the module only when a loose
  module copy exists.
- `MoviePath = [CDROM]` is not handled.
