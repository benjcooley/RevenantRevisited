// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uidragstate.cpp - Cross-pane drag-state singleton                    *
// *************************************************************************

#include "uidragstate.h"

#include "cursor.h"
#include "dialog.h"
#include "inventory.h"
#include "logging.h"
#include "player.h"
#include "revenant.h"   // DialogList, TextBar
#include "sound.h"
#include "textbar.h"

#include <cstdlib>
#include <string>

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
        case EDragSource::QuickSpell: return "quickspell";
    }
    return "?";
}

// REVSYNC: 0x00538ce6 (inventory), 0x0052d746 (belt): a grab becomes a drag
// once the pointer is 2 pixels or more from where it was pressed, across or
// down.
constexpr int32_t kDragThresholdPx = 2;

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

bool IsItemPane(EDragSource s)
{
    return s == EDragSource::Inventory || s == EDragSource::BarInv || s == EDragSource::Equip;
}

// The inventory an item pane shows: the inventory pane's container (the
// player's, or a bag he opened); the belt's and the doll's are the player's.
TObjectInstance* ContainerOf(EDragSource pane)
{
    return pane == EDragSource::Inventory ? Inventory.GetContainer() : Player;
}

// The inventory slot a cell of `pane` stands for: an inventory cell's own
// number, belt box n's 0x10b + n.
int32_t SlotOf(EDragSource pane, int32_t cell)
{
    switch (pane) {
        case EDragSource::Inventory: return cell;
        case EDragSource::BarInv:    return kInvSlotBeltFirst + cell;
        default:                     return -1;
    }
}

// The item panes' named sounds ("pop", "pouch"): found, loaded and played at
// full volume (retail 0x0049c430, 0x0049b650, 0x0049b990).
void PlayPaneSound(const char* name)
{
    const int32_t id = SoundPlayer.FindSound(name);
    if (id >= 0 && SoundPlayer.Mount(id))
        SoundPlayer.Play(id);
}

// The move every item pane falls back on (retail 0x005388e0, 0x00538bb6,
// 0x0052d425, 0x0052d684): AddToInventory's rules put the item at the slot
// (the item there makes room, a pile takes it in, ...), and "pop".
void MoveTo(TObjectInstance* container, TObjectInstance* item, int32_t slot)
{
    log_info("[drag] move '%s' to %s slot 0x%x", item->GetName(), container->GetName(), slot);
    container->AddToInventory(item, slot);
    PlayPaneSound("pop");
}

// `on` used by the main player, with `with` or alone (retail vtable 0xbc).
// Not ported (multiplayer only): the CanCombine test before it (0x0066829c).
bool UseOn(TObjectInstance* on, TObjectInstance* with)
{
    const std::string what = std::string(on->GetName()) + (with ? std::string(" with ") + with->GetName() : "");
    const bool used = on->Use(Player, with ? with->GetMapIndex() : -1);
    log_info("[drag] use %s: %s", what.c_str(), used ? "used" : "not used");
    return used;
}

// An item let go over a cell of the pane it was picked up from (the
// inventory 0x0053878e..0x005388f2, the belt 0x0052d304..0x0052d425): a
// click uses it, a drag onto another item uses that one with it, and what
// isn't used moves there.
void ReleaseInPane(TObjectInstance* container, int32_t slot, TObjectInstance* item, bool dragging)
{
    TObjectInstance* const there = container->GetInventorySlot(slot);
    const bool click = !dragging && there == item;
    const bool onto = dragging && there && there != item;
    if ((click || onto) && UseOn(there, click ? nullptr : item))
        return;
    MoveTo(container, item, slot);
}

// An item from the other carrying pane let go over a cell (the belt's on the
// inventory 0x00538af3..0x00538c06, the inventory's on the belt
// 0x0052d5c8..0x0052d6d1): never into itself (a bag onto its own contents)
// or onto the bag it is in; the item there is used with it, and what isn't
// used moves there.
void ReleaseFromPane(TObjectInstance* container, int32_t slot, TObjectInstance* item)
{
    TObjectInstance* const there = container->GetInventorySlot(slot);
    if (item == container || item->Holds(container) || (there && there == item->GetOwner()))
        return;
    if (there && UseOn(there, item))
        return;
    MoveTo(container, item, slot);
}

// A worn item let go over a cell (the inventory 0x00538992..0x00538ae5, the
// belt 0x0052d4e4..0x0052d5ba): onto an empty cell it comes off and moves
// there; onto an item that may be worn in its place, the two trade places.
// A light source then sets the walk root back (SetWalkMode 0x004cf000).
void ReleaseWorn(TObjectInstance* container, int32_t slot, TObjectInstance* worn, int32_t eqslot)
{
    TPlayer* const player = Player;
    if (player->GetEquip(eqslot) != worn)
        return;
    const bool light = worn->ObjClass() == OBJCLASS_LIGHTSOURCE;
    TObjectInstance* const there = container->GetInventorySlot(slot);
    if (!there)
    {
        player->Equip(nullptr, eqslot);
        MoveTo(container, worn, slot);
    }
    else if (player->CanEquip(there, eqslot))
        player->Equip(there, eqslot);
    if (light)
        player->SetWalkMode();
}

// The grab is over: no ghost, nothing held.
void End()
{
    ClearDragBitmap();
    SetDragObj(nullptr);
    g_state = SUIDragState{};
}

} // namespace

// ---------------------------------------------------------------------------
SUIDragState& Get() { return g_state; }

// REVSYNC: the "equip it" of the doll's button up (0x0053655a..0x0053665e)
// and of the item panes' right button up (0x0053849e..0x005384ed,
// 0x0052d1d5..0x0052d24e): the item's "eqslot" stat names its slot; -1 puts
// EQUIPUNABLE on the text bar, past the slots does nothing, else CanEquip
// (0x00519300), Equip (0x005199b0) and the item's sound; a light source
// then sets the walk root (SetWalkMode 0x004cf000: "torch"). An item without
// the stat reads 0 and CanEquip refuses it, silently.
bool EquipInOwnSlot(TObjectInstance* item)
{
    TPlayer* player = Player;
    if (!player || !item)
        return false;
    const int32_t slot = item->GetStat("EqSlot");
    if (slot == -1)
    {
        TextBar.Print("%s", DialogList.GetLine("EQUIPUNABLE"));
        return false;
    }
    if ((uint32_t)slot >= NUM_EQ_SLOTS || !player->CanEquip(item, slot))
        return false;
    const bool equipped = player->Equip(item, slot);
    if (equipped)
        item->PlayItemSound();
    if (item->ObjClass() == OBJCLASS_LIGHTSOURCE)
        player->SetWalkMode();
    return equipped;
}

bool BeginDrag(EDragSource src, int32_t slot_idx,
               TObjectInstance* item,
               int32_t mouse_x, int32_t mouse_y,
               PTBitmap icon,
               int32_t grab_x, int32_t grab_y)
{
    // By default the item's inventory icon (retail 0x0052d6e0: vtable +0x130),
    // a stack's copies as layers.
    SDragBitmapLayer layers[kMaxDragBitmapLayers] = {};
    int32_t count = 0;
    if (icon)
        layers[count++] = { icon, 0, 0 };
    else if (item)
    {
        const SInvIcon parts = item->InventoryIcon();
        for (int32_t i = 0; i < parts.count && count < kMaxDragBitmapLayers; ++i)
            layers[count++] = { parts.parts[i].image, parts.parts[i].x, parts.parts[i].y };
    }
    return BeginDragLayers(src, slot_idx, item, mouse_x, mouse_y, count ? layers : nullptr, count,
                           grab_x, grab_y);
}

namespace {

// The pending grab becomes a drag: its ghost follows the pointer, and the
// item is held (SetDragObj, which plays its sound; retail 0x00538d83,
// 0x0052d7e0, 0x00536539).
void Promote(int32_t mouse_x, int32_t mouse_y)
{
    g_state.pending = false;
    g_state.dragging = true;
    if (g_state.layer_count > 0)
        SetDragBitmapLayers(g_state.layers, g_state.layer_count, g_state.grab_x, g_state.grab_y);
    else
        ClearDragBitmap();
    SetDragObj(g_state.item.Get());
    log_info("[drag] promote %s slot=%d at (%d,%d) grab=(%d,%d)",
             SrcName(g_state.source), g_state.source_idx,
             mouse_x, mouse_y, g_state.grab_x, g_state.grab_y);
}

}  // namespace

bool BeginDragLayers(EDragSource src, int32_t slot_idx,
                     TObjectInstance* item,
                     int32_t mouse_x, int32_t mouse_y,
                     const SDragBitmapLayer* layers, int32_t layer_count,
                     int32_t grab_x, int32_t grab_y)
{
    if (!item && src != EDragSource::SpellPane)
    {
        End();
        return false;
    }

    g_state.source     = src;
    g_state.source_idx = slot_idx;
    g_state.item       = item;
    g_state.container  = ContainerOf(src);
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

    // REVSYNC: 0x005364f2..0x0053654e -- the doll has no threshold: what is
    // worn is picked up on the press.
    if (src == EDragSource::Equip)
        Promote(mouse_x, mouse_y);
    return true;
}

bool BeginSpellDrag(int32_t row, const char* talismans, PTBitmap icon,
                    int32_t mouse_x, int32_t mouse_y, int32_t grab_x, int32_t grab_y)
{
    if (!talismans || !*talismans)
        return false;
    const SDragBitmapLayer layer = { icon, 0, 0 };
    BeginDragLayers(EDragSource::SpellPane, row, nullptr, mouse_x, mouse_y, icon ? &layer : nullptr,
                    icon ? 1 : 0, grab_x, grab_y);
    g_state.spell = talismans;
    Promote(mouse_x, mouse_y);
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
        if (std::abs(dx) >= kDragThresholdPx || std::abs(dy) >= kDragThresholdPx)
            Promote(mouse_x, mouse_y);
    }
    return true;
}

// REVSYNC: the left button up of the item panes for an item picked up in one
// of them: the inventory 0x00538210 (0x0053878e..0x00538c06), the belt
// 0x0052cfb0 (0x0052d304..0x0052d6d1), the doll 0x005363e0
// (0x0053655a..0x0053668e). Each pane takes the release over its own cells;
// a pane's held slot tells it where the item came from, which the port keeps
// as the grab's source.
void Release(EDragSource dest, int32_t dest_cell)
{
    if (!IsItemPane(g_state.source))
        return;
    const EDragSource src = g_state.source;
    const int32_t src_cell = g_state.source_idx;
    const bool dragging = g_state.dragging;
    TObjectInstance* const item = g_state.item.Get();
    log_info("[drag] release %s slot=%d (%s) on %s slot=%d", SrcName(src), src_cell,
             dragging ? "drag" : "click", SrcName(dest), dest_cell);
    End();
    if (!item || !Player)
        return;

    if (dest == EDragSource::Equip)
    {
        // A pack or belt item goes to its own slot wherever it is let go on
        // the doll; one of the doll's own is just let go.
        if (src != EDragSource::Equip)
            EquipInOwnSlot(item);
        return;
    }
    TObjectInstance* const container = ContainerOf(dest);
    const int32_t slot = SlotOf(dest, dest_cell);
    if (!container || slot < 0)
        return;
    if (src == EDragSource::Equip)
        ReleaseWorn(container, slot, item, src_cell);
    else if (src == dest)
        ReleaseInPane(container, slot, item, dragging);
    else
        ReleaseFromPane(container, slot, item);
}

// REVSYNC: the inventory's left button up on its container icon
// (0x005385e9..0x00538725), wherever the press was. With nothing picked up
// in the inventory, the inventory the shown bag is in is shown; with an item
// picked up there, it moves up into that inventory when it has room
// ("pouch"). The player's own inventory has nowhere to go.
void ReleaseOnContainerIcon()
{
    if (g_state.source != EDragSource::None && !IsItemPane(g_state.source))
        return;
    const bool fromHere = g_state.source == EDragSource::Inventory;
    TObjectInstance* const item = fromHere ? g_state.item.Get() : nullptr;
    End();

    TObjectInstance* const shown = Inventory.GetContainer();
    TObjectInstance* const owner = shown ? shown->GetOwner() : nullptr;
    if (!owner)
        return;
    if (!fromHere)
    {
        log_info("[drag] container icon: up from %s to %s", shown->GetName(), owner->GetName());
        Inventory.SetContainer(owner);
        return;
    }
    const int32_t free = owner->FindFreeInventorySlot();
    if (item && free <= kInvSlotLastCarried)
    {
        log_info("[drag] container icon: '%s' up into %s", item->GetName(), owner->GetName());
        owner->AddToInventory(item, free);
        PlayPaneSound("pouch");
    }
}

// REVSYNC: the right button up of the inventory (0x00538336..0x00538517)
// and the belt (0x0052d0b8..0x0052d26d), with nothing held: a talisman the
// player carries himself goes into his Spell Pouch ("pouch"; EQUIPUNABLE
// when he has none), anything else is equipped in its own slot, and with no
// item there the container's carried items close up. Not ported: the item
// information of the stats mode (0x0065c9e0, 0x0047eff0).
void RightClick(TObjectInstance* container, TObjectInstance* item)
{
    TPlayer* const player = Player;
    if (!player || IsActive())
        return;
    if (!item)
    {
        if (container)
            container->CompactInventory();
        return;
    }
    log_info("[drag] right click '%s'", item->GetName());
    if (item->ObjClass() == OBJCLASS_TALISMAN && item->GetOwner() == player)
    {
        if (TObjectInstance* const pouch = player->FindObjInventory("Spell Pouch"))
        {
            pouch->AddToInventory(item, -1);
            PlayPaneSound("pouch");
        }
        else
            TextBar.Print("%s", DialogList.GetLine("EQUIPUNABLE"));
        return;
    }
    EquipInOwnSlot(item);
}

bool CompleteDrag(EDragSource dest, int32_t dest_slot, bool commit)
{
    if (g_state.source == EDragSource::None)
        return false;
    // Only a spell is dropped here: the ring it lands on copies it, silently
    // (retail 0x00544890). Items go through Release.
    commit = commit && g_state.source == EDragSource::SpellPane && g_state.dragging;
    log_info("[drag] %s %s slot=%d on %s slot=%d", commit ? "DROP" : "CANCEL",
             SrcName(g_state.source), g_state.source_idx, SrcName(dest), dest_slot);
    End();
    return commit;
}

void Cancel()
{
    if (g_state.source == EDragSource::None) return;
    log_info("[drag] cancelled from %s slot=%d",
             SrcName(g_state.source), g_state.source_idx);
    End();
}

void ReleaseUnclaimed()
{
    Cancel();
}

bool IsActive()
{
    return g_state.source != EDragSource::None;
}

bool IsDragging()
{
    return g_state.dragging;
}

} // namespace UIDragState
