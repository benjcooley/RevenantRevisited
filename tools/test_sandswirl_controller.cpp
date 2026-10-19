#include "game/effects/legacy/sandswirlcontroller.h"
#include <cassert>
#include <iostream>
using namespace retail_sandswirl;
int main() {
    Controller missing;
    assert(!missing.Start(false));
    assert(!missing.Alive() && missing.Reason()==Stop::MissingChild);
    assert(!missing.Advance().reset_targets && missing.Frame()==0);
    Controller no_caller;
    assert(no_caller.Start(true));
    for(int i=1;i<=74;++i) assert(no_caller.Advance().reset_targets);
    assert(no_caller.NeedsQueryNextTick());
    assert(no_caller.Advance().kill && no_caller.Reason()==Stop::MissingCaller);
    assert(!no_caller.Advance().kill && no_caller.Frame()==75);
    Caller caller;
    caller.valid=true;caller.caster=1;caller.origin={3185,3185,16};caller.count=10;
    caller.found[0]={1,{3185,3185,16},102};
    caller.found[1]={2,{3281,3185,16},0};
    for(unsigned i=2;i<10;++i)caller.found[i]={i+1,{3281+int(i),3185,16},102};
    Controller many;many.Start(true);
    for(int i=1;i<=74;++i)many.Advance();
    const auto result=many.Advance(caller);
    assert(result.spawn_count==5 && result.spawn_for[0]==3 && result.spawn_for[4]==7);
    assert(many.Targets().count==5 && many.Targets().positions[0][2]==-32);
    assert(many.Targets().speed==7.5f && many.Targets().hitrange==10);
    Controller empty;empty.Start(true);
    for(int i=1;i<=74;++i)empty.Advance();
    caller.count=0;
    assert(empty.Advance(caller).spawn_count==0 && !empty.Targets().seeking);
    for(int i=76;i<300;++i)assert(!empty.Advance().kill);
    assert(empty.Advance().kill && !empty.Alive() && empty.Frame()==300);
    assert(!empty.Advance().kill && empty.Frame()==300);
    std::cout << "Sandswirl parent guard/selection/lifetime controls pass\n";
}
