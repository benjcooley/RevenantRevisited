# Revenant Revisited — alpha build for ${platform}

> **This is alpha software, not ready for prime time.** It is built
> automatically from work in progress: expect missing features, bugs and
> crashes. We are looking for testers: if something breaks or looks
> wrong, please tell us (see *Help test it* below).

A modern build of Cinematix Studios' *Revenant*, made from the
[Revenant Revisited](${repo_url}) source.

| | |
|---|---|
| Version | ${version} |
| Commit | [`${short}`](${commit_url}) on `${branch}` |
| Built | ${built} · ${run} |
| Newest build | ${latest_url} |

## You need the original game

This download holds no game data. Revisited plays your own copy of
*Revenant*, from GOG or the original CD.

## Install

1. **Make a copy of your Revenant game folder**, for instance
   `Revenant Revisited` beside `Revenant`. Revisited plays from the copy,
   so nothing it does can reach your original game or its saves.
2. Unzip this download into the root of the copy, beside the game's
   `Revenant.exe`. Nothing in the zip replaces a game file: the program
   is `${exe}`, and its own data is the `assets/` folder.
3. Run `${exe}`.

Like the original game, Revisited keeps its settings (`Revenant.ini`) and
saves (`Save/`) in the game folder: it starts with the copied settings and
lists and opens the saves that were there. Saves are meant to work in both
games, but that is still being tested, which is why Revisited gets a copy
of the folder for now.

## ${platform} notes

<!-- osx -->
- Apple silicon, macOS 13 or later.
- The program is not signed or notarized yet, so macOS blocks it when it
  first runs. In Terminal, in the folder you unzipped to, run once:

  ```sh
  xattr -dr com.apple.quarantine ${exe} assets
  ```
<!-- /osx -->
<!-- windows -->
- 64-bit Windows 10 or 11 and a Direct3D 11 graphics card.
- The program is not signed yet, so SmartScreen may stop it: choose
  **More info → Run anyway**.
- Revisited is developed and played on macOS. Every commit compiles and
  passes the unit tests on Windows, but the game sees far less play here.
<!-- /windows -->
<!-- linux -->
- x86-64 with glibc 2.39 or newer (Ubuntu 24.04, Fedora 40 or later),
  OpenGL 3.3, and X11 or XWayland.
- Uses libGL, libX11, libXi, libXcursor, libXrandr and ALSA (libasound2),
  which desktop installs already have.
- Revisited is developed and played on macOS. Every commit compiles and
  passes the unit tests on Linux, but the game sees far less play here.
<!-- /linux -->

## Help test it

Bug reports are the most useful thing you can give us. Open an issue at
${repo_url}/issues and include:

- this build's commit, `${short}` (in the table above), and that you
  play on ${platform};
- what you did, what happened, and what you expected;
- **screenshots** of anything that looks wrong: drag them into the
  issue. <!-- osx -->`⇧⌘4` captures part of the screen, `⇧⌘3` all of it.<!-- /osx --><!-- windows -->`Win`+`Shift`+`S` captures part of the screen.<!-- /windows --><!-- linux -->Your desktop's screenshot key (often `PrtSc`) captures the screen.<!-- /linux -->
  If you can, a shot of the original game at the same spot helps too;
- `revenant.log`, which the program writes to the folder it was started
  from;
- for trouble in a saved game, the slot's folder from `Save/Single/`,
  zipped.
