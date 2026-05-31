#!/usr/bin/env bash
# *************************************************************************
#  defscreen_widget_tests.sh - per-widget interaction tests for DEF screens
# *************************************************************************
#
# Exercises EVERY interactive widget on each DEF screen via scripted input and
# captures a filmstrip per screen so each widget's response can be verified:
#   buttons (press down-state + click dispatch), checkbox toggles (flip),
#   sliders (arrow step + thumb drag), listbox (row select), EDIT (text entry).
#
# Button dispatch is also logged as "[ui-defscreen] clicked '<name>'" — grep the
# per-screen .log to confirm every button fired.
#
# Usage:  tools/ui/defscreen_widget_tests.sh [out_dir]   (default /tmp/dsw)
# Coords are Classic 640x480 content pixels.

set -u
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BIN="$ROOT/build/Revenant"
OUT="${1:-/tmp/dsw}"
mkdir -p "$OUT"
[[ -x "$BIN" ]] || { echo "build first: cmake --build build -j8" >&2; exit 1; }

run() {  # run <mode> <nframes> <script>
  local mode="$1" n="$2" script="$3"
  local prefix="$OUT/${mode#ui-}_"
  rm -f "${prefix}"*.png
  ( cd "$ROOT" && "$BIN" --test="$mode" --headless \
      --filmstrip="${n},0" --snapprefix="$prefix" \
      --input-script="$script" >"$OUT/${mode#ui-}.log" 2>&1 & )
  sleep $((6 + n))
  pkill -f "Revenant --test=$mode" 2>/dev/null
  echo "=== $mode ==="
  grep -c "clicked" "$OUT/${mode#ui-}.log" | sed 's/^/  button clicks dispatched: /'
  grep "clicked" "$OUT/${mode#ui-}.log" | sed -E "s/.*clicked '(.*)'.*/    - \1/"
  ls "${prefix}filmstrip-001.png" 2>/dev/null | sed 's/^/  /'
}

# ---- InGameMenu: press each of the 6 buttons (down-state + dispatch) --------
IGM="moveto 408 141 0; left_down; pause 120; take_snapshot; left_up; pause 60; \
moveto 408 178 0; left_down; left_up; \
moveto 408 215 0; left_down; left_up; \
moveto 408 252 0; left_down; left_up; \
moveto 408 289 0; left_down; left_up; \
moveto 408 326 0; left_down; pause 120; take_snapshot; left_up; pause 80; take_snapshot"
run ui-ingamemenu 3 "$IGM"

# ---- Options: flip all 6 toggles, set 4 sliders to distinct positions, OK ---
OPT="pause 200; take_snapshot; \
moveto 63 78 0;  left_down; left_up; moveto 63 98 0;  left_down; left_up; \
moveto 63 118 0; left_down; left_up; moveto 63 138 0; left_down; left_up; \
moveto 63 158 0; left_down; left_up; moveto 63 178 0; left_down; left_up; pause 120; take_snapshot; \
moveto 430 80 0;  left_down; moveto 445 80 120;  left_up; \
moveto 560 113 0; left_down; moveto 480 113 120; left_up; \
moveto 460 146 0; left_down; moveto 560 146 120; left_up; \
moveto 480 179 0; left_down; moveto 575 179 120; left_up; pause 150; take_snapshot; \
moveto 230 425 0; left_down; left_up; pause 60"
run ui-options 3 "$OPT"

# ---- SaveGame: select a row, focus EDIT + clear + type, press Save Game -----
SAV="pause 200; take_snapshot; \
moveto 430 221 0; left_down; left_up; pause 100; take_snapshot; \
moveto 476 99 0; left_down; left_up; pause 80; \
key_down backspace; key_down backspace; key_down backspace; key_down backspace; \
key_down backspace; key_down backspace; key_down backspace; key_down backspace; \
key_down backspace; key_down backspace; \
key_down h; key_down e; key_down r; key_down o; key_down space; key_down s; key_down a; key_down v; key_down e; \
pause 150; take_snapshot; \
moveto 228 421 0; left_down; left_up; pause 60"
run ui-savegame 3 "$SAV"

# ---- LoadGame: select rows, press Load Game + Exit --------------------------
LOA="pause 200; take_snapshot; \
moveto 430 110 0; left_down; left_up; pause 80; \
moveto 430 200 0; left_down; left_up; pause 100; take_snapshot; \
moveto 228 421 0; left_down; left_up; \
moveto 420 421 0; left_down; left_up; pause 60"
run ui-loadgame 2 "$LOA"

echo ""
echo "Filmstrips + logs under: $OUT"
