#!/usr/bin/env python3
"""Actual Metal dynamic RGBA5 interpolation/depth vs original literal controls."""
import argparse,json,hashlib
from pathlib import Path
from PIL import Image

def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def review(capture,native,out):
 m=json.load(open(capture/'manifest.json'));n=json.load(open(native/'manifest.json'));assert m['status']==n['status']=='pass';assert m['scenario']['effect']=='TPrelitQuadRGBA5_DIAGNOSTIC';im=Image.open(m['frames'][0]['path']).convert('RGB');rows=[];residuals=[]
 for case in n['cases']:
  for sample in case['samples']:
   p=sample['pixel'];word=sample['rgb565'];expected=[round(((word>>shift)&mask)*255/mask)for shift,mask in((11,31),(5,63),(0,31))];actual=im.getpixel(tuple(p));row=dict(case=case['name'],pixel=p,actual_rgb8=actual,native_rgb565=word,expected_rgb8=expected)
   if max(abs(a-b)for a,b in zip(actual,expected))>1:
    # Explicit retained original fixed8-vs-GPU-affine interpolation limits.
    # Do not hide other differences behind a broad image tolerance.
    known={('alpha_gradient',(96,56)):((107,105,107),(99,97,99)),('combined_gradient',(456,56)):((82,40,58),(74,40,58))}
    assert (tuple(actual),tuple(expected))==known.get((case['name'],tuple(p))),(row,'unexpected literal mismatch')
    residuals.append(row)
   rows.append(row)
 for sample in n['depth']:
  expected=[round(((sample['rgb565']>>shift)&mask)*255/mask)for shift,mask in((11,31),(5,63),(0,31))];actual=im.getpixel(tuple(sample['pixel']));assert max(abs(a-b)for a,b in zip(actual,expected))<=1,(sample,actual,expected);rows.append(dict(case='flat_depth',pixel=sample['pixel'],actual_rgb8=actual,native_rgb565=sample['rgb565']))
 corrected=im.getpixel((56,366));default=im.getpixel((236,366));expected=[round(v*255/m)for v,m in zip((2,5,2),(31,63,31))];assert max(abs(a-b)for a,b in zip(corrected,expected))<=1 and all(a<b for a,b in zip(corrected,default)),(corrected,default,expected)
 assert len(residuals)==2
 report=dict(status='pass_with_declared_interpolation_residual',exact_varying_samples=22,total_varying_samples=24,interpolation_residuals=residuals,cases=rows,negative_flag_control=dict(native_policy_rgb8=corrected,default_rgb8=default,expected_rgb8=expected),binary_sha256=m['binary_sha256'],image=m['frames'][0]['path'],image_sha256=sha(m['frames'][0]['path']),capture_manifest_sha256=sha(capture/'manifest.json'),native_manifest_sha256=sha(native/'manifest.json'),scope='24 actual varying RGB5/A5 samples,3original flat-depth controls, explicit flagfalse fractional brightness negativecontrol. Constant source contribution and integer sampling proven; two listed samples differ because original scanline slopes use8 fractional bits and GPU interpolation is precise. Destination RGB565 rounding remains outside GPU fixed-function blend.')
 out.write_text(json.dumps(report,indent=2)+'\n');print('PASS22/24 exact gradient samples,2 declared fixed8 interpolation residuals,3 exact depth controls and negativeflag brightness control');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--capture',type=Path,required=True);p.add_argument('--native',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();review(a.capture,a.native,a.output)
