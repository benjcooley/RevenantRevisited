// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *      hudab.cpp - the port's side of the HUD's retail A/B dumps        *
// *************************************************************************
//
// See hudab.h.

#include "hudab.h"

#include "bitmap.h"

#include <cstdint>
#include <cstring>
#include <memory>

namespace HudAB
{
namespace
{

bool Hex(const std::string& hex, std::vector<uint8_t>& out)
{
    if (hex.size() % 2)
        return false;
    out.resize(hex.size() / 2);
    for (size_t i = 0; i < out.size(); ++i)
    {
        char* end = nullptr;
        const std::string byte = hex.substr(i * 2, 2);
        out[i] = uint8_t(std::strtoul(byte.c_str(), &end, 16));
        if (end != byte.c_str() + 2)
            return false;
    }
    return true;
}

std::string ToHex(const uint8_t* data, size_t size)
{
    static const char digits[] = "0123456789abcdef";
    std::string out(size * 2, '0');
    for (size_t i = 0; i < size; ++i)
    {
        out[i * 2] = digits[data[i] >> 4];
        out[i * 2 + 1] = digits[data[i] & 15];
    }
    return out;
}

uint32_t FormatFlags(const std::string& name)
{
    if (name == "565")
        return BM_16BIT;
    if (name == "555")
        return BM_15BIT;
    if (name == "4444")
        return BM_ARGB4444;
    if (name == "1555")
        return BM_ARGB1555;
    return 0;
}

struct SBitmapDelete
{
    void operator()(TBitmap* bitmap) const { delete[] reinterpret_cast<uint8_t*>(bitmap); }
};

} // namespace

std::string DrawPut(const std::vector<std::string>& f, std::string& error)
{
    if (f.size() < 8)
    {
        error = "draw-put: need format, width, height, pixels, bitmap, x, y, mode";
        return {};
    }
    const uint32_t flags = FormatFlags(f[0]);
    const int32_t width = std::stoi(f[1]);
    const int32_t height = std::stoi(f[2]);
    std::vector<uint8_t> pixels, source;
    if (!flags || width < 1 || height < 1 || !Hex(f[3], pixels) || !Hex(f[4], source) ||
        pixels.size() != size_t(width) * height * 2 || source.size() < sizeof(TBitmapData))
    {
        error = "draw-put: bad format, size, pixels or bitmap";
        return {};
    }

    std::unique_ptr<TBitmap, SBitmapDelete> dest(TBitmap::NewBitmap(width, height, flags));
    if (!dest)
    {
        error = "draw-put: NewBitmap failed";
        return {};
    }
    memcpy(dest->data16, pixels.data(), pixels.size());

    // The source is a retail TBitmap's bytes: TBitmapData has the same layout
    // (relative block offsets), so it is used in place.
    auto* bitmap = reinterpret_cast<TBitmap*>(source.data());
    dest->Put(std::stoi(f[5]), std::stoi(f[6]), bitmap, int32_t(std::stoul(f[7])));

    return "{\"pixels\":\"" + ToHex(reinterpret_cast<const uint8_t*>(dest->data16), pixels.size()) + "\"}";
}

} // namespace HudAB
