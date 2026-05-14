// REVSYNC: TExit::WriteExitList @ 0x0050cca0 (retail)
// Source: src/exit.cpp:116 — bool TExit::WriteExitList()
//
// Globals:
//   DAT_0066d24c_TExit_exitlistdirty = exitlistdirty (bool)
//   DAT_0066d1c4_TExit_exitlist = exitlist
//   DAT_0065a784 = currentmod-index
//   DAT_0065a77c = modtable[]
//   DAT_0065d6a4 = save root path (was ClassDefPath in source)
//   DAT_0065bd48 = currentmod name buffer
//
// Behaviour matches source: skips early if !dirty; calls
// ReadExitList(reload=true) (FUN_0050c8f0_TExit_ReadExitList) to merge any disk-side adds;
// opens "<root>/<mod>/exit.def" or "<root>/<mod_path>/exit.def" depending
// on whether the active module ID is in range; then writes one line per
// SExitRef with format "%s (%d, %d, %d) level %d mapindex 0x%x ambient %d (%d, %d, %d)\r\n".
//
// The format string matches source character-for-character (literal at
// 0x005e1788). No retail-only divergence.

undefined4 TExit_WriteExitList(void)
{
    SExitRef* p;
    char path[260], modpath[260];
    void* fp;

    if (!exitlistdirty)
        return 1;

    if (!TExit_ReadExitList(/*reload=*/1))   // FUN_0050c8f0_TExit_ReadExitList
        return 0;

    // Resolve "<saveroot>/<mod>/exit.def" or "<saveroot>/exit.def"
    int modIdx = (DAT_0065a784 >= 0) ? *(int*)(DAT_0065a77c + DAT_0065a784*4) : 0;
    snprintf(modpath, sizeof modpath, "%s%s%s", saveroot, currentmodname, "exit.def");
    if (modIdx) {
        modIdx = (DAT_0065a784 >= 0) ? *(int*)(DAT_0065a77c + DAT_0065a784*4) : 0;
        snprintf(path, sizeof path, "%s%s\\%s", saveroot, *(char**)(modIdx + 0x58), "exit.def");
        if (FileExists(path))                 // FUN_00481260
            strcpy(modpath, path);
    }

    fp = TryOpen(modpath, "wb");              // FUN_004457c0
    if (!fp) return 0;

    for (p = exitlist; p != nullptr; p = p->next) {
        if (!fprintf(fp,
                "%s (%d, %d, %d) level %d mapindex 0x%x ambient %d (%d, %d, %d)\r\n",
                p->name, p->target.x, p->target.y, p->target.z,
                p->level, p->mapindex,
                p->ambient,
                p->ambcolor.b, p->ambcolor.g, p->ambcolor.r)) {
            fclose(fp);
            return 0;
        }
    }
    fclose(fp);                                // FUN_0058b4f1
    exitlistdirty = 0;
    return 1;
}
