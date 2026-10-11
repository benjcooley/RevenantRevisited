// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   playtest.h - TPlaytest, a recorded or controlled play session       *
// *************************************************************************
//
// The port as a playtest host (docs/playtest/PLAN.md §6). Three ways to run
// a session, all deterministic (DeterministicRun):
//
//   --control=stdio     a director drives the game over JSON Lines on stdin
//                       and stdout (PROTOCOL.md §5): it starts a game, steps
//                       it a number of ticks with the input for each, and
//                       observes. Lockstep: one tick and one draw a frame,
//                       and nothing moves between steps.
//   --record=<bundle>   records a session -- the director's, or a person's
//                       playing at the display's rate (`timing: frames`) --
//                       as a bundle (PROTOCOL.md §4).
//   --replay=<bundle>   plays a recording back; with --verify, compares its
//                       check lines and reports the first tick that differs.
//
// The session sits at two points of the frame (AppFrame): its start, where
// it decides whether the frame runs and writes the tick's check line, and
// the input drain, where it injects the tick's events and records what
// dispatches. Watching (PROTOCOL.md §5a): without --headless the window
// shows each stepped frame; --control-pace paces the steps, `caption`
// draws the director's note over the window, --watch-frames writes each
// frame for the viewer.
#pragma once

#include "jsonvalue.h"
#include "recording.h"

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

class JsonOut;

class TPlaytest
{
  public:
    // revmain's game boot (paths, INI, engine, the start screen): true when
    // the game is up. At launch, or in control mode on `start`.
    using TBoot = bool (*)();

    // Reads the session's arguments and sets it up: the channel, or the
    // bundle to record or replay. False when they can't work (logged).
    bool Initialize(int argc, char** argv, TBoot boot);

    [[nodiscard]] bool Active() const { return mode != EMode::None; }
    // Input comes from the channel or the recording alone.
    [[nodiscard]] bool OwnsInput() const { return mode == EMode::Control || replaying; }
    // The game boots on the director's `start`, not at launch.
    [[nodiscard]] bool WaitsForStart() const { return mode == EMode::Control; }
    // One tick per frame (otherwise the display's frame times).
    [[nodiscard]] bool Lockstep() const { return timing == ETiming::Lockstep; }

    // ---- the boot (revmain BootGame) ----
    // Before the INI is read: the session's INI becomes SavePath's.
    void BeforeIni();
    // After the INI and the command line: the seed, the start slot in place
    // for the quick start, the patches.
    void ApplyStart();
    // The game is up: tick 0. The recording starts here.
    void Started();

    // ---- the frame (revmain AppFrame) ----
    // Answers the director's requests, writes the tick's check line, and
    // decides: false when no frame should run (waiting for a step, pacing).
    bool BeginFrame();
    // The time step this frame runs with: a replay's recorded one, else
    // `dt`, which a recording under frame timing keeps.
    [[nodiscard]] double FrameTime(double dt);
    // Just before the input drain: the tick's (or frame's) events queue.
    void BeforeDrain();
    // After the frame: a step that is done is answered; a replay that is
    // done reports.
    void EndFrame();
    // Over the window, after the game's frame: the caption. ImGui's frame
    // must be open.
    void DrawOverlay() const;
    // The process is ending: the recording gets its end line.
    void Shutdown();
    // What the process should exit with: 1 when a verified replay diverged,
    // 2 when the session's arguments can't work.
    [[nodiscard]] int ExitCode() const { return exitCode; }

  private:
    enum class EMode : uint8_t { None, Control, Record, Replay };

    struct SStepEvent
    {
        int64_t     t = 0;          // the session tick it is for
        SInputEvent event;
    };

    // ---- the control channel ----
    void Serve();
    void Request(const std::string& line);
    void Respond(const JsonValue& id, bool ok, const std::string& members, const std::string& error = {});
    bool CmdHello(JsonOut& out);
    bool CmdStart(const JsonValue& req, std::string& error);
    bool CmdStep(const JsonValue& req, std::string& error);
    bool CmdObserve(const JsonValue& req, JsonOut& out, std::string& error);
    bool CmdProject(const JsonValue& req, JsonOut& out, std::string& error);
    bool CmdPick(const JsonValue& req, JsonOut& out, std::string& error);
    bool CmdExportSlot(const JsonValue& req, JsonOut& out, std::string& error);
    bool CmdMark(const JsonValue& req, JsonOut& out, std::string& error);
    bool CmdReplay(const JsonValue& req, std::string& error);
    bool CmdCaption(const JsonValue& req, std::string& error);
    void CmdQuit();

    // ---- sessions ----
    bool Boot(std::string& error);
    bool LoadReplay(const std::filesystem::path& bundle, std::string& error);
    [[nodiscard]] int64_t Tick() const;
    void Boundary();
    void CompareCheck(int64_t t);
    void FinishReplay();
    void FinishStep();
    void WriteWatchFrame();
    void End();

    EMode   mode = EMode::None;
    ETiming timing = ETiming::Lockstep;
    TBoot   boot = nullptr;
    bool    booted = false;
    bool    started = false;            // tick 0 reached
    bool    clickwalk = false;          // --clickwalk / the recording's patch
    bool    headless = false;

    // The session's start (from `start`, the command line or a recording).
    bool                  fromSlot = false;
    std::filesystem::path startSlotDir; // the slot to copy into the save slots
    std::string           startSlot;    // its name
    std::filesystem::path iniFile;      // the INI to use (empty: SavePath's)
    uint32_t              seed = 0;
    bool                  seedSet = false;
    std::string           by;

    // The clock: session tick t is the legacy frame count less this.
    int64_t  legacyBase = 0;
    int64_t  frame = 0;                 // frames begun since tick 0
    int64_t  current = 0;               // the frame running (frame timing's `f`)
    int64_t  boundary = -1;             // the tick whose boundary this frame began at
    uint64_t drawsBase = 0;
    int64_t  nextCheck = 0;             // frame timing: the next check line is due here

    // Recording.
    std::filesystem::path bundleDir;
    TRecordingWriter      writer;
    SRecordingHeader      header;

    // A step in progress (control).
    JsonValue               stepId;
    bool                    stepping = false;
    int64_t                 stepEnd = 0;
    std::vector<SStepEvent> stepEvents;
    size_t                  stepNext = 0;

    // A replay.
    bool        replaying = false;
    bool        verify = false;
    TRecording  replay;
    int64_t     replayUntil = -1;
    bool        replayDone = false;
    int64_t     lastCompared = -1;      // the tick last compared
    bool        diverged = false;
    int64_t     divergedAt = -1;
    std::string divergedFields;         // JSON object: field -> [recorded, replayed]
    bool        replayAnswer = false;   // a control `replay` waits for the answer
    JsonValue   replayId;

    // Watching.
    double                                pace = 0.0;      // ticks a second; 0: unpaced
    std::chrono::steady_clock::time_point lastPaced{};
    std::filesystem::path                 watchDir;
    std::string                           caption;

    bool quitting = false;
    bool ended = false;
    int  exitCode = 0;
};

extern TPlaytest Playtest;
