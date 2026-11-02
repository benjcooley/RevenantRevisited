#!/bin/sh
# Byte-for-byte gate for the retail object codec (docs/OBJECT_MODEL_PLAN.md).
#
#   tools/sector_gate.sh <outdir>      dump every level's sectors as the game
#                                      re-serializes them, into <outdir>/L_X_Y.DAT
#   diff -rq <before> <after>          nothing may differ but the known cases
#                                      listed in the plan (one object with a
#                                      garbage z beyond float exactness; two
#                                      NPCs whose idle state is a random pick)
#
# Needs the retail data installed (local only; CI has none). ~3 minutes.
set -e
out="$1"
if [ -z "$out" ]; then echo "usage: $0 <outdir>" >&2; exit 2; fi
here="$(cd "$(dirname "$0")/.." && pwd)"
bin="$here/build/Revenant"
mkdir -p "$out"
for level in $(seq 0 99); do
    "$bin" --headless --test=sector-dump --level="$level" --dump-out="$out" --max-runtime=120 2>&1 \
        | grep "\[sector-dump\] level" | sed 's/.*INFO  //' || true
done
echo "$(ls "$out" | wc -l | tr -d ' ') sectors in $out"
