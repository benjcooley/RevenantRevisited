# Gameflow track

This worktree (`worktrees/gameflow`, branch `feature/gameflow`) owns the
**game** half of RevenantRevisited: everything that drives a player
through a session from app launch to the credits roll, plus the audio
subsystem that the rest of the experience rides on.

Sibling tracks (`feature/vfx`, `feature/ui`, and core gameplay on
`main`) own the look, the chrome, and the moment-to-moment combat /
movement / physics. Gameflow is the connective tissue: menus, scripts,
saves, dialogs, the world's persistence as you walk between sectors,
and end-of-game.

## Scope

| # | Track                                  | Scope owner |
|---|----------------------------------------|-------------|
| 0 | Audio system (mixing, music, spatial)  | gameflow    |
| 1 | Main menu                              | gameflow    |
| 2 | Settings screen                        | gameflow + ui (shared widgets) |
| 3 | New game                               | gameflow    |
| 4 | Starting scripts / starting location   | gameflow    |
| 5 | Save-game data + state capture         | gameflow    |
| 6 | Load / save UI + slot management       | gameflow    |
| 7 | Death and restart                      | gameflow    |
| 8 | Scripting engine                       | gameflow    |
| 9 | Dialog engine                          | gameflow    |
| 10 | Exits / doors / Misthaven return     | gameflow    |
| 11 | Game log panel                       | gameflow    |
| 12 | End-of-game + credits                | gameflow    |

Out of scope for this track:
- The render pipeline, particle/spell VFX, and the HUD itself (other
  tracks).
- Combat math and movement (core).
- Level data, art, sound assets (we ship code, not content; original
  data is read-only — see [project-distribution-model] in memory).

## Where this fits in the engine

```
                     ┌──────────────┐
                     │ TMainMenu    │  (greenfield: T1 ▼)
                     │  Screen      │
                     └──────┬───────┘
                            │  StartGame / LoadGame / Settings / Quit
                            ▼
   ┌────────────┐     ┌──────────────┐    ┌─────────────┐
   │ TSaveGame  │◀────│ TPlayScreen  │───▶│ TDeathPane  │
   │ (T5/T6)    │     │  (live now)  │    │   (T7)      │
   └────────────┘     └──┬─────┬─────┘    └─────────────┘
                         │     │
        ┌────────────────┘     └──────────────┐
        ▼                                     ▼
  ┌──────────────┐                     ┌─────────────────┐
  │ TScriptMgr   │◀──triggers (T8)──── │ TExit / doors   │
  │  (T8)        │                     │  (T10)          │
  └──────┬───────┘                     └─────────────────┘
         │ say / wait response
         ▼
  ┌──────────────┐         ┌─────────────────┐
  │ TDialogPane  │         │ TGameLogPane    │
  │  (T9)        │         │  (T11)          │
  └──────────────┘         └─────────────────┘

         ┌────────────── AUDIO (T0) ────────────────┐
         │ miniaudio mixer, .ogg music, 3D voices   │
         │ feeds TSound / TSoundPlayer + CD slot    │
         └──────────────────────────────────────────┘
```

## Current state (snapshot 2026-05-16)

Module / per-area / playscreen wiring is already in place — see
`src/module.{h,cpp}`, `src/playscreen.{h,cpp}`. The boot path
(`Initialize → SetCurModule`) reaches a frame loop without a Player
and accepts `--loadmap=<file>` as a startup shortcut.

What's still 1998 code, intact, waiting on integration + verification:
- `src/script.{h,cpp}` (~1075 lines; only 3 `#if 0` blocks, all for
  retired MSVC debug-heap instrumentation)
- `src/dialog.{h,cpp}` (~543 lines, intact)
- `src/savegame.{h,cpp}` (~250 lines, intact)
- `src/death.{h,cpp}` (~115 lines, intact)
- `src/exit.{h,cpp}` (~840 lines, intact)

What's broken / stubbed:
- `src/sound.{h,cpp}` — 34 `#if 0` blocks, all marked
  *"real audio backend — Phase 3"*. DirectSound + MCI cdaudio + winmm
  WAVE loader paths are all stubbed. **No audio library vendored yet.**
- No main menu / title screen / credits / game log pane in `src/`.

## Authority and inter-track etiquette

- `feature/gameflow` is the source of truth for everything in the scope
  table above. Other agents should not edit gameflow files; merge
  conflicts here will be resolved gameflow's way unless the user
  intervenes.
- Cross-cutting code (e.g. a settings struct edited by both ui and
  gameflow) gets coordinated through plan docs in `docs/ui/` and
  `docs/gameflow/`. When in doubt: ask the user.
- Per [feedback-keep-main-runnable] in memory, `main` must stay
  buildable + playable. Disruptive multi-step changes live in this
  worktree until they're known-good.

## Documents in this folder

- [README.md](README.md) — this file (charter, scope, current state).
- [PLAN.md](PLAN.md) — phased plan covering audio (T0) and the eleven
  gameflow tracks (T1–T12).
- [BURNDOWN.md](BURNDOWN.md) — per-track checklist tracking what's
  done, in-progress, blocked, or open. The thing to update as work
  lands. Designed to be the recovery point if this chat is lost.

## How to recover from a fresh session

If this conversation is lost, a new agent should:

1. Read this file plus [PLAN.md](PLAN.md) and [BURNDOWN.md](BURNDOWN.md).
2. `cd /Users/benjamincooley/projects/RevenantRevisited/worktrees/gameflow`
   and confirm `git status` is clean / on `feature/gameflow`.
3. Check `git log --oneline main..HEAD` to see what's landed beyond
   `main` since the plan was written.
4. Read the BURNDOWN; pick the first un-checked item; resume.
