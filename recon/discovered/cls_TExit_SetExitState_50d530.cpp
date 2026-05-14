// REVSYNC: TExit::SetExitState @ 0x0050d530 (retail)
// Source: src/exit.cpp:376 — bool TExit::SetExitState(int32_t es)
//
// vtable slots:
//   0x18  = SetState(int)              — generic TObjectInstance state setter
//   0x138 = FindState(const char*, ?)  — returns animation-state index by name
//
// MAJOR retail diff:  Retail has SIX exit states, not four.
// Source enum:
//   EXIT_CLOSED  = 0  ("closed")
//   EXIT_OPEN    = 1  ("open")
//   EXIT_CLOSING = 2  ("closing"   / fallback "open to closed")
//   EXIT_OPENING = 3  ("opening"   / fallback "closed to open")
//
// Retail enum (inferred from this dispatch):
//   STATE_OPENING_OUT = 0  ("openingout"  / fallback "closed to open")
//   STATE_OPEN        = 1  ("open")
//   STATE_CLOSING_OUT = 2  ("closingout"  / fallback "open to closed")
//   STATE_CLOSED      = 3  ("closed")
//   STATE_OPENING_IN  = 4  ("openingin"   / fallback "closed to open")
//   STATE_CLOSING_IN  = 5  ("closingin"   / fallback "open to closed")
//
// Retail added states 4 and 5 so that doors can swing toward (IN) the user
// or away (OUT) based on which side the user is on. The "openingin"/"closingin"
// animations are typically the same skeleton as the OUT ones, just mirrored.
// The retail Class.Def files for Door1/Door2 supply both animation tracks.
// If the IN-side animation is missing, the dispatcher falls back to the
// "closed to open" / "open to closed" name, which any door has.

void TExit_SetExitState(TExit* this_, int es)
{
    int st;

    if (es == 1) {                                     // STATE_OPEN
        st = this_->FindState("open", -1);
    } else if (es == 3) {                              // STATE_CLOSED
        st = this_->FindState("closed", -1);
    } else if (es == 0) {                              // STATE_OPENING_OUT
        st = this_->FindState("openingout", -1);
        if (st < 0)
            st = this_->FindState("closed to open", -1);
    } else if (es == 2) {                              // STATE_CLOSING_OUT
        st = this_->FindState("closingout", -1);
        if (st < 0)
            st = this_->FindState("open to closed", -1);
    } else if (es == 4) {                              // STATE_OPENING_IN
        st = this_->FindState("openingin", -1);
        if (st < 0)
            st = this_->FindState("closed to open", -1);
    } else if (es == 5) {                              // STATE_CLOSING_IN
        st = this_->FindState("closingin", -1);
        if (st < 0)
            st = this_->FindState("open to closed", -1);
    } else {
        st = es;   // unknown literal — pass through to SetState
    }

    if (st < 0)
        st = es;

    this_->SetState(st);    // vt[0x18]
}
