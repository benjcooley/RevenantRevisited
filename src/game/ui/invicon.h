// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *         invicon.h - An item's inventory icon, as images to put        *
// *************************************************************************

#pragma once

#include <array>
#include <cstdint>

class TBitmap;

// One image of an item's inventory icon, put at (x, y) of the icon.
struct SInvIconPart
{
    TBitmap* image = nullptr;
    int32_t x = 0;
    int32_t y = 0;
};

// An item's inventory icon: its images put in order, each keyed by its own
// key, the whole clipped to INVITEMREALWIDTH x INVITEMREALHEIGHT. Most items
// have one image, their inventory image; ammo and money stack copies of
// theirs (TAmmo, TMoney::InventoryIcon), which retail composed into a bitmap
// of its own and the port draws as they are.
struct SInvIcon
{
    static constexpr int32_t kMaxParts = 64;    // a pile of 64 coins

    std::array<SInvIconPart, kMaxParts> parts{};
    int32_t count = 0;

    void Add(TBitmap* image, int32_t x, int32_t y)
        { if (image && count < kMaxParts) parts[count++] = { image, x, y }; }
    [[nodiscard]] bool Empty() const { return count == 0; }
    // One image at the icon's corner: the image is the icon.
    [[nodiscard]] bool Single() const { return count == 1 && parts[0].x == 0 && parts[0].y == 0; }
    [[nodiscard]] bool operator==(const SInvIcon& other) const;
    [[nodiscard]] bool operator!=(const SInvIcon& other) const { return !(*this == other); }
};
