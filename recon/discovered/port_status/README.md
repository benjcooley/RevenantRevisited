# Port Status Tracker

Per-class registries of which methods have been **vetted** against retail line-by-line, which are **partial** ports (skeleton or known-incomplete), which are **incomplete** stubs, and which are intentionally **out-of-scope** (rendering, animation engine).

## Why this exists

We're porting retail TCharacter / TPlayer / combat / AI / scripting / saves into our pre-release-derived codebase. Without per-method tracking we lose track of which methods are retail-faithful, which are stubbed, and which we haven't touched yet — and porting drifts. This tracker is the single source of truth for "is this method done?".

## Status values

| Status | Symbol | Meaning |
|---|---|---|
| `vetted` | ✅ | Method compared line-by-line against retail decomp; matches retail (or has documented intentional deviations). Safe to rely on. |
| `partial` | 🟡 | Skeleton / stub / missing behavior. Compiles and works for some inputs but known-incomplete vs retail. Notes column says what's missing. |
| `incomplete` | 🔴 | Stub returning a default value, or doesn't exist in our source yet. Behavior is wrong or absent. |
| `out-of-scope` | ⚪ | Rendering / animation-engine body — labeled in recon for understanding but **not** ported. Our implementation lives independently. |
| `not-started` | ⚫ | Method recognized in retail but no port begun yet. |

## How to update entries

When you port (or vet) a method:

1. Read the retail body in `recon/discovered/cls_*_<method>_<addr>.cpp` (or the cls_0x...recon file).
2. Read our source method (if any).
3. Line-by-line compare. Note differences.
4. Update the row in the appropriate `<Class>.md` table:
   - `Status` column → new status
   - `Notes` column → what's still missing or intentional deviation
   - `Vetted by / date` → your name + ISO date
5. Commit alongside the code change.

## File layout

One file per class under `recon/discovered/port_status/`:

- `TCharacter.md` — combat/AI methods (the largest)
- `TPlayer.md` — player input + extensions
- `TObjectInstance.md` + `TComplexObject.md` — base-class virtuals
- `TActionBlock.md` — action-state struct
- `TMapPane.md` + `TSector.md` + `TArea.md` — world data (mostly out-of-scope or label-only)
- `TScript.md` + `TGameState.md` — scripting
- `TSaveGame.md` — save format

## Grepping work remaining

```bash
# What's not done yet:
grep -E "🔴 incomplete|⚫ not-started|🟡 partial" recon/discovered/port_status/*.md

# What's been vetted:
grep -E "✅ vetted" recon/discovered/port_status/*.md | wc -l

# All methods in scope per class:
grep -cv "out-of-scope\|^|---\|^| Method\|^#" recon/discovered/port_status/TCharacter.md
```

## Cross-references

- Retail decompilations: `recon/discovered/cls_*.cpp` + the per-class files in `recon/classes_original/`
- Rename map: `recon/discovered/renames/*.txt` (applied via `recon/scripts/apply_renames.sh`)
- Anchors / known mappings: `recon/discovered/README.md`
