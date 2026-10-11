#!/usr/bin/env python3
"""Actual GPU literal prelitRGBA5/ARGB4444/depth controls, defaults negative check."""
import argparse,json,hashlib
from pathlib import Path
from PIL import Image
from test_argb4444 import blend565

def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def rgb(word):return tuple(round(((word>>shift)&mask)*255/mask)for shift,mask in((11,31),(5,63),(0,31)))
def load(folder):
 m=json.loads((folder/'manifest.json').read_text());assert m['status']=='pass';p=Path(m['frames'][0]['path']);assert sha(p)==m['frames'][0]['sha256'];return m,Image.open(p).convert('RGB')
def run(prelit,argb,native,previous,regression,out):
 pm,pi=load(prelit);am,ai=load(argb);rm,ri=load(regression);old,oi=load(previous);assert pm['binary_sha256']==am['binary_sha256']==rm['binary_sha256'];assert ri.tobytes()==oi.tobytes(),'Existing default diagnostic image changed'
 cases=[];n=json.loads((native/'manifest.json').read_text());assert n['status']=='pass'
 def check(label,position,expected):
  actual=pi.getpixel(position);assert max(abs(a-b)for a,b in zip(actual,rgb(expected)))<=1,(label,actual,rgb(expected));cases.append(dict(label=label,pixel=position,actual=actual,native_rgb565=expected))
 for i,c in enumerate(((8,2,3),(31,31,31),(0,31,0),(31,0,0))):
  row=next(r for r in n['colors']if r['rgb5']==list(c) and r['alpha5']==31 and r['background']==0);check('opaque'+str(c),(140+120*i,90),row['rgb565'])
 assert pi.getpixel((140,90))!=pi.getpixel((140,180)), 'Prelit flag false must select existing lighting path'
 for i,a in enumerate((0,7,15,30)):
  row=next(r for r in n['colors']if r['rgb5']==[8,2,3]and r['alpha5']==a and r['background']==0);check('alpha'+str(a),(140+120*i,270),row['rgb565'])
 for x,row in zip((180,320,460),n['depth']):check('depth'+str(x),(x,390),row['rgb565'])
 ac=[]
 for i,word in enumerate((0x0323,0x0322,0x1433,0x0755,0x0856,0x0fff,0xffff)):
  expected=rgb(blend565(word,0));actual=ai.getpixel((92+72*i,300));default=ai.getpixel((92+72*i,180));assert max(abs(a-b)for a,b in zip(actual,expected))<=1,(hex(word),actual,expected)
  if word in(0x0323,0x0322):assert any(default) and not any(actual)
  ac.append(dict(texel=hex(word),default=default,quantized=actual,expected=expected))
 report=dict(status='pass',prelit=cases,argb=ac,existing_diagnostic_byte_identical=True,binary_sha256=pm['binary_sha256'],inputs={str(p):sha(p/'manifest.json')for p in(prelit,argb,native,previous,regression)},scope='Actual same-binary GPU versus original prelitRGBA5 and literal ARGB source terms on black; near/far/equal-depth controls; negative prelit flag and existing complete ARGB diagnostic image unchanged. General destination RGB565 rounding is open.')
 out.write_text(json.dumps(report,indent=2)+'\n');print(report['status'],len(cases),len(ac),report['binary_sha256'])
if __name__=='__main__':
 p=argparse.ArgumentParser()
 for k in('prelit','argb','native','previous','regression','output'):p.add_argument('--'+k,type=Path,required=True)
 a=p.parse_args();run(a.prelit,a.argb,a.native,a.previous,a.regression,a.output)
