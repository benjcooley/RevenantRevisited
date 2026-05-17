# Recon Agent Protocol — UI Reconstruction

**Read this BEFORE any forensic recon work.** Every agent (and the coordinator) follows these rules. The rules exist because the UI reconstruction is being done by parallel agents whose findings compound; one careless agent's mislabel corrupts the well that the next agents drink from.

If you're a sub-agent being dispatched on a UI recon task — read this doc first, then your specific brief in `docs/ui/briefs/`.

If you're the coordinator about to spawn an agent — bake these rules into the agent's prompt and remind them again at the end.

---

## Rule 1 — Cost asymmetry: mislabels are MUCH more expensive than missed labels

The dispatch model is "agents see each others' identifications and the work accelerates as well-named classes anchor the next round of hunting." That's the win condition.

The failure mode is the reverse: **one wrong identification propagates as well-named confusion that the next agents trust without re-questioning.** Recovering means:
1. Notice the mislabel (often by hitting downstream nonsense)
2. Trace which renames were derived from the wrong anchor
3. Revert those renames
4. Re-identify correctly
5. Re-propagate

Each step is painful and time-multiplied across however many agents have already built on the wrong label.

**Therefore:** the cost of one false-positive identification is far greater than the cost of zero identifications. "I don't know yet, here's what's needed to confirm" is **always** the right output if the evidence is weak. **Conservative is correct.**

### 98% confidence threshold (from `recon/docs/REFACTORING_APPROACH.md`)

A rename gets applied only when you can justify it with **two or more independent lines of evidence**.

### THE GOLDEN PATH — string anchor in retail decomp + same string emitted by a known src/ function

The single strongest identification chain is:

1. **Find a string literal in the retail decomp** (`FindStringRefs.java <ascii>`)
2. **Grep `src/` for the same string** (`grep -rln "needle" src/`)
3. **Read the src/ function that emits it** to identify which class + method
4. **The retail function emitting the same string IS the same class + method**

Strings survive from pre-release to retail because both eras use them as `Status()` / log facade / error messages. When a Wave-1A-style error string like `"Trouble initializing PlyrStatusBar pane"` exists in the retail binary AND the pre-release `src/` references `PlyrStatusBar`, the cross-reference is decisive: the retail function emitting that string belongs to `TPlyrStatusBar` (or its construction wrapper).

This counts as TWO independent lines of evidence in one move:
- Line 1: the retail-side string (anchor in the decomp)
- Line 2: the src-side string + class context (anchor in our existing code)

Sub-rule (per Rule 2 below — Ghidra class merging): the string identifies the **purpose** of the function. The function may be the class's actual `Initialize` virtual, a free-function init wrapper, or a setup wrapper for a global instance. ALL of those give you a strong identification target, but the right rename depends on what kind of function it is. Safe scope of renames:

| Evidence | What you can rename |
|---|---|
| String anchor + src class forward-decl exists (e.g. `_CLASSDEF(TFooPane)`) | The init/wrapper function: `FUN_00XXXXXX` → `FUN_00XXXXXX_TFooPane_init` (or similar — describes role without overcommitting to method-vs-free-function) |
| Above + you've extracted the function body AND it contains `new cls_0xCCCC(...)` | The constructed class: `cls_0xCCCC` → `cls_0xCCCC_TFooPane` (if the structure also fingerprints as TFooPane) |
| Above + you've dumped the cls_0xCCCC vtable AND its method addresses match the src TFooPane hierarchy | Class + individual virtuals: `meth_0xMMMMMM` → `meth_0xMMMMMM_TFooPane_Initialize` (etc.) |
| Method-level string anchors inside individual methods | Each method individually |

**Don't promote evidence-level. Don't rename a class based on the init-wrapper's identification; verify the class itself.**

### Other evidence types (need one of these as the SECOND line, paired with a string anchor)

- A call into an already-confirmed method (e.g. this function passes its `this` to `TCharacter::Damage`, which we already identified — strong)
- A vtable slot match against the base class (e.g. slot 5 of TScreen's vtable is `Initialize`; finding it at this address confirms the class hierarchy)
- A struct-shape fingerprint (size + field count + offset pattern matches the src class within ±2 bytes)
- A numeric-literal density signature (e.g. matrix code has many `0x3f800000` constants)
- An asset filename loaded — strong if the filename is unique to one subsystem

**One line of evidence is not enough.** A string like `"stat pane"` in an error message tells you the PURPOSE of a function (initializing some "stat pane"), not its CLASS IDENTITY (could be a free function, a wrapper, the actual `TStatPane::Initialize`, or an init function for a global instance of a different class). Use the golden-path src-side cross-reference to disambiguate.

### When in doubt, write to the brief, not the rename file

Every brief in `docs/ui/briefs/` has a "candidates with weak evidence" section. Put borderline-confidence identifications there with a description of what next step would push them to 98%. The next agent's hunt builds on those leads without inheriting unjustified names.

---

## Rule 2 — Ghidra often merges multiple source classes into one cls_0x*

OOAnalyzer's class recovery is not perfect. Several failure modes you'll encounter:

- **Inheritance flattening.** A `TStatPane : public TButtonPane : public TPane` chain may appear as a single `cls_0xXXXX` with the union of all fields/methods. The decomp's "class" is more like "everything that hangs off this vtable address."
- **Method intrusion.** A free function or a method from a different class may appear as a method of `cls_0xXXXX` if Ghidra mistook a register/this-pointer dependency.
- **Vtable sharing.** Two distinct classes that happen to have identical vtables (e.g. a base class with two trivial subclasses that don't override anything) may collapse into one cls_0xXXXX.
- **Instance vs class confusion.** A "class" in Ghidra is identified by its vtable address. If a class has multiple compiled vtables (templates, multiple inheritance, virtual inheritance), each gets its own cls_0xXXXX even though the original source is one class.
- **Stat-pane merging.** Wave-1A saw what looked like TWO "stat pane" init function calls. That could be:
  - Two distinct TStatPane instances of the same class
  - Two distinct classes that both call themselves "stat pane" in error messages
  - One init function called twice (different params)
  - Two adjacent inits from two different classes whose strings happen to overlap

**Therefore:** when you map a cls_0xXXXX to a src-side class name, **also ask "could this single cls_0xXXXX correspond to MULTIPLE src classes that Ghidra merged?"**

### Signs of merging to look for

- **Method count too high for one class.** If the decomp's `class_index.tsv` row shows 80+ methods but the src class has 15, you're likely looking at a merged super-set.
- **Methods that don't logically belong together.** E.g. `cls_0xXXXX` has both "draw a status bar" methods AND "save a character to disk" methods — almost certainly merged.
- **Field offsets that imply incompatible struct layouts.** Fields at `mbr_0x10..0x40` for one purpose then `mbr_0x80..0xc0` for an unrelated purpose with no overlap is a hint the offsets came from two different originals.
- **Vtable slots beyond what the src hierarchy needs.** A pure leaf class shouldn't have 40 virtual slots.

### Disposition when merging suspected

- **Document the suspicion in the brief.** "Candidate identification: `cls_0xXXXX` may correspond to BOTH `TStatPane` AND `TEquipPane` (decomp may have merged them via shared vtable). Evidence for split-not-merged: ..."
- **Don't rename yet.** Renaming to one of the candidates locks in confusion. Either rename to a neutral observation (e.g. `cls_0xXXXX_StatusBarMerged`) or leave bare.
- **Bring the question to the next wave.** Suggest a specific extraction (e.g. "decompile method at 0x4abc12 — its calling convention will tell us if `this` is TStatPane or TEquipPane").

---

## Workflow checklist (every agent, every task)

Before claiming an identification:

- [ ] Read `recon/discovered/README.md` for the confirmed anchors and naming convention
- [ ] Read `recon/discovered/renames/*.txt` for already-applied identifications (your inheritance — but also: don't trust them blindly if downstream evidence contradicts)
- [ ] Read `recon/discovered/port_status/*.md` for per-class vetted status
- [ ] Identify your target class's vtable address (cls_0xXXXX)
- [ ] Dump the vtable (`DumpVtable.java`) — slot order should match src class hierarchy
- [ ] Find at least one **string anchor** in a method body (`FindStringRefs.java`)
- [ ] Find at least one **call into an already-identified method** (cross-reference) OR an **asset-filename load** that uniquely points to your target
- [ ] Verify **field count + total size** is in the right ballpark for the candidate src class
- [ ] **Check for merging signs** (method count too high, mixed-purpose methods, beyond-leaf vtable slots)
- [ ] Only NOW propose a rename — and only if all of the above hold

When proposing renames:

- [ ] One rename pair per identified token, in `recon/discovered/renames/agent_ui_<your_topic>.txt`
- [ ] Header comments at top of the file explain what you identified + which evidence supports it
- [ ] Each rename has at least one comment line above it summarizing the evidence (e.g. `# vtable slot 1 + string "Initializing TPlayScreen" + Close at 0x47b290 confirms`)

When writing the brief:

- [ ] Save as `docs/ui/briefs/B_r<N>_<topic>.md` following the existing brief structure
- [ ] **Evidence section** with every identification + its 2+ lines of evidence
- [ ] **Candidates with weak evidence** section for borderline calls — name them, but don't rename
- [ ] **Merging suspicions** section if you noticed any decomp-side class collapse
- [ ] **Hunt log** entry with date + what was done + what's pending

---

## When to ask the coordinator (instead of guessing)

- The candidate class has 3+ plausible identifications and no clean tiebreaker
- The decomp clearly merges two classes and you can't cleanly split them
- The hunt would require speculative Ghidra-CLI extractions across many addresses
- You discover an anchor that contradicts an existing confirmed identification

The coordinator can re-prioritize, spawn complementary agents, or escalate to the user (the original developer) for ground truth.

---

## Anti-patterns explicitly forbidden

- **Renaming based on filename intuition.** "It loads StatusBar.dat so it must be `TStatusBar`" — wrong; the file could load via a sibling class, a helper, a global init.
- **Renaming based on src-side names without retail-side evidence.** "src/automap.h has TAutoMap, so cls_0xXXXX that mentions 'automap' must be retail TAutoMap" — wrong; the src is pre-release and we explicitly know it diverges from retail; the name might be reused, repurposed, or removed.
- **Bundling many low-confidence renames in one commit.** Each rename should be defensible on its own.
- **Removing or contradicting another agent's renames without coordination.** If you see a prior rename that looks wrong, document it in your brief — don't unilaterally revert. The coordinator decides.

---

## TL;DR for sub-agents

1. **Better to miss than mislabel.** A mislabel propagates and is painful to revert.
2. **A decomp class may = multiple src classes.** Don't assume 1:1.
3. **Rename only with 2+ independent lines of evidence.**
4. **Borderline calls go to the brief, not the rename file.**
5. **Read prior agents' work** so you build on (and don't redo) it — but verify don't trust.
