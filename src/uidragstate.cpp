// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uidragstate.cpp - Cross-pane drag-state singleton                    *
// *************************************************************************

#include "uidragstate.h"

#include "audio_backend.h"
#include "cursor.h"
#include "dialog.h"
#include "invslot.h"
#include "logging.h"
#include "player.h"
#include "revenant.h"   // DialogList, TextBar
#include "textbar.h"

#include <cstdlib>

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

// The inventory slot number a carried or belt cell stands for.
int32_t CarriedSlotOf(EDragSource s, int32_t idx)
{
    switch (s) {
        case EDragSource::Inventory: return idx;
        case EDragSource::BarInv:    return kInvSlotBeltFirst + idx;
        default:                     return -1;
    }
}

// Move a dragged item of the main player to another HUD cell. Equipment
// goes through TPlayer::Equip (retail 0x005199b0), which puts the item it
// displaces where the dragged item was; carried and belt slots swap slot
// numbers with their occupant. This is the drop the 1998 TEquipPane /
// TInventory performed (equip.cpp:83-128) on top of retail's Equip.
// Not ported: dropping into a pouch (retail AddToInventory on the pouch).
bool CommitMove(TObjectInstance* item, EDragSource src, int32_t srcIdx,
                EDragSource dst, int32_t dstIdx)
{
    TPlayer* player = Player;
    if (!player || !item || item->GetOwner() != player)
        return false;
    if (src == dst && srcIdx == dstIdx)
        return false;

    // REVSYNC: TEquipPane::MouseClick 0x005363e0, button up. A held pack or
    // belt item goes to its own slot wherever it is let go on the pane; an
    // item held from the pane itself is just let go.
    if (dst == EDragSource::Equip)
        return src != EDragSource::Equip && EquipInOwnSlot(item);

    const int32_t to = CarriedSlotOf(dst, dstIdx);
    if (to < 0)
        return false;
    TObjectInstance* occupant = player->GetInventorySlot(to);
    if (src == EDragSource::Equip)
    {
        // An occupant that fits the vacated equipment slot trades places.
        if (occupant)
            return player->CanEquip(occupant, srcIdx) && player->Equip(occupant, srcIdx);
        player->Equip(nullptr, srcIdx);
        item->SetInventNum(short(to));
        return true;
    }
    if (occupant)
        occupant->SetInventNum(short(item->InventNum()));
    item->SetInventNum(short(to));
    return true;
}

// Sound file for UI inventory actions. The important ownership rule is now
// pinned: drag sounds belong to the transaction manager, not individual slots.
// Retail names are not confirmed from the currently extracted InventoryPane /
// TEquipPane bodies; using the available open.wav UI sound as a placeholder
// until a retail capture or sound-registry pass identifies the exact cue.
constexpr const char* kActionSound = "data/open.wav";

} // namespace

// ---------------------------------------------------------------------------
SUIDragState& Get() { return g_state; }

// REVSYNC: the "equip it" of the equip pane's button up (0x005363e0) and the
// inventory's right button up (0x00538210): the item's "eqslot" stat names
// its slot; CanEquip (0x00519300), then Equip (0x005199b0). With no slot,
// the EQUIPUNABLE line goes to the text bar. Not ported: the item's
// class sound after equipping (0x00473a10), and the walk-root refresh
// (SetWalkMode 0x004cf000) after equipping a light source, which needs the
// torch roots (GetTorchRoot, TPlayer vtable +0x30c).
bool EquipInOwnSlot(TObjectInstance* item)
{
    TPlayer* player = Player;
    if (!player || !item || item->GetOwner() != player)
        return false;
    if (item->FindStat("EqSlot") < 0)
    {
        TextBar.Print("%s", DialogList.GetLine("EQUIPUNABLE"));
        return false;
    }
    const int32_t slot = item->GetStat("EqSlot");
    if ((uint32_t)slot >= NUM_EQ_SLOTS || !player->CanEquip(item, slot))
        return false;
    return player->Equip(item, slot);
}

bool BeginDrag(EDragSource src, int32_t slot_idx,
               TObjectInstance* item,
               int32_t mouse_x, int32_t mouse_y,
               PTBitmap icon,
               int32_t grab_x, int32_t grab_y)
{
    if (!icon)
        icon = TInvSlot::ItemIcon(item);
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

    ClampGrabToVisual(g_state.layers, g_state.layer_count, g_state.grab_x, g_state.grab_y);
    g_state.icon = g_state.layer_count > 0 ? g_state.layers[0].bitmap : nullptr;

    // A mouse-down is only a pending grab. The icon stays in its slot until
    // UpdateDrag observes real movement, so click/use remains distinct from
    // click-drag. Clear any stale ghost from a previous transaction.
    ClearDragBitmap();

    log_info("[drag] pending from %s slot=%d item='%s' @ (%d,%d) grab=(%d,%d) icon=%s",
             SrcName(src), slot_idx, item ? item->GetName() : "-", mouse_x, mouse_y,
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

    log_info("[drag] click %s slot=%d", SrcName(g_state.source), g_state.source_idx);
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

    // Clear the drag ghost regardless of commit status.
    ClearDragBitmap();

    // Spell-book drags carry no item; the drop target applies them.
    TObjectInstance* const item = g_state.item.Get();
    if (commit && oldSrc != EDragSource::SpellPane)
        commit = CommitMove(item, oldSrc, oldIdx, dest, dest_slot);

    if (commit)
    {
        // An equip drop lands in the item's own slot, not the cell under it.
        const int32_t landed = (dest == EDragSource::Equip && item)
            ? item->InventNum() - kInvSlotEquipFirst : dest_slot;
        log_info("[drag] DROP %s slot=%d -> %s slot=%d",
                 SrcName(oldSrc), oldIdx, SrcName(dest), landed);
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
