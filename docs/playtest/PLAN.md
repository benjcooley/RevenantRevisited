# Playtest track: Jev plays both games

Branch `feature/playtest`, worktree `worktrees/playtest`. Started
2026-10-10.

## 1. Goal

An agent built on **Jev** (TypeSafe's decision model, through OpenRouter)
plays both versions of Revenant:

- **Retail**: the shipped `Revenant.exe` in the in-process emulator
  (`tools/retail_runtime`). It runs either byte for byte or as a named
  build of the retail assembly carrying our hooks and patches.
- **Port**: RevenantRevisited.

It produces:

1. **Save games** at named milestones in both games, in retail's save
   format. Port saves already load in retail and the reverse
   ([SAVE_GAME.md](../gameflow/forensics/SAVE_GAME.md)).
2. **Frame-accurate input recordings.** Every session, played by the agent
   or by a person, is recorded as the input delivered at each game tick,
   plus the state it started from. A recording replays exactly in the game
   that made it. Replayed in the other game, it runs the same inputs, and
   the first tick where the two disagree marks a port bug.

Revenant suits a decision model. Combat rewards choosing attacks, combos
and spells and moving around one target, not reflexes. Both hosts run in
lockstep and wait while the agent thinks.

Not used: the DOSBox lab. It has no pause, frame step, frame-stamped input
or savestate API, and it does not run deterministically (max cycles, Win98
scheduling, no fixed seed). It remains the place to look at real retail
and to check save interop ([RETAIL_CAPTURE.md](../gameflow/RETAIL_CAPTURE.md)).

## 2. Jev

Two OpenRouter products, both verified with our key (`$OPEN_ROUTER_KEY`)
on 2026-10-10:

| Model | Call | Role here |
|---|---|---|
| `typesafe/jev-1.13` | `POST /api/alpha/decisions`: a `state` and typed questions (Choice, Noul, Score); returns the answer, a probability per option and a confidence | **System One**: every tactical and navigation choice. Text only, about 350 ms and $0.00002 a call |
| `typesafe/jev-router` | Chat Completions with images and tools; Jev picks the serving model and effort | **System Two**: screens without state, unknown menus, being stuck, planning the next objective. About 10 s and $0.005 a screenshot |

TypeSafe's guidance sets the design
([System One](https://docs.typesafe.ai/concepts/system-one),
[known weak spots of 1.13](https://docs.typesafe.ai/model-jaggedness/jev-1.13)):

- Code owns the loop. Jev answers narrow questions; it is not an agent.
- Arithmetic, distances, counting and pathing stay in code. Jev gets named
  buckets ("health low", "enemy in melee reach", "enemy winding up"), not
  raw numbers.
- State is filtered to what each question needs. Unrelated state costs
  accuracy.
- Choice leans toward its first option, so the client shuffles options
  and asks again when a decision is close.
- A low-confidence answer goes to a rule or to System Two.

## 3. Architecture

```
tools/playtest/                       Python, one package
  jev.py        Decisions + router client, cost and latency log
  rec.py        the recording format (§5): write, read, diff
  host.py       the GameHost protocol (§4): PortHost, RetailHost
  director.py   the play loop: modes, objectives, saves
  walk.py       destinations on screen, click-to-walk, stall handling
  combat.py     tactics as tick-exact inputs; Jev's combat questions
  knowledge/    spells, combos, monsters, milestones (from game data)

     director ──GameHost JSONL──┬── PortHost:   Revenant --control (§6)
                                └── RetailHost: retail_runtime slots/play (§7)
```

Both hosts are separate processes speaking the same JSON Lines protocol on
stdio. The director never calls game internals: it sends input and reads
observations. Everything the agent does is therefore a recording another
host can replay.

## 4. The GameHost protocol

One JSON object per line each way; responses carry the request's `id`.

| Request | Effect |
|---|---|
| `hello` | `{game, build: {sha, name, patches}, tick_hz: 24, screen: [640, 480], capabilities}` |
| `start {from: "newgame" \| "slot" \| "checkpoint", slot?, record?: path, seed}` | Boots straight into play (retail `QUICKSTART[=slot]`, port `--quickstart[=slot]`) and starts recording at play's tick 0 |
| `step {ticks, events: [{t, …}]}` | Runs exactly `ticks` ticks. An event with `t = k` is delivered before the k-th of them runs. Returns the tick reached |
| `observe {frame?: path, objects?: radius, walkgrid?: radius}` | The observation (§8), with a PNG of the last frame if asked |
| `project {points}` / `pick {x, y}` | World to screen and screen to map, by the game's own functions |
| `export_slot {slot, to}` | Copies a save slot directory to the host |
| `checkpoint {name}` / `restore {name}` | Retail only: an image of the whole machine |
| `replay {path, verify}` | Runs a recording; with `verify`, reports the first tick whose check line differs |
| `quit` | |

A host never advances on its own: time moves only through `step`.

## 5. Ticks and the recording format

**Tick.** One game tick is one 24 Hz world update. In retail that is one
firing of the `timeSetEvent` callback and one pass of the screen loop
([TIME.md](../TIME.md)). In the port it is one `Pulse` of
`TPlayScreen` (`--fixedstep`: exactly one per frame). Events stamped for
tick N are delivered before tick N updates.

**Format** `*.rvrec`: JSON Lines, UTF-8. The exact contract, including
the control channel, is [PROTOCOL.md](PROTOCOL.md); this section is the
overview.

```jsonc
{"rvrec": 1, "game": "retail", "build": {"sha": "…", "name": "clickwalk"},
 "start": {"from": "slot", "slot": "Keep after opening", "slot_sha": "…"},
 "ini_sha": "…", "seed": 1, "tick_hz": 24, "screen": [640, 480],
 "timing": "lockstep", "created": "2026-10-10T21:30:00Z", "by": "director 0.1"}
{"t": 120, "key": "C", "down": true}
{"t": 122, "key": "C", "down": false}
{"t": 130, "mouse": [412, 233], "buttons": 1}       // the whole pointer state
{"t": 131, "mouse": [412, 233], "buttons": 0}
{"t": 160, "char": 97}                              // text: the prompt, save names
{"t": 240, "check": {"pos": [6280, 4338, 33], "level": 2, "health": 87, "draws": 18342}}
{"t": 300, "mark": "quicksave", "slot": "Quick Save 1"}
{"t": 912, "end": true}
```

- Keys are Windows virtual-key names (`VK_*` without the prefix).
- The pointer is in 640x480 game-screen pixels in both games. A mouse line
  carries the whole pointer state; hosts derive move, down and up from the
  change.
- `check` lines are written every 24 ticks and at marks. They hold fields
  both games define the same way: the player's position, facing, health,
  mana, fatigue, level and action; the target's position and health; and
  the count of game random draws. A replay compares them field by field
  and stops at the first tick that differs, naming the fields.
- `timing: "lockstep"` means one rendered frame per tick, which is what
  both hosts use under the agent and the only timing two games can share.
  A person playing the port at the display's rate records
  `timing: "frames"`, and each frame line `{"f": n, "dt": "<hex double>"}`
  carries the exact frame time, because the port, like retail, draws
  game random numbers while rendering (§6.3).
- Agent decisions, questions, answers and screenshots go in a sidecar
  `*.rvlog`, never in the recording.

## 6. Port host

What exists (`src/revmain.cpp`, `src/time.cpp`, `src/screen.cpp`):
sokol calls `AppFrame` once per display frame (an NSTimer at about 60 Hz
headless). `AppFrame` advances the clock (`--fixedstep`: exactly one tick
per frame), flushes the coalesced mouse move, runs the input simulator,
then `CurrentScreen->Tick()` (the screen's `Pulse`s for the ticks due),
draws, flips and captures. `--seed`, `--quickstart[=slot]` (synchronous
load), `--headless`, `--filmstrip`/`Display.RequestCapture` and the
`random()` draw counter and observer (`revutils.cpp`) are in place. Missing:
a live command channel, a recorder, and a structured state dump.

### 6.1 One input queue

`AppEvent` stops calling the screen. After the ImGui and focus gates, it
appends each game event to a frame queue: key down/up (VK), character,
mouse down/up with button and position, the latest move, and modifier
changes. The queue drains in one place, where the mouse flush and the
input simulator sit now, just before `CurrentScreen->Tick()`. That drain
is the recorder and the injector. Live hardware, the input simulator, the
control channel and a replay file all feed the same queue, so live play
and replay dispatch identically. It also fixes today's ordering, where
clicks run inside `AppEvent` but the move waits for the next frame.

### 6.2 Control channel: `--control=stdio`

JSON Lines on stdin and stdout; the stdout log sink is off in this mode,
and `revenant.log` keeps everything. Lockstep: `AppFrame` returns at once
unless a `step` has frames left, and each stepped frame is a fixed-step
frame (one tick, one draw). The other requests (§4) are answered between
frames.

### 6.3 Determinism

In `--control`, `--record` and `--replay`:

| Source | Fix |
|---|---|
| seed from `time(nullptr)` | `--seed` required; the header records it |
| `LoadSlice`'s 30 ms wall-clock budget per tick (`gamesession.cpp`) | the whole level in one slice, as retail's synchronous load: the world holds no ticks for a level change, in either game |
| the hover cursor from the asynchronous GPU id probe (`mappane.cpp`); it decides whether a click attacks | retail's rule: `OnObject` synchronously every 8th screen frame (MAP_INPUT.md §3), and clicks too, from the drawn frame (`EPickRead::Drawn`). A fidelity fix too |
| `GenerateUniqueID` mixes `tickcount()` | game time (`GameTickCount()`) |
| area music: `MusicPlaying()` (audio thread) and `tickcount()` before `random()` (`area.cpp`) | game time for the retry, and the music state answered as "not playing", as in a `NOSOUND` retail |
| render-time game `random()` (animators created on screen, `SubmitPartSys`) | kept, as retail draws there too; lockstep fixes the frames, and `timing: "frames"` replays the recorded `dt`s |
| effects' libc `std::rand()` | seeded with the game seed; its draws follow the frames |
| INI options (`GameSpeed`, `PlaySpeech`, …) and `--revisited` | the INI's SHA in the header; the replay uses the recorded INI |
| ImGui and focus gating | off in control mode |

A per-tick check line (`gameframes`, `RandomDraws()`, the player's
position and health, the screen) goes to the log in these modes, and
`--combattrace-rng` with the random observer bisects a divergence to the
caller.

### 6.4 Saves

The agent saves with Quick Save (`Ctrl+Backspace`, "Quick Save N"), which
is plain input; `export_slot` names the copy. `savegame.<name>` and
`loadgame.<name>` (retail console commands the port lists as not ported)
are ported over `RequestSave`/`RequestLoad` for scripts and `--exec`.

### 6.5 Observation

`observe` builds §8 from the live objects: `Player`, the action blocks,
the map pane's objects near Locke, the dialog pane, the text bar's new
lines and the current screen. `project`/`pick` use the map pane's own
conversions.

## 7. Retail host: the emulator's `play` slot

`tools/retail_runtime/slots/play/`, built on the HUD slot's machine
(`slots/hud`: `system.py`, `winfs.py`, `profile.py`, `window.py`,
`ddraw.py`, `gdi.py`). That machine already boots the shipped game by its
own code through game init in about 95 s, and caches the result as a disk
image (`hudworld.py`, `image.py`). The play slot continues from there:

1. **Command line** `NOSOUND QUICKSTART[=<slot>]`: retail skips the intro
   movie and the title and starts a new game or loads the slot, so
   neither Smacker nor Miles playback is reached. In-game movies
   (`playmovie`) are answered as finished at a recorded boundary.
2. **The tick.** The screen loop waits on the event that the
   `timeSetEvent` callback pulses. The host fires the recorded callback
   once per tick, runs the scheduler until the main thread waits again,
   and advances the virtual clock by 1000/24 ms. `GetTickCount` and
   `timeGetTime` follow the virtual clock, so retail's frame-skip logic
   never skips.
3. **Input.** A host-owned keyboard state answers `GetKeyboardState`; key,
   character, move and button messages are posted to the game window at
   the tick boundary; the virtual cursor answers `GetCursorPos`.
   `TranslateMessage` gains its character translation. DirectInput stays
   absent, as in the HUD slot. Which path retail reads for each control is
   settled by tracing, not assumed.
4. **Files.** The install stays read-only. A write overlay holds `Save\`,
   `CurMap\` and INI writes, and `export_slot` copies a slot out.
5. **Frames.** The 640x480 RGB565 front buffer after the flip, as PNG on
   request.
6. **Observation.** Read from retail memory at the layouts the recon and
   REVSYNC ports document (the player, its action blocks, the map pane's
   objects, the text bar); text on screen also arrives as the GDI calls
   the HUD slot records.
7. **Checkpoints.** Machine images at milestones (after the opening, at
   the forest, …), so a session starts in seconds instead of booting and
   replaying.

The emulator is deterministic by construction: a virtual clock, a
cooperative scheduler, and no host time. Its speed is unmeasured; it is
the first number this slot reports.

## 8. Observation

```jsonc
{"tick": 912, "screen": "play",                 // title, loading, play, dialog, death, menu
 "control": true,                               // off during scripts and scenes
 "player": {"pos": [6280, 4338, 33], "level": 2, "facing": 96,
            "hp": [87, 100], "mana": [40, 60], "fatigue": [70, 80],
            "mode": "combat", "action": "walk", "target": 1712, "gold": 31,
            "xp": {"level": 1, "exp": 140, "next": 300}},
 "dialog": {"speaker": "Sardok", "text": "…", "choices": ["Who are you people?", "…"]},
 "messages": ["Locke entered The Keep"],        // text bar lines since the last observation
 "objects": [{"id": 1712, "name": "Rahul", "class": "character", "pos": […],
              "screen": [212, 180], "bearing": "NE", "dist": "melee",
              "enemy": true, "hp": "high", "action": "attack windup"}],
 "exits": […], "waypoints": […]}
```

Distances and bearings come both raw (for code) and bucketed (for Jev).
The walk grid is a separate query because it is large.

## 9. The director

Code-owned modes, each with narrow Jev questions:

- **Menus and loading.** Scripted from known layouts. An unknown screen
  goes to System Two with the screenshot.
- **Scenes and conversations** (control off). Step until control returns.
  A dialog choice is a Jev Choice over the choice texts, given the current
  objective, answered with the digit keys `1`..`6`.
- **Exploring.** From the objective and the candidates (§10), a Jev Choice
  of where to go; on arrival the director uses, talks or takes, and Jev
  Nouls check whether the objective's condition now holds.
- **Combat.** A decision every few ticks (§11).
- **Inventory, equipment and shops.** Comparing equipment is arithmetic in
  code; buying and selling are Jev Choices over priced options.
- **Stuck.** No progress for N ticks escalates to System Two.

Objectives come from a milestone list (`knowledge/milestones.yaml`) built
from [STORY_TESTING.md](../gameflow/STORY_TESTING.md), the manual's
"Starting the quest" chapter and the
[CoreGamers walkthrough](https://coregamers.com/walkthrough/revenant).
System Two may refine the current objective from the dialog log.

## 10. Walking: click to walk, in both games

Retail walks Locke two ways ([MAP_INPUT.md](../gameflow/forensics/MAP_INPUT.md)
§4–5). Holding the right button walks toward the pointer, in the 8
directions of the numpad. A left click on an object more than 96 units
away walks to it, then uses or takes it. A left click on open ground does
nothing. Steering the 8-direction walk from outside would mean correcting
every tick, so **both games get a small change instead:**

> **Click to walk.** A left click on open ground (no object under the
> pointer, outside combat's attack cursor) walks Locke to the map point
> under the pointer, through the game's own `Goto` (retail `0x004cedb0`),
> which turns to any of 256 angles and snaps onto the point within 8.
> Holding the run key while clicking runs.

- The map point is retail's own pointer conversion, the one the
  right-button walk uses (`UpdateMouseMovement` `0x0044ee00`: the point
  under the pointer at Locke's height + 50), so both games resolve a
  click to the same point.
- **Retail**: a named build of the retail assembly, `clickwalk`, patches
  the left-release path of `TMapPane::MouseClick` (`0x0044f140`). The
  emulator runs that build; it also runs in the DOSBox lab.
- **Port**: the same rule in `TMapPane::PlayMouseRelease`, behind a
  setting (on for playtest sessions, off by default for Classic).

The director picks destinations as screen points. `walk.py` samples the
visible playfield. It keeps points whose walk-grid cells are walkable and
in the game's `LineOfSight` from Locke, and labels each by what lies that
way: an exit, an NPC, unexplored ground, the objective. Jev chooses among
the labels. The director clicks the point and steps until Locke arrives or
stalls, then picks again. A stall tries a nearer point, then A* over the
walk grid (retail's blocking rule, `TCharacter::Blocked`) for an
intermediate corner, then System Two with the screenshot. Waypoint helpers
are sparse marks where monsters got stuck: landmarks, not a navigation
graph. Exits and doors are objects to click.

## 11. Combat

In combat mode Locke steps in 8 directions while facing his one target,
and attacks need no aiming ([PLAYER_INPUT.md](../gameplay/forensics/PLAYER_INPUT.md),
[COMBAT_MOVEMENT.md](../gameplay/forensics/COMBAT_MOVEMENT.md)). The target
changes when he steps toward another enemy: the enemy in that direction
is taken if it is within about 47 units or nearer than the current
target. A left click on another enemy also takes it; any other left click
is a random swing. There is no target-cycling key.

The tactician asks Jev for one tactic every 3 to 6 ticks, or when the
enemy's action changes:

| Tactic | Inputs (shipped bindings, `Revenant.ini [Controls]`) |
|---|---|
| orbit left / right | the direction key perpendicular to the target, held |
| close in / back off | the direction key toward or away from the target |
| swing / thrust / chop | `A` / `S` / `D` |
| combo 1–3, 7–9 | `Ctrl+A/S/D`, `Q/W/E` (only those Jong has taught) |
| block | `F` held |
| dodge (8 directions) | `Ctrl` + direction |
| switch target | step toward the chosen enemy, or click it |
| quick spell 1–4 | `F1`–`F4` |
| belt item 1–5 (potions) | `F5`–`F9` |
| disengage | `C`, then click to walk or run away |

Its state: the target's kind, distance bucket, current action (winding up,
recovering, blocking, stunned, casting) and health bucket; Locke's health,
fatigue and mana buckets; the combos learned; the quick spells and what
each does; and what the knowledge base says about this kind of monster
(for example "fire-based: use ice"). Two Nouls ask alongside: whether to
disengage and heal, and whether to switch targets.

## 12. Save games

- Both games: Quick Save (`Ctrl+Backspace`) through input, then
  `export_slot` copies "Quick Save N" out under the milestone's name. The
  in-game menu's Save works too, and is a later test of the director.
- The library is `~/RevenantRevisited-testsaves/jev/<game>/<milestone>/`:
  one slot per directory, with `manifest.json` (game, build, milestone,
  tick, location, player summary) and the `.rvrec` that produced it from
  New Game. Every save can be regenerated, and every save is a start point
  for replays.

## 13. Phases

| Phase | Delivers | Done when |
|---|---|---|
| P0 | `jev.py`, `rec.py`, the protocol, the knowledge skeleton | Unit tests; one live Decisions and one router call logged |
| P1 | Port host: the input queue, `--control`, `--record`/`--replay`, the determinism fixes, `observe`, click to walk | A recorded opening replays with every check line matching; so does a person's session |
| P2 | Retail host: the play slot through the opening; the `clickwalk` build | `QUICKSTART` reaches play, input moves Locke, a frame captures, a recording replays exactly |
| P3 | Director: walking, conversations, combat | Jev plays the Keep chain (opening, Rahul, Rand, the exit) in the port and saves at each milestone |
| P4 | Cross replay | A retail recording replays in the port and reports its first divergent tick |
| P5 | Save library | Milestone saves in both games through the forest and Misthaven |

P1 and P2 run in parallel; P0 comes first because both hosts write its
format.

## 14. Ownership and rules

- Port changes: input dispatch, the command channel and screen state
  (the gameflow track's area), the recorder, and click to walk. Each lands
  with its tests, and `main` stays buildable and playable.
- Emulator changes: a new slot, `slots/play`. Changes to the HUD slot's
  machine modules or the core stay small, are documented, and keep the
  existing tests passing.
- Retail patches are named builds through `tools/retail_runtime/variants.py`
  and `tools/retail_asm/function_hook.py`; the baseline is never edited.
- Calls to OpenRouter only go through `tools/playtest/jev.py`, with the key
  from the environment and the spend logged per session.
