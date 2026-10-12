"""Compile the unchanged production decoder; exhaustively check 16-bit formats."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
from character_native_preflight import ROOT


def run(output):
    output.mkdir(parents=True, exist_ok=False)
    text = (ROOT / 'src/3dimage.cpp').read_text()
    source = text[text.index('static inline uint8_t ExpandBitsTo8('):text.index('static uint64_t I3DTextureAssetKey(')]
    harness = r'''
#include <cstdint>
#include <cstring>
#include <vector>
#include <cassert>
#include "retailmeshlighting.h"
struct PixelFormat { uint32_t dwFlags=65,dwRGBBitCount=16,dwRBitMask=0,dwGBitMask=0,dwBBitMask=0,dwRGBAlphaBitMask=0; };
struct SSurfaceDesc { uint32_t width=65536,height=1; int32_t pitch=131072; PixelFormat pixelFormat; };
'''+source+r'''
int main() {
 std::vector<uint16_t> words(65536); for(unsigned i=0;i<words.size();++i)words[i]=i;
 std::vector<uint8_t> rgba; SSurfaceDesc s;
 for(unsigned mode=0;mode<3;++mode) {
  auto& f=s.pixelFormat;
  f.dwRBitMask=mode==0?0x7c00:mode==1?0xf00:0xf800;
  f.dwGBitMask=mode==0?0x3e0:mode==1?0xf0:0x7e0;
  f.dwBBitMask=mode==1?0xf:0x1f; f.dwRGBAlphaBitMask=mode==0?0x8000:mode==1?0xf000:0;
  assert(RetailMeshTextureLighting(f)==(mode==0?2:mode==1?3:1));
  assert(DecodeTextureFrameRGBA(&s,words.data(),nullptr,rgba));
  for(unsigned w=0;w<65536;++w) {
   unsigned r,g,b,a;
   if(mode==0){r=(((w>>10)&31)*255+15)/31;g=(((w>>5)&31)*255+15)/31;b=((w&31)*255+15)/31;a=(w>>15)*255;}
   else if(mode==1){r=((w>>8)&15)*17;g=((w>>4)&15)*17;b=(w&15)*17;a=(w>>12)*17;}
   else{r=(((w>>11)&31)*255+15)/31;g=(((w>>5)&63)*255+31)/63;b=((w&31)*255+15)/31;a=255;}
   assert(rgba[w*4]==r&&rgba[w*4+1]==g&&rgba[w*4+2]==b&&rgba[w*4+3]==a);
  }
 }
}
'''
    cpp=output/'decode.cpp';cpp.write_text(harness);binary=output/'decode'
    command=['clang++','-std=c++17','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(cpp),'-o',str(binary)]
    subprocess.run(command,check=True);subprocess.run([str(binary)],check=True)
    report=dict(status='pass',words_per_format=65536,formats=['ARGB1555','ARGB4444','RGB565'],channel_assertions=786432,source_span_sha256=hashlib.sha256(source.encode()).hexdigest(),lighting_header_sha256=hashlib.sha256((ROOT/'src/retailmeshlighting.h').read_bytes()).hexdigest(),compile_command=command,scope='Actual extracted production decoder and actual policy selector only; no GPU/render/device equivalence claim.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--output',required=True,type=Path);a=p.parse_args();run(a.output)
