// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *       retailab.cpp - the port's side of the retail A/B dumps          *
// *************************************************************************
//
// See retailab.h and docs/gameflow/RETAIL_AB.md. Each target mirrors the
// retail fixture of the same name in the emulator's gameflow slot
// (tools/retail_runtime/slots/gameflow/ in the main checkout): same cases,
// same JSON schema, so the compare is a plain diff of the two dumps.

#include "retailab.h"
#include "retailab_json.h"

#include "audio_backend.h"
#include "character.h"
#include "command.h"
#include "dialog.h"
#include "jsonout.h"
#include "logging.h"
#include "mappane.h"
#include "parse.h"
#include "playscreen.h"
#include "script.h"
#include "textencoding.h"

#include <optional>

#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// The command table (command.cpp); the script-step target swaps its
// handlers for its runs and puts them back.
extern SCommand Commands[];

namespace RetailAB
{
namespace
{

// Game text (Windows-1252) as UTF-8, as the retail fixture decodes it.
std::string GameText(const char* text)
{
    return text ? ToUtf8(text) : std::string();
}

// ---- What the text bar was sent -----------------------------------------

// TTextBar::VPrint logs every message as "[textbar] <text>" (UTF-8), open bar
// or not: the port's counterpart of the retail fixture's 0x0054d170 boundary.
std::vector<std::string> g_textbar;

void CaptureLog(log_Event* ev)
{
    if (ev->level != LOG_DEBUG || !ev->fmt || std::strncmp(ev->fmt, "[textbar] ", 10) != 0)
        return;
    char buf[2048];
    vsnprintf(buf, sizeof(buf), ev->fmt, ev->ap);
    g_textbar.emplace_back(buf + 10);
}

// The text bar messages [from, to) (to = -1: all since `from`).
void WriteErrors(JsonOut& j, size_t from, size_t to = size_t(-1))
{
    j.Key("errors").Begin('[');
    const size_t end = to < g_textbar.size() ? to : g_textbar.size();
    for (size_t i = from; i < end; ++i)
    {
        j.Begin('{');
        j.FieldString("channel", "textbar");
        j.FieldString("text", g_textbar[i]);
        j.End('}');
    }
    j.End(']');
}

// ---- Cases ---------------------------------------------------------------

bool ReadCases(const std::string& file, std::vector<Case>& cases)
{
    std::ifstream in(file);
    if (!in)
        return false;
    std::string line;
    while (std::getline(in, line))
    {
        if (line.empty() || line[0] == '#')
            continue;
        Case c;
        std::istringstream fields(line);
        std::getline(fields, c.name, '\t');
        for (std::string field; std::getline(fields, field, '\t');)
            c.fields.push_back(field);
        cases.push_back(c);
    }
    return true;
}

std::string FromHex(const std::string& hex)
{
    std::string out;
    for (size_t i = 0; i + 1 < hex.size(); i += 2)
        out.push_back((char)std::stoi(hex.substr(i, 2), nullptr, 16));
    return out;
}

bool ReadFile(const std::string& path, std::string& data)
{
    std::ifstream in(path, std::ios::binary);
    if (!in)
        return false;
    std::ostringstream s;
    s << in.rdbuf();
    data = s.str();
    return true;
}

// ---- Target 1: TScriptProto::ParseScript ---------------------------------

// Labels as the retail fixture finds them in a prototype's text: a line
// whose first non-blank is ':' then a name. Each name once (case-blind).
std::vector<std::string> FindLabels(const char* text, int32_t len)
{
    std::vector<std::string> labels;
    std::vector<std::string> seen;
    int32_t i = 0;
    while (i < len)
    {
        int32_t p = i;
        while (p < len && (text[p] == ' ' || text[p] == '\t'))
            ++p;
        if (p < len && text[p] == ':')
        {
            ++p;
            while (p < len && (text[p] == ' ' || text[p] == '\t'))
                ++p;
            const int32_t b = p;
            if (p < len && (isalpha((unsigned char)text[p]) || text[p] == '_'))
                while (p < len && (isalnum((unsigned char)text[p]) || text[p] == '_'))
                    ++p;
            if (p > b)
            {
                std::string label(text + b, p - b);
                bool dup = false;
                for (const std::string& s : seen)
                    dup = dup || stricmp(s.c_str(), label.c_str()) == 0;
                if (!dup)
                {
                    seen.push_back(label);
                    labels.push_back(label);
                }
            }
        }
        while (i < len && text[i] != '\n')
            ++i;
        ++i;
    }
    return labels;
}

void DumpProto(JsonOut& j, TScriptProto* proto, const std::string& source, size_t searchfrom,
               int32_t result, int32_t line, size_t& textend)
{
    // The port keeps no text start; its text is a verbatim copy of the file
    // after the last prototype, so find it there.
    long long textstart = -1;
    if (proto->text && proto->len > 0)
    {
        const size_t at = source.find(std::string(proto->text, proto->len), searchfrom);
        if (at != std::string::npos)
        {
            textstart = (long long)at;
            textend = at + proto->len;
        }
    }

    j.Begin('{');
    j.Field("line", line);
    j.Field("result", result);
    j.Key("name");
    if (proto->name)
        j.String(GameText(proto->name));
    else
        j.Null();
    j.FieldBool("parent", proto->parent != nullptr);
    j.Key("text_start");
    if (textstart >= 0)
        j.Value(textstart);
    else
        j.Null();
    j.Field("text_len", proto->len);
    j.FieldBool("text_is_file_span", textstart >= 0);
    j.Field("numtriggers", proto->numtriggers);

    j.Key("triggers").Begin('[');
    for (int32_t i = 0; i < proto->triggers.NumItems(); ++i)
    {
        if (!proto->triggers.Used(i))
            continue;
        const SScriptTrigger& st = proto->triggers[i];
        j.Begin('{');
        j.Field("type", st.type);
        j.Field("pos", st.pos);
        char name[MAXSCRIPTNAME + 1] = {};
        memcpy(name, st.name, MAXSCRIPTNAME);
        j.FieldString("name", GameText(name));
        j.Key("cube").Begin('[');
        j.Value(st.cube.beg.x).Value(st.cube.beg.y).Value(st.cube.beg.z);
        j.Value(st.cube.end.x).Value(st.cube.end.y).Value(st.cube.end.z);
        j.End(']');
        j.Field("dist", st.dist);
        j.Field("field38", st.priority);
        char region[MAXSCRIPTNAME + 1] = {};
        memcpy(region, st.region, MAXSCRIPTNAME);
        j.FieldString("region", GameText(region));
        j.End('}');
    }
    j.End(']');

    j.Key("variables").Begin('[');
    for (const SScriptVariable& var : proto->variables)
    {
        j.Begin('{');
        j.Field("type", (int32_t)var.type);
        j.FieldString("name", GameText(var.name.c_str()));
        j.Field("number", var.number);
        j.FieldString("text", GameText(var.text.c_str()));
        j.End('}');
    }
    j.End(']');

    // Each label through the port's TScript::Jump, as the fixture does with
    // retail's 0x00493fa0: where the script goes on, at what depth.
    j.Key("labels").Begin('[');
    if (proto->text)
    {
        for (const std::string& label : FindLabels(proto->text, proto->len))
        {
            const size_t first = g_textbar.size();
            TScript script(proto);
            script.Jump(nullptr, label.c_str());
            j.Begin('{');
            j.FieldString("label", GameText(label.c_str()));
            j.Field("found", script.Ip() >= 0 ? 1 : 0);
            j.Key("ip");
            if (script.Ip() >= 0)
                j.Value(script.Ip());
            else
                j.Null();
            j.Field("depth", script.Depth());
            WriteErrors(j, first);
            j.End('}');
        }
    }
    j.End(']');
}

// Fields: path, filename.
std::string ScriptParse(const Case& c, std::string& error)
{
    std::string source;
    if (!ReadFile(c.Field(0), source))
    {
        error = "can't read " + c.Field(0);
        return {};
    }
    std::vector<char> buffer(source.begin(), source.end());
    buffer.push_back('\0');
    const std::string& name = c.Field(1).empty() ? c.name : c.Field(1);
    std::vector<char> filename(name.begin(), name.end());
    filename.push_back('\0');

    g_textbar.clear();
    JsonOut j;
    j.Begin('{');
    j.FieldString("schema", "gameflow.scriptparse.v1");
    j.FieldString("side", "port");
    j.FieldString("case", c.name);
    j.Key("protos").Begin('[');

    // TScriptManager::ParseScripts' loop, without the manager.
    TStringParseStream s(buffer.data(), (int32_t)source.size());
    TToken t(s);
    t.Get();
    size_t textend = 0;
    std::vector<std::unique_ptr<TScriptProto>> protos;
    while (t.Type() != TKN_EOF)
    {
        const int32_t line = t.LineNum();
        const size_t first = g_textbar.size();
        protos.push_back(std::make_unique<TScriptProto>(nullptr, nullptr, filename.data(), nullptr));
        TScriptProto* proto = protos.back().get();
        const int32_t result = proto->ParseScript(t);
        // The parse's errors go with the prototype; each label jump records
        // its own (after them).
        const size_t parsed = g_textbar.size();
        DumpProto(j, proto, source, textend, result, line, textend);
        WriteErrors(j, first, parsed);
        j.End('}');
        if (protos.size() > 10000)
        {
            error = "ParseScript made no progress";
            return {};
        }
    }
    j.End(']');
    j.End('}');
    return j.str();
}

// ---- Target 2: how long Say holds a line ---------------------------------

// What TCharacter::Say sets for one variant: the voice found with `voicems`
// (its decoded length), or not found (0).
void SayVariant(JsonOut& j, const char* key, int32_t frames, int32_t voicems, const char* line)
{
    const int32_t ticks = TCharacter::SpeechTicks(frames, voicems, line);
    j.Key(key).Begin('{');
    j.Field("wait", ticks);             // the say action's wait
    j.Field("ticks", ticks);            // what DialogPane.AddSpeech gets (the same)
    j.Field("line_len", (int32_t)strlen(line));
    j.End('}');
}

// Fields: frames, sound name ("" = none), voice file ("" = none), text (hex),
// and optionally a voice-length sweep "first:last:step" (ms).
std::string SayDuration(const Case& c, std::string& error)
{
    const int32_t frames = std::stoi(c.Field(0));
    const std::string& sound = c.Field(1);
    const std::string& voicepath = c.Field(2);
    const std::string text = FromHex(c.Field(3));

    char line[256];
    DialogLine(text.c_str(), line, sizeof(line));

    // The voice's length as TSoundPlayer::SampleLengthMs measures it.
    long long voicems = -1;
    if (!voicepath.empty())
    {
        std::string bytes;
        if (!ReadFile(voicepath, bytes))
        {
            error = "can't read " + voicepath;
            return {};
        }
        const std::optional<uint32_t> ms =
            audio::DecodedLengthMs(reinterpret_cast<const uint8_t*>(bytes.data()), bytes.size());
        voicems = ms ? (long long)*ms : 0;
    }

    JsonOut j;
    j.Begin('{');
    j.FieldString("schema", "gameflow.sayduration.v1");
    j.FieldString("side", "port");
    j.FieldString("case", c.name);
    j.Key("voice_ms");
    if (voicems >= 0)
        j.Value(voicems);
    else
        j.Null();
    SayVariant(j, "novoice", frames, 0, line);
    if (!sound.empty() && voicems >= 0)
        SayVariant(j, "voice", frames, (int32_t)voicems, line);

    const std::string& sweep = c.Field(4);
    if (!sweep.empty())
    {
        int32_t first = 0, last = 0, step = 1;
        if (sscanf(sweep.c_str(), "%d:%d:%d", &first, &last, &step) != 3 || step <= 0)
        {
            error = "bad sweep " + sweep;
            return {};
        }
        j.Key("sweep").Begin('[');
        for (int32_t ms = first; ms <= last; ms += step)
            j.Value(TCharacter::SpeechTicks(frames, ms, line));
        j.End(']');
    }
    j.End('}');
    return j.str();
}

// ---- Target 3: the dialog pane's layout over ticks -----------------------

// "Dialog": font.def height plus LEXTRA (DIALOG.md §4.2).
constexpr int32_t kDialogLineHeight = 20;

struct SLayoutEvent
{
    int32_t tick = 0;
    bool add = true;
    int32_t mode = 1;                   // add: 1 NPC, 2 player
    int32_t life = -1;                  // add: ticks, -1 no timeout
    std::vector<int32_t> lines;         // add: each text's line count
    int32_t entry = 0;                  // dismiss: which (creation order)
};

// "tick:add:mode:life:n,n,...;tick:dismiss:entry;..."
bool ParseLayoutEvents(const std::string& text, std::vector<SLayoutEvent>& events)
{
    std::istringstream all(text);
    for (std::string item; std::getline(all, item, ';');)
    {
        std::vector<std::string> f;
        std::istringstream parts(item);
        for (std::string p; std::getline(parts, p, ':');)
            f.push_back(p);
        SLayoutEvent ev;
        if (f.size() == 5 && f[1] == "add")
        {
            ev.tick = std::stoi(f[0]);
            ev.mode = std::stoi(f[2]);
            ev.life = std::stoi(f[3]);
            std::istringstream counts(f[4]);
            for (std::string n; std::getline(counts, n, ',');)
                ev.lines.push_back(std::stoi(n));
        }
        else if (f.size() == 3 && f[1] == "dismiss")
        {
            ev.tick = std::stoi(f[0]);
            ev.add = false;
            ev.entry = std::stoi(f[2]);
        }
        else
            return false;
        events.push_back(ev);
    }
    return true;
}

// Fields: tick count, events. A fresh pane (not the game's), measuring with
// the Dialog line height and no atlas: each text is as many lines as asked.
std::string DialogLayout(const Case& c, std::string& error)
{
    const int32_t ticks = std::stoi(c.Field(0));
    std::vector<SLayoutEvent> events;
    if (!ParseLayoutEvents(c.Field(1), events))
    {
        error = "bad events " + c.Field(1);
        return {};
    }

    TDialogPane pane;
    pane.UseFont(nullptr, kDialogLineHeight);
    std::vector<const TDialogEntry*> created;

    JsonOut j;
    j.Begin('{');
    j.FieldString("schema", "gameflow.dialoglayout.v1");
    j.FieldString("side", "port");
    j.FieldString("case", c.name);
    int32_t mx = 0, my = 0, mw = 0, mh = 0;
    PlayScreen.GetMapViewRect(mx, my, mw, mh);
    j.Key("map").Begin('[').Value(mx).Value(my).Value(mw).Value(mh).End(']');

    std::vector<int32_t> heights;
    j.Key("ticks").Begin('[');
    for (int32_t tick = 0; tick < ticks; ++tick)
    {
        for (const SLayoutEvent& ev : events)
        {
            if (ev.tick != tick)
                continue;
            if (ev.add)
            {
                std::vector<std::string> texts;
                for (int32_t n : ev.lines)
                {
                    std::string text = "line";
                    for (int32_t i = 1; i < n; ++i)
                        text += "\nline";
                    texts.push_back(text);
                }
                const TDialogEntry& entry = pane.AddEntry(nullptr, (TDialogEntry::EMode)ev.mode, 0xffffff,
                                                          texts, {}, ev.life);
                created.push_back(&entry);
                heights.push_back(entry.Height());
            }
            else if (ev.entry >= 0 && ev.entry < (int32_t)created.size())
            {
                for (const std::unique_ptr<TDialogEntry>& e : pane.Entries())
                    if (e.get() == created[ev.entry])
                        e->Dismiss();
            }
        }

        pane.Pulse();

        j.Begin('[');
        for (const std::unique_ptr<TDialogEntry>& e : pane.Entries())
        {
            int32_t id = -1;
            for (size_t i = 0; i < created.size(); ++i)
                if (created[i] == e.get())
                    id = (int32_t)i;
            const TDialogEntry::SPlacement p = e->Placement();
            j.Begin('{');
            j.Field("id", id);
            j.Field("ticksleft", p.ticksleft);
            j.Field("basex", p.basex).Field("basey", p.basey);
            j.Field("offx", p.offx).Field("offy", p.offy);
            j.Field("targetx", p.targetx).Field("targety", p.targety);
            j.Field("posx", p.posx).Field("posy", p.posy);
            j.Field("stepx", p.stepx).Field("stepy", p.stepy);
            j.FieldBool("dismissed", e->IsDismissed());
            j.Field("fade", e->Fade());
            j.End('}');
        }
        j.End(']');
    }
    j.End(']');
    j.Key("heights").Begin('[');
    for (int32_t h : heights)
        j.Value(h);
    j.End(']');
    j.End('}');
    return j.str();
}

// ---- Target 4: block stepping (TScript::Continue) -------------------------

// What `if` and `while` answer in a stepping run: every condition true, or
// every one false.
bool g_conditions = true;

// The handlers of a stepping run, as the retail fixture's boundaries answer:
// a command succeeds and does nothing; `if` / `while` give the run's outcome
// (retail 0x0041fb70 / 0x0041fbc0 encode a condition this way); `jump`
// jumps the running script, as CmdJump does through its context's script
// (retail 0x00420c70 through 0x00471290; the fixture's every object is the
// script's owner).
COMMAND(StepCommand)
{
    return 0;
}

// `wait response` and its variants (retail 0x0041fe30) also hold the
// script, so the run stops where it would wait for the player's choice;
// what the choice jumps to is a label run. (A frame wait: a response wait
// would open the dialog pane.)
COMMAND(StepWait)
{
    if (script && (t.Is("response") || t.Is("responsenohide") || t.Is("respnohide") || t.Is("respctrlon")))
        script->WaitFrames(1);
    return 0;
}

COMMAND(StepIf)
{
    return g_conditions ? CMD_CONDTRUE : CMD_CONDFALSE;
}

COMMAND(StepWhile)
{
    return g_conditions ? CMD_LOOP : CMD_SKIPBLOCK;
}

COMMAND(StepJump)
{
    if (t.Type() != TKN_IDENT && t.Type() != TKN_KEYWORD)
        return CMD_BADPARAMS;
    if (script)
        script->Jump(context, t.Text());
    return CMD_JUMP;
}

// The command table for a stepping run, put back after it: begin, end and
// else keep their handlers, if/while/jump get the ones above, every other
// command StepCommand; class contexts are dropped so every handler is
// reached (the fixture does the same to retail's table).
class StepCommands
{
  public:
    StepCommands()
    {
        for (int32_t i = 0; Commands[i].name; ++i)
        {
            SCommand& c = Commands[i];
            saved.push_back(c);
            if (!stricmp(c.name, "if"))
                c.cmdfunc = StepIf;
            else if (!stricmp(c.name, "while"))
                c.cmdfunc = StepWhile;
            else if (!stricmp(c.name, "jump"))
                c.cmdfunc = StepJump;
            else if (!stricmp(c.name, "wait"))
                c.cmdfunc = StepWait;
            else if (stricmp(c.name, "begin") && stricmp(c.name, "end") && stricmp(c.name, "else"))
                c.cmdfunc = StepCommand;
            c.classcontext = c.classcontext2 = -1;
        }
    }
    ~StepCommands()
    {
        for (size_t i = 0; i < saved.size(); ++i)
            Commands[i] = saved[i];
    }
    StepCommands(const StepCommands&) = delete;
    StepCommands& operator=(const StepCommands&) = delete;

  private:
    std::vector<SCommand> saved;
};

// The lines a run hands to the interpreter (the fixture records them at
// 0x00493942 / 0x00493947). Past the caps -- interpreter calls, and jumps,
// whose loops retail pays for by re-reading the text from the top -- it
// pauses the script, so the loop leaves after that line, as the fixture does.
class StepRecorder : public TScript::IStepObserver
{
  public:
    static constexpr size_t kCap = 256;
    static constexpr int32_t kJumpCap = 8;
    struct SLine
    {
        int32_t at = 0;                 // after the line's first token
        int32_t depth = 0;              // before the line
        int32_t bits = -1;              // what the interpreter returned
    };
    std::vector<SLine> lines;
    int32_t jumps = 0;

    void Reset()
    {
        lines.clear();
        jumps = 0;
    }
    [[nodiscard]] bool Truncated() const { return lines.size() > kCap || jumps > kJumpCap; }

    void BeforeLine(TScript& script, int32_t offset) override
    {
        lines.push_back({offset, script.Depth()});
        if (lines.size() > kCap)
            script.Break();
    }
    void AfterLine(TScript& script, int32_t bits) override
    {
        lines.back().bits = bits;
        if ((bits & CMD_JUMP) && ++jumps > kJumpCap)
            script.Break();
    }
};

void WriteTexts(JsonOut& j, const char* key, size_t from)
{
    j.Key(key).Begin('[');
    for (size_t i = from; i < g_textbar.size(); ++i)
        j.String(g_textbar[i]);
    j.End(']');
}

void WriteIp(JsonOut& j, const TScript& script)
{
    j.Key("ip");
    if (script.Ip() >= 0)
        j.Value(script.Ip());
    else
        j.Null();
    j.Field("depth", script.Depth());
}

// Continue once (commanddone) and the run's record, as the fixture's.
void StepContinue(JsonOut& j, TScript& script, StepRecorder& recorder)
{
    recorder.Reset();
    const size_t first = g_textbar.size();
    script.Continue(nullptr, true);
    j.Key("lines").Begin('[');
    for (const StepRecorder::SLine& line : recorder.lines)
    {
        j.Begin('{');
        j.Field("at", line.at);
        j.Field("depth", line.depth);
        j.Field("bits", line.bits);
        j.End('}');
    }
    j.End(']');
    j.FieldBool("truncated", recorder.Truncated());
    j.Key("end").Begin('{');
    WriteIp(j, script);
    j.FieldBool("paused", script.IsPaused());
    j.End('}');
    j.FieldBool("waiting", script.IsWaiting());
    WriteTexts(j, "errors", first);
}

// One prototype's runs: its trigger blocks from their start and its labels
// from a Jump, every condition true, then every one false.
void StepRuns(JsonOut& j, TScriptProto* proto, const char* filename, StepRecorder& recorder)
{
    TScriptProto empty(nullptr, nullptr, (char*)filename, nullptr);  // no trigger fires
    for (const bool conditions : {true, false})
    {
        g_conditions = conditions;
        const std::string policy = conditions ? "true" : "false";
        int32_t n = 0;
        for (int32_t i = 0; i < proto->triggers.NumItems(); ++i)
        {
            if (!proto->triggers.Used(i))
                continue;
            SScriptTrigger& st = proto->triggers[i];
            TScript script(&empty);
            script.StartTrigger(nullptr, proto, &st);
            j.Begin('{');
            j.FieldString("key", "trigger " + std::to_string(n) + " " + policy);
            j.FieldString("kind", "trigger");
            j.Field("id", n);
            j.Field("type", st.type);
            j.FieldString("policy", policy);
            j.Key("start").Begin('{');
            WriteIp(j, script);
            j.End('}');
            StepContinue(j, script, recorder);
            j.End('}');
            ++n;
        }
        for (const std::string& label : FindLabels(proto->text, proto->len))
        {
            TScript script(&empty);
            script.Start(proto, -1);            // the current prototype, not running
            const size_t first = g_textbar.size();
            script.Jump(nullptr, label.c_str());
            std::string key = label;
            for (char& ch : key)
                ch = (char)tolower((unsigned char)ch);
            j.Begin('{');
            j.FieldString("key", "label " + GameText(key.c_str()) + " " + policy);
            j.FieldString("kind", "label");
            j.FieldString("id", GameText(label.c_str()));
            j.FieldString("policy", policy);
            j.Key("start").Begin('{');
            j.Field("found", script.Ip() >= 0 ? 1 : 0);
            WriteIp(j, script);
            WriteTexts(j, "errors", first);
            j.End('}');
            StepContinue(j, script, recorder);
            j.End('}');
        }
    }
}

// Fields: path, filename.
std::string ScriptStep(const Case& c, std::string& error)
{
    std::string source;
    if (!ReadFile(c.Field(0), source))
    {
        error = "can't read " + c.Field(0);
        return {};
    }
    std::vector<char> buffer(source.begin(), source.end());
    buffer.push_back('\0');
    const std::string& name = c.Field(1).empty() ? c.name : c.Field(1);
    std::vector<char> filename(name.begin(), name.end());
    filename.push_back('\0');

    g_textbar.clear();
    const StepCommands commands;
    StepRecorder recorder;
    TScript::SetStepObserver(&recorder);

    // TScriptManager::ParseScripts' loop, without the manager.
    TStringParseStream s(buffer.data(), (int32_t)source.size());
    TToken t(s);
    t.Get();
    std::vector<std::unique_ptr<TScriptProto>> protos;
    std::vector<int32_t> results;
    while (t.Type() != TKN_EOF && protos.size() <= 10000)
    {
        protos.push_back(std::make_unique<TScriptProto>(nullptr, nullptr, filename.data(), nullptr));
        results.push_back(protos.back()->ParseScript(t));
    }

    JsonOut j;
    j.Begin('{');
    j.FieldString("schema", "gameflow.scriptstep.v1");
    j.FieldString("side", "port");
    j.FieldString("case", c.name);
    j.Field("cap", (int32_t)StepRecorder::kCap);
    j.Key("protos").Begin('[');
    size_t textend = 0;
    for (size_t index = 0; index < protos.size(); ++index)
    {
        TScriptProto* proto = protos[index].get();
        // The port keeps no text start: find the text after the last one.
        long long textstart = -1;
        if (proto->text && proto->len > 0)
        {
            const size_t at = source.find(std::string(proto->text, proto->len), textend);
            if (at != std::string::npos)
            {
                textstart = (long long)at;
                textend = at + proto->len;
            }
        }
        j.Begin('{');
        j.Field("index", (int32_t)index);
        j.Key("name");
        if (proto->name)
            j.String(GameText(proto->name));
        else
            j.Null();
        j.Field("result", results[index]);
        j.Key("text_start");
        if (textstart >= 0)
            j.Value(textstart);
        else
            j.Null();
        j.Field("text_len", proto->len);
        j.Key("runs").Begin('[');
        if (results[index] >= 0 && proto->text)
            StepRuns(j, proto, filename.data(), recorder);
        j.End(']');
        j.End('}');
    }
    j.End(']');
    j.End('}');
    TScript::SetStepObserver(nullptr);
    return j.str();
}

// ---- Target 5: the trigger test (TScript::Triggered) ----------------------

std::vector<std::string> Split(const std::string& text, char sep)
{
    std::vector<std::string> parts;
    std::string part;
    std::istringstream in(text);
    while (std::getline(in, part, sep))
        parts.push_back(part);
    if (!text.empty() && text.back() == sep)
        parts.emplace_back();
    return parts;
}

S3DPoint ParsePoint(const std::string& text)
{
    const std::vector<std::string> v = Split(text, ',');
    return {std::stoi(v.at(0)), std::stoi(v.at(1)), std::stoi(v.at(2))};
}

// A fixture object: a name, a class, a level and a position, registered
// under its id so a TSafeRef finds it. Built without imagery or a class
// record, and unwound as such (class -1).
template <typename Base>
class TFixtureObject : public Base
{
  public:
    TFixtureObject(const std::string& name, int32_t objclass, int32_t level, const S3DPoint& pos, int32_t id)
        : Base(nullptr)
    {
        this->objclass = (short)objclass;
        this->level = (uint16_t)level;
        this->ForcePos(pos);
        this->name = _strdup(name.c_str());
        this->mapindex = id;
        MapPane.RegisterInstance(this, id);
    }
    ~TFixtureObject() override
    {
        free(this->name);
        this->name = nullptr;
        this->objclass = -1;
    }
    TFixtureObject(const TFixtureObject&) = delete;
    TFixtureObject& operator=(const TFixtureObject&) = delete;
};

// The trigger test's world: its objects in list order (all moving) and the
// main player among them or none. ObjectInCube is MapPane.ObjectInCube's
// loop -- the first object on the level asked inside the cube, by
// S3DRect::In -- over these objects, as the retail fixture's iterator hands
// out the world's objects on that level.
class TFixtureWorld : public TScript::ITriggerWorld
{
  public:
    std::vector<TObjectInstance*> objects;
    TObjectInstance* player = nullptr;

    TObjectInstance* MainPlayer() const override { return player; }
    TObjectInstance* ObjectInCube(PS3DRect cube, int32_t level, int32_t /*objset*/) const override
    {
        for (TObjectInstance* o : objects)
            if (o->GetLevel() == level && cube->In(o->Pos()))
                return o;
        return nullptr;
    }
};

}  // namespace

// Sets a script's trigger request and guard as a case says, runs the trigger
// test and reads back what it recorded (TScript befriends it).
class TriggerProbe
{
  public:
    struct SResult
    {
        bool fires = false;
        TObjectInstance* user = nullptr;
        std::string alias;
        int32_t guard = -1;
    };

    static SResult Run(SScriptTrigger& st, int32_t priority, TObjectInstance* owner, int32_t type,
                       const std::string& str, const std::string& str2, const TSafeRef<TObjectInstance>& guard)
    {
        TScript script(nullptr);
        script.newtrigger = type;
        strncpy(script.newtriggerstr, str.c_str(), MAXSCRIPTNAME - 1);
        strncpy(script.newtriggerstr2, str2.c_str(), MAXSCRIPTNAME - 1);
        script.triggerguard = guard;
        SResult r;
        r.fires = script.Triggered(&st, priority, owner);
        r.user = script.triggerer.Get();
        r.alias = script.useralias;
        r.guard = script.triggerguard.Id();
        return r;
    }
};

namespace
{

// Fields: trigger "type|name|x0,y0,z0,x1,y1,z1|dist|priority"; request
// "type|str|str2|guard|running"; the priority Continue passes; the world
// "id|name|class|level|x,y,z;..." in list order; the player's id ("" none);
// the owner's id. Names and strings are hex (Windows-1252).
std::string TriggerTest(const Case& c, std::string& error)
{
    const std::vector<std::string> t = Split(c.Field(0), '|');
    const std::vector<std::string> r = Split(c.Field(1), '|');
    if (t.size() < 5 || r.size() < 5)
    {
        error = "bad case fields";
        return {};
    }
    SScriptTrigger st;
    st.type = std::stoi(t[0]);
    strncpy(st.name, FromHex(t[1]).c_str(), MAXSCRIPTNAME - 1);
    const std::vector<std::string> cube = Split(t[2], ',');
    st.cube.beg = {std::stoi(cube.at(0)), std::stoi(cube.at(1)), std::stoi(cube.at(2))};
    st.cube.end = {std::stoi(cube.at(3)), std::stoi(cube.at(4)), std::stoi(cube.at(5))};
    st.dist = std::stoi(t[3]);
    st.priority = std::stoi(t[4]);

    // The world.
    std::vector<std::unique_ptr<TObjectInstance>> objects;
    std::vector<std::string> ids;
    TObjectInstance* owner = nullptr;
    TFixtureWorld world;
    int32_t index = 0;
    for (const std::string& spec : Split(c.Field(3), ';'))
    {
        const std::vector<std::string> f = Split(spec, '|');
        objects.push_back(std::make_unique<TFixtureObject<TObjectInstance>>(
            FromHex(f.at(1)), std::stoi(f.at(2)), std::stoi(f.at(3)), ParsePoint(f.at(4)), 100 + index++));
        ids.push_back(f[0]);
        world.objects.push_back(objects.back().get());
        if (f[0] == c.Field(4))
            world.player = objects.back().get();
        if (f[0] == c.Field(5))
            owner = objects.back().get();
    }
    if (!owner)
    {
        error = "no owner";
        return {};
    }

    // The guard: the object that set off the running trigger, by id.
    std::unique_ptr<TObjectInstance> guardobject;
    TSafeRef<TObjectInstance> guard;
    if (r[3] == "exists")
    {
        guardobject = std::make_unique<TFixtureObject<TObjectInstance>>("Guard", 12, 0, S3DPoint{0, 0, 0}, 50);
        guard = guardobject.get();
    }
    else if (r[3] == "gone")
        guard = TSafeRef<TObjectInstance>(50, 1);

    TScript::SetTriggerWorld(&world);
    const TriggerProbe::SResult result = TriggerProbe::Run(st, std::stoi(c.Field(2)), owner, std::stoi(r[0]),
                                                           FromHex(r[1]), FromHex(r[2]), guard);
    TScript::SetTriggerWorld(nullptr);

    JsonOut j;
    j.Begin('{');
    j.FieldString("schema", "gameflow.triggertest.v1");
    j.FieldString("side", "port");
    j.FieldString("case", c.name);
    j.Field("fires", result.fires ? 1 : 0);
    j.Key("user");
    std::string user;
    for (size_t i = 0; i < objects.size(); ++i)
        if (objects[i].get() == result.user)
            user = ids[i];
    if (result.user == guardobject.get() && result.user)
        user = "guard";
    if (user.empty())
        j.Null();
    else
        j.String(user);
    j.Key("alias");
    if (result.alias.empty())
        j.Null();
    else
        j.String(result.alias);
    j.Field("guard_after", result.guard);
    j.End('}');
    return j.str();
}

// ---- Driver ----------------------------------------------------------------

bool Arg(int argc, char* argv[], const char* name, std::string& value)
{
    const size_t n = strlen(name);
    for (int i = 1; i < argc; ++i)
    {
        const char* a = argv[i];
        while (*a == '-')
            ++a;
        if (strncmp(a, name, n) == 0 && a[n] == '=')
        {
            value = a + n + 1;
            return true;
        }
    }
    return false;
}

}  // namespace

namespace
{
std::map<std::string, Target>& Registry()
{
    static std::map<std::string, Target> targets;   // built on first use (static init order)
    return targets;
}
}  // namespace

bool RegisterTarget(const char* name, Target fn)
{
    Registry()[name] = fn;
    return true;
}

Target FindTarget(const std::string& name)
{
    auto it = Registry().find(name);
    return it == Registry().end() ? nullptr : it->second;
}

bool Run(int argc, char* argv[], int& exitcode)
{
    std::string target;
    if (!Arg(argc, argv, "retail-ab", target))
        return false;

    std::string casefile, outfile;
    std::vector<Case> cases;
    if (!Arg(argc, argv, "ab-cases", casefile) || !Arg(argc, argv, "ab-out", outfile) ||
        !ReadCases(casefile, cases))
    {
        fprintf(stderr, "retail-ab: need --ab-cases=<file> (readable) and --ab-out=<file>\n");
        exitcode = 2;
        return true;
    }
    std::string (*dump)(const Case&, std::string&) = nullptr;
    if (target == "script-parse")
        dump = ScriptParse;
    else if (target == "say-duration")
        dump = SayDuration;
    else if (target == "dialog-layout")
        dump = DialogLayout;
    else if (target == "script-step")
        dump = ScriptStep;
    else if (target == "trigger-test")
        dump = TriggerTest;
    else
        dump = FindTarget(target);
    if (!dump)
    {
        fprintf(stderr, "retail-ab: unknown target '%s'\n", target.c_str());
        exitcode = 2;
        return true;
    }

    log_add_callback(CaptureLog, nullptr, LOG_DEBUG);
    std::ofstream out(outfile, std::ios::binary);
    exitcode = 0;
    for (const Case& c : cases)
    {
        std::string error;
        const std::string result = dump(c, error);
        JsonOut j;
        j.Begin('{');
        j.FieldString("id", c.name);
        j.FieldBool("ok", error.empty());
        if (!error.empty())
        {
            j.FieldString("error", error);
            exitcode = 1;
        }
        j.End('}');
        // Splice the result object in as "result".
        std::string line = j.str();
        if (error.empty())
            line.insert(line.size() - 1, ",\"result\":" + result);
        out << line << '\n';
    }
    return true;
}

}  // namespace RetailAB
