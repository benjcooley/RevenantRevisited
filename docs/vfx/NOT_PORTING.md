# VFX Effects We Will NOT Port (vestigial / unreachable in shipped)

Effects that shipped to the retail binary's class registry but have **no live
caller in shipped data** (no `spell.def` variant, no `ATTACHEFFECT`, no
`char.def` reference) — or whose **asset was dropped from shipped
`data/imagery.rvi`**. Porting them is wasted work: nothing in the shipped
game invokes them. Save these cycles for effects that actually play.

If gameplay reactivation ever surfaces one (new module, mod, etc.), the
forensics doc + (where present) a minimal port are preserved for re-enable.

---

## Status legend

- **NO-PORT** — never started; forensics only; skip in fan-outs
- **MINIMAL** — port exists at `src/effects/<name>.cpp` but rendered to the
  least-effort snapshot-faithful state and **left there**; no further iteration
- **UNREGISTERED** — port exists but commented out of `vfxtest.cpp` registration
  because the I3D asset isn't in shipped data

---

## The list

| ID | Class | Status | Why not | Asset shipped? | Notes |
|---|---|---|---|---|---|
| F03 | `TFireEffect` | NO-PORT | No live `EFFECT "fire"` caller anywhere in shipped or legacy data. Phase-B bespoke exists (`src/effect.cpp`) but the planned ambient-fire spell was WIP and never wired. | Yes (Misc/Fire.I3D) | Forensics `F-FIREFLASH` family. Per F03 inventory line: "Vestigial — no live retail caller." |
| F05 | `TFireSwarmEffect` | MINIMAL | Class + asset shipped but `"FireSwarm"` has no spell.def variant, no ATTACHEFFECT, no char.def reference. The "growing fire ring" visual users may recall in early CAVES is actually **Fire Wind** (different class). | Yes (Magic/FireSwarm.i3d) | Ported minimally: cylinder ring + center glow disc. 4 unextracted retail particle systems in `cls_0x5abe4c` left as future Ghidra task. Per `src/effects/fireswarm.cpp`. |
| F06 | `TFaultFireEffect` | MINIMAL | Vestigial / pre-release-only. No live caller. User: "doesn't look like any recognizable game effect" (because it never shipped one). | Yes | Ported minimally as a UV-scrolling textured strip. Per `src/effects/faultfire.cpp`. |
| F09 | `TFireColumnEffect` | UNREGISTERED | Vestigial + asset `Magic\FireColumn.I3D` **dropped from shipped `data/imagery.rvi`** (per F11 forensics §2.1 sister-effect cross-check). No I3D to load even if we wanted to render it. | **NO — asset missing** | Port in `src/effects/firecolumn.cpp`, registration block commented out in vfxtest.cpp. Re-enable if asset recovered. |
| F11 | `TFlameDiscEffect` | UNREGISTERED | Vestigial + asset `Magic\FlameDisc.I3D` **dropped from shipped `data/imagery.rvi`**. Class code shipped but never instantiable. | **NO — asset missing** | Port in `src/effects/flamedisc.cpp`, registration commented out. Re-enable if asset recovered. |
| M07 | `TPhotonEffect` | NO-PORT | Forensics §12: exhaustive grep across all `*.def` finds zero `EFFECT "Photon"` / `SPELL ... "photon"` / `MAGICATTACK ... "photon"` / `INVOKE ... "photon"` calls. The retail binary's only string XREFs are the registry slots themselves. Vestigial-or-NPC. | Yes (Magic/Photon.I3D) | If a future NPC/spell turns out to invoke "photon", port via shim then. Forensics `M07_TPhotonEffect.md` preserved. |
| MS | `TMeteorStormAnimator` (the WIP snapshot animator) | NO-PORT | The MeteorStorm SPELL is live in shipped — but the **shipped binary uses `TStrikeEffect`**, not the snapshot's `TMeteorStormAnimator`. The snapshot animator itself is the vestigial part. | Yes (Magic/comet.I3D) | Port TStrikeEffect (different forensics) when ready. Don't port the snapshot's TMeteorStormAnimator. |

---

## How to handle a new candidate effect

Before adding to a fan-out wave, check:

1. **Is the I3D asset in `data/imagery.rvi`?**
   `find RevenantRevisited/i3d_dump_all -iname <asset_basename>` — if absent, UNREGISTERED.
2. **Is there a live caller?**
   ```sh
   grep -niE "<RegistrationName>|<registration_name>" data/Resources/*.def legacy/*.def 2>/dev/null
   ```
   If only the binary's registry slots reference it (no `SPELL`/`EFFECT`/`ATTACHEFFECT`/`MAGICATTACK`/`INVOKE` lines), it's vestigial → NO-PORT.
3. **Does the forensics doc §12 (caller / where-to-see) explicitly say
   "vestigial" or "no live caller"?** If yes, NO-PORT.

Save the cycles. Port what plays.
