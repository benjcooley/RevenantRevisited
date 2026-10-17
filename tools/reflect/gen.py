#!/usr/bin/env python3
"""Polyfill for C++26 member annotations until our compilers read them.

Reads one header, finds every class/struct with PROP / PROP_FN annotations,
and emits -- entirely outside the class -- what P2996 + P3394 reflection
would let us write generically: a refl::SType (property table + typed
get/set thunks) registered under the class's type_info, and name tables for
any `enum class` used by an annotated member. See docs/vfx/PREFAB_SYSTEM.md §2.
The class itself carries nothing but the annotations (and, optionally, an
`OnPropChanged(uint16_t)` hook the setter thunk calls).

The parser is deliberately dumb. It understands exactly:

    class|struct Name [: bases] {        (one per line; nesting tracked by braces)
    enum class Name [: type] { A, B, ... };   (possibly spanning lines)
    PROP(id[, flags]) type name [= default];  (one per line)
    PROP_FN(id, type, name, Getter, Setter[, flags]);   accessor-backed property:
                                         Getter() returns `type`, Setter(const type&)
                                         (Setter empty -> read-only)
    void OnPropChanged(uint16_t ...);    (presence only)

Anything else is ignored. A PROP line it cannot parse is a build error.
Supported member types are a closed list (see TYPE_MAP). When a real
compiler reads the annotations, delete this file and the CMake hook; the
headers do not change.

Usage: gen.py <input.h> <output.cpp> [--include <path-as-seen-from-output>]
"""
import re
import sys
from pathlib import Path

TYPE_MAP = {
    # C++ type          : (EPropType, SPropValue field, set expression, get expression)
    "bool":        ("Bool",   "b",   "{m} = v.b;",                                   "out = SPropValue::Bool({m});"),
    "int32_t":     ("Int",    "i",   "{m} = v.i;",                                   "out = SPropValue::Int({m});"),
    "int":         ("Int",    "i",   "{m} = v.i;",                                   "out = SPropValue::Int({m});"),
    "float":       ("Float",  "f",   "{m} = v.f;",                                   "out = SPropValue::Float({m});"),
    "std::string": ("String", "str", "{m}.assign(v.str.data(), v.str.size());",       "out = SPropValue::String({m});"),
    "hmm_vec3":    ("Vec3",   "v3",  "{m} = hmm_vec3{{{{v.v3[0], v.v3[1], v.v3[2]}}}};", "out = SPropValue::Vec3({m}.X, {m}.Y, {m}.Z);"),
    "SRgba":       ("Color",  "c4",  "{m} = SRgba{{v.c4[0], v.c4[1], v.c4[2], v.c4[3]}};", "out = SPropValue::Color({m}.r, {m}.g, {m}.b, {m}.a);"),
}

RE_CLASS = re.compile(r"^\s*(?:class|struct)\s+([A-Za-z_]\w*)\s*(?::\s*(?:(?:public|protected|private|virtual)\s+)*([\w:]+)[^{;]*)?\{")
RE_ENUM  = re.compile(r"^\s*enum\s+class\s+([A-Za-z_]\w*)\s*(?::\s*[\w:]+)?\s*\{")
RE_DECL_HEAD = re.compile(r"^\s*(?:(?:class|struct)\s+[A-Za-z_]\w*\s*(?::[^{;]*)?|enum\s+class\s+[A-Za-z_]\w*\s*(?::\s*[\w:]+)?)\s*$")
RE_PROP  = re.compile(r"^\s*PROP\(\s*(\d+)\s*(?:,\s*([^)]+?)\s*)?\)\s+([\w:]+)\s+([A-Za-z_]\w*)\s*(?:=\s*[^;]*)?;")
RE_PROP_ANY = re.compile(r"^\s*PROP\(")
RE_PROP_FN  = re.compile(r"^\s*PROP_FN\(([^)]*)\)\s*;?")
RE_HOOK  = re.compile(r"\bvoid\s+OnPropChanged\s*\(")


def snake(name: str) -> str:
    s = re.sub(r"(?<=[a-z0-9])(?=[A-Z])", "_", name)
    s = re.sub(r"(?<=[A-Z])(?=[A-Z][a-z])", "_", s)
    return s.lower()


class Member:
    def __init__(self, pid, flags, ctype, name, line_no, getter=None, setter=None):
        self.id, self.flags, self.ctype, self.line_no = pid, flags, ctype, line_no
        self.name = name.rstrip("_")          # DEF key / table name
        self.getter, self.setter = getter, setter
        # Expression that reads the value / lvalue-ish target for writes.
        self.read = (getter + "()") if getter else name
        self.member = name

    @property
    def read_only(self):
        return "PropFlag_ReadOnly" in self.flags or (self.getter is not None and not self.setter)


class Klass:
    def __init__(self, name, base):
        self.name = name
        self.base = base or ""
        self.members = []
        self.has_hook = False
        self.ids = set()


def fail(path, line_no, msg):
    sys.stderr.write(f"{path}:{line_no}: reflect: {msg}\n")
    sys.exit(1)


def parse(path: Path):
    text = path.read_text(encoding="utf-8")
    lines = text.splitlines()
    classes, enums = [], {}
    stack = []           # (Klass or None, depth at which its body opened)
    depth = 0
    i = 0
    while i < len(lines):
        line = lines[i]
        code = line.split("//", 1)[0]
        # Allman style: `struct Name : Base` with the `{` on the next line.
        # Join forward until the brace so the regexes below see one line.
        # A `;` means it was a forward declaration; leave it alone.
        if RE_DECL_HEAD.match(code):
            j = i + 1
            joined = code
            while j < len(lines):
                nxt = lines[j].split("//", 1)[0]
                if nxt.strip() == "":
                    j += 1
                    continue
                if nxt.lstrip().startswith("{"):
                    joined = joined.rstrip() + " " + nxt
                    i = j
                    code = joined
                break
        m_enum = RE_ENUM.match(code)
        if m_enum:
            body = code[m_enum.end():]
            j = i
            while "}" not in body:
                j += 1
                if j >= len(lines):
                    fail(path, i + 1, "unterminated enum class")
                body += " " + lines[j].split("//", 1)[0]
            body = body[: body.index("}")]
            names = []
            for item in body.split(","):
                item = item.strip()
                if not item:
                    continue
                names.append(item.split("=", 1)[0].strip())
            enums[m_enum.group(1)] = names
            i = j + 1
            continue
        m_class = RE_CLASS.match(code)
        if m_class:
            k = Klass(m_class.group(1), m_class.group(2))
            stack.append((k, depth))
            classes.append(k)
            depth += code.count("{") - code.count("}")
            i += 1
            continue
        cur = stack[-1][0] if stack else None
        if cur is not None:
            if RE_HOOK.search(code):
                cur.has_hook = True
            m_fn = RE_PROP_FN.match(code)
            if m_fn:
                parts = [x.strip() for x in m_fn.group(1).split(",")]
                if len(parts) not in (5, 6) or not parts[0].isdigit() or not parts[3]:
                    fail(path, i + 1, "cannot parse PROP_FN line; expected `PROP_FN(id, type, name, Getter, Setter[, flags])`")
                pid = int(parts[0])
                if pid == 0 or pid > 0x3FFF:
                    fail(path, i + 1, f"PROP_FN id {pid} out of range 1..16383")
                if pid in cur.ids:
                    fail(path, i + 1, f"duplicate PROP id {pid} in {cur.name}")
                cur.ids.add(pid)
                flags = parts[5] if len(parts) == 6 else "PropFlag_None"
                if not parts[4] and "PropFlag_ReadOnly" not in flags:
                    flags = flags + " | PropFlag_ReadOnly" if flags != "PropFlag_None" else "PropFlag_ReadOnly"
                cur.members.append(Member(pid, flags, parts[1], parts[2], i + 1,
                                          getter=parts[3], setter=parts[4] or None))
                i += 1
                continue
            m_prop = RE_PROP.match(code)
            if m_prop:
                pid = int(m_prop.group(1))
                if pid == 0 or pid > 0x3FFF:
                    fail(path, i + 1, f"PROP id {pid} out of range 1..16383")
                if pid in cur.ids:
                    fail(path, i + 1, f"duplicate PROP id {pid} in {cur.name}")
                cur.ids.add(pid)
                cur.members.append(Member(pid, m_prop.group(2) or "PropFlag_None",
                                          m_prop.group(3), m_prop.group(4), i + 1))
            elif RE_PROP_ANY.match(code):
                fail(path, i + 1, "cannot parse PROP line; expected `PROP(id[, flags]) type name [= default];`")
        depth += code.count("{") - code.count("}")
        while stack and depth <= stack[-1][1]:
            stack.pop()
        i += 1
    return classes, enums


def emit(path: Path, out: Path, include: str):
    classes, enums = parse(path)
    targets = [k for k in classes if k.members]
    used_enums = {}
    o = []
    o.append("// GENERATED by tools/reflect/gen.py from %s -- do not edit.\n" % path.name)
    o.append("// Polyfill for C++26 member annotations (docs/vfx/PREFAB_SYSTEM.md §2).\n")
    o.append('#include "%s"\n' % include)
    refl_h = (Path(__file__).resolve().parent.parent.parent / "src" / "refl.h").as_posix()
    o.append('#include "%s"\n\n' % refl_h)
    o.append("#include <cstring>\n\nnamespace {\n")
    for k in targets:
        for m in k.members:
            if m.ctype in enums and m.ctype not in used_enums:
                used_enums[m.ctype] = enums[m.ctype]
    for ename, names in used_enums.items():
        o.append("const refl::SEnumOption kEnum_%s[] = {\n" % ename)
        for n in names:
            o.append('    { int32_t(%s::%s), "%s" },\n' % (ename, n, snake(n)))
        o.append("};\n")
    for k in targets:
        o.append("const refl::SProp kProps_%s[] = {\n" % k.name)
        for m in k.members:
            if m.ctype in TYPE_MAP:
                ptype = TYPE_MAP[m.ctype][0]
                opts, nopts = "nullptr", "0"
            elif m.ctype in enums:
                ptype = "Enum"
                opts, nopts = "kEnum_%s" % m.ctype, str(len(enums[m.ctype]))
            else:
                fail(path, m.line_no, f"unsupported PROP type `{m.ctype}` (closed list: "
                     + ", ".join(TYPE_MAP) + ", or an enum class declared in this header)")
            o.append('    { %d, "%s", EPropType::%s, uint8_t(%s), %s, %s },\n'
                     % (m.id, m.name, ptype, m.flags, opts, nopts))
        o.append("};\n\n")

        o.append("bool Set_%s(void* obj, uint16_t id, const SPropValue& v)\n{\n" % k.name)
        o.append("    %s& self = *static_cast<%s*>(obj);\n" % (k.name, k.name))
        o.append("    switch (id)\n    {\n")
        for m in k.members:
            if m.read_only:
                o.append("        case %d: return false;   // read-only\n" % m.id)
                continue
            if m.setter:
                if m.ctype in TYPE_MAP:
                    ptype, _, setexpr, _ = TYPE_MAP[m.ctype]
                    if ptype == "Float":
                        conv = "float _v; if (v.type == EPropType::Int) _v = float(v.i); else if (v.type == EPropType::Float) _v = v.f; else return false;"
                    elif ptype == "Int":
                        conv = "int32_t _v; if (v.type == EPropType::Float) _v = int32_t(v.f); else if (v.type == EPropType::Int || v.type == EPropType::Enum) _v = v.i; else return false;"
                    else:
                        conv = "if (v.type != EPropType::%s) return false; %s _v; %s" % (ptype, m.ctype, setexpr.format(m="_v"))
                else:
                    conv = "if (v.type != EPropType::Enum && v.type != EPropType::Int) return false; %s _v = %s(v.i);" % (m.ctype, m.ctype)
                o.append("        case %d: { %s self.%s(_v); break; }\n" % (m.id, conv, m.setter))
                continue
            tgt = "self." + m.member
            if m.ctype in TYPE_MAP:
                ptype, _, setexpr, _ = TYPE_MAP[m.ctype]
                if ptype == "Float":
                    o.append("        case %d: if (v.type == EPropType::Int) %s = float(v.i); else if (v.type == EPropType::Float) %s = v.f; else return false; break;\n" % (m.id, tgt, tgt))
                elif ptype == "Int":
                    o.append("        case %d: if (v.type == EPropType::Float) %s = int32_t(v.f); else if (v.type == EPropType::Int || v.type == EPropType::Enum) %s = v.i; else return false; break;\n" % (m.id, tgt, tgt))
                else:
                    o.append("        case %d: if (v.type != EPropType::%s) return false; %s break;\n" % (m.id, ptype, setexpr.format(m=tgt)))
            else:
                o.append("        case %d: if (v.type != EPropType::Enum && v.type != EPropType::Int) return false; %s = %s(v.i); break;\n" % (m.id, tgt, m.ctype))
        o.append("        default: return false;\n    }\n")
        if k.has_hook:
            o.append("    self.OnPropChanged(id);\n")
        o.append("    return true;\n}\n\n")

        o.append("bool Get_%s(const void* obj, uint16_t id, SPropValue& out)\n{\n" % k.name)
        o.append("    const %s& self = *static_cast<const %s*>(obj);\n" % (k.name, k.name))
        o.append("    switch (id)\n    {\n")
        for m in k.members:
            read = "self." + m.read
            if m.ctype in TYPE_MAP:
                if m.getter and m.ctype in ("hmm_vec3", "SRgba"):
                    o.append("        case %d: { const %s _g = %s; %s return true; }\n"
                             % (m.id, m.ctype, read, TYPE_MAP[m.ctype][3].format(m="_g")))
                else:
                    o.append("        case %d: %s return true;\n" % (m.id, TYPE_MAP[m.ctype][3].format(m=read)))
            else:
                o.append("        case %d: out = SPropValue::Enum(int32_t(%s)); return true;\n" % (m.id, read))
        o.append("        default: return false;\n    }\n}\n\n")

        o.append('refl::SType kType_%s = { "%s", "%s", &typeid(%s), kProps_%s, %d, &Set_%s, &Get_%s, nullptr };\n'
                 % (k.name, k.name, k.base, k.name, k.name, len(k.members), k.name, k.name))
        o.append("refl::SRegistrar kReg_%s(kType_%s);\n\n" % (k.name, k.name))
    o.append("} // namespace\n")
    out.parent.mkdir(parents=True, exist_ok=True)
    new_text = "".join(o)
    if out.exists() and out.read_text(encoding="utf-8") == new_text:
        return
    out.write_text(new_text, encoding="utf-8")


def main(argv):
    if len(argv) < 3:
        sys.stderr.write(__doc__)
        return 2
    src, dst = Path(argv[1]), Path(argv[2])
    include = str(src.resolve())
    if "--include" in argv:
        include = argv[argv.index("--include") + 1]
    emit(src, dst, include)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
