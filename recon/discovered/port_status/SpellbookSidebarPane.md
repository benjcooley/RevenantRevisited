# Spell-sidebar pane (cls_0x5a5ae8) port status

Retail vtable: **`cls_0x5a5ae8`** (46 slots). **Class identity unresolved** -
see [B.r9 brief](../../../docs/ui/briefs/B_r9_parchment_viewer.md). The class
is the **in-game "Spell" sidebar tab content pane** (CLASSIC_HUD_REFERENCE.md
section 3d), displaying the player's known-spells list as a scrollable
parchment-styled column with up/down arrow buttons.

**This file is named `SpellbookSidebarPane.md` to describe its ROLE, not to
claim a retail class name.** The src/ candidates (`TBookPane`, `TScrollPane`,
`TJournalPane`, `TSpellPane`) all fail comparison - the retail class is
likely a new class added post-pre-release-snapshot for which we have no
source name. See B.r9 brief section "Cross-reference vs src/ candidates"
for the full disambiguation.

## Global instance

- Vtable wire: `MOV ECX, 0x0065a9d8 ; CALL 0x00488620` @ 0x00480c65 (static
  init region). Single instance, no second global wire site.
- Object size: at least 0x1d4 bytes (largest field access in observed init =
  +0x1d0 + 0x4).

## Inheritance shape

```
TPane (cls_0x5a4494, 32 slots, Wave-2B confirmed)
  └─ intermediate (cls_0x5a45c8 - probable retail TButtonPane variant with
      DEF widget engine baked in; adds 14 slots: 0x436xxx button-mgmt + DEF
      dispatch helpers from B.r7)
       └─ cls_0x5a5ae8 (this class; LEAF - 46 total slots; adds 0 new
           virtuals, only overrides 10 slots: 0,1,10,11,19,20,21,25,26,31)
```

## Vtable - leaf overrides

| Slot | Method (inferred role) | Retail addr | Notes |
|---:|:---|:---|:---|
| 0 | `Initialize` | 0x005449e0 | **Confirmed**. Body in `cls_0x5a5ae8_ScrollPane_Initialize_5449e0.cpp`. Loads ScrlTop/ScrlBottom/ScrollPaper/Scroll sprites, creates scroll-up/down TButtons. |
| 1 | `Close` (probable) | 0x00544e40 | Pairs with slot 0; needs body extract. |
| 10 | ? | 0x00445210 | Overrides TPane's 0x444fb0. |
| 11 | (helper) | 0x00488810 | Used by ctor at +0x484; likely a vtable-wire/init helper. |
| 19 | scroll-update (probable) | 0x00546620 | Body present in cls_0x5a4494.cpp (mis-attributed by OOAnalyzer). Works with scroll-state fields mbr_0x17c, 0x198, 0x19c, 0x1a0. |
| 20 | ? | 0x00544eb0 | |
| 21 | ? | 0x005452d0 | |
| 25 | ? | 0x00545f10 | |
| 26 | click/select handler (probable) | 0x00546390 | Body in cls_0x5a4494.cpp. Reads mbr_0x1c8 (selection index), looks up spell name in player's spell list, draws into mbr_0x180 surface. |
| 31 | destructor scalar-dtor (probable) | 0x00488820 | Pattern matches MSVC scalar-deleting dtor. |

## Field layout (from observed accesses in Initialize + click/scroll handlers)

Inherits TPane fields at +0x00..+0x60 (see TPane.md). Plus intermediate-class
fields (likely button-list + DEF buffer state @ +0x68..+0x178 based on B.r7).
This class's own fields:

```
this+0x17c : scroll Y position (current)         <- mbr_0x17c
this+0x180 : main content mosaic surface ptr     <- TMosaic-like
this+0x184 : ScrlTop sprite buffer ptr           <- TSprite-like
this+0x188 : ScrlBottom sprite buffer ptr        <- TSprite-like
this+0x18c : ScrollPaper sprite buffer ptr       <- TSprite-like
this+0x190 : Scroll sprite ptr (= field [400])   <- TSprite-like
this+0x194 : (used by slot-26 handler)
this+0x198 : scroll flag (set to 1 in init)
this+0x19c : scroll flag (set to 1 in init)
this+0x1a0 : scroll-range-end (set to 0 in init)
this+0x1a4 : list-line count (numlines) (word)   <- written by spell-list iter
this+0x1a6 : list-buffer capacity (word, 0x40)
this+0x1a8 : list-line buffer ptr (dword*, 0x40 entries; malloc'd in ctor)
this+0x1ac : (set in slot-26 handler)
this+0x1b0 : (set in slot-26 handler)
this+0x1c8 : selection index (-1 = none, written by click handler)
this+0x1cc : (-1 init)
this+0x1d0 : scroll delta accumulator (works with 0x28 px increments)
```

## Constructor (FUN_00488620 @ 0x00488620)

3-stage MSVC inheritance ctor:
1. Write base TPane vtable `&PTR_FUN_005a4494`
2. Set field [1]=[5]=0x1c4 (width=newwidth=452), [3]=[7]=0xbc (height=newheight=188),
   [2]=[6]=0 (y), [4]=0x132 (newheight2=306 - different from height; unusual)
3. Call `FUN_0041c7f0(0x10, 0x10)` (TPane base ctor; passes x=y=16)
4. Write intermediate vtable `&PTR_FUN_005a45c8`
5. Allocate 0x100 bytes for list-buffer at field [0x6a] (count field [0x69]=0x40)
6. Write final vtable `&PTR_FUN_005a5ae8`

## Open questions

See B.r9 brief.

## Related files

- `recon/discovered/cls_0x5a5ae8_ScrollPane_Initialize_5449e0.cpp` - Initialize body (Wave-2B)
- `recon/discovered/cls_0x5a5ae8_ctor_488620.cpp` - constructor body (Wave-2B)
- `recon/discovered/cls_0x5a5ae8_vtable.txt` - full 46-slot vtable dump (this wave)
- `recon/classes/cls_0x544fb0.cpp` - contains the spell-list iteration methods (OOAnalyzer split them into a separate "class" but they operate on cls_0x5a5ae8's instance fields via the global @ 0x65a9d8)
- `recon/classes/cls_0x5a4494.cpp` lines 540-..  - contains virt_meth_0x546390 (slot 26) + virt_meth_0x546620 (slot 19) bodies (mis-attributed to TPane vtable by OOAnalyzer)
- Brief: `docs/ui/briefs/B_r9_parchment_viewer.md`
