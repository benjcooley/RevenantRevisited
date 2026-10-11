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

| Field | Source (port / retail) |
|---|---|
| `screen` | `title`, `loading`, `play`, `menu`, `dialog`, `death`, `movie`, `other` |
| `level` | the player's map level |
| `pos` | the player's world position, integers |
| `facing` | the player's facing, 0–255 |
| `health`, `mana`, `fatigue` | current values |
| `action` | the player's `doing` action number (retail numbering, PLAYER_INPUT.md §3) |
| `target` | the player's fight target's position and health, or `null` |
| `draws` | game `random()` calls since start: the port's `RandomDraws()`, the emulator's count of calls to `0x00483300` |

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
| `project` | `points`: `[[x, y, z], …]` | `screen`: `[[sx, sy] or null, …]`: game-screen pixels, `null` when off the play field |
| `pick` | `x`, `y` | `pos`: the map point under that pixel by the game's pointer conversion (the right-button walk's: Locke's height + 50), `object`: the id under it or `null` |
| `export_slot` | `slot`, `to` | `path` |
| `mark` | `kind`, other fields | writes the mark and a check line to the recording |
| `replay` | `bundle`, `verify` (bool), `until` (tick, optional) | `t`, and with `verify`: `diverged: null` or `{t, fields}` |
| `checkpoint`, `restore` | `name` | retail only |
| `quit` | | ends the recording with its end line and exits |

`start` is the only command that may run ticks of its own: loading,
entering the world, and for `newgame` nothing else. The opening's scenes
then run under `step` like everything else.

## 6. Slot hashing

`slot_sha` is the SHA-256 of, for each file under the slot directory in
sorted order of its relative path (`/`-separated, lower case), the path,
a NUL byte, the file's bytes and a NUL byte.

## 7. Observation

```json
{"t": 912, "screen": "play", "control": true,
 "player": {"pos": [6280, 4338, 33], "level": 2, "facing": 96,
            "health": [87, 100], "mana": [40, 60], "fatigue": [70, 80],
            "mode": "combat", "action": 2, "action_name": "walk",
            "target": 1712, "gold": 31, "xp": [1, 140, 300]},
 "dialog": {"speaker": "Sardok", "choices": ["Who are you people?", "Who am I?"]},
 "messages": ["Locke entered The Keep"],
 "objects": [{"id": 1712, "name": "Rahul", "class": "character",
              "pos": [6301, 4350, 33], "screen": [212, 180], "enemy": true,
              "health": [30, 40], "action": 7, "action_name": "attack"}],
 "frame": "/abs/path.png"}
```

- `control`: the player has control (scripts and scenes turn it off).
- `player.mode`: `normal`, `combat`, `bow`, `sneak`.
- `xp`: character level, experience, experience for the next level.
- `messages`: text-bar lines added since the previous `observe`.
- `objects`: within the radius, nearest first: characters, items,
  containers, exits and doors, waypoint helpers. `class` is the object
  class name in lower case (`character`, `item`, `container`, `exit`,
  `helper`, …), and `screen` is `null` off screen.
- `walkgrid` (when asked): `{"origin": [gx, gy], "cell": 16, "w": W,
  "h": H, "heights": [...]}`, row-major walk heights, 0 for no floor.

Raw numbers only: bucketing for Jev is the director's job.
