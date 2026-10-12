"""Freeze literal-tick full named Essence Drain spell original/Metal pairs."""
import argparse,json,hashlib,re
from pathlib import Path
from PIL import Image,ImageDraw

def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def run(native,metal,out):
 out.mkdir(parents=True,exist_ok=False);n=json.loads((native/'manifest.json').read_text());m=json.loads((metal/'manifest.json').read_text());assert n['variant']=={'name':'Essence Drain','maximum_damage':115};assert m['status']=='pass'and m['exit_code']==0 and len(m['frames'])==100
 log=(metal/'run.log').read_text();assert 'variant=Essence_Drain min=110 max=115'in log
 ticks=(1,20,21,40,70,90,91,100);whole=Image.new('RGB',(1280,505*len(ticks)));crop=Image.new('RGB',(1040,485*len(ticks)));a=ImageDraw.Draw(whole);b=ImageDraw.Draw(crop);pairs=[];rectangle=(600,520,860,750)
 for row,tick in enumerate(ticks):
  nr=next(r for r in n['records']if r['tick']==tick);p=Path(nr['image']);q=Path(m['frames'][tick-1]['path']);assert sha(p)==nr['image_sha256']and sha(q)==m['frames'][tick-1]['sha256']
  images=[Image.open(x).convert('RGB')for x in(p,q)]
  assert all(im.size==(1280,960)for im in images)
  for col,(im,label)in enumerate(zip(images,('Original','Actual Metal'))):
   whole.paste(im.resize((640,480)),(640*col,row*505+25));a.text((640*col+4,row*505+5),f'{label} Essence Drain SPELL tick{tick}',fill='white');crop.paste(im.crop(rectangle).resize((520,460),Image.Resampling.NEAREST),(col*520,row*485+25));b.text((col*520+4,row*485+5),f'{label} tick{tick}, literal2x crop',fill='white')
  if tick>=91:assert images[0].getbbox() is None and images[1].getbbox() is None
  pairs.append({'tick':tick,'native':str(p),'native_sha256':sha(p),'metal':str(q),'metal_sha256':sha(q),'draws':len(nr['draws']),'native_pixels':nr['pixels']})
 whole.save(out/'whole.png');crop.save(out/'detail.png');r={'status':'awaiting_independent_spell_review','accepted':False,'spell':'Essence Drain','effect':'ManaDrain','type_id':'0xad92bc13','prototype_type_not_aliased':'0xdefeca7e','pairs':pairs,'binary_sha256':m['binary_sha256'],'native_manifest_sha256':sha(native/'manifest.json'),'metal_manifest_sha256':sha(metal/'manifest.json'),'whole_sha256':sha(out/'whole.png'),'detail_sha256':sha(out/'detail.png'),'scenario':m['scenario'],'pixel_summary':{k:v for k,v in m['pixel_summary'].items()if k!='pixel_sha256'},'scope':'Genuine named Essence Drain variant110..115 executes original ManaDrain actor/spell APIs, complete owner/animator/render/RNG through100; full selected software raster vs actual100-frame Metal. Exact original camera3185,3185 removed by declared source display-origin0,0,16, screen640,640. No phase, geometry, lighting or brightness fit. Separate supplementary spell recipe, no Worm type substitution or new row credit. Natural map/moving actor/audio/hardware/exactpixel gates open.'};(out/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],r['pixel_summary']);return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('native',type=Path);p.add_argument('metal',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.metal,a.output)
