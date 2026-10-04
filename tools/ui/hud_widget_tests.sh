#!/usr/bin/env bash
# *************************************************************************
#  hud_widget_tests.sh - per-panel interaction tests for the in-game HUD
# *************************************************************************
#
# Mirrors defscreen_widget_tests.sh for the HUD side. Exercises every
# interactive widget on each HUD panel via the --input-script system and
# captures a filmstrip per panel so the user (and other agents) can browse
# the current state of each panel and its widgets at a glance.
#
# Covers: sidebar tabs (incl. Book↔Spell + Equip↔Inv auto-pairing),
# inventory page arrows + click-drag + cross-pane drag + bag-open right-
# click, spellbook scroll arrows, QuickSpell ring/label, BarInv slots,
# PlyrStatusBar, TextBar, BottomBar chrome, and each sidebar slot's
# chrome (Equip/Stats/Spellbook + Inventory/Map/SpellCreate).
#
# Usage:  tools/ui/hud_widget_tests.sh [out_dir]
#   default out_dir = ./filmstrips
#
# Every run writes:
#   <out>/<panel>/<script>-NNN-<label>.png  -- one per take_snapshot
#   <out>/<panel>/<script>-filmstrip-001.png -- NxN grid composite
#   <out>/<panel>.log                        -- engine log
# *************************************************************************

set -u
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BIN="$ROOT/build/Revenant"
OUT="${1:-$ROOT/filmstrips}"
mkdir -p "$OUT"
[[ -x "$BIN" ]] || { echo "build first: cmake --build build -j8" >&2; exit 1; }

# run <mode> <name> <nframes> <script>
#   mode    -- --test=<mode>
#   name    -- subfolder under $OUT and filmstrip filename prefix
#   nframes -- frames for --filmstrip=N,0 (manual mode)
#   script  -- inline --input-script semicolon-separated
run() {
  local mode="$1" name="$2" n="$3" script="$4"
  local prefix="$OUT/${name}/run-"
  mkdir -p "$OUT/${name}"
  rm -f "${prefix}"*.png
  # Launch with `&` at the outer level so $! is the actual child PID we
  # can kill (the inner subshell + cd pattern hides $! from the parent).
  ( cd "$ROOT" && exec "$BIN" --test="$mode" --headless \
      --filmstrip="${n},0" --snapprefix="${name}/run-" \
      --input-script="$script" ) >"$OUT/${name}.log" 2>&1 &
  local pid=$!
  # Wait long enough for: boot (~3s) + input-sim script + N captures +
  # final filmstrip composite write. Most scripts run in 4-10s; give a
  # generous 12 + n sec budget so the composite write isn't cut off.
  local wait_s=$(( 12 + n ))
  sleep "$wait_s"
  # Poll-wait an extra couple of seconds for the filmstrip composite
  # write to complete before killing (manual mode keeps running after
  # captures finish; we want it to land its FlushOutputs first).
  for _ in 1 2 3 4 5 6; do
    [[ -f "${prefix}filmstrip-001.png" ]] && break
    sleep 0.5
  done
  kill "$pid" 2>/dev/null || true
  wait "$pid" 2>/dev/null || true
  echo "=== $name ($mode, $n frames) ==="
  # grep -c always prints a count (0 if no matches) but exits 1 on 0 — so
  # use `|| true` rather than `|| echo 0` to avoid a duplicate '0' line.
  local clicks
  clicks=$(grep -cE "\[ui-sidebar\] (click|inv|spellbook|right-click)|\[drag\]" "$OUT/${name}.log" 2>/dev/null || true)
  echo "  widget events dispatched: ${clicks:-0}"
  if [[ -f "${prefix}filmstrip-001.png" ]]; then
    echo "  ${prefix}filmstrip-001.png"
  else
    echo "  WARN: no filmstrip produced"
  fi
}

# ===== sidebar tab strip — 6 buttons, pairing logic ======================
SIDETABS_PAIRING="\
log baseline; moveto 100 100 0; pause 300; take_snapshot baseline; \
moveto 601 43 200; pause 100; left_down; pause 80; left_up; pause 250; take_snapshot click upper Book; \
moveto 601 80 200; pause 100; left_down; pause 80; left_up; pause 250; take_snapshot click upper Stats; \
moveto 601 117 200; pause 100; left_down; pause 80; left_up; pause 250; take_snapshot click upper Equip; \
moveto 601 154 200; pause 100; left_down; pause 80; left_up; pause 250; take_snapshot click lower Spell; \
moveto 601 191 200; pause 100; left_down; pause 80; left_up; pause 250; take_snapshot click lower Inv; \
moveto 601 228 200; pause 100; left_down; pause 80; left_up; pause 250; take_snapshot click lower Map; \
pause 200; take_snapshot final"
run ui-hud sidebar_tabs 8 "$SIDETABS_PAIRING"

# ===== inventory L/R page arrows ========================================
INV_ARROWS="\
log baseline; moveto 100 100 0; pause 300; take_snapshot baseline page=0; \
moveto 625 330 200; pause 100; left_down; pause 80; left_up; pause 200; take_snapshot right click page=1; \
moveto 625 330 100; pause 100; left_down; pause 80; left_up; pause 200; take_snapshot right click page=2; \
moveto 625 330 100; pause 100; left_down; pause 80; left_up; pause 200; take_snapshot right click page=3; \
moveto 625 330 100; pause 100; left_down; pause 80; left_up; pause 200; take_snapshot right click page=4; \
moveto 604 329 200; pause 100; left_down; pause 80; left_up; pause 200; take_snapshot left click page=3"
run ui-hud inventory_arrows 6 "$INV_ARROWS"

# ===== inventory item drag (intra-pane) =================================
INV_DRAG="\
log baseline; moveto 100 100 0; pause 300; take_snapshot baseline; \
moveto 480 368 200; pause 100; left_down; pause 100; \
moveto 615 456 400; pause 100; take_snapshot mid drag slot 0 to 11; \
left_up; pause 200; take_snapshot drop committed; \
moveto 525 456 200; pause 100; left_down; pause 100; take_snapshot pickup slot 5; \
moveto 200 200 300; pause 100; left_up; pause 200; take_snapshot release over empty (cancel); \
pause 100; take_snapshot final"
run ui-hud inventory_drag 6 "$INV_DRAG"

# ===== inventory cross-pane drag (Inv ↔ BarInv ↔ Equip) + bag-open =====
INV_XPANE="\
log baseline; moveto 100 100 0; pause 300; take_snapshot baseline; \
moveto 480 368 200; pause 100; left_down; pause 80; moveto 240 450 300; pause 80; left_up; pause 200; take_snapshot inv0 to barinv0; \
moveto 525 412 200; pause 100; left_down; pause 80; moveto 546 57 400; pause 80; left_up; pause 200; take_snapshot inv4 to equip head; \
moveto 240 450 200; pause 100; left_down; pause 80; moveto 615 456 400; pause 80; left_up; pause 200; take_snapshot barinv0 to inv11; \
moveto 546 57 200; pause 100; left_down; pause 80; moveto 600 450 400; pause 80; left_up; pause 200; take_snapshot equip head to barinv8; \
moveto 480 368 200; pause 100; right_down; pause 80; right_up; pause 200; take_snapshot bag opened slot 0; \
moveto 480 368 100; pause 100; right_down; pause 80; right_up; pause 200; take_snapshot bag closed back to root; \
pause 100; take_snapshot final"
run ui-hud inventory_cross_pane 8 "$INV_XPANE"

# ===== spellbook scroll arrows ==========================================
SPB_SCROLL="\
log switch_to_book; moveto 601 43 200; pause 100; left_down; pause 80; left_up; pause 200; take_snapshot Book view scroll=0; \
moveto 631 187 200; pause 100; left_down; pause 80; left_up; pause 200; take_snapshot scroll down 1 = 40; \
moveto 631 187 100; pause 100; left_down; pause 80; left_up; pause 200; take_snapshot scroll down 2 = 80; \
moveto 631 187 100; pause 100; left_down; pause 80; left_up; pause 200; take_snapshot scroll down 3 = 120; \
moveto 631 163 200; pause 100; left_down; pause 80; left_up; pause 200; take_snapshot scroll up 1 = 80; \
moveto 631 163 100; pause 100; left_down; pause 80; left_up; pause 200; take_snapshot scroll up 2 = 40"
run ui-hud spellbook_scroll 6 "$SPB_SCROLL"

# ===== sidebar unit (no full HUD) =======================================
# Just snap the sidebar component on its own to confirm tab+pane composition
# without the other HUD layers cluttering the view.
SIDEBAR_BASIC="\
log baseline; pause 400; take_snapshot baseline; \
pause 1500; take_snapshot synthetic-cycle 1; \
pause 1500; take_snapshot synthetic-cycle 2; \
pause 3500; take_snapshot synthetic-cycle 3 (likely closed)"
run ui-sidebar sidebar 4 "$SIDEBAR_BASIC"

# ===== panel-only snaps (chrome rendering verification) =================
# Each just snaps the panel once so you can browse 'current state' for any
# panel without setting up an interaction script. Cycle to ~4 frames so the
# synthetic-state cycling in the test mode hits varied poses where applicable.

# Equip paperdoll
run ui-equip          equip          4 "pause 400; take_snapshot baseline; pause 800; take_snapshot mid; pause 800; take_snapshot late; pause 800; take_snapshot final"
# Stats sidebar
run ui-stats          stats          2 "pause 400; take_snapshot baseline; pause 800; take_snapshot late"
# Spellbook (top-slot tall parchment scroll)
run ui-spellbook      spellbook      2 "pause 400; take_snapshot baseline; pause 800; take_snapshot late"
# Inventory grid alone
run ui-inventory      inventory      2 "pause 400; take_snapshot baseline; pause 800; take_snapshot late"
# Map / automap
run ui-map            map            2 "pause 400; take_snapshot baseline; pause 800; take_snapshot late"
# Spell creation / talisman composer
run ui-spellcreate    spellcreate    2 "pause 400; take_snapshot baseline; pause 800; take_snapshot late"
# QuickSpell ring strip (verifies the +4/+4 ring offset + word split fix)
run ui-quickspell     quickspell     4 "pause 400; take_snapshot baseline; pause 1000; take_snapshot pressed-state; pause 1000; take_snapshot disabled-state; pause 1000; take_snapshot late"
# BarInv 9-slot quick shelf
run ui-barinv         barinv         2 "pause 400; take_snapshot baseline; pause 800; take_snapshot late"
# BottomBar chrome only
run ui-bottombar      bottombar      1 "pause 500; take_snapshot baseline"
# Player stat bar (top of screen)
run ui-plyrstatusbar  plyrstatusbar  2 "pause 400; take_snapshot baseline; pause 800; take_snapshot late"
# TextBar (transparent overlay)
run ui-textbar        textbar        2 "pause 400; take_snapshot baseline; pause 800; take_snapshot late"
# Side-tabs strip alone (used to be the only verified component)
run ui-sidetabs       sidetabs       4 "pause 400; take_snapshot baseline; pause 1500; take_snapshot cycle1; pause 1500; take_snapshot cycle2; pause 1500; take_snapshot cycle3"
# Main menu / TLogoScreen (OOG; included so the HUD agent can also eyeball it)
run ui-mainmenu       mainmenu       1 "pause 500; take_snapshot baseline" 2>/dev/null
# Loading screen
run ui-loadscreen     loadscreen     1 "pause 500; take_snapshot baseline" 2>/dev/null
# Death screen
run ui-death          death          1 "pause 500; take_snapshot baseline" 2>/dev/null

echo ""
echo "DONE -> $OUT"
echo "  browse:  open $OUT/<panel>/run-filmstrip-001.png"
ls "$OUT" 2>/dev/null | sed 's/^/  /'
