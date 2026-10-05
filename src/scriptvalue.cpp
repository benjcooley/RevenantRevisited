// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *      scriptvalue.cpp - object names and values in command lines       *
// *************************************************************************
//
// Retail ports of the context resolver (0x0041e690) and the expression
// evaluator (0x0041f230). The evaluator mirrors retail's token handling call
// for call (Get vs WhiteGet), because scripts depend on its quirks: in
// `PedSix.state=1` the `1` is consumed as the operator's spacing, so the line
// tests the state alone. Where retail's behavior is undefined (a crash, a
// stale value, an endless loop) the expression fails instead; each case is
// marked DEVIATION.

#include "scriptvalue.h"

#include <cstdint>
#include <iterator>
#include <string>
#include <unordered_set>

#include "character.h"
#include "logging.h"
#include "mappane.h"
#include "object.h"
#include "parse.h"
#include "player.h"
#include "playscreen.h"
#include "revenant.h"
#include "revutils.h"
#include "script.h"

namespace {

// Retail's operator table (0x005c8350). The evaluator's operator codes are
// these indices, so the order is the protocol.
enum class EOp : int32_t
{
    Equal, NotEqual, Greater, Less, GreaterEqual, LessEqual,
    And, Or, Not,
    Add, Subtract, Multiply, Divide,
};
constexpr const char* kOperators[] =
    { "=", "<>", ">", "<", ">=", "<=", "and", "or", "not", "+", "-", "*", "/" };

// `isatrelativeposition` holds when the object is this close to the spot.
constexpr int32_t kAtPositionRange = 50;

std::optional<EOp> FindOperator(const char* text)
{
    for (size_t i = 0; i < std::size(kOperators); ++i)
        if (stricmp(text, kOperators[i]) == 0)
            return static_cast<EOp>(i);
    return std::nullopt;
}

// REVSYNC: 0x0041f0b0. Applies `op` between the running operand and `value`.
// Comparisons and and/or/not leave `value` as the running operand (so
// `a < b < c` compares b with c); arithmetic leaves its result. Arithmetic
// wraps like the original's 32-bit registers.
int32_t ApplyOperation(int32_t& operand, EOp op, int32_t value)
{
    const int32_t left = operand;
    const auto wrap = [](int64_t v) { return static_cast<int32_t>(static_cast<uint32_t>(v)); };
    switch (op)
    {
        case EOp::Equal:        operand = value; return left == value;
        case EOp::NotEqual:     operand = value; return left != value;
        case EOp::Greater:      operand = value; return left > value;
        case EOp::Less:         operand = value; return left < value;
        case EOp::GreaterEqual: operand = value; return left >= value;
        case EOp::LessEqual:    operand = value; return left <= value;
        case EOp::And:          operand = value; return left != 0 && value != 0;
        case EOp::Or:           operand = value; return left != 0 || value != 0;
        case EOp::Not:          operand = value; return value == 0;
        case EOp::Add:          operand = wrap(int64_t(left) + value); return operand;
        case EOp::Subtract:     operand = wrap(int64_t(left) - value); return operand;
        case EOp::Multiply:     operand = wrap(int64_t(left) * value); return operand;
        case EOp::Divide:       operand = value ? wrap(int64_t(left) / value) : 0; return operand;
    }
    return 0;
}

// Quoted text compares by a hash (inline in 0x0041f230; the 1998 StringVal):
// each character's offset from 'A', shifted to its index, OR-ed together.
int32_t TextValue(const char* text)
{
    uint32_t value = 0;
    for (uint32_t i = 0; text[i]; ++i)
        value |= static_cast<uint32_t>(static_cast<signed char>(text[i]) - 'A') << (i & 31);
    return static_cast<int32_t>(value);
}

// A bare name is a prototype variable of the context's script, then a game
// state; an unknown name is 0. Prototype variables (DATA/NUMBER blocks,
// 0x00497800) aren't ported; only forest.s declares any.
int32_t VariableValue(const char* name)
{
    const int32_t state = ScriptManager.GameState(name);
    return state == STATE_INVALID ? 0 : state;
}

// A member the port can't evaluate yet fails the expression (an `if` is then
// false), and says so once.
std::optional<int32_t> NotPorted(const char* member, uint32_t retailaddr, const char* needs)
{
    static std::unordered_set<std::string> reported;
    if (reported.insert(member).second)
        log_warn("[script] '.%s' isn't ported (retail @ 0x%08x: %s); the expression fails",
                 member, retailaddr, needs);
    return std::nullopt;
}

void SkipToEndOfLine(TToken& t)
{
    while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
        t.Get();
}

// The value of `<object>.<member> ...` (the member branch of 0x0041f230). On
// entry the token is the member word; on return, the token after the value.
std::optional<int32_t> MemberValue(TToken& t, TObjectInstance& obj,
                                   TObjectInstance* caller, TScript* script)
{
    if (t.Is("state"))
    {
        const int32_t value = obj.GetState();
        t.WhiteGet();
        return value;
    }
    if (t.Is("getitemamount") || t.Is("amount"))
    {
        t.Get();
        t.WhiteGet();
        const int32_t value = obj.GetInventoryAmount(t.Text());
        t.WhiteGet();
        return value;
    }
    if (t.Is("hasemptyslot"))
    {
        const int32_t value = obj.HasEmptySlot();
        t.WhiteGet();
        return value;
    }
    if (t.Is("isoutside"))
        return NotPorted("isoutside", 0x0050d2b0, "which side of an object another one is on");
    if (t.Is("maxslots"))
        return NotPorted("maxslots", 0x00470040, "the inventory capacity of the outermost container");
    if (t.Is("timeofday"))
    {
        const int32_t value = PlayScreen.TimeOfDay();
        t.WhiteGet();
        return value;
    }
    if (t.Is("random"))
    {
        // Retail tests the keyword token, not the one after it, for a range,
        // so the optional range never applies.
        const int32_t value = random(1, 100);
        t.WhiteGet();
        return value;
    }
    if (t.Is("face"))
    {
        const int32_t value = obj.GetFace();
        t.WhiteGet();
        return value;
    }
    if (t.Is("getdistance") || t.Is("getdist"))
    {
        t.WhiteGet();
        TObjectInstance* other = ResolveScriptObject(t.Text(), caller, script);
        t.Get();
        const int32_t value = other ? obj.Distance(other) : 0;
        t.WhiteGet();
        return value;
    }
    if (t.Is("isatrelativeposition") || t.Is("isrelpos"))
    {
        t.Get();
        t.WhiteGet();
        TObjectInstance* other = ResolveScriptObject(t.Text(), caller, script);
        t.WhiteGet();
        if (t.Type() != TKN_NUMBER)
        {
            t.WhiteGet();
            return 0;
        }
        const int32_t dx = t.Index();
        t.Get();
        t.WhiteGet();
        int32_t dy = 0;
        if (t.Type() == TKN_NUMBER)
        {
            dy = t.Index();
            t.Get();
        }
        if (!other)
            return std::nullopt;            // DEVIATION: retail dereferences the missing object
        const S3DPoint at = other->Pos();
        const S3DPoint spot = { at.x + dx, at.y + dy, at.z };
        const int32_t value = Distance(spot, obj.Pos()) < kAtPositionRange;
        t.WhiteGet();
        return value;
    }
    if (t.Is("isatrelativedistance"))
        return NotPorted("isatrelativedistance", 0x0041f230,
                         "the spot at a distance and angle from an object's facing");
    if (t.Is("lastattack"))
        return NotPorted("lastattack", 0x004c62b0,
                         "whether the character's last attack landed (TCharacter +0x168)");
    if (t.Is("position"))
    {
        t.Get();                            // "."
        t.Get();
        const S3DPoint pos = obj.Pos();
        std::optional<int32_t> value;       // DEVIATION: retail reuses a stale value
        if (t.Is("x"))                      // for anything but x, y or z
            value = pos.x;
        if (t.Is("y"))
            value = pos.y;
        if (t.Is("z"))
            value = pos.z;
        t.WhiteGet();
        return value;
    }
    if (t.Is("groupinrange"))
        return NotPorted("groupinrange", 0x00428f50, "the groupinrange command");

    // Any other member word is skipped and the next word names a statistic:
    // `Rahul.stat health`.
    t.Get();
    t.WhiteGet();
    const int32_t value = obj.HasStat(t.Text()) ? obj.GetStat(t.Text()) : 0;
    t.WhiteGet();
    return value;
}

// A fighting character's opponent: the object of its root action when that
// is COMBAT or BOW (retail reads the root action block, TCharacter +0xe0).
TObjectInstance* FightingTarget(TObjectInstance* caller)
{
    if (!caller || !caller->IsCharacter())
        return nullptr;
    return static_cast<TCharacter*>(caller)->Fighting();
}

}  // namespace

// REVSYNC: 0x0041e690
TObjectInstance* ResolveScriptObject(const char* name, TObjectInstance* caller, TScript* script)
{
    if (stricmp(name, "this") == 0)
        return caller;
    if (stricmp(name, "user") == 0)
        return script ? script->User() : Player;    // DEVIATION: retail has no script to ask
    if (stricmp(name, "player") == 0)
        return Player;
    if (stricmp(name, "target") == 0)
        return FightingTarget(caller);

    // Retail's +0xd4, set only by `setcurrent` (0x00428e50, not ported; no
    // shipped script uses it), so it is always empty here.
    if (stricmp(name, "current") == 0)
        return nullptr;

    // party<N>: the Nth player sharing the user's party name. Multiplayer
    // parties aren't ported and no player has a party name, so retail's search
    // finds nobody and answers the user, or the caller without one.
    if (strnicmp(name, "party", 5) == 0)
    {
        TObjectInstance* user = script ? script->User() : nullptr;
        if (user)
            return user;
        if (caller && caller->ObjClass() == OBJCLASS_PLAYER)
            return caller;
    }

    // The running trigger's aliases: "item", "enemy", ... (TScript::Trigger).
    if (script)
        if (TObjectInstance* aliased = script->Alias(name))
            return aliased;

    return MapPane.FindClosestObject(name, caller);
}

// REVSYNC: 0x0041f230
std::optional<int32_t> EvaluateExpression(TToken& t, TObjectInstance* context, TObjectInstance* caller)
{
    TScript* const script = context ? context->GetScript() : nullptr;

    std::optional<EOp> op;                  // operator waiting for its right side
    std::optional<int32_t> operand;         // the running left side
    std::optional<int32_t> total;           // the result so far

    while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
    {
        std::optional<int32_t> value;

        if (t.Type() == TKN_SYMBOL)
        {
            if (op)
                return std::nullopt;
            std::string symbol = t.Text();
            t.Get();
            if (t.Type() == TKN_SYMBOL)
                symbol += t.Text();
            op = FindOperator(symbol.c_str());
            if (!op || (!operand && *op != EOp::Not))
                return std::nullopt;
            t.WhiteGet();
            continue;
        }

        if (t.Type() == TKN_IDENT)
        {
            if (const std::optional<EOp> word = FindOperator(t.Text()))
            {
                if (!operand && *word != EOp::Not)
                    return std::nullopt;
                op = word;
                t.WhiteGet();
                continue;
            }

            const std::string name = t.Text();
            t.Get();
            if (!t.Is("."))
                value = VariableValue(name.c_str());
            else
            {
                t.Get();
                if (t.Type() != TKN_IDENT)
                    return std::nullopt;
                TObjectInstance* obj = ResolveScriptObject(name.c_str(), caller, script);
                if (!obj)
                {
                    // An object that isn't there ends the expression with the
                    // result so far.
                    SkipToEndOfLine(t);
                    break;
                }
                value = MemberValue(t, *obj, caller, script);
                if (!value)
                    return std::nullopt;
            }
            if (t.Type() == TKN_WHITESPACE)
                t.Get();
        }
        else if (t.Type() == TKN_NUMBER || t.Type() == TKN_TEXT)
        {
            value = t.Type() == TKN_TEXT ? TextValue(t.Text()) : t.Index();
            t.WhiteGet();
        }
        else
            return std::nullopt;            // DEVIATION: retail loops on this token forever

        if (*value == STATE_INVALID)
            return std::nullopt;

        if (!operand)
        {
            // The first operand. Only `not` can precede it, and retail leaves
            // that pending rather than applying it, so `not X` reads as X.
            operand = *value;
            total = *value;
        }
        else
        {
            if (!op)
                return std::nullopt;
            total = ApplyOperation(*operand, *op, *value);
            op.reset();
        }
    }

    if (total && *total == STATE_INVALID)
        return std::nullopt;
    return total;
}
