#!/usr/bin/env bash
# *************************************************************************
#  defscreen_verify.sh - scripted headless verification of the DEF screens
# *************************************************************************
#
# Drives each DEF-engine screen (src/defscreen.{h,cpp}) headlessly with a
# scripted input sequence and writes a filmstrip PNG per screen so the static
# layout AND the interactions (button press, checkbox toggle, slider drag,
# listbox row-select) can be inspected without a visible window.
#
# Uses the engine's --headless / --filmstrip=N,0 (manual) / --input-script
# facilities. Coordinates are Classic 640x480 content pixels.
#
# Usage:  tools/ui/defscreen_verify.sh [out_dir]
#         out_dir defaults to /tmp/defscreen_verify
#
# Output: <out_dir>/<screen>_filmstrip-001.png  (NxN grid, one cell per snap)
#         plus the individual <screen>_NNN-*.png frames.

set -u
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BIN="$ROOT/build/Revenant"
OUT="${1:-/tmp/defscreen_verify}"
mkdir -p "$OUT"

if [[ ! -x "$BIN" ]]; then
  echo "error: $BIN not found — build first (cmake --build build -j8)" >&2
  exit 1
fi

run() {  # run <mode> <nframes> <script>
  local mode="$1" n="$2" script="$3"
  local prefix="$OUT/${mode#ui-}_"
  rm -f "${prefix}"*.png
  echo "=== $mode ==="
  ( cd "$ROOT" && "$BIN" --test="$mode" --headless \
      --filmstrip="${n},0" --snapprefix="$prefix" \
      --input-script="$script" >/dev/null 2>&1 & )
  sleep $((6 + n))
  pkill -f "Revenant --test=$mode" 2>/dev/null
  ls "${prefix}filmstrip-001.png" 2>/dev/null || echo "  (no filmstrip written)"
}

# InGameMenu: snapshot, press "Load Game" (down-state), release.
run ui-ingamemenu 3 \
"moveto 408 141; pause 250; take_snapshot; left_down; pause 200; take_snapshot; left_up; pause 200; take_snapshot"

# Options: snapshot, toggle the "Auto" checkbox on, drag the Music slider to max.
run ui-options 3 \
"pause 200; take_snapshot; moveto 63 98; left_down; pause 80; left_up; pause 150; take_snapshot; moveto 440 113; left_down; moveto 560 113 300; pause 80; left_up; pause 150; take_snapshot"

# SaveGame: snapshot (Misty Caves selected), click row 3 (Tomb of Set-amun).
run ui-savegame 2 \
"pause 200; take_snapshot; moveto 430 185; left_down; pause 80; left_up; pause 150; take_snapshot"

# LoadGame: snapshot, click a lower row.
run ui-loadgame 2 \
"pause 200; take_snapshot; moveto 430 175; left_down; pause 80; left_up; pause 150; take_snapshot"

echo ""
echo "Filmstrips written under: $OUT"
