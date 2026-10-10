#!/usr/bin/env python3
"""FireCone native shared Render and compiled SubmitPool raw-stage diagnostic.

Retains matrices and authored vertices/UVs/textures without a coordinate fit or
an inverse common-world adapter. Native packets are local D3D; production
packets are common-world with the actual owner's 1.5 Z stretch. Their numerical
residual is evidence about those stages, not a device pixel parity verdict.
"""
import argparse
import json
from pathlib import Path
import struct
import subprocess
import zipfile
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EBP, UC_X86_REG_ESI, UC_X86_REG_ECX, UC_X86_REG_EIP, UC_X86_REG_ESP
from firecone_state_probe import FireConeFixture, POOL_SPECS, ROOT, function, run as state_run, sha

ASSET_SHA = '8683cc1a0921f4c0bb7ef8544e94e1e391b180caca0db14260e8538fba948802'
SELECTED = (0, 1, 2, 6, 15, 20, 33, 60, 92, 93, 100)


def parse_asset(asset):
    if sha(asset) != ASSET_SHA:
        raise ValueError('Wrong shipped FireCone asset')
    u = lambda p: struct.unpack_from('<I', asset, p)[0]
    r = lambda p: p + u(p)
    if tuple(u(p) for p in (116, 124, 132, 140, 148, 156)) != (8, 4, 2, 2, 2, 0):
        raise AssertionError('Authored FireCone topology changed')
    vertices, faces, materials, textures = r(r(r(120))), r(128), r(136), r(144)
    parts = []
    for obj in range(2):
        desc = textures + obj * 120
        bits = r(r(desc + 108))
        if tuple(u(desc + p) for p in (8, 12, 88, 92, 96, 100, 104, 116)) != (64, 64, 0xf800, 0x7e0, 0x1f, 0, 0, 1):
            raise AssertionError('Authored single-frame RGB565 descriptor changed')
        records = list(struct.iter_unpack('<8f', asset[vertices + obj * 128:vertices + (obj + 1) * 128]))
        indices = list(struct.unpack_from('<6H', asset, faces + obj * 12))
        if indices != [2, 0, 3, 1, 3, 0]:
            raise AssertionError('Authored local topology changed')
        parts.append(dict(vertices=[list(v) for v in records], indices=indices,
                          material=list(struct.unpack_from('<17f', asset, materials + obj * 80 + 4)),
                          texture_offset=bits, texture_sha256=sha(asset[bits:bits + 8192]),
                          vertex_offset=vertices + obj * 128, face_offset=faces + obj * 12))
    return parts


class FireConeRenderFixture(FireConeFixture):
    def __init__(self, executable, facing=0):
        super().__init__(executable, facing)
        self.packets = []
        self.render_calls = []
        self.scene_calls = []
        self.point = self.vm.allocate(12)
        self.result = self.vm.allocate(12)
        self.matrix = self.vm.allocate(64)
        self.render_boundaries = {0x4178e0: 0, 0x417d60: 8, 0x417b00: 0,
                                  0x40c960: 0, 0x40a8f0: 24, 0x40c9c0: 4}
        for address in self.render_boundaries:
            self.vm.uc.hook_add(UC_HOOK_CODE, self.render_boundary, begin=address, end=address)
        self.vm.uc.hook_add(UC_HOOK_CODE, self.observe_render, begin=0x50c220, end=0x50c220)

    def observe_render(self, uc, address, size, user):
        system = uc.reg_read(UC_X86_REG_ECX)
        name = next(name for name, off, _ in POOL_SPECS if system == self.animator + off)
        sp = uc.reg_read(UC_X86_REG_ESP)
        args = list(struct.unpack('<2I', self.vm.uc.mem_read(sp + 4, 8)))
        if args != [0, 0]:
            raise AssertionError('FireCone must render with flicker=false, abs_pos=false')
        self.render_calls.append(dict(pool=name, arguments=args))

    def render_boundary(self, uc, address, size, user):
        sp = uc.reg_read(UC_X86_REG_ESP)
        if address in (0x4178e0, 0x417d60, 0x417b00):
            args = list(struct.unpack('<2I', self.vm.uc.mem_read(sp + 4, 8))) if address == 0x417d60 else []
            self.scene_calls.append(dict(function=hex(address), arguments=args))
            if address == 0x417d60 and args != [8, 1]:
                raise AssertionError('Unexpected source additive-mode request')
        elif address == 0x40a8f0:
            obj = self.vm.u32(sp + 4)
            system = uc.reg_read(UC_X86_REG_ESI)
            pool = next(name for name, off, _ in POOL_SPECS if system == self.animator + off)
            flags = self.vm.u32(obj)
            if flags != 0x100:
                raise AssertionError('Expected native relative MATRIX packet')
            self.packets.append(dict(pool=pool, slot=uc.reg_read(UC_X86_REG_EBP) // 88,
                                     object=self.objects.index(obj), flags=flags,
                                     matrix=list(struct.unpack('<16f', self.vm.uc.mem_read(obj + 0x58, 64))),
                                     scale=list(struct.unpack('<3f', self.vm.uc.mem_read(obj + 0x40, 12)))))
        uc.reg_write(UC_X86_REG_EIP, self.vm.u32(sp))
        uc.reg_write(UC_X86_REG_ESP, sp + 4 + self.render_boundaries[address])

    def render(self, parts):
        self.packets, self.render_calls, self.scene_calls = [], [], []
        if self.state()['alive']:
            self.vm.call(0x4eaae0, this=self.animator)
        for packet in self.packets:
            self.vm.write(self.matrix, struct.pack('<16f', *packet['matrix']))
            packet['positions'] = []
            for v in parts[packet['object']]['vertices']:
                self.vm.write(self.point, struct.pack('<3f', *v[:3]))
                self.vm.call(0x43ad80, (self.matrix, self.point, self.result))
                packet['positions'].append(list(struct.unpack('<3f', self.vm.uc.mem_read(self.result, 12))))
            packet['uvs'] = [v[6:8] for v in parts[packet['object']]['vertices']]
        return self.packets


def compile_submit(output, parts):
    source = (ROOT / 'src/effect.cpp').read_text()
    header = (ROOT / 'src/effect.h').read_text()
    first = header.index('    static constexpr int FLAME_COUNT', header.index('class TFireConeEffect_Bespoke'))
    fields = header[first:header.index('    void SimulateTick();', first)]
    particle = function((ROOT / 'src/effectcomp.h').read_text(), 'struct SParticleSystemInfo') + ';'
    body = function(source, 'void TFireConeEffect_Bespoke::SubmitPool(')
    observed = body.replace('Renderer->SubmitHelperMesh(submit);',
                            'Renderer->slot = int(&particle - pool.particles.data()); Renderer->SubmitHelperMesh(submit);')
    prelude = r'''
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <vector>
#include "math3d.h"
#undef min
#undef max
#define TORADIAN ((double)M_PI / (double)180.0)
#define FIX_Z_VALUE(z) ((float)(z)/1.46f)
#define REV_FIX_Z_VALUE(z) ((float)(z)*1.46f)
#define WORLD3D_Z_SCALE 1.5f
enum class EFxDebugMode{Normal};
struct SMeshVertex{float pos[3]{},normal[3]{},uv[2]{};};
struct Material{float diffuse[4]{},ambient[4]{},specular[4]{},emissive[4]{},power=0;};struct S3DMat{Material matdesc;};
struct SHelperMeshSubmit{unsigned mesh=1;bool additive_blend=false;int retail_lighting=0;float world[16]{},diffuse[4]{},ambient[4]{},specular[4]{},emissive[4]{},power=0,sort_depth=0;};
struct RendererType{int slot=0;const char*pool="";std::vector<SMeshVertex>*vertices=nullptr;void SubmitHelperMesh(const SHelperMeshSubmit&s){
printf("D %s %d %u %d %d",pool,slot,s.mesh-1,int(s.additive_blend),s.retail_lighting);
hmm_mat4 m={};for(int r=0;r<4;++r)for(int c=0;c<4;++c){m.Elements[r][c]=s.world[c*4+r];printf(" %.9g",m.Elements[r][c]);}
for(const auto&v:vertices[s.mesh-1]){const hmm_vec3 authored{v.pos[0],v.pos[1],v.pos[2]};hmm_vec3 p={};MtxTransform(&m,&authored,&p);printf(" %.9g %.9g %.9g",p.X,p.Y,p.Z);}puts("");}};
RendererType renderer;RendererType*Renderer=&renderer;
struct World{hmm_mat4 m;World(){MtxClear(&m);m.Elements[2][2]=WORLD3D_Z_SCALE;}const hmm_mat4&Matrix()const{return m;}};
PARTICLE
struct TFireConeEffect_Bespoke{FIELDS
Pool fire_{80},smoke_{80},burst_{100};std::vector<SMeshVertex>vertices_[2];std::vector<uint16_t>indices_[2];S3DMat materials_[2];unsigned meshes_[2]{1,2};World world;float facing_=0;
const World&Transform()const{return world;}void SubmitPool(const Pool&,int,EFxDebugMode)const;};
'''.replace('PARTICLE', particle).replace('FIELDS', fields)
    trailer = r'''
int main(int argc,char**argv){if(argc!=4)return 2;TFireConeEffect_Bespoke e;e.facing_=std::atof(argv[3]);
FILE*a=fopen(argv[1],"rb");if(!a)return 3;for(int j=0;j<2;++j){e.vertices_[j].resize(4);e.indices_[j].resize(6);
if(fread(e.vertices_[j].data(),1,128,a)!=128||fread(e.indices_[j].data(),1,12,a)!=12||fread(&e.materials_[j].matdesc,1,68,a)!=68)return 4;}fclose(a);
FILE*f=fopen(argv[2],"rb");if(!f)return 5;
for(auto*pool:{&e.fire_,&e.smoke_,&e.burst_})for(auto&p:pool->particles){float values[18];uint32_t ints[4];
if(fread(values,1,72,f)!=72||fread(ints,1,16,f)!=16)return 6;
memcpy(&p.pos,values,72);p.flicker=bool(ints[0]);p.life=ints[1];p.life_span=ints[2];p.used=bool(ints[3]);}fclose(f);
renderer.vertices=e.vertices_;renderer.pool="fire";e.SubmitPool(e.fire_,0,EFxDebugMode::Normal);
renderer.pool="smoke";e.SubmitPool(e.smoke_,1,EFxDebugMode::Normal);
renderer.pool="burst";e.SubmitPool(e.burst_,0,EFxDebugMode::Normal);}
'''
    cpp = output / 'firecone-submit.cpp'
    cpp.write_text(prelude + observed + trailer)
    binary = output / 'firecone-submit'
    command = ['clang++', '-std=c++17', '-I', str(ROOT / 'thirdparty/handmademath'), '-iquote', str(ROOT / 'src'), str(cpp), str(ROOT / 'src/math3d.cpp'), '-o', str(binary)]
    result = subprocess.run(command, capture_output=True, text=True)
    (output / 'compile.log').write_text(result.stdout + result.stderr)
    if result.returncode:
        raise RuntimeError('Compiled actual SubmitPool failed; see compile.log')
    assets = output / 'authored-parts.bin'
    assets.write_bytes(b''.join(struct.pack('<32f', *(n for v in p['vertices'] for n in v)) + struct.pack('<6H', *p['indices']) + struct.pack('<17f', *p['material']) for p in parts))
    return binary, assets, dict(command=command, original_body_sha256=sha(body.encode()),
                               instrumentation='Slot observer immediately before actual SubmitHelperMesh; geometry/body unchanged.',
                               driver_sha256=sha(cpp.read_bytes()), binary_sha256=sha(binary.read_bytes()))


def candidate_packets(binary, assets, output, state, facing, parts):
    inputs = output / 'candidate-slots.bin'
    inputs.write_bytes(b''.join(struct.pack('<18f4I', *p['values'], p['flicker'], p['life'], p['span'], p['used']) for name, _, _ in POOL_SPECS for p in state['pools'][name]))
    text = subprocess.check_output([str(binary), str(assets), str(inputs), str(facing)], text=True)
    packets = []
    for line in text.splitlines():
        p = line.split()
        m = list(map(float, p[6:22]))
        obj = int(p[3])
        points = [list(map(float, p[22 + i * 3:25 + i * 3])) for i in range(4)]
        packets.append(dict(pool=p[1], slot=int(p[2]), object=obj, additive=bool(int(p[4])), lighting=int(p[5]), matrix=m, positions=points, uvs=[v[6:8] for v in parts[obj]['vertices']]))
    return packets


def verify_state(native, candidate):
    for key in ('done', 'state', 'frame', 'alive', 'random_count'):
        if native[key] != candidate[key]:
            raise AssertionError('Render replay differs from complete state proof: ' + key)
    for name, _, _ in POOL_SPECS:
        for a, b in zip(native['pools'][name], candidate['pools'][name]):
            if a['used'] != b['used']:
                raise AssertionError('Render replay occupancy differs')
            if not a['used']:
                continue
            for key in ('life', 'span', 'flicker'):
                if key == 'flicker' and name == 'smoke':
                    continue
                if a[key] != b[key]:
                    raise AssertionError('Render replay active integer differs')
            count = 15 if name == 'burst' else 18
            if struct.pack('<' + 'f' * count, *a['values'][:count]) != struct.pack('<' + 'f' * count, *b['values'][:count]):
                raise AssertionError('Render replay active floats differ')


def run(executable, output, facings=(0, 64, 128, 192)):
    output.mkdir(parents=True, exist_ok=False)
    state_report = state_run(executable, output / 'state', 100)
    if state_report['status'] != 'pass':
        raise AssertionError('Complete production state prerequisite failed')
    candidate = json.loads((output / 'state/candidate-states.json').read_text())
    with zipfile.ZipFile(ROOT / 'data/imagery.rvi') as archive:
        asset = archive.read('Imagery/Magic/firecone.i3d')
    parts = parse_asset(asset)
    (output / 'authored-asset.i3d').write_bytes(asset)
    binary, assets, compiled = compile_submit(output, parts)
    cases = []
    for face in facings:
        fixture = FireConeRenderFixture(executable, face)
        for tick in range(101):
            if tick and fixture.state()['alive']:
                fixture.step()
            verify_state(fixture.state(), candidate[tick])
            if tick not in SELECTED:
                continue
            original = fixture.render(parts)
            facing = struct.unpack('<f', fixture.vm.uc.mem_read(fixture.animator + 0x158 + 0x20, 4))[0]
            port = candidate_packets(binary, assets, output, candidate[tick], facing, parts) if candidate[tick]['alive'] else []
            native_keys = [(p['pool'], p['slot'], p['object']) for p in original]
            modern_keys = [(p['pool'], p['slot'], p['object']) for p in port]
            paired = {k: p for k, p in zip(native_keys, original)}
            delta_by_column = [0.0] * 4
            point_delta = [0.0] * 3
            for key, packet in zip(modern_keys, port):
                if key not in paired:
                    continue
                native = paired[key]
                for i, (a, b) in enumerate(zip(native['matrix'], packet['matrix'])):
                    delta_by_column[i % 4] = max(delta_by_column[i % 4], abs(a - b))
                for a, b in zip(native['positions'], packet['positions']):
                    for i, (x, y) in enumerate(zip(a, b)):
                        point_delta[i] = max(point_delta[i], abs(x - y))
            # Retail observes all used slots before downstream imagery culling.
            expected = [(name, slot, 1 if name == 'smoke' else 0) for name, _, _ in POOL_SPECS for slot, p in enumerate(candidate[tick]['pools'][name]) if p['used']] if candidate[tick]['alive'] else []
            if native_keys != expected:
                raise AssertionError('Native packet order/coverage differs from complete compiled pool')
            cases.append(dict(face_byte=face, facing_radians=facing, tick=tick, alive=candidate[tick]['alive'],
                              native_count=len(original), production_count=len(port),
                              production_omitted_keys=[list(k) for k in native_keys if k not in modern_keys],
                              paired_packets=len(set(native_keys) & set(modern_keys)),
                              used_negative_scale_particles=sum(any(v < 0 for v in p['values'][3:6]) for name, _, _ in POOL_SPECS for p in candidate[tick]['pools'][name] if p['used']),
                              native_flicker_flags_ignored=sum(bool(candidate[tick]['pools'][p['pool']][p['slot']]['flicker']) for p in original if p['pool'] != 'smoke'),
                              raw_matrix_max_difference_by_output_column=delta_by_column,
                              raw_authored_point_max_difference_xyz=point_delta,
                              native_render_calls=fixture.render_calls, native_scene_calls=fixture.scene_calls,
                              native_packets=original, production_packets=port))
    report = dict(status='retained_raw_stage_diagnostic', full_acceptance=False, software_pixels_executed=False,
                  production_changed=False, state_proof_sha256=sha((output / 'state/manifest.json').read_bytes()),
                  state_fields_exact=state_report['float_fields'], native_random_calls=state_report['native_random_calls'],
                  every_tick_state_replay_verified=True,
                  executable_sha256=sha(Path(executable).read_bytes()), asset_sha256=ASSET_SHA, authored_parts=parts,
                  compiled_submit=compiled, original_functions=['0x4eaae0', '0x50c220', '0x43ad80'],
                  cases=cases, probe_sha256=sha(Path(__file__).read_bytes()),
                  boundaries=['Existing null-spell native Init/Animate owner/base/audio boundaries.',
                              'Scene blend Save/Set/Restore requests recorded; no device success invented.',
                              'Imagery submit/extents interfaces intercepted after real native particle matrix construction.',
                              'Candidate actual SubmitPool uses origin-zero owner with real common-world 1.5 Z scale; asset and state supplied explicitly.'],
                  coordinate_scope='Raw native relative D3D versus actual production common-world packets. No 1.46 inverse, common-Z rescaling, fitted matrix/camera, or software-pixel adapter applied. Owner/world/projector stage bridge is unresolved; raw Z residual alone is not causal production-change evidence.')
    (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    report = run(args.executable, args.output)
    print(json.dumps({k: report[k] for k in ('status', 'state_fields_exact', 'native_random_calls', 'software_pixels_executed', 'production_changed')}))


if __name__ == '__main__':
    main()
