#include "vfxreviewpreview.h"
#include "time.h"
#include <algorithm>
#include <cstring>

namespace {
struct Mapping { uint32_t type_id; const char* preview_id; };
constexpr Mapping mappings[] = {
#include "vfxreviewpreviewmap.inc"
};

bool NeedsEndpoints(uint32_t id)
{
    switch (id) {
        case 0x63fd382au: case 0x63fd3827u: case 0x113803f4u:
        case 0xb1c4c90fu: case 0x10da54d0u: case 0xad92bd1eu:
        case 0xad92bd1fu: case 0xd000f005u: return true;
        default: return false;
    }
}

const VfxTest::SEffect* Find(uint32_t id, VfxReviewPreviewInfo* unavailable = nullptr)
{
    if (NeedsEndpoints(id)) return nullptr;
    // Lazy snapshot occurs after static registration, when review requests it.
    // SEffect copies retain their callbacks independently of browser selection.
    static const auto catalogue = VfxTest::ReviewCatalogue();
    for (const auto& mapping : mappings) {
        if (mapping.type_id != id) continue;
        const auto it = std::find_if(catalogue.begin(), catalogue.end(), [&](const auto& e) {
            return e.id == mapping.preview_id;
        });
        if (it == catalogue.end() || !it->factory || !it->destroy) continue;
        if (!it->submit) {
            if (it->submit_attached && unavailable)
                *unavailable={false,it->id.c_str(),"Preview attachment API unavailable; label only",false,true};
            continue;
        }
        using Style = VfxTest::EVfxPreviewStyle;
        // Character-style entries with a standalone submit callback already
        // accept this exact origin; only attachment-only hooks need a rig.
        if (it->preview_style == Style::Projectile) {
            if (unavailable) *unavailable={false,it->id.c_str(),"Preview endpoint API unavailable; label only",true};
            continue;
        }
        return &*it;
    }
    return nullptr;
}
}

VfxReviewPreviewInfo DescribeVfxReviewPreview(uint32_t type_id)
{
    if (NeedsEndpoints(type_id)) return {false,"","Preview endpoint API unavailable; label only",true};
    VfxReviewPreviewInfo result;
    if (const auto* effect = Find(type_id,&result))
        return {true,effect->id.c_str(),"Implemented preview controller; map caller not certified",false,false,bool(effect->is_alive)};
    return result;
}

VfxReviewPreview::VfxReviewPreview(const VfxTest::SEffect& effect, void* context, const S3DPoint& origin)
    : effect_(effect), context_(context), origin_(origin), restart_remaining_(RestartInterval()) {}
VfxReviewPreview::~VfxReviewPreview() { if (context_) effect_.destroy(context_); }
float VfxReviewPreview::RestartInterval() const { return VfxTest::ReviewRestartInterval(effect_.preview_style); }
void VfxReviewPreview::Submit()
{
    // Use the existing browser cadence explicitly; this is not a claim about
    // natural lifetime. Static controllers and their internal repeats stay as-is.
    const float interval=RestartInterval();
    if (interval>0) {
        restart_remaining_-=float(TTime::DeltaTime());
        if (restart_remaining_<=0) {
            if (context_) effect_.destroy(context_);
            context_=effect_.factory(origin_);
            restart_remaining_=interval;
        }
    }
    if (context_ && effect_.submit) effect_.submit(context_,EFxDebugMode::Normal);
}
void VfxReviewPreview::SubmitWorld() { if (context_ && effect_.submit_world) effect_.submit_world(context_,EFxDebugMode::Normal); }
std::optional<bool> VfxReviewPreview::Alive() const
{
    if (!context_) return false;
    if (!effect_.is_alive) return std::nullopt;
    return effect_.is_alive(context_);
}

std::unique_ptr<VfxReviewPreview> CreateVfxReviewPreview(uint32_t type_id, const S3DPoint& origin)
{
    const auto* effect = Find(type_id);
    if (!effect) return {};
    void* context = effect->factory(origin);
    if (!context) return {};
    return std::make_unique<VfxReviewPreview>(*effect,context,origin);
}
