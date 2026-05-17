# B.r9 - "Parchment viewer" class (cls_0x5a5ae8) identification

**Wave:** 3B
**Date:** 2026-05-16
**Status:** PURPOSE IDENTIFIED; CLASS IDENTITY UNRESOLVED (NO src/ MATCH).

## TL;DR

`cls_0x5a5ae8` is the **in-game "Spell" sidebar tab pane** that displays the
player's known-spells list as a scrollable parchment-styled column. It is
**NOT** any of the candidate classes initially hypothesised
(TBookPane / TScrollPane / TJournalPane / TSpellPane). All four src/ candidates
fail in different ways - see the cross-reference table below.

Per the AGENT_PROTOCOL "cost asymmetry" rule, the class is **not renamed** -
we have a clean purpose identification but cannot tie it to a src class name
with 2+ lines of evidence. What IS renamed: the class's Initialize virtual
(`FUN_005449e0`, vtable slot 0) and the constructor (`FUN_00488620`, only call
site at 0x480c65 sets ECX = 0x65a9d8 - the documented global instance).

The retail class appears to be a NEW class added between the pre-release src/
snapshot and the final retail build - one for which we have no source name.
This is consistent with the project state (per project_overview memory note):
the spell-list sidebar mode is a HUD feature that may have post-dated the
pre-release src/ checkout.

## Step 1 - cls_0x5a5ae8 vtable dump (46 slots)

Full output at `recon/discovered/cls_0x5a5ae8_vtable.txt`. Key observations:

| Slot | Addr | Source | Notes |
|---:|---|---|---|
| 0 | 0x5449e0 | leaf | **Initialize** (Wave-2B's "FUN_005449e0 stat-pane init" -> actually this class's Initialize virtual) |
| 1 | 0x544e40 | leaf | Close (probable; pairs with slot 0) |
| 2 | 0x435010 | intermediate (5a45c8 shared) | shared with TBottomBarPane et al |
| 3-6 | 0x444f40..0x444f70 | TPane base (5a4494 shared) | inherited unchanged |
| 7 | 0x436010 | intermediate (5a45c8 shared) | overrides TPane's 0x444f80; NOT a TPane-only base method |
| 8 | 0x444f90 | TPane base | inherited |
| 9 | 0x491a80 | TPane base | inherited |
| 10 | 0x445210 | leaf/intermediate? | overrides TPane's 0x444fb0 |
| 11 | 0x488810 | leaf | the vtable-wire helper called from the ctor at +0x484 |
| 12-15 | 0x444fd0..0x445000 | TPane base | inherited |
| 16 | 0x491bb0 | TPane base | inherited |
| 17,18 | 0x445020, 0x445030 | TPane base | inherited |
| 19 | **0x546620** | leaf | scroll-state handler (reads mbr_0x198/19c/1a0, advances mbr_0x17c) |
| 20 | 0x544eb0 | leaf | leaf override |
| 21 | 0x5452d0 | leaf | leaf override |
| 22 | 0x435f90 | intermediate | shared with TBottomBarPane |
| 23 | 0x436090 | intermediate | shared with TBottomBarPane |
| 24 | 0x445080 | TPane base | inherited |
| 25 | 0x545f10 | leaf | leaf override |
| 26 | **0x546390** | leaf | click/select handler (reads mbr_0x1c8 selection index, looks up spell name in player's spell list, calls into mbr_0x180 surface) |
| 27 | 0x4361f0 | intermediate (= **DefWidget_DispatchInput**!) | shared - same as B.r7 input dispatcher |
| 28 | 0x436460 | intermediate | shared with TBottomBarPane |
| 29 | 0x436340 | intermediate | shared with TBottomBarPane |
| 30 | 0x4450c0 | TPane base | inherited |
| 31 | 0x488820 | leaf | leaf override (likely destructor scalar-dtor wrapper) |
| 32 | 0x435230 | intermediate | shared with TBottomBarPane |
| 33 | 0x435150 | intermediate (= **DefScreen_Open**) | shared - per B.r7 |
| 34-43 | 0x4364d0..0x436ec0 | intermediate | shared 0x436xxx button-mgmt + DEF dispatch helpers |
| 44 | 0x437000 (= **DefWidget_ParsePanel**) | intermediate | per B.r7 |
| 45 | 0x437080 | intermediate | DEF style-section parser region |

**Vtable size: 46 slots.** Matches sibling TBottomBarPane (also 46 slots).
Above slot 45 we cross into the next class's vtable (slot 46 @ 0x5a5ba0 =
0x546b50, which is the OTHER "stat pane" init = the genuine TStatPane's
Initialize; cls_0x5a5ba0 starts at that offset).

**Inheritance shape inferred:**
- 32 slots from TPane base (cls_0x5a4494, confirmed Wave-2B)
- +14 slots from intermediate cls_0x5a45c8 (the "TButtonPane-with-DEF-widget-engine"
  parent shared with TBottomBarPane / TStatPane / TSideTabsPane / etc - see
  Open Questions below).
- 0 NEW slots in the leaf - the leaf only OVERRIDES (slots 0, 1, 10, 11, 19,
  20, 21, 25, 26, 31). This is consistent with a TButtonPane subclass that
  doesn't add new virtuals, only customises the pane-init / draw / scroll /
  click behaviours.

## Step 2 - String anchors hunt: NO direct class anchor

Ran FindStringRefs for: `TBookPane`, `TScrollPane`, `Journal`, `Book`, `Page`,
`Read scroll`, `Talisman`, `talisman`, `SpellPane`, `multipane`, `Multipane`,
`MULTIPANE`, `Spellbook`.

Hits:
- `TBookPane` / `TScrollPane` / `Journal` / `Page` / `Talisman` / `SpellPane`
  / `multipane` / `Spellbook`: **NO HITS** (strings simply do not exist in
  the retail binary).
- `Book` @ 0x5e4720: referenced from FUN_0053cc30 (TSideTabsPane::Initialize)
  - this is the "Book" sidebar TAB LABEL, one of 6 tabs (Book / Stats /
  Equip / Spell / ? / ?).

The asset filenames in the Initialize body (already known from Wave-2B) -
`ScrlTop`, `ScrlBottom`, `ScrollPaper`, `Scroll`, `ScrlArwUU/UD/UG` (scroll
up arrow), `ScrlArwDU/DD/DG` (scroll down arrow) - are all generic asset names
for the visual style "parchment scroll with arrow buttons", not class-name
anchors. Per the protocol, asset-name evidence identifies PURPOSE not CLASS.

## Step 3 - Distinctive behaviour: iterates the player's spell list

The `cls_0x544fb0::meth_0x544fb0` and `meth_0x546830` methods (OOAnalyzer
extracted these as belonging to a separate "class" 0x544fb0 but they operate
on cls_0x5a5ae8 instance fields - this is OOAnalyzer's class-tracing missing
the connection) both iterate the player's spell list and format each entry as
"SPELL %s":

```cpp
// from cls_0x544fb0.cpp (cls_0x5a5ae8 methods - OOAnalyzer split them)
if (0 < *(int *)(DAT_00667fcc + 0x2ec)) {   // count of player's known spells
  do {
    pbVar11 = *(byte **)(*(int *)(DAT_00667fcc + 0x2fc) + iVar3 * 4); // spell name
    ...
    pcStack_84 = s_SPELL_s_005e5394;        // "SPELL %s" format string
    FUN_0058b100();                          // sprintf into local buffer
    ...
    cls_0x5a68d8::meth_0x4be2b0((cls_0x5a68d8 *)this->mbr_0x180, ...) // draw into content surface
    ...
  } while (local_5c < *(int *)(DAT_00667fcc + 0x2ec));
}
```

And the click handler `virt_meth_0x546390` (vtable slot 26):
- reads `this->mbr_0x1c8` (the selection index, set to 0xffffffff = unselected
  in the init at field [0x1c8])
- looks up `*(char **)(*(int *)(DAT_00667fcc + 0x2fc) + this->mbr_0x1c8 * 4)`
  (the indexed spell name from the player's spell list)
- writes it into a 20x20 sprite display at +0x180 (the content surface)
- sets `UNK_0065a9dc._76_4_ = 1` (a dirty/active flag in the global instance's
  state padding @ 0x65a9dc, just past the 4-byte vtable wire @ 0x65a9d8)

`DAT_00667fcc` is the current player object pointer (used by many character /
HUD systems). `+0x2ec` is the spell-list count and `+0x2fc` is the spell-list
array - confirmed by cross-reference with cls_0x45f7c0_TObjectClass.cpp at
4253 (`cls_0x544fb0::meth_0x546830((cls_0x544fb0 *)&DAT_0065a9d8)` is called
from object-class internals when player gains a new spell, with the class
instance's address explicitly passed = the global @ 0x65a9d8).

## Step 4 - Cross-reference vs src/ candidates - ALL FAIL

| src class | Match criteria | Retail evidence | Verdict |
|---|---|---|---|
| `src/scroll.h::TScrollPane` | parchment text reader for a `TScroll` game object; fields `scroll`, `scrolldata`, `scrollfont`, `line`, `numlines` | retail iterates `DAT_00667fcc+0x2ec/+0x2fc` (player spell list), not a TScroll object; sprite naming matches parchment STYLE but content is not free text | **REJECT** - wrong content source |
| `src/scroll.h::TBookPane` | TScrollPane subclass for book reading; `final` | same as above; also retail class's purpose is sidebar-mode spell list, not "open book overlay" | **REJECT** - same reason |
| (hypothetical) TJournalPane | quest journal; no src/ implementation | no string anchors; quest journal not in CLASSIC_HUD layout | **REJECT** - speculative target |
| `src/spellpane.h::TSpellPane` | talisman assembly composer; loads sprite `"spell"`; buttons `"spell"`/`"book"`/`"add"`/`"back"`/`"down"`/`"up"`/`"min"`; iterates `TalismanClass.NumTypes()`; fields `shownames`, `startline`, `clickedtal`, `onclickedtal` | retail loads `"ScrollPaper"`/`"ScrlTop"`/etc (different sprites); buttons named `"ScrlArwUU/UD"`/etc (different naming); iterates player spell list (not talisman class); has selection-index field `mbr_0x1c8` (closer to `clickedtal`?) | **REJECT** - the pre-release TSpellPane is the TALISMAN COMPOSER, a different UI mode entirely. Visual styling, button names, and content source all diverge. |

## Verdict

**cls_0x5a5ae8 is a retail-era class with no clean src/ predecessor.** Its
purpose is the **in-game "Spell" sidebar tab** (per CLASSIC_HUD_REFERENCE
section 3d: "Spell list / spell book - column of spell circles"). The class
displays the player character's known-spells list inside a scrollable
parchment-styled column with scroll-up/scroll-down arrow buttons.

Likely retail name (speculative): TSpellListPane / TKnownSpellsPane /
TSpellBookPane / repurposed TBookPane. Cannot commit to any of these without
finding a string anchor that identifies the class itself (which the FindStringRefs
sweep did not yield).

The CLASSIC_HUD reference correctly hypothesised (table at line 274):
"Books / scrolls = TBookPane / TScrollPane (src/scroll.h, implementations
present). Whether that's the actual TBookPane/TScrollPane rendered into the
sidebar slot, or a different code path that pulls scroll text from a TScroll
game object, is open."

This brief's contribution: the SPELL sidebar tab is the cls_0x5a5ae8 class,
and it is NOT TScrollPane/TBookPane (it iterates the player's spell list,
not a TScroll object's text). The Book sidebar tab (one of the other 6 tabs
in TSideTabsPane) MAY be a separate class - that's a different hunt.

## Sibling classes

- **cls_0x5a5ba0** = adjacent vtable starting at slot 46 of the 0x5a5ae8 dump
  (= 0x546b50, which is the OTHER "stat pane" init from Wave-2B). This is
  the genuine **TStatPane** (sidebar Stats tab). Its vtable wire is at
  0x65b140 (per Wave-2B).
- **cls_0x5a5808** = TBottomBarPane (46-slot TButtonPane-w-DEF subclass) -
  shares all 14 intermediate-class slots with cls_0x5a5ae8.
- **cls_0x5a5a30** = TQuickSpellPane (35-slot TButtonPane subclass) - DIFFERENT
  inheritance shape (doesn't pull in the DEF-widget-engine intermediate; goes
  TPane -> TButtonPane direct). So the 4-button quickspell row uses a
  pre-DEF-engine flavour of TButtonPane.

There does NOT appear to be a SEPARATE TBookPane vtable - if there's a book
reader UI element, either:
- it shares cls_0x5a5ae8 (the same class is reused with different content),
- it's bound to the "Book" sidebar tab and lives in a vtable nearby that
  wasn't dumped (worth a follow-up DumpVtable around 0x5a5d.. / 0x5a5e.. /
  0x5a5f..),
- OR the "Book" tab in TSideTabsPane is just labelled and not wired to
  anything in the shipped binary (the pre-release-only TBookPane was cut
  before release).

This is an open question for a future wave.

## Open questions

1. **Did pre-release TBookPane / TScrollPane (src/scroll.h) survive into retail
   at all?** No string anchors in the binary, no vtable found in the immediate
   neighbourhood of cls_0x5a5ae8. May have been cut. The "Book" sidebar tab
   in TSideTabsPane (one of 6) hints at the feature - need to follow the tab
   activation chain to see what it actually opens.

2. **What is the intermediate class cls_0x5a45c8?** It's a TButtonPane variant
   that incorporates the DEF widget engine (B.r7's DefWidget_ParsePanel /
   DefWidget_DispatchInput / DefScreen_Open / etc are wired into its vtable
   slots 27, 33, 44). Shared by cls_0x5a5ae8 (this), cls_0x5a5808
   (TBottomBarPane), cls_0x5a5ba0 (TStatPane), cls_0x5a5750 (TSideTabsPane),
   and 9+ others (see Wave-2B's grep of 0x5a45c8 wire sites). Worth a dedicated
   ID wave (separate from this one).

3. **The intermediate-base init helper `FUN_00434e40`** is wired at slot 31
   of cls_0x5a4494's vtable (TPane base) but it's also called as a
   non-virtual base-init helper from FUN_005449e0 (this class's Initialize),
   FUN_00546b50 (TStatPane's Initialize), FUN_0053cc30 (TSideTabsPane's
   Initialize), etc. Wave-2B already flagged this; possibly mis-attributed
   to TPane's vtable in OOAnalyzer's analysis when it's actually the
   intermediate cls_0x5a45c8's Initialize. Resolving this needs a careful
   look at FUN_00434e40's body relative to TPane vs intermediate field
   layout.

4. **Method-level identifications for the 10 leaf-overridden slots** of
   cls_0x5a5ae8 (slots 0, 1, 10, 11, 19, 20, 21, 25, 26, 31). Slot 0 is
   confirmed Initialize. Slot 1 is probably Close. Slot 19 is probably an
   Animate/Update for scroll movement (works with mbr_0x17c). Slot 26 is
   probably MouseClick (reads selection index + looks up clicked spell).
   Slot 31 is probably the scalar-deleting destructor. Others need
   per-method body extraction.

5. **Coordination with Wave-3A**: Wave-3A is identifying TButtonPane base.
   If Wave-3A confirms that cls_0x5a45c8 is the retail TButtonPane (a
   plausible identification given the 0x436xxx button-management methods in
   its added slots), that strengthens this brief's "no src/ match for the
   leaf class" conclusion - because pre-release TButtonPane is much simpler
   than the retail intermediate.

## Renames applied this wave

See `recon/discovered/renames/agent_ui_wave3_parchment.txt`:

- `FUN_005449e0` -> `FUN_005449e0_SpellbookSidebarPane_Initialize` (vtable
  slot 0 of cls_0x5a5ae8 confirmed; descriptive role name, not a claimed
  retail class name).
- `FUN_00488620` -> `FUN_00488620_SpellbookSidebarPane_ctor` (sole call site
  at 0x480c65 sets ECX = 0x65a9d8 = documented global instance).

NOT renamed: `cls_0x5a5ae8` itself, slot-1/19/20/26 methods (defer to next
wave once class identity resolved OR each method has its own string anchor).

## Files produced this wave

- `recon/discovered/cls_0x5a5ae8_vtable.txt` - full 46-slot DumpVtable output
- `recon/discovered/renames/agent_ui_wave3_parchment.txt` - 2 conservative
  rename pairs + extensive comment block on what's deferred and why
- `docs/ui/briefs/B_r9_parchment_viewer.md` - this file

## Hunt log

- **2026-05-16, Wave-3B run 1**: read AGENT_PROTOCOL + discovered/README +
  B.r6 brief + Wave-2B's two extracted bodies (Initialize + ctor) +
  src/scroll.h + src/spellpane.h. Ran DumpVtable @ 0x5a5ae8 (46 slots).
  Lock-contention encountered twice; resolved after ~60s backoff.
- **run 2**: ran FindStringRefs for 7 class-name candidates + 7 spell-related
  keywords. ZERO hits for class-name candidates. Identified the "SPELL %s"
  format string anchor at 0x5e5394 referenced by methods OOAnalyzer split
  into cls_0x544fb0 (but which operate on cls_0x5a5ae8's instance fields).
- **run 3**: CallersOf 0x488620 -> single hit at 0x480c65; Disasm confirmed
  `MOV ECX, 0x65a9d8 / CALL 0x488620` (global instance wire).
- Wrote 2-rename file + this brief. Did NOT apply renames (per protocol).

## Coordination notes (for Wave-4+)

- The class identity is genuinely unresolved. Next agent should NOT attempt
  to rename `cls_0x5a5ae8` to TSpellPane / TBookPane / TScrollPane without
  evidence beyond what this brief documents. The renaming requires either
  (a) discovering a string anchor in one of its method bodies that gives a
  class-name token, or (b) a port effort that ports the retail code into the
  RevenantRevisited codebase and lets the author confirm the name.
- The "Book" sidebar tab (one of 6 in TSideTabsPane) is a candidate for a
  SEPARATE class hunt. Tab y=0x2 in FUN_0053cc30 with label DAT_005e4720 =
  "Book". Trace the tab-activation handler to see what it opens.
- The intermediate cls_0x5a45c8 is the most valuable single ID for the next
  wave - identifying it as TButtonPane (or whatever the retail-era equivalent
  is called) will unlock vtable analysis for ~13 sibling pane classes.
