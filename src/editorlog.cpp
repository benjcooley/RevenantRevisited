// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *             editorlog.cpp - In-editor log viewer (ring + clipper)     *
// *************************************************************************

#include "editorlog.h"

#include "editorfonts.h"

#include <imgui.h>

#include <atomic>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <mutex>
#include <vector>

extern "C" {
#include "log.h"
}

namespace {

// Bounded ring of captured lines. ImGui list-clippers keep render cost
// O(visible) regardless of size, but we still cap so one runaway frame
// can't eat unbounded memory.
constexpr int32_t kRingCapacity = 4096;

struct SLogEntry
{
    int8_t   level    = LOG_INFO;
    uint8_t  hour     = 0;
    uint8_t  minute   = 0;
    uint8_t  second   = 0;
    char     message[240]   = {};   // truncated; rare lines wider than this
};

struct SLogStore
{
    std::mutex mtx;
    std::vector<SLogEntry> entries;   // pre-sized to kRingCapacity
    int32_t head      = 0;            // next slot to write
    int32_t count     = 0;            // up to kRingCapacity
    bool    installed = false;
    bool    auto_scroll = true;
    int32_t level_filter = LOG_INFO;
};

SLogStore& Store()
{
    static SLogStore s;
    if (s.entries.empty()) s.entries.resize(kRingCapacity);
    return s;
}

void OnLogEvent(log_Event* ev)
{
    if (!ev) return;
    SLogStore& s = Store();
    SLogEntry e;
    e.level = int8_t(ev->level);
    if (ev->time) {
        e.hour   = uint8_t(ev->time->tm_hour);
        e.minute = uint8_t(ev->time->tm_min);
        e.second = uint8_t(ev->time->tm_sec);
    }

    // Format the message portion. va_list is single-use, but the existing
    // log.c stdout/file callbacks consume it before us so we get a fresh
    // copy. Use vsnprintf into our fixed buffer; oversized lines are
    // truncated (with a trailing ellipsis-ish marker).
    va_list ap_copy;
    va_copy(ap_copy, ev->ap);
    int32_t n = std::vsnprintf(e.message, sizeof(e.message), ev->fmt ? ev->fmt : "", ap_copy);
    va_end(ap_copy);
    if (n < 0) e.message[0] = '\0';
    if (n >= int32_t(sizeof(e.message))) {
        e.message[sizeof(e.message) - 4] = '.';
        e.message[sizeof(e.message) - 3] = '.';
        e.message[sizeof(e.message) - 2] = '.';
        e.message[sizeof(e.message) - 1] = '\0';
    }

    std::lock_guard<std::mutex> lk(s.mtx);
    s.entries[s.head] = e;
    s.head = (s.head + 1) % kRingCapacity;
    if (s.count < kRingCapacity) s.count++;
}

ImVec4 ColorForLevel(int32_t level)
{
    switch (level) {
        case LOG_TRACE: return ImVec4(0.55f, 0.55f, 0.55f, 1.0f);
        case LOG_DEBUG: return ImVec4(0.65f, 0.75f, 1.00f, 1.0f);
        case LOG_INFO:  return ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
        case LOG_WARN:  return ImVec4(1.00f, 0.80f, 0.30f, 1.0f);
        case LOG_ERROR: return ImVec4(1.00f, 0.45f, 0.40f, 1.0f);
        case LOG_FATAL: return ImVec4(1.00f, 0.30f, 0.50f, 1.0f);
        default:        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}

const char* LevelTag(int32_t level)
{
    switch (level) {
        case LOG_TRACE: return "TRC";
        case LOG_DEBUG: return "DBG";
        case LOG_INFO:  return "INF";
        case LOG_WARN:  return "WRN";
        case LOG_ERROR: return "ERR";
        case LOG_FATAL: return "FTL";
        default:        return "???";
    }
}

} // namespace

void EditorLogInstall()
{
    SLogStore& s = Store();
    if (s.installed) return;
    log_add_callback(&OnLogEvent, nullptr, LOG_TRACE);
    s.installed = true;
}

int32_t EditorLogCount()
{
    SLogStore& s = Store();
    std::lock_guard<std::mutex> lk(s.mtx);
    return s.count;
}

void EditorLogClear()
{
    SLogStore& s = Store();
    std::lock_guard<std::mutex> lk(s.mtx);
    s.head = s.count = 0;
}

void EditorLogDrawPanel()
{
    SLogStore& s = Store();

    // Toolbar -------------------------------------------------------------
    if (ImGui::Button("Clear")) EditorLogClear();
    ImGui::SameLine();
    ImGui::Checkbox("Auto-scroll", &s.auto_scroll);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80.0f);
    const char* level_names[] = { "Trace", "Debug", "Info", "Warn", "Error", "Fatal" };
    ImGui::Combo("Level", &s.level_filter, level_names, 6);
    ImGui::SameLine();
    ImGui::TextDisabled("entries: %d / %d", int(s.count), int(kRingCapacity));

    // Snapshot entries under lock so the list-clipper can index without
    // touching the mutex on every row.
    std::vector<SLogEntry> snap;
    int32_t snap_count = 0;
    {
        std::lock_guard<std::mutex> lk(s.mtx);
        snap_count = s.count;
        snap.resize(snap_count);
        if (snap_count > 0) {
            // Linearise the ring (oldest -> newest).
            const int32_t start = (s.head - s.count + kRingCapacity) % kRingCapacity;
            for (int32_t i = 0; i < snap_count; ++i)
                snap[i] = s.entries[(start + i) % kRingCapacity];
        }
    }

    ImGui::Separator();
    ImGui::BeginChild("##editor_log_scroll",
                      ImVec2(0, 0), false,
                      ImGuiWindowFlags_HorizontalScrollbar);

    if (ImFont* mono = EditorFonts::GetMonoFont()) ImGui::PushFont(mono);
    ImGuiListClipper clipper;
    clipper.Begin(snap_count);
    while (clipper.Step()) {
        for (int32_t i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
            const SLogEntry& e = snap[i];
            if (e.level < s.level_filter) continue;
            ImGui::PushStyleColor(ImGuiCol_Text, ColorForLevel(e.level));
            ImGui::Text("%02u:%02u:%02u %s %s",
                        e.hour, e.minute, e.second,
                        LevelTag(e.level), e.message);
            ImGui::PopStyleColor();
        }
    }
    clipper.End();
    if (EditorFonts::GetMonoFont()) ImGui::PopFont();

    if (s.auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
}
