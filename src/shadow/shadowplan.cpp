// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *          shadowplan.cpp - reading a Shadow Smoke plan                  *
// *************************************************************************

#include "shadowplan.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <fstream>
#include <optional>
#include <sstream>

namespace {

struct SToken
{
    std::string text;
    bool        quoted = false;
};

bool IsWordChar(char c)
{
    return std::isalnum((unsigned char)c) || c == '_' || c == '@' || c == ',' || c == '.' || c == '-';
}

bool IsOperatorChar(char c)
{
    return c == '=' || c == '!' || c == '<' || c == '>';
}

// Splits a line into words, quoted text and operators; `#` ends it.
// False with `error` set on an unterminated quote or a stray character.
bool Tokenize(const std::string& line, std::vector<SToken>& tokens, std::string& error)
{
    size_t i = 0;
    while (i < line.size())
    {
        const char c = line[i];
        if (std::isspace((unsigned char)c))
        {
            ++i;
        }
        else if (c == '#')
        {
            break;
        }
        else if (c == '"')
        {
            const size_t end = line.find('"', i + 1);
            if (end == std::string::npos)
            {
                error = "unterminated quote";
                return false;
            }
            tokens.push_back({line.substr(i + 1, end - i - 1), true});
            i = end + 1;
        }
        else if (IsWordChar(c) || IsOperatorChar(c))
        {
            const bool op = IsOperatorChar(c);
            size_t end = i;
            while (end < line.size() && (op ? IsOperatorChar(line[end]) : IsWordChar(line[end])))
                ++end;
            tokens.push_back({line.substr(i, end - i), false});
            i = end;
        }
        else
        {
            error = std::string("unexpected character '") + c + "'";
            return false;
        }
    }
    return true;
}

std::string Lower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::tolower(c); });
    return s;
}

std::optional<int32_t> ParseInt(const std::string& text)
{
    int32_t value = 0;
    const char* end = text.data() + text.size();
    const auto [ptr, ec] = std::from_chars(text.data(), end, value);
    if (ec != std::errc() || ptr != end)
        return std::nullopt;
    return value;
}

std::optional<EShadowCompare> ParseCompare(const std::string& text)
{
    if (text == "=" || text == "==") return EShadowCompare::Eq;
    if (text == "!=")                return EShadowCompare::Ne;
    if (text == "<")                 return EShadowCompare::Lt;
    if (text == "<=")                return EShadowCompare::Le;
    if (text == ">")                 return EShadowCompare::Gt;
    if (text == ">=")                return EShadowCompare::Ge;
    return std::nullopt;
}

const char* CompareText(EShadowCompare compare)
{
    switch (compare)
    {
        case EShadowCompare::Eq: return "=";
        case EShadowCompare::Ne: return "!=";
        case EShadowCompare::Lt: return "<";
        case EShadowCompare::Le: return "<=";
        case EShadowCompare::Gt: return ">";
        case EShadowCompare::Ge: return ">=";
    }
    return "?";
}

// One line's tokens, read front to back by the step parsers.
class TLineReader
{
  public:
    explicit TLineReader(std::vector<SToken> tokens) : tokens(std::move(tokens)) {}

    [[nodiscard]] bool AtEnd() const { return next >= tokens.size(); }
    [[nodiscard]] const SToken* Peek() const { return AtEnd() ? nullptr : &tokens[next]; }
    const SToken* Take() { return AtEnd() ? nullptr : &tokens[next++]; }
    // The next token if it is the (unquoted) keyword `word`.
    bool TakeKeyword(const char* word)
    {
        const SToken* t = Peek();
        if (!t || t->quoted || Lower(t->text) != word)
            return false;
        ++next;
        return true;
    }

  private:
    std::vector<SToken> tokens;
    size_t              next = 0;
};

// `choose n n ...` at the end of a scene or talk line.
bool ParseChoices(TLineReader& in, std::vector<int32_t>& choices, std::string& error)
{
    if (in.AtEnd())
        return true;
    if (!in.TakeKeyword("choose"))
    {
        error = "expected 'choose' or the end of the line";
        return false;
    }
    while (const SToken* t = in.Take())
    {
        const std::optional<int32_t> n = t->quoted ? std::nullopt : ParseInt(t->text);
        if (!n || *n < 1 || *n > 9)
        {
            error = "a choice is a number from 1 to 9, not '" + t->text + "'";
            return false;
        }
        choices.push_back(*n);
    }
    if (choices.empty())
    {
        error = "'choose' needs at least one answer";
        return false;
    }
    return true;
}

bool ParseComparison(TLineReader& in, SShadowCondition& condition, std::string& error)
{
    const SToken* op = in.Take();
    const SToken* value = in.Take();
    const std::optional<EShadowCompare> compare = op && !op->quoted ? ParseCompare(op->text) : std::nullopt;
    const std::optional<int32_t> number = value && !value->quoted ? ParseInt(value->text) : std::nullopt;
    if (!compare || !number || !in.AtEnd())
    {
        error = "expected a comparison: <op> <number>, with <op> one of = != < <= > >=";
        return false;
    }
    condition.compare = *compare;
    condition.value = *number;
    return true;
}

bool ParseExpect(TLineReader& in, SShadowCondition& condition, std::string& error)
{
    if (in.TakeKeyword("level"))
    {
        condition.subject = EShadowSubject::Level;
        return ParseComparison(in, condition, error);
    }
    if (in.TakeKeyword("have"))
    {
        const SToken* item = in.Take();
        if (!item)
        {
            error = "'expect have' needs an item";
            return false;
        }
        condition.subject = EShadowSubject::Have;
        condition.name = item->text;
        if (in.AtEnd())
        {
            condition.compare = EShadowCompare::Ge;
            condition.value = 1;
            return true;
        }
        return ParseComparison(in, condition, error);
    }
    const SToken* state = in.Take();
    if (!state || state->quoted)
    {
        error = "'expect' needs a story variable, 'level' or 'have'";
        return false;
    }
    condition.subject = EShadowSubject::GameState;
    condition.name = state->text;
    return ParseComparison(in, condition, error);
}

// A step line inside a stage. False with `error` set.
bool ParseStep(const std::string& verb, TLineReader& in, SShadowStep& step, std::string& error)
{
    if (verb == "start")
    {
        step.verb = EShadowVerb::Start;
        if (!in.TakeKeyword("newgame") || !in.AtEnd())
        {
            error = "'start' takes 'newgame'";
            return false;
        }
        step.target = "newgame";
        return true;
    }
    if (verb == "scene")
    {
        step.verb = EShadowVerb::Scene;
        return ParseChoices(in, step.choices, error);
    }
    if (verb == "talk")
    {
        step.verb = EShadowVerb::Talk;
        const SToken* npc = in.Take();
        if (!npc)
        {
            error = "'talk' needs someone to talk to";
            return false;
        }
        step.target = npc->text;
        return ParseChoices(in, step.choices, error);
    }
    if (verb == "expect")
    {
        step.verb = EShadowVerb::Expect;
        return ParseExpect(in, step.condition, error);
    }
    if (verb == "save")
    {
        step.verb = EShadowVerb::Save;
        const SToken* name = in.Take();
        if (!name || !in.AtEnd())
        {
            error = "'save' takes one name";
            return false;
        }
        step.name = name->text;
        return true;
    }
    error = "unknown word '" + verb + "'";
    return false;
}

} // namespace

bool SShadowCondition::Holds(int32_t actual) const
{
    switch (compare)
    {
        case EShadowCompare::Eq: return actual == value;
        case EShadowCompare::Ne: return actual != value;
        case EShadowCompare::Lt: return actual < value;
        case EShadowCompare::Le: return actual <= value;
        case EShadowCompare::Gt: return actual > value;
        case EShadowCompare::Ge: return actual >= value;
    }
    return false;
}

std::string SShadowCondition::Describe() const
{
    std::ostringstream out;
    switch (subject)
    {
        case EShadowSubject::GameState: out << name; break;
        case EShadowSubject::Level:     out << "level"; break;
        case EShadowSubject::Have:      out << "have \"" << name << '"'; break;
    }
    out << ' ' << CompareText(compare) << ' ' << value;
    return out.str();
}

std::string SShadowStep::Describe() const
{
    std::ostringstream out;
    switch (verb)
    {
        case EShadowVerb::Start:  out << "start " << target; break;
        case EShadowVerb::Scene:  out << "scene"; break;
        case EShadowVerb::Talk:   out << "talk " << target; break;
        case EShadowVerb::Expect: out << "expect " << condition.Describe(); break;
        case EShadowVerb::Save:   out << "save \"" << name << '"'; break;
    }
    if (!choices.empty())
    {
        out << " choose";
        for (int32_t choice : choices)
            out << ' ' << choice;
    }
    return out.str();
}

bool TShadowPlan::Load(const std::string& planpath, std::string& error)
{
    path = planpath;
    module.clear();
    stages.clear();

    std::ifstream file(planpath);
    if (!file)
    {
        error = planpath + ": can't read the plan";
        return false;
    }

    std::string text;
    int32_t lineno = 0;
    auto fail = [&](const std::string& what) {
        error = planpath + ":" + std::to_string(lineno) + ": " + what;
        return false;
    };

    while (std::getline(file, text))
    {
        ++lineno;
        if (!text.empty() && text.back() == '\r')
            text.pop_back();

        std::vector<SToken> tokens;
        std::string what;
        if (!Tokenize(text, tokens, what))
            return fail(what);
        if (tokens.empty())
            continue;

        TLineReader in(std::move(tokens));
        const SToken* first = in.Take();
        if (first->quoted)
            return fail("a line starts with a word, not quoted text");
        const std::string word = Lower(first->text);

        if (word == "module")
        {
            const SToken* name = in.Take();
            if (!name || !in.AtEnd())
                return fail("'module' takes one name");
            module = name->text;
        }
        else if (word == "stage")
        {
            const SToken* id = in.Take();
            if (!id || id->quoted)
                return fail("'stage' needs an id");
            if (FindStage(id->text) >= 0)
                return fail("a second stage '" + id->text + "'");
            SShadowStage stage;
            stage.id = id->text;
            stage.line = lineno;
            if (const SToken* title = in.Take())
                stage.title = title->text;
            if (!in.AtEnd())
                return fail("'stage' takes an id and a title");
            stages.push_back(std::move(stage));
        }
        else
        {
            if (stages.empty())
                return fail("'" + first->text + "' outside a stage");
            SShadowStep step;
            step.line = lineno;
            if (!ParseStep(word, in, step, what))
                return fail(what);
            stages.back().steps.push_back(std::move(step));
        }
    }

    if (module.empty())
        return fail("no 'module' line");
    if (stages.empty())
        return fail("no stages");
    return true;
}

int32_t TShadowPlan::FindStage(const std::string& id) const
{
    for (size_t i = 0; i < stages.size(); ++i)
        if (Lower(stages[i].id) == Lower(id))
            return (int32_t)i;
    return -1;
}
