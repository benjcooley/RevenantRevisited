# Cure linked-spell predicate: corrected pointer provenance

The earlier audit incorrectly called the pointer at retail TSpell+`0x11c`
VariantData. It is **SpellData**. The cleanup word is the spell definition's
`POISONCHANCE`, not a missing variant flag or a relocated existing variant
field. The historical fail-closed implementation was conservative, but its
diagnostic identified the wrong data structure.

## Independent executable proof

- Retail TSpell constructor `0x53f090`: parameter five becomes EBP; at
  `0x53f161` EBP is stored at TSpell+`0x11c`. At `0x53f14d` the same pointer's
  `+0xa4` value supplies the initial effect delay.
- Spell definition parser `0x53e4e0` parses literal `DELAY %i` into
  definition+`0xa4` and literal `DAMAGETYPE %i` into definition+`0x80`.
- At `0x53e5c3..0x53e5d0`, the parser takes definition+`0xc8` as output for
  the literal `POISONCHANCE %i` format at string VA `0x5e4b58`.
- Definition constructor `0x53da50` defaults `+0xc8` to zero; the definition
  loader allocates `0xcc` bytes. This is the later retail SpellData layout,
  including icon/light fields, not the variant's smaller allocation.
- Accessor `0x4f54d0` returns TSpell+`0x11c`. Damage method `0x53f560`
  reads its `+0x80` as damage type, providing another independent association.
- The actual variant pointer is separately stored at TSpell+`0x120`
  (`0x53f153`), proving the distinction directly.

Cure's natural end at `0x4e1230` reads definition+`0xc8`. The comparison at
`0x4e123a`, followed by **JE** at `0x4e1240`, skips clearing poison when the
word is zero. For a nonzero word, target count must be positive and target 0
nonnull; `0x4e125d` calls SetPoisoned(FALSE) on **spell target 0**. It does not
infer the predicate from the effect/spell name and does not necessarily clear
the enemy selected by Cure's first owner Pulse.

The same definition word participates in ordinary TSpell construction at
`0x53f10b..0x53f143`: only when an explicit target array was supplied and the
chance is nonzero, process targets in array order, draw inclusively from
`0..100`, and set poison when `roll < chance`. Chance 100 succeeds on 100 of
101 possible rolls; zero draws nothing; negative nonzero chances still draw
but cannot succeed. No clamp or modern probability normalization is sourced.

## Shipped data and scope

Original `resources.rvr` member `spell.def` SHA256:
`22a0ec0799d29fcb7504d98aee0399762f9538521e7a4cd56aef07bba74cd854`.
The only POISONCHANCE tag is 90 under SPELL Poison. CureP has no tag and hence
defaults to zero. Its Cure Poison variant binds **Streamer** with FOLLOW
controller data. No shipped variant in this file binds literal effect Cure.

Consequently a character-driven test of the bespoke `0x152dafef` Cure must be
labeled a synthetic binding of the actual runtime, not proof that the shipped
CureP spell uses this animator. The shipped curative Streamer path remains a
different effect/controller scope.

## Narrow port correction

`SSpellData.poisonchance` is initialized to zero and parsed by the exact
POISONCHANCE tag. `TSpell::PoisonChance()` exposes this semantic field through
the existing SpellData pointer; SSpellVariant is not padded or enlarged to
imitate raw retail offsets. The existing constructor target-copy path now
performs the proved per-target probability loop, including its strict/inclusive
edge cases and lack of clamping.

TSpell additionally captures weak invoker/target identities while constructor
inputs are live. New weak accessors leave existing raw APIs unchanged. Cure
uses these identities for later aiming and target-zero cleanup, so deletion
and reuse of a map slot cannot mutate an unrelated replacement actor.

The previously rejected whole spell path is restored in the Cure owner:

- First Pulse retries while spell/invoker is absent; otherwise uses the
  invoker's fighting target or the first live enemy within 400 units, moves
  the effect there and sets poison, then consumes the first-time flag.
- The first actual simulation step binds the source aiming angle from the
  live spell target/invoker after normal target placement; null spell yields
  explicit zero. Submit never advances this state.
- Natural completion conditionally clears spell target 0 only for nonzero
  SpellData.poisonchance, then marks the owner for safe deferred reap and
  calls its linked spell's Kill API. Source first-Pulse mutation is not
  silently changed to an intuitive unconditional curative action.

All 80/5 visual state, mesh/material, RNG and rendering behavior is unchanged
by this correction. The renderer and shared effect/VFX implementation were
not edited in this step. Real linked-spell runtime acceptance is still pending
the parent's actual actor/owner timeline and stable-identity capture.

## Focused validation

Run `python3 /Users/benjamincooley/RevenantRetailLab/check_cure_spell.py`.
It compiles the actual TSpell header, constructor/destructor, actual Cure
FinishSpell body and POISONCHANCE parser branch with focused token/actor stubs
and the real SafeRef implementation. ASan/UBSan passed 625 cases plus the
previous spell lifetime/manager suite. Coverage includes valid/malformed
parsing; all 101 rolls for chances -1/0/1/90/100/101; explicit/null/multiple
target arrays; zero/nonzero cleanup polarity; absent definition/spell; and
deleted/recycled target and invoker identity rejection.

Hashes and logs are retained under `lab/research/cure/spell-data/`. This
standalone contract evidence is not a substitute for character-driven runtime
or retail visual acceptance. No full-engine, render or guest process was
started by this child.
