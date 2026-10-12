"""Compare four explicit Puke raster policies, preserving source state and timing."""
import argparse, hashlib, json, re
from pathlib import Path
from PIL import Image, ImageDraw, ImageChops

def sha(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()
def run(native, baseline, captures, output):
    output.mkdir(parents=True,exist_ok=False)
    ref=json.loads(native.read_text());old=json.loads(baseline.read_text())
    films=[json.loads((p/'manifest.json').read_text()) for p in captures]
    assert len(films)==4 and len({m['binary_sha256'] for m in films})==1
    source_fields=0
    for policy,(folder,m) in enumerate(zip(captures,films)):
        assert m['status']=='pass' and m['sample_simulation_ticks']==list(range(2,34))
        log=(folder/'run.log').read_text()
        states=re.findall(r'\[puke-state\] tick=(\d+) alive=(\d+) life=(\d+) scale=(\S+) rng=(\d+) policy=(\d+)',log)
        assert len(states)==27
        for t,alive,life,scale,rng,flag in states:
            n=ref['cases'][int(t)]['state']
            assert (int(alive),int(life),int(rng),int(flag))==(int(n['alive']),n['life'],n['rng_draws'],policy)
            assert abs(float(scale)-n['scale'])<1e-7;source_fields+=4
        slots=re.findall(r'\[puke-slot\] tick=(\d+) slot=(\d+) start=(\d+) p=(\S+) v=(\S+)',log)
        assert len(slots)==27*25
        for tick,slot,start,position,velocity in slots:
            n=ref['cases'][int(tick)]['state'];i=int(slot)
            assert int(start)==n['starts'][i]
            for actual,expected in zip(map(float,(position+','+velocity).split(',')),n['positions'][i]+n['velocities'][i]):assert abs(actual-expected)<1e-6
            source_fields+=7
        for frame in m['frames']:assert sha(frame['path'])==frame['sha256']
    repeat=[]
    for previous,current in zip(old['frames'],films[0]['frames']):
        a=Image.open(previous['path']).convert('RGB');b=Image.open(current['path']).convert('RGB')
        assert a.size==b.size and a.tobytes()==b.tobytes()
        repeat.append(current['sha256'])
    ticks=[2,15,*range(16,27),30,32]
    panel=Image.new('RGB',(1600,len(ticks)*265));detail=Image.new('RGB',(1600,len(ticks)*345))
    pd=ImageDraw.Draw(panel);dd=ImageDraw.Draw(detail);rows=[]
    labels=['Original','Default','Grid only','Quant only','Grid + quant']
    for row,tick in enumerate(ticks):
        n=ref['cases'][tick];paths=[Path(n['image'])]+[Path(m['frames'][tick-2]['path'])for m in films]
        assert sha(paths[0])==n['image_sha256']
        originals=Image.open(paths[0]).convert('RGB');stats=[]
        for col,path in enumerate(paths):
            im=Image.open(path).convert('RGB');assert im.size==(640,480)
            difference=ImageChops.difference(originals,im)
            stats.append(dict(path=str(path),sha256=sha(path),pixels=sum(any(p)for p in im.getdata()),different_pixels=sum(any(p)for p in difference.getdata()),absolute_rgb_error=sum(sum(p)for p in difference.getdata()),bounds=im.getbbox()))
            panel.paste(im.resize((320,240)),(col*320,row*265+25));pd.text((col*320+4,row*265+4),f'{labels[col]} tick{tick}',fill='white')
            detail.paste(im.crop((300,210,380,290)).resize((320,320),Image.Resampling.NEAREST),(col*320,row*345+25));dd.text((col*320+4,row*345+4),f'{labels[col]} tick{tick} fixed crop x4',fill='white')
        rows.append(dict(tick=tick,policies=stats))
    panel.save(output/'contact.png');detail.save(output/'detail.png')
    report=dict(status='pass_diagnostic',accepted=False,visual_pass=False,rows=rows,source_fields_checked=source_fields,
                baseline_frames_identical=len(repeat),binary_sha256=films[0]['binary_sha256'],
                native_sha256=sha(native),baseline_sha256=sha(baseline),captures={str(p):sha(p/'manifest.json')for p in captures},
                contact_sha256=sha(output/'contact.png'),detail_sha256=sha(output/'detail.png'),tool_sha256=sha(__file__),
                limits='All policies use identical phase/geometry/state/RNG/camera/lighting. Flags only change existing explicit helper vertex-grid and ARGB4444 source contribution. Late grid removes extra pixels but loses native tick24 one-pixel sample; no exact raster claim or automatic appearance acceptance.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print('PASS',source_fields,'source fields; baseline',len(repeat))
    print([(r['tick'],[x['pixels']for x in r['policies']])for r in rows])

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--native',type=Path,required=True);p.add_argument('--baseline',type=Path,required=True);p.add_argument('--captures',type=Path,nargs=4,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.baseline,a.captures,a.output)
