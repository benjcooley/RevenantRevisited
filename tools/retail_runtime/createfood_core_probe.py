"""Compile Createfood production state, explicit lookup and full matrices versus original."""
import argparse,json,struct,subprocess
from pathlib import Path
from character_native_preflight import ROOT,sha

def run(native,output):
 output.mkdir(parents=True,exist_ok=False);driver=output/'core.cpp'
 driver.write_text(r'''#include "createfoodcore.h"
#include "retailsoftwaretransform.h"
#include <cstdio>
#include <cstring>
int main(){retail_createfood::Lookup lookup;printf("L");for(auto w:lookup.words)printf(" %d",w);puts("");retail_createfood::State s;uint32_t rng=1;int count=0;auto random=[&](int lo,int hi){if(lo==hi)return lo;rng=rng*214013u+2531011u;++count;return lo+int((rng>>16)&0x7fff)%(hi-lo+1);};for(int tick=1;tick<=85;++tick){s.Pulse(random,lookup,[&](int){auto draw=[&](){rng=rng*214013u+2531011u;++count;return (rng>>16)&0x7fff;};auto index=retail_createfood::EmptyMapFoodIndex(draw,[&](uint32_t seed){rng=seed;});assert(index==1297753000u);});printf("S %d %d %d %.9g %u %d %d %d ",tick,s.ticks,s.owner_counter,s.glow,rng,count,int(s.alive),s.food_type);for(auto b:std::array<unsigned char,0>{}){}const auto*b=reinterpret_cast<const unsigned char*>(s.particles.data());for(size_t i=0;i<sizeof(s.particles);++i)printf("%02x",b[i]);puts("");hmm_mat4 owner;BuildRetailSoftwareOwner(owner,3185,3185,16,0);s.Render(lookup,[&](int obj,const hmm_mat4&local){hmm_mat4 m;MtxMultiply(&m,const_cast<hmm_mat4*>(&local),&owner);printf("D %d %d",tick,obj);for(int i=0;i<16;++i)printf(" %.9g",(&m.Elements[0][0])[i]);puts("");});}}''')
 binary=output/'core';subprocess.run(['clang++','-std=c++20','-fsanitize=address,undefined','-fno-omit-frame-pointer','-iquote',str(ROOT/'src'),'-I',str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(binary)],check=True)
 text=subprocess.check_output([str(binary)],text=True);(output/'source.txt').write_text(text);states={};draws={};errors=[]
 n=json.loads(native.read_text());table=[]
 for line in text.splitlines():
  v=line.split()
  if v[0]=='L':table=list(map(int,v[1:]));continue
  tick=int(v[1])
  if v[0]=='S':states[tick]=v[2:]
  else:draws.setdefault(tick,[]).append((int(v[2]),list(map(float,v[3:]))))
 expected=list(struct.unpack('<1024h',bytes.fromhex(n['initial']['lookup_hex'])))
 for i,(a,b)in enumerate(zip(table,expected)):
  if a!=b:errors.append(['lookup',i,a,b])
 max_float=0;matrix_checks=0;max_matrix=0;pool_checks=0
 for r in n['records']:
  tick=r['tick'];a=states[tick];s=r['state'];want=[s['life'],s['owner_counter'],s['scale'],s['seed'],s['rng_draws'],int(not s['flags']&4096)]
  for i,(x,y)in enumerate(zip(a,want)):
   if abs(float(x)-y)>1e-6:errors.append([tick,'state',i,x,y])
  aa=bytes.fromhex(a[-1]);bb=bytes.fromhex(s['pool_hex']);assert len(aa)==len(bb)==12000
  for i in range(3000):
   fmt='<f'if i%20==13 else'<i';x=struct.unpack_from(fmt,aa,i*4)[0];y=struct.unpack_from(fmt,bb,i*4)[0];pool_checks+=1
   if fmt=='<f':max_float=max(max_float,abs(x-y))
   if abs(x-y)>1e-6 and len(errors)<100:errors.append([tick,'pool',i//20,i%20,x,y])
  actual=draws.get(tick,[])
  if len(actual)!=len(r['draws']):errors.append([tick,'draw_count',len(actual),len(r['draws'])]);continue
  for (obj,m),d in zip(actual,r['draws']):
   if obj!=d['object']:errors.append([tick,'object',obj,d['object']])
   err=max(abs(x-y)for x,y in zip(m,d['matrix']));max_matrix=max(max_matrix,err);matrix_checks+=16
   if err>4e-4 and len(errors)<100:errors.append([tick,'matrix',err])
 report=dict(status='pass'if not errors else'fail',native_sha256=sha(native),core_sha256=sha(ROOT/'src/game/effects/createfoodcore.h'),lookup_words=1024,pool_fields=pool_checks,max_float_error=max_float,matrix_channels=matrix_checks,max_matrix_error=max_matrix,errors=errors);(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'errors':errors[:15]}));assert not errors
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.output)
