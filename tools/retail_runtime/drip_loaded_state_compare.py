"""Check genuine loaded owner/child state against compiled production composite."""
import argparse,json,hashlib,struct
from pathlib import Path

def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def run(native,source,output):
 a=json.loads(native.read_text());b=json.loads(source.read_text());assert b['status']=='pass'
 errors=[];checks=0
 assert len(a['records'])==144 and len(b['cases'])==145
 for record in a['records']:
  tick=record['tick'];n=record['after'];p=b['cases'][tick]['port']
  for name in('dead','time'):
   checks+=1
   if n['parent'][name]!=p['parent'][name]:errors.append([tick,'parent',name])
  if p['parent']['values_valid']:
   assert len(n['parent']['values'])==len(p['parent']['values'])==6
   for x,y in zip(n['parent']['values'],p['parent']['values']):
    checks+=1
    if struct.pack('<f',x)!=struct.pack('<f',y):errors.append([tick,'parent_value',x,y])
  checks+=1
  if len(n['children'])!=len(p['children']):errors.append([tick,'child_count'])
  for x,y in zip(n['children'],p['children']):
   for name in('id','frameon','ripframe'):
    checks+=1
    if x[name]!=y[name]:errors.append([tick,'child',name])
   checks+=2
   if x['length']!=20 or x['splash_count']!=0:errors.append([tick,'child_contract'])
  assert record['before']==record['after'],'Render unexpectedly changes simulation'
 for key in('births','deaths'):
  expected=[{k:v for k,v in x.items()if k in('tick','id')}for x in b['cases'][-1]['port'][key]]
  checks+=len(expected)
  if a[key]!=expected:errors.append([key,a[key],expected])
 geometry_checks=0;maximum_position_error=0.0
 for record in a['records']:
  if record['tick'] not in a['samples']:continue
  draws=b['cases'][record['tick']]['port']['draws']
  if len(record['draws'])!=len(draws):errors.append([record['tick'],'draw_count'])
  for original,port in zip(record['draws'],draws):
   assert original['arguments'][3]==4 and original['arguments'][5]==6
   assert struct.unpack('<6H',bytes.fromhex(original['indices_hex']))==(0,1,3,3,2,0)
   raw=bytes.fromhex(original['vertices_hex']);matrix=original['matrix']
   for i in range(4):
    point=(*struct.unpack_from('<3f',raw,i*32),1)
    transformed=[sum(point[k]*matrix[j+4*k]for k in range(4))for j in range(3)]
    error=max(abs(x-y)for x,y in zip(transformed,port['positions'][i]));maximum_position_error=max(error,maximum_position_error)
    if error>2e-5:errors.append([record['tick'],'native_render_position',error])
    for x,y in zip(struct.unpack_from('<2f',raw,i*32+24),port['uvs'][i]):
     if struct.pack('<f',x)!=struct.pack('<f',y):errors.append([record['tick'],'native_render_uv'])
    geometry_checks+=5
 assert len(a['map_returns'])==5 and all(x['index']==0xffffffff for x in a['map_returns'])
 result=dict(status='pass'if not errors else'fail',checks=checks,geometry_checks=geometry_checks,maximum_position_error=maximum_position_error,errors=errors,native_sha256=sha(native),source_sha256=sha(source),scope='144 genuine parent/standalone child records versus compiled real production methods, exact active float32 fields, all births/ages/deaths; declared managed scheduling. Original empty-map allocations remain−1. Original audio RNG is not reproduced by silent port; fixed period1 physics consumes no random draw, and length20 has no splash recursion.')
 output.write_text(json.dumps(result,indent=2)+'\n');print(result);return result
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('native',type=Path);p.add_argument('source',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();raise SystemExit(run(a.native,a.source,a.output)['status']!='pass')
