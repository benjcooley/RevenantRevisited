// *************************************************************************
// *                         Cinematix Revenant                            *
// *                 particlefx.cpp - particle expression VM                *
// *************************************************************************

#include "particlefx.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <initializer_list>

int32_t ParticleDefaultLanes(EParticleVar var)
{
    switch (var)
    {
        case EParticleVar::EmitPos:
        case EParticleVar::EmitRot:
        case EParticleVar::EmitScl:
        case EParticleVar::EmitVel:
        case EParticleVar::LifePos:
        case EParticleVar::LifeRot:
        case EParticleVar::LifeScl:
        case EParticleVar::LifeVel:
        case EParticleVar::DrawPos:
        case EParticleVar::DrawRot:
        case EParticleVar::DrawScl:
            return 3;
        case EParticleVar::EmitColor:
        case EParticleVar::LifeColor:
        case EParticleVar::DrawColor:
        case EParticleVar::DrawUvRect:
            return 4;
        default:
            return 1;
    }
}

bool ParticleLayoutAddVar(SParticleBufferLayout& layout, EParticleVar var, int32_t lanes)
{
    const int32_t index = int32_t(var);
    if (index < 0 || index >= int32_t(EParticleVar::Count))
        return false;
    if (layout.vars[index].offset >= 0)
        return true;

    const int32_t resolved_lanes = lanes > 0 ? lanes : ParticleDefaultLanes(var);
    layout.vars[index].offset = layout.stride_floats;
    layout.vars[index].lanes = resolved_lanes;
    layout.stride_floats += resolved_lanes;
    return true;
}

TParticleBucket::TParticleBucket(SParticleBucketDesc desc, SParticleBufferLayout layout)
    : desc(std::move(desc)), layout(layout)
{
}

int32_t TParticleBucket::AddParticle(float owner_id, float life)
{
    if (layout.stride_floats <= 0)
        return -1;
    const int32_t index = count++;
    data.resize(size_t(count) * size_t(layout.stride_floats), 0.0f);
    if (float* owner = VarPtr(index, EParticleVar::OwnerId))
        *owner = owner_id;
    if (float* life_ptr = VarPtr(index, EParticleVar::Life))
        *life_ptr = life;
    if (float* age = VarPtr(index, EParticleVar::Age))
        *age = 0.0f;
    if (float* age01 = VarPtr(index, EParticleVar::Age01))
        *age01 = 0.0f;
    return index;
}

void TParticleBucket::KillParticlesByOwner(float owner_id)
{
    const SParticleVarSlot& owner_slot = layout.vars[int(EParticleVar::OwnerId)];
    if (owner_slot.offset < 0)
        return;

    for (int32_t i = 0; i < count; )
    {
        const float* row = data.data() + size_t(i) * size_t(layout.stride_floats);
        const bool owner_match = row[owner_slot.offset] == owner_id;
        if (owner_match)
            RemoveAt(i);
        else
            ++i;
    }
}

float* TParticleBucket::VarPtr(int32_t particle_index, EParticleVar var)
{
    if (particle_index < 0 || particle_index >= count)
        return nullptr;
    const SParticleVarSlot& slot = layout.vars[int(var)];
    if (slot.offset < 0)
        return nullptr;
    return data.data() + size_t(particle_index) * size_t(layout.stride_floats) + size_t(slot.offset);
}

const float* TParticleBucket::VarPtr(int32_t particle_index, EParticleVar var) const
{
    return const_cast<TParticleBucket*>(this)->VarPtr(particle_index, var);
}

void TParticleBucket::RemoveAt(int32_t particle_index)
{
    if (particle_index < 0 || particle_index >= count)
        return;
    const int32_t last = count - 1;
    if (particle_index != last)
    {
        float* dst = data.data() + size_t(particle_index) * size_t(layout.stride_floats);
        const float* src = data.data() + size_t(last) * size_t(layout.stride_floats);
        std::memcpy(dst, src, size_t(layout.stride_floats) * sizeof(float));
    }
    --count;
    data.resize(size_t(count) * size_t(layout.stride_floats));
}

TParticleBucket* TParticleManager::GetOrCreateGlobalBucket(const SParticleBucketDesc& desc,
                                                           const SParticleBufferLayout& layout)
{
    if (TParticleBucket* existing = FindGlobalBucket(desc.name.c_str()))
        return existing;
    SParticleBucketDesc global_desc = desc;
    global_desc.scope = EParticleBucketScope::Global;
    global_buckets.emplace_back(global_desc, layout);
    return &global_buckets.back();
}

TParticleBucket* TParticleManager::FindGlobalBucket(const char* name)
{
    if (!name)
        return nullptr;
    for (TParticleBucket& bucket : global_buckets)
        if (bucket.Desc().name == name)
            return &bucket;
    return nullptr;
}

TParticleBucket* TParticleManager::GlobalBucket(int32_t index)
{
    if ((uint32_t)index >= (uint32_t)global_buckets.size())
        return nullptr;
    return &global_buckets[size_t(index)];
}

const TParticleBucket* TParticleManager::GlobalBucket(int32_t index) const
{
    return const_cast<TParticleManager*>(this)->GlobalBucket(index);
}

void TParticleManager::BeginDrawPulsePass()
{
    ++draw_pulse_pass;
    if (draw_pulse_pass == 0)
        draw_pulse_pass = 1;
}

void TParticleManager::Clear()
{
    global_buckets.clear();
    draw_pulse_pass = 0;
}

TParticleManager& ParticleManager()
{
    static TParticleManager manager;
    return manager;
}

namespace {

constexpr SParticleOpDef kParticleOpDefs[] = {
    {"End",      0, {}},
    {"LoadConst",1, { { "constant", EParticleArgKind::ConstIndex } }},
    {"LoadVar",  1, { { "var", EParticleArgKind::VarId } }},
    {"StoreVar", 2, { { "var", EParticleArgKind::VarId }, { "src", EParticleArgKind::FloatOffset } }},
    {"Add",      2, { { "a", EParticleArgKind::FloatOffset }, { "b", EParticleArgKind::FloatOffset } }},
    {"Sub",      2, { { "a", EParticleArgKind::FloatOffset }, { "b", EParticleArgKind::FloatOffset } }},
    {"Mul",      2, { { "a", EParticleArgKind::FloatOffset }, { "b", EParticleArgKind::FloatOffset } }},
    {"Div",      2, { { "a", EParticleArgKind::FloatOffset }, { "b", EParticleArgKind::FloatOffset } }},
    {"Mod",      2, { { "a", EParticleArgKind::FloatOffset }, { "b", EParticleArgKind::FloatOffset } }},
    {"Neg",      1, { { "src", EParticleArgKind::FloatOffset } }},
    {"Floor",    1, { { "src", EParticleArgKind::FloatOffset } }},
    {"Sin",      1, { { "src", EParticleArgKind::FloatOffset } }},
    {"Cos",      1, { { "src", EParticleArgKind::FloatOffset } }},
    {"Min",      2, { { "a", EParticleArgKind::FloatOffset }, { "b", EParticleArgKind::FloatOffset } }},
    {"Max",      2, { { "a", EParticleArgKind::FloatOffset }, { "b", EParticleArgKind::FloatOffset } }},
    {"Clamp",    3, { { "x", EParticleArgKind::FloatOffset }, { "lo", EParticleArgKind::FloatOffset }, { "hi", EParticleArgKind::FloatOffset } }},
    {"Lerp",     3, { { "a", EParticleArgKind::FloatOffset }, { "b", EParticleArgKind::FloatOffset }, { "t", EParticleArgKind::FloatOffset } }},
    {"Frame",    2, { { "t", EParticleArgKind::FloatOffset }, { "rate", EParticleArgKind::FloatOffset } }},
    {"Flipbook", 3, { { "frame", EParticleArgKind::FloatOffset }, { "cols", EParticleArgKind::FloatOffset }, { "rows", EParticleArgKind::FloatOffset } }},
    {"Vec2",     2, { { "x", EParticleArgKind::FloatOffset }, { "y", EParticleArgKind::FloatOffset } }},
    {"Vec3",     3, { { "x", EParticleArgKind::FloatOffset }, { "y", EParticleArgKind::FloatOffset }, { "z", EParticleArgKind::FloatOffset } }},
    {"Vec4",     4, { { "x", EParticleArgKind::FloatOffset }, { "y", EParticleArgKind::FloatOffset }, { "z", EParticleArgKind::FloatOffset }, { "w", EParticleArgKind::FloatOffset } }},
};

constexpr uint16_t kPfxArgStack = 0x8000;
constexpr uint16_t kPfxArgLocal = 0x4000;
constexpr uint16_t kPfxArgConst = 0xC000;
constexpr uint16_t kPfxArgMask = 0x3FFF;

struct SPfxValueRef
{
    uint16_t arg = 0;
    uint8_t lanes = 1;
};

uint16_t PfxStackArg(uint16_t slot) { return uint16_t(kPfxArgStack | (slot & kPfxArgMask)); }
uint16_t PfxLocalArg(uint16_t slot) { return uint16_t(kPfxArgLocal | (slot & kPfxArgMask)); }
uint16_t PfxConstArg(uint16_t index) { return uint16_t(kPfxArgConst | (index & kPfxArgMask)); }
uint16_t PfxVarArg(EParticleVar var) { return uint16_t(var); }
bool PfxIsStackArg(uint16_t arg) { return (arg & kPfxArgConst) == kPfxArgStack; }
bool PfxIsLocalArg(uint16_t arg) { return (arg & kPfxArgConst) == kPfxArgLocal; }
bool PfxIsConstArg(uint16_t arg) { return (arg & kPfxArgConst) == kPfxArgConst; }
uint16_t PfxArgIndex(uint16_t arg) { return uint16_t(arg & kPfxArgMask); }

class TParticleExpressionParser
{
  public:
    TParticleExpressionParser(const char* text, std::vector<uint16_t>& out_code,
                              std::vector<float>& out_constants,
                              uint16_t& out_max_stack_depth,
                              uint8_t& out_result_lanes,
                              std::string* err)
        : src(text ? text : ""),
          code(out_code),
          constants(out_constants),
          max_stack_depth(out_max_stack_depth),
          result_lanes(out_result_lanes),
          error(err) {}

    bool Parse()
    {
        code.clear();
        constants.clear();
        max_stack_depth = 0;
        result_lanes = 1;
        stack_top = 0;
        SPfxValueRef result;
        if (!ParseExpr(&result))
            return false;
        result_lanes = result.lanes;
        SkipWs();
        if (*src)
            return Fail("unexpected trailing input");
        return true;
    }

  private:
    const char* src = nullptr;
    std::vector<uint16_t>& code;
    std::vector<float>& constants;
    uint16_t& max_stack_depth;
    uint8_t& result_lanes;
    std::string* error = nullptr;
    uint16_t stack_top = 0;

    SPfxValueRef NewStackResult(uint8_t lanes)
    {
        const uint16_t slot = stack_top;
        stack_top = uint16_t(stack_top + lanes);
        if (stack_top > max_stack_depth)
            max_stack_depth = stack_top;
        return { PfxStackArg(slot), lanes };
    }

    void EmitOp(EParticleOp op) { code.push_back(uint16_t(op)); }
    void EmitArg(uint16_t arg) { code.push_back(arg); }

    uint16_t AddConst(float v)
    {
        for (uint16_t i = 0; i < constants.size(); ++i)
            if (constants[i] == v)
                return i;
        constants.push_back(v);
        return uint16_t(constants.size() - 1);
    }

    void SkipWs()
    {
        while (*src && std::isspace((unsigned char)*src))
            ++src;
    }

    bool Fail(const char* msg)
    {
        if (error)
            *error = msg;
        return false;
    }

    SPfxValueRef EmitValueOp(EParticleOp op, std::initializer_list<SPfxValueRef> values, uint8_t result_lanes_override = 0)
    {
        uint8_t lanes = result_lanes_override;
        if (lanes == 0)
        {
            lanes = 1;
            for (const SPfxValueRef& v : values)
                if (v.lanes > lanes) lanes = v.lanes;
        }
        EmitOp(op);
        for (const SPfxValueRef& v : values)
            EmitArg(v.arg);
        return NewStackResult(lanes);
    }

    SPfxValueRef EmitConst(float v) { return { PfxConstArg(AddConst(v)), 1 }; }
    SPfxValueRef EmitVar(EParticleVar v) { return { PfxVarArg(v), 1 }; }

    bool Match(char ch)
    {
        SkipWs();
        if (*src != ch)
            return false;
        ++src;
        return true;
    }

    bool MatchWord(const char* word)
    {
        SkipWs();
        const char* pcur = src;
        for (const char* w = word; *w; ++w, ++pcur)
            if (*pcur != *w)
                return false;
        if (std::isalnum((unsigned char)*pcur) || *pcur == '_')
            return false;
        src = pcur;
        return true;
    }

    bool ParseCommaExpr(SPfxValueRef& out, const char* where)
    {
        if (!Match(',')) return Fail(where);
        return ParseExpr(&out);
    }

    bool ParseExpr(SPfxValueRef* out)
    {
        SPfxValueRef lhs;
        if (!ParseTerm(&lhs)) return false;
        for (;;)
        {
            if (Match('+')) { SPfxValueRef rhs; if (!ParseTerm(&rhs)) return false; lhs = EmitValueOp(EParticleOp::Add, { lhs, rhs }); }
            else if (Match('-')) { SPfxValueRef rhs; if (!ParseTerm(&rhs)) return false; lhs = EmitValueOp(EParticleOp::Sub, { lhs, rhs }); }
            else { if (out) *out = lhs; return true; }
        }
    }

    bool ParseTerm(SPfxValueRef* out)
    {
        SPfxValueRef lhs;
        if (!ParseUnary(&lhs)) return false;
        for (;;)
        {
            if (Match('*')) { SPfxValueRef rhs; if (!ParseUnary(&rhs)) return false; lhs = EmitValueOp(EParticleOp::Mul, { lhs, rhs }); }
            else if (Match('/')) { SPfxValueRef rhs; if (!ParseUnary(&rhs)) return false; lhs = EmitValueOp(EParticleOp::Div, { lhs, rhs }); }
            else if (Match('%')) { SPfxValueRef rhs; if (!ParseUnary(&rhs)) return false; lhs = EmitValueOp(EParticleOp::Mod, { lhs, rhs }); }
            else { if (out) *out = lhs; return true; }
        }
    }

    bool ParseUnary(SPfxValueRef* out)
    {
        if (Match('-'))
        {
            SPfxValueRef v;
            if (!ParseUnary(&v)) return false;
            if (out) *out = EmitValueOp(EParticleOp::Neg, { v }, v.lanes);
            return true;
        }
        return ParsePrimary(out);
    }

    bool ParseCall(const char* name, SPfxValueRef* out)
    {
        if (!Match('(')) return Fail("expected '(' after function name");
        if (!std::strcmp(name, "floor") || !std::strcmp(name, "sin") || !std::strcmp(name, "cos"))
        {
            SPfxValueRef a;
            if (!ParseExpr(&a)) return false;
            if (!Match(')')) return Fail("expected ')' after function argument");
            EParticleOp op = !std::strcmp(name, "floor") ? EParticleOp::Floor : (!std::strcmp(name, "sin") ? EParticleOp::Sin : EParticleOp::Cos);
            if (out) *out = EmitValueOp(op, { a }, a.lanes);
            return true;
        }
        if (!std::strcmp(name, "min") || !std::strcmp(name, "max"))
        {
            SPfxValueRef a, b;
            if (!ParseExpr(&a)) return false;
            if (!ParseCommaExpr(b, "expected ',' after first argument")) return false;
            if (!Match(')')) return Fail("expected ')' after function arguments");
            if (out) *out = EmitValueOp(!std::strcmp(name, "min") ? EParticleOp::Min : EParticleOp::Max, { a, b });
            return true;
        }
        if (!std::strcmp(name, "clamp") || !std::strcmp(name, "lerp"))
        {
            SPfxValueRef a, b, c;
            if (!ParseExpr(&a)) return false;
            if (!ParseCommaExpr(b, "expected ',' after first argument")) return false;
            if (!ParseCommaExpr(c, "expected ',' after second argument")) return false;
            if (!Match(')')) return Fail("expected ')' after function arguments");
            if (out) *out = EmitValueOp(!std::strcmp(name, "clamp") ? EParticleOp::Clamp : EParticleOp::Lerp, { a, b, c });
            return true;
        }
        if (!std::strcmp(name, "frame"))
        {
            SPfxValueRef t, rate;
            if (!ParseExpr(&t)) return false;
            if (!ParseCommaExpr(rate, "expected ',' after frame time")) return false;
            if (!Match(')')) return Fail("expected ')' after frame arguments");
            if (out) *out = EmitValueOp(EParticleOp::Frame, { t, rate });
            return true;
        }
        if (!std::strcmp(name, "flipbook"))
        {
            SPfxValueRef frame, cols, rows;
            if (!ParseExpr(&frame)) return false;
            if (!ParseCommaExpr(cols, "expected ',' after flipbook frame")) return false;
            if (!ParseCommaExpr(rows, "expected ',' after flipbook columns")) return false;
            if (!Match(')')) return Fail("expected ')' after flipbook arguments");
            if (out) *out = EmitValueOp(EParticleOp::Flipbook, { frame, cols, rows }, 4);
            return true;
        }
        if (!std::strcmp(name, "vec2") || !std::strcmp(name, "vec3") || !std::strcmp(name, "vec4"))
        {
            const int lanes = name[3] - '0';
            SPfxValueRef args[4];
            for (int i = 0; i < lanes; ++i)
            {
                if (i > 0 && !Match(',')) return Fail("expected ',' in vector constructor");
                if (!ParseExpr(&args[i])) return false;
            }
            if (!Match(')')) return Fail("expected ')' after vector constructor");
            EParticleOp op = lanes == 2 ? EParticleOp::Vec2 : (lanes == 3 ? EParticleOp::Vec3 : EParticleOp::Vec4);
            if (out) *out = lanes == 2 ? EmitValueOp(op, { args[0], args[1] }, 2)
                                      : (lanes == 3 ? EmitValueOp(op, { args[0], args[1], args[2] }, 3)
                                                    : EmitValueOp(op, { args[0], args[1], args[2], args[3] }, 4));
            return true;
        }
        return Fail("unknown function");
    }

    bool ParsePrimary(SPfxValueRef* out)
    {
        SkipWs();
        if (Match('('))
        {
            SPfxValueRef expr;
            if (!ParseExpr(&expr)) return false;
            if (!Match(')')) return Fail("expected ')'");
            if (out) *out = expr;
            return true;
        }

        const char* ident_start = src;
        if (std::isalpha((unsigned char)*src) || *src == '_')
        {
            while (std::isalnum((unsigned char)*src) || *src == '_') ++src;
            std::string ident(ident_start, src);
            SkipWs();
            if (*src == '(') return ParseCall(ident.c_str(), out);
            if (ident == "time_frame") { if (out) *out = EmitVar(EParticleVar::TimeFrame); return true; }
            if (ident == "age01") { if (out) *out = EmitVar(EParticleVar::Age01); return true; }
            if (ident == "age") { if (out) *out = EmitVar(EParticleVar::Age); return true; }
            if (ident == "seed") { if (out) *out = EmitVar(EParticleVar::Seed); return true; }
            return Fail("unknown identifier");
        }

        char* end = nullptr;
        const float v = std::strtof(src, &end);
        if (end != src)
        {
            src = end;
            if (out) *out = EmitConst(v);
            return true;
        }
        return Fail("expected expression");
    }
};

} // namespace

bool TParticleExpression::Compile(const char* expr, std::string* error)
{
    source = expr ? expr : "";
    TParticleExpressionParser parser(source.c_str(), code, constants, max_stack_depth, result_lanes, error);
    valid = parser.Parse();
    if (valid)
        code.push_back(uint16_t(EParticleOp::End));
    return valid;
}

void TParticleExpression::Eval(const SParticleEvalContext& ctx, float* out_values, int32_t out_lanes) const
{
    if (!out_values || out_lanes <= 0)
        return;

    float stack[256] = {};
    uint8_t stack_lanes[256] = {};
    uint16_t sp = 0;
    uint16_t last = PfxVarArg(EParticleVar::TimeFrame);
    uint8_t last_lanes = 1;

    auto arg_lanes = [&](uint16_t arg) -> uint8_t {
        if (PfxIsStackArg(arg))
            return stack_lanes[PfxArgIndex(arg)] > 0 ? stack_lanes[PfxArgIndex(arg)] : 1;
        return 1;
    };

    auto read_arg = [&](uint16_t arg, uint8_t lane = 0) -> float {
        if (PfxIsStackArg(arg))
        {
            const uint16_t idx = PfxArgIndex(arg);
            const uint8_t lanes = stack_lanes[idx] > 0 ? stack_lanes[idx] : 1;
            const uint8_t src_lane = lane < lanes ? lane : uint8_t(lanes - 1);
            return stack[idx + src_lane];
        }
        if (PfxIsLocalArg(arg))
            return stack[PfxArgIndex(arg) + lane];
        if (PfxIsConstArg(arg))
            return PfxArgIndex(arg) < constants.size() ? constants[PfxArgIndex(arg)] : 0.0f;
        switch (EParticleVar(arg))
        {
            case EParticleVar::TimeFrame: return ctx.time_frame;
            case EParticleVar::Age: return ctx.age;
            case EParticleVar::Age01: return ctx.age01;
            case EParticleVar::Seed: return ctx.seed;
            default: return 0.0f;
        }
    };

    auto push_values = [&](const float* values, uint8_t lanes) -> uint16_t {
        const uint16_t slot = sp;
        for (uint8_t lane = 0; lane < lanes; ++lane)
            if (slot + lane < uint16_t(sizeof(stack) / sizeof(stack[0])))
                stack[slot + lane] = values[lane];
        if (slot < uint16_t(sizeof(stack_lanes) / sizeof(stack_lanes[0])))
            stack_lanes[slot] = lanes;
        sp = uint16_t(sp + lanes);
        last = PfxStackArg(slot);
        last_lanes = lanes;
        return last;
    };

    auto push_scalar = [&](float v) -> uint16_t {
        float values[1] = { v };
        return push_values(values, 1);
    };

    for (size_t pc = 0; pc < code.size(); )
    {
        const EParticleOp op = EParticleOp(code[pc++]);
        switch (op)
        {
            case EParticleOp::End:
                for (int32_t lane = 0; lane < out_lanes; ++lane)
                    out_values[lane] = read_arg(last, uint8_t(lane));
                return;
            case EParticleOp::LoadConst:
                push_scalar(read_arg(PfxConstArg(code[pc++])));
                break;
            case EParticleOp::LoadVar:
                push_scalar(read_arg(PfxVarArg(EParticleVar(code[pc++]))));
                break;
            case EParticleOp::StoreVar:
                pc += 2;
                break;
            case EParticleOp::Add:
            case EParticleOp::Sub:
            case EParticleOp::Mul:
            case EParticleOp::Div:
            case EParticleOp::Mod:
            case EParticleOp::Min:
            case EParticleOp::Max:
            {
                const uint16_t a_arg = code[pc++];
                const uint16_t b_arg = code[pc++];
                const uint8_t a_lanes = arg_lanes(a_arg);
                const uint8_t b_lanes = arg_lanes(b_arg);
                const uint8_t lanes = a_lanes > b_lanes ? a_lanes : b_lanes;
                float values[4] = {};
                for (uint8_t lane = 0; lane < lanes && lane < 4; ++lane)
                {
                    const float a = read_arg(a_arg, lane);
                    const float b = read_arg(b_arg, lane);
                    switch (op)
                    {
                        case EParticleOp::Add: values[lane] = a + b; break;
                        case EParticleOp::Sub: values[lane] = a - b; break;
                        case EParticleOp::Mul: values[lane] = a * b; break;
                        case EParticleOp::Div: values[lane] = b != 0.0f ? a / b : 0.0f; break;
                        case EParticleOp::Mod: values[lane] = b != 0.0f ? std::fmod(a, b) : 0.0f; break;
                        case EParticleOp::Min: values[lane] = a < b ? a : b; break;
                        case EParticleOp::Max: values[lane] = a > b ? a : b; break;
                        default: break;
                    }
                }
                push_values(values, lanes);
                break;
            }
            case EParticleOp::Neg:
            case EParticleOp::Floor:
            case EParticleOp::Sin:
            case EParticleOp::Cos:
            {
                const uint16_t a_arg = code[pc++];
                const uint8_t lanes = arg_lanes(a_arg);
                float values[4] = {};
                for (uint8_t lane = 0; lane < lanes && lane < 4; ++lane)
                {
                    const float a = read_arg(a_arg, lane);
                    switch (op)
                    {
                        case EParticleOp::Neg: values[lane] = -a; break;
                        case EParticleOp::Floor: values[lane] = std::floor(a); break;
                        case EParticleOp::Sin: values[lane] = std::sin(a); break;
                        case EParticleOp::Cos: values[lane] = std::cos(a); break;
                        default: break;
                    }
                }
                push_values(values, lanes);
                break;
            }
            case EParticleOp::Clamp:
            {
                const uint16_t x_arg = code[pc++], lo_arg = code[pc++], hi_arg = code[pc++];
                const uint8_t x_lanes = arg_lanes(x_arg);
                const uint8_t lo_lanes = arg_lanes(lo_arg);
                const uint8_t hi_lanes = arg_lanes(hi_arg);
                const uint8_t lanes = x_lanes > lo_lanes
                    ? (x_lanes > hi_lanes ? x_lanes : hi_lanes)
                    : (lo_lanes > hi_lanes ? lo_lanes : hi_lanes);
                float values[4] = {};
                for (uint8_t lane = 0; lane < lanes && lane < 4; ++lane)
                {
                    const float x = read_arg(x_arg, lane);
                    const float lo = read_arg(lo_arg, lane);
                    const float hi = read_arg(hi_arg, lane);
                    const float clamped_lo = x > lo ? x : lo;
                    values[lane] = clamped_lo < hi ? clamped_lo : hi;
                }
                push_values(values, lanes);
                break;
            }
            case EParticleOp::Lerp:
            {
                const uint16_t a_arg = code[pc++], b_arg = code[pc++], t_arg = code[pc++];
                const uint8_t a_lanes = arg_lanes(a_arg);
                const uint8_t b_lanes = arg_lanes(b_arg);
                const uint8_t lanes = a_lanes > b_lanes ? a_lanes : b_lanes;
                float values[4] = {};
                for (uint8_t lane = 0; lane < lanes && lane < 4; ++lane)
                {
                    const float a = read_arg(a_arg, lane);
                    const float b = read_arg(b_arg, lane);
                    const float t = read_arg(t_arg, lane);
                    values[lane] = a + (b - a) * t;
                }
                push_values(values, lanes);
                break;
            }
            case EParticleOp::Frame:
            {
                const float t = read_arg(code[pc++]);
                const float rate = read_arg(code[pc++]);
                push_scalar(std::floor(t * rate));
                break;
            }
            case EParticleOp::Flipbook:
            {
                const float f = read_arg(code[pc++]);
                const float cols_f = read_arg(code[pc++]);
                const float rows_f = read_arg(code[pc++]);
                const int32_t cols = int32_t(cols_f) > 1 ? int32_t(cols_f) : 1;
                const int32_t rows = int32_t(rows_f) > 1 ? int32_t(rows_f) : 1;
                const int32_t total = (cols * rows) > 1 ? (cols * rows) : 1;
                const int32_t frame = ((int32_t(std::floor(f)) % total) + total) % total;
                const float values[4] = {
                    float(frame % cols) / float(cols),
                    float(frame / cols) / float(rows),
                    1.0f / float(cols),
                    1.0f / float(rows),
                };
                push_values(values, 4);
                break;
            }
            case EParticleOp::Vec2:
            case EParticleOp::Vec3:
            case EParticleOp::Vec4:
            {
                const uint8_t lanes = op == EParticleOp::Vec2 ? 2 : (op == EParticleOp::Vec3 ? 3 : 4);
                float values[4] = {};
                for (uint8_t lane = 0; lane < lanes; ++lane)
                    values[lane] = read_arg(code[pc++]);
                push_values(values, lanes);
                break;
            }
        }
    }

    for (int32_t lane = 0; lane < out_lanes; ++lane)
        out_values[lane] = read_arg(last, uint8_t(lane));
}

float TParticleExpression::Eval(const SParticleEvalContext& ctx) const
{
    float value = 0.0f;
    Eval(ctx, &value, 1);
    return value;
}

