// REVSYNC: TCharacter::EffectBurst(const char* type, int32_t height_offset)
// FUN_004c85d0_EffectBurst @ 004c85d0 size=800
//
// String-anchored on "blood" / "Blood" / "sparks" — the canonical pre-release
// EffectBurst dispatch.
//
//   if (this->mbr_0x1b8 != 0 && stricmp(type, "blood") == 0) return;  // burning
//   SObjectDef def = {0};
//   def.objclass = OBJCLASS_EFFECT;        // 0x19 = 25
//   def.objtype  = FindObjType(type, 0);   // FUN_00475210
//   def.uid      = DAT_00666970;           // last-uid global
//
//   if (stricmp(type, "Blood") == 0) {
//       // ... pos = this+0x10..0x18 with z + height_offset
//       // MapPane.NewObject(&def, -1) → instance
//       // calls inst->vtable[+0x200] with rotation/face params
//       (face=this->mbr_0x36 - 0x80, 3, 5, 5, rand(1,5))
//   } else if (stricmp(type, "sparks") == 0) {
//       // pos = this+0x10..0x18; spawn the effect; sets up particle params at
//       // 0x40400000 (3.0f), 0x3f000000 (0.5f), etc.
//       // counts = 8/0x14/0x28, kind = 2.
//   }
//
// FUN_00475210 = TObjectClass::FindObjType (string→type id), called via
//   the EFFECT class global at (&DAT_0065a148_TObjectClass_classes)[OBJCLASS_EFFECT]
// FUN_00450e40_TMapPane_NewObject = MapPane.NewObject
// FUN_00452690_TMapPane_GetInstance = MapPane.GetInstance
// FUN_00483300_RandomRange = TRandom range helper
//
// (existing rename in agent_tcharacter.txt labels nearby 0x4c8500 as
// EffectCombatFlash; keep that. This is the canonical EffectBurst per the
// TCharacter source.)
