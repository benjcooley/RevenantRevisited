# Fire/Dust caller and cardinal-render audit

No animated Fire or visible Dust software configuration was established. This
follow-up does not repeat Dust's four quality/light/location controls or infer
a Fire timer from its 33 resident texture frames. No engine change or appearance
credit is proposed.

`fire_dust_caller_audit.py` checks exact type IDs and NUL-terminated names,
including suffix-pooled names, against executable text-section literals. Neither
type has a literal code reference. It scans 73 `.s`/`.def`/`.txt` files across the
two resource archives and all seven shipped module archives. The three candidate
lines are the two class declarations and unrelated Fire Flash description text;
none is a Fire/Dust spawn or attachment command. The separate placement inventory
finds neither exact ID in 4,896 Ahkuilon sectors, with zero parse errors. These are
bounded inventories, not proof that computed names or external caller mutations
are impossible.

## Positive Fire cardinal control

`fire_cardinal_native_probe.py` supplies the four predeclared cardinal facings
0/64/128/192 through the actual `SObjectDef.rotatez` field before class factory
`474bb0` executes. The definition differs only at byte `0x32`, and the actual
constructed owner is checked. All original loaded-resource initialization,
NextFrame/Pulse/Animate/PreRender/Render/PostRender, frame selection and software
raster execute. The unchanged fixture is source `(3185,3185,16)`, camera
`(3185,3185,0)`, 1280×960 with center `(640,480)`, ambient38/directional1.

Three consecutive full frames per facing give:

- Facing0 and128: no nonzero pixels.
- Facing64: one pixel `(631,462)`, RGB565 `0x1020`, on all three frames.
- Facing192: one pixel `(647,469)`, RGB565 `0x1040`, on all three frames.

Every owner/animator/texture frame remains0 and RNG calls remain0. This is a
genuine positive original software sample caused by standard construction facing;
there is no texture replacement, geometry enlargement, brightness change, fitted
camera or forced animation. It does not establish a natural placement or an
animated-flame appearance. The earlier default-facing blank finding remains true.

## Dust device boundary

Dust's complete authored color envelope and literal texture combinations remain
zero under the original RGB565 additive raster, as recorded in
`DUST_AUTHORED_NATIVE.md`. Packets are prelit; changing normal lighting cannot
restore a discarded source contribution. No alternate caller mutation was found.

The original scene dispatch at `4174c4` selects internal software `56eb30` when
`5d7a28 != 0`; the other branch at `4174fd..417528` calls actual D3DDevice3
`DrawIndexedPrimitive`. The current HUD device fixture advertises only RGB
emulation and deliberately rejects device Draw calls. Merely changing that global
or accepting the COM call would supply no original device pixels. A qualified
hardware-device investigation remains possible; it is not a positive software
reference and does not justify changing the port's software quantization.

## Frozen evidence

All paths below are relative to private
`research/vfx-wide-20261010/hard-astra/`; generated evidence is not committed.

- `fire-cardinal-final/manifest.json`:
  `8e3ca77e4f6cc114523e5e4c7e996e55c88a37f825af1cba7400e974350fbf50`.
  Twelve full original frame images, complete draw packets and input definitions.
- `fire-dust-caller-final.json`:
  `5e90f54b940c09abdd8085ba9bbe547bb67b71221b14f34931d24a57d37858ce`.
  Executable/archive/probe hashes and all candidate lines.
- `fire-dust-ahkuilon-placements.json`:
  `fbb4c08a88ca9875d2f4046607bc01087740ac8d573f65588ede2bc8344fd2b4`.

The original executable remains SHA256
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.
No modified retail build was required for these results. No root approval file,
central status ledger, shared renderer or runtime binary was changed.
