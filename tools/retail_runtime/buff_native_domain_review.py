#!/usr/bin/env python3
"""Same-binary common/native domain ablation, with independent original images."""
import argparse,json,re
from pathlib import Path
from PIL import Image,ImageDraw
from buff_visual_review import review
from firecone_state_probe import sha


def run(reference,metal,maps,output):
    report=review(reference,metal,maps,output,metal_suffix='-native')
    panels={tick:Image.new('RGB',(900,720),(24,24,24))for tick in(5,15,29)}
    for case in report['cases']:
        name=case['name'];number=case['capture']
        common=metal/(name+'-common');capture=json.loads((common/'manifest.json').read_text())
        if capture['status']!='pass' or capture['scenario'].get('native_domain') or capture['binary_sha256']!=report['metal_binary_sha256']:raise AssertionError('Common/native captures are not a same-binary ablation')
        log=(common/'run.log').read_text();rng={int(a):int(b)for a,b in re.findall(r'\[vfx-rng-scope\] before_render=(\d+) global_random_draws=(\d+)',log)}
        if rng[number+1]!=case['native_rng_draws_before_capture']:raise AssertionError('Common mode RNG differs')
        light=next(line for line in log.splitlines()if '[vfx-source-light]'in line)
        if light.split('[vfx-source-light]')[1]!=case['source_light_log'].split('[vfx-source-light]')[1]:raise AssertionError('Common mode source lighting differs')
        frames=sorted(common.glob('frame-[0-9][0-9][0-9]-*.png'));paths=[Path(case['native']),frames[number-1],Path(case['metal'])]
        images=[Image.open(p).convert('RGB')for p in paths]
        errors=[sum(sum(abs(a-b)for a,b in zip(x,y))for x,y in zip(images[0].getdata(),image.getdata()))for image in images[1:]]
        case['ablation']=dict(order=['original','metal_common','metal_native_domain'],files=[str(p)for p in paths],sha256=[sha(p.read_bytes())for p in paths],bbox=[image.getbbox()for image in images],absolute_rgb_error=errors)
        if number in panels:
            row=('Speed','Quicksilver','Fmastery').index(name);panel=panels[number];labels=ImageDraw.Draw(panel)
            for col,(image,label)in enumerate(zip(images,('Original software','Metal common','Metal native domain'))):
                x=col*300;y=row*240;panel.paste(image.crop((275,115,365,215)).resize((180,200),Image.Resampling.NEAREST),(x,y+25));labels.text((x,y+4),name+' '+label,fill='white')
    for tick,panel in panels.items():panel.save(output/f'domain-capture{tick:03d}.png')
    report['ablation_scope']='Same binary, seed, cadence, lighting, quality and original asset; only explicit native-domain preview flag differs. Native per-draw MODELZ/owner and executed-binary-derived projector; no camera/color fitting. Default real map remains common-domain. Shared raster quantization already present in both ablation branches; remaining RGB error is not claimed pixel parity.'
    report['total_absolute_rgb_error_common_native']=[sum(c['ablation']['absolute_rgb_error'][i]for c in report['cases'])for i in(0,1)]
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    for name in('reference','metal','maps','output'):p.add_argument('--'+name,type=Path,required=True)
    a=p.parse_args();r=run(a.reference,a.metal,a.maps,a.output);print(r['status'],r['total_absolute_rgb_error_common_native'])
