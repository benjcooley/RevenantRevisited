// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *        textencoding.cpp - the byte encodings UI text arrives in       *
// *************************************************************************
//
// See textencoding.h.
//
// *************************************************************************

#include "textencoding.h"

#include <array>

namespace {

// CP1252 0x80..0x9F. The undefined bytes keep their own value (C1 controls).
constexpr std::array<char16_t, 32> kCp1252High = {
    0x20AC, 0x0081, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,     // 0x80
    0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0x008D, 0x017D, 0x008F,     // 0x88
    0x0090, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,     // 0x90
    0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x009D, 0x017E, 0x0178,     // 0x98
};

char32_t DecodeUtf8(const char*& p)
{
    const auto* s = reinterpret_cast<const unsigned char*>(p);
    const unsigned char lead = s[0];
    int32_t length = 0;
    char32_t cp = 0;
    char32_t shortest = 0;      // the least code point this length may encode
    if (lead < 0x80)
    {
        ++p;
        return lead;
    }
    if ((lead & 0xE0) == 0xC0)
    {
        length = 2;
        cp = lead & 0x1F;
        shortest = 0x80;
    }
    else if ((lead & 0xF0) == 0xE0)
    {
        length = 3;
        cp = lead & 0x0F;
        shortest = 0x800;
    }
    else if ((lead & 0xF8) == 0xF0)
    {
        length = 4;
        cp = lead & 0x07;
        shortest = 0x10000;
    }
    else
    {
        ++p;
        return kReplacementChar;
    }
    for (int32_t i = 1; i < length; ++i)
    {
        if ((s[i] & 0xC0) != 0x80)      // also stops at the NUL
        {
            ++p;
            return kReplacementChar;
        }
        cp = (cp << 6) | (s[i] & 0x3F);
    }
    if (cp < shortest || cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF))
    {
        ++p;
        return kReplacementChar;
    }
    p += length;
    return cp;
}

}  // namespace

char32_t Cp1252ToUnicode(uint8_t byte)
{
    if (byte >= 0x80 && byte < 0xA0)
        return kCp1252High[byte - 0x80];
    return byte;
}

char32_t DecodeChar(const char*& p, ETextEncoding encoding)
{
    if (encoding == ETextEncoding::Utf8)
        return DecodeUtf8(p);
    return Cp1252ToUnicode(static_cast<uint8_t>(*p++));
}
