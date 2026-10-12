#!/usr/bin/env python3
"""Assert actual Metal literal swatches against original ARGB4444 source terms."""
import argparse,json,hashlib
from pathlib import Path
from PIL import Image
from test_argb4444 import blend565,source565

def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def review(capture,native,output):
 m=json.loads((capture/'manifest.json').read_text());n=json.loads((native/'manifest.json').read_text());assert m['status']==n['status']=='pass'
 assert m['scenario']['effect']=='TArgbSourceContribution_DIAGNOSTIC'
 im=Image.open(m['frames'][0]['path']).convert('RGB');rows=[]
 for col,word in enumerate((0x0204,0x0305,0x0205,0x0304,0x0306,0x0fff,0xffff)):
  default=im.getpixel((92+72*col,180));quantized=im.getpixel((92+72*col,300));packed=blend565(word,0)
  expected=tuple(round(((packed>>s)&mask)*255/mask)for s,mask in((11,31),(5,63),(0,31)))
  # 8-bit output conversion can differ by one from nearest rounding.
  assert max(abs(a-b)for a,b in zip(quantized,expected))<=1,(hex(word),quantized,expected)
  if packed==0:assert quantized==(0,0,0) and any(default),(hex(word),default,quantized)
  else:assert any(quantized)
  assert any(c['texel']==hex(word) and int(c['native_rgb565'],16)==packed for c in n['cases'])
  rows.append(dict(texel=hex(word),default_rgb8=default,quantized_rgb8=quantized,original_rgb565=hex(packed),expected_rgb8=expected))
 assert rows[-1]['default_rgb8']==rows[-1]['quantized_rgb8']
 result=dict(status='pass',cases=rows,binary_sha256=m['binary_sha256'],capture_manifest_sha256=sha(capture/'manifest.json'),native_manifest_sha256=sha(native/'manifest.json'),image=m['frames'][0]['path'],image_sha256=sha(Path(m['frames'][0]['path'])),scope='Same actual GPU frame: top flag false, bottom true, literal source words on black. Dominant authored0204 and four other subquantum words vanish only under quantization; alpha0 white remains visible. Destination rounding is not asserted.')
 output.write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--capture',type=Path,required=True);p.add_argument('--native',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();review(a.capture,a.native,a.output)
