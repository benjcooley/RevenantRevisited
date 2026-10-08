// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  buysell.cpp - the shop (retail TBuySellScreen)                       *
// *************************************************************************
//
// REVSYNC: cls_0x5a5d64, the global at 0x0065a3b8. Every number below is a
// literal from the retail decomps in recon/discovered/buysell/; the spec is
// docs/ui/forensics/BuySellScreen_SPEC.md (section numbers in the comments).

#include "buysell.h"

#include "animation.h"
#include "character.h"
#include "dialog.h"
#include "display.h"
#include "font.h"
#include "fonttable.h"
#include "imagery.h"
#include "logging.h"
#include "object.h"
#include "player.h"
#include "playscreen.h"
#include "renderer.h"
#include "revenant.h"
#include "rules.h"
#include "savegame.h"
#include "surface.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <functional>

TBuySellPane BuySellPane;

namespace {

// ---- Assets and layout (spec §2, §4; pane-local) ----

constexpr const char* kDat        = "buysell.dat";
constexpr const char* kChrome     = "BuySellMain";
constexpr const char* kUpButton       = "BuySellUp";
constexpr const char* kDownButton     = "BuySellDown";
constexpr const char* kActivateButton = "BuySellActivate";
constexpr const char* kExitButton     = "BuySellExit";
constexpr int32_t kWidth = 452;                         // ctor 0x00488ef0: 0x1c4

constexpr const char* kGold = "GOLD";                   // 0x005e3d84 / 0x005e3dcc / 0x005e3dd4

struct SCell
{
    int32_t x = 0, y = 0, w = 0, h = 0;
};

constexpr SCell kGoldCell{0x7d, 0, 0x62, 0x2c};          // paint 0x0052f90a
constexpr SCell kActivateLabel{0, 0, 0x64, 0x2c};        // paint 0x0052f9ff
constexpr SCell kExitLabel{0x159, 0, 0x64, 0x2c};        // paint 0x0052fa39

// A row's parts, for visible row r (0x0052f040): name and price at
// 42 * (r + 1), the stats line 20 below, the description 32 below, the icon
// at 44 + 43 * r.
constexpr int32_t kRowPitch    = 0x2a;
constexpr SCell   kNameCell{0x3c, 0, 0x154, 0x14};
constexpr SCell   kPriceCell{0x164, 0, 0x44, 0x14};
constexpr SCell   kStatsCell{0x40, 0x3e - kRowPitch, 0x190, 0xc};
constexpr SCell   kDescriptionCell{0x40, 0x4a - kRowPitch, 0x190, 0xc};
constexpr int32_t kIconX       = 5;
constexpr int32_t kIconY       = 0x2c;
constexpr int32_t kIconPitch   = 0x2b;
constexpr int32_t kRowsPerPage = 3;                      // 0x0052f0b4: index % 3

// Mouse rows (0x0052fd50, 0x0052fca0): x below 200, y in one of these bands.
constexpr int32_t kRowClickMaxX = 199;
struct SBand
{
    int32_t top = 0, bottom = 0;
};
constexpr std::array<SBand, kRowsPerPage> kRowBands{{{0x2b, 0x55}, {0x57, 0x81}, {0x83, 0xad}}};

// ---- Colours (stored B, G, R; listed R, G, B) ----

struct SRgb
{
    uint8_t r = 0, g = 0, b = 0;
};
constexpr SRgb kGoldColor{0xff, 0xba, 0x00};             // paint 0x0052f7dd..eb
constexpr std::array<SRgb, 3> kRowColor{{
    {0x82, 0x0d, 0xc5},                                  // 0 plain
    {0xe6, 0x96, 0xff},                                  // 1 under the mouse
    {0xc8, 0x53, 0xff},                                  // 2 selected
}};
constexpr SRgb kDetailColor{0xa2, 0xa2, 0xa2};           // row 0x0052f193..9f

// ---- Classes (spec §6.3) ----

using TClassList = std::vector<int32_t>;

// Add (0x00530670), a misc shop: the first of these that has the type. Sell
// (0x0052ff40) looks a sold type up in the same order.
const TClassList kMiscAddClasses{
    OBJCLASS_ARMOR, OBJCLASS_WEAPON, OBJCLASS_FOOD, OBJCLASS_POTION,
    OBJCLASS_CONTAINER, OBJCLASS_INVCONTAINER, OBJCLASS_AMMO};
// AddCriteria (0x00530af0), a misc shop. Armor is walked twice (retail).
const TClassList kMiscCriteriaClasses{
    OBJCLASS_WEAPON, OBJCLASS_ARMOR, OBJCLASS_ARMOR, OBJCLASS_FOOD,
    OBJCLASS_POTION, OBJCLASS_AMMO, OBJCLASS_CONTAINER, OBJCLASS_INVCONTAINER};
// RemoveCriteria (0x00532340), a misc shop.
const TClassList kMiscRemoveClasses{
    OBJCLASS_WEAPON, OBJCLASS_ARMOR, OBJCLASS_FOOD, OBJCLASS_POTION,
    OBJCLASS_AMMO, OBJCLASS_CONTAINER, OBJCLASS_INVCONTAINER};
const TClassList kWeaponClass{OBJCLASS_WEAPON};
const TClassList kArmorClass{OBJCLASS_ARMOR};

// The classes a weapon / armor / misc shop works in: weapon first, then
// armor (the criteria walks test bit 8 before bit 4), else misc.
const TClassList& ShopClasses(uint32_t shoptype, const TClassList& misc)
{
    if (shoptype & TBuySellPane::kWeapon)
        return kWeaponClass;
    if (shoptype & TBuySellPane::kArmor)
        return kArmorClass;
    return misc;
}

// The inventory slots below this are the pack (and a container's own
// slots); 0x100 + slot is equipment, 0x10b + n the belt.
constexpr int32_t kFirstEquipSlot = 0x100;

// ---- Helpers ----

// A class stat of a type: 0 when the class has no such stat or the type
// doesn't carry it (retail reads the type record's stat array bounded by
// its length).
int32_t TypeStat(const TObjectClass& cl, int32_t objtype, const char* stat)
{
    const SObjectInfo* info = cl.GetObjType(objtype);
    const int32_t statid = cl.FindStat(stat);
    if (!info || statid < 0 || statid >= info->stats.NumItems())
        return 0;
    return info->stats[statid];
}

// REVSYNC: 0x0046e7f0 -- an item's display name: the name with everything
// but letters and digits removed, read as a dialog tag; a miss ("[...") keeps
// the name. Retail's buffer holds 99 characters.
std::string DisplayName(const char* name)
{
    constexpr size_t kMax = 99;
    std::string tag;
    for (const char* c = name; *c && tag.size() < kMax - 1; ++c)
        if (std::isalnum(static_cast<unsigned char>(*c)))
            tag.push_back(*c);
    const char* line = DialogList.GetLine(tag.c_str());
    std::string shown = (line && line[0] != '[') ? line : name;
    if (shown.size() > kMax)
        shown.resize(kMax);
    return shown;
}

// A label: the tag's line when the tag exists, else retail's literal.
std::string Label(const char* tag, const char* fallback)
{
    return DialogList.FindLine(tag) >= 0 ? DialogList.GetLine(tag) : fallback;
}

// REVSYNC: 0x0048cce0 -- a STATLINE as text. For each word, the line of tag
// STATCFG<word>, a space, the number after it (digits, '-', '+', '%'; spaces
// inside it skipped), a space. Retail stops copying a word's line when the
// input is used up, so a trailing word without a number shows nothing.
std::string ExpandStatLine(const std::string& statline)
{
    const size_t end = statline.size();
    size_t at = 0;
    auto skipSpaces = [&] { while (at < end && statline[at] == ' ') ++at; };
    auto isNumber = [](char c) { return std::isdigit(static_cast<unsigned char>(c)) || c == '-' || c == '+' || c == '%'; };

    std::string text;
    skipSpaces();
    while (at < end)
    {
        std::string tag = "STATCFG";
        while (at < end && statline[at] != ' ')
            tag.push_back(statline[at++]);
        const char* line = DialogList.GetLine(tag.c_str());
        skipSpaces();
        for (const char* c = line; *c && at < end; ++c)
            text.push_back(*c);
        text.push_back(' ');
        while (at < end && isNumber(statline[at]))
        {
            text.push_back(statline[at++]);
            skipSpaces();
        }
        text.push_back(' ');
        skipSpaces();
    }
    return text;
}

// REVSYNC: the ammo name of 0x0052da90 -- BSAMMO1 ("Quiver of NUMBER NAME's")
// with NUMBER the amount and NAME the display name.
std::string QuiverName(const std::string& name, int32_t amount)
{
    std::string text = Label("BSAMMO1", "Quiver of NUMBER NAME's");
    if (const size_t at = text.find("NUMBER"); at != std::string::npos)
        text.replace(at, 6, std::to_string(amount));
    if (const size_t at = text.find("NAME"); at != std::string::npos)
        text.replace(at, 4, name);
    return text;
}

// The item's type's Value; -1 is "not for sale". Retail asks vtable 0x190,
// which the item classes implement as their class stat Value (the base
// object answers 0); the port reads the class stat for every class.
int32_t ClassValue(TObjectInstance& item)
{
    const TObjectClass* cl = TObjectClass::GetClass(item.ObjClass());
    return cl ? TypeStat(*cl, item.ObjType(), "Value") : 0;
}

// REVSYNC: 0x00470070 -- equipment: slots 0x101 .. 0x10a.
bool IsEquipped(TObjectInstance& item)
{
    return item.InventNum() > kFirstEquipSlot && item.InventNum() < kFirstEquipSlot + 0xb;
}

// The customer's items, nested inventories included, each after the item
// holding it (retail's inventory iterator 0x0046dfb0 with flag 1).
void ForEachCarried(TObjectInstance& owner, const std::function<void(TObjectInstance&)>& visit)
{
    for (int32_t i = 0; i < owner.NumInventoryItems(); ++i)
        if (TObjectInstance* item = owner.GetInventory(i))
        {
            visit(*item);
            ForEachCarried(*item, visit);
        }
}

// REVSYNC: 0x005330a0 -- the item `name` to take from `owner`: the first that
// isn't equipped, looking inside containers (classes 5, 0x11) as it meets
// them; else the last equipped one seen.
TObjectInstance* FindItemToSell(TObjectInstance& owner, const char* name)
{
    TObjectInstance* equipped = nullptr;
    for (int32_t i = 0; i < owner.NumInventoryItems(); ++i)
    {
        TObjectInstance* item = owner.GetInventory(i);
        if (!item)
            continue;
        if (stricmp(item->GetName(), name) == 0)
        {
            if (!IsEquipped(*item))
                return item;
            equipped = item;
        }
        if (item->ObjClass() == OBJCLASS_CONTAINER || item->ObjClass() == OBJCLASS_INVCONTAINER)
            if (TObjectInstance* inside = FindItemToSell(*item, name))
                return inside;
    }
    return equipped;
}

// The state-0 inventory icon of an imagery (as TObjectInstance::InventoryImage
// 0x0046f190 reads it): the baked image, else frame 0 of the icon animation.
PTBitmap StateZeroIcon(TObjectImagery* imagery)
{
    if (!imagery)
        return nullptr;
    if (PTBitmap bm = imagery->GetInvImage(0))
        return bm;
    if (PTAnimation anim = imagery->GetInvAnimation(0))
        return anim->GetFrame(0);
    return nullptr;
}

// REVSYNC: 0x004be2b0 as the shop calls it: every call carries the shadow
// flag 0x400; 1 / 2 / 4 align left / centre / right; with 0x40 the text is
// one line centred in the cell's height, without it the text word-wraps from
// the cell's top and GDI clipped it to the cell, so only the lines that
// start inside the cell show.
void DrawCellText(const SFontAtlas* font, const char* text, const SCell& cell, int32_t rowy,
                  ETextAlign align, bool vcentre, const SRgb& color, TSurface& target)
{
    if (!font || !text || !*text)
        return;
    const float lineheight = TextLineHeight(font);
    const float r = color.r / 255.0f, g = color.g / 255.0f, b = color.b / 255.0f;
    const int32_t top = rowy + cell.y;
    if (vcentre)
    {
        const int32_t y = top + static_cast<int32_t>((cell.h - lineheight) * 0.5f + 0.5f);
        DrawTextShadowedToTarget(font, text, cell.x, y, cell.w, cell.h, align, r, g, b,
                                 target.Width(), target.Height());
        return;
    }
    std::vector<std::string> lines;
    WrapTextLines(font, text, static_cast<float>(cell.w), lines);
    for (size_t i = 0; i < lines.size(); ++i)
    {
        const int32_t offset = static_cast<int32_t>(lineheight * static_cast<float>(i));
        if (offset >= cell.h)
            break;
        DrawTextShadowedToTarget(font, lines[i].c_str(), cell.x, top + offset, cell.w, cell.h,
                                 align, r, g, b, target.Width(), target.Height());
    }
}

}  // namespace

void SBuySellItem::SImageryRelease::operator()(TObjectImagery* imagery) const
{
    TObjectImagery::FreeImagery(imagery);
}

// ===========================================================================
// Lifetime
// ===========================================================================

bool TBuySellPane::Initialize()
{
    active   = true;
    top      = 0;
    selected = -1;
    hover    = -1;
    if (built)
        return true;

    shown = kRowsPerPage;
    salesperson.Clear();
    purchasetag.clear();
    nogoldtag.clear();
    customer.Clear();
    if (!OpenChrome(0, Display.Height() - kHeight, kWidth, kHeight, kDat, kChrome))
    {
        log_error("[buysell] Trouble initializing buysell pane");
        return false;
    }
    // REVSYNC: 0x0052f390 -- the four buttons from buysell.dat at their
    // registration points, with their keys.
    struct SButton { const char* name; int32_t key; };
    for (const SButton& button : {SButton{kUpButton, VK_UP}, SButton{kDownButton, VK_DOWN},
                                  SButton{kActivateButton, 'A'}, SButton{kExitButton, 'E'}})
    {
        AddSpriteButton(button.name, button.name);
        SetHotKey(button.name, button.key);
    }
    built = true;
    return true;
}

void TBuySellPane::Close()
{
    // The rows go here too (retail freed them in the destructor): their
    // imagery references must be released before the imagery system is.
    items.clear();
    purchasetag.clear();
    nogoldtag.clear();
    built  = false;
    active = false;
    TDefPane::Close();
}

void TBuySellPane::Reset()
{
    items.clear();
    active = false;
    Hide();
}

void TBuySellPane::Clear()
{
    items.clear();
    SetDirty(true);
}

// ===========================================================================
// Script API
// ===========================================================================

void TBuySellPane::SetShopType(uint32_t type)
{
    shoptype = type;
    SetDirty(true);
}

void TBuySellPane::SetCustomer(TObjectInstance* who)
{
    customer = who;
    SetDirty(true);
}

void TBuySellPane::SetSalesperson(TObjectInstance* who)
{
    salesperson = who;
}

void TBuySellPane::SetPurchaseDialog(const char* tag)
{
    purchasetag = tag ? tag : "";
}

void TBuySellPane::SetNoGoldDialog(const char* tag)
{
    nogoldtag = tag ? tag : "";
}

// REVSYNC: 0x0052da90 (spec §6.2). False when no class has a type `name`.
bool TBuySellPane::BuildItem(const char* name, int32_t amount, SBuySellItem& item) const
{
    TObjectClass* cl = nullptr;
    int32_t objtype = -1;
    for (int32_t c = 0; c < TObjectClass::NumClasses() && objtype < 0; ++c)
        if ((cl = TObjectClass::GetClass(c)) != nullptr)
            objtype = cl->FindObjType(name);
    if (objtype < 0)
        return false;
    const SObjectInfo* info = cl->GetObjType(objtype);

    item.name   = DisplayName(name);
    item.type   = name;
    item.price  = TypeStat(*cl, objtype, "Value");
    item.amount = amount;
    item.numstats = 0;
    auto addStat = [&item](std::string label, int32_t value) {
        item.stats[size_t(item.numstats++)] = {std::move(label), value};
    };

    const int32_t objclass = cl->ClassId();
    if (const SItemData* data = Rules.GetItemData(objclass, name))
    {
        // The WEAPON.DEF / ARMOR.DEF entry (0x0048cb50): its description, or
        // its STATLINE as text, and BASICMODS in file order (rules.h).
        if (!data->description.empty() && DialogList.FindLine(data->description.c_str()) >= 0)
            item.description = DialogList.GetLine(data->description.c_str());
        else if (!data->statline.empty())
            item.description = ExpandStatLine(data->statline);
        const auto& mods = data->basicmods;
        if (objclass == OBJCLASS_WEAPON)
        {
            addStat(Label("BSWEA1", "Damage"), mods[2]);
            addStat(Label("BSWEA2", "Min Strength"), mods[7]);
        }
        else
        {
            addStat(Label("BSARM1", "Protection"), mods[1]);
            addStat(Label("BSARM2", "Rst Poison"), mods[3]);
            addStat(Label("BSARM3", "Stlth"), mods[4]);
            addStat(Label("BSARM4", "Min Strn"), mods[6]);
            addStat(Label("BSARM5", "Min Cons"), mods[7]);
        }
    }
    else
    {
        auto classStat = [&](const char* stat) { return TypeStat(*cl, objtype, stat); };
        switch (objclass)
        {
          case OBJCLASS_WEAPON:
            addStat(Label("BSWEA1", "Damage"), classStat("Damage"));
            addStat(Label("BSWEA2", "Min Strength"), classStat("MinStrength"));
            break;
          case OBJCLASS_ARMOR:
            addStat(Label("BSARM1", "Protection"), classStat("Protection"));
            addStat(Label("BSARM2", "Rst Poison"), classStat("ResistPoison"));
            addStat(Label("BSARM3", "Stlth"), classStat("Stealth"));
            addStat(Label("BSARM4", "Min Strn"), classStat("MinStrength"));
            // Retail looks up BSARM4 again for the fifth (0x0052e4a5).
            addStat(Label("BSARM4", "Min Cons"), classStat("MinConstitution"));
            break;
          case OBJCLASS_FOOD:
          case OBJCLASS_POTION:
            addStat(Label("BSFOOD1", "Cure Health"), classStat("Health"));
            addStat(Label("BSFOOD2", "Cure Mana"), classStat("Mana"));
            addStat(Label("BSFOOD3", "Cure Fatigue"), classStat("Fatigue"));
            addStat(Label("BSFOOD4", "Cure Poison"), classStat("Poison"));
            break;
          case OBJCLASS_AMMO:
            item.name = QuiverName(item.name, amount);
            break;
          default:
            break;
        }
    }

    item.imagery.reset(TObjectImagery::LoadImagery(info->imageryid));
    // REVSYNC: 0x0052efe4 -- a Sell shop pays __ftol(Value * 0.3f).
    if (shoptype & kSell)
        item.price = static_cast<int32_t>(static_cast<double>(item.price) * static_cast<double>(0.3f));
    return true;
}

// REVSYNC: the sale rule of 0x00530670 / 0x00530af0 (spec §6.3): class stat
// SaleType 0 is stocked, 1 only once the player has sold one (the save's
// merchant table, 0x0048e630), anything else never.
bool TBuySellPane::Stocks(const TObjectClass& cl, int32_t objtype) const
{
    const int32_t saletype = TypeStat(cl, objtype, "SaleType");
    if (saletype == 0)
        return true;
    return saletype == 1 && ::SaveGame.HasSoldUnique(cl.ClassId(), objtype);
}

void TBuySellPane::Append(SBuySellItem&& item)
{
    items.push_back(std::move(item));
    SetDirty(true);
}

void TBuySellPane::RemoveAt(int32_t index)
{
    items.erase(items.begin() + index);
    SetDirty(true);
}

// REVSYNC: 0x00530670.
void TBuySellPane::Add(const char* type, int32_t amount)
{
    // A misc shop searches its classes; otherwise armor (bit 4) or weapons.
    const TClassList& classes = (shoptype & kMisc) ? kMiscAddClasses
                              : (shoptype & kArmor) ? kArmorClass : kWeaponClass;
    for (const int32_t objclass : classes)
    {
        const TObjectClass* cl = TObjectClass::GetClass(objclass);
        const int32_t objtype = cl ? cl->FindObjType(type) : -1;
        if (objtype < 0)
            continue;
        SBuySellItem item;
        if (Stocks(*cl, objtype) && BuildItem(type, amount, item))
            Append(std::move(item));
        return;
    }
}

// REVSYNC: 0x00530af0.
void TBuySellPane::AddCriteria(const char* stat, int32_t min, int32_t max)
{
    for (const int32_t objclass : ShopClasses(shoptype, kMiscCriteriaClasses))
    {
        const TObjectClass* cl = TObjectClass::GetClass(objclass);
        if (!cl)
            continue;
        for (int32_t objtype = 0; objtype < cl->NumTypes(); ++objtype)
        {
            const SObjectInfo* info = cl->GetObjType(objtype);
            if (!info)
                continue;
            const int32_t value = TypeStat(*cl, objtype, stat);
            if (value < min || value > max || !Stocks(*cl, objtype))
                continue;
            SBuySellItem item;
            if (BuildItem(info->name, 1, item))
                Append(std::move(item));
        }
    }
}

// REVSYNC: the class test of 0x00531b70 / 0x00531d70 / 0x00531fc0.
bool TBuySellPane::SellsToShop(TObjectInstance& item) const
{
    switch (item.ObjClass())
    {
      case OBJCLASS_WEAPON:
        return shoptype & kWeapon;
      case OBJCLASS_ARMOR:
        return shoptype & (kArmor | kMisc);
      case OBJCLASS_FOOD:
      case OBJCLASS_CONTAINER:
      case OBJCLASS_INVCONTAINER:
      case OBJCLASS_POTION:
      case OBJCLASS_AMMO:
        return shoptype & kMisc;
      default:
        return false;
    }
}

// REVSYNC: 0x00531b70 / 0x00531d70 / 0x00531fc0 -- one row per item the
// customer carries in a pack slot, that may be sold (SaleType not 2, Value
// not -1) and that the shop deals in.
void TBuySellPane::AddCustomerItems(const SCustomerFilter& filter)
{
    TObjectInstance* who = customer.Get();
    if (!who)
        return;
    ForEachCarried(*who, [&](TObjectInstance& item) {
        if (filter.name && std::strcmp(item.GetName(), filter.name) != 0)
            return;
        if (item.InventNum() >= kFirstEquipSlot || item.GetStat("SaleType") == 2)
            return;
        if (ClassValue(item) == -1 || !SellsToShop(item))
            return;
        if (filter.stat)
        {
            const int32_t value = item.GetStat(filter.stat);
            if (value < filter.min || value > filter.max)
                return;
        }
        SBuySellItem row;
        if (BuildItem(item.GetName(), item.Amount(), row))
            Append(std::move(row));
    });
}

void TBuySellPane::AddBuyItems()
{
    AddCustomerItems({});
}

void TBuySellPane::AddBuyItem(const char* name)
{
    AddCustomerItems({name, nullptr, 0, 0});
}

void TBuySellPane::AddBuyCriteria(const char* stat, int32_t min, int32_t max)
{
    AddCustomerItems({nullptr, stat, min, max});
}

// REVSYNC: 0x00532210 -- the rows of that display name (compared exactly).
void TBuySellPane::Remove(const char* name)
{
    const auto gone = std::remove_if(items.begin(), items.end(),
                                     [name](const SBuySellItem& item) { return item.name == name; });
    if (gone != items.end())
    {
        items.erase(gone, items.end());
        SetDirty(true);
    }
}

// REVSYNC: 0x00532340 (and buysellremovebuycriteria's 0x005321f0) -- for each
// class of the shop that has the stat, the rows whose display name names a
// type of that class with the stat in [min, max].
void TBuySellPane::RemoveCriteria(const char* stat, int32_t min, int32_t max)
{
    for (const int32_t objclass : ShopClasses(shoptype, kMiscRemoveClasses))
    {
        const TObjectClass* cl = TObjectClass::GetClass(objclass);
        if (!cl || cl->FindStat(stat) < 0)
            continue;
        const auto gone = std::remove_if(items.begin(), items.end(), [&](const SBuySellItem& item) {
            const int32_t objtype = cl->FindObjType(item.name.c_str());
            if (objtype < 0)
                return false;
            const int32_t value = TypeStat(*cl, objtype, stat);
            return value >= min && value <= max;
        });
        if (gone != items.end())
        {
            items.erase(gone, items.end());
            SetDirty(true);
        }
    }
}

// ===========================================================================
// Paint (spec §5)
// ===========================================================================

void TBuySellPane::LayOut()
{
    const int32_t y = Display.Height() - kHeight;
    if (GetPosX() != 0 || GetPosY() != y)
        Resize(0, y, kWidth, kHeight);
}

void TBuySellPane::Pulse()
{
    TDefPane::Pulse();
    LayOut();
    // The gold is read when the panel paints; recompose when it changes.
    if (TObjectInstance* who = customer.Get())
        if (who->GetInventoryAmount(kGold) != goldshown)
            SetDirty(true);
}

void TBuySellPane::Compose()
{
    if (!IsDirty())
        return;
    TDefPane::Compose();
    SetDirty(false);
}

// REVSYNC: Paint @ 0x0052f7d0.
void TBuySellPane::Paint()
{
    TSurface& target = *Surface();
    const SFontAtlas* large = FontTable ? FontTable->Atlas("Large") : nullptr;   // 0x00667540

    PaintBackground();

    goldshown = -1;
    if (TObjectInstance* who = customer.Get())
    {
        goldshown = who->GetInventoryAmount(kGold);
        char text[64];
        std::snprintf(text, sizeof(text), "%s %d", DialogList.GetLine("BSGOLD"), goldshown);
        DrawCellText(large, text, kGoldCell, 0, ETextAlign::Left, true, kGoldColor, target);
    }

    const int32_t count = static_cast<int32_t>(items.size());
    for (int32_t index = top; index < (std::min)(top + shown, count); ++index)
        DrawRow(index, index == hover ? 1 : index == selected ? 2 : 0);

    PaintWidgets();

    if (shoptype & (kBuy | kSell))
        DrawCellText(large, DialogList.GetLine((shoptype & kBuy) ? "BSBUY" : "BSSELL"), kActivateLabel, 0,
                     ETextAlign::Center, true, kGoldColor, target);
    DrawCellText(large, DialogList.GetLine("BSEXIT"), kExitLabel, 0, ETextAlign::Center, true, kGoldColor,
                 target);
}

// REVSYNC: 0x0052f040. `state`: 0 plain, 1 under the mouse, 2 selected.
void TBuySellPane::DrawRow(int32_t index, int32_t state)
{
    const SBuySellItem& item = items[size_t(index)];
    TSurface& target = *Surface();
    const SFontAtlas* large = FontTable ? FontTable->Atlas("Large") : nullptr;   // 0x00667540
    const SFontAtlas* small = FontTable ? FontTable->Atlas("Small") : nullptr;   // 0x0065abc4
    const int32_t row  = index % kRowsPerPage;
    const int32_t rowy = (row + 1) * kRowPitch;
    const SRgb& color  = kRowColor[size_t(state)];

    DrawCellText(large, item.name.c_str(), kNameCell, rowy, ETextAlign::Left, false, color, target);

    char text[512];
    std::snprintf(text, sizeof(text), "%d%s", item.price, DialogList.GetLine("BSGP"));
    DrawCellText(large, text, kPriceCell, rowy, ETextAlign::Right, false, color, target);

    std::string stats;
    for (int32_t i = 0; i < item.numstats; ++i)
    {
        const SBuySellItem::SStat& stat = item.stats[size_t(i)];
        if (stat.value == 0)
            continue;
        std::snprintf(text, sizeof(text), "%s: %+d  ", stat.label.c_str(), stat.value);
        stats += text;
    }
    DrawCellText(small, stats.c_str(), kStatsCell, rowy, ETextAlign::Left, false, kDetailColor, target);
    DrawCellText(small, item.description.c_str(), kDescriptionCell, rowy, ETextAlign::Left, false,
                 kDetailColor, target);

    if (PTBitmap icon = StateZeroIcon(item.imagery.get()))
        Renderer->DrawBitmapToTarget(icon, kIconX, kIconY + row * kIconPitch, target.Width(), target.Height());
}

// ===========================================================================
// Input (spec §6.5, §10)
// ===========================================================================

int32_t TBuySellPane::RowAt(int32_t x, int32_t y) const
{
    if (x > kRowClickMaxX)
        return -1;
    for (size_t row = 0; row < kRowBands.size(); ++row)
        if (y >= kRowBands[row].top && y <= kRowBands[row].bottom)
            return static_cast<int32_t>(row);
    return -1;
}

void TBuySellPane::ClampSelection()
{
    const int32_t count = static_cast<int32_t>(items.size());
    if (selected >= count)
        selected = count - 1;
}

void TBuySellPane::MouseClick(int32_t button, int32_t x, int32_t y)
{
    TDefPane::MouseClick(button, x, y);                 // the buttons (0x00436530)
    if ((button == MB_LEFTUP || button == MB_RIGHTUP) && x <= kRowClickMaxX)
    {
        if (const int32_t row = RowAt(x, y); row >= 0)
        {
            selected = top + row;
            PlayClick();
        }
        ClampSelection();
    }
    SetDirty(true);
}

void TBuySellPane::MouseMove(int32_t button, int32_t x, int32_t y)
{
    TDefPane::MouseMove(button, x, y);                  // 0x00436660
    hover = -1;
    if (x <= kRowClickMaxX)
    {
        if (const int32_t row = RowAt(x, y); row >= 0)
            hover = top + row;
        hover = (std::min)(hover, static_cast<int32_t>(items.size()) - 1);
    }
    SetDirty(true);
}

void TBuySellPane::KeyPress(int32_t key, bool down)
{
    if (down)
    {
        if (key >= '1' && key <= '3')
        {
            selected = top + (key - '1');
            SetDirty(true);
        }
        ClampSelection();
    }
    // Retail keeps B and V (the panel toggles) from the button pane.
    if (key == 'B' || key == 'V' || key == 'b' || key == 'v')
        return;
    TDefPane::KeyPress(key, down);                      // 0x004361f0: the buttons' keys
}

void TBuySellPane::Joystick(int32_t key, bool down)
{
    if (down)
    {
        const int32_t count = static_cast<int32_t>(items.size());
        switch (key)
        {
          case VK_JOYUP:
            if (--selected < top && (top -= shown) < 0)
                top = 0;
            selected = (std::max)(selected, 0);
            break;
          case VK_JOYDOWN:
            if (++selected >= top + shown)
            {
                top += shown;
                if (top > count - 1)
                    top -= shown;
            }
            if (selected >= count)
                selected = count - 1;
            break;
          case VK_JOYBUTTON1:
            Transact();
            break;
          case VK_JOYBUTTON2:
            Exit();
            break;
          default:
            break;
        }
        SetDirty(true);
    }
    TDefPane::Joystick(key, down);
}

void TBuySellPane::OnActivate(const SDefWidget& widget, int32_t /*buttonIndex*/)
{
    if (widget.name == kUpButton)
        ScrollUp();
    else if (widget.name == kDownButton)
        ScrollDown();
    else if (widget.name == kActivateButton)
        Transact();
    else if (widget.name == kExitButton)
        Exit();
}

// REVSYNC: 0x0052fe90.
void TBuySellPane::ScrollUp()
{
    top = (std::max)(top - shown, 0);
    selected = hover = -1;
    SetDirty(true);
}

// REVSYNC: 0x0052fee0.
void TBuySellPane::ScrollDown()
{
    top += shown;
    if (top > static_cast<int32_t>(items.size()) - 1)
        top -= shown;
    selected = hover = -1;
    SetDirty(true);
}

// REVSYNC: 0x00530570 -- the shop is done: the script's `wait buysell` ends,
// and PlayScreen closes the drawer on its next pulse.
void TBuySellPane::Exit()
{
    active = false;
    items.clear();
    PlayScreen.RequestBuySell(false);
}

// ===========================================================================
// Buying and selling (spec §6.4)
// ===========================================================================

// REVSYNC: 0x0052ff40.
void TBuySellPane::Transact()
{
    if (selected >= 0 && selected < static_cast<int32_t>(items.size()) && customer)
    {
        if (shoptype & kBuy)
            Buy(items[size_t(selected)]);
        else if (shoptype & kSell)
            Sell(selected);
    }
    SetDirty(true);
}

// The stock is endless and the selection stays.
void TBuySellPane::Buy(const SBuySellItem& item)
{
    TObjectInstance* who = customer.Get();
    if (who->GetInventoryAmount(kGold) < item.price)
    {
        Say(nogoldtag);
        return;
    }
    if (!who->HasEmptySlot())
        return;
    const bool misc = shoptype & kMisc;
    const std::string& type = (misc && item.type.empty()) ? item.name : item.type;
    who->AddToInventory(type.c_str(), misc ? item.amount : 1, -1);
    who->DeleteFromInventory(kGold, item.price);
    log_info("[buysell] bought %s for %d", type.c_str(), item.price);
    Say(purchasetag);
}

void TBuySellPane::Sell(int32_t index)
{
    const SBuySellItem& item = items[size_t(index)];

    // A unique type (SaleType 1) sold for the first time joins the save's
    // merchant table: from now on the shops that list it stock it.
    for (const int32_t objclass : kMiscAddClasses)
    {
        const TObjectClass* cl = TObjectClass::GetClass(objclass);
        const int32_t objtype = cl ? cl->FindObjType(item.type.c_str()) : -1;
        if (objtype < 0)
            continue;
        if (TypeStat(*cl, objtype, "SaleType") == 1 && !::SaveGame.HasSoldUnique(objclass, objtype))
            ::SaveGame.AddSoldUnique(objclass, objtype);
        break;
    }

    TObjectInstance* who = customer.Get();
    if (TObjectInstance* sold = FindItemToSell(*who, item.type.c_str()))
    {
        // Retail flags the item OF_KILL and takes it out of the inventory;
        // the port deletes it the way DeleteFromInventory does.
        sold->RemoveFromInventory();
        delete sold;
        who->AddToInventory(kGold, item.price, -1);
        if (who->ObjClass() == OBJCLASS_PLAYER)
            static_cast<TPlayer*>(who)->RefreshEquip();      // 0x00519230
        log_info("[buysell] sold %s for %d", item.type.c_str(), item.price);
    }
    RemoveAt(index);
    selected = -1;
}

// The salesperson says a line (SayTag 0x004d0a20) unless it is already
// saying one (its action is ACTION_SAY, 0x10).
void TBuySellPane::Say(const std::string& tag) const
{
    TObjectInstance* who = salesperson.Get();
    if (tag.empty() || !who || !who->IsCharacter())
        return;
    TCharacter* speaker = static_cast<TCharacter*>(who);
    if (!speaker->IsDoing(ACTION_SAY))
        speaker->SayTag(tag.c_str(), -1, nullptr);
}
