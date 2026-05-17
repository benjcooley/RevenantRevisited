# B.r10 — DEF widget subclasses + remaining activators (Wave-3C)

**Status:** Architecture clarified; one widget class (SCROLLBAR) pinned to its true ctor + vtable. Remaining seven widget ctors identified to a candidate-list (3-5 functions each) but not yet to a single address. selstart activator confirmed; userinfo activator located inside a multi-button dispatch fn.

**Recon agent:** Wave-3C, 2026-05-16 (sibling to Wave-3A TPlyrStatusBar draw + Wave-3B parchment viewer).

**Builds on:** `B_r7_def_widget_engine.md` (Wave-2C). This brief refines the widget-ctor pattern (Wave-2C had labelled the *attribute parsers* as ctors).

---

## 1. TWidget base vtable map @ 0x005a3ab8

Dumped 60 slots via `DumpVtable.java 0x5a3ab8 60`. Recurring patterns + sub-vtable boundaries identified.

| Slot | Offset | Address | Function | Likely role |
|---|---|---|---|---|
| 0 | 0x00 | 0x5a3ab8 | 0x437dc0 | Dtor/delete-self (called as `vtable[0](1)` on parse-failure cleanup) |
| 1 | 0x04 | 0x5a3abc | 0x42a790 | `ParseBody` (TWidget base impl — invoked by dispatcher post-construct) |
| 2-6 | 0x08-0x18 | | 0x438720-0x438770 | Default base methods (generic stubs) |
| 7 | 0x1c | 0x5a3ad4 | 0x42a770 | `SetFlags` (invoked at end of dispatcher: `vtable[7](flags | 0x20)`) |
| 8-16 | 0x20-0x40 | | 0x438820-0x4388c0 | More base stubs |
| 17 | 0x44 | 0x5a3afc | 0x42a940 | Specialized base method |
| 18-20 | 0x48-0x50 | | 0x4388d0-0x4388f0 | Stubs |
| 21 | 0x54 | 0x5a3b0c | 0x42a7a0 | Specialized base method |
| 22 | 0x58 | 0x5a3b10 | 0x42a7f0 | Specialized base method |
| 23 | 0x5c | 0x5a3b14 | 0x438900 | Stub |

**Base vtable ends at slot 23 (0x60 bytes total).** Slot 24 at offset 0x60 = 0x5a3b18 is the next vtable — confirmed to be a *different class* (BITMAP widget) by the register-thunk evidence below.

Notes on the `vtable+0xb4` claim from B.r7: 0xb4/4 = 45. In the 60-slot dump that lands inside the third or fourth packed sub-vtable, *not* a single-class TWidget vtable. The DispatchControl fn at 0x436ec0 accesses `vtable+0x98` and `vtable+0x9c` etc. — those are pane-class vtable indices (TPane / TDefScreen base, not TWidget), reading the pane's own error-formatting virtuals. Wave-2C confused two distinct vtables; the `*param_1 + 0x98` calls on the pane and the per-widget vtable calls on `piVar4` are different objects.

---

## 2. Widget registry structure

```c
// Globals (BSS, populated at program startup by 8 register thunks)
void* DAT_00655510[];   // array of pointers to registration records
int   DAT_0065617c;     // count (= 8 after init)

// Each registration record (heap-static; one per widget class):
struct DefWidgetClassRecord {
    void* vtable;            // +0x00 : widget-class vtable in .rdata (0x5a3xxx)
    const char* name;        // +0x04 : ASCII keyword token (BITMAP/BUTTON/...)
};
```

Dispatcher (`FUN_00436ec0`) iterates `DAT_00655510[0..DAT_0065617c)`, stricmp's each record's `name`, on hit calls `((void(*)(TPane*, TToken*))record->vtable[0])(this, token)` to construct + parse a widget instance, then invokes:
- `widget->vtable[1](this)` — parse body
- `FUN_0041c840(widget)` — attach as pane child
- `widget->vtable[7](flags | 0x20)` — set initial flags

---

## 3. Widget class roster

All 8 widget classes registered into `DAT_00655510`. Pinning to *true ctor + vtable* requires (a) the register-thunk address and (b) a function nearby that sets `*this = &PTR_005a3xxx` and calls `FUN_0042a210(this, N, ...)` with a small type-id.

| Keyword | Register thunk @ | Record addr | Vtable @ | True ctor @ | Status |
|---|---|---|---|---|---|
| BITMAP | 0x42aabc | 0x655dc8 | **0x5a3b18** | ? (candidate FUN_0042aa60 / FUN_0042abe0) | Vtable confirmed; ctor pending |
| (BGBITMAP attr-helper)\* | 0x42ae9c | 0x655508 | ? | ? | Variant — *might not be a true widget class*; see §6 |
| TEXT | 0x42b2fc | 0x656218 | ? | ? (FUN_0042b090 / FUN_0042b5d0 candidate) | Pending |
| BUTTON | 0x42bb4c | 0x6562d0 | ? | ? (FUN_0042bbb0 candidate) | Pending |
| SCROLLBAR | 0x42ddcc | 0x656388 | **0x5a3cd8** | **FUN_0042de00** | **Confirmed** (renamed) |
| LISTBOX | 0x42fe1c | 0x655f48 | ? | ? (FUN_0042f570 / FUN_0042fe80 candidate) | Pending |
| EDIT | 0x431c4c | 0x656210 | ? | ? (FUN_00431410 / FUN_00431ca0 candidate) | Pending |
| DROPLIST | 0x4335ec | 0x655dc0 | ? | ? (FUN_00433640 / FUN_004338d0 candidate) | Pending |

\* The thunk at 0x42ae9c writes to 0x655508 — that record address is BELOW the `DAT_00655510` array base (0x655508 = 0x655510 - 8). That is *unusual* — it suggests this is NOT a registration record but a different global (possibly a "default style" or the BGBITMAP / FRAME attribute-parser table). FUN_0042aed0 body (decompiled to `recon/discovered/FUN_0042aed0_BGBITMAP_attr_parser.cpp`) parses BITMAP + FRAME + DRAWMODE — exactly the BGBITMAP-with-frame composite that TEXT / BUTTON / FRAME / LISTBOX / EDIT widgets all embed. So 0x42aed0 is the shared "BGBITMAP attribute parser" helper, not a widget class. The 0x655508 store may be a parser-table side effect.

---

## 4. SCROLLBAR confirmation (the one widget pinned this wave)

Two independent lines of evidence:

1. **String anchor.** `FindStringRefs.java "SCROLLBAR"` returned 3 hits. Two are inside `FUN_0042f9f0` (LISTBOX's attribute parser — LISTBOX has a nested SCROLLBAR child). The third hit at `0x42dddd` is in *unbound code adjacent to the SCROLLBAR register thunk at 0x42ddcc*. That register thunk writes record `0x656388` into the registry — uniquely the SCROLLBAR slot.
2. **Ctor pattern.** `FUN_0042de00` body (decompiled to `recon/discovered/FUN_0042de00_TDefScrollbar_ctor.cpp`):
   ```c
   FUN_0042a210(param_2, 5, /* geom + style */ ...);        // base-init w/ type-id 5
   *param_1 = &PTR_FUN_005a3cd8;                            // SCROLLBAR widget vtable
   param_1[0x24] = 0; param_1[0x25] = 0; param_1[0x26] = 0; // scrollbar position
   param_1[0x27] = 1000; param_1[0x28] = 100;               // range + step
   ```
   Sets *this = 0x5a3cd8 (a fresh per-class vtable distinct from TWidget base), passes type-id 5 to the base-init, and initializes scrollbar-specific fields with values that are *unmistakably* a scrollbar's defaults (range 1000, step 100, position 0).

Combined: this is the SCROLLBAR widget class, vtable at 0x5a3cd8, ctor at 0x42de00.

---

## 5. Per-screen activator hunt (selstart + userinfo)

### selstart — CONFIRMED at `FUN_00469370`

90-byte function, bound by Ghidra, refs `"selstart"` directly. Body is:

```c
iVar1 = FUN_00435150(s_selstart, &DAT_00659cb8, 0x11, 0x7e, 0x41, 0x18a, 0x13c,
                     0x15c, 0x54, s_widgets, s_selstart);
if (iVar1 == 0) return 0;
param_1[0x60..0x62] = 0;
return 1;
```

Identical pattern to the other 11 confirmed per-screen activators (`FUN_005399f0` savegame, `FUN_00465a50` createchar, etc.). **Renamed.**

### userinfo — FOUND inside `FUN_00463149` (a multi-button dispatcher)

`"userinfo"` string has 4 ref sites, all in unbound regions. Force-extraction at `0x463149` via DecompileAddr produced a 1850-byte function that branches on a button name (`FUN_0059a530` = stricmp) and dispatches across:

| Button name | Action |
|---|---|
| `"userinfo"` | `FUN_00435150("userinfo", "default", 0x11, 0x7e, 0x41, 0x18a, 0x13c, 0x118, 300, "widgets")` — OPENS userinfo.def modally |
| `"hostgame"` | Multiplayer host setup (loads MP module data, calls FUN_0046e6f0) |
| `"joingame"` | Multiplayer join (validates network + module match) |
| `"?"` (single char) | Help popup |
| `"deathmatch"` | Toggle SimpleMode |
| `"newchar"` | Open createchar with no template |
| `"editchar"` | Open createchar with existing template |
| `"delchar"` | Delete-confirm popup + delete |

So **userinfo's activator call IS in this function** (the first branch). But this fn's *identity* is not "userinfo activator" — it's the **multiplayer-lobby button dispatcher** (most of the button names are MP-lobby actions). The userinfo activation is one of seven branches.

Two options for the next wave:
- **(A)** Refactor / re-extract: locate the parent button-action vtable hook that calls this dispatcher, and identify if there are smaller per-action handlers that wrap each branch.
- **(B)** Treat this fn as the multi-target activator and add a `LoadDef_userinfo_branch_at_0x463149` rename. Done conservatively here as `MPLobby_ButtonDispatch`.

Renamed conservatively as `FUN_00463149_MPLobby_ButtonDispatch`. Userinfo's true activator entry-point — if there's a thinner wrapper outside this fn that just hands `"userinfo"` to FUN_00435150 — was not located in this wave.

---

## 6. Open questions for Wave-4

1. **Widget vtable pinning.** For each of BITMAP / TEXT / BUTTON / FRAME / LISTBOX / EDIT / DROPLIST: run `FindImmRefs.java 0x655dc8` (BITMAP record), `0x656218` (TEXT), `0x6562d0` (BUTTON), `0x656388` (SCROLLBAR — already done; vtable 0x5a3cd8), `0x655f48` (LISTBOX), `0x656210` (EDIT), `0x655dc0` (DROPLIST), `0x655508` (BGBITMAP-helper). The two MOV hits per record give (a) thunk-side address + (b) the vtable write `MOV [record_addr], 0x5a3xxx`. The second MOV's immediate is the vtable address. Once all 8 vtable addresses are known, the .rdata layout from 0x5a3ab8 onward can be partitioned into per-class vtables.
2. **True ctor identification.** For each widget class: starting from the register thunk (e.g. 0x42aabc), scan the next ~200 bytes of unbound code to find the call to `FUN_0042a210(this, N, ...)` — the function containing that call IS the widget ctor. N gives the type-id (SCROLLBAR = 5; others unknown but presumably 0..7).
3. **`FUN_0042a210` identity.** This is the shared widget-base init helper used by every widget ctor. Size 306 bytes. Anchor it as `TDefWidget_BaseInit` once two ctor invocations confirm the signature.
4. **`FUN_0042a350` re-labelling.** Wave-2C named this `TWidgetBase_ParseAttrs`. Per the vtable dump, the TWidget base vtable slot 1 (ParseBody) points to `0x42a790`, not `0x42a350`. So `0x42a350` is *also* an attr-parser entry, but it's not the vtable hook. May be a free-function helper invoked from inside `0x42a790`. Worth re-verifying the role.
5. **Pre-release src class names.** Searched `worktrees/ui/src` and `RevenantRevisited/src` for `TDefButton` / `TDefText` / `TDefBitmap` / `TDefScrollbar` etc. — none present. The pre-release tree does not have these classes (which fits the brief's note that the DEF engine was a late addition for the modal screens). Naming convention proposed: `TDef<Widget>` (e.g. `TDefScrollbar`) since no prior src convention exists. Confirmed only for SCROLLBAR in this wave; the others are pending.
6. **userinfo activator decomposition.** If a per-screen activator naming convention should be preserved (1 fn per screen), `FUN_00463149` should be split into seven per-button handlers. This requires identifying whether the binary has them as separate functions that the dispatcher calls (and Ghidra merged), or whether the seven branches are inline (and the original C++ source was a single dispatch). The function size (1850 bytes for 7 simple branches) suggests inline branches — likely a single source-level fn.
7. **Wave-2C ctor-vs-attr-parser cleanup.** Wave-2C's renames on FUN_0042aaf0 / 0042b340 / 0042bd90 / 0042d730 / 0042f9f0 / 00431750 / 00433170 use the suffix `*_ctor`-ish naming via the per-widget class label. After Wave-4 pins the true ctors, the Wave-2C names should be migrated to `*_attr_parser` suffix to reflect their real role.

---

## 7. Renames applied this wave

In `recon/discovered/renames/agent_ui_wave3_def_subclasses.txt`:

1. `FUN_0042de00` → `FUN_0042de00_DefWidget_TDefScrollbar_ctor`
2. `FUN_00469370` → `FUN_00469370_LoadDef_selstart`
3. `FUN_00463149` → `FUN_00463149_MPLobby_ButtonDispatch`

Not running `apply_renames.sh` per coordinator instruction.

---

## 8. Hunt log

- **2026-05-16 (Wave-3C, ~25 min):**
  - Read AGENT_PROTOCOL + discovered/README + B.r7 brief + DefWidgetEngine.md.
  - DumpVtable.java 0x5a3ab8 60 → identified base vtable boundary at slot 23 (0x60); slot-24-onward = next class' vtable.
  - FindImmRefs.java 0x00655510 → 8 register-thunk MOVs + 4 dispatcher MOVs. Mapped thunk addresses to record addresses.
  - FindImmRefs.java 0x00655dc8 → confirmed second MOV at 0x42aad7 writes vtable address 0x5a3b18 into BITMAP record. Pattern established.
  - DecompileAddr.java 0x0042aaf0 / 0x42aed0 → these are STYLE-attribute parsers, not class ctors (signatures take 3-4 args, no `*this = vtable_ptr` write). Wave-2C's ctor labels are misnamed but harmless.
  - FindStringRefs.java "SCROLLBAR" → hit at unbound 0x42dddd (in register thunk region). DecompileAddr.java 0x0042de00 → set `*this = 0x5a3cd8`, called FUN_0042a210(this, 5, ...). SCROLLBAR confirmed.
  - FindStringRefs.java "selstart" → ref at 0x469371 inside bound FUN_00469370. Extracted body → clean 90-byte DefScreen_Open wrapper. Activator confirmed.
  - FindStringRefs.java "userinfo" → 4 hits, all unbound. DecompileAddr.java 0x00463149 → 1850-byte multi-button dispatcher with userinfo as first branch.
  - Wrote rename file (3 renames) + this brief.

- **Pending (Wave-4):**
  - Find remaining 7 widget vtables via per-record FindImmRefs (one-shot search per record address).
  - Find remaining 7 widget true ctors by scanning post-thunk regions for `FUN_0042a210(this, N, ...)` calls.
  - Identify `FUN_0042a210` (base-init helper) formally.
  - Optionally: re-extract `FUN_00463149` per-branch handlers if a per-screen activator naming convention is preferred.
  - Migrate Wave-2C's `*_ctor`-like names on the attr-parser functions to `*_attr_parser` (post-Wave-4 cleanup).
