// Compile with src/vfxreviewpreview.cpp and src/time.cpp. The tiny catalogue
// provider makes any accidental browser initialization an unresolved symbol.
#include "../src/vfxreviewpreview.h"
#include "../src/time.h"
#include "../src/effects/paralizepreview.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <iostream>
#include <cstring>

namespace {
int catalogue_reads=0, creates=0, destroys=0, ticks=0, worlds=0, dragon_negative=0, drip_negative=0, blood_negative=0;
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
    auto drip=Row("TDripEffect_LINKED_NO_SPLASH",EVfxPreviewStyle::Static);
    drip.is_alive=[](void*){return true;};
    if(drip_negative==1)drip.is_alive={};
    if(drip_negative==2)drip.submit_world={};
    if(drip_negative==3)drip.preview_style=EVfxPreviewStyle::Combat;
    if(drip_negative==4)drip.id="TDripEffect_LINKED_DRAIN";
    auto authored_blood=Row("Blood_AUTHORED",EVfxPreviewStyle::Static);
    authored_blood.is_alive=[](void* c){return *static_cast<int*>(c)<2;};
    authored_blood.submit=[](void* c,EFxDebugMode){++*static_cast<int*>(c);};
    if(blood_negative==1)authored_blood.is_alive={};
    if(blood_negative==2)authored_blood.submit_world={};
    if(blood_negative==3)authored_blood.preview_style=EVfxPreviewStyle::Combat;
    if(blood_negative==4)authored_blood.id="TBloodEffect";
    auto sparks=Row("TSparksEffect_BESPOKE",EVfxPreviewStyle::Combat);
    sparks.is_alive=[](void*){return true;};
    auto blood=Row("TBloodEffect_BESPOKE",EVfxPreviewStyle::Combat);
    blood.is_alive=[](void*){return true;};
    auto flare=Row("TCureEffect_BESPOKE",EVfxPreviewStyle::CharacterCast);
    flare.submit={};flare.submit_attached=[](void*,EFxDebugMode,const SVfxAttachment&){};
    auto ripple=Row("TRippleEffect",EVfxPreviewStyle::Static);ripple.factory=[](const S3DPoint&)->void*{return nullptr;};
    auto finite=Row("TMPAppear_AUTHORED_START",EVfxPreviewStyle::Static);
    finite.is_alive=[](void* c){return *static_cast<int*>(c)<2;};
    finite.submit=[](void* c,EFxDebugMode){++*static_cast<int*>(c);};
    auto queen=Row("QueenArrow_ORIGINAL",EVfxPreviewStyle::Projectile);
    queen.factory_endpoints=[](const S3DPoint& source,const S3DPoint& target)->void*{
        assert(source.x==-90 && source.y==40 && source.z==128);
        assert(target.x==80 && target.y==-120 && target.z==128);
        ++creates;return new int(0);
    };
    queen.is_alive=[](void* c){return *static_cast<int*>(c)<2;};
    queen.submit=[](void* c,EFxDebugMode){++*static_cast<int*>(c);};
    auto head=Row("HeadFireBall_ORIGINAL",EVfxPreviewStyle::Static);
    head.is_alive=[](void* c){return *static_cast<int*>(c)<2;};
    head.submit=[](void* c,EFxDebugMode){++*static_cast<int*>(c);};
    auto dragon=Row("Dragonattack_ORIGINAL",EVfxPreviewStyle::Static);
    dragon.is_alive=[](void* c){return *static_cast<int*>(c)<2;};
    dragon.submit=[](void* c,EFxDebugMode){++*static_cast<int*>(c);};
    if(dragon_negative==1)dragon.is_alive={};
    if(dragon_negative==2)dragon.submit_world={};
    if(dragon_negative==3)dragon.preview_style=EVfxPreviewStyle::Projectile;
    if(dragon_negative==4)dragon.id="TFireSwarmEffect_BESPOKE__dragonattack";
    auto paralize=Row("TParalize1_CONFIGURED_TARGET",EVfxPreviewStyle::Static);
    paralize.is_alive=[](void* c){return *static_cast<int*>(c)<2;};
    paralize.submit=[](void* c,EFxDebugMode){++*static_cast<int*>(c);};
    auto manadrain=paralize;manadrain.id="TManaDrain_CONFIGURED_TARGET";
    auto ymanadrain=manadrain;ymanadrain.id="TYManaDrain_CONFIGURED_TARGET";
    auto tornado=ymanadrain;tornado.id="TTornado_CONFIGURED_TARGET";
    return {authored_blood,drip,dragon,tornado,ymanadrain,manadrain,paralize,head,queen,finite,Row("TBloodEffect",EVfxPreviewStyle::Combat),blood,
            Row("TStreamerEffect_BESPOKE",EVfxPreviewStyle::Static),
            Row("TFireBallEffect",EVfxPreviewStyle::Projectile),flare,ripple,
            Row("TAuraEffect_BESPOKE",EVfxPreviewStyle::CharacterIdle),
            Row("TFogEffect_BESPOKE",EVfxPreviewStyle::Static),
            Row("TJhagaAttackEffect_BESPOKE",EVfxPreviewStyle::Projectile),
            sparks,
            Row("TBlastEffect_BESPOKE",EVfxPreviewStyle::Combat),
            Row("TFairyEffect_BESPOKE",EVfxPreviewStyle::Static),
            Row("TTornadoEffect_BESPOKE",EVfxPreviewStyle::Static),
            Row("TBuffEffect_Bespoke__Stoneskin_BESPOKE",EVfxPreviewStyle::Static),
            Row("TWaterFallEffect_BESPOKE__RiverFall",EVfxPreviewStyle::Static)};
}
// Deliberately different value proves the bridge delegates policy instead of
// hardcoding a second cadence. Production delegates to RetriggerInterval.
float ReviewRestartInterval(EVfxPreviewStyle style) {
    return style==EVfxPreviewStyle::Combat?.5f:style==EVfxPreviewStyle::Projectile?.001f:0;
}
}
int main(int argc,char** argv) {
    const S3DPoint origin{17,33,21};
    if(argc==2 && std::strlen(argv[1])==2 && argv[1][0]=='b'){
        blood_negative=argv[1][1]-'0';assert(blood_negative>=1 && blood_negative<=4);
        assert(!DescribeVfxReviewPreview(0xddc4042eu).supported);
        assert(!CreateVfxReviewPreview(0xddc4042eu,origin));
        assert(creates==0 && destroys==0);return 0;
    }
    if(argc==2 && std::strlen(argv[1])==2 && argv[1][0]=='d'){
        drip_negative=argv[1][1]-'0';assert(drip_negative>=1 && drip_negative<=4);
        assert(!DescribeVfxReviewPreview(0x5975abdeu).supported);
        assert(!CreateVfxReviewPreview(0x5975abdeu,origin));
        assert(creates==0 && destroys==0);return 0;
    }
    if(argc>1){
        assert(argc==2 && std::strlen(argv[1])==1 && argv[1][0]>='1' && argv[1][0]<='4');
        dragon_negative=argv[1][0]-'0';
        assert(!DescribeVfxReviewPreview(0xba09feadu).supported);
        assert(!CreateVfxReviewPreview(0xba09feadu,origin));
        assert(!CreateVfxReviewPreview(0xba09feadu,origin,{80,-120,128}));
        assert(creates==0 && destroys==0);
        std::cout<<"Dragon invalid route rejected\n";return 0;
    }
    assert(!DescribeVfxReviewPreview(0xffffffff).supported);
    assert(DescribeVfxReviewPreview(0x63fd382a).requires_endpoints);
    assert(!CreateVfxReviewPreview(0x63fd382a,origin));
    assert(DescribeVfxReviewPreview(0x152dafef).requires_attachment);
    assert(!CreateVfxReviewPreview(0x152dafef,origin));
    assert(!CreateVfxReviewPreview(0x12309867,origin));
    // A registered, callable factory is insufficient: the implementation audit
    // must reject null bodies and invalid animator aliases before spawning.
    for (uint32_t id : {0xd0c0f03au}) {
        assert(!DescribeVfxReviewPreview(id).supported);
        assert(!CreateVfxReviewPreview(id,origin));
    }
    for (uint32_t id : {0x0ea34fa3u,0xad92bd33u,0x00ab1d1du,0xc24b2a85u}) {
        const auto info=DescribeVfxReviewPreview(id);
        assert(info.supported && info.partial);
        assert(std::strstr(info.description,"Partial preview:"));
        assert(std::strstr(info.description,"not complete original implementation"));
    }
    const auto fairy=DescribeVfxReviewPreview(0xad92bd25u);
    assert(fairy.supported && !fairy.partial);
    const auto stationary=DescribeVfxReviewPreview(0xaeaeeb32u);
    assert(stationary.supported && stationary.partial && stationary.requires_endpoints);
    assert(std::strstr(stationary.description,"stationary"));
    assert(!DescribeVfxReviewPreview(0x14db0f2e).partial);
    assert(DescribeVfxReviewPreview(0x14db0f2e).supported); // replaced Sparks stub
    assert(creates==0);
    {
        auto blood=CreateVfxReviewPreview(0x14db0f2e,origin);
        assert(blood && blood->Id()=="TSparksEffect_BESPOKE");
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
    {
        auto finite=CreateVfxReviewPreview(0xad99fdf2,origin);
        assert(finite && finite->RestartInterval()==0 && finite->Alive()==true);
        finite->Submit(); finite->Submit();
        assert(finite->Alive()==false && creates==4 && destroys==3);
        finite->Submit();
        assert(finite->Alive()==true && creates==5 && destroys==4);
    }
    assert(creates==destroys); // Natural completion recreates without a timer.
    {
        auto partial=CreateVfxReviewPreview(0x0ea34fa3u,origin);
        assert(partial && partial->Id()=="TBlastEffect_BESPOKE");
        const int before=ticks;
        partial->Submit(); partial->SubmitWorld();
        assert(ticks==before+1);
    }
    assert(creates==destroys);
    {
        const auto info=DescribeVfxReviewPreview(0xad92bd1e);
        assert(info.supported && info.requires_endpoints && info.lifetime_known);
        assert(!CreateVfxReviewPreview(0xad92bd1e,origin));
        auto queen=CreateVfxReviewPreview(0xad92bd1e,{-90,40,128},{80,-120,128});
        assert(queen && queen->Alive()==true);
        const int initial=creates;
        TTime::BeginFixedFrame();queen->Submit();
        TTime::BeginFixedFrame();queen->Submit();
        assert(queen->Alive()==false && creates==initial);
        queen->Submit();assert(queen->Alive()==true && creates==initial+1);
    }
    assert(creates==destroys);
    {
        const auto info=DescribeVfxReviewPreview(0xd000f005u);
        assert(info.supported && !info.requires_endpoints && info.lifetime_known);
        assert(!CreateVfxReviewPreview(0xd000f005u,{-90,40,128},{80,-120,128}));
        auto head=CreateVfxReviewPreview(0xd000f005u,origin);
        assert(head && head->Id()=="HeadFireBall_ORIGINAL" && head->Alive()==true);
        const int initial=creates;
        head->Submit();head->Submit();
        assert(head->Alive()==false && creates==initial);
        head->Submit();assert(head->Alive()==true && creates==initial+1);
    }
    assert(creates==destroys);
    {
        const auto info=DescribeVfxReviewPreview(0xba09feadu);
        assert(info.supported && !info.partial && !info.requires_endpoints && info.lifetime_known);
        assert(std::strstr(info.description,"target offset(280,0), ground0"));
        assert(!CreateVfxReviewPreview(0xba09feadu,origin,{80,-120,128}));
        auto dragon=CreateVfxReviewPreview(0xba09feadu,origin);
        assert(dragon && dragon->Id()=="Dragonattack_ORIGINAL" && dragon->RestartInterval()==0);
        const int initial=creates;
        dragon->Submit();dragon->Submit();assert(dragon->Alive()==false && creates==initial);
        dragon->Submit();assert(dragon->Alive()==true && creates==initial+1);
        dragon->SubmitWorld();
    }
    assert(creates==destroys);
    {
        static_assert(paralize_preview::kConfiguredTarget.source_x==3185);
        static_assert(paralize_preview::kConfiguredTarget.target_dx==96);
        static_assert(paralize_preview::kConfiguredTarget.expected_radius==16);
        const auto info=DescribeVfxReviewPreview(0xad92bc14u);
        assert(info.supported && !info.partial && !info.requires_endpoints && info.lifetime_known);
        assert(std::strstr(info.description,"Configured genuine Acolyte"));
        assert(!CreateVfxReviewPreview(0xad92bc14u,origin,{80,-120,128}));
        auto target=CreateVfxReviewPreview(0xad92bc14u,origin);
        assert(target && target->Id()=="TParalize1_CONFIGURED_TARGET" && target->RestartInterval()==0);
        const int initial=creates;
        target->Submit();target->Submit();assert(target->Alive()==false && creates==initial);
        target->Submit();assert(target->Alive()==true && creates==initial+1);
    }
    assert(creates==destroys);
    {
        const auto info=DescribeVfxReviewPreview(0xad92bc13u);
        assert(info.supported && !info.partial && !info.requires_endpoints && info.lifetime_known);
        assert(std::strstr(info.description,"actual mana transfer"));
        assert(!CreateVfxReviewPreview(0xad92bc13u,origin,{80,-120,128}));
        auto target=CreateVfxReviewPreview(0xad92bc13u,origin);
        assert(target && target->Id()=="TManaDrain_CONFIGURED_TARGET" && target->RestartInterval()==0);
        const int initial=creates;target->Submit();target->Submit();assert(target->Alive()==false && creates==initial);
        target->Submit();assert(target->Alive()==true && creates==initial+1);
    }
    assert(creates==destroys);
    {
        const auto info=DescribeVfxReviewPreview(0x5975abdeu);
        assert(info.supported && !info.partial && info.lifetime_known && !info.requires_endpoints);
        assert(std::strstr(info.description,"SetParams(20,128,1)"));
        auto drip=CreateVfxReviewPreview(0x5975abdeu,origin);
        assert(drip && drip->Id()=="TDripEffect_LINKED_NO_SPLASH" && drip->RestartInterval()==0);
        const int initial=creates;
        for(int i=0;i<1000;++i){TTime::BeginFixedFrame();drip->Submit();drip->SubmitWorld();}
        assert(drip->Alive()==true && creates==initial);
        assert(!CreateVfxReviewPreview(0x5975abdeu,origin,{80,-120,128}));
    }
    assert(creates==destroys);
    {
        const auto info=DescribeVfxReviewPreview(0xae92bc14u);
        assert(info.supported && !info.partial && !info.requires_endpoints && info.lifetime_known);
        assert(std::strstr(info.description,"uncapped mana transfer"));
        assert(!CreateVfxReviewPreview(0xae92bc14u,origin,{80,-120,128}));
        auto target=CreateVfxReviewPreview(0xae92bc14u,origin);
        assert(target && target->Id()=="TYManaDrain_CONFIGURED_TARGET" && target->RestartInterval()==0);
        const int initial=creates;target->Submit();target->Submit();assert(target->Alive()==false && creates==initial);
        target->Submit();assert(target->Alive()==true && creates==initial+1);
    }
    assert(creates==destroys);
    {
        const auto info=DescribeVfxReviewPreview(0xddc4042eu);
        assert(info.supported && !info.partial && info.lifetime_known);
        assert(std::strstr(info.description,"live global RNG without reseed"));
        auto spray=CreateVfxReviewPreview(0xddc4042eu,origin);
        assert(spray && spray->Id()=="Blood_AUTHORED" && spray->RestartInterval()==0);
        const int initial=creates;TTime::BeginFixedFrame();spray->Submit();TTime::BeginFixedFrame();spray->Submit();assert(spray->Alive()==false);
        TTime::BeginFixedFrame();spray->Submit();assert(spray->Alive()==true && creates==initial+1);
    }
    assert(creates==destroys);
    {
        const auto info=DescribeVfxReviewPreview(0x2784abdeu);
        assert(info.supported && !info.partial && !info.requires_endpoints && info.lifetime_known);
        assert(std::strstr(info.description,"natural variant selects Funnel"));
        assert(!CreateVfxReviewPreview(0x2784abdeu,origin,{80,-120,128}));
        auto target=CreateVfxReviewPreview(0x2784abdeu,origin);
        assert(target && target->Id()=="TTornado_CONFIGURED_TARGET" && target->RestartInterval()==0);
        const int initial=creates;target->Submit();target->Submit();assert(target->Alive()==false && creates==initial);
        target->Submit();assert(target->Alive()==true && creates==initial+1);
    }
    assert(creates==destroys);
    std::cout<<"review preview mapping/lifecycle/callback tests passed\n";
}
