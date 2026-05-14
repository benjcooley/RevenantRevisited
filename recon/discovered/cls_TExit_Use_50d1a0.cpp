// REVSYNC: TExit::Use @ 0x0050d1a0 (retail)
// Source: src/exit.cpp:303 — bool TExit::Use(TObjectInstance* user, int32_t with)
//
// vtable slot offsets:
//   0x1f0 = Openable()  (TExit STATFUNC slot)
//   0x208 = Locked()    (TExit OBJSTATFUNC slot)
//
// Major retail diff from source:
//
//   * Source TExit::Use TOGGLES the exit state itself:
//         if (Openable()) {
//             if (CheckKeyUse(user, MapPane.GetInstance(with))) return true;
//             if (Locked()) { TextBar.Print("It seems to be locked."); return false; }
//             if (state == EXIT_OPEN || state == EXIT_OPENING)
//                 SetExitState(EXIT_CLOSING);
//             else
//                 SetExitState(EXIT_OPENING);
//             return true;
//         }
//
//   * Retail TExit::Use does NOT toggle here. It calls TObjectInstance::Use
//     (FUN_004705f0), which routes through the script system. The actual
//     direction-aware open/close is done by TExit::OnUsed (FUN_0050d230_TExit_OnUsed)
//     — which is dispatched as a *script "user" event*, not from Use directly.
//   * Retail uses a localized "DOORLOCKED" string-resource (FUN_0049d800 +
//     id "DOORLOCKED" -> resolves to "It seems to be locked." in en-US),
//     not a hard-coded TextBar literal. Other locales' resource files supply
//     the translation.
//   * Retail only prints the DOORLOCKED message when the user IS the player
//     (DAT_00667fcc) — NPCs trying to use a locked door silently fail.

undefined4 TExit_Use(TExit* this_, TObjectInstance* user, int with)
{
    int locked;
    TObjectInstance* withInst;

    if (!this_->Openable())                          // vt[0x1f0]
        return 0;

    withInst = MapPane.GetInstance(with);            // FUN_00452690_TMapPane_GetInstance(with, 0)

    if (CheckKeyUse(user, withInst) == 0 &&          // FUN_004dd480_UseContainer (TContainer::CheckKeyUse)
        this_->Locked()) {                           // vt[0x208]
        if (user != Player)                          // DAT_00667fcc
            return 0;
        const char* msg = LocalizeString("DOORLOCKED");  // FUN_0049d800
        TextBar.Print(msg);                          // FUN_0054d170
        return 0;
    }

    TObjectInstance::Use(user, with);                // FUN_004705f0 — runs script
    return 1;
}
