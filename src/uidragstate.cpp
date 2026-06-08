// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uidragstate.cpp - Cross-pane drag-state singleton                    *
// *************************************************************************

#include "uidragstate.h"

#include "audio_backend.h"
#include "cursor.h"
#include "logging.h"
#include "object.h"

#include <algorithm>
#include <cstdlib>
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
        case EDragSource::Playfield: return "playfield";
    }
    return "?";
}

constexpr int32_t kDragThresholdPx = 4;

int32_t SlotCount(EDragSource s)
{
    switch (s) {
        case EDragSource::Inventory: return kHarnessInvSlots;
        case EDragSource::BarInv:    return kHarnessBarInvSlots;
        case EDragSource::Equip:     return kHarnessEquipSlots;
        default:                     return 0;
    }
}

SHarnessSlot* SlotFor(EDragSource s, int32_t idx)
{
    if (idx < 0 || idx >= SlotCount(s)) return nullptr;
    switch (s) {
        case EDragSource::Inventory: return &harness_inv[idx];
        case EDragSource::BarInv:    return &harness_barinv[idx];
        case EDragSource::Equip:     return &harness_equip[idx];
        default:                     return nullptr;
    }
}

void ClampGrabToVisual(const SDragBitmapLayer* layers, int32_t layer_count,
                       int32_t& grab_x, int32_t& grab_y)
{
    if (grab_x < 0) grab_x = 0;
    if (grab_y < 0) grab_y = 0;
    int32_t w = 0;
    int32_t h = 0;
    for (int32_t i = 0; i < layer_count; ++i)
    {
        const SDragBitmapLayer& layer = layers[i];
        if (!layer.bitmap) continue;
        const int32_t layerRight = layer.x + layer.bitmap->width;
        const int32_t layerBottom = layer.y + layer.bitmap->height;
        if (layerRight > w) w = layerRight;
        if (layerBottom > h) h = layerBottom;
    }
    if (w > 0 && grab_x >= w) grab_x = w - 1;
    if (h > 0 && grab_y >= h) grab_y = h - 1;
}

// ---------------------------------------------------------------------------
// Perform the harness item swap on drop commit.
// ---------------------------------------------------------------------------
void CommitHarnessSwap(EDragSource src, int32_t src_idx,
                       EDragSource dst, int32_t dst_idx)
{
    SHarnessSlot* srcSlot = SlotFor(src, src_idx);
    SHarnessSlot* dstSlot = SlotFor(dst, dst_idx);

    if (!srcSlot || !dstSlot) return;

    if (src == dst && src_idx == dst_idx) return;   // drop onto self: no-op

    // Swap covers inventory, bottom bar, and equipment slots. Replacement is
    // just the item from the destination returning to the source.
    std::swap(*srcSlot, *dstSlot);

    log_info("[drag] harness swap committed: %s[%d] <-> %s[%d]",
             SrcName(src), src_idx, SrcName(dst), dst_idx);
}

// Sound file for UI inventory actions. The important ownership rule is now
// pinned: drag sounds belong to the transaction manager, not individual slots.
// Retail names are not confirmed from the currently extracted InventoryPane /
// TEquipPane bodies; using the available open.wav UI sound as a placeholder
// until a retail capture or sound-registry pass identifies the exact cue.
constexpr const char* kActionSound = "data/open.wav";

} // namespace

// ---------------------------------------------------------------------------
// Public shared harness state (extern in header).
// ---------------------------------------------------------------------------
SHarnessSlot harness_inv  [kHarnessInvSlots]   = {};
SHarnessSlot harness_barinv[kHarnessBarInvSlots] = {};
SHarnessSlot harness_equip[kHarnessEquipSlots] = {};

// ---------------------------------------------------------------------------
SUIDragState& Get() { return g_state; }

void ResetHarness()
{
    ResetInventoryHarness();
    ResetBarInvHarness();
    ResetEquipHarness();
}

void ResetInventoryHarness()
{
    for (auto& s : harness_inv)   s = SHarnessSlot{};
}

void ResetBarInvHarness()
{
    for (auto& s : harness_barinv) s = SHarnessSlot{};
}

void ResetEquipHarness()
{
    for (auto& s : harness_equip) s = SHarnessSlot{};
}

bool BeginDrag(EDragSource src, int32_t slot_idx,
               TObjectInstance* item,
               int32_t mouse_x, int32_t mouse_y,
               PTBitmap icon,
               int32_t grab_x, int32_t grab_y)
{
    SDragBitmapLayer layer = {};
    if (icon)
        layer = { icon, 0, 0 };
    return BeginDragLayers(src, slot_idx, item, mouse_x, mouse_y,
                           icon ? &layer : nullptr, icon ? 1 : 0,
                           grab_x, grab_y);
}

bool BeginDragLayers(EDragSource src, int32_t slot_idx,
                     TObjectInstance* item,
                     int32_t mouse_x, int32_t mouse_y,
                     const SDragBitmapLayer* layers, int32_t layer_count,
                     int32_t grab_x, int32_t grab_y)
{
    SHarnessSlot* harnessSlot = SlotFor(src, slot_idx);
    if (harnessSlot)
    {
        if (!harnessSlot->inst)
        {
            g_state = SUIDragState{};
            ClearDragBitmap();
            return false;
        }
        item = harnessSlot->inst;
    }

    if (!item && src != EDragSource::SpellPane)
    {
        g_state = SUIDragState{};
        ClearDragBitmap();
        return false;
    }

    g_state.source     = src;
    g_state.source_idx = slot_idx;
    g_state.item       = item;
    g_state.start_x    = mouse_x;
    g_state.start_y    = mouse_y;
    g_state.grab_x     = grab_x;
    g_state.grab_y     = grab_y;
    g_state.pending    = true;
    g_state.dragging   = false;

    // Auto-resolve visual from harness state if caller didn't pass one.
    // This allows uisidebartest.cpp to initiate drags without per-pane icon
    // plumbing while still preserving the original click offset.
    g_state.layer_count = 0;
    for (SDragBitmapLayer& layer : g_state.layers)
        layer = SDragBitmapLayer{};

    if (layers && layer_count > 0)
    {
        for (int32_t i = 0; i < layer_count && g_state.layer_count < kMaxDragBitmapLayers; ++i)
        {
            if (!layers[i].bitmap) continue;
            g_state.layers[g_state.layer_count++] = layers[i];
        }
    }
    if (g_state.layer_count == 0 && harnessSlot && harnessSlot->icon)
        g_state.layers[g_state.layer_count++] = { harnessSlot->icon, 0, 0 };

    ClampGrabToVisual(g_state.layers, g_state.layer_count, g_state.grab_x, g_state.grab_y);
    g_state.icon = g_state.layer_count > 0 ? g_state.layers[0].bitmap : nullptr;

    // A mouse-down is only a pending grab. The icon stays in its slot until
    // UpdateDrag observes real movement, so click/use remains distinct from
    // click-drag. Clear any stale ghost from a previous transaction.
    ClearDragBitmap();

    log_info("[drag] pending from %s slot=%d item=%p @ (%d,%d) grab=(%d,%d) icon=%s",
             SrcName(src), slot_idx, (void*)item, mouse_x, mouse_y,
             g_state.grab_x, g_state.grab_y,
             g_state.layer_count > 0 ? "yes" : "none");
    return true;
}

bool UpdateDrag(int32_t mouse_x, int32_t mouse_y)
{
    if (g_state.source == EDragSource::None)
        return false;

    if (!g_state.dragging)
    {
        const int32_t dx = mouse_x - g_state.start_x;
        const int32_t dy = mouse_y - g_state.start_y;
        if (std::abs(dx) < kDragThresholdPx && std::abs(dy) < kDragThresholdPx)
            return true;

        g_state.pending = false;
        g_state.dragging = true;

        if (g_state.layer_count > 0)
            SetDragBitmapLayers(g_state.layers, g_state.layer_count,
                                g_state.grab_x, g_state.grab_y);
        else
            ClearDragBitmap();

        log_info("[drag] promote %s slot=%d at (%d,%d) grab=(%d,%d)",
                 SrcName(g_state.source), g_state.source_idx,
                 mouse_x, mouse_y, g_state.grab_x, g_state.grab_y);
    }

    return true;
}

bool CompleteClick()
{
    if (g_state.source == EDragSource::None || !g_state.pending)
        return false;

    log_info("[drag] click %s slot=%d item=%p",
             SrcName(g_state.source), g_state.source_idx, (void*)g_state.item);
    ClearDragBitmap();
    g_state = SUIDragState{};
    return true;
}

bool CompleteDrag(EDragSource dest, int32_t dest_slot, bool commit)
{
    if (g_state.source == EDragSource::None) return false;
    if (!g_state.dragging)
    {
        CompleteClick();
        return false;
    }
    const EDragSource oldSrc = g_state.source;
    const int32_t     oldIdx = g_state.source_idx;

    if (commit && dest == EDragSource::Equip)
    {
        TObjectInstance* item = g_state.item;
        if (oldSrc != EDragSource::Inventory &&
            oldSrc != EDragSource::BarInv &&
            oldSrc != EDragSource::Equip)
        {
            commit = false;
        }
        else if (!item)
        {
            commit = false;
        }
        else if (item->FindStat("EqSlot") < 0 || item->GetStat("EqSlot") != dest_slot)
        {
            log_info("[drag] reject equip drop: item=%p EqSlot=%d dest=%d",
                     (void*)item,
                     item->FindStat("EqSlot") >= 0 ? item->GetStat("EqSlot") : -1,
                     dest_slot);
            commit = false;
        }
    }

    // Clear the drag ghost regardless of commit status.
    ClearDragBitmap();

    if (commit)
    {
        log_info("[drag] DROP %s slot=%d -> %s slot=%d",
                 SrcName(oldSrc), oldIdx, SrcName(dest), dest_slot);
        CommitHarnessSwap(oldSrc, oldIdx, dest, dest_slot);
        // Sound: play the shared drag/drop action cue on successful commit.
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

bool IsTracking()
{
    return g_state.source != EDragSource::None;
}

bool IsPending()
{
    return g_state.pending;
}

bool IsDragging()
{
    return g_state.dragging;
}

} // namespace UIDragState
