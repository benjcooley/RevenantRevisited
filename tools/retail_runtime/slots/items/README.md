# Items slot: retail item behaviour in the emulator

The gameplay track's item fixtures: original item code run over the HUD
slot's booted world (`../hud/hudworld.py`, its cached boot image), where the
class tables, the imagery and the game's messages are retail's own, so a
fixture makes real items with retail's NewObject / AddToInventory. Each
fixture checkpoints the world and restores it per case; with `--serve` it
speaks retail_ab.py's JSONL (`{"id", "case"}` -> `{"id", "ok", "result"}`).

| Fixture | Runs | A/B target |
|---|---|---|
| `food_use.py` | TFood::Use `0x0050e780` (FOOD's and POTION's Use) on a real item and a fixture user; records the stat writes, FindSound / Play, the text bar and the cure's map scan (docs/gameplay/forensics/CONSUMABLES.md) | `item-food-use` (`tools/retail_ab/targets_items.py`, port side `src/retailab_items.cpp`) |

```sh
PY=/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python
"$PY" tools/retail_runtime/slots/items/food_use.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe            # one Bread, as an example
python3 tools/retail_ab/retail_ab.py item-food-use          # every case, both sides
```

The first run in a checkout boots the world (about four minutes) and caches
the image under `../hud/__pycache__/`; later runs load it in seconds.
