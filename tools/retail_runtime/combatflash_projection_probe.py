"""Bound the admitted CombatFlash helper projection against original transformed vertices.

This evaluates production shader coefficients on original observed matrices;
it is a projection contract, not GPU vertex readback or appearance acceptance.
"""
import argparse,hashlib,json,struct
from pathlib import Path


def run(trace,output):
    output.mkdir(parents=True,exist_ok=False)
    source=Path(__file__).resolve().parents[2]/'src/engine/render/renderer.cpp'
    shader=source.read_text()
    coefficients=(1.0101525783538818,.5050762891769409,1.237179160118103,
                  .8748177289962769,.7142857909202576)
    assert all(str(c) in shader for c in coefficients)
    cases=[]
    for row in json.loads(trace.read_text())['rows']:
        for draw in row['draws']:
            if draw['object']!=2 or draw['arguments'][1]!=0x112:continue
            vertices=bytes.fromhex(draw['vertices_hex'])
            transformed=bytes.fromhex(draw['transformed_hex'])
            matrix=draw['matrix']
            for i in range(draw['arguments'][3]):
                local=struct.unpack_from('<3f',vertices,i*32)
                x,y,z=[sum(matrix[c*4+r]*local[c] for c in range(3))+matrix[12+r] for r in range(3)]
                predicted=[320+coefficients[0]*(x-y),240+coefficients[1]*(x+y)-coefficients[2]*z,
                           2750-coefficients[3]*(x+y)-coefficients[4]*z]
                native=list(struct.unpack_from('<3f',transformed,i*36));native[1]=-native[1]
                cases.append(dict(tick=row['tick'],vertex=i,predicted=predicted,native=native,
                                  maximum_error=max(abs(a-b) for a,b in zip(predicted,native))))
    assert cases and max(c['maximum_error'] for c in cases)<.0002
    report=dict(status='pass_bounded_projection',accepted=False,type_id='0xad92bd29',cases=cases,
        trace_sha256=hashlib.sha256(trace.read_bytes()).hexdigest(),
        renderer_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),
        probe_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
        scope='Production helper projection coefficients evaluated on observed original object2 matrices/local vertices; explicit camera0/center320240/owner0. Original inverted screenY normalized. Does not read back GPU vertices, prove UV/raster/texture/color equality, or grant appearance credit.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(len(cases),'channels',len(cases)*3,'max',max(c['maximum_error'] for c in cases))
    return report

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('--output',type=Path,required=True)
    a=p.parse_args();run(a.trace,a.output)
