// clang++ -std=c++17 -O2 -Wall -Wextra -pedantic tools/test_vfx_review_layout.cpp -o /tmp/test_vfx_review_layout
#include "../src/vfxreviewlayout.h"
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
static void NoOverlap(double leftFootprint, double rightFootprint, double separation)
{
    const double leftEnd = leftFootprint * 0.5;
    const double rightBegin = separation - rightFootprint * 0.5;
    assert(rightBegin - leftEnd >= 96.0 - 1e-8);
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
            // A smaller requested setting cannot crowd more than four centers.
            layout.requested_minimum=180;
            const double small=layout.Clearance(32,180);
            assert(small>=240 && small+1e-8>=span/3.0);
            assert(MaxVisibleCenters(span,small)<=4);
            layout.requested_minimum=420;
            const double usual=layout.Clearance(32);
            assert(usual>=420);
            assert(MaxVisibleCenters(span,usual)<=4);
            assert(Near(layout.Clearance(32,1200),(std::max)(1200.0,usual)));
            // Large effects reserve their own span plus a whole viewport.
            const double bigFootprint=(std::max)(1000.0,0.75*span);
            const double big=layout.Clearance(bigFootprint);
            assert(Near(big,(std::max)(layout.MinimumClearance(),bigFootprint+span+96)));
            assert(span/big<1.0); // time-averaged visible center density
            NoOverlap(bigFootprint,bigFootprint,layout.Separation(bigFootprint,big,bigFootprint,big));
            NoOverlap(bigFootprint,32,layout.Separation(bigFootprint,big,32,usual));
            NoOverlap(32,bigFootprint,layout.Separation(32,usual,bigFootprint,big));
            assert(Near(layout.Separation(bigFootprint,big,32,usual),
                        layout.Separation(32,usual,bigFootprint,big)));
            // An arbitrary rectangular screen footprint projects to |dx|W+|dy|H.
            const double rectangle=(ratio/norm)*400+(1/norm)*200;
            const double other=(ratio/norm)*120+(1/norm)*300;
            NoOverlap(rectangle,other,layout.Separation(rectangle,layout.Clearance(rectangle),
                                                       other,layout.Clearance(other)));
            // Exact threshold opts into isolation; just below retains basic margin.
            const double threshold=0.75*span;
            assert(Near(layout.Clearance(threshold),
                        (std::max)(layout.MinimumClearance(),threshold+span+96)));
            assert(Near(layout.Clearance(threshold-0.01),
                        (std::max)(layout.MinimumClearance(),threshold-0.01+96)));
            // Separation still prevents footprint overlap when callers supply
            // small clearances rather than precomputing this policy's clearance.
            assert(Near(layout.Separation(1000,180,200,180),696));
        }
    }
    VfxReviewLayout defaults;
    assert(Near(defaults.MinimumClearance(),240));
    assert(MaxVisibleCenters(defaults.ViewSpan(),defaults.Clearance(32))==3);
    assert(defaults.ViewSpan()/defaults.Clearance(1000)<1.0);
    defaults.width=0; assert(defaults.ViewSpan()==0);
    assert(defaults.MinimumClearance()==240);
    defaults.ratio=std::numeric_limits<double>::quiet_NaN(); assert(defaults.ViewSpan()==0);
    assert(defaults.Clearance(-1,-10)==240);
    std::puts("VFX review layout: density, isolation, mixed footprints, cropped views and interval separation pass");
}
