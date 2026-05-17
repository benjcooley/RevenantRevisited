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
    {"StoreVar", 3, { { "var", EParticleArgKind::VarId }, { "lane", EParticleArgKind::ImmediateU16 }, { "src", EParticleArgKind::FloatOffset } }},
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
    {"Rand01",   0, {}},
    {"Rand",     2, { { "lo", EParticleArgKind::FloatOffset }, { "hi", EParticleArgKind::FloatOffset } }},
    {"Step",     2, { { "edge", EParticleArgKind::FloatOffset }, { "x", EParticleArgKind::FloatOffset } }},
    {"Select",   3, { { "cond", EParticleArgKind::FloatOffset }, { "t", EParticleArgKind::FloatOffset }, { "f", EParticleArgKind::FloatOffset } }},
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

// Identifier → EParticleVar mapping for tick/spawn expressions.
// Returns false if the identifier is not a known particle-var alias.
// is_writable is true for slots the parser will accept as LHS of assignment;
// emit_pos/emit_vel are read-only (caller-bound bucket-anchor values).
bool PfxResolveParticleVarIdent(const char* ident, EParticleVar* out_var, bool* out_writable)
{
    struct SAlias
    {
        const char* name;
        EParticleVar var;
        bool writable;
    };
    static constexpr SAlias kAliases[] = {
        {"pos",       EParticleVar::DrawPos,    true},
        {"vel",       EParticleVar::EmitVel,    true},
        {"color",     EParticleVar::DrawColor,  true},
        {"scale",     EParticleVar::DrawScl,    true},
        {"frame",     EParticleVar::DrawFrame,  true},
        {"rot",       EParticleVar::DrawRot,    true},
        {"life",      EParticleVar::Life,       true},
        {"emit_pos",  EParticleVar::EmitPos,    false},
        {"emit_vel",  EParticleVar::EmitVel,    false},  // read-only alias for the bucket-anchor velocity
    };
    for (const SAlias& a : kAliases)
    {
        if (std::strcmp(ident, a.name) == 0)
        {
            if (out_var)      *out_var = a.var;
            if (out_writable) *out_writable = a.writable;
            return true;
        }
    }
    return false;
}

// Swizzle char → lane index. 'x'/'r'=0, 'y'/'g'=1, 'z'/'b'=2, 'w'/'a'=3.
int PfxSwizzleLane(char c)
{
    switch (c)
    {
        case 'x': case 'r': return 0;
        case 'y': case 'g': return 1;
        case 'z': case 'b': return 2;
        case 'w': case 'a': return 3;
        default: return -1;
    }
}

class TParticleExpressionParser
{
  public:
    TParticleExpressionParser(const char* text, std::vector<uint16_t>& out_code,
                              std::vector<float>& out_constants,
                              uint16_t& out_max_stack_depth,
                              uint8_t& out_result_lanes,
                              std::string* err,
                              EParticleExprMode mode)
        : src(text ? text : ""),
          code(out_code),
          constants(out_constants),
          max_stack_depth(out_max_stack_depth),
          result_lanes(out_result_lanes),
          error(err),
          mode(mode) {}

    bool Parse()
    {
        code.clear();
        constants.clear();
        max_stack_depth = 0;
        result_lanes = 1;
        stack_top = 0;

        if (mode == EParticleExprMode::Statements)
        {
            // Statement form: zero or more `var [.swizzle] = expr ;` then EOF.
            // No final result lanes; tick/spawn expressions don't return a value.
            while (true)
            {
                SkipWs();
                if (*src == 0)
                    break;
                if (!ParseStatement())
                    return false;
                // Each statement resets the stack frame -- intermediates are
                // gone after StoreVar, no inter-statement temporaries.
                stack_top = 0;
            }
            result_lanes = 0;
            return true;
        }

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
    EParticleExprMode mode = EParticleExprMode::Expression;

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

    // Emit a LoadVar for `v` and push it onto the stack with the var's
    // natural lane count (vec3 for pos/vel/scale/rot, vec4 for color, scalar
    // otherwise). Returns the stack ref. Lane==-1 means "all lanes"; a
    // non-negative lane reads a specific lane and pushes a scalar.
    SPfxValueRef EmitLoadVar(EParticleVar var, int lane = -1)
    {
        const int32_t natural = ParticleDefaultLanes(var);
        const uint8_t total_lanes = uint8_t(natural > 0 ? natural : 1);
        EmitOp(EParticleOp::LoadVar);
        EmitArg(uint16_t(var));
        if (lane >= 0)
        {
            // Read full var to stack, then pick a single lane via a
            // post-load swizzle. We model this by emitting LoadVar with
            // the full natural lane count and then using a "lane pick" --
            // implemented as a 1-lane stack push from the loaded slot.
            // Easiest: emit a scalar Op that reads the chosen lane.
            // The eval treats LoadVar as a multi-lane push automatically.
            // We then push a single-lane "Lerp(self, self, 0)" or a
            // simpler shape -- but the cleanest is to use a dedicated
            // lane-pick op. To avoid yet-another op, materialize the
            // single lane with a `Add(self_lane, 0_const)` pattern --
            // since read_arg handles lane-broadcast on scalar consts, we
            // can pop the full-lane stack entry and re-push only lane N
            // by emitting nothing here and tagging the SPfxValueRef
            // lanes=1 with the slot already wide enough. The evaluator's
            // read_arg(stack, lane) walks into stack[idx+lane] -- so for
            // a single-lane pick we just create a stack ref pointing at
            // the chosen sub-lane of the loaded slot.
            //
            // Implementation: bump the stack pointer past the loaded var
            // (which occupies natural lanes), then return a ref whose
            // arg points at stack[loaded_slot + lane] with lanes=1. We
            // rely on the fact that loaded var lanes are contiguous in
            // the eval stack[] array.
            const uint16_t base_slot = stack_top;
            stack_top = uint16_t(stack_top + total_lanes);
            if (stack_top > max_stack_depth) max_stack_depth = stack_top;
            return { PfxStackArg(uint16_t(base_slot + uint16_t(lane))), 1 };
        }
        return NewStackResult(total_lanes);
    }

    SPfxValueRef EmitVar(EParticleVar v) { return EmitLoadVar(v, -1); }

    // Parse one statement: `ident [.swizzle] = expr ;`
    // Statements compile to a StoreVar that writes the expression result
    // into the named particle var. For full-var assignment the result
    // expression's lane count must match the var's natural lane count.
    // For swizzle assignment (e.g. `pos.z = ...`) the RHS must be scalar.
    bool ParseStatement()
    {
        SkipWs();
        const char* ident_start = src;
        if (!(std::isalpha((unsigned char)*src) || *src == '_'))
            return Fail("expected identifier at start of statement");
        while (std::isalnum((unsigned char)*src) || *src == '_') ++src;
        const std::string ident(ident_start, src);

        EParticleVar var = EParticleVar::TimeFrame;
        bool writable = false;
        if (!PfxResolveParticleVarIdent(ident.c_str(), &var, &writable))
            return Fail("unknown statement LHS identifier");
        if (!writable)
            return Fail("LHS identifier is read-only");

        int lane = -1;
        if (Match('.'))
        {
            SkipWs();
            if (!*src) return Fail("expected swizzle after '.'");
            lane = PfxSwizzleLane(*src);
            ++src;
            if (lane < 0) return Fail("invalid swizzle");
        }

        if (!Match('=')) return Fail("expected '=' in statement");

        SPfxValueRef rhs;
        if (!ParseExpr(&rhs)) return false;
        if (!Match(';')) return Fail("expected ';' at end of statement");

        // Validate lane shape.
        const int32_t natural = ParticleDefaultLanes(var);
        if (lane < 0)
        {
            // Whole-var assignment.
            if (rhs.lanes != uint8_t(natural))
            {
                // Permit scalar→vec broadcast: if rhs is scalar and var
                // is multi-lane, emit Vec ops to fan out. Simplest: just
                // disallow and ask the author to write vec3(s,s,s).
                if (natural > 1 && rhs.lanes == 1)
                    return Fail("scalar->vec assignment requires explicit constructor (e.g. vec3(s,s,s))");
                if (rhs.lanes < uint8_t(natural))
                    return Fail("RHS lane count smaller than target var");
                // rhs has more lanes than var -- truncate by taking the first N.
                rhs.lanes = uint8_t(natural);
            }
        }
        else
        {
            if (rhs.lanes != 1)
                return Fail("swizzle assignment requires scalar RHS");
        }

        // Emit StoreVar(var, lane, src). For full-var the eval writes N
        // lanes from rhs starting at lane 0. For swizzle the eval writes
        // a single lane.
        EmitOp(EParticleOp::StoreVar);
        EmitArg(uint16_t(var));
        EmitArg(uint16_t(lane < 0 ? 0xFFFF : uint16_t(lane)));
        EmitArg(rhs.arg);
        return true;
    }

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
        if (!std::strcmp(name, "rand01"))
        {
            if (!Match(')')) return Fail("expected ')' after rand01()");
            EmitOp(EParticleOp::Rand01);
            if (out) *out = NewStackResult(1);
            return true;
        }
        if (!std::strcmp(name, "rand"))
        {
            SPfxValueRef lo, hi;
            if (!ParseExpr(&lo)) return false;
            if (!ParseCommaExpr(hi, "expected ',' after rand lo")) return false;
            if (!Match(')')) return Fail("expected ')' after rand arguments");
            if (out) *out = EmitValueOp(EParticleOp::Rand, { lo, hi });
            return true;
        }
        if (!std::strcmp(name, "step"))
        {
            SPfxValueRef edge, x;
            if (!ParseExpr(&edge)) return false;
            if (!ParseCommaExpr(x, "expected ',' after step edge")) return false;
            if (!Match(')')) return Fail("expected ')' after step arguments");
            if (out) *out = EmitValueOp(EParticleOp::Step, { edge, x });
            return true;
        }
        if (!std::strcmp(name, "select"))
        {
            SPfxValueRef cond, t, f;
            if (!ParseExpr(&cond)) return false;
            if (!ParseCommaExpr(t, "expected ',' after select cond")) return false;
            if (!ParseCommaExpr(f, "expected ',' after select t")) return false;
            if (!Match(')')) return Fail("expected ')' after select arguments");
            if (out) *out = EmitValueOp(EParticleOp::Select, { cond, t, f });
            return true;
        }
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

            // Optional swizzle (single lane: .x/.y/.z/.w/.r/.g/.b/.a).
            int lane = -1;
            if (*src == '.')
            {
                const char* dot_src = src;
                ++src;
                if (*src && (std::isalpha((unsigned char)*src)))
                {
                    lane = PfxSwizzleLane(*src);
                    if (lane < 0) return Fail("invalid swizzle");
                    ++src;
                }
                else
                {
                    // Not a swizzle, roll back.
                    src = dot_src;
                }
            }

            // Built-in eval-context scalars (always available).
            EParticleVar ctx_var = EParticleVar::TimeFrame;
            bool ctx_match = true;
            if      (ident == "time_frame") ctx_var = EParticleVar::TimeFrame;
            else if (ident == "age01")      ctx_var = EParticleVar::Age01;
            else if (ident == "age")        ctx_var = EParticleVar::Age;
            else if (ident == "seed")       ctx_var = EParticleVar::Seed;
            else ctx_match = false;
            if (ctx_match)
            {
                if (lane > 0) return Fail("swizzle on scalar identifier");
                if (out) *out = EmitVar(ctx_var);
                return true;
            }

            // Particle-var aliases (pos, vel, color, scale, frame, rot,
            // emit_pos, emit_vel, life). Resolved in both Expression and
            // Statements mode -- the stateless Eval path returns 0 for
            // particle vars (no bucket bound), the EvalParticle path
            // reads them from the bucket's slot.
            EParticleVar pv = EParticleVar::TimeFrame;
            bool writable = false;
            if (PfxResolveParticleVarIdent(ident.c_str(), &pv, &writable))
            {
                if (out) *out = EmitLoadVar(pv, lane);
                return true;
            }

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

bool TParticleExpression::Compile(const char* expr, std::string* error, EParticleExprMode mode)
{
    source = expr ? expr : "";
    this->mode = mode;
    TParticleExpressionParser parser(source.c_str(), code, constants, max_stack_depth, result_lanes, error, mode);
    valid = parser.Parse();
    if (valid)
        code.push_back(uint16_t(EParticleOp::End));
    return valid;
}

namespace {

// Park-Miller 32-bit LCG. Used by Rand01 to advance the per-particle
// rng_state on each call. Returns U[0,1).
float PfxRandAdvance(uint32_t& state)
{
    // Avoid the zero-trap that locks the generator: re-seed from a
    // golden-ratio constant if state==0.
    if (state == 0)
        state = 0x9e3779b9u;
    // multiplier 48271, modulus 2^31-1.
    constexpr uint64_t kMul = 48271ull;
    constexpr uint64_t kMod = 0x7fffffffull;
    state = uint32_t((uint64_t(state) * kMul) % kMod);
    return float(state) / float(kMod);
}

}   // namespace

namespace {

// Shared evaluator. bucket==nullptr → stateless ctx-only eval (matches
// the legacy Eval semantics: particle-var loads return 0 unless they're
// one of the four ctx-scalars). bucket != nullptr → tick/spawn eval:
// LoadVar reads from bucket[particle_index], StoreVar writes back.
void PfxEvalImpl(const std::vector<uint16_t>& code,
                 const std::vector<float>& constants,
                 const SParticleEvalContext& ctx,
                 float* out_values, int32_t out_lanes,
                 TParticleBucket* bucket,
                 int32_t particle_index,
                 const float* emit_pos,
                 const float* emit_vel,
                 uint16_t* out_last_arg,
                 uint8_t* out_last_lanes)
{
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

    auto read_ctx_var = [&](EParticleVar var, uint8_t lane) -> float {
        switch (var)
        {
            case EParticleVar::TimeFrame: return ctx.time_frame;
            case EParticleVar::Age:       return ctx.age;
            case EParticleVar::Age01:     return ctx.age01;
            case EParticleVar::Seed:      return ctx.seed;
            case EParticleVar::EmitPos:
                return emit_pos ? emit_pos[lane < 3 ? lane : 2] : 0.0f;
            case EParticleVar::EmitVel:
                if (bucket && particle_index >= 0)
                {
                    // EmitVel is per-particle-writable when bucket-bound;
                    // we still allow caller-supplied emit_vel override
                    // (read-only alias). Prefer the bucket-stored value
                    // if bucket is bound.
                    if (const float* slot = bucket->VarPtr(particle_index, EParticleVar::EmitVel))
                        return lane < 3 ? slot[lane] : slot[2];
                }
                return emit_vel ? emit_vel[lane < 3 ? lane : 2] : 0.0f;
            default:
                // Other particle-vars: only resolvable via bucket.
                if (bucket && particle_index >= 0)
                {
                    if (const float* slot = bucket->VarPtr(particle_index, var))
                    {
                        const int32_t lanes_for_var = ParticleDefaultLanes(var);
                        const uint8_t safe_lane = lane < uint8_t(lanes_for_var)
                                                   ? lane
                                                   : uint8_t(lanes_for_var > 0 ? lanes_for_var - 1 : 0);
                        return slot[safe_lane];
                    }
                }
                return 0.0f;
        }
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
        return read_ctx_var(EParticleVar(arg), lane);
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
                if (out_values)
                    for (int32_t lane = 0; lane < out_lanes; ++lane)
                        out_values[lane] = read_arg(last, uint8_t(lane));
                if (out_last_arg)   *out_last_arg = last;
                if (out_last_lanes) *out_last_lanes = last_lanes;
                return;
            case EParticleOp::LoadConst:
                push_scalar(read_arg(PfxConstArg(code[pc++])));
                break;
            case EParticleOp::LoadVar:
            {
                // Push the var's natural lane count onto the stack. This
                // is what lets `pos` evaluate to a vec3 in expressions.
                const EParticleVar var = EParticleVar(code[pc++]);
                const int32_t natural = ParticleDefaultLanes(var);
                const uint8_t lanes = uint8_t(natural > 0 ? natural : 1);
                float values[4] = {};
                for (uint8_t lane = 0; lane < lanes && lane < 4; ++lane)
                    values[lane] = read_ctx_var(var, lane);
                push_values(values, lanes);
                break;
            }
            case EParticleOp::StoreVar:
            {
                // StoreVar(var_id, lane_or_0xFFFF, src_arg). Writes the
                // src expression's value back into the bucket particle's
                // slot. No-op if bucket isn't bound (e.g. stateless eval
                // of a statement-mode expression -- which the parser
                // doesn't currently emit, but be defensive).
                const EParticleVar var = EParticleVar(code[pc++]);
                const uint16_t lane_arg = code[pc++];
                const uint16_t src_arg  = code[pc++];
                if (bucket && particle_index >= 0)
                {
                    if (float* slot = bucket->VarPtr(particle_index, var))
                    {
                        const int32_t natural = ParticleDefaultLanes(var);
                        if (lane_arg == 0xFFFF)
                        {
                            // Whole-var write: copy `natural` lanes from src.
                            for (int32_t lane = 0; lane < natural && lane < 4; ++lane)
                                slot[lane] = read_arg(src_arg, uint8_t(lane));
                        }
                        else if (int32_t(lane_arg) < natural)
                        {
                            slot[lane_arg] = read_arg(src_arg, 0);
                        }
                    }
                }
                break;
            }
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
            case EParticleOp::Rand01:
            {
                push_scalar(PfxRandAdvance(ctx.rng_state));
                break;
            }
            case EParticleOp::Rand:
            {
                const uint16_t lo_arg = code[pc++];
                const uint16_t hi_arg = code[pc++];
                const uint8_t lo_lanes = arg_lanes(lo_arg);
                const uint8_t hi_lanes = arg_lanes(hi_arg);
                const uint8_t lanes = lo_lanes > hi_lanes ? lo_lanes : hi_lanes;
                float values[4] = {};
                for (uint8_t lane = 0; lane < lanes && lane < 4; ++lane)
                {
                    const float lo = read_arg(lo_arg, lane);
                    const float hi = read_arg(hi_arg, lane);
                    const float u  = PfxRandAdvance(ctx.rng_state);
                    values[lane] = lo + (hi - lo) * u;
                }
                push_values(values, lanes);
                break;
            }
            case EParticleOp::Step:
            {
                const uint16_t edge_arg = code[pc++];
                const uint16_t x_arg    = code[pc++];
                const uint8_t e_lanes = arg_lanes(edge_arg);
                const uint8_t x_lanes = arg_lanes(x_arg);
                const uint8_t lanes = e_lanes > x_lanes ? e_lanes : x_lanes;
                float values[4] = {};
                for (uint8_t lane = 0; lane < lanes && lane < 4; ++lane)
                {
                    const float e = read_arg(edge_arg, lane);
                    const float x = read_arg(x_arg, lane);
                    values[lane] = x < e ? 0.0f : 1.0f;
                }
                push_values(values, lanes);
                break;
            }
            case EParticleOp::Select:
            {
                const uint16_t cond_arg = code[pc++];
                const uint16_t t_arg    = code[pc++];
                const uint16_t f_arg    = code[pc++];
                const uint8_t t_lanes = arg_lanes(t_arg);
                const uint8_t f_lanes = arg_lanes(f_arg);
                const uint8_t lanes = t_lanes > f_lanes ? t_lanes : f_lanes;
                float values[4] = {};
                for (uint8_t lane = 0; lane < lanes && lane < 4; ++lane)
                {
                    const float c = read_arg(cond_arg, lane);
                    const float t = read_arg(t_arg, lane);
                    const float f = read_arg(f_arg, lane);
                    values[lane] = c != 0.0f ? t : f;
                }
                push_values(values, lanes);
                break;
            }
        }
    }

    if (out_values)
        for (int32_t lane = 0; lane < out_lanes; ++lane)
            out_values[lane] = read_arg(last, uint8_t(lane));
    if (out_last_arg)   *out_last_arg = last;
    if (out_last_lanes) *out_last_lanes = last_lanes;
}

}   // namespace

void TParticleExpression::Eval(const SParticleEvalContext& ctx, float* out_values, int32_t out_lanes) const
{
    if (!out_values || out_lanes <= 0)
        return;
    SParticleEvalContext local_ctx = ctx;
    // Stateless eval: per-particle PRNG seeds from `seed`, advanced per
    // Rand01 call but not persisted (the caller's ctx is copied first).
    if (local_ctx.rng_state == 0)
        local_ctx.rng_state = uint32_t(local_ctx.seed * 1.0f) ^ 0x9e3779b9u;
    PfxEvalImpl(code, constants, local_ctx,
                out_values, out_lanes,
                /*bucket*/ nullptr, /*particle_index*/ -1,
                /*emit_pos*/ nullptr, /*emit_vel*/ nullptr,
                /*out_last_arg*/ nullptr, /*out_last_lanes*/ nullptr);
}

float TParticleExpression::Eval(const SParticleEvalContext& ctx) const
{
    float value = 0.0f;
    Eval(ctx, &value, 1);
    return value;
}

void TParticleExpression::EvalParticle(const SParticleEvalContext& ctx,
                                       TParticleBucket& bucket,
                                       int32_t particle_index,
                                       const float emit_pos[3],
                                       const float emit_vel[3]) const
{
    if (!valid || particle_index < 0)
        return;
    SParticleEvalContext local_ctx = ctx;
    if (local_ctx.rng_state == 0)
    {
        // Seed per-particle PRNG from the particle's own seed slot if it
        // has one, otherwise fall back to the supplied ctx.seed.
        if (const float* seed_slot = bucket.VarPtr(particle_index, EParticleVar::Seed))
            local_ctx.rng_state = uint32_t(*seed_slot * 16807.0f) ^ 0xdeadbeefu;
        else
            local_ctx.rng_state = uint32_t(local_ctx.seed * 16807.0f) ^ 0xdeadbeefu;
    }
    // No out_values -- statement expressions don't materialise a result.
    // Tick/spawn expressions only mutate via StoreVar.
    PfxEvalImpl(code, constants, local_ctx,
                /*out_values*/ nullptr, /*out_lanes*/ 0,
                &bucket, particle_index, emit_pos, emit_vel,
                /*out_last_arg*/ nullptr, /*out_last_lanes*/ nullptr);
    // Persist the advanced rng state back into the particle's seed slot
    // so successive ticks see distinct random values.
    if (float* seed_slot = bucket.VarPtr(particle_index, EParticleVar::Seed))
        *seed_slot = float(local_ctx.rng_state & 0x7fffffu) / float(0x7fffffu);
}

bool TParticleExpression::EvalParticlePredicate(const SParticleEvalContext& ctx,
                                                TParticleBucket& bucket,
                                                int32_t particle_index,
                                                const float emit_pos[3],
                                                const float emit_vel[3]) const
{
    if (!valid || particle_index < 0)
        return false;
    SParticleEvalContext local_ctx = ctx;
    if (local_ctx.rng_state == 0)
    {
        if (const float* seed_slot = bucket.VarPtr(particle_index, EParticleVar::Seed))
            local_ctx.rng_state = uint32_t(*seed_slot * 16807.0f) ^ 0xfeedfaceu;
        else
            local_ctx.rng_state = uint32_t(local_ctx.seed * 16807.0f) ^ 0xfeedfaceu;
    }
    float result = 0.0f;
    PfxEvalImpl(code, constants, local_ctx,
                &result, 1,
                &bucket, particle_index, emit_pos, emit_vel,
                nullptr, nullptr);
    return result != 0.0f;
}

