# Sandswirl natural-caller and archived registration audit

No natural Sandswirl caller was found in this finite supplied-source/CD audit.
The restored Sand binding remains an evidenced **modified-retail configuration**;
the real Quicksand spell used by the fixture remains an explicit caller control.
This finding does not remove that qualification from the stationary or moving
target appearance proofs.

## Spell and script evidence

- `legacy/spell.def:334–368` and `legacy/cbspell.def` define Quicksand level
  variants that explicitly select the `Quicksand` effect, not Sandswirl.
- The supplied CD `Resources/spell.def` selects `Quicksand` for both named
  `Quicksand` and `Swamp Pit`. No active textual variant selects Sand or Sandswirl.
  The audit counts 99 textual variant records in this file, not 99 successfully
  loaded runtime variants; the prior genuine startup proof loaded 74.
- `legacy/spell.cpp:388–423` implements the ordinary caller: `TSpell::Timer`
  looks up `variant->effect`, creates that object and calls `SetSpell(this)`.
  It provides no Sand/Sandswirl substitution or fallback.
- All 10,944 non-directory members of the seven supplied CD module archives
  were read, including 15 `.s` script members. None contains the case-insensitive
  token `Sandswirl`, the whole token `sand`, or either exact little-endian ID
  `0xbecefeda` / `0xfecaface`. This is a raw byte search, not a new semantic
  sector-parser proof. It supports the prior parsed Ahkuilon placement negative.
- Among 634 tracked legacy `.cpp/.h/.def/.s/.txt` files, exact quoted Sand or
  Sandswirl literals occur only in the five known class-definition snapshots and
  the three `effect.cpp` snapshots. Those source hits are builder/animator
  registrations and the parent lookup of its `sand` child; no separate quoted
  natural caller was discovered.

## Commented registration block

`legacy/effect.cpp:4483–4486` and its backup comment out particle registrations
for `icedsparks`, `sand`, `blood` and `snow`, while retaining `sparks`.
`legacy/walkcode/effect.cpp:4450–4454` actively registers all five names using
`TParticle3DAnimator`. This corroborates the already restored Sand dependency,
but does not justify enabling every old alias in the shipped configuration.
In particular, current Blood and Snow have separately verified implementations,
and current Icedsparks' exact default geometry proof is not an old particle alias.

The finite scan also records disabled Lightning2 registrations and duplicate
Quicksand/Vortex implementations later in the source. Their presence is archive
history, not evidence that the shipped registrations or current source need a
fallback. No additional resource alias or registration change is proposed.

## Reproduction and limits

Run `tools/retail_runtime/sandswirl_caller_archive_audit.py` with `--legacy`,
`--retail` and a fresh `--output`. The tool only reads the supplied inputs and
writes its private report; it does not boot, patch, or mutate any registry.
It pins every searched module, each script member, the spell definitions, all
matching legacy sources and the legacy commit.

Frozen report:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/sandswirl-caller-archive-final/manifest.json`

SHA-256: `56fe8d415cff73a34ed46ca5513cbb3464e9bb3d4a2727067289fd564904251a`.
Legacy commit: `83f806bb65b762b2cc3e1ea509010f05342caa12`.

The earlier `sandswirl-caller-archive-audit` output is superseded: its registration
regex omitted digits in `3DANIMATOR`, undercounting commented macros. The final
run corrects that scanner; its spell/module absence results are unchanged.

Dynamic construction of names, unsupplied modules/releases, natural actor AI,
and exact shipped enabled behavior remain outside this search. No change to
production source or the existing appearance scope follows from an absence
result. A future unqualified natural-caller claim needs new positive caller/data
evidence, not a renamed Quicksand variant or an invented Sand alias.
