#!/usr/bin/env python3
# Build a grep-friendly index over recon/classes/*.cpp for retail-sync class ID work.
#
# Walks every cls_0xADDR*.cpp file and emits one TSV with rows:
#     cls_0xADDR<TAB>kind<TAB>count<TAB>value
# kinds:
#   str     Ghidra string-symbol slug (s_<slug>_<VA>)
#   api     Win32 / CRT import call (CreateFontA, _strncpy, ...)
#   num     numeric literal (hex or decimal >= threshold)
#   meth    method decl, value = meth_0xADDR:<sig-code>:<line-count>
#   sum     one per class: fields=<N> methods=<N> lines=<N>
#
# Signature code normalizes arg types: d=dword/int, p=pointer, b=byte/bool,
# w=word, q=qword/double, f=float, s=struct/cls_, ?=unknown. Return type
# is the first code (e.g. "d(d,p,p)" = int method(int,T*,T*)).

from __future__ import annotations
import os, re, sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]  # recon/
CLASSES_DIR = ROOT / "classes"
OUT = ROOT / "analysis" / "class_index.tsv"

# Match "cls_0xHEX" at start of filename so "cls_0xADDR_TName.cpp" and
# "cls_0xADDR__vftable_...cpp" both bucket under the same ADDR.
CLS_RE = re.compile(r"^(cls_0x[0-9a-fA-F]+)")

# String-symbol: s_<slug>_<8-hex-VA>. Keep slug; drop VA since it's an .rdata offset.
STR_RE = re.compile(r"\bs_([A-Za-z0-9_]+?)_([0-9a-f]{6,8})\b")

# Win32 API / CRT call: identifier that is NOT a Ghidra-generated synthetic.
# Accept: MixedCase ending in capital (Win32), or _lowercase (CRT), or plain
# all-lowercase CRT names used without underscore in-call (rare).
API_RE = re.compile(r"\b(?:[A-Z][A-Za-z0-9]{2,}[A-Z]|_[a-z][a-z0-9_]{1,})\b")
SYNTHETIC_PREFIXES = ("DAT_", "FUN_", "PTR_", "LAB_", "UNK_", "SUB_", "EXT_")

# Numeric literals. Hex 0x... with >=2 digits, or decimal >= 16.
NUM_HEX_RE = re.compile(r"\b0x[0-9a-fA-F]{2,}\b")
NUM_DEC_RE = re.compile(r"(?<![\w.])([0-9]{2,})\b")

# Method decl. The Ghidra decomp often wraps long signatures onto multiple
# lines, so match across newlines. We anchor to "__thiscall OOAnalyzer::cls_"
# and capture return-type token (the identifier just before __thiscall),
# meth id, and the full parenthesized arg list.
METH_RE = re.compile(
    r"([A-Za-z_][\w\s\*]*?)\s+__thiscall\s+OOAnalyzer::cls_0x[0-9a-fA-F]+::(meth_0x[0-9a-fA-F]+)\s*\(([^)]*)\)",
    re.DOTALL,
)

# Struct field decl line inside the header block.
FIELD_RE = re.compile(r"^\s*\d+\s+\w+\s+\d+\s+mbr_0x[0-9a-fA-F]+")


# ---- numeric-literal filter ----------------------------------------------
# Drop noise that won't discriminate classes.
NOISE_HEX = {
    "0x0", "0x1", "0x2", "0x3", "0x4", "0x5", "0x6", "0x7",
    "0x8", "0x9", "0xa", "0xb", "0xc", "0xd", "0xe", "0xf",
    "0xff", "0x00", "0x01", "0x0f", "0x10", "0x20", "0x40", "0x80",
    "0xffff", "0xffffffff",
}

def keep_num(n: str) -> bool:
    if n.startswith("0x") or n.startswith("0X"):
        return n.lower() not in NOISE_HEX
    # decimal
    try:
        v = int(n)
    except ValueError:
        return False
    # Powers of 2 up to 1024 show up as sizes/flags everywhere — noisy.
    if v in (16, 32, 64, 100, 128, 256, 512, 1000, 1024):
        return False
    return v >= 16


# ---- signature normalization --------------------------------------------
def sig_code(ret: str, args: str) -> str:
    def code(tok: str) -> str:
        t = tok.strip().lower()
        if not t: return "?"
        if "*" in t: return "p"
        if t.startswith("cls_") or "struct" in t: return "s"
        if "float"  in t: return "f"
        if "double" in t or "longlong" in t or "__int64" in t or t.startswith("qword"): return "q"
        if t.startswith("byte") or "bool" in t or t == "char" or t.startswith("undefined1"): return "b"
        if t.startswith("word") or t.startswith("short") or t.startswith("undefined2"): return "w"
        if "void" in t and "*" not in t: return "v"
        if any(k in t for k in ("int","dword","uint","long","undefined","undefined4")): return "d"
        return "?"

    rc = code(ret)
    parts = []
    for raw in args.split(","):
        raw = raw.strip()
        if not raw: continue
        # drop trailing param name: e.g. "cls_0x... *this" -> type is "cls_0x... *"
        # strip name = last identifier if not a type keyword
        toks = raw.split()
        if len(toks) > 1 and re.match(r"[A-Za-z_]\w*$", toks[-1]) and "*" not in toks[-1]:
            toks = toks[:-1]
        type_str = " ".join(toks)
        parts.append(code(type_str))
    # drop the leading "this" (always a pointer to own class)
    if parts and parts[0] == "p":
        parts = parts[1:]
    return f"{rc}({','.join(parts)})"


# ---- main ----------------------------------------------------------------
def process_file(path: Path) -> tuple[str, dict]:
    """Return (cls_id, stats_dict)."""
    m = CLS_RE.match(path.name)
    if not m:
        return None, None
    cls = m.group(1)

    text = path.read_text(errors="ignore")
    lines = text.splitlines()

    strings = Counter()
    apis = Counter()
    nums = Counter()
    methods = []  # list of (meth_name, sig_code, line_count)
    fields = 0

    # Field count (per-line, cheap).
    for line in lines:
        if FIELD_RE.match(line):
            fields += 1

    # Methods: whole-text scan (signatures may span newlines).
    decl_lines = []  # list of (line_idx, meth_name, sig)
    for mm in METH_RE.finditer(text):
        ret, meth_name, args = mm.group(1), mm.group(2), mm.group(3)
        # last token of ret is the actual return type (drop any leading qualifiers)
        ret_tok = ret.strip().split()[-1] if ret.strip() else "?"
        li = text.count("\n", 0, mm.start())
        decl_lines.append((li, meth_name, sig_code(ret_tok, args)))
    decl_lines.sort(key=lambda x: x[0])

    # method body spans [decl_i, next_decl_i)
    for idx, (li, meth_name, sig) in enumerate(decl_lines):
        end = decl_lines[idx + 1][0] if idx + 1 < len(decl_lines) else len(lines)
        methods.append((meth_name, sig, end - li))

    # Literals / APIs / nums on full text — we already have per-class granularity.
    for slug, _va in STR_RE.findall(text):
        # trim trailing underscores that are just separators before VA
        slug = slug.strip("_")
        if slug:
            strings[slug] += 1

    for tok in API_RE.findall(text):
        if any(tok.startswith(p) for p in SYNTHETIC_PREFIXES):
            continue
        # drop common type names / keywords that are all-caps but not calls
        if tok in ("HANDLE","HGDIOBJ","HWND","HDC","LPCSTR","LPSTR","LPVOID",
                   "BOOL","DWORD","WORD","BYTE","FLOAT","LPBYTE","LPDWORD",
                   "WARNING","XREF","NULL","TRUE","FALSE","RETURN"):
            continue
        apis[tok] += 1

    for n in NUM_HEX_RE.findall(text):
        if keep_num(n):
            nums[n.lower()] += 1
    for n in NUM_DEC_RE.findall(text):
        if keep_num(n):
            nums[n] += 1

    return cls, {
        "fields": fields,
        "strings": strings,
        "apis": apis,
        "nums": nums,
        "methods": methods,
        "lines": len(lines),
    }


def main():
    files = sorted(CLASSES_DIR.glob("cls_0x*.cpp"))
    if not files:
        print(f"no class files under {CLASSES_DIR}", file=sys.stderr)
        sys.exit(1)

    # Merge by cls_id (one ADDR can have multiple files: body + vftable).
    per_cls: dict[str, dict] = {}
    for f in files:
        cls, stats = process_file(f)
        if not cls: continue
        agg = per_cls.setdefault(cls, {
            "fields": 0, "strings": Counter(), "apis": Counter(),
            "nums": Counter(), "methods": [], "lines": 0,
        })
        agg["fields"]  = max(agg["fields"], stats["fields"])
        agg["lines"]  += stats["lines"]
        agg["strings"].update(stats["strings"])
        agg["apis"].update(stats["apis"])
        agg["nums"].update(stats["nums"])
        agg["methods"].extend(stats["methods"])

    OUT.parent.mkdir(parents=True, exist_ok=True)
    with OUT.open("w") as out:
        out.write("# cls\tkind\tcount\tvalue\n")
        for cls in sorted(per_cls):
            s = per_cls[cls]
            out.write(f"{cls}\tsum\t1\tfields={s['fields']} methods={len(s['methods'])} lines={s['lines']}\n")
            for v, c in sorted(s["strings"].items(), key=lambda x: (-x[1], x[0])):
                out.write(f"{cls}\tstr\t{c}\t{v}\n")
            for v, c in sorted(s["apis"].items(),    key=lambda x: (-x[1], x[0])):
                out.write(f"{cls}\tapi\t{c}\t{v}\n")
            for v, c in sorted(s["nums"].items(),    key=lambda x: (-x[1], x[0])):
                out.write(f"{cls}\tnum\t{c}\t{v}\n")
            for meth, sig, nl in s["methods"]:
                out.write(f"{cls}\tmeth\t1\t{meth}:{sig}:{nl}\n")

    n_rows = sum(1 for _ in OUT.open())
    print(f"wrote {OUT} ({n_rows} rows, {len(per_cls)} classes)")


if __name__ == "__main__":
    main()
