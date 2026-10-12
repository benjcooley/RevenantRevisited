# Qualified Sandswirl seeking-particle core

This is source/controller progress, **not Sandswirl appearance acceptance**. The
licensed registry still lacks `sand`; the original guest changes exactly the
child-name immediate to licensed Sparks. The real named Quicksand spell is an
explicit public caller control, not a discovered Sandswirl variant. No retail
mapping, existing Sparks implementation, asset alias or runtime factory changes.

`effects/seekingparticles.h` independently implements original InitParticles
4e55b0, ResetTargetInfo 4e54b0, Animate 4e5830 and ordered transient Render packets.
It preserves integer life/start/targets, original range RNG, asymmetric x87 float
store boundaries, turn clamping in XY but not Z, gravity/bounce, strict XY hit
boxes, delayed completion and non-mutating trail submission. Reset intentionally
retains the original initialized seekz (41 in this control). Invalid initial
counts/target counts and the original old-zero/new-positive target allocation
bug fail closed. The core chooses no imagery.

All **264,430 pool/RNG/kill fields are bit-exact**, including 158,100 floating
channels (maximum error zero), over the full 310-tick real-actor fixture.
All **5,697 original RenderObject object-index/position packets are bit-exact**.
The child completes at97 and remains stopped. One real map-allocation RNG event
at75 is supplied as an explicit external event; every other Init/Reset/Animate
RNG transition is independently derived, not overwritten per frame.

Read-only entry/render observers preserve every prior state, ordered full draw
hash and framebuffer hash through310, plus the18 sampled PNG hashes. Unit guards
exercise render immutability/order/bounce, equal-bound RNG, target reset retaining
seekz, post-kill suppression and invalid allocation rejection. This does not
certify every possible parameter combination, moving actors, source GPU output,
natural callers, the absent intended sand imagery or exact cross-device pixels.

Private evidence under
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/`:

- `sandswirl-particle-render-native/manifest.json`: SHA256
  `2d86e87d40cb8df6abd98015430a3cfcc38b9749e2b4df818b2fd6860b5e4634`.
- `seeking-particle-proof/manifest.json`: SHA256
  `5b2eb35d76459fa95d2c62c4094e103c39f3f8d1e03d57d1a5316747841fbba6`.
- Prior unchanged full reference: `sandswirl-spell-control-final/manifest.json`,
  SHA256 `c9e022878153e80a6621d7aacd7d5c9a18fa35e8e3e92527c1dc813c587f64e3`.

Reproduce with `seeking_particles_probe.py --trace <new-native-manifest>
--baseline <prior-manifest> --output <fresh-directory>`. It compiles current
source with contraction disabled, compares raw float bytes and integer fields,
checks original render submissions, runs independent guards and freezes hashes.

Root independently reran the current integrated source:264,430 fields and5,697 packets pass bit-exact, with all guards and baseline observer controls passing. Retained report: `recon/retail_asm/runtime/effects/current-sandswirl-parent-20261011/seeking-source/manifest.json`. Visual count remains171/176.

## Public moving-target control

A separate full310 replay invokes actual TCharacter SetPos46ed70 at60 to
(3265,3233,16), then85 to(3249,3265,16). The actual tick75 map query samples target
(80,48,-32); the later actor move does not retarget the particles. Both genuine
characters retain health and flags. The child completes at96 in this scenario.
No direct position-field writes or query-return replacements are used.

Compiled source again matches **264,430 fields bit-exact** and **5,679 original
RenderObject packets**. The parent also matches all4,340 fields, using genuine
actor positions at the query rather than their creation positions. This proves
sampled target acquisition; it does not claim continuous actor tracking.

- `sandswirl-moving-target-native/manifest.json` SHA256
  `3e00fb0d475ab30d520fd3557298714effa925e7eec85cfd2042843c851f6e3a`.
- `sandswirl-moving-particle-proof/manifest.json` SHA256
  `40d8e2412aa65b882c0abbc09698446bc7e0729e43eab9ea10555b86c718f57a`.
- `sandswirl-moving-parent-proof/manifest.json` SHA256
  `549698442282fda8b4554829c24ad91a6553b1329d28364c796690979b614f17`.

Use `sandswirl_qualified_child_probe.py --substitute-sparks
--quicksand-spell-control --move-target` with the same executable and boot image,
then both compiled probes. The two actual SetPos calls are separately logged.

Root independently reran both moving-target source probes:4,340 parent fields,264,430 particle/RNG/kill fields and5,679 render packets pass exactly. Both reports are pinned under `current-sandswirl-parent-20261011`.
