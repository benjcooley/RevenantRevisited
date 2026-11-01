#!/bin/sh
# The seal's test for the object records (docs/OBJECT_MODEL_PLAN.md phase 5):
# every object of every level, and the new game's player, through
# Capture -> Apply -> Capture; the records must match.
#
#   tools/state_gate.sh            every level (0..99); prints the levels that
#                                  differ, exit 1 if any does
#
# Needs the retail data installed (local only; CI has none). ~2 minutes.
here="$(cd "$(dirname "$0")/.." && pwd)"
bin="$here/build/Revenant"
status=0
for level in $(seq 0 99); do
    out="$("$bin" --headless --test=state-roundtrip --level="$level" --max-runtime=120 2>&1 | grep "state-roundtrip\] level" | sed 's/.*INFO  //')"
    [ -n "$out" ] || continue
    echo "$out"
    case "$out" in *" 0 differ") ;; *) status=1 ;; esac
done
exit $status
