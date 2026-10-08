// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   consoleexec.cpp - startup console-command queue (--exec)            *
// *************************************************************************

#include "consoleexec.h"

#include <cstdlib>
#include <deque>
#include <string>

#include "command.h"
#include "editorstub.h"
#include "logging.h"
#include "parse.h"
#include "player.h"
#include "testconfig.h"

namespace {

std::deque<std::string> g_queue;
bool g_parsed = false;
int32_t g_sleepTicks = 0;

std::string Trim(const std::string& s)
{
    const size_t b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return {};
    const size_t e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

void ParseQueue()
{
    g_parsed = true;
    std::string all = StartupExec;
    size_t start = 0;
    while (start <= all.size())
    {
        size_t semi = all.find(';', start);
        if (semi == std::string::npos) semi = all.size();
        std::string cmd = Trim(all.substr(start, semi - start));
        if (!cmd.empty()) g_queue.push_back(cmd);
        start = semi + 1;
    }
    log_info("[exec] queued %zu command(s)", g_queue.size());
}

}  // namespace

void PulseStartupExec()
{
    if (!StartupExec[0] || !Player) return;
    if (!g_parsed) ParseQueue();
    if (g_sleepTicks > 0) { --g_sleepTicks; return; }
    if (g_queue.empty()) return;

    std::string cmd = g_queue.front();
    g_queue.pop_front();

    if (cmd.rfind("sleep", 0) == 0)
    {
        g_sleepTicks = std::atoi(cmd.c_str() + 5);
        log_info("[exec] sleep %d", g_sleepTicks);
        return;
    }

    // CommandInterpreter expects a line-terminated buffer it may tokenize in
    // place; keep a private copy with a trailing newline.
    std::string line = cmd + "\n";
    TStringParseStream s(line.data(), (int32_t)line.size());
    TToken t(s);
    t.WhiteGet();
    const int32_t result = CommandInterpreter(Player, t, MINCMDABREV, nullptr);
    log_info("[exec] > %s  (result=0x%x)", cmd.c_str(), result);
}
