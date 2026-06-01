// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uidragstate.cpp - Cross-pane drag-state singleton                    *
// *************************************************************************

#include "uidragstate.h"

#include "logging.h"

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
} // namespace

SUIDragState& Get() { return g_state; }

void BeginDrag(EDragSource src, int32_t slot_idx,
               TObjectInstance* item,
               int32_t mouse_x, int32_t mouse_y)
{
    g_state.source     = src;
    g_state.source_idx = slot_idx;
    g_state.item       = item;
    g_state.start_x    = mouse_x;
    g_state.start_y    = mouse_y;
    log_info("[drag] begin from %s slot=%d item=%p @ (%d,%d)",
             SrcName(src), slot_idx, (void*)item, mouse_x, mouse_y);
}

bool CompleteDrag(EDragSource dest, int32_t dest_slot, bool commit)
{
    if (g_state.source == EDragSource::None) return false;
    const EDragSource oldSrc = g_state.source;
    const int32_t     oldIdx = g_state.source_idx;
    if (commit)
        log_info("[drag] DROP %s slot=%d -> %s slot=%d",
                 SrcName(oldSrc), oldIdx, SrcName(dest), dest_slot);
    else
        log_info("[drag] CANCEL %s slot=%d (drop on %s slot=%d rejected)",
                 SrcName(oldSrc), oldIdx, SrcName(dest), dest_slot);
    g_state = SUIDragState{};
    return commit;
}

void Cancel()
{
    if (g_state.source == EDragSource::None) return;
    log_info("[drag] cancelled from %s slot=%d", SrcName(g_state.source), g_state.source_idx);
    g_state = SUIDragState{};
}

bool IsActive()
{
    return g_state.source != EDragSource::None;
}

} // namespace UIDragState
