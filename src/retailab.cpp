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

#include "audio_backend.h"
#include "character.h"
#include "dialog.h"
#include "logging.h"
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
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace RetailAB
{
namespace
{

// ---- JSON output ---------------------------------------------------------

// Minimal streaming JSON writer: the dumps are flat records of numbers,
// booleans, strings and lists. Strings arrive as UTF-8.
class JsonOut
{
  public:
    std::string str() const { return out.str(); }

    JsonOut& Begin(char bracket) { Sep(); out << bracket; first = true; return *this; }
    JsonOut& End(char bracket) { out << bracket; first = false; return *this; }
    JsonOut& Key(const char* key) { Sep(); Quote(key); out << ':'; first = true; return *this; }
    JsonOut& Value(long long v) { Sep(); out << v; return *this; }
    JsonOut& Bool(bool v) { Sep(); out << (v ? "true" : "false"); return *this; }
    JsonOut& Null() { Sep(); out << "null"; return *this; }
    JsonOut& String(const std::string& utf8) { Sep(); Quote(utf8); return *this; }

    template <typename T> JsonOut& Field(const char* key, T v) { Key(key); return Value((long long)v); }
    JsonOut& FieldBool(const char* key, bool v) { Key(key); return Bool(v); }
    JsonOut& FieldString(const char* key, const std::string& utf8) { Key(key); return String(utf8); }

  private:
    void Sep()
    {
        if (!first)
            out << ',';
        first = false;
    }
    void Quote(const std::string& s)
    {
        out << '"';
        for (unsigned char c : s)
        {
            switch (c)
            {
                case '"': out << "\\\""; break;
                case '\\': out << "\\\\"; break;
                case '\n': out << "\\n"; break;
                case '\r': out << "\\r"; break;
                case '\t': out << "\\t"; break;
                default:
                    if (c < 0x20)
                    {
                        char buf[8];
                        snprintf(buf, sizeof(buf), "\\u%04x", c);
                        out << buf;
                    }
                    else
                        out << c;
            }
        }
        out << '"';
    }

    std::ostringstream out;
    bool first = true;
};

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

// One case per line, tab-separated: the name, then the target's fields.
struct Case
{
    std::string name;
    std::vector<std::string> fields;

    [[nodiscard]] const std::string& Field(size_t i) const
    {
        static const std::string none;
        return i < fields.size() ? fields[i] : none;
    }
};

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
