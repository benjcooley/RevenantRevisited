"""Unfitted Halo four-policy differential with all public-case state and pixels."""
import argparse,hashlib,json,re
from pathlib import Path
from PIL import Image,ImageChops,ImageDraw,ImageStat

def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def nonzero(im):
    r,g,b=im.split();return im.width*im.height-ImageChops.lighter(ImageChops.lighter(r,g),b).histogram()[0]
def run(native,captures,baseline,output):
    output.mkdir(parents=True,exist_ok=False);n=json.loads(native.read_text());films=[json.loads((p/'manifest.json').read_text())for p in captures]
    assert len(films)==4 and len({m['binary_sha256']for m in films})==1
    ticks=len(n['rows']);total=ticks-10;state_fields=0
    for policy,(folder,m)in enumerate(zip(captures,films)):
        assert m['status']=='pass'and m['sample_simulation_ticks']==list(range(1,ticks+1))
        states=re.findall(r'\[halo-state\] frame=(\d+) scale=(\S+) done=(\d+) rng=(\d+) policy=(\d+)',(folder/'run.log').read_text())
        assert len(states)==total+1
        for frame,scale,done,rng,flag in states:
            row=n['rows'][int(frame)-1]
            assert row['frame']==int(frame)and bool(int(row['flags'],16)&4096)==bool(int(done))and int(rng)==0 and int(flag)==policy
            assert abs(row['scale']-float(scale))<1e-7;state_fields+=4
        for frame in m['frames']:assert sha(frame['path'])==frame['sha256']
    previous=json.loads(baseline.read_text());same=0
    for a,b in zip(previous['frames'],films[0]['frames']):
        old=Image.open(a['path']).convert('RGB');new=Image.open(b['path']).convert('RGB')
        same+=old.size==new.size and old.tobytes()==new.tobytes()
    rows=[];images={}
    for row in n['rows']:
        tick=row['tick'];paths=[Path(row['image'])]+[Path(m['frames'][tick-1]['path'])for m in films]
        assert sha(paths[0])==row['image_sha256'];ims=[Image.open(p).convert('RGB')for p in paths];stats=[]
        for p,im in zip(paths,ims):
            assert im.size==(640,480);diff=ImageChops.difference(ims[0],im)
            stats.append(dict(path=str(p),sha256=sha(p),pixels=nonzero(im),bounds=im.getbbox(),different_pixels=nonzero(diff),absolute_rgb_error=int(sum(ImageStat.Stat(diff).sum))))
        rows.append(dict(tick=tick,scale=row['scale'],policies=stats));images[tick]=ims
    selected=[t for t in (1,3,6,10,15,20,21,29,30,31,45,59,60,61,70)if t<=ticks]
    whole=Image.new('RGB',(1600,265*len(selected)));detail=Image.new('RGB',(1600,281*len(selected)));wd=ImageDraw.Draw(whole);dd=ImageDraw.Draw(detail)
    for r,tick in enumerate(selected):
        for c,(im,label)in enumerate(zip(images[tick],('Original','Default','Grid','Quant','Grid + quant'))):
            whole.paste(im.resize((320,240)),(c*320,r*265+25));wd.text((c*320+4,r*265+4),f'{label} tick{tick}',fill='white')
            detail.paste(im.crop((288,263,352,327)).resize((256,256),Image.Resampling.NEAREST),(c*320,r*281+25));dd.text((c*320+4,r*281+4),f'{label} tick{tick} fixed ROI x4',fill='white')
    whole.save(output/'contact.png');detail.save(output/'detail.png')
    report=dict(status='pass_diagnostic',accepted=False,visual_pass=False,totalframes=total,source_fields_checked=state_fields,
                baseline_frames_identical=same,frames=ticks,rows=rows,binary_sha256=films[0]['binary_sha256'],
                total_absolute_rgb_error=[sum(r['policies'][c]['absolute_rgb_error']for r in rows)for c in range(5)],
                native_sha256=sha(native),baseline_sha256=sha(baseline),capture_sha256={str(p):sha(p/'manifest.json')for p in captures},
                contact_sha256=sha(output/'contact.png'),detail_sha256=sha(output/'detail.png'),tool_sha256=sha(__file__),
                limits='Same public caller parameters, geometry/state/RNG/lighting/camera/cadence. Flags change only original integer lattice and ARGB source contribution. Metrics are diagnostics, not fitted thresholds or automatic appearance approval; fixed-point scan conversion and destination rounding remain open.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',state_fields,'baseline',same,'errors',report['total_absolute_rgb_error'])

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--native',type=Path,required=True);p.add_argument('--captures',type=Path,nargs=4,required=True);p.add_argument('--baseline',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.captures,a.baseline,a.output)
