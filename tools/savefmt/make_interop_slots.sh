#!/bin/zsh
# Writes the port-saved test slots for docs/gameflow/SAVE_INTEROP_TEST.md.
#
#   tools/savefmt/make_interop_slots.sh <retail slot dir> <output dir>
#
# <retail slot dir> is a retail save slot (game.sav, ss.bmp, CurMap/), e.g.
# "New Game1". The output dir gets three slots, each saved by the port:
#   Port New Game  - a new game (the module's newgame.sav), saved before the
#                    first simulation tick
#   Port Resave    - the retail slot loaded and saved before the first tick
#   Port Played    - the retail slot loaded, played for 600 frames, saved
# Every run is isolated under a scratch REVENANT_SAVE_PATH; the install is
# only read (set REVENANT_DATA_PATH when the build can't find it, e.g. in a
# worktree whose data/ holds LFS pointers).
set -e
if [ $# -ne 2 ]; then
  echo "usage: $0 <retail slot dir> <output dir>" >&2
  exit 2
fi
SLOT_SRC=${1:A}
OUT=${2:A}
HERE=${0:A:h}
ROOT=${HERE:h:h}
BIN=$ROOT/build/Revenant
WORK=$(mktemp -d)
mkdir -p "$OUT"

run() {   # run <slot name to write> <quickstart arg> <frames>
  local name=$1 quick=$2 frames=$3
  rm -rf "$WORK/run" && mkdir -p "$WORK/run/Save/Single"
  cp "$HOME/Library/Application Support/Revenant/Revenant.ini" "$WORK/run/"
  cp -R "$SLOT_SRC" "$WORK/run/Save/Single/Retail"
  REVENANT_SAVE_PATH="$WORK/run" \
    "$BIN" $quick --headless --savecycle-test=$frames --max-runtime=120 > "$WORK/$name.log" 2>&1 || true
  if [ ! -f "$WORK/run/Save/Single/savecycle/game.sav" ]; then
    echo "$name: no save written; see $WORK/$name.log" >&2
    exit 1
  fi
  rm -rf "$OUT/$name"
  cp -R "$WORK/run/Save/Single/savecycle" "$OUT/$name"
  echo "$name: $(ls "$OUT/$name/CurMap" | wc -l | tr -d ' ') sectors, $(stat -f %z "$OUT/$name/game.sav") byte game.sav"
}

run "Port New Game" --quickstart 0
run "Port Resave" --quickstart=Retail 0
run "Port Played" --quickstart=Retail 600
rm -rf "$WORK"
