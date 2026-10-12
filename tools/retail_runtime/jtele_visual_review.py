#!/usr/bin/env python3
"""Unchanged whole finite Jtele/Jteled native/Metal phases, no camera or color fit."""
import argparse,json
from pathlib import Path
from PIL import Image,ImageDraw
from speed_profile_contract import sha

def digest(p):return sha(Path(p).read_bytes())
def review(evidence,output):
 output.mkdir(parents=True,exist_ok=False);native=json.loads((evidence/'native-final/manifest.json').read_text());cases=[]
 for n in native['cases']:
  name=n['name'];metal=json.loads((evidence/('metal-final-'+name.capitalize())/'manifest.json').read_text());assert metal['status']=='pass'and metal['scenario']['frames']==53 and metal['scenario']['native_domain'];assert metal['pixel_summary']['nonblack_frames']==48
  ticks=(2,6,16,25,40,48,49,50,53);whole=Image.new('RGB',(1280,len(ticks)*505));detail=Image.new('RGB',(960,len(ticks)*485));w=ImageDraw.Draw(whole);d=ImageDraw.Draw(detail);pairs=[]
  for row,tick in enumerate(ticks):
   r=n['rows'][tick-1];m=metal['frames'][tick-2];paths=[Path(r['image']),Path(m['path'])];hashes=[r['image_sha256'],m['sha256']];assert list(map(digest,paths))==hashes;images=[Image.open(p).convert('RGB')for p in paths];blank=tick>=50;assert all((im.getbbox()is None)==blank for im in images)
   pairs.append(dict(tick=tick,frame=r['frame'],native_draws=len(r['draws']),native_owner_flags=r['owner_flags'],native_rng_after=r['rng_after'],images=list(map(str,paths)),sha256=hashes,visible_bbox=[im.getbbox()for im in images],blank_tail=blank,software_rejected_beam_triangles=[dict(object=p['object'],count=p['oversized_triangles'],bounds=p.get('geometry_bounds'))for p in r['draws']if p.get('oversized_triangles')]))
   for col,(im,label)in enumerate(zip(images,('Original','Actual Metal'))):whole.paste(im.resize((640,480)),(col*640,row*505+25));w.text((col*640+4,row*505+5),f'{label} {name} tick{tick}',fill='white');detail.paste(im.crop((400,240,880,700)),(col*480,row*485+25));d.text((col*480+4,row*485+5),f'{label} same actual-size crop tick{tick}',fill='white')
  whole.save(output/(name+'-whole.png'));detail.save(output/(name+'-fixed-crop.png'));cases.append(dict(name=name,type_id=n['type_id'],asset_sha256=n['asset_sha256'],binary_sha256=metal['binary_sha256'],pairs=pairs,whole_sha256=digest(output/(name+'-whole.png')),fixed_crop_sha256=digest(output/(name+'-fixed-crop.png')),actual_movie_frames=53,actual_nonblack_frames=48,actual_distinct_frames=49))
 report=dict(status='awaiting_independent_review',accepted=False,cases=cases,appearance_fitted=False,scope='Literal exactownJtele/Jteled full originaldefaultowner/Initialize/Pulse/Animate/Render/software vsactualMetal ownprofile/decoder/cached3emitters/PSRuntime/base6meshes/scrollUV. Declared1280x960 owner/camera0 ambient38dir1/source32white/initialblend1/quality0/seed1 andflatground0 bounds. Source softwarebounds640x480 appliedonlyStartupVfxNativeDomain&&UsesSW atauthoredtriangles; ordinarygeometrykept, originalconstants-onlywidening provesbeamrejectionseparately. Wholevisiblehalo/stars/trails andrealfinitekill50blanktail, no geometry/phase/camera/brightness/colorfit. Rawrejectedbeambecomesunrenderableinthisconfiguration; itsoutsideviewportboundisnotclaimoffullordinary/devicebeamcoverage. Jteled3strictfloatZresiduals4.58e-5,nodepth/color/pixel/natural/deviceequality claimed.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status']);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--evidence',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();review(a.evidence,a.output)
