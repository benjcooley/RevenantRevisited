# Revenant Revisited — development build for ${platform}

A modern build of Cinematix Studios' *Revenant*, made from the
[Revenant Revisited](${repo_url}) source. It is built automatically from
work in progress: expect missing features, bugs and crashes.

| | |
|---|---|
| Commit | [`${short}`](${commit_url}) on `${branch}` |
| Built | ${built} · ${run} |
| Newest build | ${latest_url} |

## You need the original game

This download holds no game data. Revisited plays your own copy of
*Revenant*, from GOG or the original CD, and never changes its files.

## Install

1. Unzip into your Revenant install folder, beside the game's own program.
   Nothing in the zip replaces a file of the game's: this program is
   `${exe}`, and its own data stays in `assets/`.
2. Run `${exe}`.

To keep it elsewhere, unzip it anywhere (`assets/` must stay beside the
program) and set the environment variable `REVENANT_DATA_PATH` to the
install folder.

Settings and saves go to your user folder, never the install:
<!-- osx -->`~/Library/Application Support/Revenant/`.<!-- /osx --><!-- windows -->`%LOCALAPPDATA%\Revenant\`.<!-- /windows --><!-- linux -->`$$XDG_DATA_HOME/Revenant/` (usually `~/.local/share/Revenant/`).<!-- /linux -->

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

## Reporting problems

Open an issue at ${repo_url}/issues. Name the commit above and attach
`revenant.log`, which the program writes to the folder it was started from.
