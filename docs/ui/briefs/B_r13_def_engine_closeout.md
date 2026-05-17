# B.r13 — DEF widget engine close-out: per-widget ctors + Wave-2C relabels

**Status:** All 8 widget classes pinned (vtable + factory + parse-time ctor + type-id). Wave-2C "ctor" labels formally corrected to "attr_parser". TWidget base instance vtable map (Wave-3C's slot dump) inherited; new slot identification deferred for lack of golden-path evidence in the 0x42aa00-0x437fff window.

**Recon agent:** Wave-4C, 2026-05-17 (sibling to Wave-4A sidebar cascade + Wave-4B OOG closeout).

**Builds on:** B.r7 (Wave-2C engine identification), B.r10 (Wave-3C subclass framing). This brief CLOSES the DEF widget engine subclass-pinning loop — all eight widget classes are now confirmed to two-independent-lines confidence.

---

## 1. Architecture refinement (corrects both Wave-2C and Wave-3C)

`DAT_00655510` is a `void**` registry of 8 records (`DAT_0065617c == 8`). Each record is 8 bytes:

```c
struct DefWidgetClassRecord {
    void* class_descriptor_vt;  // +0x00 : 12-byte ptr-to-3-fnptr struct in .rdata
    const char* name;           // +0x04 : ASCII keyword
};
```

The class-descriptor vtables (3 slots × 4 bytes = 12 bytes) are packed in .rdata starting at 0x5a3b18, on uniform 0x6c (108-byte) strides. The three slots are: `[0]` = parse-time factory, `[1]` = ?, `[2]` = ?.

`DefWidget_DispatchControl` (0x436ec0) calls `descriptor_vt[0](pane, token)` — the factory. Disassembled call site (0x436f73 `CALL [EAX]` with EAX=descriptor_vt) corroborates this.

Each factory is a 106-byte trampoline (uniform shape across all 8):

```c
iVar1 = FUN_00482fb0(WIDGET_SIZE, this);   // operator new
if (iVar1 != 0)
    return REAL_CTOR(pane, token, 0);      // hand off to per-widget ctor
return 0;
```

The REAL_CTOR (one per widget) is the per-widget parse-time class constructor and the target of this brief.

There are TWO disjoint vtable families:

- **Registry descriptor vtables** (3 slots): 0x5a3b18, 0x5a3b84, 0x5a3bf0, 0x5a3c5c, 0x5a3ccc, 0x5a3d38, 0x5a3da4, 0x5a3e10.
- **Per-class INSTANCE vtables** (24+ slots, TWidget polymorphism): 0x5a3ab8 (TWidget base), 0x5a3b24, 0x5a3b90, 0x5a3bfc, 0x5a3c68, 0x5a3cd8, 0x5a3d44, 0x5a3db0, 0x5a3e1c.

**Wave-3C's vtable-dump enumeration of slots at 0x5a3ab8 (instance side) is correct;** what Wave-3C identified as "the SCROLLBAR vtable at 0x5a3cd8" is in fact the SCROLLBAR *instance* vtable (each scrollbar object's `*this` points here), not the registry descriptor vtable. The registry descriptor is at 0x5a3ccc (12 bytes earlier).

---

## 2. Per-widget identification verdict (4C.1)

All 8 confirmed via a 3-line evidence chain:

1. **Record FindImmRefs** — `FindImmRefs.java <record_addr>` returns the two writing MOVs (thunk-side store of record into registry array, and store of descriptor-vt-addr into record[0]).
2. **Factory dump+decomp** — DumpBytes 12 bytes at descriptor-vt address gives slot[0] = factory. Decomp of each factory body is uniform-shape allocate-and-construct.
3. **Ctor body pattern match** — `FUN_0042a350(pane, TYPE_ID, token); *this = &PTR_FUN_005a3xxx; <subclass field init>`.

Where the literal TYPE_ID is obscured by Ghidra's decomp (LISTBOX, DROPLIST), the registry-position number is used; the pattern shape is otherwise identical so confidence remains HIGH.

| # | Keyword | Record | RegistryVT | Factory | Size | True Ctor | InstVT | TypeID |
|---|---|---|---|---|---|---|---|---|
| 1 | BITMAP | 0x655dc8 | 0x5a3b18 | 0x437e00 | 0x98 (152) | **0x42abe0** | 0x5a3b24 | 1 |
| 2 | FRAME | 0x655508 | 0x5a3b84 | 0x437ee0 | 0xc0 (192) | **0x42b090** | 0x5a3b90 | 2 |
| 3 | TEXT | 0x656218 | 0x5a3bf0 | 0x437fd0 | 0xe0 (224) | **0x42b5d0** | 0x5a3bfc | 3 |
| 4 | BUTTON | 0x6562d0 | 0x5a3c5c | 0x4380c0 | 0x148 (328) | **0x42c6a0** | 0x5a3c68 | 4 |
| 5 | SCROLLBAR | 0x656388 | 0x5a3ccc | 0x438220 | 0x168 (360) | **0x42df00** | 0x5a3cd8 | 5 |
| 6 | LISTBOX | 0x655f48 | 0x5a3d38 | 0x438380 | 0x1d0 (464) | **0x430090** | 0x5a3d44 | 6 |
| 7 | EDIT | 0x656210 | 0x5a3da4 | 0x4385c0 | 0x148 (328) | **0x431ca0** | 0x5a3db0 | 7 |
| 8 | DROPLIST | 0x655dc0 | 0x5a3e10 | 0x438930 | 0x240 (576) | **0x433640** | 0x5a3e1c | 8 |

### FRAME (record 0x655508) — special confirmation

Wave-3C noted this record's address (0x655508 = 0x655510 - 8) was *below* the registry array base and tentatively labelled it "BGBITMAP-helper / FRAME attr variant". I confirmed it as the FRAME widget by disassembling the register thunk at 0x42aead:

```
0x42aead  MOV [0x65550c], 0x5cdc30    # store name string into record[+4]
```

Then `DumpBytes 0x5cdc30 16` returned `46 52 41 4d 45 00 00 00 54 45 58 54 ...` = `"FRAME\0\0\0TEXT\0\0\0\0"`. The name string at that address IS "FRAME". The record's BSS-below-array placement is a coincidence of linker layout — it's a normal 8th widget record.

The 8 widgets are exactly the 8 STYLE-eligible keywords in widgets.def vocabulary: BITMAP, FRAME, TEXT, BUTTON, SCROLLBAR, LISTBOX, EDIT, DROPLIST.

### SCROLLBAR — Wave-3C correction

Wave-3C identified FUN_0042de00 as the SCROLLBAR parse-time ctor. The actual parse-time ctor is **FUN_0042df00** (one 0x100 block higher). I verified the SCROLLBAR factory at 0x438220 calls FUN_0042df00 (not FUN_0042de00), and FUN_0042df00's body has the parse-time pattern (calls FUN_0042a350 with type-id 5).

FUN_0042de00 is the **programmatic** SCROLLBAR ctor — confirmed by its sole caller being the LISTBOX ctor (FUN_00430090) which uses it to instantiate the nested scrollbar child with explicit geometry. It calls FUN_0042a210 (programmatic base init) instead of FUN_0042a350 (parse-time base init). Same SCROLLBAR class (same `*this = 0x5a3cd8`), different invocation style. The relabel file demotes Wave-3C's name to `*_programmatic` to preserve history.

---

## 3. Wave-2C "ctor" → "attr_parser" relabel verdict (4C.2)

All 6 Wave-2C names verified as ATTRIBUTE PARSERS (not constructors). Method per function: decompiled body, checked for `*this = &PTR_FUN_005a3xxx` writes (count = 0 in all 6) and `FUN_0042a350` / `FUN_0042a210` base-init calls (absent in all 6). All 6 instead match the attr-parser pattern: signature `(out_state*, in_state*, pane*, token)`, no `this` write, body is a sequence of `FUN_00479700(s_KEYWORD)` token-lookup followed by `FUN_0047a410(token, format, dest)` value-parse.

| Wave-2C name | Address | Size | Real role | Strings observed |
|---|---|---|---|---|
| TEXT widget ctor | FUN_0042b340 | 644 | TEXT attr parser | BGBITMAP/DRAWMODE/RECT/COLOR |
| BUTTON widget ctor | FUN_0042bd90 | 1332 | BUTTON attr parser | Bitmap-name lookups |
| FRAME widget ctor | FUN_0042d730 | 1680 | FRAME attr parser | BGBITMAP/FRAME/DRAWMODE/RELR/RELB |
| LISTBOX widget ctor | FUN_0042f9f0 | 1053 | LISTBOX attr parser | BGBITMAP/FRAME/DRAWMODE/RECT |
| EDIT widget ctor | FUN_00431750 | 1253 | EDIT attr parser | RECT + UP/DOWN spin RELR |
| DROPLIST widget ctor | FUN_00433170 | 1128 | DROPLIST attr parser | BGBITMAP/FRAME/DRAWMODE/RECT |

(Wave-2C's `FUN_0042aaf0_BITMAP` and `FUN_0042aed0_(FRAME-bitmap-variant)` were already labelled `*_attr_parser` in their discovered/ files, so they need no relabel — only the wave-2 rename file's `_BITMAP` label persists, but the existing discovered/ convention overrides.)

These attr parsers are called from inside the widget ctor's ParseBody flow (via instance vtable+0x04 → 0x42a790 base impl, which dispatches per-class attrs).

---

## 4. TWidget base vtable slot findings (4C.3)

Wave-3C dumped the TWidget base vtable @ 0x5a3ab8 to 24 slots (its full extent). The named slots they pinned remain authoritative:

- Slot 0 → 0x437dc0 — scalar destructor (`delete this` style; vtable[0](flags & 1) frees on demand).
- Slot 1 → 0x42a790 — ParseBody (per-class attr-parse hook, dispatcher invokes after construct).
- Slot 7 → 0x42a770 — SetFlags (dispatcher invokes at end of construct as `vtable[7](flags | 0x20)`).

No additional slots cleanly identifiable from my evidence window (0x42aa00-0x437fff). The remaining slots (17, 21, 22) live at addresses 0x42a940 / 0x42a7a0 / 0x42a7f0 — all under 0x42aa00, outside my address-space scope. Deferred without rename.

A separate slot-map renames file was NOT generated (insufficient evidence under conservative rule). Future agent with the 0x42a000-0x42aa00 window can pick up TWidget base method identification.

---

## 5. FUN_0042a350 (TWidget base parse-time init) — confirmed

Wave-2C named this `_TWidgetBase_ParseAttrs` with HIGH confidence (4 string anchors in body). I now have 7 additional confirming call sites: every parse-time widget ctor (1-7; DROPLIST also calls it but Ghidra obscured the args) calls `FUN_0042a350(pane, TYPE_ID, token)` as its first non-prologue operation. This is the canonical TWidget base init for the parse-time path; Wave-2C's name remains correct.

**FUN_0042a210** (the *programmatic* TWidget base init, used by helper code like FUN_0042de00) is NOT renamed this wave — only one confirmed caller (FUN_0042de00). Needs one more caller for the 2-line threshold. Candidate place to look: code that programmatically constructs widgets without a token (e.g. for engine-built popup buttons).

---

## 6. Outputs

- **Brief**: this file (`docs/ui/briefs/B_r13_def_engine_closeout.md`).
- **Renames** (in `recon/discovered/renames/`):
  - `agent_ui_wave4_def_widget_ctors.txt` — 8 ctor renames + 8 factory renames + 1 Wave-3C SCROLLBAR-programmatic relabel.
  - `agent_ui_wave4_def_relabels.txt` — 6 Wave-2C attr-parser relabels.
  - (No `agent_ui_wave4_twidget_slots.txt` — see §4 above.)
- **Decompilations** (in `recon/discovered/`):
  - 8 × `FUN_<addr>_DefWidget_<Type>_ctor.cpp` (one per widget class).
  - 8 × `FUN_<addr>_DefWidget_<Type>_factory.cpp` (one per widget class).
- **Port-status**: `recon/discovered/port_status/DefWidgetEngine.md` updated with full widget roster table (§7 below).

---

## 7. Hunt log

**2026-05-17 (Wave-4C, ~50 min):**

- Read AGENT_PROTOCOL, B.r10, B.r7, DefWidgetEngine.md, Wave-2C+Wave-3C rename files.
- Extracted 7 Wave-3C candidate "ctors" — confirmed FUN_0042b090 (FRAME) and FUN_00433640 (DROPLIST) match pattern; rest were draw/hit-test/method bodies not relevant.
- Extracted FUN_0042a210 — confirmed it sets `*this = 0x5a3ab8` (TWidget base) and is the programmatic base-init.
- Extracted FUN_0042a350 (already labelled by Wave-2C) — confirmed it's the parse-time variant that also sets `*this = 0x5a3ab8` AND parses POS/SIZE/etc.
- Realized Wave-3C confused per-instance vtables (FUN_0042de00's `*this = 0x5a3cd8` write) with registry descriptor vtables (record[0]'s actual content).
- FindImmRefs on all 8 record addresses (with lock-retry-backoff loop) → got the 8 registry descriptor vtable addresses: 0x5a3b18, 0x5a3b84, 0x5a3bf0, 0x5a3c5c, 0x5a3ccc, 0x5a3d38, 0x5a3da4, 0x5a3e10.
- DumpBytes 12 bytes at each descriptor → slot 0 = factory for each widget.
- Decompiled all 8 factories → uniform 106-byte allocate-and-construct trampoline; harvested the 8 REAL_CTOR addresses each calls.
- Decompiled all 6 unknown ctors (FUN_0042abe0, FUN_0042b5d0, FUN_0042c6a0, FUN_0042df00, FUN_00430090, FUN_00431ca0) → all match the canonical `FUN_0042a350; *this = 0x5a3xxx; field init` ctor pattern with widget-specific type-id.
- Verified FRAME (record 0x655508) name string via disassembly of register thunk + DumpBytes of string at 0x5cdc30.
- Decompiled all 6 Wave-2C "ctor" functions → confirmed all are attr parsers (zero vtable writes; uniform attr-parser signature + token-lookup pattern).
- Wrote brief + 2 rename files + per-ctor + per-factory discovered/ files; updated port-status.

**Pending:**
- TWidget base instance vtable slots in the 0x42a000-0x42aa00 range (outside Wave-4C scope).
- `FUN_0042a210` formal rename to `TDefWidget_BaseInit_programmatic` (needs second confirmed caller).
- TWidget instance vtable per-class virtual slot identification (e.g. SCROLLBAR-specific virtuals at 0x5a3cd8+0xN).
- Port effort: 8 widget classes now have enough metadata to start C++ skeletons in `src/ui/def/` (size + type-id + factory shape known).
