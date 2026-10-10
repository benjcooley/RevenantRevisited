#!/usr/bin/env python3
"""Bounded null-spell FireCone leaf/pool state comparison, without rendering.

Original Init/Animate and TParticleSystem Init/Animate/Add execute in Unicorn.
Only base animator, owner mutation, asset GetObject and unavailable audio are
explicit boundaries. Native random results are replayed into compiled production.
"""
import argparse
import hashlib
import json
from collections import Counter
from pathlib import Path
import struct
import subprocess
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EIP, UC_X86_REG_ESP
from runtime import Runtime
from software_probe import RETAIL_SHA

ROOT = Path(__file__).resolve().parents[2]
POOL_SPECS = (("fire", 0x158, 80), ("smoke", 0xfc, 80), ("burst", 0x124, 100))


def sha(data):
    return hashlib.sha256(data).hexdigest()


def function(source, signature):
    first = source.index(signature)
    opening = source.index('{', first)
    depth = 1
    end = opening + 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[first:end]


class FireConeFixture:
    def __init__(self, executable, facing=0):
        if sha(Path(executable).read_bytes()) != RETAIL_SHA:
            raise ValueError('Unsupported executable')
        self.vm = Runtime(executable)
        self.vm.call(0x58ed0d, stop_address=0x58ed8e)
        self.animator = self.vm.allocate(0x200)
        self.owner = self.vm.allocate(0x400)
        vtable = self.vm.allocate(0x204)
        self.vm.put_u32(self.animator, 0x5aacac)
        self.vm.put_u32(self.animator + 4, self.owner)
        self.vm.put_u32(self.owner, vtable)
        self.vm.write(self.owner + 0x10, struct.pack('<3i', 10000, 10000, 16))
        self.vm.write(self.owner + 0x36, bytes([facing]))
        # Explicit owner API boundaries preserve the actual requested values.
        pos = self.vm.allocate(32)
        self.vm.write_code(pos, bytes.fromhex('8b4424048b108951108b50048951148b5008895118c20c00'))
        self.vm.put_u32(vtable + 8, pos)
        done = self.vm.allocate(16)
        self.vm.write_code(done, bytes.fromhex('31c0c20400'))
        self.vm.put_u32(vtable + 0x158, done)
        flags = self.vm.allocate(16)
        self.vm.write_code(flags, bytes.fromhex('8b442404894108c20400'))
        self.vm.put_u32(vtable + 0x40, flags)
        kill = self.vm.allocate(16)
        self.vm.write_code(kill, bytes.fromhex('31c0c3'))
        self.vm.put_u32(vtable + 0x1b4, kill)
        self.objects = [self.vm.allocate(0x200) for _ in range(2)]
        for _, offset, count in POOL_SPECS:
            system = self.animator + offset
            self.vm.put_u32(system, 0x5a96d8)
            self.vm.put_u32(system + 4, self.vm.call(0x482fb0, (count * 88,)))
            self.vm.put_u32(system + 0x1c, count)
        self.random = []
        self.pending = []
        self.calls = []
        self.boundaries = {0x40dd60: 0, 0x40e2e0: 0, 0x40eef0: 4, 0x49c430: 4}
        for address in self.boundaries:
            self.vm.uc.hook_add(UC_HOOK_CODE, self.external, begin=address, end=address)
        for address in (0x483300, 0x48332c, 0x50c170, 0x50c1b0, 0x50c560):
            self.vm.uc.hook_add(UC_HOOK_CODE, self.observe, begin=address, end=address)
        self.vm.call(0x4e9f00, this=self.animator)

    def external(self, uc, address, size, user):
        sp = uc.reg_read(UC_X86_REG_ESP)
        if address == 0x40eef0:
            index = self.vm.u32(sp + 4)
            if index not in (0, 1):
                raise AssertionError('Unexpected authored object request')
            uc.reg_write(UC_X86_REG_EAX, self.objects[index])
        elif address == 0x49c430:
            # Audio lookup fails honestly; original skips the sound branch.
            uc.reg_write(UC_X86_REG_EAX, 0xffffffff)
        uc.reg_write(UC_X86_REG_EIP, self.vm.u32(sp))
        uc.reg_write(UC_X86_REG_ESP, sp + 4 + self.boundaries[address])

    def observe(self, uc, address, size, user):
        if address == 0x483300:
            sp = uc.reg_read(UC_X86_REG_ESP)
            self.pending.append(struct.unpack('<2i', self.vm.uc.mem_read(sp + 4, 8)))
        elif address == 0x48332c:
            result = struct.unpack('<i', struct.pack('<I', uc.reg_read(UC_X86_REG_EAX)))[0]
            self.random.append((*self.pending.pop(), result))
        else:
            system = uc.reg_read(UC_X86_REG_ECX)
            name = next(name for name, off, _ in POOL_SPECS if system == self.animator + off)
            self.calls.append((hex(address), name))

    def state(self):
        pools = {}
        for name, offset, count in POOL_SPECS:
            base = self.vm.u32(self.animator + offset + 4)
            pools[name] = [dict(values=list(struct.unpack('<18f', self.vm.uc.mem_read(base + slot * 88, 72))),
                                flicker=self.vm.u32(base + slot * 88 + 72),
                                life=self.vm.u32(base + slot * 88 + 76),
                                span=self.vm.u32(base + slot * 88 + 80),
                                used=self.vm.u32(base + slot * 88 + 84)) for slot in range(count)]
        return dict(done=self.vm.u32(self.animator + 0x14c), state=self.vm.u32(self.animator + 0x150),
                    frame=self.vm.u32(self.animator + 0x154), alive=not bool(self.vm.u32(self.owner + 8) & 0x1000),
                    random_count=len(self.random), pools=pools)

    def step(self):
        self.calls = []
        self.vm.call(0x4ea060, this=self.animator)


def build_port(output, inputs, ticks):
    source = (ROOT / 'src/effect.cpp').read_text()
    header = (ROOT / 'src/effect.h').read_text()
    start = header.index('    static constexpr int FLAME_COUNT', header.index('class TFireConeEffect_Bespoke'))
    end = header.index('    void SimulateTick();', start)
    fields = header[start:end]
    methods = {name: function(source, signature) for name, signature in (
        ('add', 'void TFireConeEffect_Bespoke::Pool::Add('),
        ('animate', 'void TFireConeEffect_Bespoke::Pool::Animate('),
        ('tick', 'void TFireConeEffect_Bespoke::SimulateTick('))}
    particle_header = (ROOT / 'src/effectcomp.h').read_text()
    particle = function(particle_header, 'struct SParticleSystemInfo') + ';'
    prelude = r'''
#include <cstdio>
#include <fstream>
#include <map>
#include <vector>
#include <stdexcept>
#include "math3d.h"
#undef min
#undef max
std::map<const void*,const char*> pool_names;
std::vector<std::pair<const char*,const char*>> pool_calls;
void Observe(const void*pool,const char*method){pool_calls.emplace_back(method,pool_names.at(pool));}
struct Spell {void Kill(){}};
void log_info(const char*,...){}
std::ifstream inputs;int rng_count=0;
int random(int lo,int hi){int a,b,v;if(!(inputs>>a>>b>>v)||a!=lo||b!=hi)throw std::runtime_error("Native/production RNG order mismatch");++rng_count;return v;}
PARTICLE
struct TFireConeEffect_Bespoke {
FIELDS
Pool fire_{FLAME_COUNT},smoke_{FLAME_COUNT},burst_{FLAME_BURST};
int frame_count_=0,state_=FLAME_STATE_START,sim_ticks_=0;
bool done_=false,alive_=true,runtime_owned_=true;Spell*spell=nullptr;
void SetCommandDone(bool){}void KillThisEffect(){}int GetMapIndex(){return 0;}
void SimulateTick();
};
'''.replace('PARTICLE', particle).replace('FIELDS', fields)
    trailer = r'''
int main(int argc,char**argv){inputs.open(argv[1]);TFireConeEffect_Bespoke e;
pool_names[&e.fire_]="fire";pool_names[&e.smoke_]="smoke";pool_names[&e.burst_]="burst";
for(int tick=0;tick<=TICKS;++tick){pool_calls.clear();if(tick && e.alive_)e.SimulateTick();
printf("S %d %d %d %d %d %d\n",tick,int(e.done_),e.state_,e.frame_count_,int(e.alive_),rng_count);
for(const auto&call:pool_calls)printf("C %d %s %s\n",tick,call.first,call.second);
for(auto item:{std::make_pair("fire",&e.fire_),std::make_pair("smoke",&e.smoke_),std::make_pair("burst",&e.burst_)}){
int slot=0;for(const auto&p:item.second->particles){printf("P %d %s %d %d %d %d %d",tick,item.first,slot++,int(p.used),p.life,p.life_span,int(p.flicker));
for(const auto*v:{&p.pos,&p.scl,&p.rot,&p.acc,&p.vel,&p.temp})printf(" %.9g %.9g %.9g",v->X,v->Y,v->Z);puts("");}}}}
'''.replace('TICKS', str(ticks))
    cpp = output / 'firecone-production.cpp'
    instrumented = dict(methods)
    for name in ('add', 'animate'):
        instrumented[name] = instrumented[name].replace('{', '{Observe(this, \"' + name + '\");', 1)
    cpp.write_text(prelude + '\n'.join(instrumented.values()) + trailer)
    binary = output / 'firecone-production'
    command = ['clang++', '-std=c++17', '-I', str(ROOT / 'thirdparty/handmademath'), '-iquote', str(ROOT / 'src'), str(cpp), '-o', str(binary)]
    result = subprocess.run(command, capture_output=True, text=True)
    (output / 'compile.log').write_text(result.stdout + result.stderr)
    if result.returncode:
        raise RuntimeError('Compiled production leaf failed; see compile.log')
    result = subprocess.run([str(binary), str(inputs)], capture_output=True, text=True)
    (output / 'port-trace.txt').write_text(result.stdout)
    (output / 'port-run.log').write_text(result.stderr)
    if result.returncode:
        raise RuntimeError('Production RNG/state execution failed; see port-run.log')
    frames = []
    for line in result.stdout.splitlines():
        p = line.split()
        if p[0] == 'S':
            frames.append(dict(done=int(p[2]), state=int(p[3]), frame=int(p[4]), alive=bool(int(p[5])), random_count=int(p[6]), calls=[], pools={name: [] for name, _, _ in POOL_SPECS}))
        elif p[0] == 'C':
            frames[-1]['calls'].append((p[2], p[3]))
        else:
            frames[-1]['pools'][p[2]].append(dict(used=int(p[4]), life=int(p[5]), span=int(p[6]), flicker=int(p[7]), values=list(map(float, p[8:]))))
    return frames, dict(source_sha256={name: sha(text.encode()) for name, text in methods.items()}, driver_sha256=sha(cpp.read_bytes()), binary_sha256=sha(binary.read_bytes()), command=command)


def run(executable, output, ticks):
    output.mkdir(parents=True, exist_ok=False)
    native = FireConeFixture(executable)
    states = [native.state()]
    initialization_calls = list(native.calls)
    ordering = []
    for tick in range(1, ticks + 1):
        native.calls = []
        if states[-1]['alive']:
            native.step()
        states.append(native.state())
        ordering.append(dict(tick=tick, calls=native.calls))
    inputs = output / 'native-rng.txt'
    inputs.write_text(''.join('%d %d %d\n' % row for row in native.random))
    candidate, provenance = build_port(output, inputs, ticks)
    if len(states) != ticks + 1 or len(candidate) != ticks + 1:
        raise AssertionError('Incomplete state timeline')
    errors = []; float_errors = []; float_fields = 0
    address_names = {'0x50c1b0': 'animate', '0x50c560': 'add'}
    for tick, (a, b) in enumerate(zip(states, candidate)):
        if tick:
            original_calls = [(address_names[address], name) for address, name in ordering[tick-1]['calls']]
            if original_calls != b['calls']:
                errors.append(dict(tick=tick, key='pool_call_order', original=original_calls, port=b['calls']))
        for key in ('done', 'state', 'frame', 'alive', 'random_count'):
            if a[key] != b[key]:
                errors.append(dict(tick=tick, key=key, original=a[key], port=b[key]))
        for name, _, _ in POOL_SPECS:
            count = next(count for pool_name, _, count in POOL_SPECS if pool_name == name)
            if len(a['pools'][name]) != count or len(b['pools'][name]) != count:
                raise AssertionError('Incomplete pool record coverage')
            for slot, (p, q) in enumerate(zip(a['pools'][name], b['pools'][name])):
                if p['used'] != q['used']:
                    errors.append(dict(tick=tick, pool=name, slot=slot, key='used'))
                if not p['used']:
                    continue
                for key in ('life', 'span', 'flicker'):
                    # Native smoke promotion leaves flicker uninitialized;
                    # it is never used because FireCone Render passes false.
                    if name == 'smoke' and key == 'flicker':
                        continue
                    if p[key] != q[key]:
                        errors.append(dict(tick=tick, pool=name, slot=slot, key=key, original=p[key], port=q[key]))
                for field, (x, y) in enumerate(zip(p['values'], q['values'])):
                    # Burst temp is copied from uninitialized stack storage;
                    # neither its Animate nor Render reads that field.
                    if name == 'burst' and field >= 15:
                        continue
                    x = struct.unpack('<f', struct.pack('<f', x))[0]
                    y = struct.unpack('<f', struct.pack('<f', y))[0]
                    float_fields += 1
                    if struct.pack('<f', x) != struct.pack('<f', y):
                        float_errors.append(dict(tick=tick, pool=name, slot=slot, field=field, original=x, port=y, absolute_difference=abs(x-y)))
    report = dict(status='differences_found' if errors or float_errors else 'pass', executable_sha256=sha(Path(executable).read_bytes()), ticks=ticks,
                  pool_slots=260, state_errors=errors, float_difference_count=len(float_errors), float_fields=float_fields,
                  first_float_differences=float_errors[:30], max_float_difference=max((r['absolute_difference'] for r in float_errors), default=0),
                  native_random_calls=len(native.random), owner_position=list(struct.unpack('<3i', native.vm.uc.mem_read(native.owner + 0x10,12))),
                  initialization_calls=initialization_calls, native_calls=ordering, float_differences_by_field=dict(Counter(f"{r['pool']}.{r['field']}" for r in float_errors)), native_summary=[dict(tick=t, **{k:v for k,v in s.items() if k!='pools'}, active={n:sum(p['used']!=0 for p in ps) for n,ps in s['pools'].items()}) for t,s in enumerate(states)],
                  production=provenance, scope='Null-spell state-only leaf plus genuine shared particle Init/Animate/Add; original CRT allocator and RNG. Full pool occupancy plus every initialized/read active field; stale unused slots, burst temp and smoke flicker have no defined native value and are excluded explicitly. No rendering, live actors, damage, audio or complete frontend credit.')
    (output / 'native-states.json').write_text(json.dumps(states) + '\n')
    (output / 'candidate-states.json').write_text(json.dumps(candidate) + '\n')
    report['native_state_sha256'] = sha((output / 'native-states.json').read_bytes())
    report['candidate_state_sha256'] = sha((output / 'candidate-states.json').read_bytes())
    (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--ticks', type=int, default=100)
    args = parser.parse_args()
    result = run(args.executable, args.output, args.ticks)
    print(json.dumps({k: result[k] for k in ('status','ticks','pool_slots','native_random_calls','float_difference_count','max_float_difference')}))


if __name__ == '__main__':
    main()
