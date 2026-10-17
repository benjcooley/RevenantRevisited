"""Compile all three actual FX RGB565 blocks and compare original raster pixels."""
import random
import struct
import subprocess
import tempfile
import unittest
from pathlib import Path
from lit_software_probe import LitSoftwareFixture

ROOT = Path(__file__).resolve().parents[2]


class RGB565QuadShaderTests(unittest.TestCase):
    def test_actual_shader_arithmetic_matches_original_colored_additive_kernel(self):
        functions = []
        for backend in ('metal', 'glsl', 'hlsl'):
            source = (ROOT / ('src/shaders/fx.' + backend + '.h')).read_text()
            start = source.index('// Original RGB565 litadd:')
            body = source[start:source.index('    } else {', start)]
            # Only adapt vector type construction and shader input spelling to
            # Clang's native vector extension. Arithmetic/swizzles are unchanged.
            body = body.replace('in.color', 'color').replace('i.color', 'color').replace('v_color', 'color')
            body = body.replace('vec3', 'float3').replace('vec4', 'float4')
            body = body.replace('float3(', 'make3(').replace('float4(', 'make4(')
            functions.append('float4 ' + backend + '(float4 texel,float4 color){float4 c;\n' + body + '\nreturn c;}')
        driver = r'''
#include <cassert>
#include <cmath>
#include <cstdio>
typedef float float2 __attribute__((ext_vector_type(2)));
typedef float float3 __attribute__((ext_vector_type(3)));
typedef float float4 __attribute__((ext_vector_type(4)));
float3 make3(float x,float y,float z){return {x,y,z};}
float4 make4(float3 rgb,float a){return {rgb.x,rgb.y,rgb.z,a};}
float3 floor(float3 v){return {std::floor(v.x),std::floor(v.y),std::floor(v.z)};}
float2 floor(float2 v){return {std::floor(v.x),std::floor(v.y)};}
float3 clamp(float3 v,float lo,float hi){for(int i=0;i<3;++i)v[i]=std::fmin(hi,std::fmax(lo,v[i]));return v;}
FUNCTIONS
int main(){unsigned texel,r,g,b;
 while(scanf("%u %u %u %u",&texel,&r,&g,&b)==4){
  float4 t={(texel>>11)/31.f,((texel>>5)&63)/63.f,(texel&31)/31.f,1};
  float4 color={r/31.f,g/31.f,b/31.f,1};
  const auto a=metal(t,color),c=glsl(t,color),d=hlsl(t,color);
  for(int i=0;i<4;++i)assert(a[i]==c[i]&&a[i]==d[i]);
  printf("%ld %ld %ld\n",std::lround(a.r*31),std::lround(a.g*63),std::lround(a.b*31));
 }
}
'''.replace('FUNCTIONS', '\n'.join(functions))
        rng = random.Random(8)
        texels = [0, 0xffff, 0x9492, 0xef7d, 0xf800, 0x07e0, 0x001f] + [rng.randrange(65536) for _ in range(9)]
        rows = [(texel, light, (light*7+4)%32, (light*13+3)%32)
                for texel in texels for light in range(32)]
        rows += [(0x9492, 1, 4, 3), (0xef7d, 13, 15, 19)]
        with tempfile.TemporaryDirectory(prefix='rgb565-shader-') as folder:
            p = Path(folder); (p/'test.cpp').write_text(driver)
            compiled = subprocess.run(['clang++', '-std=c++17', '-O1', '-fsanitize=address,undefined',
                str(p/'test.cpp'), '-o', str(p/'test')], text=True, capture_output=True)
            self.assertEqual(compiled.returncode, 0, compiled.stderr)
            process = subprocess.run([str(p/'test')], input=''.join(' '.join(map(str,row))+'\n' for row in rows),
                                     text=True, capture_output=True)
            self.assertEqual(process.returncode, 0, process.stderr)
            shader = [tuple(map(int,line.split())) for line in process.stdout.splitlines()]
        f = LitSoftwareFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe', 32, 32)
        for address,value in ((0x5d7a28,1),(0x66818c,0),(0x5e8790,0)):
            f.vm.put_u32(address,value)
        f.vm.call(0x417d60,(16,1),this=0x65a57c)
        self.assertEqual(len(shader),len(rows))
        for row, expected in zip(rows,shader):
            texel,*rgb=row
            f.set_texture(2,2,struct.pack('<4H',*[texel]*4));f.clear()
            quad=[(x,y,100,*rgb,31,0,0)for x,y in ((8,8),(24,8),(24,24),(8,24))]
            output,_=f.draw(quad,[0,1,2,0,2,3],z_enabled=True,z_write=False)
            pixel=struct.unpack_from('<H',output,2*(16*32+16))[0]
            actual=(pixel>>11,(pixel>>5)&63,pixel&31)
            with self.subTest(texel=texel,rgb=rgb):self.assertEqual(actual,expected)
        # The previous normalized multiplication preserves red/blue here;
        # original additive output is pure green, explaining fspray's tint.
        self.assertEqual(shader[-2],(0,4,0))
        self.assertEqual(shader[-1],(12,28,16))


if __name__ == '__main__':
    unittest.main()
