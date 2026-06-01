// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uidragstate.cpp - Cross-pane drag-state singleton                    *
// *************************************************************************

#include "uidragstate.h"

#include "audio_backend.h"
#include "cursor.h"
#include "logging.h"

#include <algorithm>
#include <cstring>

namespace UIDragState {

namespace {
SUIDragState g_state;

const char* SrcName(EDragSource s)
{
    switch (s) {
        case EDragSource::None:      return "none";
        case EDragSource::Inventory: return "inv";
        case EDragSource::BarInv:    return "barinv";
        case EDragSource::Equip:     return "equip";
        case EDragSource::SpellPane: return "spell";
    }
    return "?";
}

// ---------------------------------------------------------------------------
// Perform the harness item swap on drop commit.
// ---------------------------------------------------------------------------
void CommitHarnessSwap(EDragSource src, int32_t src_idx,
                       EDragSource dst, int32_t dst_idx)
{
    const bool srcInv   = (src == EDragSource::Inventory);
    const bool srcEquip = (src == EDragSource::Equip);
    const bool dstInv   = (dst == EDragSource::Inventory);
    const bool dstEquip = (dst == EDragSource::Equip);

    // Bounds-check both indices.
    if (srcInv   && (src_idx < 0 || src_idx >= kHarnessInvSlots))   return;
    if (srcEquip && (src_idx < 0 || src_idx >= kHarnessEquipSlots)) return;
    if (dstInv   && (dst_idx < 0 || dst_idx >= kHarnessInvSlots))   return;
    if (dstEquip && (dst_idx < 0 || dst_idx >= kHarnessEquipSlots)) return;

    SHarnessSlot* srcSlot = srcInv ? &harness_inv[src_idx]
                          : srcEquip ? &harness_equip[src_idx]
                          : nullptr;
    SHarnessSlot* dstSlot = dstInv ? &harness_inv[dst_idx]
                          : dstEquip ? &harness_equip[dst_idx]
                          : nullptr;

    if (!srcSlot || !dstSlot) return;

    if (src == dst && src_idx == dst_idx) return;   // drop onto self — no-op

    // Swap (handles all four Inv↔Equip/Equip↔Inv/Inv↔Inv/Equip↔Equip cases).
    std::swap(*srcSlot, *dstSlot);

    log_info("[drag] harness swap committed: %s[%d] ↔ %s[%d]",
             SrcName(src), src_idx, SrcName(dst), dst_idx);
}

// Sound file for UI inventory actions.  Retail names not confirmed from recon
// (spec §0 — no PlayWave XREFs found in InventorySidebarPane / TEquipPane
// recon files); using the available "open.wav" UI sound as a placeholder.
// Spec gap: retail-specific sound names (equip/unequip/move/use) TBD.
constexpr const char* kActionSound = "data/open.wav";

} // namespace

// ---------------------------------------------------------------------------
// Public shared harness state (extern in header).
// ---------------------------------------------------------------------------
SHarnessSlot harness_inv  [kHarnessInvSlots]   = {};
SHarnessSlot harness_equip[kHarnessEquipSlots] = {};

// ---------------------------------------------------------------------------
SUIDragState& Get() { return g_state; }

void ResetHarness()
{
    for (auto& s : harness_inv)   s = SHarnessSlot{};
    for (auto& s : harness_equip) s = SHarnessSlot{};
}

void BeginDrag(EDragSource src, int32_t slot_idx,
               TObjectInstance* item,
               int32_t mouse_x, int32_t mouse_y,
               PTBitmap icon)
{
    g_state.source     = src;
    g_state.source_idx = slot_idx;
    g_state.item       = item;
    g_state.start_x    = mouse_x;
    g_state.start_y    = mouse_y;

    // Auto-resolve icon from harness state if caller didn't pass one.
    // This allows uisidebartest.cpp (which calls BeginDrag without an icon)
    // to still get a drag ghost for inventory and equip pane items.
    if (!icon)
    {
        if (src == EDragSource::Inventory
            && slot_idx >= 0 && slot_idx < kHarnessInvSlots)
        {
            icon = harness_inv[slot_idx].icon;
        }
        else if (src == EDragSource::Equip
                 && slot_idx >= 0 && slot_idx < kHarnessEquipSlots)
        {
            icon = harness_equip[slot_idx].icon;
        }
    }

    // Attach the drag ghost so it follows the cursor.  Offset (0,0) puts the
    // icon TL at the cursor; retail uses the click-offset within the cell as
    // the grab point.  For the test harness (0,0) is correct — the spec's
    // grab-offset logic (InventoryPane_SPEC §9 "promote" step) requires per-
    // cell sub-pixel position which is not tracked in this harness path.
    if (icon)
        SetDragBitmap(icon, 0, 0);
    else
        ClearDragBitmap();

    log_info("[drag] begin from %s slot=%d item=%p @ (%d,%d) icon=%s",
             SrcName(src), slot_idx, (void*)item, mouse_x, mouse_y,
             icon ? "yes" : "none");
}

bool CompleteDrag(EDragSource dest, int32_t dest_slot, bool commit)
{
    if (g_state.source == EDragSource::None) return false;
    const EDragSource oldSrc = g_state.source;
    const int32_t     oldIdx = g_state.source_idx;

    // Clear the drag ghost regardless of commit status.
    ClearDragBitmap();

    if (commit)
    {
        log_info("[drag] DROP %s slot=%d -> %s slot=%d",
                 SrcName(oldSrc), oldIdx, SrcName(dest), dest_slot);
        CommitHarnessSwap(oldSrc, oldIdx, dest, dest_slot);
        // Sound: play an action sound on successful drop (#14).
        // Spec gap: retail-specific sound name not confirmed from recon.
        audio::PlayOneShot(kActionSound);
    }
    else
    {
        log_info("[drag] CANCEL %s slot=%d (drop on %s slot=%d rejected)",
                 SrcName(oldSrc), oldIdx, SrcName(dest), dest_slot);
    }

    g_state = SUIDragState{};
    return commit;
}

void Cancel()
{
    if (g_state.source == EDragSource::None) return;
    ClearDragBitmap();
    log_info("[drag] cancelled from %s slot=%d",
             SrcName(g_state.source), g_state.source_idx);
    g_state = SUIDragState{};
}

bool IsActive()
{
    return g_state.source != EDragSource::None;
}

} // namespace UIDragState
