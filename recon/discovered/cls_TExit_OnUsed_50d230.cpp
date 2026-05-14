// REVSYNC: TExit::OnUsed (direction-aware door toggle) @ 0x0050d230 (retail)
//
// This is a RETAIL-ONLY helper. There is no equivalent in src/exit.cpp 1998.
// In retail, TExit::Use does not change state itself — instead, the script
// system invokes this helper as the "user" event handler, and it picks
// open-OUT vs open-IN (or close-OUT vs close-IN) based on which side of
// the door the user is standing on.
//
// State id mapping (matches retail SetExitState at 0x0050d530):
//   0 = OPENING_OUT  ("openingout"  / fallback "closed to open")
//   1 = OPEN         ("open")
//   2 = CLOSING_OUT  ("closingout"  / fallback "open to closed")
//   3 = CLOSED       ("closed")
//   4 = OPENING_IN   ("openingin"   / fallback "closed to open")
//   5 = CLOSING_IN   ("closingin"   / fallback "open to closed")
//
// Source's enum is { EXIT_CLOSED=0, EXIT_OPEN=1, EXIT_CLOSING=2, EXIT_OPENING=3 }
// — different ordering AND no IN/OUT split. Retail added states 4 and 5 for
// the "swing toward viewer" half of the animation set.

void TExit_OnUsed(TExit* this_, TObjectInstance* user)
{
    int state = this_->state;   // *(short*)(this+0xc)

    if (state == 1 || state == 0) {
        // Currently open or opening — close it
        if (UserOnFrontSide(this_, user))    // FUN_0050d2b0_TExit_UserOnFrontSide — dot-product against face vector
            this_->SetExitState(2);          // CLOSING_OUT
        else
            this_->SetExitState(5);          // CLOSING_IN
    } else if (state == 3 || state == 2) {
        // Currently closed or closing — open it
        if (UserOnFrontSide(this_, user))
            this_->SetExitState(0);          // OPENING_OUT (closed -> open, away from user)
        else
            this_->SetExitState(4);          // OPENING_IN  (closed -> open, toward user)
    }
}

// Helper at 0x0050d2b0 — returns 1 if user is on the front (facing) side of the exit.
// Calls vt[0x254] = GetFacingVector(distance=10, out_point, 0) twice:
//   once with face (forward) and once with face+0x7f (~180 degrees).
// Returns whichever side is closer to user's pos.
int UserOnFrontSide(TExit* this_, TObjectInstance* user)
{
    if (!user) return 0;
    S3DPoint forward, backward, delta;
    this_->GetFacingVector(10, &forward,  0);                 // raw face byte
    this_->GetFacingVector(10, &backward, /*+= 0x7f*/0);      // opposite
    delta = user->pos - this_->pos;
    int dForward  = SignedDot(&forward,  &delta);  // FUN_0046de60_Distance2D
    int dBackward = SignedDot(&backward, &delta);
    return (dBackward < dForward) ? 1 : 0;
}
