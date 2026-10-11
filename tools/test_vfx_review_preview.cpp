// Compile with src/vfxreviewpreview.cpp and src/time.cpp. The tiny catalogue
// provider makes any accidental browser initialization an unresolved symbol.
#include "../src/vfxreviewpreview.h"
#include "../src/time.h"
#include <cassert>
#include <iostream>

namespace {
int catalogue_reads=0, creates=0, destroys=0, ticks=0, worlds=0;
void* Spawn(const S3DPoint& p) {
    assert(p.x==17 && p.y==33 && p.z==21); ++creates; return new int(0);
}
VfxTest::SEffect Row(const char* id,VfxTest::EVfxPreviewStyle style) {
    VfxTest::SEffect e; e.id=id;e.preview_style=style;e.factory=Spawn;
    e.submit=[](void*,EFxDebugMode){++ticks;};
    e.submit_world=[](void*,EFxDebugMode){++worlds;};
    e.destroy=[](void* c){++destroys;delete static_cast<int*>(c);};
    return e;
}
}
namespace VfxTest {
std::vector<SEffect> ReviewCatalogue() {
    ++catalogue_reads;
    auto blood=Row("TBloodEffect_BESPOKE",EVfxPreviewStyle::Combat);
    blood.is_alive=[](void*){return true;};
    auto flare=Row("TFlareAnimator_BESPOKE",EVfxPreviewStyle::CharacterCast);
    flare.submit={};flare.submit_attached=[](void*,EFxDebugMode,const SVfxAttachment&){};
    auto ripple=Row("TRippleEffect",EVfxPreviewStyle::Static);ripple.factory=[](const S3DPoint&)->void*{return nullptr;};
    return {Row("TBloodEffect",EVfxPreviewStyle::Combat),blood,
            Row("TStreamerEffect_BESPOKE",EVfxPreviewStyle::Static),
            Row("TFireBallEffect",EVfxPreviewStyle::Projectile),flare,ripple};
}
// Deliberately different value proves the bridge delegates policy instead of
// hardcoding a second cadence. Production delegates to RetriggerInterval.
float ReviewRestartInterval(EVfxPreviewStyle style) { return style==EVfxPreviewStyle::Combat?.5f:0; }
}
int main() {
    const S3DPoint origin{17,33,21};
    assert(!DescribeVfxReviewPreview(0xffffffff).supported);
    assert(DescribeVfxReviewPreview(0x63fd382a).requires_endpoints);
    assert(!CreateVfxReviewPreview(0x63fd382a,origin));
    assert(DescribeVfxReviewPreview(0x120054d1).requires_attachment);
    assert(!CreateVfxReviewPreview(0x120054d1,origin));
    assert(!CreateVfxReviewPreview(0x12309867,origin));
    assert(creates==0);
    {
        auto blood=CreateVfxReviewPreview(0xddc4042e,origin);
        assert(blood && blood->Id()=="TBloodEffect_BESPOKE");
        assert(blood->Alive()==true && blood->RestartInterval()==.5f);
        for(int i=0;i<10;++i) {TTime::BeginFixedFrame();blood->Submit();blood->SubmitWorld();}
        assert(creates==1 && destroys==0 && ticks==10 && worlds==10);
        for(int i=0;i<3;++i) {TTime::BeginFixedFrame();blood->Submit();}
        assert(creates==2 && destroys==1 && ticks==13 && worlds==10);
        auto stream=CreateVfxReviewPreview(0x482dfe82,origin);
        assert(stream && !stream->Alive().has_value() && stream->RestartInterval()==0);
        for(int i=0;i<100;++i) {TTime::BeginFixedFrame();stream->Submit();}
        assert(creates==3 && destroys==1);
    }
    assert(destroys==3 && catalogue_reads==1);
    std::cout<<"review preview mapping/lifecycle/callback tests passed\n";
}
