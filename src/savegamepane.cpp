// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   savegamepane.cpp - the load and save dialogs                        *
// *************************************************************************

#include "savegamepane.h"

#include "logging.h"
#include "renderer.h"
#include "savegame.h"
#include "surface.h"

#include <cstring>

namespace {

// Retail's literal for the "charname" field (0x005e430c / 0x005e43d4): every
// slot shows the single-player hero.
constexpr const char* kCharacterName = "Locke";

}  // namespace

// ******************
// * TSaveSlotPane  *
// ******************

bool TSaveSlotPane::OpenSlots(const char* defName, uint32_t defFlags, int32_t x, int32_t y)
{
    ::SaveGame.RefreshSlots();
    selected     = -1;            // retail DAT_0066fb04 = -1
    hasPicture   = false;
    pictureDirty = false;
    picture.clear();
    return Open(defName, "default", defFlags, x, y, WIDTH, HEIGHT, defName);
}

void TSaveSlotPane::OnOpened()
{
    std::vector<std::vector<std::string>> rows;
    for (const SSaveSlot& slot : ::SaveGame.Slots())
        rows.push_back({ slot.name });
    const int32_t count = static_cast<int32_t>(rows.size());
    SetListRows("gamelist", std::move(rows));
    SetHotKey(ConfirmButton(), VK_RETURN);
    SetHotKey("exit", VK_ESCAPE);
    SelectListRow("gamelist", count - 1);
}

void TSaveSlotPane::OnListSelect(const SDefWidget& list, int32_t row)
{
    if (list.name != "gamelist")
        return;
    selected = row;
    const std::vector<SSaveSlot>& slots = ::SaveGame.Slots();
    if (row < 0 || row >= static_cast<int32_t>(slots.size()))
        return;
    const SSaveSlot& slot = slots[static_cast<size_t>(row)];

    // REVSYNC-DIVERGENCE: retail loaded <slot>\ss.bmp over the previous
    // picture only when it read, so a slot without one kept the last slot's
    // picture; the port shows black (INGAME_MENU.md §10).
    hasPicture   = TSaveGame::ReadThumbnail(slot.dir, picture);
    pictureDirty = hasPicture;

    SetText("gamename", slot.name);
    SetText("modname", slot.module);
    SetText("charname", kCharacterName);
}

void TSaveSlotPane::Compose()
{
    // The thumbnail goes up before the pane's render pass opens (and at most
    // once a frame, as a streamed image allows).
    if (pictureDirty && Renderer)
    {
        if (thumbnail == kInvalidTexture)
            thumbnail = Renderer->CreateDynamicTexture(TSaveGame::kThumbnailWidth,
                                                       TSaveGame::kThumbnailHeight,
                                                       ERendererTextureFilter::Nearest);
        if (thumbnail != kInvalidTexture)
            Renderer->UpdateDynamicTexture(thumbnail, picture.data(), picture.size());
        pictureDirty = false;
    }
    TDefPane::Compose();
}

void TSaveSlotPane::DrawField(const SDefWidget& widget)
{
    if (widget.field != "picture")
    {
        TDefPane::DrawField(widget);
        return;
    }
    TSurface* target = Surface();
    if (!target)
        return;
    // Retail's thumbnail bitmap starts cleared to black (0x004a22f0).
    if (hasPicture && thumbnail != kInvalidTexture)
        Renderer->Composite(thumbnail, widget.x, widget.y, TSaveGame::kThumbnailWidth,
                            TSaveGame::kThumbnailHeight, target->Width(), target->Height());
    else
        Renderer->DrawSolidRectToTarget(widget.x, widget.y, TSaveGame::kThumbnailWidth,
                                        TSaveGame::kThumbnailHeight, target->Width(),
                                        target->Height(), 0, 0, 0, 255);
}

// REVSYNC: Close @ 0x00539440 / 0x00539ab0. The thumbnail's texture is kept
// for the next opening.
void TSaveSlotPane::Close()
{
    selected   = -1;
    hasPicture = false;
    picture.clear();
    TDefPane::Close();
}

// ******************
// * TLoadGamePane  *
// ******************

bool TLoadGamePane::OpenLoad(bool fromGame, int32_t x, int32_t y)
{
    return OpenSlots("loadgame", fromGame ? DEF_INGAME : 0, x, y);
}

std::string TLoadGamePane::SelectedSlot() const
{
    const std::vector<SSaveSlot>& slots = ::SaveGame.Slots();
    const int32_t row = Selected();
    return (row >= 0 && row < static_cast<int32_t>(slots.size())) ? slots[static_cast<size_t>(row)].name
                                                                    : std::string();
}

void TLoadGamePane::OnActivate(const SDefWidget& widget, int32_t buttonIndex)
{
    if (widget.name == "loadgame" && Selected() < 0)
        return;
    if (widget.name == "loadgame" || widget.name == "exit")
        TDefPane::OnActivate(widget, buttonIndex);
}

// ******************
// * TSaveGamePane  *
// ******************

bool TSaveGamePane::OpenSave(int32_t x, int32_t y)
{
    savename.clear();
    return OpenSlots("savegame", DEF_INGAME, x, y);
}

std::string TSaveGamePane::SanitizeName(const std::string& typed)
{
    std::string name;
    for (const char c : typed)
        if (c >= 0x20 && c <= 0x7e && !std::strchr(".\\/?*|<>:\"", c))
            name.push_back(c);
    return name;
}

// REVSYNC: 0x00539c00 event 5000 -- the slot's name into "nameedit" too.
void TSaveGamePane::OnListSelect(const SDefWidget& list, int32_t row)
{
    TSaveSlotPane::OnListSelect(list, row);
    const std::vector<SSaveSlot>& slots = ::SaveGame.Slots();
    if (list.name == "gamelist" && row >= 0 && row < static_cast<int32_t>(slots.size()))
        SetText("nameedit", slots[static_cast<size_t>(row)].name);
}

void TSaveGamePane::OnActivate(const SDefWidget& widget, int32_t buttonIndex)
{
    (void)buttonIndex;
    if (widget.name == "savegame")
    {
        const SDefWidget* edit = Find("nameedit");
        const std::string name = SanitizeName(edit ? edit->text : std::string());
        if (name.empty())
            return;
        savename = name;
        Finish(RESULT_DONE);
    }
    else if (widget.name == "exit")
        Finish(RESULT_EXIT);
}
