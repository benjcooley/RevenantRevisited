// REVSYNC: TExit::Pulse @ 0x0050d640 (retail)
// Source: src/exit.cpp:403 — void TExit::Pulse()
//
// vtable slots used:
//   0x024 = GetImagery()           — returns TObjectImagery*; +8 in imagery is anim-table base
//   0x040 = SetFlags(uint32_t)     — TObjectInstance flag write
//   0x088 = imagery::GetStateName(short stateIdx)  — returns char*
//   0x090 = imagery::GetStateLength(short stateIdx) — returns frame count
//   0x0c4 = imagery::Pulse(short)
//   0x154 = CommandDone()
//   0x158 = Pulse(int)             — TContainer::Pulse fallback
//   0x1f0 = Openable()
//   0x248 = Activate(...)          — virtual (TPressPlate / TSpikeWall override)
//   0x24c = Unactivate()           — virtual
//   0x26c = Delay()                — STATFUNC slot
//   0x27c = GetExitStrip(rx,ry,rz, w,l,h)
//
// Globals:
//   DAT_00668154 = "edit mode" / Editor (1 = editor)
//   DAT_00667fcc = Player
//   FUN_0051ee70_TPlayerManager_NumPlayers = TMapPane::CharacterCount(int)
//   FUN_0051eea0_TPlayerManager_GetPlayer = TMapPane::GetCharacter(idx, ?)
//   FUN_004cdf30_TCharacter_SetOnExit = TCharacter::SetOnExit (sets OF_ONEXIT bit 0x100000 + exittimestamp)
//
// Major retail diffs from source TExit::Pulse:
//
//   1) Retail does NOT call TContainer::Pulse() up-front (only in the
//      "no imagery" tail at the end). The TContainer pulsing is folded
//      into a deferred call.
//
//   2) Retail does NOT auto-step state from CLOSING -> CLOSED or
//      OPENING -> OPEN by reading the source-style enum compare. Instead
//      it READS THE CURRENT ANIMATION STATE NAME via imagery vt[0x88]
//      and string-compares against "CLOSING" / "OPENING". This means
//      the trigger-edge for "transitions complete" is now driven by the
//      animation track's labelled segments, not by enum bookkeeping.
//      States with name "CLOSING" → set state 3 (CLOSED).
//      States with name "OPENING" → set state 1 (OPEN).
//
//   3) Auto-activation while-on-strip is GATED to objtypes
//      "Door1", "Door2", "PortEW", "PortNS" only. In source, ANY exit
//      auto-activated when the player walked over its strip. In retail,
//      every other exit (Halfdoor, CryptDoor, Elevator, Lever, …) requires
//      explicit Use() / script invocation.
//
//   4) Retail iterates ALL CHARACTERS on the map (not just Player) for
//      strip-overlap detection. So NPCs can also activate doors. (See the
//      iteration over FUN_0051ee70_TPlayerManager_NumPlayers in the disassembly.)
//
//   5) Retail's "just-teleported" anti-bounce now uses TWO bits, not one:
//        flags = exitflags & ~5;   then conditionally |= 1 (EX_ON-equivalent)
//        and |= 4 if the character had OF_ONEXIT (0x100000) set on entry.
//      Bit 0 = "char on strip", bit 2 = "from another exit".
//      Source uses (EX_ON | EX_ACTIVATED | EX_FROMEXIT) at bits 0/1/2.
//      The numeric layout is *similar* but not byte-identical.

void TExit_Pulse(TExit* this_)
{
    int regx, regy, regz, w, l, h;
    int dx, dy;
    char* sname;
    int snlen;
    TObjectImagery* im;

    SomeFrameTick();    // FUN_004708e0_OnScreen

    if (Editor != 0) {
        // Editor mode — only fall through to TContainer::Pulse via vt[0x158]
        if (this_->GetImagery() == nullptr)
            this_->ContainerPulse(1);
        return;
    }

    // Auto step-state via animation-track name match
    if (this_->CommandDone() && this_->Openable() && this_->GetImagery()
        && this_->GetImagery()->animTable != nullptr) {

        snlen = im->GetStateLength((short)this_->state);
        if (snlen > 1) {
            sname = im->GetStateName((short)this_->state);
            if (stricmp(sname, "CLOSING") == 0)
                this_->SetExitState(3);   // -> CLOSED
            if (stricmp(sname, "OPENING") == 0)
                this_->SetExitState(1);   // -> OPEN
        }
    }

    // Per-character strip overlap → activate
    if (Editor == 0 && (im = this_->GetImagery()) != nullptr
        && (DAT_0067682c != 0 || DAT_0066829c == 0)) {

        this_->GetExitStrip(regx, regy, regz, w, l, h);
        this_->exitflags &= ~5u;     // clear bit0 (on-strip) and bit2 (from-exit)

        for (int ci = 0; ci < TMapPane_CharacterCount(0); ++ci) {
            TCharacter* ch = TMapPane_GetCharacter(ci, 0);
            if (!ch) continue;

            int gx = (regx*16 - this_->pos.x + ch->pos.x) >> 4;
            int gy = (regy*16 + (ch->pos.y - this_->pos.y)) >> 4;

            im->Pulse((short)this_->state);  // refresh imagery state once

            if (gx < 0 || gy < 0 || gx >= w || gy >= l) continue;

            // Auto-activation only for Door1 / Door2 / PortEW / PortNS objtypes
            const char* tn = this_->ObjType();
            if (stricmp(tn, "Door1")  != 0 &&
                stricmp(tn, "Door2")  != 0 &&
                stricmp(tn, "PortEW") != 0 &&
                stricmp(tn, "PortNS") != 0)
                continue;

            uint32_t fl = this_->exitflags;
            this_->exitflags = fl | 1;       // mark on-strip
            if ((ch->flags & 0x100000u) != 0)
                this_->exitflags = fl | 5;   // also from-exit
            else
                this_->Activate(ch, 0);      // vt[0x248]

            TCharacter_SetOnExit(ch);        // FUN_004cdf30_TCharacter_SetOnExit
            ch->onexit = this_;              // ch[0xe4] = this
        }
    }

    if (this_->GetImagery() == nullptr)
        this_->ContainerPulse(1);            // vt[0x158]
}
