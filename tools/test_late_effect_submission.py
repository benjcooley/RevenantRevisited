#!/usr/bin/env python3
"""Compile the actual per-gather replacement gate with focused owner fixtures.

This exercises dispatch counts, not the GPU or full map/component lifecycle.
"""
from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parents[1]
source=(root/'src/maprenderer.cpp').read_text()
start=source.index('        // A late replacement can inherit several cached submesh records.')
end=source.index('        // obj_id = drawable index + 1',start)
gate=source[start:end]
declaration='    std::unordered_set<TObjectInstance*> submitted_replacement_owners;'
assert declaration in source
prelude=r'''
#include <array>
#include <cassert>
#include <type_traits>
#include <unordered_set>
#include <vector>
struct TParticleEffectComponent{};
struct TFlipbookBillboardComponent{bool replacement=false;bool ReplacesDefaultVisual()const{return replacement;}};
struct TObjectInstance{int id;bool particles=false,has_visual=false;TParticleEffectComponent particle;TFlipbookBillboardComponent visual;
 template<class T>T*GetComponent(){if constexpr(std::is_same_v<T,TParticleEffectComponent>)return particles?&particle:nullptr;else return has_visual?&visual:nullptr;}};
struct Ref{TObjectInstance*owner;TObjectInstance*Get()const{return owner;}};
struct Record{Ref src;};struct Scene{std::vector<Record>sectorDrawInst;};
'''
body=r'''
std::array<int,5>Gather(Scene&s){std::array<int,5>calls{};
DECLARATION
 for(int idx=0;idx<int(s.sectorDrawInst.size());++idx){
GATE
  if(auto*owner=s.sectorDrawInst[idx].src.Get())++calls[owner->id];
 }
 return calls;
}
int main(){TObjectInstance late{0},ordinary{1},mixed_cached{2},particle_owner{3},current_billboard{4};
 late.has_visual=mixed_cached.has_visual=current_billboard.has_visual=particle_owner.has_visual=true;
 late.visual.replacement=mixed_cached.visual.replacement=current_billboard.visual.replacement=particle_owner.visual.replacement=true;
 particle_owner.particles=true;
 // late: three cached mesh/texture runs; mixed: an old billboard plus mesh;
 // ordinary: three real mesh runs; particle owners retain their existing path.
 Scene s{{{{&late}},{{&late}},{{&late}},{{&ordinary}},{{&ordinary}},{{&ordinary}},{{&mixed_cached}},{{&mixed_cached}},{{&particle_owner}},{{&particle_owner}},{{&particle_owner}},{{&current_billboard}},{{nullptr}}}};
 auto first=Gather(s),next=Gather(s);
 assert((first==std::array<int,5>{1,3,1,3,1}));assert(next==first);
 // Before the new gate, late/mixed were submitted three/two times per gather.
}
'''
with tempfile.TemporaryDirectory(prefix='revenant-late-replacement-')as tmp:
 path=Path(tmp)/'regression.cpp';binary=Path(tmp)/'regression'
 path.write_text(prelude+body.replace('DECLARATION',declaration).replace('GATE',gate))
 subprocess.run(['clang++','-std=c++17',str(path),'-o',str(binary)],check=True)
 subprocess.run([str(binary)],check=True)
 # The same meaningful fixture must fail with the pre-fix gather dispatch.
 path.write_text(prelude+body.replace('DECLARATION','').replace('GATE',''))
 subprocess.run(['clang++','-std=c++17',str(path),'-o',str(binary)],check=True)
 old=subprocess.run([str(binary)],capture_output=True)
 assert old.returncode!=0,'Pre-fix duplicate submission unexpectedly passed'
print('PASS: late/mixed replacements once per gather, next gather resets; ordinary, particle and existing billboard paths preserved; pre-fix fails.')
