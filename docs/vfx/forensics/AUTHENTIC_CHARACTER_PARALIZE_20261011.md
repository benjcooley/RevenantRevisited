# Authentic character dependency preflight for actor effects

This supplies actual constructed characters and original actor methods, not a
new visual pass. `character_native_preflight.py` reuses the source-matched
`HudWorld` original startup image: CRT,571 static constructors, WinMain/game
init, resource packs, rules,26 classes and2968 types. The private copied image
`hud-boot-09c83243d43516b6.image` SHA256 is
`01b0a34a14e9eac284b941a69453183a7356d64b95dd93ca591b903380c2b4e3`.
Its original cache was in the UI worktree; neither that image nor its sources
were mutated. The owned redundant cold boot was stopped after finding the
matching cache. Source-only tools require the matching executable/boot-source
cache key and accept a private image path.

## Minimal authentic setup demonstrated

The boot checkpoint precedes the first screen. A negative control asks original
CHARACTER NewObject `474bb0` to construct shipped **Acolyte** at(3185,3185,16).
It executes the real base constructor `46e1f0`, complex initialization `4db000`
and ClearChar `4c18a0`, then faults exactly at`4c18ea` on CurrentScreen(null)+48.
The class factory inlines the character constructor rather than entering the
separate out-of-line constructor`4c16b0`.

The positive fixture executes **original ShowScreen `4909d0..4909f0` only**:
it selects the already-constructed global PlayScreen`65caf0` and zeros its
frame count. It stops before BeginScreen/map startup. The same original class
factory then returns a genuine character with vtable`5a7848`, original imagery,
action-block constructor`4da9f0`, Rules lookup`48c780` and authentic stats.
Two Acolytes at(3185,3185,16) and(3281,3185,16) each have original Health102,
Radius16 and no_paralyze0. Original BeginFighting`4d3b90(target,3)` creates the
real action3/root target. Original Disable`4cefb0` and Stop`4cee70` execute and
return. There are **no added character/stat/radius/pose/action/damage seams**.
Inherited HUD boot logging/network/host-API boundaries remain declared.

## Real target-bound Paralize1 owner

`paralize_actor_preflight.py` loads all74 shipped spells through original
SpellList.Load`53ead0`, selects the actual Physical Paralysis definition and
variant, and executes Spell creator`542200`→constructor`53f090`. It constructs
exact `paralize1` via original class NewObject, then applies the documented
inline TEffect::SetSpell pointer binding. No owner physics or actor fields are
seeded in place of original initialization.

Complete original NextFrame and Pulse`4fadb0`→first Init`4fa540` run280 ticks.
The actual fighting target is accepted; variant1, target_count1, duration240,
initial grow, movement, shrink and **kill260** execute. Target Disable/Stop and
paralysis flags run throughout; cleanup restores original target flags, and
Health102/declared position remain unchanged. Physical Paralysis is DT_NONE,
and observation confirms **no damage calls**; none were intercepted or faked.
Post-kill owner bytes and RNG-call count stay unchanged. All actual particle
positions stay within X[3183.5,3350.5962],Y[3139.2090,3246.4094],Z[20.7000,71.5]
for this declared target case. This bounded branch avoids the genuine null-
invoker singular trajectory without altering its source behavior.

The full target-bound renderer, three animated egg passes, compiled source
state comparison and actual Metal appearance are next gates. The fixture does
not run full PlayScreen.BeginScreen/map startup or independent character Pulse,
does not render victim bodies/bones, and does not claim natural spell invocation,
audio, damage behavior of other variants, device fidelity or appearance credit.
The previous null-invoker partial preview remains unchanged.

Evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/`.
- `character-authentic/preflight-final/manifest.json`: `683e22236f895950d4fcd6feb221a7fc2d66a0956318b723ae7c51611ce76c54`
- `paralize-actor-native/manifest.json`: `610d00000b8dde2c0f187ac809569246276318758cff145ea5cf4d4b20bc2230`
