"""Exact loaded Nakrnoth owner, controller and full default-render reference."""
import argparse
import json
import struct
import zipfile
from pathlib import Path
from character_native_preflight import ROOT, sha
from fire_authentic_preflight import FireScene
from software_probe import save_rgb565_png
from speed_controller_preflight import read_asset
from lit_software_probe import LitSoftwareFixture
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX

class NakrnothScene(FireScene):
    def __init__(self, executable, boot_image, width=1280, height=960, center=None, fast=False):
        self.fast = fast
        super().__init__(executable, boot_image)
        if (width,height)!=(self.width,self.height):
            # Directional lights belong to the original scene, not the Python
            # fixture. Remove its registered light before replacing RAM targets;
            # otherwise the new fixture adds a second directional contribution.
            if self.software.directional is not None:
                self.vm.call(0x56d120, (self.software.directional,))
                self.software.directional = None
            self.width,self.height=width,height
            self.software=LitSoftwareFixture(executable,width,height,camera=(3185,3185,0),vm=self.vm)
            self.software.lighting((38,38,38),(1,1,1))
        if center is not None:self.center=tuple(center)
    def observe_draw(self, uc, *unused):
        if self.fast and not self.raster_enabled:
            sp=uc.reg_read(UC_X86_REG_ESP)
            uc.reg_write(UC_X86_REG_EAX,0);uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+32)
            return
        return super().observe_draw(uc,*unused)
    def new_object(self, name, *args, **kwargs):
        return super().new_object('Nakrnoth', *args, **kwargs)

    def controller_state(self):
        v = self.vm
        result = []
        for i in range(v.u32(self.animator + 0x58)):
            c = v.u32(v.u32(self.animator + 0x68) + 4*i)
            row = dict(vtable=hex(v.u32(c)))
            if v.u32(c) == 0x5a3544:
                pool, capacity = v.u32(c+0x2c), v.u32(c+0xf8)
                row.update(capacity=capacity, credit=struct.unpack('<f',v.uc.mem_read(c+0x16c,4))[0],
                           pool_hex=bytes(v.uc.mem_read(pool,capacity*252)).hex())
            result.append(row)
        return result


def run(executable, boot_image, output, samples, frames=790, width=1280, height=960, center=None, fast=False):
    output.mkdir(parents=True, exist_ok=False)
    f = NakrnothScene(executable, boot_image,width,height,center,fast)
    v, call = f.vm, f.world.call
    with zipfile.ZipFile(ROOT/'data/imagery.rvi') as archive:
        data = archive.read(next(n for n in archive.namelist()
                                 if n.lower() == 'imagery/magic/nakrnoth.i3d'))
    asset = read_asset(data)
    rows = []
    for tick in range(1, frames+1):
        v.put_u32(0x65cb38, tick)
        call(0x470cc0, this=f.owner)
        call(0x4de800, this=f.owner)
        f.raster_enabled = tick in samples
        pixels = f.render()
        row = dict(tick=tick, state=f.state(), controllers=f.controller_state() if not fast else [],
                   bounds=f.draw_bounds(), draw_count=len(f.draws),
                   draws=f.draws if f.raster_enabled else [])
        if f.raster_enabled:
            name = f'native-{tick:03d}.png'
            save_rgb565_png(output/name, pixels, f.width, f.height)
            row.update(image=name, sha256=sha(output/name),
                       pixels=sum(x!=(0,) for x in struct.iter_unpack('<H', pixels)))
        rows.append(row)
        if tick % 10 == 0 or f.raster_enabled:
            print(tick, row['state']['frame'], row['state']['flags'],
                  len(f.draws), row.get('pixels'), row['bounds'], flush=True)
    result = dict(effect='Nakrnoth', type_id='0xad92bd21', records=rows,
                  asset={k:asset[k] for k in ('sha256','states','state0_frames','tags')},
                  executable_sha256=sha(executable), boot_image_sha256=sha(boot_image),
                  viewport=[f.width,f.height], center=list(f.center), camera=[3185,3185,0],
                  source=[3185,3185,16], samples=samples,
                  scope='Actual typed NewObject/default CreateAnimator and shipped resource/tag bytes. '
                        'Original NextFrame/whole owner Pulse and Animate/BeginScene/PreRender/'
                        'Render/PostRender/EndScene every tick. Declared RAM target and ambient38/dir1; '
                        'only terminal raster dispatch omitted on unsampled ticks. Original srand1/rand. '
                        'Null spell, fixed owner, empty map screen prefix. Natural boss/spell/audio/device '
                        'and exact-pixel acceptance remain open.')
    (output/'manifest.json').write_text(json.dumps(result,indent=2)+'\n')
    return result

if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe')
    p.add_argument('--boot-image',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True)
    p.add_argument('--frames',type=int,default=790)
    p.add_argument('--width',type=int,default=1280);p.add_argument('--height',type=int,default=960);p.add_argument('--center',type=int,nargs=2);p.add_argument('--fast',action='store_true')
    p.add_argument('--samples',type=int,nargs='+',default=[1,17,41,80,120,160,200,240,280,320,360,400,440,480,520,560,600,640,680,700,720,740,750,760,770,775,780,790])
    a=p.parse_args();run(a.executable,a.boot_image,a.output,a.samples,a.frames,a.width,a.height,a.center,a.fast)
