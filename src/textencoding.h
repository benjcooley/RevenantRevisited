// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *        textencoding.h - the byte encodings UI text arrives in         *
// *************************************************************************
//
// The text renderer (font.h) looks glyphs up by Unicode code point and
// decodes each string into code points here. See docs/ui/TEXT_RENDERING.md.
//
// *************************************************************************

#pragma once

#include <cstdint>

enum class ETextEncoding : uint8_t
{
    Cp1252,     // Windows-1252: the retail game's text (english.def, scripts, DEFs)
    Utf8,       // UTF-8: for the Revisited locale string packs
};

// Retail drew its text through GDI in code page 1252; every string the game
// data supplies is in it.
inline constexpr ETextEncoding kGameTextEncoding = ETextEncoding::Cp1252;

// What a malformed sequence decodes to.
inline constexpr char32_t kReplacementChar = U'�';

// The Unicode code point of a Windows-1252 byte. 0x80..0x9F hold CP1252's
// punctuation and letters (0x92 is U+2019); the rest equal ASCII and
// Latin-1. The five bytes CP1252 leaves undefined (0x81, 0x8D, 0x8F, 0x90,
// 0x9D) map to the C1 control of the same value, as Windows'
// MultiByteToWideChar does, so distinct bytes never share a code point.
[[nodiscard]] char32_t Cp1252ToUnicode(uint8_t byte);

// Decodes the character at `p` and advances `p` past it. `p` must not point
// at the string's terminating NUL. Malformed UTF-8 decodes to
// kReplacementChar and advances one byte, so a NUL is never stepped over.
[[nodiscard]] char32_t DecodeChar(const char*& p, ETextEncoding encoding);
