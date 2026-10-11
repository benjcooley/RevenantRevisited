#include "partsysdefinition.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <set>
#include <utility>

namespace authored_partsys {
Value Expression::Evaluate(int parameter) const
{
    if (keys.empty()) return constant;
    std::size_t right = 0;
    const float frame = static_cast<float>(parameter);
    while (right < keys.size() && keys[right].percent <= frame) ++right;
    // 0x4014cf and 0x4015ac both select the last key, even before first key.
    if (right == 0 || right == keys.size()) return keys.back().value;
    const Key& a = keys[right - 1];
    const Key& b = keys[right];
    const int left_time = static_cast<int>(a.percent);
    const int right_time = static_cast<int>(b.percent);
    const auto denominator = std::max<std::int64_t>(1,
        static_cast<std::int64_t>(right_time) - left_time);
    Value result{};
    for (int i = 0; i < dimensions; ++i) {
        // Original x87 keeps intermediates extended until its final fstp.
        const long double left = a.value[i];
        result[i] = static_cast<float>(
            ((static_cast<long double>(b.value[i]) - left) / denominator)
            * static_cast<float>(static_cast<std::int64_t>(parameter) - left_time) + left);
    }
    return result;
}

namespace {
struct Reader {
    std::string_view text;
    std::size_t at = 0;
    std::string error;

    void Space() { while (at < text.size() && std::isspace(static_cast<unsigned char>(text[at]))) ++at; }
    bool Take(char c) { Space(); if (at < text.size() && text[at] == c) { ++at; return true; } return false; }
    bool Fail(std::string reason) { if (error.empty()) error = std::move(reason) + " at byte " + std::to_string(at); return false; }
    bool Name(std::string& value) {
        Space(); const auto start = at;
        while (at < text.size()) {
            const unsigned char c = text[at];
            if (!(std::isalnum(c) || c == '_' || c == '#' || c == '.' || c == '-' || c == '\\')) break;
            ++at;
        }
        if (start == at) return Fail("expected object or field name");
        value.assign(text.substr(start, at - start)); return true;
    }
    bool ValueName(std::string& value) {
        Space();
        if (at >= text.size() || text[at] != '"') return Name(value);
        const auto start = ++at;
        while (at < text.size() && text[at] != '"') {
            if (text[at]=='\\' || std::iscntrl(static_cast<unsigned char>(text[at])))
                return Fail("unsupported quoted-name escape or control character");
            ++at;
        }
        if (at == text.size() || at == start) return Fail("expected nonempty closed quoted name");
        value.assign(text.substr(start,at-start));++at;return true;
    }
    bool Number(float& value, double* exact=nullptr) {
        Space(); const std::string tail(text.substr(at)); char* end = nullptr;
        const double parsed = std::strtod(tail.c_str(), &end);
        if (end == tail.c_str() || !std::isfinite(parsed) ||
            std::abs(parsed) > std::numeric_limits<float>::max()) return Fail("expected finite numeric value");
        at += static_cast<std::size_t>(end - tail.c_str()); value = static_cast<float>(parsed);
        if(exact)*exact=parsed;return true;
    }
    bool Tuple(Value& value, int count, bool wrapped) {
        if (wrapped && !Take('(')) return Fail("expected vector '(' ");
        for (int i = 0; i < count; ++i) {
            if (i && !Take(wrapped ? ',' : ':')) return Fail("wrong expression width");
            if (!Number(value[i])) return false;
        }
        if (wrapped && !Take(')')) return Fail("expected vector ')'");
        return true;
    }
    bool Expr(Expression& expression) {
        if (!Take('[')) return Tuple(expression.constant, expression.dimensions, false);
        // Original 4010f3 clears only the first destination before selecting
        // literal/curve parsing. A curve preserves the remaining defaults;
        // e.g. bounce becomes (0,100), while absent fields remain unchanged.
        expression.constant[0] = 0;
        do {
            Key key;
            if (!Number(key.percent) || !Take(':')) return Fail("expected key time ':'");
            if (key.percent < static_cast<float>(std::numeric_limits<int>::min()) ||
                key.percent >= static_cast<float>(std::numeric_limits<int>::max())) return Fail("key time outside integer evaluator range");
            const bool vector = expression.dimensions > 1;
            if (!Tuple(key.value, expression.dimensions, vector)) return false;
            if (!expression.keys.empty() && key.percent < expression.keys.back().percent)
                return Fail("unsupported descending curve keys");
            expression.keys.push_back(key);
            if (Take(']')) return true;
        } while (Take(','));
        return Fail("expected curve ',' or ']'");
    }
};
std::string Lower(std::string value) {
    for (char& c : value) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return value;
}
Expression* Field(Definition& d, const std::string& name) {
    if (name == "pps") return &d.pps;
    if (name == "initialvelocity") return &d.initialvelocity;
    if (name == "lifespan") return &d.lifespan;
    if (name == "globalrotation") return &d.globalrotation;
    if (name == "localrotation") return &d.localrotation;
    if (name == "rglobalrotation") return &d.rglobalrotation;
    if (name == "rlocalrotation") return &d.rlocalrotation;
    if (name == "friction") return &d.friction;
    if (name == "gravity") return &d.gravity;
    if (name == "color") return &d.color;
    if (name == "alpha") return &d.alpha;
    if (name == "spread") return &d.spread;
    if (name == "azimuth") return &d.azimuth;
    if (name == "relvel") return &d.relvel;
    if (name == "charvel") return &d.charvel;
    if (name == "scale") return &d.scale;
    if (name == "bounce") return &d.bounce;
    return nullptr;
}
} // namespace

bool ParseDefinition(std::string_view tag, Definition& output, std::string& diagnostic)
{
    Reader r{tag}; Definition d; std::set<std::string> fields;
    bool success = true;
    while (success) {
        r.Space(); if (r.at == tag.size()) break;
        std::string name; if (!r.Name(name)) { success = false; break; }
        name = Lower(name);
        if (!fields.insert(name).second) { success = r.Fail("unsupported duplicate field " + name); break; }
        if (!r.Take('=')) { success = r.Fail("expected '=' after " + name); break; }
        if (name == "obj") {
            const bool list = r.Take('(');
            do { std::string object; if (!r.ValueName(object)) { success = false; break; } d.objects.push_back(std::move(object)); }
            while (list && r.Take(','));
            if (success && list && !r.Take(')')) success = r.Fail("expected object-list ')'");
        } else if (name == "particle") {
            success = r.ValueName(d.particle);
        } else if (name == "emittertype" || name == "blendmode") {
            std::string value; success = r.ValueName(value); value = Lower(value);
            if (success && name == "emittertype") {
                const std::array<const char*,4> types{{"sphere","cube","circle","square"}};
                auto found = std::find(types.begin(), types.end(), value);
                if (found == types.end()) success = r.Fail("unsupported emittertype " + value);
                else d.emittertype = static_cast<int>(found - types.begin());
            } else if (success) {
                const std::array<std::pair<const char*,int>,9> modes{{{"none",0},{"normal",1},{"alpha",2},{"litalpha",4},{"add",8},{"litadd",16},{"alphaadd",32},{"nocheckz",64},{"litalphaz",68}}};
                auto found = std::find_if(modes.begin(), modes.end(), [&](const auto& mode) { return value == mode.first; });
                if (found == modes.end()) success = r.Fail("unsupported blendmode " + value);
                else d.blendmode = found->second;
            }
        } else if (name == "emittersize") {
            float value = 0;double exact = 0;const auto start=r.at;
            success = r.Number(value,&exact);
            // ParseItem404432..404443 copies numeric token8's integer view.
            // Decimal0.25 therefore stores0; it is not a fractional radius.
            const auto literal=tag.substr(start,r.at-start);
            if(success && (literal.find_first_of("eEpPxX")!=std::string_view::npos ||
                exact < std::numeric_limits<int>::min() ||
                exact >= static_cast<double>(std::numeric_limits<int>::max())+1))
                success = r.Fail("unsupported emittersize numeric literal");
            if (success) d.emittersize = static_cast<int>(exact);
        } else if (Expression* expression = Field(d, name)) {
            success = r.Expr(*expression);
        } else {
            success = r.Fail("unsupported partsys field " + name);
        }
        if (success) {
            r.Space(); if (r.at == tag.size()) break;
            if (!r.Take(',')) success = r.Fail("expected field ','");
            else { r.Space(); if (r.at == tag.size()) success = r.Fail("trailing field ','"); }
        }
    }
    if (success && d.particle.empty()) success = r.Fail("missing particle prototype");
    if (success && d.objects.empty()) success = r.Fail("missing emitter object list");
    diagnostic = r.error;
    if (success) output = std::move(d);
    return success;
}
} // namespace authored_partsys
