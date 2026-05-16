// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *    uitextbartest.cpp - --test=ui-textbar: TTextBar data API           *
// *************************************************************************
//
// See uitextbartest.h.
//
// *************************************************************************

#include "uitextbartest.h"

#include "display.h"
#include "logging.h"
#include "textbar.h"
#include "time.h"

#include <cmath>
#include <cstring>
#include <memory>

namespace {

std::unique_ptr<TTextBar> g_textbar;

// Reach in via a friend-free shim: the protected members of TTextBar are
// not exposed publicly. For verification we infer state from the public
// observable side -- IsDirty, rect, and "has health display" via the
// behavior of ClearHealthDisplay (which clears dirty only if name was
// non-empty). Logs the observable surface; the protected text / name
// fields stay opaque.
void LogObservable(const char* label)
{
    if (!g_textbar) { log_info("[ui-textbar] %s (null pane)", label); return; }
    log_info("[ui-textbar] %-30s rect=(x=%d y=%d w=%d h=%d) dirty=%s open=%s",
             label,
             g_textbar->GetPosX(), g_textbar->GetPosY(),
             g_textbar->GetWidth(), g_textbar->GetHeight(),
             g_textbar->IsDirty() ? "YES" : "no",
             g_textbar->IsOpen()  ? "YES" : "no");
}

}  // namespace

bool InitializeUITextBarMode()
{
    log_info("[ui-textbar] === TTextBar data-API contract ===");

    g_textbar = std::make_unique<TTextBar>();
    LogObservable("after construct");

    g_textbar->Initialize();
    LogObservable("after Initialize()");

    // Print a message. text[] becomes "hello bar", name[] cleared, dirty=true.
    char msg[] = "hello bar";
    g_textbar->Print(msg);
    LogObservable("after Print(\"hello bar\")");

    // Reading a (printf-style) format
    char fmt[] = "level %d of %s";
    char arg2[] = "the keep";
    g_textbar->Print(fmt, 3, arg2);
    LogObservable("after Print(\"level %d of %s\", 3, \"the keep\")");

    // Health display path: SetHealthDisplay sets name[] + level fields.
    g_textbar->SetHealthDisplay("Skeleton", 75);
    LogObservable("after SetHealthDisplay(\"Skeleton\", 75)");

    // Updating the same name just retargets level; dirty NOT bumped again
    // (per textbar.cpp: only the name-change branch sets dirty).
    g_textbar->SetDirty(false);   // reset so we can observe the contract
    g_textbar->SetHealthDisplay("Skeleton", 50);
    LogObservable("after SetHealthDisplay(\"Skeleton\", 50) (same name)");

    // New name -> dirty
    g_textbar->SetDirty(false);   // reset
    g_textbar->SetHealthDisplay("Orc", 90);
    LogObservable("after SetHealthDisplay(\"Orc\", 90) (new name)");

    // ClearHealthDisplay should set dirty (since name was non-empty)
    g_textbar->ClearHealthDisplay();
    LogObservable("after ClearHealthDisplay()");

    // ClearHealthDisplay when already clear should be a no-op (no dirty bump)
    g_textbar->SetDirty(false);
    g_textbar->ClearHealthDisplay();
    LogObservable("after ClearHealthDisplay() (already clear)");

    g_textbar->Clear();
    LogObservable("after Clear()");

    log_info("[ui-textbar] (DrawBackground / visual render is a follow-up "
             "commit -- needs GameData->Bitmap(\"texthealthbar\") + Font(\"silverfont\"))");

    return true;
}

void RenderUITextBarMode()
{
    const double t = TTime::Time();
    const float r = 0.10f + 0.04f * float(std::sin(t * 0.6));
    const float g = 0.12f + 0.04f * float(std::sin(t * 0.8 + 1.0));
    const float b = 0.16f + 0.04f * float(std::sin(t * 1.0 + 2.0));
    Display.BackBuffer()->StartPass(r, g, b, 1.0f);
    Display.BackBuffer()->EndPass();
}

void CloseUITextBarMode()
{
    if (g_textbar)
        g_textbar->Close();
    g_textbar.reset();
}
