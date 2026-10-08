// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *    combattrace.cpp - a tick-by-tick record of fights, for replays     *
// *************************************************************************
//
// See combattrace.h.

#include "combattrace.h"

#include "character.h"
#include "imagery.h"
#include "mappane.h"
#include "playscreen.h"
#include "revutils.h"

#include <cstdarg>
#include <cstdio>
#include <dlfcn.h>
#include <set>

namespace CombatTrace
{
namespace
{

FILE* g_out = nullptr;
int64_t g_tick = 0;
std::set<int32_t> g_traced;      // map indices of characters seen fighting
int64_t g_rngFrom = 1, g_rngTo = 0;

void OnRandom(int32_t value, const void* caller)
{
    if (!g_out || g_tick < g_rngFrom || g_tick > g_rngTo)
        return;
    Dl_info info;
    uintptr_t at = (uintptr_t)caller;
    if (dladdr(caller, &info) && info.dli_fbase)
        at -= (uintptr_t)info.dli_fbase;
    fprintf(g_out, "%lld\trng\tn=%llu\tvalue=%d\tat=0x%llx\n", (long long)g_tick,
            (unsigned long long)RandomDraws(), value, (unsigned long long)at);
}

const char* Name(const TObjectInstance* o)
{
    return o && o->GetName() ? o->GetName() : "-";
}

void Block(const char* key, const TActionBlock* ab)
{
    if (!ab)
    {
        fprintf(g_out, "\t%s=-", key);
        return;
    }
    fprintf(g_out, "\t%s=%d:%s:%d:%d:%d", key, (int)ab->action, ab->name, ab->angle, ab->moveangle,
            ab->turnrate);
}

void Character(TCharacter* c)
{
    const S3DPoint p = c->Pos();
    const TObjectImagery* im = c->GetImagery();
    const int32_t st = c->TObjectInstance::GetState();
    const char* anim = im ? im->GetAniName(st) : nullptr;
    const TActionBlock* doing = c->DoingBlock();
    const TObjectInstance* target = doing && doing->obj ? doing->obj
                                  : c->RootBlock() ? c->RootBlock()->obj : nullptr;
    fprintf(g_out, "%lld\tchar\tname=%s\tid=%d\tpos=%d,%d,%d\tface=%d\tmove=%d\tstate=%s\tframe=%d", (long long)g_tick,
            Name(c), c->GetMapIndex(), p.x, p.y, p.z, c->GetFace(), c->GetMoveAngle(), anim ? anim : "?",
            (int)c->GetFrame());
    Block("doing", doing);
    Block("desired", c->DesiredBlock() != doing ? c->DesiredBlock() : nullptr);
    fprintf(g_out, "\thp=%d\tfat=%d\tmana=%d\ttarget=%s\n", c->Health(), c->Fatigue(), c->Mana(), Name(target));
}

}  // namespace

bool Open(const char* path)
{
    g_out = fopen(path, "w");
    return g_out != nullptr;
}

void TraceRandom(int64_t from, int64_t to)
{
    g_rngFrom = from;
    g_rngTo = to;
    SetRandomObserver(OnRandom);
}

bool Enabled()
{
    return g_out != nullptr;
}

void Tick(int64_t tick)
{
    if (!g_out)
        return;
    g_tick = tick;      // the tick's own events came in under this number
    for (TMapIterator i(nullptr, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
    {
        auto* c = static_cast<TCharacter*>(i.Item());
        if (!c || !c->IsCharacter())
            continue;
        if (c->IsFighting())
            g_traced.insert(c->GetMapIndex());
        if (g_traced.count(c->GetMapIndex()))
            Character(c);
    }
    fprintf(g_out, "%lld\ttick\trng=%llu\n", (long long)tick, (unsigned long long)RandomDraws());
    fflush(g_out);
    g_tick = tick + 1;  // events from here on belong to the next tick
}

void Event(const TCharacter* who, const char* what, const char* fmt, ...)
{
    if (!g_out)
        return;
    fprintf(g_out, "%lld\tevent\twhat=%s\tname=%s\tid=%d", (long long)g_tick, what, Name(who),
            who ? who->GetMapIndex() : -1);
    if (fmt)
    {
        fputc('\t', g_out);
        va_list ap;
        va_start(ap, fmt);
        vfprintf(g_out, fmt, ap);
        va_end(ap);
    }
    fputc('\n', g_out);
}

}  // namespace CombatTrace
