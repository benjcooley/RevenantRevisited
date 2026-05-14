// REVSYNC: TObjectClass::NewObject(SObjectDef* def)
// FUN_00474bb0_NewObject @ 00474bb0 size=326
//
// Confirmed by structural match against src/object.cpp:2780:
//   if (def->pos.x < 0) def->pos.x = 0;       // (param_2+0xc, +0x10, +0x14)
//   if (def->objtype >= numtypes(this+0x24)) return 0;
//   info = objinfo[def->objtype]; if (!info) info = objinfo_fallback;
//   if (!info->objbuilder) return 0;
//   if (img < 0) img = info->imageryid; img = LoadImagery(img);
//   inst = info->objbuilder->Build(def, img);     // virtual via objbuilder vtable[0]
//   if (inst->mapindex < 1) inst->mapindex = MapPane.MakeIndex();
//   return inst;
//
// Field offsets discovered:
//   this+0x24 = numtypes
//   this+0x34 = objinfo array (TVirtualArray<SObjectInfo*>)
//   this+0x38 = objinfo fallback / "default" entry pointer
//   inst+0x40 = mapindex
