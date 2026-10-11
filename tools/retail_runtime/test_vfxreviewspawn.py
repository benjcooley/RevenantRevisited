"""Compile actual review admission/dispatch and real FireBall endpoint setter."""
import subprocess,tempfile,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]

STUB=r'''
#include <cassert>
#include <cstdint>
#include <cstring>
#include <strings.h>
#include <vector>
#define stricmp strcasecmp
#include "vfxreviewspawn.h"
struct S3DPoint{int x=0,y=0,z=0;};
constexpr int OBJCLASS_EFFECT=25;
struct TFlipbookBillboardComponent{};
struct TObjectImagery{virtual ~TObjectImagery()=default;};
struct Tag{int state=0;const char*name;};
struct T3DImagery:TObjectImagery{bool admitted=false;std::vector<Tag>tags;std::vector<const char*>names{"mesh"};std::vector<int>faces{2};
 int NumObjects(){return names.size();}const char*GetObjectName(int i){return names[i];}int NumObjFaces(int i){return faces[i];}
 int NumTags(){return tags.size();}Tag*GetTag(int i){return &tags[i];}
 bool HasGoldPartSysProfile(){return admitted;}bool HasCombatFlashStart1PartSysProfile(){return admitted;}
 bool HasRetailMightPartSysProfile(){return admitted;}bool HasRetailImmortalmightPartSysProfile(){return admitted;}
 bool HasRetailFmasteryPartSysProfile(){return admitted;}bool HasRetailSpeedFamilyPartSysProfile(uint32_t){return admitted;}
 bool HasRetailStaticParticleProfile(uint32_t){return admitted;}};
struct TObjectInstance{virtual~TObjectInstance()=default;uint32_t id=0;int cls=25;TObjectImagery*image=nullptr;bool component=false;S3DPoint position{};
 int ObjClass(){return cls;}uint32_t ObjId(){return id;}TObjectImagery*GetImagery(){return image;}void ForcePos(const S3DPoint&p){position=p;}
 template<class T>T*GetComponent(){static T c;return component?&c:nullptr;}};
struct TFireBallEffect:TObjectInstance{int state_=0;S3DPoint projectile_destination_{};bool has_projectile_destination_=false;
 bool SetProjectileEndpoints(const S3DPoint&,const S3DPoint&);};
struct TObjectBuilder{const char*name;const char*GetTypeName(){return name;}};
struct SObjectInfo{uint32_t uniqueid;TObjectBuilder*objbuilder;int imageryid=0;};
struct TObjectClass{bool present=true;std::vector<SObjectInfo>entries;static TObjectClass*catalog;
 static TObjectClass*GetClass(int){return catalog;}int FindObjType(uint32_t id){for(int i=0;i<int(entries.size());++i)if(entries[i].uniqueid==id)return i;return-1;}
 SObjectInfo*GetObjType(int i){return present?&entries[i]:nullptr;}};TObjectClass*TObjectClass::catalog=nullptr;
'''
MAIN=r'''
int main(){
 assert(!DescribeVfxReviewSpawn(1).safe_factory);TObjectClass c;TObjectClass::catalog=&c;
 TObjectBuilder generic{"EFFECT"},fire{"FireBall"},unknown{"Unknown"};
 c.entries={{1,&generic},{0x63fd382a,&fire},{0x63fd3827,&generic},{0x113803f4,&generic},{0xb1c4c90f,&generic},{0xad92bd1f,&generic},{2,&unknown},{3,nullptr},{4,&generic,-1},{0x10ac03de,&generic}};
 assert(!DescribeVfxReviewSpawn(42).safe_factory);assert(DescribeVfxReviewSpawn(1).safe_factory);
 assert(!DescribeVfxReviewSpawn(2).safe_factory);assert(!DescribeVfxReviewSpawn(3).safe_factory);assert(!DescribeVfxReviewSpawn(4).safe_factory);
 for(uint32_t id:{0x63fd3827u,0x113803f4u,0xb1c4c90fu,0xad92bd1fu}){auto d=DescribeVfxReviewSpawn(id);assert(d.projectile&&!d.safe_factory&&!d.supports_endpoints);}
 c.present=false;assert(!DescribeVfxReviewSpawn(1).safe_factory);c.present=true;
 TObjectInstance owner;owner.id=1;owner.position={100,200,16};S3DPoint source{80,180,60},destination{180,280,60};
 auto d=ConfigureVfxReviewSpawn(owner,source,destination);assert(d.safe_factory&&!d.renderer_supported&&!d.uses_effect_runtime);assert(owner.position.x==100);
 owner.cls=3;assert(!ConfigureVfxReviewSpawn(owner,source,destination).safe_factory);owner.cls=25;
 owner.id=0x63fd382a;assert(!ConfigureVfxReviewSpawn(owner,source,destination).safe_factory);owner.id=1;
 T3DImagery image;owner.image=&image;d=ConfigureVfxReviewSpawn(owner,source,destination);assert(d.renderer_supported&&!d.uses_effect_runtime);
 image.tags={{0,"partsys"}};assert(!ConfigureVfxReviewSpawn(owner,source,destination).renderer_supported);
 owner.id=0x10ac03de;image.admitted=true;d=ConfigureVfxReviewSpawn(owner,source,destination);assert(d.renderer_supported&&d.uses_effect_runtime);
 image.admitted=false;image.tags={{0,"unknowncont"},{0,"play"},{2,"play"}};owner.id=1;d=ConfigureVfxReviewSpawn(owner,source,destination);assert(!d.renderer_supported&&d.authored_sound_tags==2);
 image.tags={{2,"unknowncont"}};assert(ConfigureVfxReviewSpawn(owner,source,destination).renderer_supported);
 image.tags.clear();image.names={"*hidden"};assert(!ConfigureVfxReviewSpawn(owner,source,destination).renderer_supported);
 image.names={"mesh"};image.faces={0};assert(!ConfigureVfxReviewSpawn(owner,source,destination).renderer_supported);
 owner.component=true;d=ConfigureVfxReviewSpawn(owner,source,destination);assert(d.renderer_supported&&d.uses_effect_runtime);
 TFireBallEffect ball;ball.id=0x63fd382a;ball.image=&image;auto ready=DescribeVfxReviewSpawn(ball.id);assert(ready.safe_factory&&ready.projectile&&ready.supports_endpoints);
 d=ConfigureVfxReviewSpawn(ball,source,destination);assert(d.supports_endpoints&&ball.has_projectile_destination_&&ball.position.x==source.x&&ball.projectile_destination_.x==destination.x);
 ball.state_=1;assert(!ConfigureVfxReviewSpawn(ball,source,destination).supports_endpoints);
 ball.state_=0;assert(!ConfigureVfxReviewSpawn(ball,source,source).supports_endpoints);
}
'''

def function(text,signature):
 a=text.index(signature);b=text.index('{',a);depth=1;end=b+1
 while depth:depth+=(text[end]=='{')-(text[end]=='}');end+=1
 return text[a:end]

class ReviewSpawnTests(unittest.TestCase):
 def test_actual_admission_and_dispatch(self):
  source=(ROOT/'src/vfxreviewspawn.cpp').read_text();source='\n'.join(l for l in source.splitlines()if not l.startswith('#include'))
  endpoint=function((ROOT/'src/effect.cpp').read_text(),'bool TFireBallEffect::SetProjectileEndpoints(')
  with tempfile.TemporaryDirectory(prefix='vfxreviewspawn-')as tmp:
   p=Path(tmp);driver=p/'contract.cpp';driver.write_text(STUB+'\n'+endpoint+'\n'+source+'\n'+MAIN);binary=p/'contract'
   q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-iquote',str(ROOT/'src'),str(driver),'-o',str(binary)],capture_output=True,text=True)
   self.assertEqual(q.returncode,0,q.stdout+q.stderr);q=subprocess.run([str(binary)],capture_output=True,text=True);self.assertEqual(q.returncode,0,q.stdout+q.stderr);self.assertEqual(q.stderr,'')
if __name__=='__main__':unittest.main()
