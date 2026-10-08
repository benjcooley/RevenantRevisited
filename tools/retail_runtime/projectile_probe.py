#!/usr/bin/env python3
"""Run original missile movement/launch/impact against the current Fireball rig.

Original Move and Pulse compute the trajectory. Guest facade methods only store
SetPos/SetState/SetFlags results; the host never supplies per-tick positions.
Character enumeration, health/enmity and ground are selected fixture inputs.
This is shared missile behavior, not a complete Fireball animator/damage world.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import subprocess
import time
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EIP,UC_X86_REG_ESP
from runtime import Runtime
from build_contract import verify_build

ROOT=Path(__file__).resolve().parents[2]
RETAIL_SHA='28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5'
FIELDS={'position':0x10,'velocity':0x1c,'accumulator':0x28,'state':0xc,
        'flags':8,'range':0x184,'speed':0x188,'status':0x18c,'spell':0xd8,'animator':0x58}


def digest(data):return hashlib.sha256(data).hexdigest()


class MissileProbe:
    """Direct original shared missile machine with explicit two endpoints."""
    def __init__(self,executable,build=None,vm=None):
        self.build=verify_build(executable,build)
        started=time.perf_counter();self.vm=vm if vm is not None else Runtime(executable)
        if digest(self.vm.image)!=self.build['executable_sha256']:
            raise ValueError('Named executable changed during loading')
        vm=self.vm
        # Original color-table subrange builds original Angle/Distance/DistX/Y
        # tables with x87 math. It is a bounded entry, not a host table formula.
        vm.call(0x41e2de,stop_address=0x41e535,instruction_limit=20000000)
        if struct.unpack('<h',vm.uc.mem_read(0x634f44,2))[0]!=256:
            raise AssertionError('Original facing tables did not initialize')
        self.lookup=bytes(vm.uc.mem_read(0x634d44,1024))
        self.obj=vm.allocate(0x600);self.source=vm.allocate(0x600);self.target=vm.allocate(0x600)
        self.spell=vm.allocate(0x140);self.vtable=vm.allocate(0x210)
        vm.write(self.vtable,bytes(vm.uc.mem_read(0x5b3e18,0x210)))
        # Tiny guest interface stores do not calculate movement or collision.
        # SetPos(point,level,flags): copy exact original Move-calculated XYZ.
        stores={
            8:bytes.fromhex('8b4424048b108951108b50048951148b5008895118b801000000c20c00'),
            0x18:bytes.fromhex('8b4424046689410cb801000000c20400'),
            0x40:bytes.fromhex('8b442404894108c20400'),
        }
        for offset,code in stores.items():
            pointer=vm.allocate(len(code));vm.write(pointer,code);vm.put_u32(self.vtable+offset,pointer)
        vm.put_u32(self.vtable+0x1c0,0x477e50)  # Explicit fixture character-health interface.
        for entity in (self.obj,self.source,self.target):
            vm.put_u32(entity,self.vtable);vm.put_u32(entity+4,25)
            vm.put_u32(entity+0x78,0xffffffff)  # Inventory slot -1, see original477a10.
        vm.put_u32(self.obj+0xd8,self.spell)
        vm.put_u32(self.spell+4,self.source);vm.put_u32(self.spell+0xc,1)
        vm.put_u32(self.spell+0x10,self.target)
        self.tick=0;self.events=[];self.calls={};self.ground_height=1
        self.wall_x=None;self.target_hp=100;self.enemy=True
        # These are external world inputs and the excluded general effect chain.
        for address in (0x452e10,0x44ceb0,0x44d080,0x477e50,0x4c89c0,
                        0x4de800,0x477a10,0x4def00):
            vm.uc.hook_add(UC_HOOK_CODE,self.boundary,begin=address,end=address)
        # Track actual executed original function entries without replacing them.
        for address in (0x510220,0x470920,0x4df070,0x46db20,0x46dbe0,0x46dc60,
                        0x46ea20,0x4defe0):
            vm.uc.hook_add(UC_HOOK_CODE,self.original_entry,begin=address,end=address)
        vm.checkpoint();self.load_ms=(time.perf_counter()-started)*1000

    def original_entry(self,uc,address,size,user):
        key=hex(address);self.calls[key]=self.calls.get(key,0)+1

    def ret(self,cleanup=0,result=None):
        vm=self.vm;uc=vm.uc;sp=uc.reg_read(UC_X86_REG_ESP)
        if result is not None:uc.reg_write(UC_X86_REG_EAX,result&0xffffffff)
        uc.reg_write(UC_X86_REG_EIP,vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+cleanup)

    def boundary(self,uc,address,size,user):
        vm=self.vm;sp=uc.reg_read(UC_X86_REG_ESP);receiver=uc.reg_read(UC_X86_REG_ECX)
        if address==0x452e10:
            xyz=struct.unpack('<3i',vm.uc.mem_read(vm.u32(sp+4),12))
            height=10000 if self.wall_x is not None and xyz[0]>=self.wall_x else self.ground_height
            self.ret(12,height)
        elif address==0x44ceb0:
            # Deterministic character query includes source and target. Target
            # health/enemy and ORIGINAL distance checks decide whether it hits.
            vm.write(receiver,bytes(0x30));vm.put_u32(receiver+0xc,self.source)
            vm.put_u32(receiver+8,self.target);self.ret(24,receiver)
        elif address==0x44d080:
            vm.put_u32(receiver+0xc,vm.u32(receiver+8));vm.put_u32(receiver+8,0);self.ret()
        elif address==0x477e50:
            self.ret(result=self.target_hp if receiver==self.target else 100)
        elif address==0x4c89c0:self.ret(4,int(self.enemy))
        elif address==0x477a10:self.ret(result=0)  # Explicit no inventory.
        elif address==0x4def00:self.ret(result=0)  # Skip spell completion callback.
        elif address==0x4de800:self.ret()  # General effect/script/animation chain excluded.

    def configure(self,source,target,speed=8,target_hp=100,enemy=True,wall_x=None,
                  ground_height=1,launch_ready=True,animator_present=False):
        if len(source)!=3 or len(target)!=3 or source==target:
            raise ValueError('Projectile requires distinct source and destination XYZ')
        if not isinstance(speed,int) or not 1<=speed<=120:raise ValueError('Speed must be an integer1..120')
        self.vm.restore();self.tick=0;self.events=[];self.calls={}
        # Configure is a new launch, including after a mid-flight checkpoint.
        # Clear stored state/velocity/fractional accumulators, never per-tick XYZ.
        self.vm.write(self.obj+0xc,bytes(0x28))
        self.ground_height=ground_height;self.wall_x=wall_x;self.target_hp=target_hp;self.enemy=enemy
        self.source_xyz=tuple(source);self.target_xyz=tuple(target)
        for entity,xyz in ((self.source,source),(self.target,target),(self.obj,source)):
            self.vm.write(entity+0x10,struct.pack('<3i',*xyz))
        self.vm.put_u32(self.obj+8,1)
        self.vm.put_u32(self.obj+0x184,32768);self.vm.put_u32(self.obj+0x188,speed*65536)
        self.vm.put_u32(self.obj+0x18c,int(launch_ready))
        self.vm.put_u32(self.obj+0x58,self.source if animator_present else 0)
        self.angle=self.vm.call(0x4df070,this=self.obj)
        return self.inspect()

    def inspect(self):
        vm=self.vm
        return dict(tick=self.tick,position=list(struct.unpack('<3i',vm.uc.mem_read(self.obj+0x10,12))),
            velocity_fixed=list(struct.unpack('<3i',vm.uc.mem_read(self.obj+0x1c,12))),
            accumulator=list(struct.unpack('<3i',vm.uc.mem_read(self.obj+0x28,12))),
            state=vm.u32(self.obj+0xc)&0xffff,flags=vm.u32(self.obj+8),range=vm.u32(self.obj+0x184),
            angle=vm.u32(self.obj+0xe0),kill_requested=bool(vm.u32(self.obj+8)&0x1000),
            milliseconds=vm.milliseconds)

    def step(self,ticks=1):
        rows=[]
        for _ in range(ticks):
            before=self.inspect()
            self.vm.call(0x510220,this=self.obj)
            self.tick+=1;self.vm.advance();after=self.inspect();rows.append(after)
            if before['state']!=after['state']:
                self.events.append(dict(tick=self.tick,event={1:'launch',2:'impact'}[after['state']],
                    position=after['position'],remaining_range=after['range']))
            if not before['kill_requested'] and after['kill_requested']:
                self.events.append(dict(tick=self.tick,event='kill_requested',position=after['position']))
        return rows

    def checkpoint(self):
        from copy import deepcopy
        if not hasattr(self,'source_xyz'):
            raise ValueError('Configure the missile fixture before checkpointing')
        self.vm.checkpoint()
        fields=('tick','events','calls','ground_height','wall_x','target_hp','enemy',
                'source_xyz','target_xyz','angle')
        self.control_snapshot=deepcopy({name:getattr(self,name) for name in fields})

    def reset(self):
        from copy import deepcopy
        if not hasattr(self,'control_snapshot'):
            raise ValueError('Missile fixture has no control checkpoint')
        restored=self.vm.restore()
        for name,value in deepcopy(self.control_snapshot).items():setattr(self,name,value)
        return dict(restored_pages=restored,state=self.inspect())

    def set_source(self,position):
        if not isinstance(position,(tuple,list)) or len(position)!=3 or any(
                type(value) is not int or not -0x80000000<=value<=0x7fffffff for value in position):
            raise ValueError('Source fixture position must be three signed int32 values')
        self.vm.write(self.source+0x10,struct.pack('<3i',*position))
        self.source_xyz=tuple(position)
        return list(self.source_xyz)

    def set_target(self,position,target_hp=None,enemy=None):
        if not isinstance(position,(tuple,list)) or len(position)!=3 or any(
                type(value) is not int or not -0x80000000<=value<=0x7fffffff for value in position):
            raise ValueError('Target fixture position must be three signed int32 values')
        if target_hp is not None and (type(target_hp) is not int or not -0x80000000<=target_hp<=0x7fffffff):
            raise ValueError('Target fixture hp must fit signed int32')
        if enemy is not None and type(enemy) is not bool:
            raise ValueError('Target fixture enemy flag must be boolean')
        self.vm.write(self.target+0x10,struct.pack('<3i',*position))
        self.target_xyz=tuple(position)
        if target_hp is not None:self.target_hp=target_hp
        if enemy is not None:self.enemy=enemy
        return dict(position=list(self.target_xyz),target_hp=self.target_hp,enemy=self.enemy)


def compile_current_port(output,probe):
    """Compile production shared controller and actual engine math helpers."""
    obj=(ROOT/'src/object.cpp').read_text()
    def function(signature):
        start=obj.index(signature);opening=obj.index('{',start);depth=1;end=opening+1
        while depth:
            if obj[end]=='{':depth+=1
            elif obj[end]=='}':depth-=1
            end+=1
        return obj[start:end]
    helpers='\n'.join(function(signature) for signature in (
        'void ConvertToVector(', 'int32_t ConvertToFacing(const S3DPoint& target)',
        'int32_t ConvertToFacing(const S3DPoint& pos, const S3DPoint& target)',
        'int32_t Distance(const S3DPoint& pos)',
        'int32_t Distance(const S3DPoint& pos, const S3DPoint& target)'))
    generated=r'''
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include "missilestate.h"
#define absval(i) ((i)>0?(i):-(i))
using S3DPoint=missile_state::Point;
short DistX[256],DistY[256];
unsigned char DistTable[256][256],AngleTable[256][256];
void ConvertToVector(int32_t,int32_t,S3DPoint&,int32_t=0);
'''+helpers+r'''
int main(int argc,char**argv){
    if(argc!=16)return 2;
    FILE*f=std::fopen(argv[1],"rb");if(!f)return 3;
    if(std::fread(DistX,1,512,f)!=512||std::fread(DistY,1,512,f)!=512||
       std::fread(DistTable,1,65536,f)!=65536||std::fread(AngleTable,1,65536,f)!=65536)return 4;
    std::fclose(f);
    missile_state::State effect;
    effect.position={std::atoi(argv[2]),std::atoi(argv[3]),std::atoi(argv[4])};
    S3DPoint target{std::atoi(argv[5]),std::atoi(argv[6]),std::atoi(argv[7])};
    int target_hp=std::atoi(argv[8]);bool enemy=std::atoi(argv[9]);
    int wall_x=std::atoi(argv[10]),ground=std::atoi(argv[11]);
    effect.speed_fixed=std::atoi(argv[12])*65536;
    missile_state::Inputs input;input.launch_ready=std::atoi(argv[13]);input.animator_present=std::atoi(argv[14]);
    input.ground_height=[&](const S3DPoint&p){return p.x>=wall_x?10000:ground;};
    input.aim_angle=[&](){return ConvertToFacing(effect.position,target);};
    input.convert_vector=[](int a,int32_t speed){S3DPoint v;ConvertToVector(a,speed,v);return v;};
    input.character_hit=[&](const S3DPoint&p){return target_hp>0&&enemy&&Distance(p,target)<=32;};
    // Original GetAngle is observable before launch in the reference fixture.
    effect.angle=ConvertToFacing(effect.position,target);
    for(int tick=0;tick<std::atoi(argv[15]);++tick){
        effect.Advance(input);
        std::printf("%d %d %d %d %d %d %d %d %d %d %d %u %d %d %d\n",tick+1,effect.state,
            effect.position.x,effect.position.y,effect.position.z,
            effect.velocity_fixed.x,effect.velocity_fixed.y,effect.velocity_fixed.z,
            effect.accumulator.x,effect.accumulator.y,effect.accumulator.z,
            effect.flags,effect.range,effect.angle,bool(effect.flags&missile_state::Kill));
    }
}
'''
    source=output/'missile-port-driver.cpp';source.write_text(generated)
    lookup=probe.lookup+bytes(probe.vm.uc.mem_read(0x5e9200,65536))+bytes(probe.vm.uc.mem_read(0x63da64,65536))
    table=output/'original-movement-tables.bin';table.write_bytes(lookup)
    binary=output/'missile-port-driver'
    command=['clang++','-std=c++17','-iquote',str(ROOT/'src'),str(source),
             str(ROOT/'src/missilestate.cpp'),'-o',str(binary)]
    result=subprocess.run(command,capture_output=True,text=True)
    (output/'port-compile.log').write_text(result.stdout+result.stderr)
    if result.returncode:raise RuntimeError('Current Fireball Pulse compile failed; inspect port-compile.log')
    return binary,table,dict(command=command,generated_source_sha256=digest(source.read_bytes()),
                            binary_sha256=digest(binary.read_bytes()),lookup_sha256=digest(lookup))


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable',type=Path);parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--build',type=Path,help='Explicit named fixed-layout variant build.json')
    args=parser.parse_args();args.output.mkdir(parents=True,exist_ok=True)
    paths=[ROOT/p for p in ('src/effect.cpp','src/effect.h','src/object.cpp','src/missileeffect.cpp',
                          'src/missilestate.cpp','src/missilestate.h')]
    before={str(p.relative_to(ROOT)):digest(p.read_bytes()) for p in paths}
    probe=MissileProbe(args.executable,args.build);binary,table,compiled=compile_current_port(args.output,probe)
    probe.configure([0,0,128],[320,0,128])
    probe.vm.call(0x510bd0,this=probe.obj)
    leaf_defaults=dict(function='0x510bd0',speed_fixed=probe.vm.u32(probe.obj+0x188),
        status=probe.vm.u32(probe.obj+0x18c),range=probe.vm.u32(probe.obj+0x184),
        damage_once=probe.vm.u32(probe.obj+0x194))
    if leaf_defaults['speed_fixed']!=8*65536 or leaf_defaults['status']!=1:
        raise AssertionError('Actual retail Fireball defaults differ from production initialization')
    cases=[];differences=[];timings=[];checks=0
    definitions=[
        dict(name='range_expiry_x',source=[0,0,128],target=[1000,0,128],target_hp=100,ticks=64),
        dict(name='target_impact_x',source=[0,0,128],target=[320,0,128],target_hp=100,ticks=64),
        dict(name='dead_target_is_skipped',source=[0,0,128],target=[320,0,128],target_hp=0,ticks=64),
        dict(name='friendly_target_is_skipped',source=[0,0,128],target=[320,0,128],enemy=False,ticks=64),
        dict(name='wall_blocks',source=[0,0,128],target=[1000,0,128],wall_x=160,ticks=64),
        dict(name='diagonal_accumulator',source=[-20,40,128],target=[300,360,128],target_hp=0,ticks=64),
        dict(name='negative_x',source=[0,0,128],target=[-1000,0,128],ticks=64),
        dict(name='positive_y',source=[0,0,128],target=[0,1000,128],ticks=64),
        dict(name='negative_y',source=[0,0,128],target=[0,-1000,128],ticks=64),
        dict(name='negative_diagonal',source=[20,-40,128],target=[-300,-360,128],target_hp=0,ticks=64),
    ]
    for speed in (1,3,5,16,32):
        definitions.append(dict(name=f'speed_{speed}_range',source=[0,0,128],target=[1000,0,128],
            speed=speed,ticks=480//speed+4))
    for spec in definitions:
        actual=[];initial=None
        for repeat in range(2):
            start=time.perf_counter();options={k:v for k,v in spec.items() if k not in ('name','ticks')}
            state=probe.configure(**options);rows=probe.step(spec['ticks'])
            result=dict(initial=state,rows=rows,events=list(probe.events),original_calls=dict(probe.calls))
            timings.append((time.perf_counter()-start)*1000)
            if repeat==0:initial=result;actual=rows
            elif result!=initial:raise AssertionError('Original projectile did not replay exactly')
        if actual[-1]['state']!=2 or not actual[-1]['kill_requested']:
            raise AssertionError(f"Original missile did not impact/request kill: {spec['name']}")
        if len({tuple(row['position']) for row in actual})<2:
            raise AssertionError('Original projectile did not actually move')
        command=[str(binary),str(table),*[str(v) for v in spec['source']],*[str(v) for v in spec['target']],
                 str(spec.get('target_hp',100)),str(int(spec.get('enemy',True))),str(spec.get('wall_x',2147483647)),
                 str(spec.get('ground_height',1)),str(spec.get('speed',8)),str(int(spec.get('launch_ready',True))),
                 str(int(spec.get('animator_present',False))),str(spec['ticks'])]
        port=[[int(x) for x in line.split()] for line in subprocess.check_output(command,text=True).splitlines()]
        if len(port)!=len(actual):raise AssertionError('Port trace length mismatch')
        case_errors=[]
        for row,ported in zip(actual,port):
            tick,state,x,y,z,vx,vy,vz,ax,ay,az,flags,range_left,angle,kill=ported
            expected=[row['tick'],row['state'],*row['position'],*row['velocity_fixed'],*row['accumulator'],
                      row['flags'],row['range'],row['angle'],int(row['kill_requested'])]
            names=['tick','state','x','y','z','vx_fixed','vy_fixed','vz_fixed','accum_x','accum_y','accum_z',
                   'flags','range','angle','kill_requested']
            for field,a,b in zip(names,expected,ported):
                checks+=1
                if a!=b:case_errors.append(dict(tick=row['tick'],field=field,retail=a,port=b))
        differences.extend(dict(case=spec['name'],**error) for error in case_errors)
        cases.append(dict(**spec,**initial,port_rows=port,comparison_errors=case_errors))
    after={str(p.relative_to(ROOT)):digest(p.read_bytes()) for p in paths}
    if before!=after:raise AssertionError('Production projectile source changed during run')
    report=dict(status='differences_found' if differences else 'pass',original_trajectory_status='pass',
        port_trajectory_status='fail' if differences else 'pass',retail_sha256=probe.build['executable_sha256'],
        baseline_sha256=RETAIL_SHA,build=probe.build,
        source_sha256=after,original_fields={k:hex(v) for k,v in FIELDS.items()},
        original_functions=['0x510220','0x470920','0x4df070','0x46db20','0x46ea20','0x4defe0'],
        original_table_setup=dict(start='0x41e2de',stop='0x41e535',load_ms=probe.load_ms),
        original_leaf_defaults=leaf_defaults,compiled_port=compiled,case_count=len(cases),replays_per_case=2,
        total_ticks_compared=sum(case['ticks'] for case in cases),integer_field_comparisons=checks,
        mean_warm_scenario_ms=sum(timings)/len(timings),error_count=len(differences),errors=differences,
        cases=cases,guest_os_boots=0,dosbox_used=False,host_per_tick_position_forcing=False,
        projectile_motion_executed=True,source_destination_used=True,
        impact_selection_executed=True,kill_request_executed=True,map_reaper_executed=False,
        fireball_animator_and_damage_executed=False,pixel_comparison=False,
        external_boundaries=['ground/wall-height queries return selected fixture topology',
            'character query enumerates source+target; explicit target health/enmity supplied',
            'guest SetPos stores XYZ calculated entirely by original Move; no host trajectory updates',
            'guest SetState/SetFlags store original requested state/flags; imagery callbacks excluded',
            'general TEffect::Pulse script/animation chain and spell completion callback excluded'],
        port_profile='Production missilestate module used by Fireball actual Pulse and preview. '
            'Actual object.cpp facing/vector/distance helpers compiled with identical original-initialized lookup inputs. '
            'Source/destination, target health/enmity and ground fixtures are matched; no speed scaling or fractional truncation.',
        scope='Strict state/position comparison against original shared missile launch, fixed-point Move, '
            'character/terrain/range impact and kill request. This is not full Fireball burst/spark/render '
            'or damage/map removal acceptance. Original integer position, velocity, accumulator, flags, range, '
            'facing and kill request compare exactly; this does not certify all-world query implementations.')
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k not in ('cases','errors')},indent=2))
    if differences:raise SystemExit(1)


if __name__=='__main__':main()
