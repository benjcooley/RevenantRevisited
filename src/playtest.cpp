// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   playtest.cpp - TPlaytest, a recorded or controlled play session     *
// *************************************************************************

#include "playtest.h"

#include "argh.h"
#include "buildinfo.h"
#include "display.h"
#include "framesnap.h"
#include "inputqueue.h"
#include "jsonout.h"
#include "logging.h"
#include "mappane.h"
#include "observation.h"
#include "platform/stdiopipe.h"
#include "player.h"
#include "playscreen.h"
#include "revenant.h"
#include "savegame.h"
#include "sha256.h"
#include "testconfig.h"
#include "time.h"

#include "imgui.h"
#include <sokol_app.h>

#include <algorithm>
#include <cstdio>
#include <ctime>
#include <system_error>

namespace fs = std::filesystem;

TPlaytest Playtest;

namespace
{

constexpr int32_t kCheckEvery     = 24;     // ticks between check lines (PROTOCOL.md §3)
constexpr int32_t kObjectRadius   = 800;    // observe's default
constexpr int32_t kPointerStartX  = 320;    // PROTOCOL.md §2: the pointer starts mid-screen
constexpr int32_t kPointerStartY  = 240;
constexpr const char* kClickWalk  = "clickwalk";

std::string NowUtc()
{
    const std::time_t now = std::time(nullptr);
    char buf[32] = {};
    if (const std::tm* utc = std::gmtime(&now))
        std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", utc);
    return buf;
}

std::string Argument(const argh::parser& cmd, const char* name, bool& given)
{
    std::string value;
    given = static_cast<bool>(cmd(name) >> value) || cmd[name];
    return value;
}

bool CopyFile(const fs::path& from, const fs::path& to, std::string& error)
{
    std::error_code ec;
    fs::create_directories(to.parent_path(), ec);
    fs::copy_file(from, to, fs::copy_options::overwrite_existing, ec);
    if (ec)
        error = "can't copy " + from.string() + " to " + to.string() + ": " + ec.message();
    return !ec;
}

bool WritePng(const fs::path& file, const uint8_t* rgba, int32_t w, int32_t h, int32_t compression = 8)
{
    if (!rgba || w <= 0 || h <= 0)
        return false;
    std::error_code ec;
    fs::create_directories(file.parent_path(), ec);
    return FrameSnap::WritePngRGBA(file.string().c_str(), w, h, rgba, compression);
}

} // namespace

// ****************
// * Setting up   *
// ****************

bool TPlaytest::Initialize(int argc, char** argv, TBoot bootGame)
{
    boot = bootGame;
    argh::parser cmd;
    cmd.add_params({"control", "record", "replay", "control-pace", "watch-frames", "seed", "quickstart"});
    cmd.parse(argc, argv);

    bool given = false;
    const std::string control = Argument(cmd, "control", given);
    const bool controlled = given;
    const std::string record = Argument(cmd, "record", given);
    const bool recording = given;
    const std::string replayBundle = Argument(cmd, "replay", given);
    const bool replayed = given;
    headless = cmd["headless"];
    clickwalk = cmd[kClickWalk];

    exitCode = 2;      // until the arguments turn out to work
    if (int(controlled) + int(recording) + int(replayed) > 1)
    {
        log_error("[playtest] --control, --record and --replay are separate sessions; "
                  "a controlled session records through `start`");
        return false;
    }
    if (!controlled && !recording && !replayed)
    {
        exitCode = 0;
        return true;
    }

    // Every session is deterministic: no wall clock, no other thread's say.
    SetDeterministicRun(true);

    std::string value;
    if (cmd("control-pace") >> value)
        pace = (std::max)(0.0, std::atof(value.c_str()));
    if (cmd("watch-frames") >> value)
        watchDir = value;

    if (controlled)
    {
        if (control != "stdio")
        {
            log_error("[playtest] --control=%s: only stdio is implemented", control.c_str());
            return false;
        }
        if (!rev_platform::OpenStdioPipe())
        {
            log_error("[playtest] can't take stdin/stdout for the control channel");
            return false;
        }
        // The protocol owns stdout and the director may not drain stderr:
        // the log goes to revenant.log alone.
        log_set_quiet(true);
        mode     = EMode::Control;
        timing   = ETiming::Lockstep;
        exitCode = 0;
        log_info("[playtest] control channel on stdio%s", headless ? " (headless)" : "");
        return true;
    }

    if (replayed)
    {
        std::string error;
        mode   = EMode::Replay;
        verify = cmd["verify"];
        if (!LoadReplay(replayBundle, error))
        {
            log_error("[playtest] --replay=%s: %s", replayBundle.c_str(), error.c_str());
            return false;
        }
        exitCode = 0;
        return true;
    }

    // --record: a session at the command line's start (--quickstart[=slot]).
    char* end = nullptr;
    const bool seeded = static_cast<bool>(cmd("seed") >> value);
    if (seeded)
        seed = uint32_t(std::strtoul(value.c_str(), &end, 10));
    if (!seeded || value.empty() || !end || *end)
    {
        log_error("[playtest] --record needs --seed=N (the recording replays from it)");
        return false;
    }
    if (!cmd("quickstart") && !cmd["quickstart"])
    {
        log_error("[playtest] --record needs --quickstart or --quickstart=<slot>: "
                  "a recording starts from a new game or a slot");
        return false;
    }
    mode      = EMode::Record;
    seedSet   = true;
    timing    = cmd["fixedstep"] ? ETiming::Lockstep : ETiming::Frames;
    bundleDir = record;
    by        = "port, interactive";
    exitCode  = 0;
    log_info("[playtest] recording to %s (%s timing)", record.c_str(),
             timing == ETiming::Frames ? "frame" : "lockstep");
    return true;
}

bool TPlaytest::LoadReplay(const fs::path& bundle, std::string& error)
{
    if (!replay.Load(bundle / Bundle::kRecording, error))
        return false;
    const SRecordingHeader& h = replay.Header();
    timing       = h.timing;
    seed         = h.seed;
    seedSet      = true;
    fromSlot     = h.fromSlot;
    startSlot    = h.slot;
    startSlotDir = fromSlot ? bundle / Bundle::kStartDir / h.slot : fs::path();
    iniFile      = bundle / Bundle::kIni;
    clickwalk    = h.HasPatch(kClickWalk);
    replaying    = true;
    replayDone   = false;
    if (timing == ETiming::Lockstep && replay.End() < 0)
    {
        error = "the recording has no end line";
        return false;
    }
    std::error_code ec;
    if (!fs::exists(iniFile, ec) || (fromSlot && !fs::is_directory(startSlotDir, ec)))
    {
        error = "the bundle lacks its INI or its start slot";
        return false;
    }
    log_info("[playtest] replaying %s: %s, seed %u, %s timing, from %s%s%s", bundle.string().c_str(),
             h.buildName.c_str(), seed, timing == ETiming::Frames ? "frame" : "lockstep",
             fromSlot ? "slot " : "a new game", fromSlot ? h.slot.c_str() : "",
             verify ? ", verifying" : "");
    if (h.buildSha != kBuildSha)
        log_warn("[playtest] the recording was made by %s; this is %s", h.buildName.c_str(), kBuildName);
    return true;
}

// ************
// * The boot *
// ************

void TPlaytest::BeforeIni()
{
    if (!Active() || iniFile.empty())
        return;
    std::string error;
    if (!CopyFile(iniFile, fs::path(SavePath) / Bundle::kIni, error))
        log_error("[playtest] %s", error.c_str());
}

void TPlaytest::ApplyStart()
{
    if (!Active())
        return;

    if (seedSet)
    {
        SeedRandom(seed);
        log_info("[playtest] seed %u", seed);
    }

    if (mode == EMode::Record)
    {
        fromSlot  = StartupQuickstartSave[0] != '\0';
        startSlot = StartupQuickstartSave;
    }

    // The start slot goes among the save slots, replacing one of its name,
    // so the game loads exactly what the session names.
    if (fromSlot && !startSlotDir.empty())
    {
        const fs::path dest = TSaveGame::SlotsDir() / startSlot;
        std::error_code ec;
        if (!fs::equivalent(startSlotDir, dest, ec))
        {
            fs::remove_all(dest, ec);
            std::string error;
            if (!Bundle::CopyDirectory(startSlotDir, dest, error))
                log_error("[playtest] %s", error.c_str());
        }
    }
    StartupQuickstart = true;
    strncpyz(StartupQuickstartSave, fromSlot ? startSlot.c_str() : "", sizeof(StartupQuickstartSave));
    MapPane.SetClickWalk(clickwalk);
    if (replaying && replay.Header().revisited)
        RevisitedEnabled = true;

    if (bundleDir.empty())
        return;

    // The bundle: the INI as the game reads it, the start slot, the header.
    std::string error;
    std::error_code ec;
    fs::create_directories(bundleDir, ec);
    const fs::path ini = bundleDir / Bundle::kIni;
    if (!CopyFile(fs::path(SavePath) / Bundle::kIni, ini, error))
        log_error("[playtest] %s", error.c_str());

    header           = {};
    header.buildSha  = kBuildSha;
    header.buildName = kBuildName;
    if (clickwalk)
        header.patches.push_back(kClickWalk);
    header.fromSlot  = fromSlot;
    header.slot      = startSlot;
    if (fromSlot)
    {
        const fs::path copy = bundleDir / Bundle::kStartDir / startSlot;
        fs::remove_all(copy, ec);
        if (!Bundle::CopyDirectory(TSaveGame::SlotsDir() / startSlot, copy, error))
            log_error("[playtest] %s", error.c_str());
        header.slotSha = Bundle::SlotSha(copy);
    }
    header.iniSha    = TSha256::OfFile(ini);
    header.seed      = seed;
    header.timing    = timing;
    header.created   = NowUtc();
    header.by        = by.empty() ? "port" : by;
    header.revisited = RevisitedEnabled;
    writer.Open(bundleDir / Bundle::kRecording, header);
}

void TPlaytest::Started()
{
    if (!Active())
        return;
    started    = true;
    legacyBase = TTime::LegacyFrameCount();
    frame      = 0;
    current    = 0;
    boundary   = -1;
    nextCheck  = 0;
    drawsBase  = RandomDraws();
    InputQueue.Reset(kPointerStartX, kPointerStartY);
    // Each event as it dispatches: into the recording, and into the log with
    // its tick, where a developer lines it up with the game's own trace.
    InputQueue.SetObserver([this](const SInputEvent& event) {
        const int64_t when = timing == ETiming::Frames ? current : boundary;
        if (writer.IsOpen())
            writer.Event(when, event);
        JsonOut fields;
        fields.Begin('{');
        WriteEventFields(fields, event);
        fields.End('}');
        log_info("[input] t=%lld%s %s", (long long)boundary,
                 timing == ETiming::Frames ? (" f=" + std::to_string(current)).c_str() : "",
                 fields.str().c_str());
    });
    log_info("[playtest] tick 0: %s", Observation::CheckFields(drawsBase).c_str());
}

bool TPlaytest::Boot(std::string& error)
{
    booted = true;
    if (!boot || !boot() || !started)
    {
        error = "the game didn't start";
        return false;
    }
    return true;
}

// *************
// * The frame *
// *************

int64_t TPlaytest::Tick() const
{
    return TTime::LegacyFrameCount() - legacyBase;
}

bool TPlaytest::BeginFrame()
{
    if (mode == EMode::Control)
        Serve();
    if (!started || quitting || ended)
        return false;

    const bool replayRuns = replaying && !replayDone;
    if (mode == EMode::Control && !stepping && !replayRuns)
        return false;
    if (mode == EMode::Replay && !replayRuns)
        return false;

    // Watching: stepped ticks no faster than the pace. Waiting changes
    // nothing but when the next frame runs.
    if (pace > 0.0 && mode == EMode::Control)
    {
        const auto now = std::chrono::steady_clock::now();
        const auto period = std::chrono::duration_cast<std::chrono::steady_clock::duration>(
            std::chrono::duration<double>(1.0 / pace));
        if (now < lastPaced + period)
            return false;
        lastPaced = (now - lastPaced) < 2 * period ? lastPaced + period : now;
    }

    Boundary();
    if (replaying && replayDone)
        return false;
    current = frame++;
    return true;
}

// The tick boundary the frame starts at: the state after `t` ticks, before
// tick t's input. The check lines are written and compared here.
void TPlaytest::Boundary()
{
    const int64_t t = Tick();
    boundary = t;

    if (Player)
    {
        const S3DPoint pos = Player->Pos();
        log_debug("[playtest] t=%lld frame=%lld gameframes=%d draws=%llu screen=%s pos=(%d,%d,%d) health=%d",
                  (long long)t, (long long)frame, PlayScreen.GameFrame(),
                  (unsigned long long)(RandomDraws() - drawsBase), Observation::ScreenName(),
                  pos.x, pos.y, pos.z, Player->Health());
    }

    if (writer.IsOpen())
    {
        const bool due = timing == ETiming::Lockstep ? t % kCheckEvery == 0 : t >= nextCheck;
        if (due)
        {
            writer.Check(t, Observation::CheckFields(drawsBase));
            nextCheck = (t / kCheckEvery + 1) * kCheckEvery;
        }
    }

    if (replaying && !replayDone)
    {
        CompareCheck(t);
        const bool over = timing == ETiming::Lockstep
                        ? t >= (replayUntil >= 0 ? (std::min)(replayUntil, replay.End()) : replay.End())
                        : frame >= replay.Frames() || (replayUntil >= 0 && t >= replayUntil);
        if (diverged || over)
            FinishReplay();
    }
}

// A check line describes the first boundary at its tick (under frame timing
// several frames can begin at one tick); so is it compared.
void TPlaytest::CompareCheck(int64_t t)
{
    const JsonValue* recorded = replay.CheckAt(t);
    if (!recorded || !verify || diverged || t == lastCompared)
        return;
    lastCompared = t;
    JsonValue now;
    std::string error;
    if (!JsonValue::Parse(Observation::CheckFields(drawsBase), now, error))
        return;

    JsonOut fields;
    fields.Begin('{');
    bool differs = false;
    for (const auto& [key, value] : recorded->Members())
    {
        if (!now.Has(key))
            continue;           // a field one side lacks isn't compared
        const std::string was = value.Serialize();
        const std::string is  = now[key].Serialize();
        if (was == is)
            continue;
        differs = true;
        fields.Key(key.c_str()).Begin('[').Raw(was).Raw(is).End(']');
    }
    fields.End('}');
    if (!differs)
        return;
    diverged       = true;
    divergedAt     = t;
    divergedFields = fields.str();
    log_error("[playtest] replay diverged at t=%lld: %s", (long long)t, divergedFields.c_str());
}

double TPlaytest::FrameTime(double dt)
{
    if (replaying && !replayDone && timing == ETiming::Frames && current < replay.Frames())
        return replay.FrameDt(current);
    if (writer.IsOpen() && timing == ETiming::Frames)
        writer.Frame(current, dt);
    return dt;
}

void TPlaytest::BeforeDrain()
{
    if (!started)
        return;
    if (stepping)
        for (; stepNext < stepEvents.size() && stepEvents[stepNext].t <= boundary; ++stepNext)
            InputQueue.Push(stepEvents[stepNext].event);
    if (replaying && !replayDone)
        if (const auto* events = replay.EventsAt(timing == ETiming::Frames ? current : boundary))
            for (const SInputEvent& event : *events)
                InputQueue.Push(event);
    if (!watchDir.empty())
        WriteWatchFrame();
}

void TPlaytest::EndFrame()
{
    if (stepping && Tick() >= stepEnd)
        FinishStep();
}

void TPlaytest::FinishStep()
{
    stepping = false;
    stepEvents.clear();
    stepNext = 0;
    JsonOut out;
    out.Begin('{').Key("id").Raw(stepId.Serialize()).FieldBool("ok", true);
    out.Field("t", Tick()).End('}');
    rev_platform::WriteStdioPipeLine(out.str());
}

void TPlaytest::FinishReplay()
{
    replayDone = true;
    const int64_t t = Tick();
    if (diverged)
        log_error("[playtest] replay: diverged at t=%lld (%lld ticks run)", (long long)divergedAt, (long long)t);
    else
        log_info("[playtest] replay: %lld ticks, %s", (long long)t,
                 verify ? "every check line matches" : "not verified");

    if (replayAnswer)
    {
        JsonOut out;
        out.Begin('{').Key("id").Raw(replayId.Serialize()).FieldBool("ok", true);
        out.Field("t", t);
        if (verify)
        {
            out.Key("diverged");
            if (diverged)
                out.Begin('{').Field("t", divergedAt).Key("fields").Raw(divergedFields).End('}');
            else
                out.Null();
        }
        out.End('}');
        rev_platform::WriteStdioPipeLine(out.str());
        replayAnswer = false;
        return;
    }

    // --replay: the verdict on stdout, and the exit code.
    if (diverged)
        std::printf("replay: diverged at t=%lld: %s\n", (long long)divergedAt, divergedFields.c_str());
    else
        std::printf("replay: %lld ticks, %s, draws %llu\n", (long long)t,
                    verify ? "no divergence" : "not verified",
                    (unsigned long long)(RandomDraws() - drawsBase));
    std::fflush(stdout);
    exitCode = diverged ? 1 : 0;
    sapp_request_quit();
}

void TPlaytest::WriteWatchFrame()
{
    const fs::path dir = watchDir;
    Display.RequestCapture([dir](const uint8_t* rgba, int32_t w, int32_t h) {
        // Every tick: fast compression keeps up with a watched pace.
        const fs::path tmp = dir / "frame.tmp.png";
        if (!WritePng(tmp, rgba, w, h, 1))
            return;
        std::error_code ec;
        fs::rename(tmp, dir / "frame.png", ec);
    });
}

void TPlaytest::DrawOverlay() const
{
    if (caption.empty() || headless)
        return;
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, 8.0f), ImGuiCond_Always, ImVec2(0.5f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.65f);
    constexpr ImGuiWindowFlags kFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                                        ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav |
                                        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;
    if (ImGui::Begin("##playtest-caption", nullptr, kFlags))
        ImGui::TextUnformatted(caption.c_str());
    ImGui::End();
}

void TPlaytest::Shutdown()
{
    End();
}

void TPlaytest::End()
{
    if (ended)
        return;
    ended = true;
    if (!started || !writer.IsOpen())
        return;
    const int64_t t = Tick();
    writer.Check(t, Observation::CheckFields(drawsBase));
    writer.End(t);
    log_info("[playtest] recording ended at t=%lld", (long long)t);
}

// ***********************
// * The control channel *
// ***********************

void TPlaytest::Serve()
{
    std::string line;
    while (!stepping && !quitting && !(replaying && !replayDone))
    {
        if (!rev_platform::ReadStdioPipeLine(line))
        {
            if (rev_platform::StdioPipeClosed())
            {
                log_warn("[playtest] the director closed the channel; quitting");
                End();
                quitting = true;
                sapp_request_quit();
            }
            return;
        }
        if (!line.empty())
            Request(line);
    }
}

void TPlaytest::Respond(const JsonValue& id, bool ok, const std::string& members, const std::string& error)
{
    JsonOut out;
    out.Begin('{').Key("id").Raw(id.Serialize()).FieldBool("ok", ok);
    if (!ok)
        out.FieldString("error", error);
    out.End('}');
    std::string text = out.str();
    if (ok && !members.empty())
        text.insert(text.size() - 1, "," + members);
    rev_platform::WriteStdioPipeLine(text);
}

void TPlaytest::Request(const std::string& line)
{
    JsonValue req;
    std::string error;
    if (!JsonValue::Parse(line, req, error) || !req.IsObject())
    {
        log_warn("[playtest] not a request: %s", line.c_str());
        Respond(JsonValue(), false, {}, "not a JSON object: " + error);
        return;
    }
    const JsonValue& id = req["id"];
    const std::string cmd = req["cmd"].Str();
    log_info("[control] t=%lld %s %s", (long long)(started ? Tick() : 0), cmd.c_str(),
             line.size() > 300 ? (line.substr(0, 300) + "...").c_str() : line.c_str());

    // The members of the answer, written without the braces.
    JsonOut out;
    out.Begin('{');
    bool ok = false;
    if (cmd == "hello")
        ok = CmdHello(out);
    else if (cmd == "start")
        ok = CmdStart(req, error);
    else if (cmd == "step")
    {
        if (CmdStep(req, error))
        {
            stepId = id;        // answered when the ticks have run
            return;
        }
    }
    else if (cmd == "observe")
        ok = CmdObserve(req, out, error);
    else if (cmd == "project")
        ok = CmdProject(req, out, error);
    else if (cmd == "pick")
        ok = CmdPick(req, out, error);
    else if (cmd == "export_slot")
        ok = CmdExportSlot(req, out, error);
    else if (cmd == "mark")
        ok = CmdMark(req, out, error);
    else if (cmd == "replay")
    {
        replayId = id;
        if (CmdReplay(req, error))
            return;             // answered when the replay is over
    }
    else if (cmd == "caption")
        ok = CmdCaption(req, error);
    else if (cmd == "quit")
    {
        Respond(id, true, {});
        CmdQuit();
        return;
    }
    else if (cmd == "checkpoint" || cmd == "restore")
        error = cmd + " is retail only";
    else
        error = "unknown command '" + cmd + "'";

    if (cmd == "start" && ok)
        out.Field("t", Tick());
    out.End('}');
    std::string members = out.str();
    members = members.substr(1, members.size() - 2);
    Respond(id, ok, ok ? members : std::string(), error);
}

bool TPlaytest::CmdHello(JsonOut& out)
{
    out.FieldString("game", "port");
    out.Key("build").Begin('{');
    out.FieldString("sha", kBuildSha);
    out.FieldString("name", kBuildName);
    out.Key("patches").Begin('[');
    if (clickwalk)
        out.String(kClickWalk);
    out.End(']');
    out.End('}');
    out.Field("protocol", 1);
    out.Field("tick_hz", 24);
    out.Key("screen").Begin('[').Value(640).Value(480).End(']');
    out.Key("capabilities").Begin('[');
    for (const char* c : {"observe", "project", "pick", "export_slot", "mark", "replay", "caption"})
        out.String(c);
    out.End(']');
    return true;
}

bool TPlaytest::CmdStart(const JsonValue& req, std::string& error)
{
    if (booted)
    {
        error = "the game has started already (one session a process)";
        return false;
    }
    const std::string from = req["from"].Str();
    if (from != "newgame" && from != "slot")
    {
        error = "from must be newgame or slot";
        return false;
    }
    if (!req["seed"].IsInt())
    {
        error = "start needs a seed";
        return false;
    }
    seed     = uint32_t(req["seed"].Int());
    seedSet  = true;
    fromSlot = from == "slot";
    if (fromSlot)
    {
        startSlotDir = req["slot"].Str();
        std::error_code ec;
        if (startSlotDir.empty() || !fs::is_directory(startSlotDir, ec))
        {
            error = "no slot directory '" + req["slot"].Str() + "'";
            return false;
        }
        startSlot = fs::path(startSlotDir).lexically_normal().filename().string();
        if (startSlot.empty())
            startSlot = startSlotDir.parent_path().filename().string();
    }
    if (req.Has("ini"))
    {
        iniFile = req["ini"].Str();
        std::error_code ec;
        if (!fs::is_regular_file(iniFile, ec))
        {
            error = "no INI '" + iniFile.string() + "'";
            return false;
        }
    }
    if (req.Has("record"))
        bundleDir = req["record"].Str();
    by = req.Has("by") ? req["by"].Str() : std::string("director");
    return Boot(error);
}

bool TPlaytest::CmdStep(const JsonValue& req, std::string& error)
{
    if (!started)
    {
        error = "no game: start first";
        return false;
    }
    const int64_t ticks = req["ticks"].Int(0);
    if (!req["ticks"].IsInt() || ticks < 1)
    {
        error = "ticks must be 1 or more";
        return false;
    }
    std::vector<SStepEvent> events;
    for (const JsonValue& e : req["events"].Items())
    {
        const int64_t t = e["t"].Int(-1);
        if (!e["t"].IsInt() || t < 0 || t >= ticks)
        {
            error = "an event's t must be 0 <= t < ticks";
            return false;
        }
        SStepEvent step;
        step.t = Tick() + t;
        if (!ReadEventFields(e, step.event, error))
            return false;
        events.push_back(step);
    }
    std::stable_sort(events.begin(), events.end(),
                     [](const SStepEvent& a, const SStepEvent& b) { return a.t < b.t; });
    stepEvents = std::move(events);
    stepNext   = 0;
    stepEnd    = Tick() + ticks;
    stepping   = true;
    return true;
}

bool TPlaytest::CmdObserve(const JsonValue& req, JsonOut& out, std::string& error)
{
    out.Field("t", started ? Tick() : 0);
    const int32_t objects  = int32_t(req["objects"].Int(kObjectRadius));
    const int32_t walkgrid = req.Has("walkgrid") ? int32_t(req["walkgrid"].Int(-1)) : -1;
    Observation::Write(out, objects, walkgrid);

    if (req.Has("frame"))
    {
        const fs::path file = fs::absolute(req["frame"].Str());
        bool written = false;
        Display.CaptureNow([&](const uint8_t* rgba, int32_t w, int32_t h) {
            written = WritePng(file, rgba, w, h);
        });
        if (!written)
        {
            error = "can't capture the frame to " + file.string();
            return false;
        }
        out.FieldString("frame", file.string());
    }
    return true;
}

bool TPlaytest::CmdProject(const JsonValue& req, JsonOut& out, std::string& error)
{
    out.Key("screen").Begin('[');
    for (const JsonValue& p : req["points"].Items())
    {
        if (!p.IsArray() || p.Items().size() < 2)
        {
            error = "points are [x, y, z]";
            return false;
        }
        const S3DPoint world(int32_t(p[0].Int()), int32_t(p[1].Int()), int32_t(p[2].Int()));
        int32_t sx = 0, sy = 0;
        if (Observation::Project(world, sx, sy))
            out.Begin('[').Value(sx).Value(sy).End(']');
        else
            out.Null();
    }
    out.End(']');
    return true;
}

bool TPlaytest::CmdPick(const JsonValue& req, JsonOut& out, std::string& error)
{
    S3DPoint pos;
    int32_t object = -1;
    if (!Observation::Pick(int32_t(req["x"].Int()), int32_t(req["y"].Int()), pos, object))
    {
        error = "no map on screen";
        return false;
    }
    out.Key("pos").Begin('[').Value(pos.x).Value(pos.y).Value(pos.z).End(']');
    out.Key("object");
    if (object >= 0)
        out.Value(object);
    else
        out.Null();
    return true;
}

bool TPlaytest::CmdExportSlot(const JsonValue& req, JsonOut& out, std::string& error)
{
    std::string slot = req["slot"].Str();
    if (slot == "@last")
    {
        slot = ::SaveGame.LastSaved();
        if (slot.empty())
        {
            error = "no slot written yet";
            return false;
        }
    }
    if (slot.empty() || !req["to"].IsString())
    {
        error = "export_slot needs slot and to";
        return false;
    }
    const fs::path to = fs::absolute(req["to"].Str());
    if (!Bundle::CopyDirectory(TSaveGame::SlotsDir() / slot, to, error))
        return false;
    out.FieldString("path", to.string());
    out.FieldString("slot", slot);
    return true;
}

bool TPlaytest::CmdMark(const JsonValue& req, JsonOut& out, std::string& error)
{
    if (!started)
    {
        error = "no game: start first";
        return false;
    }
    const std::string kind = req["kind"].Str();
    if (kind.empty())
    {
        error = "a mark needs a kind";
        return false;
    }
    if (writer.IsOpen())
    {
        // The request's other fields go with the mark.
        JsonOut extra;
        extra.Begin('{');
        for (const auto& [key, value] : req.Members())
            if (key != "id" && key != "cmd" && key != "kind")
                extra.Key(key.c_str()).Raw(value.Serialize());
        extra.End('}');
        JsonValue fields;
        if (!JsonValue::Parse(extra.str(), fields, error))
            return false;
        writer.Mark(Tick(), kind, fields);
        writer.Check(Tick(), Observation::CheckFields(drawsBase));
    }
    out.Field("t", Tick());
    return true;
}

bool TPlaytest::CmdReplay(const JsonValue& req, std::string& error)
{
    if (booted)
    {
        error = "a replay boots its own game: send it to a fresh host, before start";
        return false;
    }
    verify      = req["verify"].Bool();
    replayUntil = req.Has("until") ? req["until"].Int(-1) : -1;
    if (!LoadReplay(fs::path(req["bundle"].Str()), error))
        return false;
    replayAnswer = true;
    return Boot(error);
}

bool TPlaytest::CmdCaption(const JsonValue& req, std::string& error)
{
    if (!req["text"].IsString())
    {
        error = "caption needs text";
        return false;
    }
    caption = req["text"].Str();
    if (!watchDir.empty())
    {
        std::error_code ec;
        fs::create_directories(watchDir, ec);
        if (FILE* f = std::fopen((watchDir / "caption.txt").string().c_str(), "wb"))
        {
            std::fwrite(caption.data(), 1, caption.size(), f);
            std::fclose(f);
        }
    }
    return true;
}

void TPlaytest::CmdQuit()
{
    End();
    quitting = true;
    sapp_request_quit();
}
