"""Freeze same-tick loaded Drip/managed Ripple native and actual Metal panels."""
import argparse, hashlib, json, shutil
from pathlib import Path
from PIL import Image, ImageDraw

def pin(path):
    path=Path(path).resolve()
    return dict(path=str(path),sha256=hashlib.sha256(path.read_bytes()).hexdigest())

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--lab',type=Path,required=True);ap.add_argument('--binary',type=Path,required=True);args=ap.parse_args()
    p=args.lab;out=p/'drip-review-final';out.mkdir(exist_ok=True)
    npath=p/'drip-loaded-corrected/manifest.json';mpath=p/'drip-metal-native/manifest.json'
    n=json.loads(npath.read_text());m=json.loads(mpath.read_text())
    ns={r['tick']:r for r in n['records'] if 'image' in r};ms=dict(zip(m['sample_simulation_ticks'],m['frames']))
    ticks=sorted(ns.keys()&ms.keys());assert ticks==[1,13,25,37,45,49,61,73,85,97,109,121,133,141]
    # Fixed same crop and isotropic nearest scale; all raw whole-view pairs retained.
    crop=(592,520,688,660);scale=4;rows=[];files=[]
    whole=Image.new('RGB',(640, (len(ticks)+1)//2*264),(24,24,24))
    detail=Image.new('RGB',(768,len(ticks)*584),(24,24,24))
    for i,t in enumerate(ticks):
        a_path=p/'drip-loaded-corrected'/ns[t]['image'];b_path=Path(ms[t]['path'])
        assert pin(a_path)['sha256']==ns[t]['sha256'] and pin(b_path)['sha256']==ms[t]['sha256']
        a=Image.open(a_path).convert('RGB');b=Image.open(b_path).convert('RGB');assert a.size==b.size==(1280,960)
        pair=Image.new('RGB',(2560,960));pair.paste(a,(0,0));pair.paste(b,(1280,0));pp=out/f'pair-{t:03}.png';pair.save(pp);files.append(pin(pp))
        x=(i%2)*320;y=(i//2)*264
        whole.paste(pair.resize((320,120),Image.Resampling.NEAREST),(x,y+24))
        ImageDraw.Draw(whole).text((x+3,y+4),f'tick {t}: original / Metal',fill='white')
        for k,im in enumerate((a,b)):
            detail.paste(im.crop(crop).resize((384,560),Image.Resampling.NEAREST),(k*384,i*584+24))
        ImageDraw.Draw(detail).text((3,i*584+4),f'tick {t}: original / Metal; identical 4x crop',fill='white')
        rows.append(dict(tick=t,native=pin(a_path),metal=pin(b_path),native_bbox=a.getbbox(),metal_bbox=b.getbbox(),pair=pin(pp)))
    for name,im in [('whole-life.png',whole),('detail.png',detail)]:im.save(out/name);files.append(pin(out/name))
    # Small pages remain readable without viewer downsampling the full strip.
    for start in range(0,len(ticks),4):
        name=out/f'detail-{start//4+1}.png';detail.crop((0,start*584,768,min(len(ticks),start+4)*584)).save(name);files.append(pin(name))
    binary=out/'Revenant-captured'
    if not binary.exists():shutil.copy2(args.binary,binary)
    assert pin(binary)['sha256']==m['binary_sha256']
    proofs=[p/'drip-loaded-repeat/replay.json',p/'drip-loaded-corrected/source-geometry-comparison.json',p/'drip-metal-common-repeat/pixel-regression.json']
    report=dict(status='ready-for-independent-review',effect='Drip',type_id='0x5975abde',parameters=[20,128,1],drain_before_tick=121,
        scope='Genuine loaded original parent and standalone public-factory Ripple children after observed empty-map allocation failures; explicitly managed child schedule.',
        configuration=dict(viewport=[1280,960],center=[640,640],source=[0,0,0],native_camera=[0,0,0],ambient=[38,38,38],directional=[1,1,1],simulation_hz=24,native_domain=True,detail_crop=crop,detail_scale=scale),
        limits=['Natural sector residency/map insertion not reproduced.','Silent port omits original audio RNG; period1 physics is deterministic; arbitrary periods and splash recursion not accepted here.','Residual one-pixel edges, sampling, RGB565 framebuffer quantization and low-intensity hue remain; no exact pixels/device claim.','Period changes through public SetParams to1000000 at121 for drain; ambient parent is not artificially killed.'],
        native_manifest=pin(npath),metal_manifest=pin(mpath),binary=pin(binary),proofs=[pin(f) for f in proofs],pairs=rows,panels=files)
    (out/'pair-manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(pin(out/'pair-manifest.json')))
if __name__=='__main__':main()
