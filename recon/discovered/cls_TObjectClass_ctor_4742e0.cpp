// REVSYNC: TObjectClass::TObjectClass(char *classname, int32_t classid, uint16_t flags, TObjectClass* base)
// FUN_004742e0_TObjectClassCtor @ 004742e0 size=415
//
// Confirmed via per-class registration sites: each TXxxClass static is
// constructed with this->ECX, then PUSH(name, id, flags, base):
//
//   CharacterClass: ECX=0x66ca68  PUSH (0x5df578="CHARACTER", 0xc=12, 0,    0)
//   PlayerClass:    ECX=0x66d910  PUSH (0x5e21d4="PLAYER",    0xb=11, 0,    0x66ca68=&CharacterClass)
//   ContainerClass: ECX=0x66cb28  PUSH (0x5e0794="CONTAINER", 0x5,    0,    0)
//   InvContainerClass:ECX=0x66cb90 PUSH(0x5e07a0="INVCONTAINER",0x11=17,0, 0x66cb28=&ContainerClass)
//   FoodClass:      ECX=0x66d2a8  PUSH (0x5e18dc="FOOD",      0x4,    0,    0)
//   PotionClass:    ECX=0x66d268  PUSH (0x5e18e4="POTION",    0x12=18,0,   0x66d2a8=&FoodClass)
//   ... (full table in agent_inventory.txt)
//
// Body assigns:
//   this[1] = name; this[2] = id; *(short*)(this+0xe) = flags; this[4] = base
//   *this = &PTR_LAB_005a52d8        ; vptr (TObjectClass vtable)
//   (&DAT_0065a148_TObjectClass_classes)[id] = this        ; classes[id] = this
//   if (numclasses_065a258 <= id) numclasses_065a258 = id + 1
//   Clear() inlined below: stat tables zeroed.
//
// Matches src/object.cpp:2707 TObjectClass::TObjectClass.
