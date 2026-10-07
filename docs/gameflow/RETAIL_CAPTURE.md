# Retail reference captures

How the gameflow track gets shots and recordings of the shipped game to
compare the port against, and the opening's reference set. The retail
side of a comparison is always the shipped game in the dosbox-x lab, never
a memory of it.

## 1. The lab

`~/RevenantRetailLab` runs Windows 98 in dosbox-x with Revenant installed
at `C:\REVENANT`:

- the executable is `Rev98.exe`, the shipped `Revenant.exe` with a 3-byte
  Win98 compatibility patch;
- the output is 640×480 with software 3D;
- the INI has `RealTimeLight=No`, `EnhancedLighting=No`, `GammaLevel=3`,
  `MaxLights=3`, `Ambient3D=130`.

The VFX track shares the lab and expects the editor running with the lab's
own INI. Leave it that way when done.

`tools/retaillab/retail.py` wraps the lab's serial guest agent and the
emulator's control socket. Run it with the lab's Python:

```
P=~/RevenantRetailLab/.venv/bin/python
$P tools/retaillab/retail.py start ShowDialog=Yes AutoCombat=No   # closes the editor, backs the INI up once, launches the game
$P tools/retaillab/retail.py record start                         # lossless AVI with audio, 60 fps
$P tools/retaillab/retail.py newgame                              # title screen → New Game
$P tools/retaillab/retail.py key 3                                # a dialog choice
$P tools/retaillab/retail.py capture shot.png
$P tools/retaillab/retail.py prompt alreadydead --scratch <dir>
$P tools/retaillab/retail.py record stop                          # prints the AVI path (lab captures/)
$P tools/retaillab/retail.py restore                              # closes the game, restores the INI, relaunches the editor
```

`start` takes the lab (`~/RevenantRetailLab/gameflow-lab.lock`, naming
`--owner`) and refuses while another run holds it; `restore` releases it.
`start` keeps the lab's original INI in `C:\MCP\GFPRE.INI` and doesn't
overwrite that copy on later starts; `restore` writes it back. Retail
rewrites its INI on exit, so always end with `restore`.

## 2. Driving the game

- **New Game.** It goes straight to the Keep. The loading screen shows for
  about 12 s, then the opening plays with no movie.
- **Choices.** The digit keys pick choices. A digit pressed with no menu up
  is ignored during the opening; once control returns, the digits switch
  the side panels (`Spells=1` … `Map=6`). Watch for this when a script
  presses keys on a timer.
- **The Enter prompt.** The prompt (TTextBar `CharPress` `0x0054d4a0`)
  takes `@<script line>` (run on the player) and the cheat words
  (`SubmitInput` `0x0054d700`, TTextBar_SPEC §10):
  - `alreadydead`: the player takes no damage, and the health shows full.
  - `nahkranoth`: the player's hits do 100000 damage (`0x004c4950`).
  - The others are toggles too. Entering a word again turns it off.
- **When the prompt opens.** Only with control on and the player's action
  neither COMBAT nor BOW. While it's open and the player goes back into
  combat, the next key commits the line as typed so far (a "Locke: nahk"
  line).
  - In a fight: press C to leave combat, then Enter, check that
    "Message:" shows, then type the whole line in one burst. `prompt` does
    exactly this. Getting hit puts the player back in combat even with
    `AutoCombat=No`.
  - Keys typed while the prompt is closed are hotkeys: `o` opens Options,
    and Escape from there opens the in-game menu.
- **Options Cancel re-applies the gamma ramp** (`0x0053b0c7`), and under
  dosbox-x that visibly changes the picture (§3, 13). After visiting
  Options, use the session for positions and timing only, not colour.
- **The opening fight.** Unarmed and with no player input, Locke (100 hp)
  loses to Rahul in 2–3 minutes. The run below used `alreadydead`, then
  swings (A/S/D) until Rahul fell.

## 3. The opening reference set (2026-10-06)

The shots are in [reference/opening/](reference/opening/). They come from
two New Game runs:

- run 1 with the lab INI plus `ShowDialog=Yes`;
- run 2 adding `AutoCombat=No`.

The full recordings are in `~/RevenantRetailLab/captures/gameflow/`:

- `opening-20261006-run1.avi` (11.7 min): the opening, then a lost fight
  and Game Over;
- `opening-20261006-run2.avi` (15.6 min): the opening, the fight with
  cheats, Rahul's death, and Tendrick's scene to the end.

| # | Shot | Beat | Use |
|---|---|---|---|
| 01 | `01_loading.png` | "Loading Game" with the Ahkuilon art and the red bar | loading screen |
| 02–04 | `02_vortex_swirl.png`, `03_vortex_glow.png`, `04_vortex_column.png` | gvortex: green ribbon round the circle, the glowing floor disc, the column over Locke | appear effect |
| 05 | `05_s1_pit.png` | Locke standing in the pit, default camera, before any dialog | lighting shot S1 (LIGHTING_FIDELITY §7) |
| 06 | `06_locke_where_am_i.png` | Locke's "Where am I?" | player line, bottom stack |
| 07 | `07_sardok_welcome.png` | Sardok's "Welcome back from the dead, Revenant." | NPC line, top stack |
| 08 | `08_choices.png` | The three choices, side panel open | choice list |
| 09 | `09_tendrick_three_lines.png` | Tendrick's three-line line | wrap, line spacing |
| 10 | `10_rahul_fight.png` | Rahul attacking; his portrait and health at the top right | combat HUD, combat log |
| 11 | `11_rahul_killed.png` | "Critical strike vs. Rahul Dmg:55"; Sardok and Tendrick come back | Rahul's death |
| 12 | `12_game_over.png` | Game Over (Restart / Load / Exit) | death screen |
| 13 | `13_after_options_cancel.png` | After Options → Cancel: the gamma ramp applied, lavender | gamma (S16), not the S1 look |

Facts the set settles:

- **Side panel.** The side panel is open on the equipment page through the
  whole opening, and the lower panel is the inventory. The port does the
  same.
- **Dialog placement** (DIALOG.md §4.4). NPC lines hang from y≈122, the
  player's stack sits above the map's bottom less 50, and x starts at the
  map's left with the portrait ring at x≈2–40. The port is within 2–3 px
  (§4).
- **Dialog colours.** Locke and the choices are azure, Sardok green,
  Tendrick yellow, Rahul cyan. The text has a 1 px black shadow.
- **The gvortex** lights Locke and the floor green.
- **The pit** inside the resurrection circle shows a blue-violet cracked
  surface, not black.
- **Gamma ramp.** At boot under dosbox-x the ramp isn't in effect; it is
  after Options → Cancel (13).

## 4. Port against retail (5a4d9ea, `--resolution=640x480`, retail-default INI)

Side-by-sides were made at the same beats. Measured text extents:

| | Retail | Port |
|---|---|---|
| Sardok's line (green pixels) | x 49–342, y 137–152 | x 51–345, y 134–147 |
| The choice list (azure pixels) | x 50–298, y 293–369 | x 52–300, y 290–365 |

- **Text position.** The port's text sits 2 px right and 3–4 px high. The
  canonical glyph walk (`font.cpp`) puts the baseline at the tallest ASCII
  glyph's rise less 2 (`kGdiTopLeading`), calibrated on the HUD's small
  fonts; GDI puts it at the font's `tmAscent`. For Times New Roman 20
  that's the 3–4 px. The fix belongs in the shared glyph walk, with every
  panel re-checked (BURNDOWN, cross-track).
- **Text weight.** Retail's text is heavier: aliased GDI against
  antialiased Tinos, a documented deviation (DIALOG.md §6).
- **The rest.** Lighting, the pit and the gvortex differ; they are tracked
  in LIGHTING_FIDELITY.md and the VFX burndown.
