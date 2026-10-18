// clang++ -std=c++17 -O2 -Wall -Wextra -pedantic tools/test_vfx_review_layout.cpp -o /tmp/test_vfx_review_layout
#include "../src/game/ab/vfxreviewlayout.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <cstdio>
#include <initializer_list>
#include <limits>
#include <utility>

static bool Near(double a, double b) { return std::abs(a-b) < 1e-8; }
static unsigned MaxVisibleCenters(double span, double separation)
{
    // Includes both viewport edges: N intervals contain N+1 centers.
    return unsigned(std::floor(span / separation)) + 1;
}
int main()
{
    for (double ratio : {2.0, 2.5, 3.0}) {
        for (const auto dimensions : {std::pair<double,double>{640,480}, {640,340},
                                       {640,120}, {320,480}, {1280,480}}) {
            VfxReviewLayout layout;
            layout.ratio=ratio; layout.width=dimensions.first; layout.height=dimensions.second;
            const double norm=std::hypot(ratio,1.0);
            const double span=(std::min)(layout.width*norm/ratio,layout.height*norm);
            assert(Near(layout.ViewSpan(),span));
            // A smaller requested setting can expose more small effects while explicit overrides remain absolute.
            layout.requested_minimum=180;
            const double small=layout.Clearance(32,180);
            assert(small>=180 && small+1e-8>=span/7.5);
            assert(MaxVisibleCenters(span,small)<=8);
            layout.requested_minimum=420;
            const double usual=layout.Clearance(32);
            assert(usual>=420);
            assert(MaxVisibleCenters(span,usual)<=4);
            assert(Near(layout.Clearance(32,1200),(std::max)(1200.0,usual)));
            // Automatic large-effect gaps stay near one viewport; explicit requests can be wider.
            const double bigFootprint=(std::max)(1000.0,0.75*span);
            const double big=layout.Clearance(bigFootprint);
            assert(Near(big,(std::max)(layout.MinimumClearance(),span*0.46)));
            assert(span/big<2.5); // time-averaged visible center density
            assert(layout.Separation(bigFootprint,big,bigFootprint,big)<= (std::max)(layout.MinimumClearance(),span*0.46));
            assert(layout.Separation(bigFootprint,big,32,usual)==big);
            assert(layout.Separation(32,usual,bigFootprint,big)==big);
            assert(Near(layout.Separation(bigFootprint,big,32,usual),
                        layout.Separation(32,usual,bigFootprint,big)));
            // An arbitrary rectangular screen footprint projects to |dx|W+|dy|H.
            const double rectangle=(ratio/norm)*400+(1/norm)*200;
            const double other=(ratio/norm)*120+(1/norm)*300;
            assert(layout.Separation(rectangle,layout.Clearance(rectangle),other,layout.Clearance(other)) <= (std::max)(layout.MinimumClearance(),span*0.46));
            // Large-footprint threshold stays capped; just below keeps its basic margin.
            const double threshold=0.75*span;
            assert(Near(layout.Clearance(threshold),
                        (std::max)(layout.MinimumClearance(),span*0.46)));
            assert(Near(layout.Clearance(threshold-0.01),
                        (std::max)(layout.MinimumClearance(),(std::min)((threshold-0.01+96)*0.4,span*0.46))));
            // Automatic gaps are bounded even when callers supply
            // small clearances rather than precomputing this policy's clearance.
            assert(Near(layout.Separation(1000,180,200,180),(std::max)(180.0,(std::min)(278.4,span*0.46))));
        }
    }
    VfxReviewLayout defaults;
    assert(Near(defaults.MinimumClearance(),96));
    assert(MaxVisibleCenters(defaults.ViewSpan(),defaults.Clearance(32))==8);
    assert(defaults.ViewSpan()/defaults.Clearance(1000)<2.5);
    defaults.width=0; assert(defaults.ViewSpan()==0);
    assert(defaults.MinimumClearance()==96);
    defaults.ratio=std::numeric_limits<double>::quiet_NaN(); assert(defaults.ViewSpan()==0);
    assert(defaults.Clearance(-1,-10)==96);
    std::puts("VFX review layout: compact density, bounded automatic gaps and explicit overrides pass");
}
