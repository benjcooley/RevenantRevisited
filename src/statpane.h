// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   statpane.h - FORSAKEN Stat Pane                     *
// *************************************************************************

#pragma once

#include "button.h"
#include "saferef.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class TMulti;
class TObjectInstance;
struct SFontAtlas;

// *************
// * TStatPane *
// *************

// REVSYNC: TStatPane @ 0x0065b140 (vtable 0x005a5ba0; d:\revenant\StatPane.cpp;
// docs/ui/forensics/StatPane_RETAIL.md, docs/ui/HUD_REBUILD.md §6f). The side
// panel's stats, in its upper slot: what statpane.def lays out for the bound
// object -- the main player's two pages (stats, skills), or an examined
// character's or item's sheet. The DEF picks a block by the object's class
// (and the format, "page1" / "page2"), and its block is a little program:
// a cursor that TEXTs and FIELDs move along a line, colours, invisible hot
// spots (BUTTON) and the object's portrait (ICON).
//
// Retail re-read the DEF on every paint; the port parses it once at
// Initialize into blocks of operations and runs the selected block on each
// compose. Retail repainted when a stat of the bound object changed (the
// stat-changed notification, 0x004735c0, which the port doesn't have); the
// port recomposes whenever anything the pane shows changes (Pulse).
class TStatPane final : public TButtonPane
{
  public:
    static constexpr int32_t kWidth = 0xbc;
    static constexpr int32_t kHeight = 0x132;

    TStatPane();
    ~TStatPane() override;
    TStatPane(const TStatPane&) = delete;
    TStatPane& operator=(const TStatPane&) = delete;

    bool Initialize() override;     // 0x00546b50
    void Close() override;          // 0x00546d70
    void Pulse() override;          // what it shows changed: recompose
    void MouseClick(int32_t button, int32_t x, int32_t y) override;    // 0x005493b0
    void OnControl(TButton *button, int32_t msg) override;             // 0x00549470

    // REVSYNC: 0x005496a0 -- shows `object` (none: the main player), on
    // "page1".
    void SetObject(TObjectInstance *object);
    // REVSYNC: 0x00488a30 -- the format the DEF's blocks are picked by.
    void SetFormat(const char *name);

    // The object shown: the one bound, or the main player when it is gone.
    [[nodiscard]] TObjectInstance *Object() const;
    [[nodiscard]] const std::string &Format() const { return format; }
    // A multipage field's page shown, and its last (retail +0x1ac / +0x1b0).
    [[nodiscard]] int32_t DescPage() const { return descpage; }

  protected:
    void ComposeBackground(int32_t target_w, int32_t target_h) override;    // 0x005491c0

  private:
    // The text alignment the DC has: TA_UPDATECP with left, right or
    // centre (SetTextAlign 1 / 3 / 7), or Fixed, plain TA_LEFT after a block
    // field (0): the cursor doesn't move along the text.
    enum class EAlign : uint8_t { Fixed, Left, Right, Center };
    struct SColor3 { uint8_t r = 0xff, g = 0xff, b = 0xff; };
    struct SOp;
    struct SBlock;
    struct SRun;
    struct SWalk;

    bool LoadLayout();
    [[nodiscard]] const SBlock *SelectBlock(TObjectInstance &object) const;
    void SyncButtons();
    void Walk(const SBlock &block, TObjectInstance &object, SWalk &walk);       // 0x005475e0
    bool Resolve(TObjectInstance &object, const char *field, std::string &text,
                 std::string &colour) const;                                    // 0x00547240
    [[nodiscard]] int32_t TextWidthOf(const std::string &text) const;
    [[nodiscard]] int32_t BlockHeight(const std::string &text, int32_t w, int32_t h) const;
    void DrawRun(const SRun &run, TObjectInstance &object, int32_t target_w, int32_t target_h) const;
    void DrawIcon(TObjectInstance &object, int32_t x, int32_t y, int32_t target_w, int32_t target_h) const;
    [[nodiscard]] static std::string Signature(const SWalk &walk);

    std::unique_ptr<TMulti> archive;            // StatsPane.dat (retail DAT_0065c6f4)
    std::shared_ptr<TMulti> icons;              // SpellIcons.dat
    TBitmap *chrome = nullptr;                  // "Stats"
    TBitmap *ringt = nullptr;                   // "RingT", round a character's portrait (+0x188)
    const SFontAtlas *font = nullptr;           // "Small"
    std::vector<SBlock> blocks;                 // statpane.def
    bool layoutvalid = false;                   // the DEF parsed (else "Invalid Stat Layout")

    TSafeRef<TObjectInstance> object;           // +0x198
    std::string format;                         // +0x19c
    const SBlock *buttonsfor = nullptr;         // the block whose hot spots exist (+0x1a8)
    int32_t descpage = 0;                       // +0x1ac
    int32_t desclast = 0;                       // +0x1b0
    std::string shown;                          // what the pane last composed (Shown)
};
