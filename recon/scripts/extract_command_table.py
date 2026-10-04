import re, sys, json
path = sys.argv[1]
mem = {}
labels = {}   # addr -> (label, strval)
cur = None
addr_re = re.compile(r'^\s+([0-9a-f]{8})\s+((?:[0-9a-f]{2}\s{1,2})+)\s*(\S+)?\s*(.*)$')
cont_re = re.compile(r'^\s{10,}((?:[0-9a-f]{2}\s*)+)$')
lines = open(path, errors='replace').read().split('\n')
for ln in lines:
    m = addr_re.match(ln)
    if m:
        a = int(m.group(1), 16)
        if not (0x5c0000 <= a < 0x5d0000):
            cur = None; continue
        bs = m.group(2).split()
        for i, b in enumerate(bs):
            mem[a+i] = int(b, 16)
        cur = a + len(bs)
        kind = m.group(3); rest = m.group(4)
        if kind == 'addr':
            mm = re.match(r'(\S+)\s*(?:=\s*"(.*)")?', rest)
            if mm:
                labels[a] = (mm.group(1), mm.group(2))
        elif kind == 'ds':
            mm = re.match(r'"(.*)"', rest.strip())
            if mm: labels.setdefault(('ds', a), mm.group(1))
        continue
    m = cont_re.match(ln)
    if m and cur is not None:
        bs = m.group(1).split()
        for i, b in enumerate(bs):
            mem[cur+i] = int(b, 16)
        cur += len(bs)
        continue
    cur = None
def dw(a):
    return int.from_bytes(bytes(mem.get(a+i,0) for i in range(4)), 'little', signed=True)
def cstr(p):
    s=b''
    while mem.get(p,0)!=0 and len(s)<200:
        s+=bytes([mem[p]]); p+=1
    return s.decode('latin1')
# find start: scan back from addinv entry
start = int(sys.argv[2], 16); end = int(sys.argv[3], 16)
out = []
a = start
while a < end:
    p = dw(a)
    if not (0x5c0000 <= p < 0x5d0000): break
    lab = labels.get(a)
    name = lab[1] if (lab and lab[1]) else cstr(p)
    fn = labels.get(a+4)
    us = labels.get(a+24)
    if fn is None: fn = ('raw_%08x' % (dw(a+4) & 0xffffffff), None)
    out.append(dict(addr=hex(a), name=name, func=(fn[0] if fn else hex(dw(a+4))),
                    cc=dw(a+8), cc2=dw(a+12), reqp=dw(a+16), edonly=dw(a+20),
                    usage=(labels.get(a+24) or (None,None))[1] or cstr(dw(a+24))))
    a += 28
for e in out:
    print(f"{e['addr']}  {e['name']:<22} {e['func']:<14} cc={e['cc']:>3} cc2={e['cc2']:>3} req={e['reqp']} ed={e['edonly']}  {e['usage']!r}")
print(len(out), 'entries', file=sys.stderr)
