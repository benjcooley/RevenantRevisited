# Save interop test — port saves in retail, retail saves in the port

Port saves must load in retail Revenant (dosbox-x), and retail saves must
load in the port, with all state intact. The format work is in
[SAVE_GAME.md](forensics/SAVE_GAME.md) §10–§11. This page is the manual
check in retail, plus how to rerun the byte-level checks behind it.

## 1. What is already verified (no retail run needed)

All of these run on the Mac with `tools/savefmt/revsave.py`:

| Check | Result |
|---|---|
| Retail slot `New Game1` loaded in the port and saved before the first tick (`Port Resave`), `game.sav` vs the retail file | identical except game time and one player flag bit (AI, `0x20`; retail never reads it back, §3) |
| Same, sector files | 243 of 283 byte-identical, 36 more equal ignoring the constructor-owned flags, 4 differ by retail's own load rules (§3) |
| Sector state hashes (`statehash`) | every port-written sector verifies; the same code reproduces all 558 retail-written sectors and 4,822 of the 4,823 hashed shipped base-map sectors (`46_6_9.dat` ships with a stale hash) |
| Thumbnails (`bmp`) | retail's format, 216×160, 24-bit, 103,734 bytes |
| Port save loaded in the port and saved again | `game.sav` identical except game time, 567 of 567 sectors byte-identical |
| Retail `New Game1` in the port | Locke on level 0 at (5806, 24815, 34), level 1, 9 inventory items including a spell pouch with 4 talismans, quick spells, known spells, 82 game states, automap of levels 0, 2, 6: all checked field by field |

What isn't verified: retail itself loading a port save. That needs
dosbox-x, below.

## 2. The test slots

`docs/gameflow/SAVE_INTEROP_SLOTS.zip` (Git LFS) holds three slot
folders written by the port:

| Slot | Made from | Use |
|---|---|---|
| `Port Resave` | retail `New Game1`, loaded and saved before the first tick | main check: should be indistinguishable from `New Game1` |
| `Port Played` | retail `New Game1`, loaded, 600 frames of play, saved | a save after the port's simulation has run: monsters and NPCs may have moved |
| `Port New Game` | the module's `newgame.sav`, saved before the first tick | the new-game player (a v14 record written back as v15) and level 2 (the Keep) written by the port |

To regenerate them (about 6 minutes; needs a build and the retail slot):

```
tools/savefmt/make_interop_slots.sh "<retail slot>/New Game1" <output dir>
```

Set `REVENANT_DATA_PATH=<install>` when the build can't find the
data (for example in a worktree whose `data/` holds LFS pointers). The
script runs the port in a scratch `REVENANT_SAVE_PATH` and never writes
into the install. Two runs of `Port New Game` can differ in one
character's walk animation: loading picks among weighted animation
variants at random, as retail does.

## 3. Differences to expect (all deliberate or retail behaviour)

- **Player flag `0x20` (AI)** is set in port saves, clear in retail's.
  Retail's `TObjectInstance::Load` takes that bit from the constructor,
  never from the file, so it can't change anything in retail.
- **36 sectors differ only in constructor-owned flags** (MOVING, set by
  the port's animators on objects off screen); ignored by retail's load
  for the same reason.
- **4 sectors differ by retail's own load rules.** Six characters in
  `0_5_22` and one in `0_5_24` were saved mid-action; loading puts each
  back in its root animation (retail's `TComplexObject::Load` does the
  same). Two characters in `0_0_29` / `0_1_29` were saved with 1,800
  health; retail's `TCharacter::Load` clamps a character to its
  character data (725 and 25), and in retail's game something raised
  it again after loading. Expect those characters to look the same as
  after loading `New Game1` in retail.
- **284 more sector files** than the retail slot: the port loads the
  player's whole level and saves every sector it loaded; retail saved
  only the sectors it had streamed in. The extra files are level 0's
  base-map sectors written back in version 15, and retail reads them
  in place of the base map.
- **`Port Played`** also differs in what 600 frames of the port's
  simulation changed: Locke's animation frame, his health and mana (they
  recover toward 100 and 105: 40 → 43, 7 → 10) and recovery timers, his
  last-poison-damage time (-1 in retail's file; not looked into), his
  play clock (`statetime`, +2496: 600 frames of game time, which retail's
  `TPlayer::Animate` advances too), and creatures that moved (12
  of the 283 sectors differ, against 4 for `Port Resave`).
- **Thumbnails** are the port's own rendering. `Port Resave`'s is solid
  magenta: it was captured from the first frame after the load, which
  the port presents before drawing the world (a port rendering issue,
  not a format one). The other two show the game.

## 4. Port saves in retail (dosbox-x)

`<install>` is the retail game directory inside the dosbox-x guest (the
one with `Revenant.exe`); retail lists one save per folder in
`<install>\Save\Single\`.

1. **Back up** `<install>\Save\Single\` (and `<install>\Curmap\`, which
   every load rewrites).
2. **Unzip** `SAVE_INTEROP_SLOTS.zip` on the Mac and copy the folders
   `Port Resave`, `Port Played` and `Port New Game` into
   `<install>\Save\Single\`, beside `New Game1`. Each folder holds
   `game.sav`, `ss.bmp` and `CurMap\`.
3. Start retail, choose **Load Game**. Expected: all three slots listed
   by name, each with a thumbnail (magenta for `Port Resave`, §3), no
   error.
4. **Reference first:** load `New Game1`. Take a screenshot and note the
   items in the checklist below.
5. Load **`Port Resave`** and go through the checklist. Every item
   should match step 4.
6. Load **`Port Played`**: the same checklist; nearby creatures may
   stand elsewhere.
7. **Round trip:** with `Port Resave` loaded, save from retail's save
   dialog as `Retail From Port`. Copy that folder back to the Mac and
   run

   ```
   python3 tools/savefmt/revsave.py diff "Port Resave/game.sav" "Retail From Port/game.sav"
   python3 tools/savefmt/revsave.py cmpdir --fixed-flags "Port Resave/CurMap" "Retail From Port/CurMap"
   ```

   Expected: the player record matches except state time and what a few
   seconds of play change (position if Locke moved, recovery timers).
   Sectors: differences only where creatures moved.
8. Load **`Port New Game`**. Expected: Locke in the Keep's resurrection
   chamber on level 2, level 1, 25/100 health, no items, no spells. The
   module's `newgame.sav` stores Locke with INVISIBLE set and the
   opening scene clears it; please note whether Locke is visible and
   whether the opening plays. Either way it tells us what retail does
   with a save made at that moment; a mismatch here is not necessarily
   a port bug.

### Checklist (steps 4–6)

| Item | `New Game1` (from the file) |
|---|---|
| Location | level 0, The Forest; Locke at world (5806, 24815), sector 5_24 |
| Level, experience | 1, 50 of 300 |
| Health, fatigue, mana | 40, 78, 7 (current values; compare the bars) |
| Attributes | strength 18, constitution 12, agility 14, reflexes 14, mind 14, luck 14 |
| Inventory (9) | Spell Pouch (Life, Moon, Soul, Sky talismans), Short Sword, Black Cloth Boots, Brown Cloth Pants, Red Cloth Shirt, 601 Gold, Lesser Healing, Watermelon, Scroll (`MGCSCRL1`) |
| Equipped | as in `New Game1` (compare the paper doll) |
| Quick spell buttons | construct slot `LI`, buttons `LI`, `E`, `BE`, `L` |
| Known spells | `B`, `E`, `BE`, `L`, `LI` |
| Sidebar | open; upper and lower panels in the same modes as `New Game1` |
| Automap | explored areas on levels 0, 2 and 6 as in `New Game1` |
| Story state | talk to a nearby NPC who has spoken before; the conversation continues where `New Game1` left it |
| World | doors, chests, dropped items and creatures near Locke as in `New Game1` |

Report per slot: loads (yes / no / crash and message), checklist items
that differ, and the step 7 diff output.

## 5. Retail saves in the port

Verified with `New Game1` (§1). For another retail save:

1. Copy the slot folder from `<install>\Save\Single\` to
   `~/Library/Application Support/Revenant/Save/Single/` (the port's
   `<SavePath>/Save/Single/`).
2. `build/Revenant --quickstart="<slot name>"` loads it directly. Check
   the log line `[savegame] loaded …` (module, version, player, level,
   position, item count), then play.
3. Byte check: `tools/savefmt/make_interop_slots.sh "<slot>" <dir>`, then
   `revsave.py diff "<slot>/game.sav" "<dir>/Port Resave/game.sav"` and
   `revsave.py cmpdir --fixed-flags "<slot>/CurMap" "<dir>/Port Resave/CurMap"`.
   Expected differences are the ones in §3.

## 6. Tools

`tools/savefmt/revsave.py`:

| Command | Does |
|---|---|
| `dump <file>` | every field of a `game.sav`, `newgame.sav` or sector file, with offsets |
| `diff [--fixed-flags] <a> <b>` | field-level diff of two files of the same kind; ignores game time |
| `cmpdir [--fixed-flags] <dirA> <dirB>` | `diff` for every sector file present in both directories |
| `statehash <file or dir> …` | recomputes each sector's state hash and compares it with the stored one |
| `bmp <ss.bmp> …` | checks a thumbnail against retail's format |

`--fixed-flags` ignores the object flag bits retail's load takes from
the constructor (`0xc00e0028`: MOVING, AI, COMPLEX, NOTIFY, NONMAP,
INVENTORY, CALLEDPREDEL).
