# TSideTabsPane — Reconstruction Burndown
Spec: TSideTabsPane_SPEC.md   Status: done (Classic path; visually verified --test=ui-sidetabs)

## Assets to load            (spec §2)
- [x] `sidebartabsnotex.dat`:`Up`     52×232 RGB555 → idle strip   (§2)
- [x] `sidebartabsnotex.dat`:`Down`   52×232 RGB555 → pressed strip (§2)
- [x] `sidebartabsnotex.dat`:`Select` 52×232 RGB555 → highlight strip (§2)
- [x] decode via normal TMulti path; `kc=0` keys pure-black `0x0000` only → real alpha, NO extra magenta key (§11)

## Surfaces / render target  (spec §3)
- [x] compose chip into one fixed RT 52×232; blit in HUD pass via DrawSurface (§3 chip model, NOMENCLATURE §3)

## Static elements           (spec §4)
- [x] button0 Book  pane-local (3,2)   36×35 src (3,2,36,35)     upper grp1 (§4)
- [x] button1 Stats pane-local (3,39)  36×35 src (3,39,36,35)    upper grp1 (§4)
- [x] button2 Equip pane-local (3,76)  36×35 src (3,76,36,35)    upper grp1 (§4)
- [x] button3 Spell pane-local (3,113) 36×35 src (3,113,36,35)   lower grp2 (§4)
- [x] button4 Inv   pane-local (3,150) 36×35 src (3,150,36,35)   lower grp2 (§4)
- [x] button5 Map   pane-local (3,187) 36×35 src (3,187,36,35)   lower grp2 (§4)

## Draw order                (spec §5)
- [x] step a: blit base strip (Up) opaque, full 52×232, at pane origin (§5.2a)
- [x] step b: per button with cur>0: overlay Select sub-rect tinted alpha (§5.2b)
- [x] step b': current selection per region held pressed (Down art) (§9/§10)

## Algorithms                (spec §6)
- [x] hover-fade alpha = round(cur*255/8); table cur0..8 → 0..255 (§6)
- [x] per-button overlay blit: src rect == dst rect within strip (§6)

## Effects / shadows         (spec §7)
- [x] none — no drop shadow, no text shadow (§7)
- [x] blit-effect flush is a GPU no-op (§7)

## Text                      (spec §8)
- [x] none — icons baked into strip art (§8)

## Animation                 (spec §9, 24Hz)
- [x] ramp cur 0..8, ±1/tick toward target (8 hovered / 0 not), gated to kSimTickMs (§9)
- [x] selected button held pressed (static), independent of hover ramp (§9)

## Input / dispatch          (spec §10)
- [x] (test) synthetic state cycles both region selections + walking hover; all states exercised (§10)
- [x] two independent 3-way region selectors verified out-of-phase in capture (§1/§10)

## New port primitives needed (spec §13)
- [x] none — DrawBitmapSubrect[Tinted]ToTarget + DrawSurface cover it (§13)

## Errors to close            (spec §0)
- [x] none — no prior port for this panel (§0)

## Spec gaps found during build (feed back to forensics)
- [x] UNCONFIRMED-A (pane screen position constants) blocks exact screen placement.
      pane_x/pane_y use BSS layout constants (DAT_0066614c / 0065be5c / 00667c60 /
      0065be60) not in any body — harness right-anchors with a fixed inset. The
      strip's intrinsic 52×232 size + per-button layout ARE exact; only the
      on-screen offset is approximated. (spec §3 / §14)
- [x] Button-index → command-id wiring (UNCONFIRMED-D) not needed for the visual
      reconstruction (no live click dispatch in the isolated harness); region/mode
      mapping in §4 was sufficient to drive synthetic pressed state.
- [x] Pressed-state art source: spec §9/§10 say the selected button shows the
      "pressed/Select" art but does not pin whether the radio-pressed visual is the
      Down strip or the Select strip. Used Down for the held-pressed selection and
      Select for the hover overlay (matches the decoded strips: Down = magenta-rim
      pressed, Select = green-rim highlight). Minor: if retail uses Select for the
      held selection too, swap g_down→g_select in the selection pass. (spec §9)
- [x] Decoder note: spec §11 warns "do not magenta-key these strips." The existing
      bitmapdecode.cpp already keys only exact-magenta + the per-bitmap keycolor
      (here 0 = pure black 0x0000 = the transparent background); dark-but-nonzero
      ring pixels survive. No code change needed — confirmed against the dump tool
      and the live capture (no black holes). (spec §11)
</content>
</invoke>
