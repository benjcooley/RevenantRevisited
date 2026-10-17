# Save files

The port saves a slot as one file, `<SaveGamePath>Single/<name>.rvsave`,
instead of retail's slot folder (`game.sav`, `ss.bmp`, `CurMap/*.DAT`;
[forensics/SAVE_GAME.md](forensics/SAVE_GAME.md) §2). The game state
inside the file is retail's `game.sav` unchanged; the world is held as the
changes to each sector's objects against the module's own sectors.

- Retail slot folders still load and still list in the dialogs.
- `--oldsaves`, or `OldSaves=true` under `[Revisited]` in `Revenant.ini`,
  saves retail slot folders instead.
- A slot named by both a file and a folder is the newer of the two.
  Slots list alphabetically and the last is selected, as in retail.

Code: [src/savefile.h](../../src/savefile.h) (the format and the
sector-image arithmetic, free of the engine),
[src/sectorstore.h](../../src/sectorstore.h) (the working set),
`TSaveGame` in [src/savegame.cpp](../../src/savegame.cpp) (the slots).
Tests: `test_savefile`, `test_sectorstore`.

## The working set

Retail kept every sector the game had written in a `curmap` folder and
read a sector from there before the module's map. The port keeps no
`curmap`: `SectorStore` holds those sectors in memory as sector images
(the bytes of a `.DAT` file), and a sector the same as the base map's
isn't kept. The base map is the module's `Map/` through the resource
layer, after `<SavePath>map/` (where the editor publishes) and the
Revisited overlay. A new game is an empty working set; a load replaces
it with the slot's sectors.

## File layout

IFF-style chunks, all little-endian: a 4-byte id, a 4-byte size, then
that many bytes. The file is one `RVSV` chunk holding the others. A
reader skips chunks it doesn't know; a rewrite of the file (the late
thumbnail below) carries them over.

| Chunk | Contents |
|---|---|
| `HEAD` | u32 format version (1), u32 flags (0). A file of a newer version is refused. |
| `GAME` | retail's `game.sav`, as is (header and body, SAVE_GAME.md §3). |
| `THUM` | the thumbnail: a 216×160 RGB PNG file, retail's `ss.bmp` size. Optional. |
| `SECT` | the changed sectors, sorted by level, x, y. |

`SECT` is a u32 count, then each sector:

```
i32 level, x, y
u8  whole
whole:    u32 size, the sector image
otherwise:
  i32 version       sector version the records are written in
  u64 basehash      XXH64 (seed 0) of the base sector the changes were made against
  u32 count, then each change:
    u8  kind        1 add, 2 delete, 3 replace
    u64 key         delete / replace: the base object's key (below)
    i16 objclass    delete / replace: the base object's class
    u32 objtype     delete / replace: the base object's type
    u32 size, the record (add / replace)
```

A record is one slot of a sector file as `SaveObject` writes it: from the
object version through the end of the inventory
([SECTOR_FILE_FORMAT.md](../../recon/docs/SECTOR_FILE_FORMAT.md),
SAVE_GAME.md §11.1).

## Object changes

**Keys.** Sector files give objects no stable id: the slot index shifts,
and the map index repeats across sectors. A base object is found by a key
made from its sector and its record: `XXH64(record, seed)`, with seed
`XXH64("<level>_<x>_<y>.DAT", 0) + copy × 0x9E3779B97F4A7C15`, where
`copy` counts the identical records before it in the same sector. Over
the 107,904 objects of the shipped Ahkuilon map no two keys are equal
(31 byte-identical stacked tiles are told apart by `copy`).

**Making the changes** (`MakeSectorDelta`). Slots are paired by index.
The same bytes in both are no change. The same object in both (class,
type and map index) with other bytes is a replace. Otherwise the base's
object is deleted and the saved one added. A sector with no changes
holds the base's objects (perhaps saved at a later sector version) and
isn't written to the file.

**Applying them** (`ApplySectorDelta`). The base's objects stay in their
slots, a replaced one takes the saved record, a deleted one leaves an
empty slot (as the game leaves one), and the added ones follow. A delete
or replace finds its object by key, class and type, and each base object
is claimed once. The sector's state hash is recomputed (SAVE_GAME.md
§11.6). A change whose object the base no longer has is dropped with a
warning: that object is the module's.

So when a module's sector changes under a save (an editor publish, a
patched module), the save's changes to the objects still there still
apply. An object both changed is the module's.

**Versions.** Sector versions 14 and 15 frame records alike, but an
object's body can differ between them (`TMonsterGen` saves its spawn
clocks from 15). The shipped Ahkuilon map mixes version-14 and 15
sectors, and the game writes 15. Changes made against a base of another
version apply only to that same base (`basehash`): the base records they
keep were checked byte for byte against the version-15 sector when the
save was made. A base saved at the delta's version since is matched by
key as usual. Different framing (versions below 14, below 8) stores the
whole sector.

**Whole sectors** are stored when the module has no such sector (made by
the game) or its framing differs.

## Saving and loading

`TSaveGame::Save` writes the file (`SaveFileSlot`) or, with OldSaves, the
folder (`SaveFolder`, the working set's sectors whole). The file goes to
`<name>.rvsave.partial` and is renamed over the slot, so a failed write
leaves the previous save. The thumbnail is the one captured when the menu
opened; when there is none (a quick save), the next frame is captured
and put into the file once it's read back.

`TSaveGame::Load` takes either kind: a file restores the working set
from its deltas (`TMapManager::LoadCurMap(deltas)`), a folder from its
`CurMap/` (`LoadCurMap(dir)`).

**Retail interop.** Retail can't read a `.rvsave`. `ExportRetail(name,
dir)` writes a retail slot folder from either kind: `game.sav`, `ss.bmp`
in retail's BMP layout, and `CurMap/` with each saved sector rebuilt
whole (the playtest bundles use it). Sectors left out because they equal
the base map are read from the module by retail as by the port.

## The editor

Editing writes sectors into the working set as play does. `savemap`
publishes them (`TMapManager::PublishCurMap`): each is written over its
file in `<SavePath>map/`, the sectors published before are kept, and the
published ones leave the working set. Saves made before a publish still
load: their changes apply to the new base object by object, and a change
to an object the editor also changed is dropped with a warning.

## Notes

- Sector images round-trip through the port with 98.7% of records
  byte-identical; most of the rest is `OF_MOVING`, which load clears.
  Each such object costs a replace in the file, not a wrong result.
