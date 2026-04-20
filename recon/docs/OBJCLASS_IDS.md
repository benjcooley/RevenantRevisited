# TObjectClass ID Table (retail-authoritative)

Source of truth: retail `Revenant.exe` disassembly.

The retail binary exposes a numeric-ID → class-name lookup at
`0x00483850` (internally `FindClassByID(int id)` / switch dispatch).
The function tail-jumps through an indexed table at `0x004839a4`:

```text
0x00483850: mov  eax, [esp+4]          ; id
0x00483855: cmp  eax, 0x1a             ; 26
0x00483859: ja   0x0048397a            ; -> "NONE" branch
0x0048385f: jmp  [0x004839a4 + eax*4]  ; dispatch
```

Each table target loads the corresponding class-name string into `EDI`
and falls into a common `rep movsb` tail that copies the name into an
out-buffer.

## Definitive ID map (0..26)

| ID  | Name             | Status in 1998 source     | Notes                                         |
| --- | ---------------- | ------------------------- | --------------------------------------------- |
| 0   | `ITEM`           | `OBJCLASS_ITEM`           |                                               |
| 1   | `WEAPON`         | `OBJCLASS_WEAPON`         |                                               |
| 2   | `ARMOR`          | `OBJCLASS_ARMOR`          |                                               |
| 3   | `TALISMAN`       | `OBJCLASS_TALISMAN`       |                                               |
| 4   | `FOOD`           | `OBJCLASS_FOOD`           |                                               |
| 5   | `CONTAINER`      | `OBJCLASS_CONTAINER`      |                                               |
| 6   | `LIGHTSOURCE`    | `OBJCLASS_LIGHTSOURCE`    |                                               |
| 7   | `TOOL`           | `OBJCLASS_TOOL`           |                                               |
| 8   | `MONEY`          | `OBJCLASS_MONEY`          |                                               |
| 9   | `TILE`           | `OBJCLASS_TILE`           |                                               |
| 10  | `EXIT`           | `OBJCLASS_EXIT`           |                                               |
| 11  | `PLAYER`         | `OBJCLASS_PLAYER`         |                                               |
| 12  | `CHARACTER`      | `OBJCLASS_CHARACTER`      |                                               |
| 13  | `TRAP`           | `OBJCLASS_TRAP`           |                                               |
| 14  | `SHADOW`         | `OBJCLASS_SHADOW`         |                                               |
| 15  | `HELPER`         | `OBJCLASS_HELPER`         |                                               |
| 16  | `KEY`            | `OBJCLASS_KEY`            |                                               |
| 17  | `INVCONTAINER`   | **retail-added**          | Ported as `TInvContainer : TContainer` stub   |
| 18  | `POTION`         | **retail-added**          | Ported as `TPotion : TObjectInstance` stub    |
| 19  | `???`            | (placeholder)             | Unnamed `"???"` slot in retail switch         |
| 20  | `???`            | (placeholder)             | Unnamed `"???"` slot in retail switch         |
| 21  | `AMMO`           | `OBJCLASS_AMMO`           |                                               |
| 22  | `SCROLL`         | `OBJCLASS_SCROLL`         |                                               |
| 23  | `RANGEDWEAPON`   | `OBJCLASS_RANGEDWEAPON`   |                                               |
| 24  | `???`            | (placeholder)             | Unnamed `"???"` slot in retail switch         |
| 25  | `EFFECT`         | `OBJCLASS_EFFECT`         |                                               |
| 26  | `MAPSCROLL`      | **retail-added**          | Ported as `TMapScroll : TObjectInstance` stub |

## How these were confirmed

1. Every class-name string in `.rdata` (`"ITEM"`, `"WEAPON"`, ..., `"MAPSCROLL"`)
   has **exactly one** xref from `.text`, and all xrefs cluster between
   `0x00483866..0x0048394e` — clearly one function.
2. The block starting at `0x00483850` is a classic `cmp eax, 0x1a / ja / jmp [table + eax*4]`
   switch. The jump table at `0x004839a4` is 27 entries of 4 bytes.
3. Reading each jump-table entry and resolving its target back to the
   `mov edi, <strptr>` pairs produces the table above. Three IDs (19, 20, 24)
   reuse the same target (`0x00483906`), and that target loads the literal
   string `"???"` — they are the placeholder / reserved slots.

Extraction script (self-contained):

```python
import struct
with open('data/Revenant.exe','rb') as f: data = f.read()
tbl_va = 0x4839a4; tbl_f = tbl_va - 0x5c5000 + 0x1c5000
for id_ in range(27):
    target = struct.unpack('<I', data[tbl_f + id_*4:tbl_f + id_*4 + 4])[0]
    # Each target begins with "BF XX XX XX XX" = mov edi, <strptr>
    off = target - 0x400000
    assert data[off] == 0xBF
    strva = struct.unpack('<I', data[off+1:off+5])[0]
    sp = strva - 0x5c5000 + 0x1c5000
    name = data[sp:sp+32].split(b'\x00')[0].decode()
    print(f'ID={id_:2d}  {name}')
```

## Consequences for the port

- `MAXOBJECTCLASSES = 64` so all 27 IDs fit with headroom.
- Our enum must stay numerically aligned with retail so serialized
  `objclass` bytes in `N_X_Y.DAT` sector files mean the same thing on
  both sides. The enum in `src/object.h` is now annotated with
  explicit IDs and matches the table above.
- The three retail-only classes are registered as per-class TUs
  (one header + one `.cpp` each, matching the rest of the tree's
  `key.{h,cpp}` / `container.{h,cpp}` convention):
  - `src/invcontainer.{h,cpp}` → `TInvContainer : TContainer`,
    `InvContainerClass("INVCONTAINER", OBJCLASS_INVCONTAINER, 0)`
  - `src/potion.{h,cpp}`       → `TPotion : TObjectInstance`,
    `PotionClass("POTION", OBJCLASS_POTION, 0)`
  - `src/mapscroll.{h,cpp}`    → `TMapScroll : TObjectInstance`,
    `MapScrollClass("MAPSCROLL", OBJCLASS_MAPSCROLL, 0)`

  The registrations exist so `TObjectClass::classes[id]` is non-null
  during sector load. Without them, v14+ sector files drop 2 POTION
  instances as `bad_class` in `0_2_26` (and similar numbers on other
  maps).
- `class.def` / `legacy/Class.Def` predates these three classes and has
  no `CLASS "INVCONTAINER"`, `CLASS "POTION"` or `CLASS "MAPSCROLL"`
  blocks, so `TObjectClass::NewObject` may still return null for
  specific objtypes tied to those classes (counted as `new_obj_fail`,
  not `bad_class`). Populating `objinfo` for the retail classes is a
  Phase-3 task tied to the effect/potion/bag gameplay port.

## Test expectations (`--test=sector --sector=0_2_26`)

Before the enum rename and stub registrations:

```text
LoadObject buckets: ok=7113 ... bad_class=2 ... new_obj_fail=58
```

After:

```text
LoadObject buckets: ok=7115 ... bad_class=0 ... new_obj_fail=58
```

The remaining 58 `new_obj_fail` are EFFECT subtypes (objclass 25) whose
`objtype` indices aren't registered by `effect.cpp` (currently the
Phase-2 stub) or the legacy `Class.Def`. These are tracked separately.
