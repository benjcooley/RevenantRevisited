#!/usr/bin/env python3
"""Bounded recursive Ripple: actual native splash RNG, children and authored draws."""
import argparse,json,struct,subprocess,zipfile,statistics,time
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_EIP,UC_X86_REG_ESP
from ripple_probe import RippleFixture,ASSET_SHA,INDICES
from drip_ripple_probe import build_port as build_scaffold,production_spans,ROOT
from software_probe import SoftwareFixture,save_rgb565_png,RETAIL_SHA
from fizzle_probe import sha,f32
PROFILES={'length64_origin':(64,(0,0,0)),'length96_xy_positive_negative':(96,(37,-29,0)),'length64_xy_negative_positive':(64,(-37,29,0))}
TICKS=120;WIDTH=512;HEIGHT=512


class RippleFamily(RippleFixture):
    def __init__(self,image,asset):
        super().__init__(image,asset);self.asset=asset;self.active=None;self.tick=0;self.random=[];self.pending=[];self.births=[];self.deaths=[];self.packets=[];self.actors={}
        self.vm.uc.hook_add(UC_HOOK_CODE,self.child,begin=0x4f07e0,end=0x4f07e0)
        for a in(0x483300,0x483312,0x48332c,0x58c5a3):self.vm.uc.hook_add(UC_HOOK_CODE,self.observe_random,begin=a,end=a)
        self.software.checkpoint()
        self.raster=SoftwareFixture(image,WIDTH,HEIGHT);self.raster.set_texture(256,256,asset[0x4f0:0x4f0+131072],format='ARGB4444')
    def reset(self,length,origin):
        self.software.restore();self.active=None;self.tick=0;self.random=[];self.pending=[];self.births=[];self.deaths=[];self.packets=[];self.actors={};self.create(length,origin,None)
    def create(self,length,origin,parent):
        ident=len(self.actors);a=dict(id=ident,length=length,origin=list(origin),parent=parent,children=[],anim=self.vm.allocate(0x200),owner=self.vm.allocate(0x400),objects=[],alive=True)
        self.actors[ident]=a;self.vm.put_u32(a['anim'],0x5ac5b0);self.vm.put_u32(a['anim']+4,a['owner']);self.vm.put_u32(a['anim']+8,self.vm.allocate(0x200));self.vm.put_u32(a['owner'],self.vm.u32(self.owner));self.vm.put_u32(a['owner']+0x300,length);self.vm.write(a['owner']+0x10,struct.pack('<3i',*origin))
        for offset in(0x144,0x1c4):
            obj=self.vm.allocate(0x200);verts=self.vm.allocate(128);self.vm.put_u32(obj+0xa0,4);self.vm.put_u32(obj+0xa4,verts)
            points=[]
            for i in range(4):
                v=struct.unpack_from('<8f',self.asset,offset+i*32);points.append(v[:3]);self.vm.write(verts+i*32,struct.pack('<3f3I2f',*v[:3],0,0xffffffff,0,*v[6:]))
            a['objects'].append(dict(obj=obj,verts=verts,points=points))
        previous=self.active;self.active=a;self.vm.call(0x4f0780,this=a['anim']);self.active=previous
        if parent is not None:self.births.append(dict(tick=self.tick,id=ident,parent=parent,arguments=[*origin,length]));self.actors[parent]['children'].append(ident)
        return ident
    def observe_random(self,uc,address,size,user):
        if address==0x483300:
            sp=uc.reg_read(UC_X86_REG_ESP);lo,hi=struct.unpack('<2i',self.vm.uc.mem_read(sp+4,8));self.pending.append([lo,hi,-1,self.active['id'],self.vm.u32(sp)])
        elif address==0x58c5a3:self.pending[-1][2]=uc.reg_read(UC_X86_REG_EAX)
        else:
            lo,hi,raw,ident,site=self.pending.pop()
            if site not in(0x4f09cd,0x4f0a0d,0x4f0a34,0x4f0a67):raise AssertionError(f'Unexpected Ripple random site {site:x}')
            value=struct.unpack('<i',struct.pack('<I',uc.reg_read(UC_X86_REG_EAX)))[0];self.random.append((lo,hi,value,raw,ident,self.tick))
    def child(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP);arguments=struct.unpack('<4i',self.vm.uc.mem_read(sp+4,16));parent=self.active['id']
        # Do not recursively enter Unicorn from a hook. Creation occurs after
        # the parent's actual Animate returns, retaining request order/age0.
        self.requests.append((parent,arguments));uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+20)
    def external(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP)
        if address==0x40eef0:
            index=self.vm.u32(sp+4)
            if index not in(0,1):raise AssertionError('Unexpected Ripple authored object')
            uc.reg_write(UC_X86_REG_EAX,self.active['objects'][index]['obj'])
        elif address==0x40a8f0:
            obj=self.vm.u32(sp+4);matches=[x for x in self.active['objects']if x['obj']==obj]
            if len(matches)!=1:raise AssertionError('Wrong native Ripple object')
            entry=matches[0];self.packets.append(dict(id=self.active['id'],origin=self.active['origin'],matrix=bytes(self.vm.uc.mem_read(obj+0x58,64)),points=entry['points'],uvs=[list(struct.unpack('<2f',self.vm.uc.mem_read(entry['verts']+i*32+24,8)))for i in range(4)]))
        elif address==0x417d60:
            if (self.vm.u32(sp+4),self.vm.u32(sp+8))!=(2,1):raise AssertionError('Ripple blend changed')
        uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+self.boundaries[address])
    def step_actor(self,ident):
        a=self.actors[ident]
        for child in a['children'].copy():self.step_actor(child)
        self.active=a;self.requests=[]
        if a['alive']:self.vm.call(0x4f08b0,this=a['anim'])
        if a['alive']and self.vm.u32(a['owner']+8)&0x1000:
            a['alive']=False
            if a['parent']is not None:self.deaths.append(dict(tick=self.tick,id=ident));self.actors[a['parent']]['children'].remove(ident)
        for parent,args in self.requests.copy():self.create(args[3],args[:3],parent)
    def step(self):self.tick+=1;self.step_actor(0)
    def state(self):
        records=[]
        def visit(ident):
            a=self.actors[ident];n=self.vm.u32(a['anim']+0x108);ptr=self.vm.u32(a['anim']+0x114)
            if n not in(0,2,3):raise AssertionError('Unexpected native splash cardinality')
            records.append(dict(id=ident,length=a['length'],origin=a['origin'],frameon=self.vm.u32(a['anim']+0x104),ripframe=self.vm.u32(a['anim']+0x100),alive=a['alive'],drops=[list(struct.unpack('<6fI',self.vm.uc.mem_read(ptr+28*i,28)))for i in range(n)]))
            for child in a['children']:visit(child)
        visit(0);return dict(actors=records,births=self.births.copy(),deaths=self.deaths.copy(),random_count=len(self.random))
    def draws(self):
        self.packets=[]
        def visit(ident):
            a=self.actors[ident];self.active=a
            if a['alive']:self.vm.call(0x4f0c10,this=a['anim'])
            for child in a['children']:visit(child)
        visit(0);out=[]
        for packet in self.packets:
            self.vm.write(self.obj+0x58,packet['matrix']);points=[]
            for xyz in packet['points']:
                self.vm.write(self.point,struct.pack('<3f',*xyz));self.vm.call(0x43ad80,(self.obj+0x58,self.point,self.result));local=struct.unpack('<3f',self.vm.uc.mem_read(self.result,12));points.append([struct.unpack('<f',struct.pack('<f',x+y))[0]for x,y in zip(local,packet['origin'])])
            out.append(dict(positions=points,uvs=packet['uvs']))
        return out
    def pixels(self,draws):
        s=self.raster;s.clear()
        for d in draws:
            points=s.project(d['positions'],camera=(0,0,0),zdist=1925)
            if len(points)!=4 or len(d['uvs'])!=4:raise AssertionError('Quad cardinality')
            if any(not(0<=p[0]<WIDTH and 0<=p[1]<HEIGHT)for p in points):raise AssertionError('Authored geometry outside declared viewport')
            s.draw([(*p,31,31,31,31,*uv)for p,uv in zip(points,d['uvs'])],INDICES,z_enabled=True,z_write=False,instruction_limit=5000000)
        return s.vm.surface_bytes('screen'),s.vm.surface_bytes('depth')


def build_family(output,asset,inputs,length,origin):
    # Reuse the shipped-asset/base-object boundary already tested by the linked
    # probe. This first scaffold run is separate; the final native-RNG driver is
    # compiled again and never uses its Drip trace as Ripple input.
    with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:drip=z.read('Imagery/Magic/drip.i3d')
    warm=output/'scaffold-rng.txt';warm.write_text('0 0 0 -1\n'*5)
    _,scaffold=build_scaffold(output,drip,asset,warm);generated=output/'drip-ripple-production.cpp';code=generated.read_text()
    old='extern "C" int rand(void){throw std::runtime_error("Unexpected raw RNG: native short no-splash physics requires none");}'
    code=code.replace(old,'int current_raw=-1;bool raw_consumed=false;extern "C" int rand(void){if(current_raw<0||raw_consumed)throw std::runtime_error("Unobserved raw Ripple draw");raw_consumed=true;return current_raw;}')
    old='int WaterRandom(int low,int high){int a,b,value,raw;if(!(rng>>a>>b>>value>>raw)||a!=low||b!=high||raw!=-1)throw std::runtime_error("Linked physics RNG contract changed");int got=production_random::WaterRandom(low,high);if(got!=value)throw std::runtime_error("Linked RNG result differs");++rng_count;return got;}'
    new='int WaterRandom(int low,int high){int a,b,value,raw;if(!(rng>>a>>b>>value>>raw)||a!=low||b!=high)throw std::runtime_error("Ripple RNG order differs");current_raw=raw;raw_consumed=false;int got=production_random::WaterRandom(low,high);if(got!=value||raw_consumed!=(raw>=0))throw std::runtime_error("Ripple RNG value/consumption differs");++rng_count;return got;}'
    if code.count(old)!=1:raise AssertionError('Scaffold RNG boundary changed')
    code=code.replace(old,new);start=code.index('int main(int argc,char**argv)');trailer=code[start:];asset_prefix=trailer[trailer.index('S3DVertex head[4]='):trailer.index(' TDripEffect parent(nullptr);')]
    program=r'''
#include <functional>
#include <set>
struct Birth{int tick,id,parent,length,x,y,z;};std::vector<Birth>allbirths;std::set<int>seen;
int main(int argc,char**argv){rng.open(argv[1]);if(!rng)return 2;ASSETS
 TRippleEffect root(nullptr);root.SetLength(LENGTH);root.ForcePos(S3DPoint{OX,OY,OZ});root.ring_vertices_.assign(ring,ring+4);root.splash_vertices_.assign(splash,splash+4);root.ring_texture_=2;root.splash_texture_=2;
 for(now=0;now<=120;++now){if(now)root.Advance(1.0/24.0);renderer.draws.clear();root.Submit(EFxDebugMode::Normal);printf("F %d %d\n",now,rng_count);
  std::function<void(TRippleEffect&,int)>visit=[&](TRippleEffect&e,int parent){int id=e.GetMapIndex();const auto&p=e.Pos();if(id&&seen.insert(id).second)allbirths.push_back({now,id,parent,e.GetLength(),p.x,p.y,p.z});
   printf("A %d %d %d %d %d %d %d %d %d %zu",now,id,e.GetLength(),p.x,p.y,p.z,e.frameon_,e.ripframe_,int(e.IsAlive()),e.splash_drops_.size());for(const auto&d:e.splash_drops_)printf(" %.9g %.9g %.9g %.9g %.9g %.9g %d",d.pos.X,d.pos.Y,d.pos.Z,d.vel.X,d.vel.Y,d.vel.Z,int(d.dead));puts("");for(auto&c:e.spawned_ripples_)visit(*c,id);};visit(root,-1);
  for(auto&d:renderer.draws){printf("Q %d",now);for(int i=0;i<4;++i)printf(" %.9g %.9g %.9g %.9g %.9g",d.world_pos[i][0],d.world_pos[i][1],d.world_pos[i][2],d.uv[i][0],d.uv[i][1]);puts("");}}
 for(auto&b:allbirths)printf("B %d %d %d %d %d %d %d\n",b.tick,b.id,b.parent,b.x,b.y,b.z,b.length);for(auto&d:deaths)printf("D %d %d\n",d.tick,d.id);int a,b,c,d;if(rng>>a>>b>>c>>d)throw std::runtime_error("Unused Ripple native RNG");}
'''.replace('ASSETS',asset_prefix).replace('LENGTH',str(length)).replace('OX',str(origin[0])).replace('OY',str(origin[1])).replace('OZ',str(origin[2]))
    generated=output/'ripple-family-production.cpp';generated.write_text(code[:start]+program);binary=output/'ripple-family-production';cmd=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(generated),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    r=subprocess.run(cmd,capture_output=True,text=True);(output/'family-compile.log').write_text(r.stdout+r.stderr)
    if r.returncode:raise RuntimeError('Ripple family compile failed')
    r=subprocess.run([str(binary),str(inputs)],capture_output=True,text=True);(output/'family-port-trace.txt').write_text(r.stdout);(output/'family-port-run.log').write_text(r.stderr)
    if r.returncode:raise RuntimeError('Ripple family native RNG mismatch')
    rows={};births=[];deaths=[]
    for line in r.stdout.splitlines():
        p=line.split();tick=int(p[1])
        if p[0]=='F':rows[tick]=dict(actors=[],births=[],deaths=[],random_count=int(p[2]),draws=[])
        elif p[0]=='A':
            n=int(p[10]);values=p[11:]
            if len(values)!=n*7:raise AssertionError('Production splash record cardinality')
            rows[tick]['actors'].append(dict(id=int(p[2]),length=int(p[3]),origin=list(map(int,p[4:7])),frameon=int(p[7]),ripframe=int(p[8]),alive=bool(int(p[9])),drops=[[float(x)for x in values[i*7:i*7+6]]+[int(values[i*7+6])]for i in range(n)]))
        elif p[0]=='Q':
            if len(p)!=22:raise AssertionError('Production draw cardinality')
            rows[tick]['draws'].append(dict(positions=[list(map(float,p[2+i*5:5+i*5]))for i in range(4)],uvs=[list(map(float,p[5+i*5:7+i*5]))for i in range(4)]))
        elif p[0]=='B':births.append(dict(tick=tick,id=int(p[2]),parent=int(p[3]),arguments=list(map(int,p[4:8]))))
        elif p[0]=='D':deaths.append(dict(tick=tick,id=int(p[2])))
    for tick,row in rows.items():row.update(births=[x for x in births if x['tick']<=tick],deaths=[x for x in deaths if x['tick']<=tick])
    return rows,dict(command=cmd,generated_source_sha256=sha(generated.read_bytes()),binary_sha256=sha(binary.read_bytes()),scaffold=scaffold)


def compare_state(native,port):
    errors=[]
    for key in('births','random_count'):
        if native[key]!=port[key]:errors.append(key)
    # Native observer sees kill flags; production sees unique_ptr destruction.
    # Removal identity/tick is the contract, not incidental erase compaction order.
    if sorted(native['deaths'],key=lambda d:(d['tick'],d['id']))!=sorted(port['deaths'],key=lambda d:(d['tick'],d['id'])):errors.append('deaths')
    if len(native['actors'])!=len(port['actors']):return errors+['actor cardinality']
    for a,b in zip(native['actors'],port['actors']):
        for key in('id','length','origin','frameon','ripframe','alive'):
            if a[key]!=b[key]:errors.append(f"actor{a['id']}.{key}")
        if len(a['drops'])!=len(b['drops']):errors.append(f"actor{a['id']}.drop cardinality");continue
        for i,(d,e)in enumerate(zip(a['drops'],b['drops'])):
            if len(d)!=7 or len(e)!=7:raise AssertionError('Splash record cardinality')
            if any(f32(x)!=f32(y)for x,y in zip(d[:6],e[:6]))or d[6]!=e[6]:errors.append(f"actor{a['id']}.drop{i}")
    return errors


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);p.add_argument('--state-only',action='store_true');args=p.parse_args();args.output.mkdir(parents=True,exist_ok=True)
    before={k:sha(v.encode())for k,v in production_spans().items()}
    with zipfile.ZipFile(args.archive)as z:asset=z.read('Imagery/Magic/ripples.i3d')
    if sha(asset)!=ASSET_SHA:raise ValueError('Wrong Ripple asset')
    fixture=RippleFamily(args.executable,asset);profiles=[];allfailures=[]
    for name,(length,origin)in PROFILES.items():
        output=args.output/name;output.mkdir(exist_ok=True);fixture.reset(length,origin)
        for _ in range(TICKS):fixture.step()
        random=fixture.random.copy();inputs=output/'native-random.txt';inputs.write_text(''.join(f'{a} {b} {c} {d}\n'for a,b,c,d,_,_ in random));(output/'native-random-observations.json').write_text(json.dumps(random,indent=2)+'\n');port,compiled=build_family(output,asset,inputs,length,origin);cases=[];failures=[];timings=[];samples=set(range(0,121,6))|{1,2,13,14,15,19,20,26,27,45,64,65,96,97,119,120}
        for repeat in range(2):
            fixture.reset(length,origin)
            for tick in range(TICKS+1):
                if tick:fixture.step()
                native=fixture.state();row=port[tick];errors=compare_state(native,row);image=None;diff=None;corner=0
                if repeat and native!=cases[tick]['native']:raise AssertionError('Native family warm state changed')
                if tick in samples and not args.state_only:
                    start=time.perf_counter();draws=fixture.draws();color,depth=fixture.pixels(draws);mcolor,mdepth=fixture.pixels(row['draws']);timings.append((time.perf_counter()-start)*1000);image=[sha(color),sha(mcolor),sha(depth),sha(mdepth)]
                    if len(draws)!=len(row['draws']):errors.append('draw cardinality')
                    for a,b in zip(draws,row['draws']):
                        if len(a['uvs'])!=4 or len(b['uvs'])!=4:raise AssertionError('Quad UV cardinality')
                        if any(f32(x)!=f32(y)for u,v in zip(a['uvs'],b['uvs'])for x,y in zip(u,v)):errors.append('UV')
                    corner=max([abs(x-y)for a,b in zip(draws,row['draws'])for u,v in zip(a['positions'],b['positions'])for x,y in zip(u,v)]or[0]);diff=sum(a!=b for a,b in zip(struct.unpack('<'+str(WIDTH*HEIGHT)+'H',color),struct.unpack('<'+str(WIDTH*HEIGHT)+'H',mcolor)))
                    if depth!=mdepth:errors.append('depth')
                    if repeat and image!=cases[tick]['image_hashes']:raise AssertionError('Native/production family warm pixels changed')
                    if not repeat:save_rgb565_png(output/f'retail-{tick:03d}.png',color,WIDTH,HEIGHT);save_rgb565_png(output/f'port-{tick:03d}.png',mcolor,WIDTH,HEIGHT)
                if not repeat:
                    if errors or corner>2e-5 or diff:failures.append(dict(tick=tick,errors=errors,corner_error=corner,differing_pixels=diff))
                    cases.append(dict(tick=tick,native=native,port=row,errors=errors,max_corner_error=corner,differing_pixels=diff,image_hashes=image))
            if fixture.random!=random:raise AssertionError('Family native RNG replay changed')
        profiles.append(dict(name=name,length=length,origin=origin,random_invocations=len(random),births=fixture.births,deaths=fixture.deaths,port_components=compiled,case_count=len(cases),median_warm_pair_ms=statistics.median(timings)if timings else None,cases=cases,failures=failures));allfailures.extend([dict(profile=name,**x)for x in failures])
    after={k:sha(v.encode())for k,v in production_spans().items()}
    if before!=after:raise AssertionError('Production family spans changed')
    report=dict(status='fail'if allfailures else'pass',failures=allfailures,retail_sha256=RETAIL_SHA,asset_sha256=ASSET_SHA,probe_sha256=sha(Path(__file__).read_bytes()),source_span_sha256=after,scope='Actual native splash allocation/range+raw RNG/Animate/Render and recursive child Init in one shared Runtime; independent compiled actual production Ripple factory/Advance/Submit and strict observed raw draws. Explicit children-before-parent component order; original sector/map allocation/install, natural lighting, modern GPU separate. Compare removal identities/ticks; preserve raw flag/destructor journals without claiming within-tick destructor order. Identity facing, origin selected without fitting; native 43ad80 local transforms plus declared integer owner translation adapter, not the full retail map world-render path.',profiles=profiles,guest_os_boots=0,dosbox_used=False,pixel_rendering_intercepted=False,full_game_integration=False,metal_backend_compared=False,replays_per_case=2,viewport=[WIDTH,HEIGHT],raster_instruction_limit=5000000)
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items()if k!='profiles'},indent=2))
    if allfailures:raise SystemExit(1)

if __name__=='__main__':main()
