# Options pane settings — forensics

What each control of the retail Options pane (`options.def`, panel
`default`) reads and writes, when a change takes effect, what Cancel
restores, and how the `[Options]` INI section is read and written. The
pane's hosting (in-game menu, title Options screen, DEF flags) is in
[INGAME_MENU.md](INGAME_MENU.md) §7–8; the widget layout is in
`docs/ui/forensics/OptionsDef_SPEC.md`.

Sources: Ghidra decomps of the retail `Revenant.exe` (addresses below),
`.data` defaults read with `peread.py`, the GOG install's `Revenant.ini`,
the manual (`docs/MANUAL.md`, "Options").

## 1. The pane

`cls_0x5b9744`, one instance at `0x0066fcc0`, shared by the in-game menu
and the title Options screen. `+0x17c` is `fromGame`. Open `0x0053a8b0`
copies the setting globals into members; the controls edit the members;
OK copies them back.

| Control (`options.def`) | Member | Global | INI key (`[Options]`) | `.data` default | Range |
|---|---|---|---|---|---|
| `Realtime` "Enable Real Time Lights" | `+0x180` | `DAT_005d7a18` | `RealTimeLight` | 1 | toggle |
| `Auto` "Automatically Begin Combat" | `+0x184` | `DAT_005d7a68` | `AutoCombat` | 1 | toggle |
| `Dialog` "Play Audio Dialog" | `+0x188` | `DAT_005d7a60` | `PlaySpeech` | 1 | toggle |
| `Face` "Always Face Enemy In Combat" | `+0x18c` | `DAT_005d7a64` | `CombatFace` | 1 | toggle |
| `Enhanced` "Use Enhanced 3D Lighting" | `+0x190` | `DAT_005e91c0` | `EnhancedLighting` | 0 | toggle |
| `Limit` "Limit Game Speed" | `+0x194` | `!DAT_006682a8` | none (never saved) | `DAT_006682a8` = 0, so on | toggle |
| `NoCombatRes` "No Combat Results" | `+0x198` | `DAT_00668194` | `NoCombatResults` | 0 | toggle |
| `Violence` | `+0x19c` | `DAT_005d79e8` | `Violence` | 5 | slider 0..4, page 1 |
| `Music` | `+0x1a0` | `DAT_005d7a9c` | `MusicVolume` | 127 | slider 0..`0x60`, page `0xc` |
| `Sound` | `+0x1a4` | `DAT_005d7aa0` | `EffectsVolume` | 127 | slider 0..`0x7f`, page `0xf` |
| `Gamma` | `+0x1a8` | `DAT_005d7a48` | `GammaLevel` | 3 | slider 0..4, page 1 |
| `controller` list | `+0x1ac` | the control map | `[Controls]` | — | rebind buffer |

The GOG `Revenant.ini` ships `Violence = 4`, `GammaLevel = 4`,
`MusicVolume = 96`, `EffectsVolume = 127`, `EnhancedLighting = Yes`,
`RealTimeLight = Yes`, `CombatFace = Yes`, `AutoCombat = Yes`,
`PlaySpeech = Yes`, `ShowDialog = Yes`, `NoCombatResults = No`,
`DoubleTapTicks = 10` (and an unused `ViolenceLevel = 4`, which no code
reads).

## 2. Open — `0x0053a8b0`

`DefScreen_Open("options", "default", fromGame ? 0x11 : 0, …)`, then the
member copies in the table above (`Limit` = `DAT_006682a8 == 0`). The
rebind buffer `+0x1ac` is freed if left over and reallocated at
`NumControls * 0x24` bytes: each control's three codes × three keys
(`0x00439060`, the flags are not copied). `0x00437620` then parses the
DEF and sends control event 1.

## 3. Event 1 — `0x0053aa90`

- Each toggle's on state (`BTNFLAG_DOWN` `0x10000`) is set from its
  member (and `0x20`, redraw). No toggle event is sent.
- `Enhanced` is disabled (flag 4) when the device reports neither
  MODULATE2X nor MODULATE4X (`DAT_006697bc`, `DAT_006697c0`) or
  `DAT_006680f4` is set; enabled otherwise.
- Each slider: SetRange `0x0042e3d0(0, max)`, page `+0xa0`, then
  SetValue `0x0042e440(member)`. SetValue clamps into the range and,
  **when the value changed**, sends event 4000 (`0x0042a820`, synchronous,
  through the pane's control handler). A fresh slider holds 0, so every
  nonzero member fires 4000 during event 1:
  - `Violence` 5 (the `.data` default) clamps to 4 and the member becomes
    4. OK then saves 4.
  - `Music` applies at once (§4): opening the pane sets the CD volume
    from `MusicVolume` while the CD plays (127 clamps to `0x60`).
  - `Gamma` applies its ramp at once (§4).
- The controller list is sized from the control count
  (`0x00430c50(NumControls * 2)`), and the `name0..7` labels take the
  names of the controls at its scroll position.

## 4. Changes while the pane is open

| Event | Control | Effect |
|---|---|---|
| `0xbb9` toggle on / `0xbba` toggle off | the seven toggles, by name | member = 1 / 0 |
| 4000 slider changed | `Violence` | member only |
| | `Music` | member, and CD SetVolume `0x0049a5c0(value)` **live** |
| | `Sound` | member only |
| | `Gamma` | member, and the gamma ramp `0x004a98f0(value)` **live** |
| `0x1389` / `0x138a` | `controller` list | the rebind path (key captured into the buffer) |

## 5. OK — event 3000 `ok`

In order (`0x0053af35`–`0x0053afcd`): `RealTimeLight`, `PlaySpeech`,
`AutoCombat`, `EnhancedLighting`, `NoCombatResults`, `Violence`,
`MusicVolume` from the members; `DAT_006682a8 = !Limit`; CD SetVolume
`0x0049a5c0(MusicVolume)`; `EffectsVolume`, `GammaLevel`; the ramp
`0x004a98f0(GammaLevel)`; `TMapPane::SetAmbientLight(MapPane.ambient, 1)`
(`0x00453640`, §7.11); `CombatFace`; SaveOptions `0x00484ed0` (§6.2).
Then the rebind buffer goes back into the control map (`0x00439060` /
`0x00439110`, keys only) and `[Controls]` is saved (`0x00439dc0`). From
the game the pane closes (back to the menu); from the title the next
screen is the title.

## 6. Cancel, and the INI

### 6.1 Cancel

`cancel` re-applies the gamma ramp from `GammaLevel` (`0x0053b0c7`) — that
is all it restores. The members are dropped, so no global changes, and the
rebind buffer is freed unused by Close `0x0053aa60`. **The music is not
restored**: a `Music` drag sets the CD volume live (§4) and Cancel leaves
it there, while `MusicVolume` keeps the old value. The next open sets the
CD back from `MusicVolume` (§3). `ControlSetup` (not in `options.def`) is
the multiplayer path.

### 6.2 `[Options]` reader `0x00484ae0` and writer `0x00484ed0`

The reader runs at boot from `0x004865a0`: GetINISettings `0x00484500`
(`0x0048675f`), the reader (`0x00486764`), then GetParameters
`0x00483bc0` (`0x0048676c`), so the command line wins. Keys, in order:
`DoubleTapTicks`, `ZoomSpeed`, `VertPanSpeed`, `HorzPanSpeed` (ints);
`ShowDialog` (`0x00668188`), `PlaySpeech`, `CombatFace`, `AutoCombat`,
`Software3D` (`0x0066818c`), `ShortEditorDisplay` (Yes/No); `Violence`,
`GammaLevel`, `MusicVolume`, `EffectsVolume` (ints); `EnhancedLighting`,
`RealTimeLight`, `NoTexOverlay` (`0x006680c8`), `SquishyScroll`
(`0x005d7a40`), `NoCombatResults`, `NoEAX`, `No3DAudio`,
`Cache3DImagery`, `BeepOnChat`, `VidMemZBuffer`, `WaitMapUpdate`
(Yes/No); `LastEditedModule`, `LastOptions` (strings). Every read writes
the value back, so a missing key is added with its default.

- Ints: `GetPrivateProfileInt` with the global as the default, written
  back with `"%d"` (`0x005d7c48`). No clamping.
- Yes/No (`0x00482840`): the value lowercased and matched against the
  tokens `"Yes"` / `"No"` (`0x005d7c94` / `0x005d7c90`, lowercased), else
  the default; written back as `Yes` / `No` (`0x004829c0`).

The writer is called by OK (`0x0053afcd`), at shutdown after the main loop
(`0x004870ee`), and by the chat pane's BeepOnChat toggle (`0x00464fc0`).
It writes, in order: `DoubleTapTicks`, `ZoomSpeed`, `VertPanSpeed`,
`HorzPanSpeed`, `ShowDialog`, `PlaySpeech`, `CombatFace`, `AutoCombat`,
`Software3D`, `Violence`, `GammaLevel`, `MusicVolume`, `EffectsVolume`,
`EnhancedLighting`, `RealTimeLight`, `SquishyScroll`, `NoCombatResults`,
`BeepOnChat`, `LastOptions` (quoted), then `[DedicatedServer]`
`Autosave`, `AutosaveTime`, `LogEvents`, `EventsLogFile`. Command-line
overrides and debug-key toggles held in these globals are saved too.

### 6.3 `[Controls]`

The control map is built from the table at `0x005d5500` (69 entries)
and then read from `[Controls]` at boot: Initialize `0x00438f50`
(`0x00486177`), Load `0x00439ba0` (`0x00486186`), in `0x00485870`. It is
saved by the Options pane's OK (`0x0053b061`) and at shutdown
(`0x004864f4`, in `0x004863b0`, just before SaveOptions). Each value is
the control's codes separated by commas, each code its keys joined with
`-` (`CTRL-A`, `JOY7-JOY4`); an empty value is unbound.

## 7. What each setting does

1. **RealTimeLight** — picks the tile-lighting path (light grid when on,
   DLS when off): [../../LIGHTING_FIDELITY.md](../../LIGHTING_FIDELITY.md)
   §1. Also set by the command line (`0x00483c75`) and a debug toggle
   (`0x00448cc0`, case `0x52`).
2. **AutoCombat** — read by `0x004c18a0` (character reset) and
   `0x004c1bb0`: combat begins by itself when an enemy is in range and
   faced.
3. **PlaySpeech** — the voice of `TCharacter::Say` `0x004d0610`.
4. **CombatFace** — `0x004c7980` (combat move resolution) and `0x004ce350`
   (walk), for the player's character (class `0xb`): it keeps turning to
   face its fighting target.
5. **EnhancedLighting** — MODULATE2X/4X overbright on 3D objects
   (LIGHTING_FIDELITY §3.1). Debug toggle `0x00448cc0`, case `0x59`.
6. **Limit Game Speed** — TimerLoop `0x004911b0` waits for the 24 Hz tick
   unless `DAT_006682a8` is set. A debug toggle (`0x00448cc0`, case
   `0x54`) flips it; an editor drag sets it for the drag (`0x0040e110` /
   `0x0040e210`). Never saved: every run starts limited.
7. **NoCombatResults** — hit resolution `0x004c62b0` prints the
   `FULLCOMBATRES` / `BASEOFF` / `BASEDEF` / `BASEDMG` lines when the main
   player is involved and this is off.
8. **Violence** — blood and gore (`0x004f1a20`, `0x004f1d84`; the port's
   `effectcomp.cpp`): 0 none, the particle count and size clamp to it,
   `< 4` and `< 3` drop more. Also set from the command line
   (`0x00483ef6`). The slider stops at 4, so 5 is reachable only through
   the INI or the command line.
9. **MusicVolume** — the CD (redbook) music. CD object `0x0065abc8`:
   `+0` redbook handle, `+4` base volume, `+8` playing, `+0xc` current.
   Init `0x0049a270` reads the device's own volume into current and sets
   the base to `0x60`. SetVolume `0x0049a5c0` clamps to 0..`0x60` and,
   while the CD plays, sets the device (`AIL_redbook_set_volume`) and
   takes the readback as base and current. `setcdvolume half|full`
   (`0x00428b20` → `0x0049a610`) sets current to base/2 or base; the
   fade `0x0049a380` steps current toward base. **`MusicVolume` is never
   applied at boot**: it reaches the CD when the Options pane opens (the
   slider's first SetValue, §3), on a slider drag, and on OK. Until then
   the CD keeps the device's volume — the Windows CD-audio mixer level,
   which outlives the process, so on a 1999 machine the last level the
   player set normally carried over — and the base `setcdvolume` scales
   is `0x60`.
10. **EffectsVolume** — every sample's Miles volume is
    `max(0, EffectsVolume - 0x7f + volume)` (`0x0049b990` 2D and 3D,
    `0x0049c760`); movies play at `SmackVolumePan(EffectsVolume * 255)`
    (`0x004bc470`). Read at each play, so a change applies to the next
    sound.
11. **GammaLevel** — two effects:
    - The display gamma ramp `0x004a98f0`: the primary surface's
      `IDirectDrawGammaControl::SetGammaRamp` with one of five tables
      (`0x005dcd5c` + `0x600·level`; levels outside 0..4 use level 2).
      Also set at display init `0x004a91a0`. No effect when the device has
      no gamma control (the manual: "will not work with all video
      cards"). The ramps darken the midtones: input ½ maps to 0.15, 0.20,
      0.29, 0.34, 0.46 of full for levels 0–4 (power curves of about
      2.7, 2.3, 1.8, 1.6, 1.1).
    - The map ambient. `TMapPane::SetAmbientLight` `0x00453640` stores
      `light + (GammaLevel * 5 - 10) * 2` (floored at 0); map init
      `0x0044d5c0` inlines it with light 10 (`GammaLevel * 10 - 10`). So
      level 2 adds nothing, 3 adds 10, 4 adds 20. Area entry
      (`0x0041ba00`), the day/night update (`0x0041b770`) and the script's
      `ambient` command (`0x0042596d`) all go through it; FadeAmbient
      `0x00453720`, the cross-fade between nearby areas, doesn't add it. OK
      calls SetAmbientLight on `MapPane.ambient` (`+0x8cc`, `0x006671a4`),
      which already holds the offset, so each OK adds the offset again
      until the next area ambient change.
    - What the shipped game shows, from the dosbox-x captures
      (LIGHTING_FIDELITY.md §8): the ambient offset is in effect (the Keep
      renders at ambient 14 at level 3), and the ramp never is. In the
      capture session the pane opened several times (the `o` of a cheat
      word typed while the prompt was closed) and each time it closed the
      map got brighter while the HUD's pixels stayed identical. A ramp
      would change the HUD too; only OK's `SetAmbientLight` touches the map
      (Cancel re-applies the ramp alone, and the pane's close
      `0x00435010` only fades it), so the pane closed through OK, and the
      brightening is question 93 at work.

## 8. Scrollbar behaviour (DEF engine)

SetRange `0x0042e3d0` clamps the value into the new range without an
event. SetValue `0x0042e440` clamps, moves the thumb and sends 4000 when
the value changed. A click on the track pages toward the cursor by the
page size (`+0xa0`; `0x0042f16d`–`0x0042f1fa`, with a click sound); a
release on an arrow steps by 1 (`0x0042f249`); the thumb drags
(`0x0042f2e0`).

## 9. Port (2026-10-05)

The settings stay globals, as in retail. The ones the port already had
keep their homes (`AutoBeginCombat`, `PlaySpeech`, `ShowDialog`,
`ViolenceLevel`, `DoubleTapTicks` in `revmain.cpp`, `EnhancedLighting` in
`3dscene.cpp`, all declared in `revenant.h`); the ones the pane brought
(`MusicVolume`, `EffectsVolume`, `GammaLevel`, `RealTimeLight`,
`CombatFace`, `NoCombatResults`, `NoGameSpeedLimit`) and the `[Options]`
reader and writer are in `src/gameoptions.{h,cpp}`. They go through the
existing INI layer (`revutils.cpp`: the user's INI over the install's) in
retail's keys and format.

| Retail | Port |
|---|---|
| reader `0x00484ae0` | `ReadOptions()`, called by `AppInit` after `GetINISettings()` and before `GetParameters()`. Reads the keys that have a port owner: `DoubleTapTicks`, `ShowDialog`, `PlaySpeech`, `CombatFace`, `AutoCombat`, `Violence`, `GammaLevel`, `MusicVolume`, `EffectsVolume`, `EnhancedLighting`, `RealTimeLight`, `NoCombatResults` (`GetINISettings` no longer reads `DoubleTapTicks` / `EnhancedLighting` itself). `ShowDialog` is new to the port: the shipped INI's `Yes` now reaches the say action's text (DIALOG.md, step 5) |
| writer `0x00484ed0` | `SaveOptions()`: the same keys, `%d` and `Yes`/`No`; called by OK and at shutdown (`AppCleanup`) |
| pane members, Open `0x0053a8b0` | `SOptionsPaneValues`, copied from the globals by `TOptionsPane::OpenOptions` |
| event 1 | `TOptionsPane::OnOpened`: toggles from the copies; `SetSliderRange` + `SetSliderValue` (clamp, change event) |
| toggle events `0xbb9` / `0xbba` | `TOptionsPane::OnActivate` on a toggle: its copy = the toggle's state |
| slider event 4000 | `TDefPane::OnSliderChanged` (new), raised by `SetSliderValue` and by the player's arrow / track / drag changes; `Music` applies live |
| OK | `TOptionsPane::Apply`: globals, music and effects volume, `SaveOptions()`, rebinds committed, `[Controls]` saved; then the host closes / returns to the title |
| Cancel | the copies and the rebind buffer are dropped; the music stays where a drag left it, as retail |
| rebind buffer `+0x1ac` | `TOptionsPane::bindings` (each control's keys); rebinding edits the buffer, OK commits it. Before this the port wrote rebinds straight into the live control map, so Cancel kept them |
| control map Load `0x00439ba0` at boot | `InitDefaultControlMap()` reads `[Controls]` after building the table (it never did, so OK used to overwrite the player's `[Controls]` with the port's table). The port's table (from the 1998 source) isn't retail's `0x005d5500`: its order, several defaults and some controls differ. A control the INI names gets the INI's keys; the others keep the table's, and the port-only ones are added to `[Controls]` on first read |
| CD SetVolume `0x0049a5c0` | `ApplyMusicVolume(level)` (`sound.cpp`): 0..`0x60`, the music group's gain `level / 0x60` |
| SetAmbientLight `0x00453640` | `TMapPane::SetAmbientLight` adds `GammaAmbientOffset(GammaLevel)` (`gameoptions.h`), floored at 0; FadeAmbient doesn't (2026-10-07) |
| OK's `SetAmbientLight(MapPane.ambient, 1)` `0x0053afbc` | `TOptionsPane::Apply` re-sets `MapPane`'s ambient, adding the offset again as retail (question 93) |
| per-sample `EffectsVolume` | `ApplyEffectsVolume(level)`: the sfx group's gain `level / 0x7f`; game sounds and movie audio both play through the sfx group |

Live in the port: Auto, Dialog, Enhanced (the light model reads it every
frame), Violence (read at each blood effect), Music, Sound.

**Deviations**

- `MusicVolume` and `EffectsVolume` are applied at boot. Retail never
  applied `MusicVolume` at boot (§7.9); its level persisted through the
  OS CD mixer, which the port doesn't have, so the INI value is the
  persistence. The audio backend keeps a group volume set before the
  device opens and applies it when it does.
- `setcdvolume half` is half of the player's level; retail before the
  first Options open halved `0x60` instead. Same result at the shipped
  `MusicVolume = 96`.
- Effects volume is a gain on the sfx group (`level / 0x7f`), not
  retail's per-sample subtraction in Miles units: the same for a
  full-volume sound, a little louder for quiet ones at low settings.
- Gamma reaches the screen through the map ambient only (§7.11): the
  port has no display gamma ramp, in Classic or Revisited. The retail
  captures are the reference, and in them the ramp has no effect: the HUD
  is pixel-identical at boot and after the pane re-applied it, so the
  dosbox-x display driver ignores `SetGammaRamp`. On a
  1999 card that honoured it, the ramp would darken the midtones at every
  level below 4 (½ → 0.15–0.34) and leave level 4 near linear; the
  shipped INI's level 4 would look close to the captures either way.
  Whether the ramp should come back as a Revisited option is question 92.
- Real Time Lights is stored, shown and saved but inert: Classic always
  renders retail's `RealTimeLight=No` image (LIGHTING_FIDELITY §1).
- Always Face Enemy and No Combat Results are stored, shown and saved;
  the combat code doesn't read them yet (`docs/gameplay/BURNDOWN.md`).
- Limit Game Speed is shown and kept for the session, never saved
  (retail too); the port's simulation always runs on the 24 Hz tick, so
  it has no effect.
- `Enhanced` is never disabled: every device the port runs on has the
  overbright modes.
- The DEF slider's track click jumps to the cursor and drags; retail
  pages by the slider's page size (§8). The DEF engine's slider input is
  unchanged here.
- `[Controls]` isn't saved at shutdown: only OK changes the port's
  control map, and it saves.
- Keys the port has no owner for (`ZoomSpeed`, `VertPanSpeed`,
  `HorzPanSpeed`, `Software3D`, `ShortEditorDisplay`, `NoTexOverlay`,
  `SquishyScroll`, `NoEAX`, `No3DAudio`, `Cache3DImagery`, `BeepOnChat`,
  `VidMemZBuffer`, `WaitMapUpdate`, `LastEditedModule`, `LastOptions`,
  `[DedicatedServer]`) are neither read nor written; the INI layer keeps
  whatever the file holds, so a retail-written INI round-trips them.

Revisited-only settings would extend this pane (a Revisited `options.def`
in the overlay) or the `[Revisited]` section (`SRevisitedSettings`); they
don't belong in `[Options]`, which retail reads.

## 10. Open questions

[../AUTHOR_QUESTIONS.md](../AUTHOR_QUESTIONS.md) §"Options" (90–), and 16
(settled here, §7.9).

## 11. Decomps used

`0x0053a8b0`, `0x0053aa60`, `0x0053aa90`, `0x00437620`, `0x0042a820`,
`0x0042e3d0`, `0x0042e440`, `0x0042e580`, `0x0042f2e0` and the
un-functioned click handler around `0x0042f16d`, `0x00484500`,
`0x00484ae0`, `0x00484ed0`, `0x00482840`, `0x004829c0`, `0x00482330`,
`0x004865a0` (call sites), `0x00464fc0`, `0x0049a270`, `0x0049a380`,
`0x0049a560`, `0x0049a5c0`, `0x0049a610`, `0x00428b20`, `0x0049b990`,
`0x0049c760`, `0x004bc470`, `0x004a98f0`, `0x004a91a0`, `0x00453640`,
`0x00453720`, `0x0044d5c0`, `0x00448cc0`, `0x0040e110`, `0x0040e210`,
`0x004911b0`, `0x004c62b0`, `0x004c7980`, `0x004ce350`, `0x004d0610`,
`0x00439ba0`, `0x00439dc0` and their call sites (`0x00486177`,
`0x00486186`, `0x004864f4`); cross-references to every global in §1; the
gamma tables read from the exe.
