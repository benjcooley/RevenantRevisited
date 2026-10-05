// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  cmd_buysell.cpp - the buysell* commands                              *
// *************************************************************************
//
// The buy/sell family of the retail command table (docs/gameflow/forensics/
// COMMAND_SYSTEM.md §6.6; decomps in recon/discovered/commands/cmd_buysell*).
// Each handler parses its line as retail's does and calls the shop
// (TBuySellPane, src/buysell.h) or the play screen. The interpreter and the
// table stay in command.cpp (ARCHITECTURE.md §6.1).

#include "command.h"

#include "buysell.h"
#include "mappane.h"
#include "object.h"
#include "playscreen.h"
#include "revenant.h"
#include "script.h"

#include <string>

namespace {

// Text or an identifier: the names and tags these commands take.
bool IsName(const TToken& t)
{
    return t.Type() == TKN_TEXT || t.Type() == TKN_IDENT;
}

// One bound of a criteria command: a number, or the target's prototype number
// variable of that name (0x00497800; an undeclared one is retail's
// -20000000, STATE_INVALID). Retail steps onto it with Get + WhiteGet.
bool ReadBound(TToken& t, const TObjectInstance* target, const char* which, int32_t& value)
{
    t.Get();
    t.WhiteGet();
    if (t.Type() == TKN_NUMBER)
    {
        value = t.Index();
        return true;
    }
    if (t.Type() != TKN_IDENT)
    {
        Output("Invalid Params");
        return false;
    }
    value = ScriptManager.VariableNumber(t.Text(), target);
    if (value == STATE_INVALID)
    {
        Output("Invalid %s value", which);
        return false;
    }
    return true;
}

// The three criteria commands' line: `<stat> <min> <max>`. Retail copies the
// stat name into a 32-byte buffer (overrunning it on a longer name).
struct SCriteria
{
    std::string stat;
    int32_t     min = 0;
    int32_t     max = 0;
};

bool ReadCriteria(TToken& t, const TObjectInstance* target, SCriteria& criteria)
{
    constexpr size_t kStatNameMax = 31;
    if (!IsName(t))
    {
        Output("Name required");
        return false;
    }
    criteria.stat = std::string(t.Text()).substr(0, kStatNameMax);
    return ReadBound(t, target, "min", criteria.min) && ReadBound(t, target, "max", criteria.max);
}

}  // namespace

// REVSYNC: buysellinit @ 0x00427080 -> 0x0052f390.
COMMAND(CmdBuySellInit)
{
    BuySellPane.Initialize();
    return 0;
}

// REVSYNC: buysellsalesperson @ 0x004279f0 -> 0x00532fb0: the closest object
// of that name, measured from the target.
COMMAND(CmdBuySellSalesPerson)
{
    if (!IsName(t))
        return CMD_BADPARAMS;
    BuySellPane.SetSalesperson(MapPane.FindClosestObject(t.Text(), context, false));
    return 0;
}

// REVSYNC: buysellnogolddialog @ 0x00427a20 -> 0x00533040.
COMMAND(CmdBuySellNoGoldDialog)
{
    if (!IsName(t))
        return CMD_BADPARAMS;
    BuySellPane.SetNoGoldDialog(t.Text());
    return 0;
}

// REVSYNC: buysellpurchasedialog @ 0x00427a50 -> 0x00532fe0.
COMMAND(CmdBuySellPurchaseDialog)
{
    if (!IsName(t))
        return CMD_BADPARAMS;
    BuySellPane.SetPurchaseDialog(t.Text());
    return 0;
}

// REVSYNC: buysellshoptype @ 0x00427870 -- `buy|sell weapon|armor|misc`.
COMMAND(CmdBuySellShopType)
{
    uint32_t mode = 0;
    if (t.Is("BUY"))
        mode = TBuySellPane::kBuy;
    else if (t.Is("SELL"))
        mode = TBuySellPane::kSell;
    else
    {
        Output("Buy Sell Option");
        return CMD_BADPARAMS;
    }
    t.Get();
    t.WhiteGet();

    uint32_t goods = 0;
    if (t.Is("WEAPON"))
        goods = TBuySellPane::kWeapon;
    else if (t.Is("ARMOR"))
        goods = TBuySellPane::kArmor;
    else if (t.Is("MISC"))
        goods = TBuySellPane::kMisc;
    else
    {
        Output("Missing Shop Type");
        return CMD_BADPARAMS;
    }
    BuySellPane.SetShopType(mode | goods);
    t.Get();
    return 0;
}

// REVSYNC: buyselladd @ 0x00427500 -> 0x00530670 -- `[<amount>] <type>`.
COMMAND(CmdBuySellAdd)
{
    int32_t amount = 1;
    if (t.Type() == TKN_NUMBER)
    {
        amount = t.Index();
        t.Get();
        t.WhiteGet();
    }
    BuySellPane.Add(t.Text(), amount);
    return 0;
}

// REVSYNC: buyselladdcriteria @ 0x00427240 -> 0x00530af0.
COMMAND(CmdBuySellAddCriteria)
{
    SCriteria criteria;
    if (!ReadCriteria(t, context, criteria))
        return CMD_BADPARAMS;
    BuySellPane.AddCriteria(criteria.stat.c_str(), criteria.min, criteria.max);
    return 0;
}

// REVSYNC: buysellremovecriteria @ 0x004273a0 -> 0x00532340.
COMMAND(CmdBuySellRemoveCriteria)
{
    SCriteria criteria;
    if (!ReadCriteria(t, context, criteria))
        return CMD_BADPARAMS;
    BuySellPane.RemoveCriteria(criteria.stat.c_str(), criteria.min, criteria.max);
    return 0;
}

// REVSYNC: buyselladdbuyitems @ 0x00427860 -> 0x00531b70.
COMMAND(CmdBuySellAddBuyItems)
{
    BuySellPane.AddBuyItems();
    return 0;
}

// REVSYNC: buyselladdbuyitem @ 0x00427810 -> 0x00531d70.
COMMAND(CmdBuySellAddBuyItem)
{
    if (!IsName(t))
        return CMD_BADPARAMS;
    BuySellPane.AddBuyItem(t.Text());
    return 0;
}

// REVSYNC: buyselladdbuycriteria @ 0x00427550 -> 0x00531fc0.
COMMAND(CmdBuySellAddBuyCriteria)
{
    SCriteria criteria;
    if (!ReadCriteria(t, context, criteria))
        return CMD_BADPARAMS;
    BuySellPane.AddBuyCriteria(criteria.stat.c_str(), criteria.min, criteria.max);
    return 0;
}

// REVSYNC: buysellremovebuycriteria @ 0x004276b0 -> 0x005321f0, which is
// 0x00532340 (buysellremovecriteria's).
COMMAND(CmdBuySellRemoveBuyCriteria)
{
    SCriteria criteria;
    if (!ReadCriteria(t, context, criteria))
        return CMD_BADPARAMS;
    BuySellPane.RemoveCriteria(criteria.stat.c_str(), criteria.min, criteria.max);
    return 0;
}

// REVSYNC: buysellremove @ 0x00427840 -> 0x00532210.
COMMAND(CmdBuySellRemove)
{
    BuySellPane.Remove(t.Text());
    return 0;
}

// REVSYNC: buysellscreen @ 0x00427090 (single player) -- the customer is the
// target if a player, else the caller if a player, else the script's user if
// a player; PlayScreen opens the drawer on its next pulse. Retail also wrote
// the dialog pane's responder and response-control flag, which nothing reads
// before SetWait rewrites them (COMMAND_SYSTEM.md §6.6); the network game's
// branch (0x00586a60) isn't ported.
COMMAND(CmdBuySellScreen)
{
    auto isPlayer = [](TObjectInstance* obj) { return obj && obj->ObjClass() == OBJCLASS_PLAYER; };
    TObjectInstance* who = isPlayer(context) ? context
                         : isPlayer(scriptcontext) ? scriptcontext
                         : script ? script->AliasedUser() : nullptr;
    BuySellPane.SetCustomer(who);
    PlayScreen.RequestBuySell(true);
    return 0;
}
