"""Observe Puke's actual ARGB4444 integer packets without replacing its raster."""
import argparse, json, struct, zipfile
from pathlib import Path
from puke_visual_reference import PukeScene, ROOT, sha
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP

def run(reference, output):
    output.mkdir(parents=True, exist_ok=False)
    baseline=json.loads(reference.read_text())
    with zipfile.ZipFile(ROOT/'data/imagery.rvi') as z:
        data=z.read(next(n for n in z.namelist() if n.lower()=='imagery/magic/puke.i3d'))
    scene=PukeScene(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data)
    vm=scene.vm; transformed=[]; packets=[]; frames=[]
    def observe_transform(uc,*_):
        sp=uc.reg_read(UC_X86_REG_ESP);count=vm.u32(sp+4)
        transformed[:]=list(struct.iter_unpack('<3f4I2f',uc.mem_read(vm.u32(0x675fb4),count*36)))
    def observe_packet(uc,*_):
        sp=uc.reg_read(UC_X86_REG_ESP)
        args=list(struct.unpack('<20i',uc.mem_read(sp+4,80)))
        actual=[args[1+i*5:6+i*5] for i in range(3)]
        # Actual texture metadata supplies width/height masks, not an inferred
        # texture size or a new UV convention.
        width,height=args[18]+1,args[19]+1
        expected=[[int(v[0]),int(-v[1]),int(v[2]),int(v[7]*width),int(v[8]*height)] for v in transformed]
        assert all(v in expected for v in actual),(actual,expected)
        packets.append(dict(tick=scene.tick,object=scene.current_object,
                            floating=list(transformed),integer=actual,
                            texture_size=[width,height]))
    vm.uc.hook_add(UC_HOOK_CODE,observe_transform,begin=0x56d960,end=0x56d960)
    vm.uc.hook_add(UC_HOOK_CODE,observe_packet,begin=0x54e050,end=0x54e050)
    for tick,previous in enumerate(baseline['cases']):
        if tick:scene.advance()
        before=len(packets);raw=scene.render()
        draws=json.loads(json.dumps(scene.draws))
        if scene.state()!=previous['state'] or draws!=previous['draws']:
            (output/'difference.json').write_text(json.dumps(dict(tick=tick,state=scene.state(),expected_state=previous['state'],draws=scene.draws,expected_draws=previous['draws']),indent=2))
        assert scene.state()==previous['state'] and draws==previous['draws'],tick
        assert sha(raw)==previous['rgb565_sha256'],tick
        frames.append(dict(tick=tick,packets=len(packets)-before,pixels=previous['nonzero_pixels'],unchanged=True))
    report=dict(status='pass',accepted=False,visual_pass=False,frames=frames,packets=packets,
                packet_count=len(packets),integer_channels=len(packets)*15,
                reference_sha256=sha(reference.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),
                scope=__doc__,limits='All original state, draw inputs and software framebuffer hashes preserved. Exact executed54e050 packet XYZ and UV*texture-size truncate toward zero. This does not prove identical GPU triangle coverage or native fixed-point interpolation.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print('PASS',len(frames),'unchanged frames',len(packets),'packets')

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--reference',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.reference,a.output)
