// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   recording.h - playtest recordings (*.rvrec) and their bundles       *
// *************************************************************************
//
// A recording is the input the game received at each tick, plus check lines
// a replay compares, written as JSON Lines: a header, then events, check
// lines and marks in time order, then the end. Its bundle is the directory
// that also holds the INI and the start slot it was made from. The format
// is shared with the retail emulator and the director's Python reader:
// docs/playtest/PROTOCOL.md §2-§4 and §6.
#pragma once

#include "game/ui/inputqueue.h"
#include "engine/core/jsonvalue.h"

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <map>
#include <string>
#include <utility>
#include <vector>

// The files of a bundle (PROTOCOL.md §4).
namespace Bundle
{
inline constexpr const char* kRecording = "session.rvrec";
inline constexpr const char* kIni       = "Revenant.ini";
inline constexpr const char* kStartDir  = "start";

// PROTOCOL.md §6: every file under `slot` in the sorted order of its lower
// case, '/'-separated relative path; each hashed as the path, a NUL, its
// bytes and a NUL. Empty when the directory can't be read.
[[nodiscard]] std::string SlotSha(const std::filesystem::path& slot);

// Copies the directory `from` to `to` (created, files overwritten).
bool CopyDirectory(const std::filesystem::path& from, const std::filesystem::path& to,
                   std::string& error);
} // namespace Bundle

// The protocol's key names: Windows virtual-key names without VK_ ("A",
// "RETURN", "OEM_1"), else the code as "0x5D".
namespace KeyNames
{
[[nodiscard]] std::string Name(int32_t vk);
[[nodiscard]] int32_t Code(const std::string& name);    // 0: not a key
} // namespace KeyNames

enum class ETiming : uint8_t
{
    Lockstep,       // one frame per tick; lines keyed by tick `t`
    Frames,         // the display's frames, each with its exact time step
};

struct SRecordingHeader
{
    std::string              buildSha;
    std::string              buildName;
    std::vector<std::string> patches;       // behaviour patches ("clickwalk")
    bool                     fromSlot = false;
    std::string              slot;          // the start slot's name
    std::string              slotSha;
    std::string              iniSha;
    uint32_t                 seed = 0;
    ETiming                  timing = ETiming::Lockstep;
    std::string              created;       // ISO 8601, UTC
    std::string              by;            // who drove the session
    bool                     revisited = false;     // --revisited (port only)

    [[nodiscard]] bool HasPatch(const std::string& patch) const;
};

// An event as a body line's fields (without its time key), and back.
void WriteEventFields(class JsonOut& out, const SInputEvent& event);
[[nodiscard]] bool ReadEventFields(const JsonValue& line, SInputEvent& event, std::string& error);

class TRecordingWriter
{
  public:
    ~TRecordingWriter() { Close(); }

    bool Open(const std::filesystem::path& file, const SRecordingHeader& header);
    [[nodiscard]] bool IsOpen() const { return file != nullptr; }
    [[nodiscard]] ETiming Timing() const { return timing; }

    // `when` is the event's tick, or under frame timing its frame.
    void Event(int64_t when, const SInputEvent& event);
    // Under frame timing, a display frame and its exact time step.
    void Frame(int64_t f, double dt);
    // A check line (PROTOCOL.md §3), `fields` a JSON object. One a tick.
    void Check(int64_t t, const std::string& fields);
    // A mark and its other fields (name, value as JSON).
    void Mark(int64_t t, const std::string& kind,
              const std::vector<std::pair<std::string, std::string>>& fields);
    // The end line; closes the file.
    void End(int64_t t);
    void Close();

    [[nodiscard]] int64_t LastCheck() const { return lastCheck; }

  private:
    void Line(const std::string& json);

    FILE*   file = nullptr;
    ETiming timing = ETiming::Lockstep;
    int64_t lastCheck = -1;
};

class TRecording
{
  public:
    bool Load(const std::filesystem::path& file, std::string& error);

    [[nodiscard]] const SRecordingHeader& Header() const { return header; }
    // The events at a tick (or under frame timing a frame), in file order.
    [[nodiscard]] const std::vector<SInputEvent>* EventsAt(int64_t when) const;
    // The check line at a tick (the last, if several), or null.
    [[nodiscard]] const JsonValue* CheckAt(int64_t t) const;
    // Under frame timing: the frames recorded and each one's time step.
    [[nodiscard]] int64_t Frames() const { return int64_t(frameDts.size()); }
    [[nodiscard]] double FrameDt(int64_t f) const { return frameDts[size_t(f)]; }
    // The tick the recording ended at; -1 without an end line.
    [[nodiscard]] int64_t End() const { return end; }

  private:
    SRecordingHeader                           header;
    std::map<int64_t, std::vector<SInputEvent>> events;
    std::map<int64_t, JsonValue>               checks;
    std::vector<double>                        frameDts;
    int64_t                                    end = -1;
};
