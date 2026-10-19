// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   jsonvalue.cpp - a small strict JSON reader                           *
// *************************************************************************

#include "engine/core/jsonvalue.h"

#include "engine/core/jsonout.h"

#include <cstdio>
#include <cstdlib>

class JsonValue::TParser
{
  public:
    explicit TParser(const std::string& s) : s(s) {}

    bool Document(JsonValue& out)
    {
        if (!Value(out, 0))
            return false;
        Space();
        return at == s.size() || Fail("trailing text");
    }

    std::string error;

  private:
    static constexpr int kMaxDepth = 64;

    bool Fail(const char* what)
    {
        if (error.empty())
            error = std::string(what) + " at offset " + std::to_string(at);
        return false;
    }

    void Space()
    {
        while (at < s.size() && (s[at] == ' ' || s[at] == '\t' || s[at] == '\n' || s[at] == '\r'))
            ++at;
    }

    bool Literal(const char* word)
    {
        const std::string w = word;
        if (s.compare(at, w.size(), w) != 0)
            return Fail("unknown literal");
        at += w.size();
        return true;
    }

    static void AppendUtf8(std::string& out, uint32_t cp)
    {
        if (cp < 0x80)
            out += char(cp);
        else if (cp < 0x800)
        {
            out += char(0xc0 | (cp >> 6));
            out += char(0x80 | (cp & 0x3f));
        }
        else if (cp < 0x10000)
        {
            out += char(0xe0 | (cp >> 12));
            out += char(0x80 | ((cp >> 6) & 0x3f));
            out += char(0x80 | (cp & 0x3f));
        }
        else
        {
            out += char(0xf0 | (cp >> 18));
            out += char(0x80 | ((cp >> 12) & 0x3f));
            out += char(0x80 | ((cp >> 6) & 0x3f));
            out += char(0x80 | (cp & 0x3f));
        }
    }

    bool Hex4(uint32_t& cp)
    {
        if (at + 4 > s.size())
            return Fail("short \\u escape");
        cp = 0;
        for (int i = 0; i < 4; ++i)
        {
            const char c = s[at++];
            cp <<= 4;
            if (c >= '0' && c <= '9')      cp |= uint32_t(c - '0');
            else if (c >= 'a' && c <= 'f') cp |= uint32_t(c - 'a' + 10);
            else if (c >= 'A' && c <= 'F') cp |= uint32_t(c - 'A' + 10);
            else return Fail("bad \\u escape");
        }
        return true;
    }

    bool String(std::string& out)
    {
        ++at;   // the opening quote
        while (at < s.size() && s[at] != '"')
        {
            const char c = s[at++];
            if (c != '\\')
            {
                if ((unsigned char)c < 0x20)
                    return Fail("control character in a string");
                out += c;
                continue;
            }
            if (at >= s.size())
                break;
            const char e = s[at++];
            switch (e)
            {
              case '"': case '\\': case '/': out += e; break;
              case 'b': out += '\b'; break;
              case 'f': out += '\f'; break;
              case 'n': out += '\n'; break;
              case 'r': out += '\r'; break;
              case 't': out += '\t'; break;
              case 'u':
              {
                  uint32_t cp = 0;
                  if (!Hex4(cp))
                      return false;
                  if (cp >= 0xd800 && cp < 0xdc00 && s.compare(at, 2, "\\u") == 0)
                  {
                      at += 2;
                      uint32_t low = 0;
                      if (!Hex4(low))
                          return false;
                      if (low < 0xdc00 || low >= 0xe000)
                          return Fail("unpaired surrogate");
                      cp = 0x10000 + ((cp - 0xd800) << 10) + (low - 0xdc00);
                  }
                  AppendUtf8(out, cp);
                  break;
              }
              default:
                  return Fail("bad escape");
            }
        }
        if (at >= s.size())
            return Fail("unterminated string");
        ++at;   // the closing quote
        return true;
    }

    bool Number(JsonValue& v)
    {
        const size_t start = at;
        if (at < s.size() && s[at] == '-')
            ++at;
        bool real = false;
        while (at < s.size())
        {
            const char c = s[at];
            if (c >= '0' && c <= '9')
                ++at;
            else if (c == '.' || c == 'e' || c == 'E' || c == '+' || (c == '-' && real))
            {
                real = true;
                ++at;
            }
            else
                break;
        }
        const std::string digits = s.substr(start, at - start);
        if (digits.empty() || digits == "-")
            return Fail("bad number");
        char* end = nullptr;
        if (real)
        {
            v.kind = EKind::Double;
            v.real = std::strtod(digits.c_str(), &end);
        }
        else
        {
            v.kind = EKind::Int;
            v.integer = std::strtoll(digits.c_str(), &end, 10);
            v.real = double(v.integer);
        }
        return (end && *end == '\0') || Fail("bad number");
    }

    bool Value(JsonValue& v, int depth)
    {
        if (depth > kMaxDepth)
            return Fail("too deep");
        Space();
        if (at >= s.size())
            return Fail("unexpected end");
        const char c = s[at];
        if (c == '{')
        {
            v.kind = EKind::Object;
            ++at;
            Space();
            if (at < s.size() && s[at] == '}')
            {
                ++at;
                return true;
            }
            for (;;)
            {
                Space();
                if (at >= s.size() || s[at] != '"')
                    return Fail("object key expected");
                std::string key;
                if (!String(key))
                    return false;
                Space();
                if (at >= s.size() || s[at] != ':')
                    return Fail("':' expected");
                ++at;
                if (!Value(v.object[key], depth + 1))
                    return false;
                Space();
                if (at < s.size() && s[at] == ',')
                {
                    ++at;
                    continue;
                }
                if (at < s.size() && s[at] == '}')
                {
                    ++at;
                    return true;
                }
                return Fail("',' or '}' expected");
            }
        }
        if (c == '[')
        {
            v.kind = EKind::Array;
            ++at;
            Space();
            if (at < s.size() && s[at] == ']')
            {
                ++at;
                return true;
            }
            for (;;)
            {
                v.array.emplace_back();
                if (!Value(v.array.back(), depth + 1))
                    return false;
                Space();
                if (at < s.size() && s[at] == ',')
                {
                    ++at;
                    continue;
                }
                if (at < s.size() && s[at] == ']')
                {
                    ++at;
                    return true;
                }
                return Fail("',' or ']' expected");
            }
        }
        if (c == '"')
        {
            v.kind = EKind::String;
            return String(v.text);
        }
        if (c == 't')
        {
            v.kind = EKind::Bool;
            v.integer = 1;
            return Literal("true");
        }
        if (c == 'f')
        {
            v.kind = EKind::Bool;
            return Literal("false");
        }
        if (c == 'n')
            return Literal("null");
        return Number(v);
    }

    const std::string& s;
    size_t at = 0;
};

bool JsonValue::Parse(const std::string& text, JsonValue& out, std::string& error)
{
    out = JsonValue();
    TParser parser(text);
    if (parser.Document(out))
        return true;
    error = parser.error;
    out = JsonValue();
    return false;
}

int64_t JsonValue::Int(int64_t otherwise) const
{
    switch (kind)
    {
      case EKind::Int:
      case EKind::Bool:   return integer;
      case EKind::Double: return int64_t(real);
      default:            return otherwise;
    }
}

double JsonValue::Double(double otherwise) const
{
    switch (kind)
    {
      case EKind::Int:
      case EKind::Double: return real;
      default:            return otherwise;
    }
}

bool JsonValue::Bool(bool otherwise) const
{
    return kind == EKind::Bool ? integer != 0 : otherwise;
}

const JsonValue& JsonValue::operator[](const std::string& key) const
{
    static const JsonValue none;
    const auto it = object.find(key);
    return it == object.end() ? none : it->second;
}

const JsonValue& JsonValue::operator[](size_t index) const
{
    static const JsonValue none;
    return index < array.size() ? array[index] : none;
}

namespace
{
void Write(JsonOut& out, const JsonValue& v)
{
    switch (v.Kind())
    {
      case JsonValue::EKind::Null:   out.Null(); break;
      case JsonValue::EKind::Bool:   out.Bool(v.Bool()); break;
      case JsonValue::EKind::Int:    out.Value(v.Int()); break;
      case JsonValue::EKind::Double:
      {
          char buf[32];
          std::snprintf(buf, sizeof(buf), "%.17g", v.Double());
          out.Raw(buf);
          break;
      }
      case JsonValue::EKind::String: out.String(v.Str()); break;
      case JsonValue::EKind::Array:
          out.Begin('[');
          for (const JsonValue& item : v.Items())
              Write(out, item);
          out.End(']');
          break;
      case JsonValue::EKind::Object:
          out.Begin('{');
          for (const auto& [key, member] : v.Members())
          {
              out.Key(key.c_str());
              Write(out, member);
          }
          out.End('}');
          break;
    }
}
} // namespace

std::string JsonValue::Serialize() const
{
    JsonOut out;
    Write(out, *this);
    return out.str();
}
