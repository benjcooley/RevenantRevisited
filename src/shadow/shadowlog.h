// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *          shadowlog.h - a Shadow Smoke run's event log                  *
// *************************************************************************
//
// shadow.jsonl: one JSON object a line, each an event of the run with the
// game tick it happened on (docs/shadow/ARCHITECTURE.md §13). The same
// events go to revenant.log as `[smoke]` lines, for reading beside the
// game's own trace.

#pragma once

#include "../jsonout.h"

#include <cstdint>
#include <cstdio>
#include <functional>
#include <string>

class TShadowLog
{
  public:
    TShadowLog() = default;
    TShadowLog(const TShadowLog&) = delete;
    TShadowLog& operator=(const TShadowLog&) = delete;
    ~TShadowLog() { Close(); }

    // Creates `dir` (and its parents) and opens shadow.jsonl in it.
    bool Open(const std::string& dir);
    void Close();
    [[nodiscard]] const std::string& Dir() const { return dir; }

    // The tick events are stamped with.
    void SetTick(int64_t now) { tick = now; }

    // An event: {"tick":N,"event":"<type>", ...}; `fill` adds the fields.
    // `text` is the one-line form for revenant.log.
    void Event(const char* type, const std::string& text, const std::function<void(JsonOut&)>& fill = {});

    // A file in the run's directory, written whole.
    bool WriteFile(const char* name, const std::string& contents) const;

  private:
    std::string dir;
    FILE*       file = nullptr;
    int64_t     tick = 0;
};
