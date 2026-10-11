"""Compare actual compiled CombatFlash base matrix with identified native draws."""
import argparse
import json
import struct
import subprocess
import zipfile
from pathlib import Path
from speed_profile_contract import ROOT, PRELUDE, CLASS, function, sha
from speed_controller_preflight import read_asset


def run(trace, output):
    output.mkdir(parents=True, exist_ok=False)
    source = (ROOT / 'src/3dimage.cpp').read_text()
    body = (ROOT / 'src/3dimagebody.h').read_text()
    render = (ROOT / 'src/render3d_types.h').read_text()
    prefixes = ('SMALLKEY', 'ANIKEY32_', 'ANIFLAG32_', 'ANICODE32_',
                'ANIKEY_POSSCALE', 'ANIKEY_ANGSCALE', 'I3D_ANIKEY32')
    constants = '\n'.join(line for line in body.splitlines() if line.startswith('#define ')
                          and any(line.startswith('#define ' + p) for p in prefixes))
    records = '\n'.join(function(text, signature) + ';' for text, signature in (
        (body, 'struct SAniKey32\n'), (body, 'struct S3DFace\n'),
        (render, 'struct S3DVertex\n'), (render, 'struct SRenderColor\n'),
        (body, 'struct S3DMaterial\n')))
    prelude = PRELUDE.replace('struct hmm_vec3 {float X=0,Y=0,Z=0;};',
                              '#include "goldauthoredmatrix.h"\n#include <strings.h>\n#define stricmp strcasecmp')
    model = CLASS.replace(' bool ValidateRetailSpeedPartSysProfile();', '''
 bool HasRetailSpeedFamilyPartSysProfile(uint32_t)const{return false;}
 bool combat_profile=false;
 bool HasCombatFlashStart1PartSysProfile()const{return combat_profile;}
 int prevstate=0,prevframe=0;
 struct Header{int numstates=1;struct State{int aniflags=0,frames=19;}states[1];}header;
 Header*GetHeader(){return &header;}char*FindTag(char*,int){return nullptr;}
 void SetPrevState(int s,int f){prevstate=s;prevframe=f;}
 bool GetAniKey(int,int,int,hmm_vec3&,hmm_vec3&,hmm_vec3&);
 bool ValidateRetailSpeedPartSysProfile();''')
    fixture = r'''
struct Owner{int frame=0;hmm_mat4 world;Owner(){MtxClear(&world);}
 int GetFrame(){return frame;}int GetPrevState(){return 0;}int GetPrevFrame(){return 0;}
 Owner&Transform(){return *this;}const hmm_mat4&Matrix()const{return world;}};
struct T3DAnimator{Owner*inst;T3DImagery*im;T3DImagery*Get3DImagery(){return im;}
 bool CombatFlashStart1BaseMeshBlend(int,uint32_t& b){b=16;return true;}
 bool CombatFlashStart1BaseMeshWorldMatrix(hmm_mat4&);};
'''
    methods = '\n'.join(function(source, s) for s in (
        'static inline int32_t SkipAniKey32(', 'static inline void GetAniKey32(',
        'bool T3DImagery::GetUninterpolatedAniKey(',
        'bool T3DImagery::GetAniKey(',
        'bool T3DAnimator::CombatFlashStart1BaseMeshWorldMatrix('))
    main = r'''
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);
 T3DImagery im;im.flags=FLAGS;im.nstates=1;im.frames=FRAMES;Owner owner;
 for(int i=0;i<3;++i){uint32_t n;f.read((char*)&n,4);im.keys[i].resize(n);f.read((char*)im.keys[i].data(),n*4);im.keycount[i]=n;im.keyptr[i]=im.keys[i].data();im.objects[i].numanikeys=&im.keycount[i];im.objects[i].anikeys=&im.keyptr[i];}
 T3DAnimator animator;animator.inst=&owner;animator.im=&im;
 for(int inclusive=0;inclusive<2;++inclusive){im.combat_profile=inclusive;
 for(int frame=0;frame<im.frames;++frame){owner.frame=frame;hmm_mat4 m{};
 bool alive=animator.CombatFlashStart1BaseMeshWorldMatrix(m);
 printf("%d %d %d",inclusive,frame,int(alive));for(int c=0;c<4;++c)for(int r=0;r<4;++r)printf(" %.9g",m.Elements[c][r]);puts("");}}
}
'''
    with zipfile.ZipFile(ROOT / 'data/imagery.rvi') as archive:
        data = archive.read(next(n for n in archive.namelist()
                                 if n.lower() == 'imagery/misc/impact.i3d'))
    asset = read_asset(data)
    keys = output / 'keys.bin'
    keys.write_bytes(b''.join(struct.pack('<I', len(o['states'][0]['keys'])) +
        struct.pack('<' + 'I'*len(o['states'][0]['keys']), *o['states'][0]['keys'])
        for o in asset['objects'][:3]))
    main = main.replace('FLAGS', str(asset['flags'])).replace('FRAMES', str(asset['state0_frames']))
    cpp = output / 'matrix.cpp'
    interpolation = '\n'.join(function(source, s) for s in (
        'static void InterpolatePoints(', 'static void NormalizeRot(') if s in source)
    cpp.write_text('\n'.join((prelude, constants, records, model, fixture,
        'bool Interpolate=true; constexpr int kLegacyTransitionBlendFrames=5; constexpr int AF_NOINTERPOLATION=0x40;',
        interpolation, methods, main)))
    binary = output / 'matrix'
    command = ['clang++', '-std=c++17', '-O1', '-fsanitize=address,undefined',
        '-iquote', str(ROOT / 'src'), '-I', str(ROOT / 'thirdparty/handmademath'),
        str(cpp), str(ROOT / 'src/math3d.cpp'), '-o', str(binary)]
    result = subprocess.run(command, capture_output=True, text=True)
    (output / 'compile.log').write_text(result.stdout + result.stderr)
    if result.returncode:
        raise RuntimeError('Actual matrix compile failed: '+str(output / 'compile.log'))
    result = subprocess.run([str(binary), str(keys)], capture_output=True, text=True)
    assert result.returncode == 0 and not result.stderr, result.stderr
    (output / 'matrices.txt').write_text(result.stdout)
    matrices = {(int(v[0]), int(v[1])): (bool(int(v[2])), list(map(float, v[3:])))
                for v in map(str.split, result.stdout.splitlines())}
    cases = []
    for row in json.loads(trace.read_text())['rows']:
        for draw in row['draws']:
            if draw['object'] != 2 or draw['arguments'][1] != 0x112:
                continue
            errors = {}
            for inclusive in (0, 1):
                alive, matrix = matrices[inclusive, row['frame']]
                errors[str(inclusive)] = dict(alive=alive,
                    maximum_matrix_error=max(abs(a-b) for a,b in zip(matrix, draw['matrix'])))
            cases.append(dict(tick=row['tick'], frame=row['frame'], policies=errors))
    assert cases
    report = dict(status='diagnostic', accepted=False, type_id='0xad92bd29',
        asset_sha256=sha(data), trace_sha256=sha(trace.read_bytes()), cases=cases,
        scope='Actual production decoder and base-matrix body, immutable first-state own keys; owner identity/previous-state absent declared. Compare half-open/default versus explicit inclusive policy without changing native frames or source. Does not prove shader/projection/color or other states.')
    (output / 'manifest.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps(cases, indent=2))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('trace', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    run(args.trace, args.output)
