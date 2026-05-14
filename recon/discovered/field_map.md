# Retail Field Offset Cross-Reference

For each major retail class, this maps `mbr_0xXX` field references in
recon/**/*.cpp back to the corresponding source field name in `src/*.h`.
Without this every port-from-Ghidra has to re-derive offsets from scratch.

**Confidence**: high = direct evidence (a known method reads/writes it);
medium = strong structural fit (size, position, ctor pattern); low = guess.
TCharacter / TPlayer entries below medium are intentionally omitted -- the
map is only useful if every entry is trustworthy.

Evidence pointers cite (a) recon-discovered cpp bodies under
`recon/discovered/cls_*.cpp`, (b) rename-pair files under
`recon/discovered/renames/agent_*.txt`, and (c) source declarations under
`src/`. "Slot N" in evidence refers to a vtable slot offset relative to
the class's vtable address.

Source field declarations follow the order in the corresponding header
(`object.h`, `complexobj.h`, `character.h`, `player.h`, `rules.h`).
The "Source loc" column gives the source line in the header where the
field is declared.


## TObjectInstance -- `cls_0x5b8e94`

Vtable @ 0x5b8e94 (125 slots). `class TObjectInstance : protected SObjectDef`
in `src/object.h:671`. Layout = vptr@0 + SObjectDef inline (0x34 bytes
of POD), then TObjectInstance's own protected fields. Retail equivalent
to source field count (≈22 declared fields after SObjectDef).

Cross-references rely on:
- agent_baseclass.txt note "+0x10/+0x14/+0x18 = pos.{x,y,z}; +0x58 =
  animator; +0x80 = notifyflags; +0x84 = script" (confirmed by Notify,
  SetPos, CreateAnimator, FreeAnimator decompiles).
- TObjectInstance::~TObjectInstance recon
  (`cls_TObjectInstance_Dtor_46e420.cpp`) walks animator (+0x58),
  inventory list, and stats array.
- AI / SetFlags / OnScreen field-byte references in
  `cls_0x5a7b98_TCharacter_AI_4c8b60.cpp`.

| Offset | Size | Retail token | Source field | Source loc | Confidence | Evidence |
|---|---|---|---|---|---|---|
| 0x00 | 4 | vptr | (vtable) | -- | high | every method begins with `*param_1` indirect call; vptr value 0x5a50e8 confirmed in dtor recon |
| 0x04 | 2 | mbr_0x4 | objclass (SObjectDef) | object.h:628 | high | TCharacter::AI tests `(short)param_1[1] == 0xb` for OBJCLASS_PLAYER |
| 0x06 | 2 | mbr_0x6 | objtype (SObjectDef) | object.h:629 | high | matches SObjectDef ordering, MapPane.GetInstance reads short at +0x6 |
| 0x08 | 4 | mbr_0x8 | flags (SObjectDef) | object.h:630 | high | AI tests `*(byte*)(this+8) & 0x40` = OF_DISABLED (0x40); SetFlags @ 0x472e90 manages bits at +8 (agent_baseclass.txt) |
| 0x0c | 2 | mbr_0xc | state (SObjectDef) | object.h:632 | high | SObjectDef.state is the next u16 after flags; SetPos updates short at +0xe (level), so state is at +0xc |
| 0x0e | 2 | mbr_0xe | level (SObjectDef) | object.h:633 | high | SetPos updates "level at +0xe (short)" per agent_baseclass.txt note on virt_meth_0x46ed70 |
| 0x10 | 4 | mbr_0x10 | pos.x (SObjectDef) | object.h:635 | high | TCharacter::AI reads `param_1[4]` for pos.x; agent_baseclass.txt and Distance recon both cite +0x10 |
| 0x14 | 4 | mbr_0x14 | pos.y | object.h:635 | high | TPlayer::Pulse uses `param_1[5]` for pos.y; AI uses param_1[5/6] in waypoint dist; distance LUT at +0x10/+0x14 (Distance @ 0x46ea20) |
| 0x18 | 4 | mbr_0x18 | pos.z | object.h:635 | high | EffectBurst recon notes "pos = this+0x10..0x18" |
| 0x1c | 4 | mbr_0x1c | vel.x (SObjectDef) | object.h:636 | medium | structural fit -- vel follows pos in SObjectDef; ResolveHit applies impulse vel here |
| 0x20 | 4 | mbr_0x20 | vel.y | object.h:636 | medium | as above |
| 0x24 | 4 | mbr_0x24 | vel.z | object.h:636 | medium | as above |
| 0x28 | 4 | mbr_0x28 | accum.x (SObjectDef) | object.h:637 | medium | structural fit; ClearAccum() memsets sizeof(S3DPoint) at this position |
| 0x2c | 4 | mbr_0x2c | accum.y | object.h:637 | medium | as above |
| 0x30 | 4 | mbr_0x30 | accum.z | object.h:637 | medium | as above |
| 0x34 | 1 | mbr_0x34 | rotatex (SObjectDef) | object.h:639 | medium | structural fit at end of SObjectDef byte triple |
| 0x35 | 1 | mbr_0x35 | rotatey | object.h:640 | medium | as above |
| 0x36 | 1 | mbr_0x36 | rotatez/facing | object.h:641-644 | high | TCharacter::Face @ 0x4d7020 sets `mbr_0x36` and moveangle (`mbr_0xb0`); EffectBurst uses this->mbr_0x36 - 0x80 for facing-relative spawn |
| 0x37 | 1 | mbr_0x37 | group (SObjectDef) | object.h:647 | medium | last byte of SObjectDef |
| 0x38 | 8 | mbr_0x38 | stats (TStatisticList = TSizableArray<int32_t>) | object.h:1313 | medium | dtor: "Frees stats array (this[0xe])" -> byte 0x38; TSizableArray<i32> is 8 bytes (numitems@0, size@2, items@4) |
| 0x40 | 4 | mbr_0x40 | mapindex (or similar id) | object.h:1256 | high | MapPane.GetInstance walks "obj.mapindex(at offset 0x40)==index" per agent_actionsmap.txt |
| 0x4c | 4 | mbr_0x4c | inf (SObjectInfo*) | object.h:1264 | high | AI_PerMonster reads `*(char**)(param_1[0x13])` and stricmp's result vs monster type names ("Solifuge", "Jhaga", ...). this[0x13] = byte 0x4c. inf->name (SObjectInfo::name) string lookup. |
| 0x50 | 4 | mbr_0x50 | shadow | object.h:1265 | high | TMapPane::AddShadow @ 0x451cb0: "stores shadow index back into oi+0x50" (agent_actionsmap.txt) |
| 0x54 | 4 | mbr_0x54 | imagery (TObjectImagery*) | object.h:1268 | medium | dtor calls FUN_00446ba0(this[0x15]) at +0x54 cleanup; imagery typically released here |
| 0x58 | 4 | mbr_0x58 | animator / first component | object.h:1275 | high | agent_baseclass.txt: "+0x58 = animator"; CreateAnimator (0x46e8b0) tests +0x58, FreeAnimator (0x46e900) zeros it |
| 0x60 | 4 | mbr_0x60 | owner | object.h:1300 | medium | RemoveFromInventory (0x46faf0) clears "+0x19 (parent inventory ptr at byte offset 0x64)" per agent_baseclass.txt -- owner is the field directly preceding inventory |
| 0x64 | 16 | mbr_0x64 | inventory (TPointerArray<TObjectInstance,0,4>) | object.h:1301 | high | dtor: "Iterates inventory list (this[0x1e] count this[0x1a])"; this[0x1a]=byte 0x68 = numitems, this[0x1e]=byte 0x78 = items[]. TPointerArray is 16 bytes starting at +0x64 |
| 0x74 | 2 | mbr_0x74 | inventnum | object.h:1302 | medium | follows TPointerArray; RemoveFromInventory clears this region |
| 0x76 | 2 | mbr_0x76 | invindex | object.h:1303 | medium | as above; pair of shorts after inventory array |
| 0x78 | 1 | mbr_0x78 | commanddone | object.h:1306 | medium | bool follows the inventnum/invindex shorts |
| 0x7c | 4 | mbr_0x7c | script (PTScript) | object.h:1307 | medium | dtor frees this[0x27] (=0x9c) and this[0x28] (=0xa0) -- two script handles; this is actually for prevstate region. Script is at +0x84 per Notify recon. (See +0x84 row.) |
| 0x80 | 4 | mbr_0x80 | notifyflags | object.h:1255 | high | agent_baseclass.txt: "+0x80 = notifyflags (set by SetNotify, no-arg)" |
| 0x84 | 4 | mbr_0x84 | script (PTScript) | object.h:1307 | high | agent_baseclass.txt: "+0x84 = script (used by Notify)"; SetNotify and Notify decompiles confirm |
| 0x88 | (varies) | mbr_0x88 | lightdef (SLightDef inline) | object.h:1310 | medium | SLightDef is a small struct; SetLightFlags / GetLightDef pattern matches a contiguous block here; not yet field-walked exhaustively |

Notes / fields not yet placed with high confidence: `name` (char*),
`safe_ref_gen` (uint32_t -- post-retail addition; absent from retail
binary), `frame`, `framerate`, `prevstate`, `prevframe`, `cl`, `sector`,
`components`. The retail code never had `safe_ref_gen` or
`std::vector<unique_ptr<TObjectComponent>>` -- it had a single animator
pointer at +0x58, so the source's component vector replaces what was
likely just the animator slot in retail.

**TObjectInstance documented fields: 31** (high: 14, medium: 17).


## TComplexObject -- folded into TCharacter

TComplexObject is not instantiated standalone -- its base-class fields
are part of TCharacter's layout (TCharacter inherits TComplexObject
inherits TObjectInstance). Per agent_baseclass.txt and
agent_actionsmap.txt, only three fields are TComplexObject-specific:

| Offset | Size | Retail token | Source field | Source loc | Confidence | Evidence |
|---|---|---|---|---|---|---|
| 0xd8 | 4 | mbr_0xd8 | doing (PTActionBlock) | complexobj.h:214 | high | TCharacter::AI repeatedly accesses `param_1[0x36]` and dereferences `*(int*)param_1[0x36]` for action id (matches `doing->action`). SetDoing @ 0x4db340 operates on +0xd8. |
| 0xdc | 4 | mbr_0xdc | root (PTActionBlock) | complexobj.h:216 | high | SetRoot @ 0x4db2d0 sets root, sets root->flags |= 0x40, clears doing->interrupt. agent_baseclass.txt: "+0xdc = root". Note: source order is doing/desired/root but retail layout is doing/root/desired (compiler reordering). |
| 0xe0 | 4 | mbr_0xe0 | desired (PTActionBlock) | complexobj.h:215 | high | SetDesired @ 0x4db3a0; agent_baseclass.txt: "+0xe0 = desired". TCharacter::AI uses `param_1[0x38]` (=byte 0xe0) for desired (e.g. `*piVar10 != 3`). |

**TComplexObject documented fields: 3** (all high-confidence).


## TActionBlock -- 100 bytes (retail), 92 bytes (source)

NOT a vtable class. Identified by ctor signatures
(`cls_TActionBlock_Ctor1_4da9f0.cpp`, copy ctor `cls_TActionBlock_CopyCtor_4daae0.cpp`,
clear `cls_TActionBlock_ClearOrCtor_4da180.cpp`). Source: `complexobj.h:60-130`.
Field layout per agent_actionsmap.txt and direct ctor decompile:

| Offset | Size | Retail token | Source field | Source loc | Confidence | Evidence |
|---|---|---|---|---|---|---|
| 0x00 | 4 | mbr_0x0 | action (ACTION enum, int32_t) | complexobj.h:87 | high | Ctor1 sets `*param_1 = 1` (=ACTION_ANIMATE); copy ctor preserves `*param_1 = a` |
| 0x04 | 32 | mbr_0x4 | name[RESNAMELEN] | complexobj.h:88 | high | Ctor1 strcpy's name into `(char*)(param_1 + 1)` (=byte 0x4); RESNAMELEN=32 (revdefs.h:44) |
| 0x24 | 4 | mbr_0x24 | frame | complexobj.h:89 | high | Ctor1: `param_1[9] = 0xffffffff` (= frame=-1). Index 9 = byte 0x24. |
| 0x28 | 4 | mbr_0x28 | wait | complexobj.h:90 | high | Ctor1: `param_1[0xa] = 0` (wait=0). Index 0xa = byte 0x28. |
| 0x2c | 4 | mbr_0x2c | angle | complexobj.h:91 | high | TCharacter::AI sets `*(undefined4*)(param_1[0x36] + 0x2c) = uVar5` after Face to update doing->angle |
| 0x30 | 4 | mbr_0x30 | moveangle | complexobj.h:91 | high | structural; ctor zeros index 0xc |
| 0x34 | 4 | mbr_0x34 | turnrate | complexobj.h:91 | high | Ctor1: `param_1[0xd] = 0x10` (turnrate=16, the source default). Index 0xd = byte 0x34. |
| 0x38 | 4 | mbr_0x38 | target.x | complexobj.h:92 | high | source order; ctor zeros 0xe..0x10 |
| 0x3c | 4 | mbr_0x3c | target.y | complexobj.h:92 | high | as above |
| 0x40 | 4 | mbr_0x40 | target.z | complexobj.h:92 | high | as above |
| 0x44 | 4 | mbr_0x44 | obj (TObjectInstance*) | complexobj.h:93 | high | TPlayer::Pulse reads `*(int*)(param_1[0x38] + 0x44)` to get desired->obj (combat target). Common pattern for `ab->obj`. |
| 0x48 | 4 | mbr_0x48 | attack (PSCharAttackData) | complexobj.h:94 | high | DoAttack @ 0x4d2120 fills attack at +0x48 |
| 0x4c | 4 | mbr_0x4c | impact (PSCharAttackImpact) | complexobj.h:95 | high | DoAttack fills impact at +0x4c |
| 0x50 | 4 | mbr_0x50 | damage | complexobj.h:96 | high | DoAttack fills damage at +0x50 |
| 0x54 | 4 | mbr_0x54 | data (void*) | complexobj.h:97 | medium | structural -- ctor zeros 0x15; dtor `if (data) free(data)` matches |
| 0x58 | 4 | mbr_0x58 | (retail-only padding, no source equivalent) | -- | medium | The 100B retail size vs 92B source = 8 bytes of extra fields. agent_actionsmap.txt notes "?@0x58". Likely an extra TActionBlock field that didn't make the source snapshot. |
| 0x5c | 4 | mbr_0x5c | flags (uint32_t union) | complexobj.h:99-115 | high | TCharacter::AI checks `(*(uint*)(param_1[0x58] + 0x24) & 0x1000000)` for ab->flags-bits via field offset 0x24 of action block ptr at param_1+0x58 (which is itself pointing to a TActionBlock); flags is at offset 0x24+0x38=0x5c from TActionBlock start when accessed differently. SetRoot writes "root->flags |= 0x40"; SetDesired/SetDoing manage bit 0x20 (interrupt) at offset 0x5c. |
| 0x60 | 4 | mbr_0x60 | (retail-only -- firsttime as separate dword?) | -- | medium | Ctor1: `param_1[0x18] = 1` (= firsttime=1). In source, firsttime is bit 0 of the flags union -- but retail seems to keep a separate dword. Probably an artifact: bit 0 of flags at 0x5c is also set by Ctor1's earlier write. Treat 0x60 as the second dword of the flags union. |

**TActionBlock documented fields: 17** (15 high, 2 medium).

**Retail-only / no source equivalent**: `mbr_0x58` (4-byte gap) and
the second flags dword at `mbr_0x60`. The 8-byte size delta vs source
(100 vs 92) suggests two `int32_t` fields were dropped between retail
and the pre-release snapshot.


## TCharacter -- `cls_0x5a7b98`

Inherits TComplexObject -> TObjectInstance. Field offsets here are for
TCharacter's own members, located after the base-class fields at and
beyond ~0xe4 (after desired @ 0xe0).

Source: `src/character.h:430-610` (protected member declaration block).
Cross-references rely on:
- agent_tcharacter.txt offset notes (header comment lines 4-8)
- TCharacter::AI recon (`cls_0x5a7b98_TCharacter_AI_4c8b60.cpp`)
- TCharacter::EffectBurst recon (`cls_TCharacter_EffectBurst_4c85d0.cpp`)
- TPlayer::CalcDamage recon (which inherits TCharacter charflags etc.)

| Offset | Size | Retail token | Source field | Source loc | Confidence | Evidence |
|---|---|---|---|---|---|---|
| 0xb0 | 4 | mbr_0xb0 | (TObjectInstance) moveangle | object.h:1316 | high | TCharacter::Face @ 0x4d7020 sets mbr_0xb0 to face param (per agent_tcharacter.txt offset note); SetMoveAngle inline in object.h sets moveangle |
| 0xb4 | 4 | mbr_0xb4 | (TObjectInstance) movedist | object.h:1317 | medium | structural (next int after moveangle) |
| 0xb8 | 4 | mbr_0xb8 | (TObjectInstance) movevert | object.h:1318 | medium | structural (next int after movedist) |
| 0xbc | 4 | mbr_0xbc | (TObjectInstance) movebits | object.h:1319 | medium | structural (last of move-quad block) |
| 0xfc | 4 | mbr_0xfc | chardata (PSCharData) | character.h:516 | high | TCharacter::AI: `param_1[0x3f] + 0x160` etc. -- chardata ptr at +0xfc. agent_tcharacter.txt header note. Used everywhere as `chardata->X`. |
| 0x100 | 4 | mbr_0x100 | (per-monster state buffer pointer) | (no direct equivalent) | high | TCharacter::AI dtor (FUN_004da180) frees this[0x40] (=0x100); used by AI_PerMonster as `param_1[0xa1]` (=byte 0x284) for monster-type-specific state buffer. The +0x100 here may be a different per-instance allocated block. |
| 0x110 | 4 | mbr_0x110 | charflags | character.h:524 | high | TPlayer::CalcDamage reads `param_1[0x44]` (=byte 0x110) and tests bits 0x200, 0x100, 0x10 -- matches the charflags bitfield (resistance/immunity flags). TCharacter::AI also reads `*(byte*)(param_1+0x44) & 4`. NOT TObjectInstance::flags (that's at 0x8); this is the TCharacter `uint32_t charflags` member. |
| 0x120 | 4 | mbr_0x120 | nextattack | character.h:533 | high | TCharacter::AI: `param_1[0x48]` is decremented per frame and reset to `random(chardata->minattackfreq*0x18/100, maxattackfreq*0x18/100)` when negative -- exactly the source `nextattack` semantics. Index 0x48 = byte 0x120. |
| 0x124 | 4 | mbr_0x124 | waitticks | character.h:519 | high | TCharacter::AI: `param_1[0x49]` decremented alongside nextattack; matches source's adjacent waitticks counter. Index 0x49 = byte 0x124. |
| 0x128 | 4 | mbr_0x128 | (charflags low) -- alt charflags slot | character.h:524 | medium | TCharacter::AI tests `param_1[0x4a] & 1U` and `& 6U`; this could be a second charflags word OR a separate uint32 field. Charflags at 0x110 is the documented one; 0x128 is likely retail-only or a different bit-block. |
| 0x12c | 4 | mbr_0x12c | (ai-state count, "chainhits"?) | character.h:548 | medium | TCharacter::AI: `param_1[0x4b] = param_1[0x4b] + -1` after a successful attack. Decrement-on-attack matches `chainhits` semantics. |
| 0x130 | 4 | mbr_0x130 | glimpse | character.h:534 | high | agent_tcharacter.txt header: "mbr_0x130=glimpse"; ResetStealthValues @ 0x4cdbb0 writes to it. |
| 0x134 | 4 | mbr_0x134 | noise | character.h:535 | high | agent_tcharacter.txt header: "mbr_0x134=noise"; same recon function |
| 0x138 | 4 | mbr_0x138 | lasthealthrecov | character.h:538 | medium | TCharacter::Save (0x4d50d0) writes 3 consecutive ints near here; structural fit for the recovery-timer triple |
| 0x13c | 4 | mbr_0x13c | lastfatiguerecov | character.h:539 | medium | as above |
| 0x140 | 4 | mbr_0x140 | lastmanarecov | character.h:540 | medium | as above |
| 0x160 | 4 | mbr_0x160 | (subset of attack tracking; possibly lastpoisondamage) | character.h:543 | medium | structural; falls in the attack/recovery range that Save touches |
| 0x16c | 4 | mbr_0x16c | lastattack (PSCharAttackData) | character.h:546 | medium | structural fit; TCharacter::ResolveHit recon assigns through this region. Not yet 100% pinned. |
| 0x170 | 4 | mbr_0x170 | lastattackticks | character.h:547 | medium | structural fit (next int after lastattack) |
| 0x194 | 4 | mbr_0x194 | fade | character.h:558 | high | UpdateFade @ 0x4d57a0 decrements mbr_0x194 by mbr_0x198, clamps vs mbr_0x19c. agent_tcharacter.txt header note. |
| 0x198 | 4 | mbr_0x198 | fade_step | character.h:559 | high | as above |
| 0x19c | 4 | mbr_0x19c | fade_limit | character.h:560 | high | as above |
| 0x1a8 | 4 | mbr_0x1a8 | teleport_level | character.h:566 | medium | TCharacter::Save (0x4d50d0) writes mbr_0x1a8 in the teleport block; agent_tcharacter.txt comment cites this region for save/load |
| 0x1ac | 12 | mbr_0x1ac | teleport_position (S3DPoint) | character.h:567 | medium | structural fit (S3DPoint following teleport_level) |
| 0x1b8 | 4 | mbr_0x1b8 | burning (TObjectInstance*) | character.h:570 | high | EffectBurst recon: `if (this->mbr_0x1b8 != 0 && stricmp(type, "blood") == 0) return;` -- the "is character burning?" early-out exactly matches source's `if (burning) ...` |
| 0x1bc | 4 | mbr_0x1bc | (burn-related counter) | character.h:570 (vicinity) | medium | TCharacter::Burn @ 0x4d3590 sets mbr_0x1bc=50 (per agent_tcharacter.txt note). May be an internal tick counter for the burn effect; no direct source equivalent. Likely retail-only. |
| 0x1c0 | 16*8=128 | mbr_0x1c0 | hasseen[MAXHASSEEN] (SHasSeen[8]) | character.h:573 | high | agent_tcharacter.txt: "mbr_0x1c0..1cc = hasseen[8] triple of (chr, time, noautocombat)"; SetHasSeen @ 0x4c5940 writes 8-entry triples here. SHasSeen = {TCharacter*, int32_t, bool} = 12 bytes per entry, MAXHASSEEN=8 = 96 bytes (some padding may push to 128). |
| 0x240 | 4 | mbr_0x240 | snapticks | character.h:576 | medium | structural fit (after hasseen array) |
| 0x244 | 4 | mbr_0x244 | combatflashticks | character.h:579 | medium | structural fit |
| 0x254 | 4 | mbr_0x254 | (AI-cache target_out_of_sight or last_position_count) | character.h:605-606 | medium | TCharacter::AI uses `param_1[0x95]` as a boolean-ish flag set/cleared based on AI sight checks; matches `target_out_of_sight` semantics. Not 100% sure which of {target_out_of_sight, last_position_count, last_position_distance} maps to which slot. |
| 0x258 | 4 | mbr_0x258 | (AI-cache; pairs with above) | character.h:605-607 | medium | TCharacter::AI `param_1[0x96]` set alongside [0x95] |
| 0x25c | 4 | mbr_0x25c | (AI-cache; pairs with above) | character.h:605-607 | medium | TCharacter::AI `param_1[0x97]` decrement counter |
| 0x234 | 12 | mbr_0x234 | target_last_position (S3DPoint) | character.h:603 | medium | TCharacter::AI: `iStack_64 = param_1[0x90]; iStack_68 = param_1[0x8f]; iStack_60 = param_1[0x91]` -- a 3-int (S3DPoint) read; 0x8f/0x90/0x91 = bytes 0x23c/0x240/0x244 ... actually re-checking: index 0x8f = 0x23c, 0x90 = 0x240, 0x91 = 0x244. Tentatively the target_last_position triple. |
| 0x238 | 4 | mbr_0x238 | (last_position_count or last_waypoint instance) | character.h:605 | medium | TCharacter::AI: `param_1[0x8d]` stores a TObjectInstance* (waypoint) or zero -- the AI's last-target-position cache. |
| 0x248 | 4 | mbr_0x248 | (waypoint counter) | character.h (no direct equivalent) | medium | TCharacter::AI: `param_1[0x92]` decremented per frame as waypoint-stick counter. May be retail-only. |
| 0x280 | 4 | mbr_0x280 | (per-monster type id) | (retail-only, no source equivalent) | high | AI_PerMonster: `param_1[0xa0] = 1` for first monster name match, `=2` for Solifuge, etc. Type discriminator for the per-monster state buffer. Source has no equivalent (the per-monster AI was added late). |
| 0x284 | 4 | mbr_0x284 | (per-monster state buffer ptr) | (retail-only) | high | AI_PerMonster: `param_1[0xa1] = malloc(0x58)` for type-1 monsters, etc. Owned per-monster scratch buffer. |

**TCharacter documented fields: 33** (12 high, 21 medium).

**Retail-only fields**: `mbr_0x280` (per-monster type id) and
`mbr_0x284` (per-monster state buffer pointer) -- the late-development
per-monster AI hook documented in `recon/discovered/README.md`'s
"AI_PerMonster" anchor. Source's TCharacter has no equivalent slots;
the pre-release snapshot predates this AI extension.

`mbr_0x1bc` (burn counter, set to 50 in Burn) is also likely a
retail-only or unconfirmed-source field.


## TPlayer -- `cls_0x5b4f30`

Extends TCharacter; only TPlayer-specific offsets documented here.
Source: `src/player.h:64-258` (private member block at the end).

Cross-references:
- agent_tplayer.txt vtable-override notes (slot 16 RemoveItem walks
  equipment[0x2a0..0x2c8])
- TPlayer::SetExperience recon (`cls_0x5b4f30_TPlayer_SetExperience_51d810.cpp`)
- TPlayer::Pulse, Move, Damage recon

| Offset | Size | Retail token | Source field | Source loc | Confidence | Evidence |
|---|---|---|---|---|---|---|
| 0x2a0 | 4 | mbr_0x2a0 | equipment[EQ_HEAD] | player.h:255 + EQ_HEAD=0 | high | agent_tplayer.txt slot-16 RemoveItem: "iterates the 11 equipment slots (0x2a0..0x2c8) clearing the matching pointer". 11 slots * 4 bytes = 0x2c bytes; 0x2a0..0x2cb. |
| 0x2a4 | 4 | mbr_0x2a4 | equipment[EQ_NECK] | player.h:255 + EQ_NECK=1 | high | as above; sequential offsets |
| 0x2a8 | 4 | mbr_0x2a8 | equipment[EQ_BODY] | player.h:255 + EQ_BODY=2 | high | StealthMod recon reads "body (0x2a8)->GetStat('Stealth')" (agent_tplayer.txt slot 744) |
| 0x2ac | 4 | mbr_0x2ac | equipment[EQ_OFFHAND] | player.h:255 + EQ_OFFHAND=3 | high | sequential |
| 0x2b0 | 4 | mbr_0x2b0 | equipment[EQ_PRIMEHAND] | player.h:255 + EQ_PRIMEHAND=4 | high | sequential |
| 0x2b4 | 4 | mbr_0x2b4 | equipment[EQ_R_ACCESSORY] | player.h:255 + EQ_R_ACCESSORY=5 | high | sequential |
| 0x2b8 | 4 | mbr_0x2b8 | equipment[EQ_L_ACCESSORY] | player.h:255 + EQ_L_ACCESSORY=6 | high | GetTorchRoot @ 0x51b3f0 reads "off-hand (field_0x2b8) carries class 6 (Torch)" -- agent_tplayer.txt slot 780 |
| 0x2bc | 4 | mbr_0x2bc | equipment[EQ_RANGEDWEAPON] | player.h:255 + EQ_RANGEDWEAPON=7 | high | sequential |
| 0x2c0 | 4 | mbr_0x2c0 | equipment[EQ_AMMO] | player.h:255 + EQ_AMMO=8 | high | sequential |
| 0x2c4 | 4 | mbr_0x2c4 | equipment[EQ_LEGS] | player.h:255 + EQ_LEGS=9 | high | sequential |
| 0x2c8 | 4 | mbr_0x2c8 | equipment[EQ_FEET] | player.h:255 + EQ_FEET=10 | high | StealthMod recon reads "feet (0x2c8)->GetStat('Stealth')" -- agent_tplayer.txt slot 744 |
| 0x2cc | 4 | mbr_0x2cc | (start of quickspells[5][MAXTALISMANLEN]) | player.h:256 | medium | quickspells follows equipment in source order. MAXTALISMANLEN = SPELLSIZE+1 (spell.h:21). 5 entries * MAXTALISMANLEN bytes. Exact size depends on SPELLSIZE. |
| 0x36c | 4 | mbr_0x36c | (player state bitfield) | (likely retail-only) | medium | SetPlayerState @ 0x51d680: "sets field_0x36c bitfield, stops walk if motion bit changes" -- agent_tplayer.txt. No direct source equivalent (the OnTheHog bool is too narrow). Possibly an extension of charflags. |
| 0x370 | 4 | mbr_0x370 | (Experience cache; OBJSTAT(Exp)) | player.h:176 | high | GetExperience @ 0x51d800: `return *(undefined4*)(param_1 + 0x370);`. SetExperience @ 0x51d810 writes it. The OBJSTAT(Exp) macro generates a virtual; retail caches the value here too. |
| 0x374 | 4 | mbr_0x374 | (cached experience-derived stat) | (retail-only) | high | SetExperience: `*(int*)(param_1 + 0x374) = exp * 24 / 100`. agent_tplayer.txt: "level threshold cache". No source equivalent. |
| 0x300 | 4 | mbr_0x300 | OnTheHog | player.h:257 | medium | GetOnYerHog @ 0x51b420: "flips field_0x300" -- agent_tplayer.txt. The bool OnTheHog is the only player-specific bool toggle that fits. |

**TPlayer documented fields: 16** (14 high, 2 medium).

**Retail-only fields**: `mbr_0x36c` (player state bitfield),
`mbr_0x374` (experience-derived cached stat). Both predate the source
snapshot's stat-array refactor.


## SCharData -- `cls_0x5b9fcc` (struct, no vtable)

Source: `src/rules.h:159-218`. The auto-derived yaml at
`recon/mappings/SCharData_cls_0x5b9fcc_map.yaml` flagged size mismatch
(76 vs 92 bytes) and stopped at +0x48. The actual retail SCharData is
much larger (~0x440+ based on TCharacter::AI accessing
`chardata + 0x440`), and the yaml's auto-mapping is wrong about
`mbr_0x10..0x14` (the source has `name[32]` + `groups[48]` + `enemies[48]`
strings before the int fields, putting `objtype` much later).

Re-derived from source layout + AI / ResolveHit accesses:

| Offset | Size | Retail token | Source field | Source loc | Confidence | Evidence |
|---|---|---|---|---|---|---|
| 0x00 | 32 | mbr_0x0 | name[RESNAMELEN] | rules.h:169 | high | structural -- first field, RESNAMELEN=32 |
| 0x20 | 48 | mbr_0x20 | groups[CHARGROUPLEN] | rules.h:170 | high | structural (CHARGROUPLEN=48 from rules.h:79) |
| 0x50 | 48 | mbr_0x50 | enemies[CHARGROUPLEN] | rules.h:171 | high | structural; 0x20 + 48 = 0x50. agent_tcharacter.txt header: "chardata->enemies at chardata+0x70" -- DOES NOT MATCH our derivation (we get 0x50). The agent's note may have been based on old/different layout, or the source got reordered. Treating source order as canonical here: enemies @ 0x50. |
| 0x80 | 4 | mbr_0x80 | objtype | rules.h:172 | high | follows enemies[48] at 0x50+0x30 = 0x80 |
| 0x84 | 4 | mbr_0x84 | objclass | rules.h:173 | high | next int |
| 0x88 | 4 | mbr_0x88 | flags | rules.h:174 | high | next int |
| 0x8c | 16 | mbr_0x8c | attacks (TCharAttackArray = TVirtualArray) | rules.h:175 | high | TVirtualArray header is ~16 bytes |
| 0x9c | 40 | mbr_0x9c | damagemods[NUMDAMAGETYPES] | rules.h:176 | high | NUMDAMAGETYPES=10 (rules.h:64); 10 * 4 = 40 bytes |
| 0xc4 | 32 | mbr_0xc4 | blocksounds[SOUNDLISTLEN] | rules.h:177 | high | SOUNDLISTLEN=32 |
| 0xe4 | 32 | mbr_0xe4 | misssounds[SOUNDLISTLEN] | rules.h:178 | high | next 32 bytes |
| 0x104 | 4 | mbr_0x104 | playerblockmin | rules.h:179 | medium | source order; structural |
| 0x108 | 4 | mbr_0x108 | playerblockstep | rules.h:180 | medium | as above |
| 0x10c | 4 | mbr_0x10c | playerblockinc | rules.h:181 | medium | as above |
| 0x110 | 4 | mbr_0x110 | combatrangemin | rules.h:182 | high | agent_tcharacter.txt header: "combatrangemin/max at +0x150/+0x154". DERIVED OFFSET DISAGREES with agent (we get 0x110, agent says 0x150). 0x40 byte gap suggests retail has additional fields between misssounds and combatrangemin (e.g. extra damage tables or magic resists). For source-port purposes, the *source field* is combatrangemin; the retail offset is what the agent observed (0x150). Treat retail offset as authoritative for ports. |
| 0x150 | 4 | mbr_0x150 | combatrangemin (retail offset) | rules.h:182 | high | TCharacter agent triangulated: agent_tcharacter.txt header. Direct evidence: AI uses `chardata+0x160` as combat range threshold; combat range fields cluster here. |
| 0x154 | 4 | mbr_0x154 | combatrangemax | rules.h:183 | high | agent_tcharacter.txt header |
| 0x158 | 4 | mbr_0x158 | maxattackrange | rules.h:184 | high | agent_tcharacter.txt header |
| 0x15c | 4 | mbr_0x15c | swipecolor (SColor) | rules.h:185 | medium | structural, follows maxattackrange |
| 0x160 | 32 | mbr_0x160 | bodytype[MAXANIMNAME] | rules.h:186 | medium | MAXANIMNAME=32 (imageres.h:44); follows swipecolor |
| 0x180 | 4 | mbr_0x180 | classdata (PSClassData) | rules.h:187 | medium | TPlayer::MaxHealth uses `classdata->healthmod`; structural fit |
| 0x184..0x19c | 4 each | -- | blockfreq, blockmin, blockmax, sightmin, sightmax, sightrange, sightangle | rules.h:188-194 | medium | source-order int sequence after classdata |
| 0x1a0 | 4 | mbr_0x1a0 | walkspeed | rules.h:208 | high | agent_tcharacter.txt header: "walkspeed at +0x1a0". Used by Move/MoveStep speed lookup. |
| 0x1a4 | 4 | mbr_0x1a4 | runspeed | rules.h:209 | medium | structural (next int) |
| 0x1a8 | 4 | mbr_0x1a8 | sneakspeed | rules.h:210 | medium | structural |
| 0x1ac | 4 | mbr_0x1ac | combatwalkspeed | rules.h:211 | high | agent_tcharacter.txt header: "combatwalkspeed at +0x1ac" |
| 0x1d0 | 4 | mbr_0x1d0 | minattackfreq | rules.h:203 | high | TCharacter::AI: `random(chardata+0x1d0 * 0x18 / 100, chardata+0x1d4 * 0x18 / 100)` for nextattack reset. Index 0x1d0/0x1d4 = minattackfreq/maxattackfreq. |
| 0x1d4 | 4 | mbr_0x1d4 | maxattackfreq | rules.h:204 | high | as above |
| 0x1d8 | 4 | mbr_0x1d8 | (mana or fatigue) | rules.h:205 | medium | TCharacter::AI: `random(chardata+0x1d8 * 0x18 / 100, chardata+0x1dc * 0x18 / 100)` for waitticks reset. The source's mana/fatigue/health follow attackfreq, so 0x1d8 = mana. |
| 0x1dc | 4 | mbr_0x1dc | (mana or fatigue) | rules.h:206 | medium | source order: mana, fatigue. 0x1dc = fatigue. |

**Note on the 0x40-byte gap discrepancy:** the source-derived layout
(strings + ints) reaches `combatrangemin` at +0x110, but agent
observation puts it at +0x150. This means retail SCharData has ~64
bytes of additional fields between `misssounds` and the
playerblock/combatrange block. Possible candidates: per-damage-type
resistance arrays, magic-attack metadata. Pinning these requires
explicit chardata accessor decompiles -- not done in this pass.

**SCharData documented fields: 27** (16 high, 11 medium).


## Per-class summary

| Class | Total fields | High-conf | Medium-conf | Retail-only / no source eq. |
|---|---|---|---|---|
| TObjectInstance | 31 | 14 | 17 | 0 |
| TComplexObject | 3 | 3 | 0 | 0 |
| TActionBlock | 17 | 15 | 2 | 2 (mbr_0x58 padding, mbr_0x60 second flags dword) |
| TCharacter | 33 | 12 | 21 | 3 (mbr_0x280, mbr_0x284 per-monster AI; mbr_0x1bc burn counter) |
| TPlayer | 16 | 14 | 2 | 2 (mbr_0x36c player-state bitfield, mbr_0x374 exp-derived) |
| SCharData | 27 | 16 | 11 | ~16 bytes between 0x110 (source-end of strings) and 0x150 (retail combatrangemin) -- retail-only fields not yet identified |
| **Total** | **127** | **74** | **53** | **7 retail-only fields + ~16 bytes unidentified retail SCharData fields** |

## How to use this map

When you decompile a function and see e.g. `*(int*)(this + 0xfc)`, look
up `mbr_0xfc` in the relevant class table here:

- TCharacter member -> `chardata` (PSCharData)
- TObjectInstance member -> follow the SObjectDef + protected-block layout

If the offset is in a base-class range (< 0xe4 for TCharacter) consult
the parent class's table first.

When porting a method, prefer rewriting accesses as named field
references (`this->chardata->minattackfreq`) rather than preserving
the `mbr_0x1d0` bytecode-style names. The map's job is to make that
rewrite mechanical.
