// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   jsonvalue.h - a small strict JSON reader                             *
// *************************************************************************
//
// For the playtest control channel and recordings (docs/playtest/
// PROTOCOL.md): one JSON object per line. Strings decode to UTF-8 (\u
// escapes, surrogate pairs included); numbers are integers, or doubles when
// they have a fraction or an exponent. A lookup that isn't there reads as
// null, so optional fields read as `v["key"].Int(default)`. Its writing
// counterpart is jsonout.h. (The retail A/B harness has its own reader,
// retailab_json.h, whose \u escapes carry Windows-1252 bytes.)
#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

class JsonValue
{
  public:
    enum class EKind : uint8_t { Null, Bool, Int, Double, String, Array, Object };

    // Parses one JSON text; false (with `error` set) when it isn't one.
    [[nodiscard]] static bool Parse(const std::string& text, JsonValue& out, std::string& error);

    [[nodiscard]] EKind Kind() const { return kind; }
    [[nodiscard]] bool IsNull() const   { return kind == EKind::Null; }
    [[nodiscard]] bool IsInt() const    { return kind == EKind::Int; }
    [[nodiscard]] bool IsString() const { return kind == EKind::String; }
    [[nodiscard]] bool IsArray() const  { return kind == EKind::Array; }
    [[nodiscard]] bool IsObject() const { return kind == EKind::Object; }
    [[nodiscard]] bool Has(const std::string& key) const { return kind == EKind::Object && object.count(key) > 0; }

    [[nodiscard]] int64_t Int(int64_t otherwise = 0) const;
    [[nodiscard]] double Double(double otherwise = 0.0) const;
    [[nodiscard]] bool Bool(bool otherwise = false) const;
    [[nodiscard]] const std::string& Str() const { return text; }
    [[nodiscard]] const std::vector<JsonValue>& Items() const { return array; }
    [[nodiscard]] const std::map<std::string, JsonValue>& Members() const { return object; }

    // The value as compact JSON (object members in key order).
    [[nodiscard]] std::string Serialize() const;

    [[nodiscard]] const JsonValue& operator[](const std::string& key) const;
    [[nodiscard]] const JsonValue& operator[](size_t index) const;

  private:
    class TParser;

    EKind                            kind = EKind::Null;
    int64_t                          integer = 0;
    double                           real = 0.0;
    std::string                      text;
    std::vector<JsonValue>           array;
    std::map<std::string, JsonValue> object;
};
