// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *            jsonout.h - a minimal streaming JSON writer                *
// *************************************************************************
//
// For the A/B dumps (retailab.cpp, the --test=ab-* hosts): flat records of
// numbers, booleans, strings and lists. Strings arrive as UTF-8.

#pragma once

#include <cstdio>
#include <sstream>
#include <string>

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
