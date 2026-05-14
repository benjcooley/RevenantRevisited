// REVSYNC: TExit::Unactivate @ 0x0050d510 (retail)
// Source: src/exit.cpp:370 — void TExit::Unactivate()
//
// vtable slots:
//   0x1f0 = Openable()
//
// Calls SetExitState(2) (CLOSING_OUT) when Openable() returns nonzero.
// Source calls SetExitState(EXIT_CLOSING). Numeric difference because retail
// reordered the state enum (see SetExitState 50d530 notes).

void TExit_Unactivate(TExit* this_)
{
    if (this_->Openable())                  // vt[0x1f0]
        this_->SetExitState(2);             // CLOSING_OUT (was EXIT_CLOSING in source)
}
