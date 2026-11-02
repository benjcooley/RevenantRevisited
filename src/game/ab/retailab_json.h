// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *    retailab_json.h - JSON in and out for the retail A/B targets       *
// *************************************************************************
//
// JsonOut writes the dumps (shared by every target in retailab*.cpp).
// JsonValue reads a case given as one JSON object (the combat targets: a
// case is a world of characters and action blocks, too nested for flat
// tab-separated fields). Both are for the A/B harness only: small, strict,
// UTF-8 strings, integer numbers.

#pragma once

#include <cstdint>
#include <cstdio>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace RetailAB
{

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
    JsonOut& Raw(const std::string& json) { Sep(); out << json; return *this; }   // a value already JSON

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

// A parsed JSON value. Numbers are integers (the cases hold no fractions);
// a lookup that isn't there returns a null value, so optional keys read as
// `v["key"].Int(default)`.
class JsonValue
{
  public:
    enum class Kind { Null, Bool, Int, String, Array, Object };

    static JsonValue Parse(const std::string& text)
    {
        size_t at = 0;
        JsonValue v = ParseValue(text, at);
        SkipSpace(text, at);
        if (at != text.size())
            throw std::runtime_error("json: trailing text");
        return v;
    }

    [[nodiscard]] Kind GetKind() const { return kind; }
    [[nodiscard]] bool IsNull() const { return kind == Kind::Null; }
    [[nodiscard]] bool Has(const std::string& key) const { return kind == Kind::Object && object.count(key); }

    [[nodiscard]] int64_t Int(int64_t otherwise = 0) const
    {
        if (kind == Kind::Int)
            return number;
        if (kind == Kind::Bool)
            return number ? 1 : 0;
        return otherwise;
    }
    [[nodiscard]] bool Bool(bool otherwise = false) const { return kind == Kind::Null ? otherwise : Int() != 0; }
    [[nodiscard]] const std::string& Str() const { return text; }
    [[nodiscard]] const std::vector<JsonValue>& Items() const { return array; }
    [[nodiscard]] const std::map<std::string, JsonValue>& Members() const { return object; }

    const JsonValue& operator[](const std::string& key) const
    {
        static const JsonValue none;
        auto it = object.find(key);
        return it == object.end() ? none : it->second;
    }
    const JsonValue& operator[](size_t i) const
    {
        static const JsonValue none;
        return i < array.size() ? array[i] : none;
    }

  private:
    static void SkipSpace(const std::string& s, size_t& at)
    {
        while (at < s.size() && (s[at] == ' ' || s[at] == '\t' || s[at] == '\n' || s[at] == '\r'))
            ++at;
    }

    static JsonValue ParseValue(const std::string& s, size_t& at)
    {
        SkipSpace(s, at);
        if (at >= s.size())
            throw std::runtime_error("json: unexpected end");
        JsonValue v;
        const char c = s[at];
        if (c == '{')
        {
            v.kind = Kind::Object;
            ++at;
            SkipSpace(s, at);
            if (at < s.size() && s[at] == '}')
            {
                ++at;
                return v;
            }
            for (;;)
            {
                JsonValue key = ParseValue(s, at);
                if (key.kind != Kind::String)
                    throw std::runtime_error("json: object key is not a string");
                SkipSpace(s, at);
                if (at >= s.size() || s[at] != ':')
                    throw std::runtime_error("json: ':' expected");
                ++at;
                v.object[key.text] = ParseValue(s, at);
                SkipSpace(s, at);
                if (at < s.size() && s[at] == ',')
                {
                    ++at;
                    continue;
                }
                if (at < s.size() && s[at] == '}')
                {
                    ++at;
                    return v;
                }
                throw std::runtime_error("json: ',' or '}' expected");
            }
        }
        if (c == '[')
        {
            v.kind = Kind::Array;
            ++at;
            SkipSpace(s, at);
            if (at < s.size() && s[at] == ']')
            {
                ++at;
                return v;
            }
            for (;;)
            {
                v.array.push_back(ParseValue(s, at));
                SkipSpace(s, at);
                if (at < s.size() && s[at] == ',')
                {
                    ++at;
                    continue;
                }
                if (at < s.size() && s[at] == ']')
                {
                    ++at;
                    return v;
                }
                throw std::runtime_error("json: ',' or ']' expected");
            }
        }
        if (c == '"')
        {
            v.kind = Kind::String;
            ++at;
            while (at < s.size() && s[at] != '"')
            {
                if (s[at] == '\\' && at + 1 < s.size())
                {
                    const char e = s[++at];
                    switch (e)
                    {
                        case 'n': v.text += '\n'; break;
                        case 'r': v.text += '\r'; break;
                        case 't': v.text += '\t'; break;
                        case 'u':
                        {
                            // The case files are ASCII; \u escapes carry
                            // Windows-1252 bytes the driver wrote as code points.
                            const unsigned code = (unsigned)std::stoul(s.substr(at + 1, 4), nullptr, 16);
                            v.text += (char)(code & 0xff);
                            at += 4;
                            break;
                        }
                        default: v.text += e;
                    }
                    ++at;
                }
                else
                    v.text += s[at++];
            }
            if (at >= s.size())
                throw std::runtime_error("json: unterminated string");
            ++at;
            return v;
        }
        if (s.compare(at, 4, "true") == 0)
        {
            v.kind = Kind::Bool;
            v.number = 1;
            at += 4;
            return v;
        }
        if (s.compare(at, 5, "false") == 0)
        {
            v.kind = Kind::Bool;
            at += 5;
            return v;
        }
        if (s.compare(at, 4, "null") == 0)
        {
            at += 4;
            return v;
        }
        size_t used = 0;
        v.kind = Kind::Int;
        v.number = std::stoll(s.substr(at, 24), &used);
        if (used == 0)
            throw std::runtime_error("json: bad value");
        at += used;
        return v;
    }

    Kind kind = Kind::Null;
    int64_t number = 0;
    std::string text;
    std::vector<JsonValue> array;
    std::map<std::string, JsonValue> object;
};

}  // namespace RetailAB
