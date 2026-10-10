#!/usr/bin/env python3
"""Independent production buff keys/base matrices vs original normal-lit bases.

Speed/Quicksilver emitter matrices are independently produced too. The complete
particle composite and modern GPU projection remain separate; no native pose
is supplied to the compiled production matrix builder.
"""
import argparse
import json
from pathlib import Path
import struct
import subprocess
import zipfile
from unicorn.x86_const import UC_X86_REG_EBX, UC_X86_REG_ECX, UC_X86_REG_ESP
from firecone_state_probe import function, sha, ROOT
from speed_controller_preflight import NativeSpeed, read_asset
from speed_profile_contract import PRELUDE, CLASS
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png

PROFILES = (
    ('speed', 'speed', 1, 80, '65c45c03bcf3cd5cfe549232f88fb18550a30720c5b9500d0cfd5ef8154fce42'),
    ('quicksilver', 'quicksilver', 1, 80, '07c6a0a48c32d891ccd29367b12a882a4244a3eea004e4b06a7c2dee7aa609b4'),
    ('Fmastery', 'fmaster', 2, 16, '9d94bf16e45a88366946811775747063b905be3406d44b0a9a47e26e5b51878d'))


def compile_matrices(output, asset, name):
    source = (ROOT / 'src/3dimage.cpp').read_text()
    body = (ROOT / 'src/3dimagebody.h').read_text()
    render = (ROOT / 'src/render3d_types.h').read_text()
    prefixes = ('SMALLKEY', 'ANIKEY32_', 'ANIFLAG32_', 'ANICODE32_', 'ANIKEY_POSSCALE', 'ANIKEY_ANGSCALE', 'I3D_ANIKEY32')
    constants = '\n'.join(line for line in body.splitlines() if line.startswith('#define ') and any(line.startswith('#define ' + p) for p in prefixes))
    records = '\n'.join(function(text, signature) + ';' for text, signature in (
        (body, 'struct SAniKey32\n'), (body, 'struct S3DFace\n'),
        (render, 'struct S3DVertex\n'), (render, 'struct SRenderColor\n'), (body, 'struct S3DMaterial\n')))
    prelude = PRELUDE.replace('struct hmm_vec3 {float X=0,Y=0,Z=0;};',
        '#include "speedauthoredmatrix.h"\n#undef min\n#undef max')
    model = CLASS.replace(' bool ValidateRetailSpeedPartSysProfile();', '''
 bool HasRetailSpeedFamilyPartSysProfile(uint32_t)const{return true;}
 void SetPrevState(int,int){}
 bool GetAniKey(int o,int s,int f,hmm_vec3&p,hmm_vec3&r,hmm_vec3&z){return GetUninterpolatedAniKey(o,s,f,p,r,z);}
 bool ValidateRetailSpeedPartSysProfile();''')
    fixture = r'''
constexpr uint32_t OBJ3D_GOLD_CAMERA_FACING=0x20000000;
struct Owner{int frame=0;uint32_t id=0;hmm_mat4 world;
 Owner(){MtxClear(&world);}int GetState(){return 0;}uint32_t ObjId(){return id;}
 int GetFrame(){return frame;}int GetPrevState(){return -1;}int GetPrevFrame(){return 0;}
 Owner&Transform(){return *this;}const hmm_mat4&Matrix()const{return world;}};
struct Bone{uint32_t flags=OBJ3D_GOLD_CAMERA_FACING;};
struct Bones{Bone b[3];bool Used(int i){return i>=0&&i<3;}Bone*operator[](int i){return &b[i];}};
struct T3DAnimator{Owner*inst;T3DImagery*im;Bones animobjs;
 T3DImagery*Get3DImagery(){return im;}int GetFrame(){return inst->frame;}
 bool IsObjectEnabled(int){return true;}
 bool SpeedBaseMeshBlend(int,uint32_t&v){v=80;return true;}
 bool FmasteryBaseMeshBlend(int,uint32_t&v){v=16;return true;}
 bool SpeedEmitterLocalMatrix(hmm_mat4&,hmm_vec3*,hmm_vec3*);
 bool SpeedBaseMeshWorldMatrix(hmm_mat4&,const hmm_mat4* = nullptr);
 bool FmasteryBaseMeshWorldMatrix(hmm_mat4&,const hmm_mat4* = nullptr);};
'''
    signatures = ('static inline int32_t SkipAniKey32(', 'static inline void GetAniKey32(',
        'bool T3DImagery::GetUninterpolatedAniKey(', 'bool T3DAnimator::SpeedEmitterLocalMatrix(',
        'bool T3DAnimator::SpeedBaseMeshWorldMatrix(', 'bool T3DAnimator::FmasteryBaseMeshWorldMatrix(')
    methods = {s: function(source, s) for s in signatures}
    main = r'''
int main(int argc,char**argv){if(argc!=2)return 2;std::ifstream f(argv[1],std::ios::binary);
 T3DImagery im;Owner owner;owner.id=ID;im.PROFILE=true;im.flags=FLAGS;im.nstates=1;im.frames=FRAMES;
 for(int i=0;i<3;++i){uint32_t n;f.read((char*)&n,4);im.keys[i].resize(n);f.read((char*)im.keys[i].data(),n*4);
 im.keycount[i]=n;im.keyptr[i]=im.keys[i].data();im.objects[i].numanikeys=&im.keycount[i];im.objects[i].anikeys=&im.keyptr[i];}
 T3DAnimator a;a.inst=&owner;a.im=&im;
 for(int frame=0;frame<im.frames;++frame){owner.frame=frame;hmm_mat4 base{},emitter{};hmm_vec3 p{},s{};
 if(!(FMASTER?a.FmasteryBaseMeshWorldMatrix(base):a.SpeedBaseMeshWorldMatrix(base)))return 3;
 if(!FMASTER&&!a.SpeedEmitterLocalMatrix(emitter,&p,&s))return 4;
 printf("%d",frame);for(const auto&m:{base,emitter})for(int r=0;r<4;++r)for(int c=0;c<4;++c)printf(" %.9g",m.Elements[r][c]);
 printf(" %.9g %.9g %.9g %.9g %.9g %.9g\n",p.X,p.Y,p.Z,s.X,s.Y,s.Z);}
}
'''.replace('FMASTER', 'true' if name == 'Fmastery' else 'false')
    main = main.replace('ID', {'speed':'0xad92bd36u', 'quicksilver':'0xad92bd35u', 'Fmastery':'0xb0e024dfu'}[name])
    main = main.replace('PROFILE', {'speed':'speed_partsys_profile', 'quicksilver':'quicksilver_partsys_profile', 'Fmastery':'fmastery_partsys_profile'}[name])
    main = main.replace('FLAGS', str(asset['flags'])).replace('FRAMES', str(asset['state0_frames']))
    inputs = output / 'keys.bin'
    inputs.write_bytes(b''.join(struct.pack('<I', len(o['states'][0]['keys'])) + struct.pack('<' + 'I'*len(o['states'][0]['keys']), *o['states'][0]['keys']) for o in asset['objects']))
    cpp = output / 'matrix-driver.cpp'
    cpp.write_text('\n'.join((prelude, constants, records, model, fixture, '\n'.join(methods.values()), main)))
    binary = output / 'matrix-driver'
    command = ['clang++', '-std=c++17', '-O1', '-fsanitize=address,undefined', '-I', str(ROOT / 'thirdparty/handmademath'), '-iquote', str(ROOT / 'src'), str(cpp), str(ROOT / 'src/math3d.cpp'), '-o', str(binary)]
    result = subprocess.run(command, capture_output=True, text=True)
    (output / 'compile.log').write_text(result.stdout + result.stderr)
    if result.returncode:
        raise RuntimeError('Actual buff matrix/decoder compile failed; see compile.log')
    result = subprocess.run([str(binary), str(inputs)], capture_output=True, text=True)
    if result.returncode or result.stderr:
        raise RuntimeError('Actual buff matrix/decoder execution or sanitizer failed: ' + result.stderr)
    (output / 'production-matrices.txt').write_text(result.stdout)
    matrices = {}
    for line in result.stdout.splitlines():
        values = line.split()
        floats = [struct.unpack('<f', struct.pack('<f', float(x)))[0] for x in values[1:]]
        matrices[int(values[0])] = dict(base=floats[:16], emitter=floats[16:32], position=floats[32:35], scale=floats[35:38])
    return matrices, dict(command=command, source_sha256={k:sha(v.encode()) for k,v in methods.items()},
        driver_sha256=sha(cpp.read_bytes()), binary_sha256=sha(binary.read_bytes()), keys_sha256=sha(inputs.read_bytes()))


def punctuation(f):
    v = f.vm
    for index, obj in enumerate(f.asset['objects']):
        name = f.put_string(obj['name'])
        sp = v.STACK + v.STACK_SIZE - 0x1000
        v.put_u32(sp + 0x1c, name)
        v.uc.reg_write(UC_X86_REG_ESP, sp)
        v.uc.reg_write(UC_X86_REG_ECX, name)
        v.uc.reg_write(UC_X86_REG_EBX, f.animobjs[index])
        v.uc.emu_start(0x409f61, 0x409fe0, count=10000)


def run(executable, output, profiles=PROFILES):
    output.mkdir(parents=True, exist_ok=False)
    cases = []
    for name, asset_name, base_index, blend, pinned in profiles:
        destination = output / name
        destination.mkdir()
        with zipfile.ZipFile(ROOT / 'data/imagery.rvi') as z:
            members = {n.lower():n for n in z.namelist()}
            data = z.read(members['imagery/magic/' + asset_name + '.i3d'])
        if sha(data) != pinned:
            raise ValueError('Exact shipped buff changed')
        asset = read_asset(data)
        asset['name'] = name
        matrices, compiled = compile_matrices(destination, asset, name)
        native = NativeSpeed(executable, asset)
        punctuation(native)
        software = LitSoftwareFixture(executable)
        software.lighting((38, 38, 38), (1, 1, 1))
        v = software.vm
        for address, value in ((0x5d7a28, 1), (0x66818c, 0), (0x5e91c0, 0), (0x5e8740, 0), (0x5c61ac, 1), (0x5e8790, 0)):
            v.put_u32(address, value)
        v.call(0x417d60, (blend, 1), this=0x65a57c)
        obj = asset['objects'][base_index]
        vertices = list(struct.iter_unpack('<8f', bytes.fromhex(obj['vertex_hex'])))
        runs = obj['texture_runs']
        slot = next(t for t in range(asset['textures'] + 1) if runs[2*t+1])
        indices = list(struct.unpack_from('<6H', bytes.fromhex(asset['face_hex']), runs[2*slot] * 6))
        u = lambda p: struct.unpack_from('<I', data, p)[0]
        r = lambda p: p + u(p)
        descriptor = r(asset['body_offset'] + 40) + (slot - 1) * 120
        height, width = struct.unpack_from('<2I', data, descriptor + 8)
        bits = r(r(descriptor + 108))
        software.set_texture(width, height, data[bits:bits + width*height*2])
        for frame, candidate in matrices.items():
            vm = native.vm
            if vm.call(0x40a420, (native.animobjs[base_index], 0, frame, 0, 0), this=native.imagery) != 1:
                raise AssertionError('Native authored base rejected')
            matrix = list(struct.unpack('<16f', vm.uc.mem_read(native.animobjs[base_index] + 0x58, 64)))
            difference = max(abs(a-b) for a,b in zip(matrix, candidate['base']))
            emitter_difference = max(abs(a-b) for a,b in zip(matrix, candidate['emitter'])) if name != 'Fmastery' else None
            images, depths, colors = [], [], []
            for label, world in (('native', matrix), ('production', candidate['base'])):
                software.clear()
                image, depth, transformed = software.draw_authored(vertices, indices, world, z_enabled=blend != 80)
                images.append(image); depths.append(depth); colors.append([list(p[3:7]) for p in transformed])
                if frame in (0, 5, 15, 29):
                    save_rgb565_png(destination / f'{label}-{frame:02d}.png', image, 512, 512)
            # Diagnostic only: the actual map packs the common-world owner's
            #1.5 Z stretch into this basis. Original lighting on that basis
            # demonstrates why local-matrix parity alone cannot certify Metal.
            stretched = list(candidate['base'])
            for row in range(4): stretched[row*4+2] *= 1.5
            _, _, transformed = software.draw_authored(vertices, indices, stretched, raster=False)
            software.clear()
            repeated_image, repeated_depth, _ = software.draw_authored(vertices, indices, matrix, z_enabled=blend != 80)
            if (repeated_image, repeated_depth) != (images[0], depths[0]):
                raise AssertionError('Original lit base warm replay differs')
            cases.append(dict(name=name, frame=frame, matrix_max_error=difference,
                independent_emitter_matrix_max_error=emitter_difference,
                differing_pixels=sum(a!=b for a,b in zip(struct.iter_unpack('<H', images[0]), struct.iter_unpack('<H', images[1]))),
                depth_equal=depths[0] == depths[1], nonzero_pixels=sum(p!=(0,) for p in struct.iter_unpack('<H', images[0])),
                original_warm_replay_equal=True,
                native_colors=colors[0], production_colors=colors[1],
                common_world_stretched_colors=[list(p[3:7]) for p in transformed]))
        (destination / 'compiled.json').write_text(json.dumps(compiled, indent=2) + '\n')
    report = dict(status='pass' if all(c['matrix_max_error'] < 2e-6 and c['differing_pixels'] == 0 and c['depth_equal'] and c['nonzero_pixels'] > 0 for c in cases) else 'differences_found',
        cases=cases, independent_production_emitter_frames=sum(c['independent_emitter_matrix_max_error'] is not None for c in cases),
        common_world_lighting_difference_frames=sum(c['native_colors'] != c['common_world_stretched_colors'] for c in cases),
        full_effect_accepted=False, base_only=True, native_illumination_producer=True,
        scope='Actual production integer decoder and SpeedEmitterLocalMatrix/SpeedBaseMeshWorldMatrix/FmasteryBaseMeshWorldMatrix execute from independent shipped keys. Identity owner in native D3D local domain; exact profile guards and no-interpolation resource accessor are explicit fixture boundaries. Actual native punctuation/CalcObjectMatrix and full D3DVERTEX normal illumination/projector/raster. Shared scene descriptors only; no native pose input to production. Full particle composite, common-world owner/projector, Metal, natural map/caster/audio and full scene-light selection remain separate.',
        probe_sha256=sha(Path(__file__).read_bytes()))
    (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    result = run(args.executable, args.output)
    print(json.dumps({k:v for k,v in result.items() if k != 'cases'}))
