// REVSYNC: TExit::AddExit @ 0x0050cfc0 (retail)
// Source: src/exit.cpp:226 — bool TExit::AddExit(char* name, TObjectInstance* inst, bool getamb)
// Confirmation: matches source line-by-line; uses exitlist (DAT_0066d1c4_TExit_exitlist) and exitlistdirty
// (DAT_0066d24c_TExit_exitlistdirty). MapPane.GetMapLevel() inlined as DAT_00666970. Ambient color/level read
// from globals when getamb=true.
//
// SExitRef layout in retail (32 bytes; matches source 1:1):
//   0x00 char*   name
//   0x04 int32   target.x
//   0x08 int32   target.y
//   0x0c int32   target.z
//   0x10 int32   level
//   0x14 int32   mapindex
//   0x18 int32   ambient
//   0x1c byte[3] ambcolor (rgb)
//   0x20 SExitRef* next
//
// FUN_0050d24c is the retail address-equivalent of `exitlist`/`exitlistdirty`:
//   DAT_0066d1c4_TExit_exitlist = exitlist (head pointer)
//   DAT_0066d24c_TExit_exitlistdirty = exitlistdirty (bool)
//
// Behaviour matches source identically. No retail-only divergence in AddExit.

undefined4 FUN_0050cfc0_TExit_AddExit(char *name, TObjectInstance* inst, int getamb)
{
    SExitRef *ref;
    int dummy_w, dummy_h, dummy_l, regx, regy;
    int regz_unused;
    char acStack_strip[16];

    if (!name || !*name || !inst)
        return 0;

    // Find existing entry (linear scan over exitlist)
    ref = exitlist;
    while (ref != nullptr) {
        if (stricmp(ref->name, name) == 0)
            goto fill;
        ref = ref->next;
    }

    // not found — allocate a new node and prepend
    ref = (SExitRef*)malloc(0x24);
    ref->name = strdup(name);
    ref->next = exitlist;
    exitlist = ref;

fill:
    if (inst->ObjClass() == OBJCLASS_EXIT) {
        if (inst->GetImagery() == nullptr) {
            ref->target.x = ref->target.y = ref->target.z = 0;
        } else {
            // GetExitStrip via vtable[0x27c]
            inst->GetExitStrip(regx, regy, regz_unused, dummy_w, dummy_l, dummy_h);
            ref->target.x = (dummy_w * 16 - regx * 32) / 2;  // center of strip
            ref->target.y = (dummy_l * 16 - regy * 32) / 2;
            ref->target.z = 0;
        }
        ref->target.x += inst->pos.x;
        ref->target.y += inst->pos.y;
        ref->target.z += inst->pos.z;
        ref->mapindex = inst->mapindex;
        if (getamb) {
            ref->ambient = MapPane.ambient;     // DAT_006671a4
            GetAmbientColor(&ref->ambcolor);    // FUN_0041d7d0
        } else {
            ref->ambient = -1;
            ref->ambcolor.r = ref->ambcolor.g = ref->ambcolor.b = 0xff;
        }
    } else {
        // non-EXIT instance: just copy pos, no strip math
        ref->target.x = inst->pos.x;
        ref->target.y = inst->pos.y;
        ref->target.z = inst->pos.z;
        ref->mapindex = -1;
    }

    ref->level = MapPane.GetMapLevel();   // DAT_00666970
    exitlistdirty = 1;
    return 1;
}
