#!/usr/bin/env python3
"""Exact authored buff triage and original speed constructor/controller preflight.

No engine changes, guessed actor or replacement geometry. A failure is retained
as a boundary report and cannot be promoted to rendered acceptance.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import zipfile

from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_EIP, UC_X86_REG_ESP
from default_static_probe import original_registry
from software_probe import SoftwareFixture, RETAIL_SHA

ROOT = Path(__file__).resolve().parents[2]
NAMES = ('speed', 'Ogrestrength', 'Regeneration', 'trollblood', 'Dexterity',
         'Antimagic', 'Restorelife', 'stoneskin', 'ironskin')
SPEED_SHA = '65c45c03bcf3cd5cfe549232f88fb18550a30720c5b9500d0cfd5ef8154fce42'


def sha(data):
    return hashlib.sha256(data).hexdigest()


def read_asset(data):
    u = lambda a: struct.unpack_from('<I', data, a)[0]
    r = lambda a: a + u(a)
    string = lambda a: data[a:data.index(0, a)].decode('latin1')
    body = 20 + u(16)
    objects = r(body + 48)
    tags = r(body + 56)
    vertices = r(r(r(body + 16)))
    faces = r(body + 24)
    result = dict(sha256=sha(data), body_offset=body, flags=u(body), version=u(body+4),
                  vertices=u(body+12), faces=u(body+20), materials=u(body+28),
                  textures=u(body+36), states=u(24), objects=[], tags=[])
    for i in range(u(body+44)):
        p = objects + 48*i
        material, start, count = struct.unpack_from('<3H', data, p+32)
        states = r(p+44)
        tracks = []
        for state in range(u(24)):
            st = states + state*12
            parent, n = struct.unpack_from('<2i', data, st)
            keys = list(struct.unpack_from('<'+'I'*n, data, r(st+8)))
            tracks.append(dict(parent=parent, keys=keys))
        runs = list(struct.unpack_from('<'+'H'*(2*(result['textures']+1)), data, r(p+40)))
        result['objects'].append(dict(index=i, name=string(p), material=material,
            vertex_start=start, vertex_count=count, states=tracks, texture_runs=runs,
            vertex_hex=data[vertices+start*32:vertices+(start+count)*32].hex()))
    result['face_hex'] = data[faces:faces+6*result['faces']].hex()
    for i in range(u(body+52)):
        p = tags + i*16
        result['tags'].append(dict(state=u(p), frame=u(p+4), name=string(r(p+8)),
                                  parameters=string(r(p+12))))
    result['state_header_hex'] = data[28:28+u(24)*76].hex()
    result['state0_frames'] = struct.unpack_from('<h', data, 70)[0]
    return result


class NativeSpeed:
    def __init__(self, executable, asset):
        self.software = SoftwareFixture(executable, 512, 512)
        self.vm = self.software.vm
        v = self.vm
        if sha(v.image) != RETAIL_SHA:
            raise ValueError('Requires pinned original retail executable')
        self.asset = asset
        particles = [t for t in asset['tags'] if t['name']=='partsys' and t['state']==0]
        if len(particles)!=1:
            raise ValueError('Requires exactly one explicit state0 particle tag')
        import re
        particle_name = re.search(r'\bparticle=([^,]+)',particles[0]['parameters'])[1]
        self.prototype_index = next(o['index']for o in asset['objects']if o['name']==particle_name)
        emitter_names = re.search(r'\bobj=\(([^)]+)\)',particles[0]['parameters'])[1].split(',')
        self.required_pose_indices = {0,self.prototype_index}
        self.required_pose_indices.update(o['index']for o in asset['objects']if o['name']in emitter_names)
        self.unused_rejected_pose_indices = set()
        self.calls = {}
        self.boundaries = {}
        self.owner = v.allocate(0x200)
        self.imagery = v.allocate(0x200)
        self.dispatch = original_registry(v, (asset.get('name','speed'),))
        self.put_string = lambda s: self.bytes(s.encode('latin1')+b'\0')
        # Immutable resource identity boundary. The actual animator factory,
        # TObjectAnimator constructor and dynamic-array constructors execute.
        h = v.uc.hook_add(UC_HOOK_CODE, self.imagery_lookup, begin=0x46e8a0, end=0x46e8a0)
        try:
            self.animator = v.call(0x40dc00, (self.owner,), this=0x5e8508)
        finally:
            v.uc.hook_del(h)
        if v.u32(self.animator) != 0x5a370c or v.u32(self.animator+8) != self.imagery:
            raise AssertionError('Complete native default factory returned wrong animator')
        self.constructor = dict(factory='0x40dc00', base_constructor='0x445940',
            array_constructor='0x41c7f0', actual_vtable=hex(v.u32(self.animator)),
            resource_boundary='0x46e8a0 returns exact decoded imagery provider; no owner replaced')
        self.setup_resources()
        for address in (0x40df90, 0x403320, 0x405940, 0x40d750, 0x4042e0, 0x4010d0,
                        0x405e20, 0x4059e0, 0x403760, 0x4031c0, 0x402a20):
            v.uc.hook_add(UC_HOOK_CODE, self.observe, begin=address, end=address)
        # Vertex duplication is a declared resource-interface boundary. It
        # copies the exact asset quad and never supplies a stand-in mesh.
        v.uc.hook_add(UC_HOOK_CODE, self.copy_vertices, begin=0x40a0c0, end=0x40a0c0)
        v.call(0x403300)
        v.call(0x405750)

    def bytes(self, data):
        address = self.vm.allocate(len(data))
        self.vm.write(address, data)
        return address

    def ret(self, result, cleanup):
        v = self.vm
        sp = v.uc.reg_read(UC_X86_REG_ESP)
        v.uc.reg_write(UC_X86_REG_EAX, result)
        v.uc.reg_write(UC_X86_REG_EIP, v.u32(sp))
        v.uc.reg_write(UC_X86_REG_ESP, sp+4+cleanup)

    def imagery_lookup(self, uc, address, size, user):
        self.ret(self.imagery, 0)

    def observe(self, uc, address, size, user):
        self.calls[hex(address)] = self.calls.get(hex(address), 0)+1

    def copy_vertices(self, uc, address, size, user):
        v = self.vm
        sp = uc.reg_read(UC_X86_REG_ESP)
        obj, vertex_type = v.u32(sp+4), v.u32(sp+8)
        if obj != self.animobjs[self.prototype_index] or vertex_type != 0x1e2:
            raise AssertionError('Full controller Initialize requested unexpected mesh')
        # Retail LVERTEX is 32 bytes as is the asset VERTEX; UVs are at24.
        # Positions/UVs remain literal, normal bytes are replaced by draw color.
        prototype = self.asset['objects'][self.prototype_index]
        v.put_u32(obj+0xa4, self.bytes(bytes.fromhex(prototype['vertex_hex'])))
        self.boundaries['0x40a0c0'] = dict(object_index=self.prototype_index, name=prototype['name'],
            vertex_type=hex(vertex_type), exact_vertex_sha256=sha(bytes.fromhex(prototype['vertex_hex'])))
        self.ret(1, 8)

    def setup_resources(self):
        v, m = self.vm, self.asset
        v.put_u32(0x5d7a14, 0)  # One state, explicit disabled cross-state interpolation.
        self.animobjs = []
        objects = v.allocate(len(m['objects'])*4)
        v.put_u32(self.imagery+0x0c, 1)
        v.put_u32(self.imagery+0x1c, m['flags'])
        v.put_u32(self.imagery+0x64, len(m['objects']))
        v.put_u32(self.imagery+0x74, objects)
        # Real compressed walker reads only these immutable header methods.
        vt = v.allocate(0x100)
        v.put_u32(self.imagery, vt)
        state_method = v.allocate(16)
        v.write_code(state_method, b'\xb8'+struct.pack('<I',m['states'])+b'\xc3')
        length_method = v.allocate(16)
        v.write_code(length_method, b'\xb8'+struct.pack('<I', m['state0_frames'])+b'\xc2\x04\x00')
        v.put_u32(vt+0x3c, state_method)
        v.put_u32(vt+0x90, length_method)
        anim_array = v.u32(self.animator+0x54)
        if len(m['objects'])>16:
            # Resource-provider array capacity, not a guessed object. The
            # native factory's original 16-slot constructor ran above.
            anim_array = v.allocate(len(m['objects'])*4)
            v.put_u32(self.animator+0x54,anim_array)
        for i, o in enumerate(m['objects']):
            resource = v.allocate(0x48)
            v.write(resource, o['name'].encode()+b'\0')
            track = o['states'][0]
            v.put_u32(resource+0x40, self.bytes(struct.pack('<I', len(track['keys']))))
            v.put_u32(resource+0x44, self.bytes(struct.pack('<I', self.bytes(struct.pack('<'+'I'*len(track['keys']), *track['keys'])))))
            v.put_u32(objects+i*4, resource)
            obj = v.allocate(0x34c)
            v.put_u32(obj+4, i)
            v.put_u32(obj+0xa0, o['vertex_count'])
            if o['vertex_count']:
                v.put_u32(obj+0xa4, self.bytes(bytes.fromhex(o['vertex_hex'])))
            v.put_u32(anim_array+i*4, obj)
            self.animobjs.append(obj)
            if track['parent']!=-1:
                raise ValueError('Parented authored fixture requires its actual hierarchy; no fallback')
            if track['keys'] and v.call(0x40a420, (obj, 0, 0, 0, 0), this=self.imagery) != 1:
                if i in self.required_pose_indices:
                    raise AssertionError(f'Original required asset key/matrix decode failed for {i}:{o["name"]}')
                # Record the native rejection of an unused state0 track. Do
                # not install a transform or replace an emitter/prototype.
                self.unused_rejected_pose_indices.add(i)
        v.put_u32(self.animator+0x44, len(m['objects']))
        v.put_u32(self.animator+0x6c, 0xffffffff)
        tags = v.allocate(len(m['tags'])*4)
        v.put_u32(self.imagery+0x7c, len(m['tags']))
        v.put_u32(self.imagery+0x8c, tags)
        for i, t in enumerate(m['tags']):
            p = self.bytes(struct.pack('<4I', t['state'], t['frame'],
                self.put_string(t['name']), self.put_string(t['parameters'])))
            v.put_u32(tags+i*4, p)

    def initialize(self, quality):
        v = self.vm
        self.quality = quality
        v.put_u32(0x5d79e4, quality)
        v.call(0x40df90, (0,), this=self.animator, instruction_limit=5000000)
        count = v.u32(self.animator+0x58)
        expected = len([t for t in self.asset['tags'] if t['state']==0])
        if count != expected:
            raise AssertionError(f'Expected {expected} complete native controllers; got {count}')
        pointers = v.u32(self.animator+0x68)
        rows = []
        for i in range(count):
            c = v.u32(pointers+i*4)
            row = dict(index=i, vtable=hex(v.u32(c)), state=v.u32(c+4), tagframe=v.u32(c+8),
                       emitter_count=v.u32(c+0x18))
            if v.u32(c) == 0x5a3544:
                self.controller = c
                row.update(capacity=v.u32(c+0xf8), stored_pps=struct.unpack('<f',v.uc.mem_read(c+0x10c,4))[0],
                    particle_prototype_index=v.u32(v.u32(c+0x164)+4),
                    object_zero_initial_origin=list(struct.unpack('<3f',v.uc.mem_read(c+0x170,12))))
            else:
                row['mode'] = v.u32(c+0x2c)
            rows.append(row)
        return dict(quality=quality, controllers=rows,
            object_flags=[hex(v.u32(p)) for p in self.animobjs],
            object_blend_modes=[v.u32(p+0x348) for p in self.animobjs],
            original_call_counts=self.calls, resource_boundaries=self.boundaries)

    def state_trace(self, ticks=90, frame_offset=0, move_tick=60):
        """Full initialized controller Pulse and actual live RenderSample path.

        Emitters use the original asset decoder/matrix; no emitter-transform
        interception. Only RNG and a declared flat ground query are boundaries.
        Rendering stops before the owner's animator virtual / device handoff.
        """
        v = self.vm
        def boundary(uc, address, size, user):
            if address == 0x58c582:
                self.ret(v.random_msvc(), 0)
            else:
                self.ret(0, 12)
        for address in (0x58c582, 0x452e10):
            v.uc.hook_add(UC_HOOK_CODE, boundary, begin=address, end=address)
        v.put_u32(0x5d7a28, 1)
        v.rng_seed = 1
        rows = []
        poses = []
        c = self.controller
        pool, capacity = v.u32(c+0x2c), v.u32(c+0xf8)
        rf = lambda address: struct.unpack('<f',v.uc.mem_read(address,4))[0]
        initialized_origin = [rf(c+0x170+i*4)for i in range(3)]
        for tick in range(ticks):
            frame = (tick + frame_offset) % self.asset['state0_frames']
            v.put_u32(self.animator+0x14, frame)
            if tick == move_tick:
                for off,value in zip((16,20,24),(16,-8,4)):
                    v.put_u32(self.owner+off,value&0xffffffff)
            for obj in self.animobjs:
                # Prototype transforms are mutable during sampling. Decode
                # only the real emitter/base track before controller Pulse.
                if obj == self.animobjs[self.prototype_index]:
                    continue
                index = self.animobjs.index(obj)
                if index in self.unused_rejected_pose_indices:
                    continue
                if not self.asset['objects'][index]['states'][0]['keys']:
                    continue  # Literal absent state0 track; no emitter substituted.
                if v.call(0x40a420,(obj,0,frame,0,0),this=self.imagery) != 1:
                    raise AssertionError('Actual per-tick emitter pose rejected')
            emitters = [v.u32(v.u32(c+0x28)+i*4) for i in range(v.u32(c+0x18))]
            emitter = emitters[0]
            poses.append(dict(tick=tick,frame=frame,
                position=[rf(emitter+0x10+i*4) for i in range(3)],
                scale=[rf(emitter+0x40+i*4) for i in range(3)],
                matrix=[rf(emitter+0x58+i*4) for i in range(16)],
                emitters=[dict(position=[rf(e+0x10+i*4)for i in range(3)],
                    scale=[rf(e+0x40+i*4)for i in range(3)],
                    matrix=[rf(e+0x58+i*4)for i in range(16)])for e in emitters]))
            v.call(0x403760,this=c,instruction_limit=5000000)
            samples = {}
            for slot in range(capacity):
                p = pool+slot*252
                if not v.u32(p+0xe4):
                    continue
                v.call(0x4026d0,this=p,stop_address=0x4028e2)
                prototype = self.animobjs[self.prototype_index]
                packed = v.u32(v.u32(prototype+0xa4)+16)
                samples[slot] = [*[rf(prototype+0x10+i*4) for i in range(3)],
                    *[rf(prototype+0x28+i*4) for i in range(3)],
                    (packed>>16)&255,(packed>>8)&255,packed&255,(packed>>24)/255,
                    rf(prototype+0x40)]
            for slot in range(capacity):
                p = pool+slot*252
                rows.append([tick,slot,v.rng_seed,v.u32(p+0xe4),v.u32(p+0xd0),
                    *[rf(p+0xb8+i*4) for i in range(3)],
                    *[rf(p+0xc4+i*4) for i in range(3)],rf(c+0x16c),v.u32(p+0xd4),
                    rf(p+0x64),*[rf(p+0x50+i*4) for i in range(3)],
                    rf(p+0x60),rf(p+0x4c),rf(p),
                    *samples.get(slot,[0]*11)])
        return dict(rows=rows,poses=poses,capacity=capacity,quality=self.quality,original_call_counts=self.calls,
            object_zero_initial_origin=initialized_origin,
            unused_state0_pose_rejections=sorted(self.unused_rejected_pose_indices),
            original_emitter_matrix_executed=True,full_parser_initialize_executed=True,
            render_stop='0x4028e2 before owner virtual/device geometry handoff',
            explicit_external_inputs=['MSVC RNG seed1','ground height0',f'owner initially0; MOVE(16,-8,4) at tick{move_tick}', f'animation frame offset{frame_offset}'])


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('executable', type=Path)
    p.add_argument('--archive', type=Path, default=ROOT/'data/imagery.rvi')
    p.add_argument('--output', type=Path, required=True)
    args = p.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(args.archive) as z:
        members = {n.lower(): n for n in z.namelist()}
        definitions = z.read(members['class.def']).decode('latin1')
        import re
        profiles = []
        for name in NAMES:
            line = next(l for l in definitions.splitlines() if '"'+name+'"' in l)
            path = re.search(r'"([^"]+\.i3d)"',line,re.I)[1]
            member = members['imagery/'+path.replace('\\','/').lower()]
            profile = read_asset(z.read(member))
            profile.update(name=name, class_definition=line.strip(), archive_member=member)
            profiles.append(profile)
    speed = profiles[0]
    if speed['sha256'] != SPEED_SHA or len(speed['objects']) != 3 or len(speed['tags']) != 2:
        raise ValueError('Speed exact asset revision changed')
    (args.output/'asset-triage.json').write_text(json.dumps(profiles,indent=2)+'\n')
    report = dict(status='preflight_pending', production_changes=False, accepted=False,
        dosbox_used=False, full_game_integration=False, new_rendered_ab_cases=0,
        asset_sha256=SPEED_SHA, original_executable_sha256=sha(args.executable.read_bytes()),
        probe_sha256=sha(Path(__file__).read_bytes()), ranked_candidates=list(NAMES),
        defer_reasons={'Dexterity':'scljitter unsupported', 'Antimagic':'scljitter unsupported',
            'Restorelife':'scljitter unsupported', 'stoneskin':'malformed localrotation + numtrails unsupported',
            'ironskin':'malformed localrotation + numtrails unsupported'})
    try:
        fixture = NativeSpeed.__new__(NativeSpeed)
        fixture.__init__(args.executable, speed)
        report['native_dispatch'] = fixture.dispatch
        report['full_default_constructor'] = fixture.constructor
        report['native_controller_initialization'] = fixture.initialize(0)
        trace = fixture.state_trace()
        (args.output/'retail-state-trace.json').write_text(json.dumps(trace,indent=2)+'\n')
        report['state_trace'] = dict(rows=len(trace['rows']),ticks=len(trace['poses']),
            capacity=trace['capacity'],full_parser_initialize_executed=True,
            original_emitter_matrix_executed=True,render_stop=trace['render_stop'])
        report['status'] = 'full_native_controller_init_pass_render_pending'
    except Exception as e:
        report['status'] = 'native_preflight_boundary'
        report['boundary'] = dict(type=type(e).__name__,message=str(e))
        if 'fixture' in locals():
            report['boundary']['original_pc'] = hex(fixture.vm.uc.reg_read(UC_X86_REG_EIP))
            report['boundary']['executed_original_calls'] = fixture.calls
    report['scope'] = ('Exact shipped speed metadata and whole default animator factory/constructor; '
        'whole RefreshControllers/controller constructors/parser/Initialize attempted. Decoded asset/provider '
        'interfaces and exact prototype vertex-copy boundary explicit. No synthesized controller packet. '
        'No controller render, geometry/software pair, runtime/Metal/spell or all176 acceptance.')
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k!='native_dispatch'},indent=2))


if __name__ == '__main__':
    main()
