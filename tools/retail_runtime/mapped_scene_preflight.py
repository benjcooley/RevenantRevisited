"""Load an unchanged retail scene through original module/play/map code.

Camera selection is an explicit upstream input. This does not spawn substitute
effects, change saved actor fields, or certify scheduler/display pacing.
"""
import argparse, json, struct, traceback
from types import SimpleNamespace
from pathlib import Path
from character_native_preflight import AuthenticCharacters, HudWorld, ROOT, sha
from blood_actor_preflight import BOOT
from unicorn import UC_PROT_READ

MAP, PLAY, MODULES = 0x6668d8, 0x65caf0, 0x65a768

def run(output, camera, move_player=False, instruction_limit=100000000, accelerate_stores=False, resume=None, skip_precache=False, boot=BOOT):
    output.mkdir(parents=True, exist_ok=False)
    executable=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
    if resume:
        world=HudWorld(executable,image_path=resume);f=SimpleNamespace(world=world,vm=world.vm)
    else:f=AuthenticCharacters(executable,boot)
    v=f.vm;stages=[];checks=[];failure=None
    stores=None
    if accelerate_stores:
        from native_store_loop import install
        stores=install(v)
    precache_calls=[]
    if skip_precache:
        from unicorn import UC_HOOK_CODE
        from unicorn.x86_const import UC_X86_REG_EIP,UC_X86_REG_ESP,UC_X86_REG_ECX
        # Optional CacheImagery only warms ChunkCache; drawing still uses
        # original cache-miss decompression. This is a declared diagnostic
        # boundary until warm/cold scene pixels have been compared.
        def cache(uc,address,size,user):
            sp=uc.reg_read(UC_X86_REG_ESP);precache_calls.append(hex(uc.reg_read(UC_X86_REG_ECX)))
            uc.reg_write(UC_X86_REG_EIP,v.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4)
        v.uc.hook_add(UC_HOOK_CODE,cache,begin=0x418890,end=0x418890)
    def readable(args):
        start,size=args;end=start+size;covered=start
        if not size:return 0
        if not start or end>0x100000000:return 1
        for lo,hi,permissions in sorted(v.uc.mem_regions()):
            if hi<covered:continue
            if lo>covered or not permissions&UC_PROT_READ:break
            covered=min(end,hi+1)
            if covered==end:break
        result=int(covered!=end);checks.append(dict(address=hex(start),size=size,bad=result));return result
    assert readable((0,4))==1 and readable((0,0))==0 and readable((MAP,4))==0
    v.handlers['IsBadReadPtr']=(2,readable)
    def stage(name,address,args=(),this=None,limit=100000000):
        print(name,flush=True);value=f.world.call(address,args,this=this,instruction_limit=limit)
        stages.append(dict(name=name,address=hex(address),args=list(args),result=value));print('PASS',name,value,flush=True)
    try:
        if resume:
            from unicorn.x86_const import UC_X86_REG_EIP
            pc=v.uc.reg_read(UC_X86_REG_EIP);print('resume original call',hex(pc),flush=True)
            result=v.scheduler.execute(pc,instruction_limit)
            if v.scheduler.threads[1].state!='returned':raise RuntimeError('Resumed call did not return')
            stages.append(dict(name='resume original call without changing stack/arguments',address=hex(pc),result=result))
        else:
            f.select_screen_prefix()
            stage('SetCurModule Ahkuilon',0x4609f0,(0,),MODULES)
            stage('whole original BeginScreen/PlayScreen Initialize',0x48e8f0,(),PLAY,1000000000)
            level,x,y,z=camera;point=v.allocate(12);v.write(point,struct.pack('<3i',x,y,z))
            if move_player:
                player=v.u32(0x667fcc)
                if not player:raise RuntimeError('Original play initialization has no player')
                # Original Move's slot8 is SetPos; use the real player override.
                stage('original player SetPos',v.u32(v.u32(player)+8),(point,level,0),player)
            stage('CenterOnPos',0x453940,(point,level,8),MAP)
            stage('UpdateMapPos',0x4539d0,(),MAP)
            stage('UpdateSectors',0x459220,(),MAP,instruction_limit)
    except Exception as exc:
        failure=dict(error=str(exc),fault=f.world.last_fault,trace=traceback.format_exc());print('FAIL',failure,flush=True)
    sectors=[]
    for i in range(v.u32(0x668578)):
        sector=v.u32(v.u32(0x668588)+i*4);objects=[]
        for slot in range(v.u32(sector+0xb8)):
            owner=v.u32(v.u32(sector+0xc8)+slot*4)
            if not owner:continue
            objects.append(dict(slot=slot,address=hex(owner),vtable=hex(v.u32(owner)),map_index=v.u32(owner+0x40),
                position=list(struct.unpack('<3i',v.uc.mem_read(owner+0x10,12))),
                class_id=struct.unpack('<H',v.uc.mem_read(owner+4,2))[0],
                type_index=struct.unpack('<H',v.uc.mem_read(owner+6,2))[0],
                state=struct.unpack('<H',v.uc.mem_read(owner+12,2))[0],
                frame=struct.unpack('<h',v.uc.mem_read(owner+0x5c,2))[0],
                frame_increment=struct.unpack('<h',v.uc.mem_read(owner+0x5e,2))[0],
                flags=hex(v.u32(owner+8)),imagery=hex(v.u32(owner+0x54)),animator=hex(v.u32(owner+0x58))))
        sectors.append(dict(filename=v.string(sector+0x14),level=v.u32(sector+4),x=v.u32(sector+8),y=v.u32(sector+12),objects=objects))
    checkpoint=output/('scene-load-gate.image'if failure else'resident-map.image')
    f.world.save(checkpoint)
    report=dict(status='genuine_scene_load_complete'if not failure else'genuine_scene_load_gate',accepted=False,
        camera=list(camera),move_player=move_player,stages=stages,failure=failure,sectors=sectors,pointer_checks=checks,
        checkpoint_sha256=sha(checkpoint),instruction_limit=instruction_limit,scheduler_trace=v.scheduler.trace[-100:],
        resumed_checkpoint_sha256=sha(resume)if resume else None,
        skipped_bitmap_precache=skip_precache,precache_calls=precache_calls,
        store_acceleration={k:value for k,value in stores.items()if not k.endswith('hook')}if stores else None,
        executable_sha256=sha(executable),boot_sha256=sha(boot),probe_sha256=sha(__file__),
        module_sha256=sha(ROOT/'data/Modules/Ahkuilon.rvm'),boundary_calls=f.world.boundary_calls,scope=__doc__)
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True)
    p.add_argument('--camera',type=int,nargs=4,required=True,metavar=('LEVEL','X','Y','Z'))
    p.add_argument('--move-player',action='store_true',help='Use original public SetPos at the selected camera')
    p.add_argument('--instruction-limit',type=int,default=100000000)
    p.add_argument('--accelerate-stores',action='store_true',help='Use independently checked instruction-equivalent bulk stores')
    p.add_argument('--resume',type=Path,help='Continue a preserved instruction-budget gate without restarting the native call')
    p.add_argument('--boot-image',type=Path,default=BOOT,help='Explicit original startup checkpoint (e.g. original SOFTWARE3D configuration)')
    p.add_argument('--skip-bitmap-precache',action='store_true',help='Qualified diagnostic: omit optional all-state bitmap cache warming; original drawing/decompression remain')
    a=p.parse_args()
    if a.instruction_limit<1:p.error('--instruction-limit must be positive')
    result=run(a.output,a.camera,a.move_player,a.instruction_limit,a.accelerate_stores,a.resume,a.skip_bitmap_precache,a.boot_image)
    raise SystemExit(1 if result['failure'] else 0)
