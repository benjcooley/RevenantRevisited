#!/bin/bash
# Apply network-effect renames across recon/. Reads rename pairs from
# every .txt file in recon/discovered/renames/ (one BARE|RENAMED per line,
# # comments ignored) and rewrites every .cpp under recon/ in place.
#
# Multiple parallel agents can append findings to their own .txt file in
# recon/discovered/renames/ without conflicting. Re-run this script to
# apply the merged set; idempotent because already-renamed tokens no
# longer match the bare pattern.
#
# Usage: bash recon/scripts/apply_renames.sh

set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
RECON="$ROOT/recon"
RENAME_DIR="$RECON/discovered/renames"

# Collect rename pairs across all .txt files. Sort by bare-token length DESC
# so longer tokens get rewritten first (avoids `meth_0x...` rewriting a
# substring of `virt_meth_0x...`).
TMPMAP=$(mktemp)
trap "rm -f $TMPMAP" EXIT

find "$RENAME_DIR" -maxdepth 1 -type f -name "*.txt" -print0 |
while IFS= read -r -d '' f; do
  grep -E '^[A-Za-z_][^|]*\|[^[:space:]]+$' "$f" || true
done | awk -F'|' '{ print length($1), $1, $2 }' | sort -rn | awk '{ print $2 "|" $3 }' > "$TMPMAP"

PAIR_COUNT=$(wc -l < "$TMPMAP" | tr -d ' ')
echo "applying $PAIR_COUNT rename pairs across $(find "$RECON" -type f -name "*.cpp" | wc -l | tr -d ' ') .cpp files"

# Build a single perl pass with all pairs to avoid re-walking files.
PERL_SCRIPT=$(awk -F'|' '{
  bare=$1; new=$2;
  gsub(/\\/, "\\\\\\\\", bare); gsub(/\//, "\\/", bare);
  gsub(/\\/, "\\\\\\\\", new);  gsub(/\//, "\\/", new);
  print "s/\\b" bare "(?![A-Za-z0-9_])/" new "/g;"
}' "$TMPMAP")

if [ -z "$PERL_SCRIPT" ]; then
  echo "no rename pairs found in $RENAME_DIR"
  exit 0
fi

find "$RECON" -type f -name "*.cpp" -print0 |
xargs -0 perl -i -pe "$PERL_SCRIPT"

echo "rename pass complete."
