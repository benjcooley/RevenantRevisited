"""Guard the merged effect Pulse policies using actual production function bodies.

The harness isolates animator/header/visual ownership and map-reap flags.
It does not claim native API/device or complete effect lifetime equivalence.
Run directly or include in unittest discovery; requires a C++17 compiler.
"""
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT=Path(__file__).resolve().parents[2]


def function(source,signature):
    start=source.index(signature);opening=source.index('{',start);depth=1;end=opening+1
    while depth:
        if source[end]=='{':depth+=1
        elif source[end]=='}':depth-=1
        end+=1
    return source[start:end]


class EffectLifecycleTests(unittest.TestCase):
    def test_actual_merged_pulse_lifecycle(self):
        source=(ROOT/'src/effect.cpp').read_text()
        prelude=r'''#include <cstdio>
#include <cstdlib>
#include <initializer_list>
#include <functional>
#define log_debug(...) ((void)0)
constexpr unsigned OF_KILL=0x1000,AF_LOOPING=1;
struct TFlipbookBillboardComponent{bool replacing;bool ReplacesDefaultVisual()const{return replacing;}};
struct Image{int flags{};int GetAniFlags(int)const{return flags;}};
struct TObjectInstance{int pulses{};std::function<void()> script;void Pulse(){++pulses;if(script)script();}};
struct TEffect:TObjectInstance{void*spell{};bool animator=true,done=true;Image*imagery{};unsigned flags=0x40004;TFlipbookBillboardComponent*visual{};int frame=7;
 template<class T>T*GetComponent()const{return static_cast<T*>(visual);}bool HasAnimator()const{return animator;}
 bool CommandDone()const{return done;}void SetCommandDone(bool x){done=x;}void SetFrame(int x){frame=x;}
 int GetState()const{return 0;}const char*GetName()const{return "fixture";}
 unsigned GetFlags()const{return flags;}void SetFlags(unsigned x){flags|=x;}void Pulse();};
struct TFireBallEffect:TEffect{bool alive_=true;int steps{};void StepMissilePulse(){++steps;}void StepAnimate(){++steps;}void KillThisEffect(){flags|=OF_KILL;}void Pulse();};
'''
        tests=r'''
void check(bool condition,const char*name){if(!condition){std::fprintf(stderr,"FAIL %s\n",name);std::exit(1);}}
int main(){Image loop{1},once{0};TFlipbookBillboardComponent replacement{true},ordinary{false};int count=0;
for(int kind=0;kind<8;++kind){TEffect e;e.imagery=&once;bool killed=true;
 switch(kind){case 0:break;case 1:e.done=false;killed=false;break;case 2:e.imagery=&loop;killed=false;break;case 3:e.spell=&once;killed=false;break;case 4:e.visual=&replacement;killed=false;break;case 5:e.visual=&ordinary;break;case 6:e.animator=false;killed=false;break;case 7:e.imagery=nullptr;killed=false;break;}
 e.Pulse();check(bool(e.flags&OF_KILL)==killed,"generic/loop/spell/replacing visual");check((e.flags&0x40004)==0x40004,"map flags preserved");check(e.pulses==1&&!e.done,"base Pulse and command handshake");check(e.frame==7,"owner animation frame preserved");++count;}
for(const char*owner:{"Cure","Mist","Drip"}){TEffect e;e.imagery=&once;e.visual=&replacement;e.Pulse();check(!(e.flags&OF_KILL),owner);++count;}
for(const char*owner:{"Might","Immortalmight","Fmastery","Shadowfist","Warriorborn","Teleportation","WaterFlft","PunchAndJudy"}){TEffect e;e.imagery=&loop;e.Pulse();check(!(e.flags&OF_KILL)&&e.frame==7,owner);++count;}
for(const char*owner:{"GoldEffect","CombatFlash-state0","MPAppear","gvortex","Partsys nonloop"}){TEffect e;e.imagery=&once;e.Pulse();check((e.flags&OF_KILL)&&e.frame==7,owner);++count;}
TEffect story;Image changing{0};story.imagery=&changing;story.script=[&]{changing.flags=AF_LOOPING;};story.Pulse();check(!(story.flags&OF_KILL),"script state transition precedes cleanup");++count;
TFireBallEffect projectile;projectile.imagery=&once;projectile.Pulse();check(!(projectile.flags&OF_KILL)&&projectile.steps==2,"FireBall explicit original command handshake");++count;
projectile.alive_=false;projectile.Pulse();check(projectile.flags&OF_KILL,"FireBall terminal simulator requests kill");++count;
std::printf("PASS %d actual production lifecycle scenarios\n",count);}
'''
        bodies=function(source,'void TEffect::Pulse()')+'\n'+function(source,'void TFireBallEffect::Pulse()')
        with tempfile.TemporaryDirectory(prefix='revenant-effect-lifecycle-')as tmp:
            cpp=Path(tmp)/'lifecycle.cpp';binary=Path(tmp)/'lifecycle';cpp.write_text(prelude+bodies+tests)
            result=subprocess.run([os.environ.get('CXX','clang++'),'-std=c++17',str(cpp),'-o',str(binary)],text=True,capture_output=True)
            self.assertEqual(result.returncode,0,result.stderr)
            run=subprocess.run([str(binary)],text=True,capture_output=True)
            self.assertEqual(run.returncode,0,run.stdout+run.stderr)
            self.assertIn('PASS 27 actual production lifecycle scenarios',run.stdout)


if __name__=='__main__':unittest.main()
