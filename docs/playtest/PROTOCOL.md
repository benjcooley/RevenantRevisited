# Playtest protocol and recording format (v1)

The contract between the director (`tools/playtest`) and the two game
hosts: the port (`Revenant --control=stdio`) and the emulator's play slot
(`tools/retail_runtime/slots/play/server.py`). Both hosts implement it
exactly; [PLAN.md](PLAN.md) explains why it looks this way.

All text is UTF-8 JSON, one object per line, `\n`-terminated. Integers are
JSON numbers. Unknown fields are ignored by readers, so fields can be added
without a version bump; removing or changing one bumps `rvrec`/`protocol`.

## 1. Time

- **Tick**: one 24 Hz world update. Retail: one firing of the
  `timeSetEvent` callback and one pass of the screen loop. Port: one
  `TScreen::Pulse` (`--fixedstep` gives one per frame).
- **Session tick** `t`: ticks since play started (`start`). `t = 0` is the
  first tick after the start state is in place.
- An event for tick `t` is delivered before tick `t` updates, in file
  order, at one point per host: the port's input-queue drain just before
  `CurrentScreen->Tick()`, the emulator's tick boundary before it fires
  the timer callback.
- **Lockstep**: under a host's control channel, one tick is one frame (one
  update, one draw), and nothing advances between requests.

## 2. Input events

Every event has a time key (`t`, or `f` under frame timing, §4) and exactly
one of:

| Field | Meaning |
|---|---|
| `"key": NAME, "down": bool` | a key down or up. `NAME` is a Windows virtual-key name without `VK_`: `A`–`Z`, `0`–`9`, `F1`–`F12`, `LEFT` `RIGHT` `UP` `DOWN` `HOME` `END` `PRIOR` (Page Up) `NEXT` (Page Down) `INSERT` `DELETE` `RETURN` `ESCAPE` `SPACE` `BACK` `TAB` `SHIFT` `CONTROL` `MENU` (Alt), `NUMPAD0`–`NUMPAD9`, `OEM_*`, or a code as `"0x5D"`. Chords are separate events: `CONTROL` down, `A` down, `A` up, `CONTROL` up |
| `"char": CODE` | a character (`WM_CHAR`), as its Windows-1252 code: `13` Enter, `8` Backspace, `27` Escape, `97` `a`. Typed text, save names and the text-bar prompt read characters, not keys |
| `"mouse": [X, Y], "buttons": MASK` | the whole pointer state after the event: position in 640x480 game-screen pixels and the held buttons (bit 0 left, bit 1 right, bit 2 middle). Hosts derive move, down and up from the change from the previous state. Pointer state starts at `[320, 240]`, no buttons |

Hosts never synthesize input of their own, apart from the start rules in
§5. Character translation is not implied: a typed letter is a `key` down,
a `char`, and a `key` up, as Windows delivers it.

## 3. Check lines

```json
{"t": 240, "check": {"screen": "play", "level": 2, "pos": [6280, 4338, 33],
  "facing": 96, "health": 87, "mana": 40, "fatigue": 70, "action": 2,
  "target": {"pos": [6301, 4350, 33], "health": 30}, "draws": 18342}}
```

Written by a recording host every 24 ticks, at every mark, and at the end.
A check line at tick `t` is the state at that tick's boundary: after `t`
ticks have run (with, in lockstep, the `t` frames that drew them) and
before tick `t`'s events are delivered; `t = 0` is the start state. Under
frame timing a frame can run no tick or several, so the line goes at the
first frame boundary at or after each multiple of 24, with the tick it
reached.

| Field | Source (port / retail) |
|---|---|
| `screen` | `title`, `loading`, `play`, `menu`, `dialog`, `death`, `movie`, `other` |
| `level` | the player's map level |
| `pos` | the player's world position, integers |
| `facing` | the player's facing, 0–255 |
| `health`, `mana`, `fatigue` | current values |
| `action` | the player's `doing` action number (retail numbering, PLAYER_INPUT.md §3) |
| `target` | the player's fight target's position and health, or `null` |
| `draws` | draws from the game's random stream since start: the port's `RandomDraws()`; the emulator's count of calls to the CRT `rand` `0x0058c582` (`random()` `0x00483300` draws through it only when its bounds differ, as the port's does, and 32 other callers draw directly, as the port's `GameRand()` users do) |

A replay compares each check line field by field and stops at the first
tick with a difference, reporting the fields that differ. Fields a host
can't produce are omitted, and comparisons skip fields missing on either
side.

## 4. The recording: `*.rvrec`

Line 1 is the header; then events, check lines and marks in tick order;
the last line is the end.

```json
{"rvrec": 1, "game": "port", "build": {"sha": "…", "name": "feature/playtest@599264bf", "patches": ["clickwalk"]},
 "start": {"from": "slot", "slot": "Keep after opening", "slot_sha": "…"},
 "ini_sha": "…", "seed": 1, "tick_hz": 24, "screen": [640, 480],
 "timing": "lockstep", "created": "2026-10-10T21:30:00Z", "by": "director 0.1"}
```

| Header field | Meaning |
|---|---|
| `game` | `port` or `retail` |
| `build.sha` | port: the git commit; retail: the executable's SHA-256 |
| `build.name`, `build.patches` | human-readable build and the behaviour patches it carries (`clickwalk`) |
| `start.from` | `newgame` (retail `QUICKSTART`, port `--quickstart`: the opening plays) or `slot` |
| `start.slot`, `start.slot_sha` | the slot's name and the SHA-256 of its directory (§6) |
| `ini_sha` | SHA-256 of the `Revenant.ini` used |
| `seed` | the game random seed (port `--seed`; the emulator's CRT seed) |
| `timing` | `lockstep` or `frames` |

Body lines:

- events (§2), keyed by `t` under `lockstep`;
- under `frames`, each display frame opens with `{"f": N, "dt": "HEX"}`,
  the frame's exact time step as the 16 hex digits of its IEEE-754 double,
  and the events delivered before that frame's ticks follow it, keyed by
  `"f": N`. Check lines still use `t`;
- check lines (§3);
- marks: `{"t": T, "mark": KIND, …}`, for example
  `{"t": 300, "mark": "quicksave", "slot": "Quick Save 1"}` or
  `{"t": 420, "mark": "milestone", "name": "rahul dead"}`. Marks are notes:
  replay ignores them except to compare the check line written with them;
- the end: `{"t": T, "end": true}`, the tick count reached.

A recording is self-contained only with its **bundle**, a directory:

```
<name>/
  session.rvrec       the recording
  Revenant.ini        the INI used (its SHA is ini_sha)
  start/<slot>/       the start slot, when start.from is "slot"
  session.rvlog       director notes: questions, answers, costs (optional)
  frames/             PNGs the director asked for (optional)
```

## 5. The control channel

Requests are `{"id": N, "cmd": NAME, …}`; each gets exactly one response,
`{"id": N, "ok": true, …}` or `{"id": N, "ok": false, "error": "…"}`, in
order. A host writes nothing else on its stdout. Logs go to files and
stderr.

| `cmd` | Request fields | Response fields |
|---|---|---|
| `hello` | | `game`, `build`, `protocol: 1`, `tick_hz`, `screen`, `capabilities` (list of the optional commands it implements) |
| `start` | `from`, `slot` (a bundle `start/` path when `from` is `slot`), `seed`, `record` (bundle directory, optional), `ini` (path, optional) | `t: 0` once play is reached and control can be given |
| `step` | `ticks`, `events` (each with `t` relative to this step, `0 <= t < ticks`) | `t`: the session tick reached |
| `observe` | `frame` (PNG path, optional), `objects` (radius in world units, default 800), `walkgrid` (radius, optional) | the observation (§7) |
| `project` | `points`: `[[x, y, z], …]` | `screen`: `[[sx, sy] or null, …]`: game-screen pixels, `null` when off the play field; `world`: `[bool, …]`, whether a click at each reaches the world rather than the HUD or a modal (the port's `TPlayScreen::ClickRoute`; a host without it omits it). A click's map point is taken at Locke's height + 50, so project a point to click at that height |
| `pick` | `x`, `y` | `pos`: the map point under that pixel by the game's pointer conversion (the right-button walk's: Locke's height + 50), `object`: the id under it or `null`, `world`: whether a click there reaches the world. The retail host answers `pos` only: retail finds the object under the pointer with its 3D pick, which draws a whole frame and changes the game's pick cache, so asking would change the game |
| `export_slot` | `slot` (a slot name, or `@last` for the slot the game wrote most recently: Quick Save picks its own "Quick Save N"), `to` | `path`, `slot` (the name exported) |
| `mark` | `kind`, other fields | writes the mark and a check line to the recording |
| `replay` | `bundle`, `verify` (bool), `until` (tick, optional) | `t`, and with `verify`: `diverged: null` or `{t, fields}`. A replay starts the way the recording did: a new game, or the bundle's own `start/<slot>` with its seed and INI |
| `checkpoint`, `restore` | `name` | retail only |
| `caption` | `text` (one or two short lines; empty clears) | shown to a person watching (§5a); never part of the game, the frame `observe` captures or the recording |
| `quit` | | ends the recording with its end line and exits |

`start` is the only command that may run ticks of its own: loading,
entering the world, and for `newgame` nothing else. The opening's scenes
then run under `step` like everything else.

A replay boots its own game from the bundle's start, as the recording's
host did, so the port answers `replay` only in place of `start`, in a
fresh host: a process that has run a game is no longer in the state the
recording started from.

## 5a. Watching

A person can watch either game while the director plays. Watching never
changes what the game does: it only adds waiting between ticks and pixels
outside the game's own frame.

| | Port | Emulator |
|---|---|---|
| see the game | `--control=stdio` without `--headless`: the game window | `server.py --watch DIR`: after every tick, the front buffer as `DIR/frame.png` (written to a temporary name, then renamed), and `DIR/caption.txt` |
| real speed | `--control-pace=24`: stepped ticks no faster than 24 a second | `--pace 24`: the same |
| captions | drawn over the window | in `caption.txt`, for the viewer |
| the viewer | optional `--watch-frames=DIR`: the same `frame.png` per tick, for the viewer page | `tools/playtest/watch.py DIR` |

`tools/playtest/watch.py` serves a page with the live frame, the caption,
and the director's decisions as they happen.

## 6. Slot hashing

`slot_sha` is the SHA-256 of, for each file under the slot directory in
sorted order of its relative path (`/`-separated, lower case), the path,
a NUL byte, the file's bytes and a NUL byte.

## 7. Observation

```json
{"t": 912, "screen": "play", "control": true,
 "player": {"pos": [6280, 4338, 33], "screen": [300, 210], "level": 2, "facing": 96,
            "health": [87, 100], "mana": [40, 60], "fatigue": [70, 80],
            "mode": "combat", "action": 3, "action_name": "combat", "action_tick": 14,
            "target": 1712, "gold": 31, "xp": [1, 140, 300],
            "skills": {"attack": 4, "defense": 3, "invoke": 1, "weapon": 2},
            "weapon": {"name": "Short Sword", "mask": 2},
            "quick_spells": ["Fire Flash", null, null, null],
            "belt": ["Healing Potion", null, null, null, null],
            "pack": [{"slot": 5, "name": "Short Sword", "class": "weapon", "eqslot": 4, "screen": [505, 392]}],
            "equipped": {"2": "Red Cloth Shirt"}},
 "dialog": {"speaker": "Sardok", "choices": ["Who are you people?", "Who am I?"]},
 "messages": ["Locke entered The Keep"],
 "objects": [{"id": 1712, "name": "Rahul", "type": "Rahul", "class": "character",
              "pos": [6301, 4350, 33], "screen": [212, 180], "enemy": true,
              "health": [30, 40], "action": 7, "action_name": "atkchop", "action_tick": 3,
              "target": 0}],
 "frame": "/abs/path.png"}
```

- `control`: the player has control (scripts and scenes turn it off).
- `player.mode`: `normal`, `combat`, `bow`, `sneak`.
- `action_name`: the name on the character's `doing` action block (an
  animation or attack name, e.g. `atkchop`, the names char.def's ATTACK
  lines use); `action_tick`: ticks since that block started. With
  char.def's `impacttime` and `blocktime`, the director tells a wind-up
  from an impact or a recovery.
- `target` (player and characters): the id of the character it fights, or
  `null`; the player's id in `objects` is `0`.
- `skills`: the character stats combat gates on (attack, defense, invoke,
  and the skill of the weapon in hand); `weapon.mask` is char.def's weapon
  mask bit for the weapon in hand (`WM_*`), `0` bare-handed.
- `quick_spells`, `belt`: what the four quick-spell slots (F1-F4) and the
  five belt slots (F5-F9) hold, by name, `null` when empty.
- `pack`: the player's items in the inventory pane's twelve cells in view:
  slot, name, class (lower case), `eqslot` (the item's own equipment slot,
  its EqSlot stat; -1 for none) and the cell's screen centre (`null` while
  the pane is hidden). A right click there equips the item in its own slot
  (PLAYER_INPUT.md §6). `equipped`: equipment slot (player.h `EQ_*`) to the
  item worn or wielded there.
- `xp`: character level, experience, experience for the next level.
- `messages`: text-bar lines added since the previous `observe`.
- `objects`: within the radius, nearest first: characters, items,
  containers, exits and doors, waypoint helpers. `name` is the instance
  name, `type` the object type name (char.def's CHARACTER names for
  characters), `class` the object class name in lower case (`character`,
  `item`, `container`, `exit`, `helper`, …), and `screen` is `null` off
  screen.
- `walkgrid` (when asked): `{"origin": [gx, gy], "cell": 16, "w": W,
  "h": H, "heights": [...]}`, row-major walk heights: 0 for no floor, -1 for
  a cell no loaded sector holds (unknown, not "no floor"). Cell k is centred
  on world k*16, as `GetWalkHeight` reads it (`(x + 8) >> 4`).
- `player.radius`: the radius the game tests Locke's steps over
  (`GetWalkHeightRadius`).

Raw numbers only: bucketing for Jev is the director's job.
