#!/usr/bin/env python3
"""Polyfill for C++26 member annotations until our compilers read them.

Reads one header, finds every class/struct with PROP / PROP_FN annotations,
and emits -- entirely outside the class -- what P2996 + P3394 reflection
would let us write generically: a refl::SType (property table + typed
get/set thunks) registered under the class's type_info, and name tables for
any `enum class` used by an annotated member. See docs/vfx/PREFAB_SYSTEM.md §2.
The class itself carries nothing but the annotations (and, optionally, an
`PropChanged(uint16_t)` hook the setter thunk calls).

The parser is deliberately dumb. It understands exactly:

    class|struct Name [: bases] {        (one per line; nesting tracked by braces)
    enum class Name [: type] { A, B, ... };   (possibly spanning lines)
    PROP(args...) type name [= default];         (one per line; the annotation may
                                                  also sit alone on the line above
                                                  the declaration)
    PROP_FN(id, type, name, Getter, Setter[, args...]);   accessor-backed property:
                                         Getter() returns `type`, Setter(const type&)
                                         (Setter empty -> read-only)

    A property is serialized and visible in the editor by default. The rare
    variants are separate macros: PROP_NOSAVE / PROP_HIDDEN / PROP_READONLY and
    PROP_FN_NOSAVE / PROP_FN_HIDDEN. Annotation args are order-free: an integer
    is the id, a "quoted string" the description, def(literal) the default. When
    the annotation gives no description, the declaration's trailing // comment
    is used (the house style for documenting a member).
    Defaults come from the member initializer and must be simple literals:
    numbers, true / false, a string literal, a brace list of numbers, or an
    enumerator (EType::Name). Named constants, macros and expressions are
    rejected.
    void PropChanged(uint16_t ...);    (presence only; called after a set)
    void PostLoad();                     (presence only; called once a loader has
                                          filled the object -- the post-deserialize
                                          constructor, Unity-style)
    DEF_BLOCK("name") | DEF_COMPONENT("name")   block keyword + factory (one per class)

Anything else is ignored. A PROP line it cannot parse is a build error.
Supported member types are a closed list (see TYPE_MAP), plus nested records:
a member whose type is another annotated class in the same header is a
Struct (`key { }` in a DEF), and std::vector<ThatClass> is a List (repeated
`key [name] { }`; the label fills the record's `name` property). When a real
compiler reads the annotations, delete this file and the CMake hook; the
headers do not change.

Usage: gen.py <input.h> <output.cpp> [--include <path-as-seen-from-output>]
"""
import re
import sys
from pathlib import Path

SRC_ROOT = Path(__file__).resolve().parent.parent.parent / "src"

TYPE_MAP = {
    # C++ type          : (EPropType, SPropValue field, set expression, get expression)
    "bool":        ("Bool",   "b",   "{m} = v.b;",                                   "out = SPropValue::Bool({m});"),
    "int32_t":     ("Int",    "i",   "{m} = v.i;",                                   "out = SPropValue::Int({m});"),
    "int":         ("Int",    "i",   "{m} = v.i;",                                   "out = SPropValue::Int({m});"),
    "uint32_t":    ("Int",    "i",   "{m} = uint32_t(v.i);",                         "out = SPropValue::Int(int32_t({m}));"),
    "float":       ("Float",  "f",   "{m} = v.f;",                                   "out = SPropValue::Float({m});"),
    "std::string": ("String", "str", "{m}.assign(v.str.data(), v.str.size());",       "out = SPropValue::String({m});"),
    "hmm_vec3":    ("Vec3",   "v3",  "{m} = hmm_vec3{{{{v.v3[0], v.v3[1], v.v3[2]}}}};", "out = SPropValue::Vec3({m}.X, {m}.Y, {m}.Z);"),
    "SRgba":       ("Color",  "c4",  "{m} = SRgba{{v.c4[0], v.c4[1], v.c4[2], v.c4[3]}};", "out = SPropValue::Color({m}.r, {m}.g, {m}.b, {m}.a);"),
    # Retail's integer world point rides the Vec3 lanes.
    "S3DPoint":    ("Vec3",   "v3",  "{m} = S3DPoint(int32_t(std::lround(v.v3[0])), int32_t(std::lround(v.v3[1])), int32_t(std::lround(v.v3[2])));",
                                     "out = SPropValue::Vec3(float({m}.x), float({m}.y), float({m}.z));"),
}

RE_CLASS = re.compile(r"^\s*(?:class|struct)\s+([A-Za-z_]\w*)\s*(?:final\s*)?(?::\s*(?:(?:public|protected|private|virtual)\s+)*([\w:]+)[^{;]*)?\{")
RE_ENUM  = re.compile(r"^\s*enum\s+class\s+([A-Za-z_]\w*)\s*(?::\s*[\w:]+)?\s*\{")
RE_DECL_HEAD = re.compile(r"^\s*(?:(?:class|struct)\s+[A-Za-z_]\w*\s*(?:final\s*)?(?::[^{;]*)?|enum\s+class\s+[A-Za-z_]\w*\s*(?::\s*[\w:]+)?)\s*$")
RE_PROP  = re.compile(r"^\s*(PROP|PROP_NOSAVE|PROP_HIDDEN|PROP_READONLY)\((.*?)\)\s+([\w:]+(?:<[\w:]+>)?)\s+([A-Za-z_]\w*)\s*(?:=\s*([^;]*?))?\s*;")
RE_VECTOR = re.compile(r"^std::vector<([A-Za-z_]\w*)>$")
RE_PROP_ANY = re.compile(r"^\s*PROP(_NOSAVE|_HIDDEN|_READONLY)?\(")
RE_PROP_ALONE = re.compile(r"^\s*(PROP|PROP_NOSAVE|PROP_HIDDEN|PROP_READONLY)\((.*)\)\s*$")
RE_PROP_FN  = re.compile(r"^\s*(PROP_FN|PROP_FN_NOSAVE|PROP_FN_HIDDEN)\((.*)\)\s*;?")
RE_HOOK  = re.compile(r"\bvoid\s+PropChanged\s*\(")
RE_POSTLOAD = re.compile(r"\bvoid\s+PostLoad\s*\(\s*\)")
RE_DEFNAME = re.compile(r"^\s*(DEF_BLOCK|DEF_COMPONENT)\(\s*\"([^\"]+)\"\s*\)")


def snake(name: str) -> str:
    s = re.sub(r"(?<=[a-z0-9])(?=[A-Z])", "_", name)
    s = re.sub(r"(?<=[A-Z])(?=[A-Z][a-z])", "_", s)
    return s.lower()


class Member:
    def __init__(self, pid, meta, ctype, name, line_no, getter=None, setter=None, default=None):
        self.id, self.ctype, self.line_no = pid, ctype, line_no
        self.name = name.rstrip("_")          # DEF key / table name
        self.getter, self.setter = getter, setter
        self.desc = meta["desc"]
        self.serialized = meta["serialized"]
        self.visible = meta["visible"]
        self.read_only = meta["read_only"] or (getter is not None and not setter)
        self.default = default                # C++ literal text, or None
        # Expression that reads the value / lvalue-ish target for writes.
        self.read = (getter + "()") if getter else name
        self.member = name


class Klass:
    def __init__(self, name, base):
        self.name = name
        self.base = base or ""
        self.members = []
        self.has_hook = False
        self.has_postload = False
        self.ids = set()
        self.def_name = ""
        self.def_kind = ""      # "node" | "component"


def fail(path, line_no, msg):
    sys.stderr.write(f"{path}:{line_no}: reflect: {msg}\n")
    sys.exit(1)


def split_args(text):
    """Split a macro argument list on top-level commas (respects () {} [] and strings)."""
    out, depth, cur, in_str = [], 0, "", False
    i = 0
    while i < len(text):
        c = text[i]
        if in_str:
            cur += c
            if c == "\\" and i + 1 < len(text):
                cur += text[i + 1]
                i += 1
            elif c == '"':
                in_str = False
        elif c == '"':
            in_str = True
            cur += c
        elif c in "({[":
            depth += 1
            cur += c
        elif c in ")}]":
            depth -= 1
            cur += c
        elif c == "," and depth == 0:
            out.append(cur.strip())
            cur = ""
        else:
            cur += c
        i += 1
    out.append(cur.strip())
    return out if text.strip() else []


def classify_args(macro, args, path, line_no):
    """Order-free annotation args -> (id, meta dict, default_override)."""
    pid, default = None, None
    meta = {"desc": '""',
            "serialized": not macro.endswith("_NOSAVE"),
            "visible": not macro.endswith("_HIDDEN"),
            "read_only": macro.endswith("_READONLY")}
    for a in args:
        if a.isdigit():
            if pid is not None:
                fail(path, line_no, "two ids in one annotation")
            pid = int(a)
        elif a.startswith('"') and a.endswith('"'):
            meta["desc"] = a
        elif a.startswith("def(") and a.endswith(")"):
            default = a[4:-1].strip()
        else:
            fail(path, line_no, "unknown annotation argument `%s` (expected an id, a \"description\" "
                 "or def(literal))" % a)
    if pid is None:
        fail(path, line_no, "annotation needs an integer id")
    return pid, meta, default


RE_NUM = re.compile(r"^[-+]?(?:\d+\.?\d*|\.\d+)(?:[eE][-+]?\d+)?[fFuUlL]*$")
RE_ENUMERATOR = re.compile(r"^[A-Za-z_]\w*::[A-Za-z_]\w*$")


def check_literal(init, ptype, ctype, path, line_no):
    """Enforce the simple-literal rule for defaults. Returns the normalised text."""
    t = init.strip()
    if t in ("{}", "{{}}"):
        return None
    if ptype == "Bool" and t in ("true", "false"):
        return t
    if ptype in ("Int", "Float") and RE_NUM.match(t):
        return t
    if ptype == "String":
        if t.startswith("{") and t.endswith("}"):
            t = t[1:-1].strip()
        if t.startswith('"') and t.endswith('"'):
            return t
    if ptype in ("Vec3", "Color"):
        inner = t.strip("{} ")
        parts = [x.strip() for x in inner.split(",") if x.strip()]
        if parts and all(RE_NUM.match(x) for x in parts):
            return "{" + ", ".join(parts) + "}"
    if ptype == "Enum" and RE_ENUMERATOR.match(t) and t.split("::")[0] == ctype:
        return t
    fail(path, line_no, "default for `%s` must be a simple literal (number, true/false, "
         "\"string\", {numbers} or %s::Enumerator); got `%s`" % (ctype, ctype, init))


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
            if RE_POSTLOAD.search(code):
                cur.has_postload = True
            m_def = RE_DEFNAME.match(code)
            if m_def:
                if cur.def_name:
                    fail(path, i + 1, f"{cur.name} declares more than one DEF_BLOCK / DEF_COMPONENT")
                cur.def_name = m_def.group(2)
                cur.def_kind = "node" if m_def.group(1) == "DEF_BLOCK" else "component"
                i += 1
                continue
            m_fn = RE_PROP_FN.match(code)
            if m_fn:
                raw = split_args(m_fn.group(2))
                # Setter may be empty: `Getter, )` / `Getter, , "desc"`.
                if len(raw) < 5 or not raw[0].isdigit() or not raw[3]:
                    fail(path, i + 1, "cannot parse PROP_FN line; expected `PROP_FN(id, type, name, Getter, Setter[, args...])`")
                setter = raw[4]
                extras = [x for x in raw[5:] if x]
                pid, meta, default = classify_args(m_fn.group(1), [raw[0]] + extras, path, i + 1)
                if pid == 0 or pid > 0x3FFF:
                    fail(path, i + 1, f"PROP_FN id {pid} out of range 1..16383")
                if pid in cur.ids:
                    fail(path, i + 1, f"duplicate PROP id {pid} in {cur.name}")
                cur.ids.add(pid)
                cur.members.append(Member(pid, meta, raw[1], raw[2], i + 1,
                                          getter=raw[3], setter=setter or None, default=default))
                i += 1
                continue
            # Annotation alone on a line: it applies to the next declaration.
            if RE_PROP_ALONE.match(code):
                j = i + 1
                while j < len(lines) and lines[j].split("//", 1)[0].strip() == "":
                    j += 1
                if j < len(lines):
                    code = code.rstrip() + " " + lines[j].split("//", 1)[0].strip()
                    i = j
            trailing = line.split("//", 1)[1].strip() if "//" in line and not line.lstrip().startswith("//") else ""
            m_prop = RE_PROP.match(code)
            if m_prop:
                pid, meta, override = classify_args(m_prop.group(1), split_args(m_prop.group(2)), path, i + 1)
                if meta["desc"] == '""' and trailing:
                    meta["desc"] = '"' + trailing.replace("\\", "\\\\").replace('"', '\\"') + '"'
                if pid == 0 or pid > 0x3FFF:
                    fail(path, i + 1, f"PROP id {pid} out of range 1..16383")
                if pid in cur.ids:
                    fail(path, i + 1, f"duplicate PROP id {pid} in {cur.name}")
                cur.ids.add(pid)
                init = m_prop.group(5).strip() if m_prop.group(5) else None
                cur.members.append(Member(pid, meta, m_prop.group(3), m_prop.group(4), i + 1,
                                          default=override if override is not None else init))
            elif RE_PROP_ANY.match(code):
                fail(path, i + 1, "cannot parse PROP line; expected `PROP(args...) type name [= literal];`")
        depth += code.count("{") - code.count("}")
        while stack and depth <= stack[-1][1]:
            stack.pop()
        i += 1
    return classes, enums


def default_expr(m, ptype, path):
    """SPropValue factory expression for the member's default (literals only)."""
    if ptype in ("List", "Struct"):
        return "SPropValue()"
    init = check_literal(m.default, ptype, m.ctype, path, m.line_no) if m.default is not None else None
    if ptype == "Enum":
        return "SPropValue::Enum(int32_t(%s))" % (init if init else "%s(0)" % m.ctype)
    if ptype == "Bool":
        return "SPropValue::Bool(%s)" % (init if init else "false")
    if ptype == "Int":
        return "SPropValue::Int(%s)" % (init if init else "0")
    if ptype == "Float":
        return "SPropValue::Float(%s)" % (init if init else "0.0f")
    if ptype == "String":
        return "SPropValue::String(%s)" % (init if init else '""')
    if ptype in ("Vec3", "Color"):
        lanes = 3 if ptype == "Vec3" else 4
        if init is None:
            nums = ["0", "0", "0"] if ptype == "Vec3" else ["1", "1", "1", "1"]
        else:
            nums = [x.strip() for x in init.strip("{}").split(",")]
            if len(nums) == 3 and ptype == "Color":
                nums.append("1")
            if len(nums) != lanes:
                fail(path, m.line_no, "%s default must list %d numbers: %s" % (ptype, lanes, m.default))
        return "SPropValue::%s(%s)" % (ptype, ", ".join("float(%s)" % n.rstrip("fF") for n in nums))
    return "SPropValue()"


RE_INCLUDE = re.compile(r'^\s*#\s*include\s+"([^"]+)"')


def included_annotated(path: Path, seen=None):
    """Names of annotated classes declared in headers this one includes
    (quoted includes resolved next to the header, recursively), so a member
    of such a type is a Struct / List here."""
    if seen is None:
        seen = set()
    names = set()
    for line in path.read_text(encoding="utf-8").splitlines():
        m = RE_INCLUDE.match(line)
        if not m:
            continue
        # Includes are spelled from src/ ("engine/fx/particledef.h"); the
        # header's own folder is tried first for the rare relative one.
        dep = (path.parent / m.group(1)).resolve()
        if not dep.is_file():
            dep = (SRC_ROOT / m.group(1)).resolve()
        if dep in seen or not dep.is_file():
            continue
        seen.add(dep)
        try:
            dep_classes, _ = parse(dep)
        except SystemExit:
            continue
        names.update(k.name for k in dep_classes if k.members)
        names.update(included_annotated(dep, seen))
    return names


def emit(path: Path, out: Path, include: str):
    classes, enums = parse(path)
    targets = [k for k in classes if k.members or k.def_name]
    used_enums = {}
    o = []
    o.append("// GENERATED by tools/reflect/gen.py from %s -- do not edit.\n" % path.name)
    o.append("// Polyfill for C++26 member annotations (docs/vfx/PREFAB_SYSTEM.md §2).\n")
    o.append('#include "%s"\n' % include)
    o.append('#include "engine/refl/refl.h"\n\n')
    o.append("#include <cmath>\n#include <cstring>\n\nnamespace {\n")
    for k in targets:
        for m in k.members:
            if m.ctype in enums and m.ctype not in used_enums:
                used_enums[m.ctype] = enums[m.ctype]
    for ename, names in used_enums.items():
        o.append("const refl::SEnumOption kEnum_%s[] = {\n" % ename)
        for n in names:
            o.append('    { int32_t(%s::%s), "%s" },\n' % (ename, n, snake(n)))
        o.append("};\n")
    annotated = {k.name for k in classes if k.members} | included_annotated(path)

    def kind_of(m):
        """-> (ptype, elem_class or None)"""
        if m.ctype in TYPE_MAP:
            return TYPE_MAP[m.ctype][0], None
        if m.ctype in enums:
            return "Enum", None
        mv = RE_VECTOR.match(m.ctype)
        if mv and mv.group(1) in annotated:
            return "List", mv.group(1)
        if m.ctype in annotated:
            return "Struct", m.ctype
        fail(path, m.line_no, f"unsupported PROP type `{m.ctype}` (closed list: "
             + ", ".join(TYPE_MAP) + ", an enum class, an annotated class, or std::vector of one, "
             "all declared in this header)")

    for k in targets:
        if not k.members:
            continue
        o.append("const refl::SProp kProps_%s[] = {\n" % k.name)
        for m in k.members:
            ptype, elem = kind_of(m)
            if ptype == "Enum":
                opts, nopts = "kEnum_%s" % m.ctype, str(len(enums[m.ctype]))
            else:
                opts, nopts = "nullptr", "0"
            if ptype in ("List", "Struct") and m.getter:
                fail(path, m.line_no, "nested records cannot be accessor-backed (PROP_FN)")
            o.append('    { %d, "%s", %s, EPropType::%s, %s, %s, %s, %s, %s, %s, "%s" },\n'
                     % (m.id, m.name, m.desc, ptype,
                        "true" if m.serialized else "false",
                        "true" if m.visible else "false",
                        "true" if m.read_only else "false",
                        default_expr(m, ptype, path), opts, nopts, elem or ""))
        o.append("};\n\n")

        o.append("} // namespace\n\n")
        nested_members = [m for m in k.members if kind_of(m)[0] in ("List", "Struct")]
        o.append("template <> struct refl::Access<%s>\n{\n" % k.name)
        if k.members:
            o.append("    static bool Set(void* obj, uint16_t id, const SPropValue& v);\n")
            o.append("    static bool Get(const void* obj, uint16_t id, SPropValue& out);\n")
        if nested_members:
            o.append("    static void*   Nested(void* obj, uint16_t id);\n")
            o.append("    static int32_t ListCount(const void* obj, uint16_t id);\n")
            o.append("    static void*   ListAt(void* obj, uint16_t id, int32_t index);\n")
            o.append("    static void*   ListAdd(void* obj, uint16_t id);\n")
            o.append("    static void    ListClear(void* obj, uint16_t id);\n")
        if k.has_postload:
            o.append("    static void PostLoad(void* obj) { static_cast<%s*>(obj)->PostLoad(); }\n" % k.name)
        o.append("};\n\n")
        if k.members:
          o.append("bool refl::Access<%s>::Set(void* obj, uint16_t id, const SPropValue& v)\n{\n" % k.name)
          o.append("    %s& self = *static_cast<%s*>(obj);\n" % (k.name, k.name))
          o.append("    switch (id)\n    {\n")
          for m in k.members:
              if m.read_only or kind_of(m)[0] in ("List", "Struct"):
                  o.append("        case %d: return false;   // read-only / nested\n" % m.id)
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
              o.append("    self.PropChanged(id);\n")
          o.append("    return true;\n}\n\n")

          o.append("bool refl::Access<%s>::Get(const void* obj, uint16_t id, SPropValue& out)\n{\n" % k.name)
          o.append("    const %s& self = *static_cast<const %s*>(obj);\n" % (k.name, k.name))
          o.append("    switch (id)\n    {\n")
          for m in k.members:
              read = "self." + m.read
              if kind_of(m)[0] in ("List", "Struct"):
                  continue
              if m.ctype in TYPE_MAP:
                  if m.getter and m.ctype in ("hmm_vec3", "SRgba", "S3DPoint"):
                      o.append("        case %d: { const %s _g = %s; %s return true; }\n"
                               % (m.id, m.ctype, read, TYPE_MAP[m.ctype][3].format(m="_g")))
                  else:
                      o.append("        case %d: %s return true;\n" % (m.id, TYPE_MAP[m.ctype][3].format(m=read)))
              else:
                  o.append("        case %d: out = SPropValue::Enum(int32_t(%s)); return true;\n" % (m.id, read))
          o.append("        default: return false;\n    }\n}\n\n")

        if nested_members:
            structs = [m for m in nested_members if kind_of(m)[0] == "Struct"]
            lists   = [m for m in nested_members if kind_of(m)[0] == "List"]
            o.append("void* refl::Access<%s>::Nested(void* obj, uint16_t id)\n{\n" % k.name)
            o.append("    %s& self = *static_cast<%s*>(obj); (void)self;\n    switch (id)\n    {\n" % (k.name, k.name))
            for m in structs:
                o.append("        case %d: return &self.%s;\n" % (m.id, m.member))
            o.append("        default: return nullptr;\n    }\n}\n")
            o.append("int32_t refl::Access<%s>::ListCount(const void* obj, uint16_t id)\n{\n" % k.name)
            o.append("    const %s& self = *static_cast<const %s*>(obj); (void)self;\n    switch (id)\n    {\n" % (k.name, k.name))
            for m in lists:
                o.append("        case %d: return int32_t(self.%s.size());\n" % (m.id, m.member))
            o.append("        default: return 0;\n    }\n}\n")
            o.append("void* refl::Access<%s>::ListAt(void* obj, uint16_t id, int32_t index)\n{\n" % k.name)
            o.append("    %s& self = *static_cast<%s*>(obj); (void)self; (void)index;\n    switch (id)\n    {\n" % (k.name, k.name))
            for m in lists:
                o.append("        case %d: return (index >= 0 && index < int32_t(self.%s.size())) ? &self.%s[size_t(index)] : nullptr;\n" % (m.id, m.member, m.member))
            o.append("        default: return nullptr;\n    }\n}\n")
            o.append("void* refl::Access<%s>::ListAdd(void* obj, uint16_t id)\n{\n" % k.name)
            o.append("    %s& self = *static_cast<%s*>(obj); (void)self;\n    switch (id)\n    {\n" % (k.name, k.name))
            for m in lists:
                o.append("        case %d: self.%s.emplace_back(); return &self.%s.back();\n" % (m.id, m.member, m.member))
            o.append("        default: return nullptr;\n    }\n}\n")
            o.append("void refl::Access<%s>::ListClear(void* obj, uint16_t id)\n{\n" % k.name)
            o.append("    %s& self = *static_cast<%s*>(obj); (void)self;\n    switch (id)\n    {\n" % (k.name, k.name))
            for m in lists:
                o.append("        case %d: self.%s.clear(); break;\n" % (m.id, m.member))
            o.append("        default: break;\n    }\n}\n\n")
        o.append("namespace {\n")
        props = ("kProps_%s" % k.name) if k.members else "nullptr"
        setf  = ("&refl::Access<%s>::Set" % k.name) if k.members else "nullptr"
        getf  = ("&refl::Access<%s>::Get" % k.name) if k.members else "nullptr"
        cnode = ("[]() -> TNode* { return new %s(); }" % k.name) if k.def_kind == "node" else "nullptr"
        ccomp = ("[]() -> TComponent* { return new %s(); }" % k.name) if k.def_kind == "component" else "nullptr"
        loaded = ("&refl::Access<%s>::PostLoad" % k.name) if k.has_postload else "nullptr"
        if nested_members:
            nest = ", ".join("&refl::Access<%s>::%s" % (k.name, fn) for fn in ("Nested", "ListCount", "ListAt", "ListAdd", "ListClear"))
        else:
            nest = "nullptr, nullptr, nullptr, nullptr, nullptr"
        o.append('refl::SType kType_%s = { "%s", "%s", &typeid(%s), %s, %d, %s, %s, nullptr, "%s", %s, %s, %s, %s };\n'
                 % (k.name, k.name, k.base, k.name, props, len(k.members), setf, getf, k.def_name, cnode, ccomp, loaded, nest))
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
