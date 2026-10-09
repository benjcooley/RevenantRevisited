# Forensics: combat data provenance and parsing (katas D0, D1)

**D0 verdict.** The shipped Revenant.exe reads **the archive copy first**.
Every combat-data loader (rules.def, stats.def, char.def, weapon.def,
armor.def, equip.def, class.def, spell.def, and also master.s and
statpane.def) opens its file through the pack-aware open `0x4a13f0` →
`0x4a1240` with a third argument of **0**. That means: look in every mounted
pack whose folder matches the path, and only if no pack has the entry,
`fopen` the loose file. A loose file is read only when no pack has that
name. The only loader that passes 1 (loose first) is the .def screen loader
`0x4377c0` (widgets.def plus one screen file). So in the DOSBox lab
(`~/RevenantRetailLab/retail-cd/REVENANT/`, copied into the Win98 guest by
INSTALL.BAT) the 86,689-byte 1998 `Resources/rules.def` is **never
opened**: retail reads resources.rvr's 8,634-byte rules.def (zip date
1999-09-24), imagery.rvi's 327,573-byte char.def (1999-10-08) and
233,484-byte class.def, and resources.rvr's 42,033-byte spell.def. **The
2026-10-06 Rahul fight captures (`docs/gameflow/reference/opening/10_rahul_fight.png`,
`11_rahul_killed.png`) ran on the 1999 archive data, not on 1998 rules.** A
runtime check backs up the static reading. The loose 1998 rules.def holds
18 CHARACTER blocks, 13 of them (Default, Arakna, Dorogar, Hopper,
Issathi, …) also in char.def, and retail treats a repeated CHARACTER name
as fatal ("More than one %s in CHAR.DEF file", `0x48bfe9`). If retail had
read it, the lab could not have reached the opening. The port on
`feature/combat` now resolves the same way: gameflow's `36faa5f` made
`rev_fopen` pack-first. The arena logs show `per level H/F/M 25/3/25`,
which is the archive's FATIGUEDATA 3; the lab's loose 1998 file has 25. The
main checkout (`main`, d876a3f) still opens loose files first and reads
the 1998 file.

**Status:** 2026-10-08. Static analysis of the unchanged retail image
(sha1 `4547b6d3…` = the lab's and GOG's Revenant.exe). Every claim below
was read in the disassembly; "unverified" marks what wasn't. No RNG in
scope (§3).
**Evidence:** asm via the dojo `rdis` helper; Ghidra decompile
`recon/classes/cls_0x48b690.cpp` (Initialize/Load: it agrees with the asm).
No decompiles exist for `0x489850`, `0x4891c0` or `0x49c9f0`; those were
read from the asm alone.

## 1. Function table

| Address | Identity | Convention | Confidence / evidence |
|---|---|---|---|
| `0x483670` | SetupPaths(cmdline, exeDir `0x6666cc`, workDir `0x65d254`) | cdecl, `ret` | asm: GetModuleFileNameA, test.fil write probe, "c:\Revenant" fallback |
| `0x484500` | GetINISettings, [Paths] | cdecl | asm: strings ClassDefPath/ResourcePath/ImageryPath, defaults |
| `0x4865a0` | WinMain (pack mounts at `0x486875`, `0x486a62`) | — | asm; recon FREE_FUNCTION_IDENTIFICATIONS |
| `0x49ee20` | TPackFile::Open(path, mode) | thiscall, ret 8 | asm: fopen, strrchr '\\', registry add `0x6687f8` |
| `0x4a0380` | TPackFile::Find(path, hint) → entry index / −1 | thiscall, ret 8 | asm: _strnicmp dir, name stem, bsearch cmp `0x49ff00` |
| `0x49ff00` | entry compare: _stricmp(dir), then _stricmp(name) | cdecl | asm |
| `0x4a1240` | zfopen(fullpath, mode, looseFirst) → ZFILE* | cdecl | asm: 32 ZFILE slots `0x668b10` ("Out of ZFILES!!"), pack loop, fopen `0x58b5db` |
| `0x4a13f0` | zfopen_rel(path, mode, looseFirst): workDir then exeDir | cdecl | asm |
| `0x4a1c00` → `0x4a1c30` | FileExists(path, sub) (isdir=0): pack entry, else `_findfirst` | cdecl | asm |
| `0x483120` | MakeFullPath(path, buf, len): workDir + relative | cdecl | asm, same `.\` rule as `0x4a13f0` |
| `0x4789c0` | TToken::Open(path): `_strupr`, zfopen_rel(path,"rb",0), fatal "Unable to find file %s" | thiscall, ret 4 | asm |
| `0x4788d0` | TToken::SetFile(zfile, name): reads whole file, XOR-0xCC decode, string stream vtable `0x5a36f8` | thiscall, ret 8 | asm |
| `0x4834e0` / `0x483540` | IsScrambled (byte0 & 0x80) / Unscramble (XOR 0xCC every byte) | cdecl | asm |
| `0x47a410` | Parse(t, fmt, …) → ParseAnything `0x479ad0`(1, t, fmt, va) | cdecl | asm |
| `0x478a10` `0x479580` `0x4795a0` `0x479680` | Get, WhiteGet, LineGet, SkipBlanks | thiscall | asm (token type at `+0x10`: 1 white, 3 keyword, 4 ident, 6 text, 8 number, 9 return, 10 EOF) |
| `0x479700` | Is(str, minlen): `_stricmp` when minlen 0 | thiscall, ret 8 | asm |
| `0x479790` | BlockBegin: BEGIN, RETURN, skip blanks; fatal "BEGIN Expected!" | thiscall | asm |
| `0x479450` | DefineGet (`#define NAME %d`) | thiscall | asm ("%32t %d") |
| `0x479950` | TToken::Error(fmt, arg) → FatalError | thiscall, ret 8 | asm → `0x481c10` |
| `0x481c10` | FatalError(fmt, arg): message, exit | cdecl | asm ("Press any key to exit", `0x58d065`) |
| `0x48b690` | TRules::Initialize | thiscall, ret 0 | asm; called from `0x4861cb` ("Loading game rules"), ecx `0x65d7a8` |
| `0x48b990` | TRules::Load | thiscall, ret 0 | asm + decompile |
| `0x4891c0` | SClassData::Load(name, t) | thiscall, ret 8 | asm, strings |
| `0x489470` | SCharData ctor (array ctor + `0x4895d0`) | thiscall | asm |
| `0x4895d0` | SCharData::SetDefaults | thiscall, ret 0 | asm |
| `0x489850` | SCharData::Load(name, t) | thiscall, ret 8 | asm, strings |
| `0x48d230` | TCharAttackArray::Add(rec): malloc 0x320 copy | thiscall, ret 4 | asm |
| `0x48ac30` / `0x48b1a0` | weapon / armor block Load(name, t) | thiscall, ret 8 | asm, strings |
| `0x49c9b0` / `0x49c9f0` / `0x49cbf0` | TStatLevels Init / Parse(name, t) / Get(stat, value) | thiscall | asm |
| `0x48c730` | TRules::GetClass(name) | thiscall, ret 4 | asm |
| `0x48c780` | TRules::GetCharData(objtype, objclass) | thiscall, ret 8 | asm |
| `0x48cab0` → `0x48c930`, `0x48c7f0` | BindTypes → BindType(name, typeidx), BindTypeData(name, cls) | thiscall | asm; called at `0x47654f` (end of LoadClasses), `0x47c9a0`, `0x47cb97` |
| `0x48cc20` `0x48cc40` `0x48cc60` `0x48cc90` `0x48ccb0` | StatLevel, ExpForLevel, LevelForExp, SkillExpForLevel, SkillLevelForExp | thiscall | asm |
| `0x47eb50` | ConvertMinutesToFrames(m) = m·daylength/1440 | cdecl | asm (magic `0xb60b60b7`, `[0x65d804]` = Rules+0x5c) |
| `0x476140` | TObjectClass::LoadClasses(module) (class.def) | — | asm, strings "%sclass.def" |
| `0x53ead0` | TSpellList::Load (spell.def) | thiscall | asm |
| `0x4377c0` | DefWidget_LoadFile (passes looseFirst = 1) | — | asm `0x437816`, `0x437866` |

## 2. File resolution (D0)

### 2.1 Paths

- `0x483670`: exeDir = the directory of GetModuleFileNameA, keeping the
  trailing `\`. If a `test.fil` can be written there, workDir = exeDir.
  Otherwise (a CD run) workDir = `c:\Revenant\`, then `d:`, … up to 5
  drives, mkdir'd. `SetCurrentDirectory(exeDir)` (`0x58d715`).
- `0x484500` [Paths], with defaults and a `\` appended:
  ClassDefPath `0x65bd48` = `.\Resources\`, ResourcePath `0x65dde8` =
  `.\Resources\`, ImageryPath `0x65bc44` = `.\Imagery\`, ModulesPath
  `0x65d6a4` = `.\Modules\`. The lab's revenant.ini uses exactly these.
- After mounting, if `[0x65c558]==0` and `0x487300(ImageryPath,0,1)` fails
  (a directory check, not traced), ImageryPath becomes ResourcePath
  (`0x486b6c`).

### 2.2 Pack mount (WinMain)

- `0x4867a0`–`0x486875`: ResourcePath with `.\` stripped is joined to
  workDir, the trailing `\` is dropped and `.rvr` appended:
  `<workDir>Resources.rvr`. Then `TPackFile(0x666448)::Open(path, 1)`.
  Same at `0x486983`–`0x486a62` for `<workDir>Imagery.rvi` → object
  `0x65c3c8`. Module packs (`Modules\<name>.rvm`) use object `0x65b2f8`
  (`0x460b1b`).
- `0x49ee20` Open: fopen(path, "rb") → `+0x190`. It reads the directory
  (`0x49fb80`); the pack name is the text after the last `\` → `+0x110`
  ("Resources.rvr") and the folder up to the last `\` → `+0x0c`
  (workDir). It registers in the pack list (count `0x6687f8`, array
  `0x668808`) only on success. A missing pack is not fatal: WinMain
  checks `[0x65b2fc]`, the **module** pack's error field (`0x65b2f8+4`),
  not the resource pack's (`0x66644c`). It reports only "Compressed files
  found" (pack flags & 8). With no pack registered, every lookup falls
  through to loose files.

### 2.3 The open (`0x4a13f0` → `0x4a1240`)

```
zfopen_rel(path, mode, looseFirst):                       // 0x4a13f0
  if path[0]=='\\' || path[1]==':' || path starts ".." -> return zfopen(path, mode, 0)  // NB: flag dropped
  strip ".\" (a leading '.', then any '\')
  z = zfopen(workDir + rel, mode, looseFirst)
  if !z && _stricmp(exeDir, workDir) != 0: z = zfopen(exeDir + rel, mode, looseFirst)
  return z

zfopen(full, mode, looseFirst):                           // 0x4a1240, mutex 0x65b8ac
  slot = first free of 32 ZFILEs {FILE*, pack*, entry*} at 0x668b10 (else fatal "Out of ZFILES!!")
  if looseFirst: slot.fp = fopen(full, mode); if ok return slot
  for each registered pack p with p.fp && (p.flags & 1):
      i = p.Find(full, i)                                   // 0x4a0380
      if i >= 0: break
  if found: 'r' in mode -> slot.entry = p.OpenEntry(i) (0x4a06a0)
            'w' in mode -> slot.entry = p.CreateEntry(full) (0x4a0890); return slot or 0
  slot.fp = fopen(full, mode); return slot or 0
```

`Find` (`0x4a0380`): `_strnicmp(full, p.dir, len(p.dir))`. The next path
component must equal the pack name up to its `.` (toupper per char). It is
followed by `\`. The rest is split at its last `\` into (subdir, name) and
binary-searched with `_stricmp`. So `C:\REVENANT\RESOURCES\RULES.DEF` is
answered by `Resources.rvr` entry `rules.def`, case-blind. Wildcards go to
`0x4a0180`. FileExists (`0x4a1c30`) uses the same pack test (isdir 0),
then `_findfirst` and a not-a-directory check. It answers for a pack entry
or a loose file.

### 2.4 Who passes what

| Loader | Path | looseFirst | Evidence |
|---|---|---|---|
| Rules::Load, all six files | via `0x4789c0` | 0 | `0x4789cf push 0` |
| spell.def | ClassDefPath via `0x4789c0` | 0 | `0x53eb7a` |
| class.def | ImageryPath if FileExists, else ClassDefPath; module: `Modules\<m>\class.def` | 0 | `0x476207 push ebx(0)` |
| master.s and scripts | module dir, then ClassDefPath | 0 | `0x49650a push 0`, `0x496524` |
| statpane.def | ResourcePath | 0 | `0x546c73 push ebx`, ebx zeroed at `0x546b86` |
| .def screens: widgets.def plus the named screen | ResourcePath | **1** | `0x437816`, `0x437866` |

Rules::Load file order (`0x48bac8`–`0x48c694`, loop `esi` 0..5). Every
file goes through the **same** tag dispatcher, so any file can hold any
block:

| i | File | Lookup | Missing |
|---|---|---|---|
| 0 | rules.def | ClassDefPath, no exists check | fatal "Unable to find file %s" |
| 1 | stats.def | ClassDefPath if FileExists | skipped |
| 2 | char.def | ImageryPath if FileExists, else ClassDefPath if FileExists | skipped |
| 3 | weapon.def | same as char.def | skipped |
| 4 | armor.def | same | skipped |
| 5 | equip.def | same | skipped |

resources.rvr's stats.def is comments only ("File no longer used.. please
delete me!"); no install ships equip.def.

Before parsing, `0x4788d0` reads the whole file. If its first byte has
bit 7 set, every byte is XORed with 0xCC (`0x483540`). The shipped files
are plain text.

### 2.5 Consequence for the lab install

The lab's `retail-cd/REVENANT` is staged by `prepare_retail.py` from the
main checkout's `data/`. Note that the gameflow commit `9bce7c8` restored
the stock 8,642-byte rules.def on feature/combat only. INSTALL.BAT then
`xcopy /D`s it into the guest's `C:\REVENANT`. Each loose file against its
pack copy:

| Loose file | Size | Pack copy | Read by retail? |
|---|---|---|---|
| Resources/rules.def | 86,689 (1998 merge; = main checkout `data/`) | rvr 8,634 | **no**, the pack wins |
| Resources/spell.def | 42,034 (GOG stock) | rvr 42,033 | no |
| Resources/master.s | 15,953 (GOG) | rvr 16,245 | no |
| Resources/statpane.def | 8,763 (GOG) | rvr 8,679 | no |
| Resources/options.def | 5,785 (GOG) | rvr 5,671 | **yes**, if opened by `0x4377c0` (loose first; screen names not traced) |
| Resources/joingame.def | 3,444 (GOG) | rvr 3,451 | same as options.def |
| Resources/mpingame.def | 5,347 (GOG) | rvr 4,546 | same as options.def |
| Imagery/char.def | 327,568 (GOG) | rvi 327,573 | no |
| Modules/Deathmatch6/area.def | loose dir | Deathmatch6.rvm | no, while the module pack is mounted (pack first) |
| Modules/Demo/* | loose only, no Demo.rvm | — | yes (the only copy) |
| Resources/effects.def, render_metadata.def | port files | none | never: neither name is in the exe |

So if loose files were read first, eight files would shadow pack copies:
rules.def, spell.def, master.s, statpane.def, options.def, joingame.def,
mpingame.def and char.def, plus Deathmatch6/area.def. Of these, only the
.def screens are really read loose. The GOG install (`worktrees/combat/data`)
ships the same loose set, except rules.def is GOG's own 8,642-byte file
(md5 `99dd4124…`). It also differs from the pack: CLASS Veteran
HEALTHMOD 40 / MANAMOD −40 (pack 10 / −30); Shaman FATIGUEMOD −20 /
HEALTHMOD −20 / MANAMOD 50 (pack 0 / −5 / 20); Assassin 40 / −10 / −20
(pack 10 / 0 / 0). GOG's spell.def differs from the pack in one LIGHT COLOR
line (`250,250,0` against the pack's `255,50,0`). For the shipped exe, all
of these loose combat files are dead data.

**Imagery/char.def, loose against imagery.rvi.** The size differs by 5
bytes. That comes from two inserted blank CRLF lines (after Bayne's and
Morganna's BOWWAIT) plus one `-` sign. Underneath are 14 changed values.
The pack (retail-read) values are on the right:

| Character | Tag (field) | Loose | Pack |
|---|---|---|---|
| Issathi | DAMAGEMODS poison (index 9) | 100 | **−100** |
| Kantha | ATTACK ×3, damagemod / fatigue | 0/40, 0/30, 0/35 | 4/20, 5/30, 4/35 |
| War Kantha | ATTACKFREQ | 30, 90 | 30, 60 |
| War Kantha | ATTACK ×3, damagemod / fatigue | 17/40, 15/30, 19/35 | 18/20, 20/30, 22/25 |
| War Kantha | ATTACK attackpcnt (WM_HAND line) | 65 | 75 |
| Vashar | WEAPONDAMAGE | 22 | 23 |
| Vashar | ATTACK ×5, damagemod / fatigue | 8/70, 4/30, 6/40, 8/55, 10/80 | 8/45, 6/25, 7/35, 8/40, 10/50 |

Under retail CalculateDamage (`(100 − Resist)`, see the dojo doc §5.5),
the pack's −100 doubles poison damage on Issathi, and the loose +100 would
cancel it. The pack entry is dated 1999-10-08 19:45, the newest .def in
either pack. Which file is older can't be told from the data.
