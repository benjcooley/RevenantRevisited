#!/usr/bin/env python3
"""Compile actual bounded Strike/TEffect methods and replay a genuine owner trace.

External actor/map/light calls are explicit fixtures; this is not a renderer or
whole-engine proof. No native per-tick state is fed into the source simulation.
"""
import argparse,hashlib,json,os,subprocess,tempfile
from pathlib import Path
from test_effect_lifecycle import function
ROOT=Path(__file__).resolve().parents[2]
PRELUDE=r'''
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <strings.h>
#include "effectownercore.h"
#define stricmp strcasecmp
#define log_debug(...) ((void)0)
constexpr unsigned OF_KILL=0x1000,OF_PULSE=0x8000,OF_LIGHT=4,AF_LOOPING=1;
constexpr int N_DELETING=3,OBJCLASS_EFFECT=5,SPAT_NONE=0;
#define NOTIFY_DELETED(ptr,obj) (notify==1 && ptr==obj)
bool Editor=false,RealTimeLight=true;int random_calls=0;int random(int a,int b){++random_calls;if(a||b)std::abort();return 0;}
struct S3DPoint{int x=0,y=0,z=0;};struct SColor{int red{},green{},blue{};};
struct Control{const char*name="Strike";int flags=0,radius=0,hits=1,duration=2880,duration2=24,wait=0,pattern=0,shake=0,repeatdamage=0,rangedamage[2]={};char attach[1]={};void*attachmultiple=nullptr;char sound[1]={};int imagery=-1,hittarget=0,oncaster=0,attachset=0,follow=1,posset=1,repeatset=0,multipletargets=0,pos[3]={};};
struct Data{struct{int intensity=160,fadein=5,fadeout=5,color[3]={255,255,255},mult=1,pos[3]={};}light;int poisonchance=0;};
struct Variant{const char*name="Regeneration";const char*effect="Regeneration";Control*controldata=nullptr;bool facing=false;};
struct TSpell;
struct TObjectInstance{virtual~TObjectInstance()=default;void Notify(int,void*){}S3DPoint pos{3185,3185,16};unsigned flags=0x4c001;bool resident=true;int pulses=0,moves=0,screen_calls=0,light_value=0;void*GetSector(){return resident?this:nullptr;}virtual void Pulse(){++pulses;}void Move(){++moves;}S3DPoint Pos(){return pos;}int GetLevel(){return 0;}int GetFace(){return 0;}void SetPos(S3DPoint p,int,bool){pos=p;}void SetNotify(int){}unsigned Flags(){return flags;}void ResetFlags(unsigned f){flags=f;}bool IsLight(){return flags&4;}void SetLightColor(SColor){}void SetLightMultiplier(uint8_t){}void SetLightPos(S3DPoint&){}void SetLightIntensity(uint8_t v){light_value=v;}void OnScreen(){++screen_calls;flags|=8;}void OffScreen(){} };
struct TCharacter:TObjectInstance{TObjectInstance*target=nullptr;TObjectInstance*Fighting(){return target;}};
struct Ref{TObjectInstance*p=nullptr;Ref&operator=(TObjectInstance*x){p=x;return *this;}TObjectInstance*Get(){return p;}void Clear(){p=nullptr;}operator bool()const{return p;}};
struct TFlipbookBillboardComponent{bool ReplacesDefaultVisual()const{return false;}};struct Image{int GetAniFlags(int){return AF_LOOPING;}};
struct TEffect:TObjectInstance{retail_effect_owner::State retail_owner;Ref retail_follow;TSpell*spell=nullptr;Image image;Image*imagery=&image;bool done=false;void SetSpell(TSpell*s){spell=s;retail_owner.ResetLight();}void ConfigureRetailSpellOwner(TObjectInstance*,int32_t,int32_t,int32_t);void SetRetailOwnerFlags(uint32_t);void KillRetailSpellOwner();void Notify(int32_t,void*);bool BeginRetailOwnerPulse();void AdvanceRetailOwnerPulse();void Pulse()override;template<class T>T*GetComponent(){return nullptr;}bool HasAnimator(){return true;}bool CommandDone(){return done;}void SetCommandDone(bool x){done=x;}int GetState(){return 0;}const char*GetName(){return "Regeneration";}void SetFlags(unsigned x){flags|=x;}unsigned GetFlags(){return flags;}};
struct TSpell{Data*spell;Variant*variant;Ref effect,invoker_ref,target_refs[1];TObjectInstance*targets[1]={};int timer=-1,wait=5,frame=0,base_timer=0,base_kill=0;Data*SpellData(){return spell;}Variant*VariantData(){return variant;}bool Timer(){++base_timer;return false;}void Kill(){++base_kill;} };
struct TStrikeSpell:TSpell{TObjectInstance*striketarget=nullptr;bool HasRetailRegenerationControl()const;void Pulse();bool Timer();void Kill();};
struct SObjectDef{int objclass=0,level=0,facing=0,objtype=0;S3DPoint pos;};
struct{int FindObjType(const char*s){if(strcasecmp(s,"Regeneration"))std::abort();return 0;}}EffectClass;
struct Map{TEffect*owner=nullptr;int creates=0;int GetMapLevel(){return 0;}int NewObject(SObjectDef*d){++creates;owner=new TEffect;owner->pos=d->pos;return 1;}TObjectInstance*GetInstance(int){return owner;}~Map(){delete owner;}}MapPane;
'''
MAIN=r'''
int main(){Control c;Data data;Variant variant;variant.controldata=&c;TCharacter actor,target;actor.target=&target;TStrikeSpell s;s.spell=&data;s.variant=&variant;s.invoker_ref=&actor;
if(!s.HasRetailRegenerationControl())return 2;
// Every unsupported branch independently declines. No borrowed timer behavior.
int* rejected[]={&c.flags,&c.radius,&c.pattern,&c.shake,&c.repeatdamage,&c.rangedamage[0],&c.rangedamage[1],&c.hittarget,&c.oncaster,&c.attachset,&c.repeatset,&c.multipletargets,&c.pos[0],&c.pos[1],&c.pos[2],&data.poisonchance};
for(auto*p:rejected){*p=1;if(s.HasRetailRegenerationControl())return 3;*p=0;}
variant.name="Speed";s.Pulse();s.Timer();s.Kill();if(s.frame||s.base_timer!=1||s.base_kill!=1||MapPane.creates)return 4;variant.name="Regeneration";
for(int tick=0;tick<2895;++tick){s.Pulse();bool done=s.Timer();if(tick==8)actor.pos={3300,3190,24};TEffect*e=dynamic_cast<TEffect*>(s.effect.Get());if(e)e->Pulse();
if(!e){std::printf("%d %d 0\n",tick,done);continue;}
auto&o=e->retail_owner;
std::printf("%d %d 1 %u %d %d %d %d %d %d %d %d %d %d\n",tick,done,e->flags,e->pos.x,e->pos.y,e->pos.z,o.follow_remaining,o.remaining,o.delay,o.light_in,o.light_out,o.light_value,o.light_peak);
}
if(MapPane.creates!=1||random_calls!=1||s.frame!=2895||s.wait!=-1||!(MapPane.owner->flags&OF_KILL)||MapPane.owner->spell||MapPane.owner->retail_owner.light_value)return 5;
// Resident vs nonresident scheduling, selected source methods themselves.
TStrikeSpell isolated;isolated.spell=&data;isolated.variant=&variant;TEffect owner;owner.resident=false;isolated.effect=&owner;isolated.Pulse();if(owner.pulses!=1||owner.moves!=1)return 6;
TEffect deletion;deletion.ConfigureRetailSpellOwner(&actor,2880,24,0);deletion.Notify(1,&target);if(deletion.flags&OF_KILL)return 7;deletion.Notify(1,&actor);if(!(deletion.flags&OF_KILL)||deletion.retail_follow.Get())return 8;
return 0;}
'''
def main():
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--branches',type=Path);a=p.parse_args()
 inputs={name:hashlib.sha256((ROOT/name).read_bytes()).hexdigest() for name in ('src/effect.cpp','src/effect.h','src/effectownercore.h','src/spell.cpp','src/spell.h')}
 effect=(ROOT/'src/effect.cpp').read_text();spell=(ROOT/'src/spell.cpp').read_text()
 bodies='\n'.join(function(effect,s)for s in ['void TEffect::ConfigureRetailSpellOwner(', 'void TEffect::SetRetailOwnerFlags(', 'void TEffect::KillRetailSpellOwner()', 'void TEffect::Notify(', 'bool TEffect::BeginRetailOwnerPulse()', 'void TEffect::AdvanceRetailOwnerPulse()', 'void TEffect::Pulse()'])
 bodies+='\n'+'\n'.join(function(spell,s)for s in ['bool TStrikeSpell::HasRetailRegenerationControl() const','void TStrikeSpell::Pulse()','bool TStrikeSpell::Timer()','void TStrikeSpell::Kill()'])
 branch_code=''
 if a.branches:
  for i,row in enumerate(json.loads(a.branches.read_text())['rows']):
   before=row['before'];after=row['after'];fields=before['fields']
   bindings={'follow_remaining':'0xe8','remaining':'0x12c','timed':'0x130','blink':'0x134','delay':'0x138','light_in':'0x13c','light_out':'0x140','light_value':'0x144','light_peak':'0x148'}
   assignments=''.join(f'owner.retail_owner.{member}={fields[offset]};'for member,offset in bindings.items())
   expected=[int(after['flags'],16),*[after['fields'][k]for k in ['0xe8','0x12c','0x138','0x13c','0x140','0x144','0x148']]]
   boundary="if(j==0 && (actual[j]^expected[j])==0x8000)continue;" if row['name']=='lights_on' else ''
   branch_code+=f"{{TEffect owner;owner.spell=&s;owner.retail_follow=&actor;owner.retail_owner.enabled=true;owner.flags={int(before['flags'],16)}u;{assignments}RealTimeLight={'true'if row['real_time_light']else'false'};owner.Pulse();auto&o=owner.retail_owner;int actual[]={{int(owner.flags),o.follow_remaining,o.remaining,o.delay,o.light_in,o.light_out,o.light_value,o.light_peak}};int expected[]={{{','.join(map(str,expected))}}};for(int j=0;j<8;++j)if(actual[j]!=expected[j]){{{boundary}std::fprintf(stderr,\"branch {row['name']} field%d got%d expected%d\\n\",j,actual[j],expected[j]);return {20+i};}}}}"
 main_source=MAIN.replace('return 0;}','RealTimeLight=true;'+branch_code+'return 0;}')
 with tempfile.TemporaryDirectory(prefix='generic-owner-source-')as d:
  cpp=Path(d)/'probe.cpp';binary=Path(d)/'probe';cpp.write_text(PRELUDE+bodies+main_source)
  subprocess.run([os.environ.get('CXX','clang++'),'-std=c++17','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(cpp),'-o',str(binary)],check=True)
  run=subprocess.run([str(binary)],text=True,capture_output=True);assert run.returncode==0,run.stderr
 lines=[[int(x)for x in line.split()]for line in run.stdout.splitlines()];native=json.loads(a.native.read_text());assert len(lines)==len(native['rows'])
 fields=['0xe8','0x12c','0x138','0x13c','0x140','0x144','0x148'];count=0
 for actual,row in zip(lines,native['rows']):
  assert actual[:3]==[row['tick'],row['done'],int(bool(row['after']))],(actual,row['tick'])
  if row['after']:
   n=row['after'][0];expected=[int(n['flags'],16),*n['position'],*[n['fields'][k]for k in fields]]
   assert actual[3:]==expected,(row['tick'],actual[3:],expected)
   count+=len(expected)
 assert all(hashlib.sha256((ROOT/name).read_bytes()).hexdigest()==sha for name,sha in inputs.items()),'source changed during proof'
 a.output.write_text(json.dumps(dict(status='pass',source_sha256=inputs,ticks=len(lines),owner_fields=count,branch_controls=7 if a.branches else 0,known_boundary=('Artificially clearing OF_PULSE before light reactivation: original animator creation restores it, source initializer does not; selected natural lifetime retains OF_PULSE throughout.' if a.branches else None),scope='Actual source Strike Timer/Pulse/Kill and TEffect Pulse replay; stubbed actor/map/scene boundaries, OnScreen models documented animator MOVING flag; not full engine or raster proof.'),indent=2))
 print('PASS',len(lines),'ticks',count,'owner fields')
if __name__=='__main__':main()
