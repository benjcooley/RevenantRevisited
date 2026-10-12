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
        prelude=r'''#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <initializer_list>
#include <functional>
#include "effectlightramp.h"
#define log_debug(...) ((void)0)
// object.h's values: the fixture's 0x40004 is OF_NOTIFY|OF_LIGHT.
constexpr unsigned OF_LIGHT=1u<<2,OF_INVISIBLE=1u<<7,OF_KILL=1u<<12,OF_ANIMATE=1u<<14,OF_PULSE=1u<<15,AF_LOOPING=1;
constexpr int OBJCLASS_CHARACTER=1,OBJCLASS_PLAYER=2;
bool RealTimeLight=true;
struct S3DPoint{int32_t x{},y{},z{};};
struct SColor{uint8_t red{},green{},blue{};};
struct TFlipbookBillboardComponent{bool replacing;bool ReplacesDefaultVisual()const{return replacing;}};
struct Image{int flags{};int GetAniFlags(int)const{return flags;}};
struct TObjectInstance{int pulses{};std::function<void()> script;void Pulse(){++pulses;if(script)script();}
 S3DPoint Pos()const{return {};}int GetLevel()const{return 0;}int ObjClass()const{return 0;}void SetPoisoned(int){}};
struct TActionBlock{bool Is(const char*)const{return false;}};
struct TCharacter:TObjectInstance{const TActionBlock*RootBlock()const{return nullptr;}};
struct Ref{TObjectInstance*Get()const{return nullptr;}};
// A spell without LIGHT, variant or poison: the lifecycle boundary only.
// Follow, timed life, RANGEDAMAGE and the light ramp have the effect-pulse
// kata and the Regeneration owner compare.
struct SSpellControlData{int32_t pos[3]{};int32_t rangedamage[2]{};};
struct SSpellVariant{SSpellControlData*controldata{};int32_t mindamage{},maxdamage{};};
struct SSpellData{struct{int32_t intensity{},color[3]{},mult{},pos[3]{},fadein{},fadeout{};}light;int32_t damagetype{};};
struct TSpell{SSpellData data;SSpellData*SpellData(){return &data;}SSpellVariant*VariantData(){return nullptr;}
 TObjectInstance*PoisonTarget()const{return nullptr;}Ref GetInvokerRef()const{return {};}};
void ConvertToVector(int32_t,int32_t,S3DPoint&){}
bool ObjectMapOffset(TObjectInstance*,const char*,S3DPoint&){return false;}
void AreaDamage(TObjectInstance*,S3DPoint,int32_t,int32_t,int32_t,int32_t,int32_t){}
struct TEffect:TObjectInstance{TSpell*spell{};bool animator=true,done=true;Image*imagery{};unsigned flags=0x40004;TFlipbookBillboardComponent*visual{};int frame=7;
 int32_t startwait{},followtime{},orbitangle{},orbitdist{},duration{},blinktime{};bool orbit{},timed{};char attachname[0x32]{},visiblewhen[32]{};
 Ref follow;SEffectLightRamp lightramp;
 template<class T>T*GetComponent()const{return static_cast<T*>(visual);}bool HasAnimator()const{return animator;}
 bool CommandDone()const{return done;}void SetCommandDone(bool x){done=x;}void SetFrame(int x){frame=x;}int GetFrame()const{return frame;}
 int GetState()const{return 0;}const char*GetName()const{return "fixture";}
 unsigned GetFlags()const{return flags;}void SetFlags(unsigned x){flags|=x;}void ResetFlags(unsigned x){flags=x;}
 bool IsLight()const{return flags&OF_LIGHT;}void OnScreen(){}void OffScreen(){}void SetEffectFlags(uint32_t);
 void SetLightColor(SColor){}void SetLightIntensity(uint8_t){}void SetLightMultiplier(uint8_t){}void SetLightPos(S3DPoint){}
 void SetPos(S3DPoint,int,bool){}void Pulse();};
// PulseMissile is the combat wrapper introduced on main. This lifecycle
// boundary counts dispatch only; missile physics/damage have their own katas.
struct TFireBallEffect:TEffect{bool alive_=true;int steps{};void PulseMissile(){++steps;}void StepAnimate(){++steps;}void KillThisEffect(){flags|=OF_KILL;}void Pulse();};
'''
        tests=r'''
void check(bool condition,const char*name){if(!condition){std::fprintf(stderr,"FAIL %s\n",name);std::exit(1);}}
int main(){Image loop{1},once{0};TSpell spelled;TFlipbookBillboardComponent replacement{true},ordinary{false};int count=0;
for(int kind=0;kind<8;++kind){TEffect e;e.imagery=&once;bool killed=true;
 switch(kind){case 0:break;case 1:e.done=false;killed=false;break;case 2:e.imagery=&loop;killed=false;break;case 3:e.spell=&spelled;killed=false;break;case 4:e.visual=&replacement;killed=false;break;case 5:e.visual=&ordinary;break;case 6:e.animator=false;killed=false;break;case 7:e.imagery=nullptr;killed=false;break;}
 e.Pulse();check(bool(e.flags&OF_KILL)==killed,"generic/loop/spell/replacing visual");check((e.flags&0x40004)==0x40004,"map flags preserved");check(e.pulses==1&&!e.done,"base Pulse and command handshake");check(e.frame==7,"owner animation frame preserved");++count;}
for(const char*owner:{"Cure","Mist","Drip"}){TEffect e;e.imagery=&once;e.visual=&replacement;e.Pulse();check(!(e.flags&OF_KILL),owner);++count;}
for(const char*owner:{"Might","Immortalmight","Fmastery","Shadowfist","Warriorborn","Teleportation","WaterFlft","PunchAndJudy"}){TEffect e;e.imagery=&loop;e.Pulse();check(!(e.flags&OF_KILL)&&e.frame==7,owner);++count;}
for(const char*owner:{"GoldEffect","CombatFlash-state0","MPAppear","gvortex","Partsys nonloop"}){TEffect e;e.imagery=&once;e.Pulse();check((e.flags&OF_KILL)&&e.frame==7,owner);++count;}
TEffect story;Image changing{0};story.imagery=&changing;story.script=[&]{changing.flags=AF_LOOPING;};story.Pulse();check(!(story.flags&OF_KILL),"script state transition precedes cleanup");++count;
TFireBallEffect projectile;projectile.imagery=&once;projectile.Pulse();check(!(projectile.flags&OF_KILL)&&projectile.steps==2,"FireBall explicit original command handshake");++count;
projectile.alive_=false;projectile.Pulse();check(projectile.flags&OF_KILL,"FireBall terminal simulator requests kill");++count;
std::printf("PASS %d actual production lifecycle scenarios\n",count);}
'''
        bodies='\n'.join(function(source,signature)for signature in(
            'void TEffect::Pulse()','void TEffect::SetEffectFlags(uint32_t value)','void TFireBallEffect::Pulse()'))
        with tempfile.TemporaryDirectory(prefix='revenant-effect-lifecycle-')as tmp:
            cpp=Path(tmp)/'lifecycle.cpp';binary=Path(tmp)/'lifecycle';cpp.write_text(prelude+bodies+tests)
            result=subprocess.run([os.environ.get('CXX','clang++'),'-std=c++17','-iquote',str(ROOT/'src'),str(cpp),'-o',str(binary)],
                                  text=True,capture_output=True)
            self.assertEqual(result.returncode,0,result.stderr)
            run=subprocess.run([str(binary)],text=True,capture_output=True)
            self.assertEqual(run.returncode,0,run.stdout+run.stderr)
            self.assertIn('PASS 27 actual production lifecycle scenarios',run.stdout)


if __name__=='__main__':unittest.main()
