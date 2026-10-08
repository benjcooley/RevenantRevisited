# UI Text Rendering

How the port draws UI text: the one glyph walk, the two kinds of font
atlas, and how text bytes become glyphs. Code: `src/font.{h,cpp}`,
`src/ttfatlas.cpp`, `src/textencoding.{h,cpp}`, `TFontTable::Atlas`
(`src/fonttable.cpp`).

## 1. One glyph walk

Every UI string is measured, wrapped and drawn by the functions in
`font.h`: `TextWidth`, `TextAscent`, `TextLineHeight`, `WrapTextLines`,
`DrawTextAtBaseline`, `DrawTextToTarget`, `DrawTextShadowedToTarget`,
`DrawTextShadowedAtBaseline`. They all step through the string the same
way (`ForEachGlyph` in `font.cpp`): decode a character, look its glyph up
in the atlas, use the glyph's metrics. A pane never walks glyphs itself.

An atlas (`SFontAtlas`) is a font's glyphs baked into one texture, with
stb-style metrics per glyph (`xoff/yoff/xoff2/yoff2/xadvance`), so the walk
draws both kinds the same way:

| Kind | Built by | Source | Used for |
|---|---|---|---|
| bitmap | `BuildFontAtlas(TFont*)` | the retail BMFONT glyphs (`sysfont`, `scrlfont`, `smallgold`, `medgold`) | FONT.DEF `BMFONT` entries |
| TrueType | `BuildTTFAtlas(path, px)` | Arimo (for Arial) or Tinos (for Times New Roman), 2x2 oversampled, metrics rounded to whole pixels | FONT.DEF `WINFONT` entries, DEF screens, HUD panels |

## 2. Text encoding

Retail drew its WINFONT faces through GDI (`DrawTextA`) in code page 1252.
The game's text is Windows-1252 bytes: `english.def` (the base list in
`Resources` and each module's list) uses `0x92` for the apostrophe in
"you’ll" and "Gina’s", and a few other characters outside ASCII (survey,
§5). The port must draw those bytes as GDI did.

**The rule.** A text byte is a Windows-1252 character. `Cp1252ToUnicode`
(`textencoding.h`) maps it to a Unicode code point: `0x00..0x7F` and
`0xA0..0xFF` map to the same value (ASCII and Latin-1); `0x80..0x9F` map to
CP1252's 27 punctuation marks and letters (`0x92` is U+2019). The five
bytes CP1252 leaves undefined (`0x81 0x8D 0x8F 0x90 0x9D`) map to the C1
control of the same value, as Windows' `MultiByteToWideChar` does. So every
byte has one code point and no two bytes share one.

**Atlases are keyed by code point.** `SFontAtlas::Glyph(char32_t)` is the
only lookup. It answers every code point: one the atlas doesn't hold
draws the atlas's fallback glyph (glyph 0). There is no byte-indexed table
left to read out of range.

**The decoder.** The walk turns bytes into code points with
`DecodeChar(p, encoding)`. Every text function takes a trailing
`ETextEncoding` that defaults to `kGameTextEncoding` (CP1252), so the
retail data needs nothing at the call sites. `ETextEncoding::Utf8` is
there for the planned Revisited locale packs
([project localization](ARCHITECTURE.md#4-localization)): a caller holding
UTF-8 text passes it, and the same walk draws it. Malformed UTF-8 decodes
to U+FFFD. Wrapping splits on the bytes `' '` and `'\n'`, which never occur
inside a UTF-8 sequence, so `WrapTextLines` serves both encodings. A
locale with characters beyond the atlas repertoire (§3) needs the atlas
to grow too (more ranges, or glyphs added on demand); the lookup by code
point is already what that needs.

## 3. What each atlas holds

**TrueType.** Glyph 0 is the face's `.notdef` glyph (the empty box a
face draws for a character it lacks), then every printable character
the CP1252 rule produces: U+0020..U+007E, the 27 CP1252 specials and
U+00A0..U+00FF, 218 in all. Arimo and Tinos map all 218. A character the
face lacks would draw `.notdef` (and is logged at build). `.notdef` is
packed through stb by asking for U+FFFF, a noncharacter no face maps, so
`stbtt_FindGlyphIndex` returns glyph 0 for it. Control characters
(`'\n'`, C1 controls, the undefined bytes) are not in the atlas and draw
`.notdef`; `WrapTextLines` turns control bytes into spaces and breaks
lines at `'\n'`, so wrapped text never reaches the walk with them.

**Bitmap.** The retail fonts are drawn by byte, as retail drew them:
the glyph for byte `b` is registered under `Cp1252ToUnicode(b)`, which
the CP1252 decode maps straight back, so every byte reaches the same
glyph as before. Glyph 0 is empty with no advance: retail's `TFont`
drew nothing for a byte outside the font. `sysfont` and `scrlfont`
cover bytes `0x20..0x7B` and `smallgold` `0x20..0x8A` (`--test=font`), so
a `0x92` in book or scroll text draws nothing in Classic, as in retail.

**Line metrics.** `TextAscent` / `TextLineHeight` read `ascent` and
`descent`, set once at build. TrueType takes them from the printable ASCII
glyphs (U+0020..U+007E), as before the repertoire grew, so the new
glyphs move no baseline: an accented capital rises above `ascent`
instead of pushing every line down. Bitmap takes them from all its
glyphs.

**Atlas size.** The builder adds up the oversampled glyph boxes (plus
padding), picks a 512-wide atlas (wider when the area needs it) with the
smallest power-of-two height that holds 1.6 times that area, and grows
the atlas if stb still can't pack every glyph (logged as a warning).
stb's built-in row packer fills 60–70% of an atlas, so 1.6 packs Arimo
and Tinos first time at every size from 8 to 64 px. Packing takes 2–4 ms
per face at UI sizes, unoptimized. The sizes the game builds:

| Face @ px | Atlas (RGBA8) | Before (ASCII only) |
|---|---|---|
| Arimo 10 | 512×128 (256 KiB) | 512×128 |
| Arimo 11, 12, 14, 16 | 512×256 (512 KiB) | 512×128 / 256 |
| Tinos 18, 20 | 512×512 (1 MiB) | 512×256 |
| 28 | 1024×512 | 512×256 |
| 48 | 1024×1024 | 1024×1024 |

The `[ttf] atlas built` log line gives each atlas's size, glyph count and
line metrics.

## 4. What stays as it was

- ImGui and the editor use their own UTF-8 font path (`imgui`), not
  these atlases.
- The legacy software `TextDraw` (`graphics.cpp`) indexes `TFont` by
  byte, as retail did.
- Text input in DEF edit fields accepts ASCII only (`TDefPane::CharPress`).
- An accented capital (É, Š) in Arimo 12 reaches 9.3–9.4 px above the
  baseline, past the top of the text bar's 12 px line slot (baseline at 9),
  so its accent loses its top pixel row. No shipped text has one; the
  lowercase è (7.9 px) fits.

Verification: `--test=ui-textbar` prints `I2MIY01` ("I’ve") and
`XII12NAV01` ("crème") at ticks 6 and 12; in game,
`--quickstart --exec "sleep 48; player.say I4TEN07; player.say XII12NAV01"`
draws Tendrick's "you’ll" and "crème" in the Dialog font (Tinos 20).

## 5. Survey: non-ASCII bytes in the shipped text

From the GOG data the game reads (`resources.rvr`, `Modules/Ahkuilon.rvm`,
`imagery.rvi`, loose `Resources/` and `Modules/Demo/`), every `.def` and `.s`:

| Byte | Char | Where |
|---|---|---|
| `0x92` | ’ U+2019 | `Resources/english.def` (62: item and creature descriptions, scroll text), `Ahkuilon/english.def` (2: `I2MIY01`, Tendrick's `I4TEN07`), `Demo/english.def` (63) |
| `0x91` | ‘ U+2018 | `Resources/english.def` (1: `ADVSCRL6`), `Demo/english.def` (1) |
| `0xE8` | è U+00E8 | `Ahkuilon/english.def` (1: `XII12NAV01` "crème de menthe"), `Demo/english.def` (1) |
| `0x85` | … U+2026 | `Demo/english.def` (1) |
| `0x93` `0x94` | “ ” U+201C/D | `Demo/english.def` (1 each) |

No `.s` script, `font.def`, `class.def`, `char.def`, `weapon.def`,
`armor.def`, `spell.def` or DEF screen holds a byte ≥ `0x80`. The only
control character inside quoted strings is `\n` (written `\n` in the files;
the parser turns it into `0x0A`).
