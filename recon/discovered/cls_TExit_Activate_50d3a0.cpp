// REVSYNC: TExit::Activate @ 0x0050d3a0 (retail)
// Source: src/exit.cpp:342 — bool TExit::Activate()
//
// vtable slots:
//   0x208 = Locked()       (TExit OBJSTATFUNC slot — see TExit::Use)
//   0x0dc = ?              (returns 1 normally; 0 gates non-forced activation)
//                          probably IsActive() or "exitflags & EX_FROMEXIT not set"
//
// Globals:
//   DAT_0066d1c4_TExit_exitlist = exitlist (head)
//   DAT_0066d1cc = (something stashed during script-trigger setup)
//   DAT_00667fcc = Player
//   DAT_00667fd0 = CurrentScreen
//   DAT_0066829c, DAT_0067682c = multiplayer-mode / is-server flags
//   DAT_006669b0/4 = some user/local-player flag
//
// Major retail diffs from source:
//
//   1. Source calls GetScript()->Trigger(TRIGGER_ACTIVATE) (an enum trigger).
//      Retail calls FUN_00492640(6, 0, 0, user, "user", 0) — a STRING-named
//      script event. The "user" string at 0x005e17d8 confirms the script
//      event-name is literally "user".
//
//   2. Source's "if (stricmp(GetTypeName(),"Door")==0 && Player) targ += vect"
//      hack to push the player past the door is GONE in retail. Retail
//      relies on the door's exit-strip being adjacent to the destination.
//
//   3. Retail adds a MULTIPLAYER guard: if (multiplayer && !is_server) bail
//      early. After the local SetPos, a server-broadcast call FUN_00586bb0
//      is issued so other clients update.
//
//   4. Retail checks Locked() up-front and refuses to activate locked
//      exits — source does not (source assumes Pulse won't reach Activate
//      while locked, which is wrong).
//
//   5. Source's `if (exitflags & EX_FROMEXIT) return false` early-out is
//      represented in retail by the vtable[0xdc] check (likely returns
//      0 when EX_FROMEXIT is set or when activation is otherwise gated).

undefined4 TExit_Activate(TExit* this_, TObjectInstance* user, int forced)
{
    SExitRef* ref;
    bool gated;

    // Retail-only multiplayer client bail
    if (DAT_0066829c != 0 && DAT_0067682c == 0)
        return 0;

    // user defaults to the global Player
    if (user == nullptr)
        user = Player;                     // DAT_00667fcc

    // Retail-only: refuse if locked
    if (this_->Locked())                   // vt[0x208]
        return 0;

    // exitflags & EX_FROMEXIT (or similar) — only enforced when !forced
    if (this_->exitflags_eq != 0 && forced == 0) {
        // Trigger script event "user" (param at 0x005e17d8)
        ScriptTrigger(6, 0, 0, user, "user", 0);   // FUN_00492640
        return 1;
    }

    gated = this_->vt0xdc();
    if (!gated && forced == 0)
        return 0;

    // Search exitlist for an entry whose name matches this->objtype name
    const char* myname = this_->ObjType();   // param_1[0xe]
    for (ref = exitlist; ref != nullptr; ref = ref->next) {
        if (stricmp(ref->name, myname) == 0)
            break;
    }
    if (!ref) {
        return (this_->exitflags_eq != 0) ? 1 : 0;
    }

    // Set the source player's onexit / clear local-player ack flag
    if (user == GlobalLocalPlayerRef())     // (DAT_006669b0 & 1) ? 0 : DAT_006669b4
        DAT_006669b0 |= 8;

    // Apply teleport. SetPos is virtual at slot 8 (TObjectInstance::SetPos
    // → eventually MapPane.CheckPos → cross-sector / cross-level transfer).
    SomeWalkmapHook(this_);                 // FUN_004d4790_SetFighting(0)
    user->SetPos(ref->target, ref->level, /*override=*/0);   // vt[8]

    // Multiplayer broadcast
    if (DAT_0066829c != 0 && DAT_0067682c != 0)
        NetSendTeleport(user, &iStack_1c, ref->level);  // FUN_00586bb0

    return 1;
}
