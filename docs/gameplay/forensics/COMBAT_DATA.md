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

## 3. TRules (D1): object at `0x65d7a8`, 0x254 bytes, BSS (zeroed)

### 3.1 Initialize (`0x48b690`)

`if initialized (+0x00) return 1`. It frees the chardata and class
arrays, sets def `+0x54` = 0 and calls `0x49c9b0` (6 STATLEVEL tables,
each entry `-2000000` = `0xffe17b80`). It then sets these defaults
**before** Load:

| Field | Default | Field | Default |
|---|---|---|---|
| `+0x80` healthrecovrate | 1 | `+0x94` TOHITCENTER | 50 |
| `+0x84` fatiguerecovrate | 1 | `+0x98` TOHITRANGECHAR | 10 |
| `+0x88` manarecovrate | 1 | `+0x9c` TOHITRANGEPLYR | 10 |
| `+0x90` poisondamagerate | 1 | `+0xa0` TOHITBLOCK | 25 |
| `+0xdc..+0xec` AMMODATA | 20, 6, 4, 1, 25 | `+0xa4` TOHITFACE | 25 |

Then `Load()`. On success it sets `+0x00 = 1` and builds three tables of
30 entries each, from formulas rather than the file:

- `+0xf0` player level exp: `[0]=0, [1]=300, [i]=[i−1]+(5i+10)·20`.
- `+0x168` skill exp: `[0]=300, [i]=[i−1]+(5i+15)·20` (= +100i+300).
- `+0x1e0`: `[0]=500, [i]=[i−1]+2000i+500`. Nothing in this doc reads it
  (identity unknown).

Getters: ExpForLevel `0x48cc40` (0→0, else `[min(l−1,29)]`),
SkillExpForLevel `0x48cc90` (same over `+0x168`). LevelForExp `0x48cc60`
scans indexes 0..30 (index 30 reads `+0x168[0]`, harmless) and returns the
first i with exp < table[i], else 30.

### 3.2 Load (`0x48b990`): dispatcher

Per file (§2.4): TToken on the stack, Open, then `DefineGet` (fatal
"Syntax error in header" on failure). While the token is not EOF: it must
be IDENT (type 4), else fatal "Rules block name or tag expected". The tag
is copied (39 chars) and `WhiteGet`. The tag is compared with `_stricmp`
in this order. Then: not ok → fatal "Error parsing tag %s"; token must be
RETURN or EOF, else fatal "Return expected"; `DefineGet`, else fatal
"Rules file syntax error". **An unknown tag is fatal: "Invalid block or tag %s".**

| Tag (string addr) | Format | Lands at | Notes |
|---|---|---|---|
| DAYLENGTH `0x5d99d0` | `%i` | `+0x5c` | |
| TWILIGHT `0x5d99e0` | `%i, %i` | `+0x60`, `+0x64` | if GameSpeed `[0x5d79e4]`==5: `+0x64 = ConvertMinutesToFrames(+0x60)` = `+0x60·daylength/1440` (`0x47eb50`, truncates toward 0) |
| STEALTH `0x5d99f4` | `%i, %i, %i` | `+0xd0`, `+0xd4`, `+0xd8` | max, sneak, min |
| CHARACTER `0x5d9a08` | `%s\n` (64-byte buffer) | chardata array `+0x18` (count), `+0x28` (data) | malloc 0x590, ctor; **a repeated name is fatal**, "More than one %s in CHAR.DEF file", searched across all files; `0x489850`; objtype `+0xc0`==−1 → def `+0x54`; then Get |
| CLASS `0x5d9a54` | `%s\n` | class array `+0x04` / `+0x14` | malloc 0x74; `0x4891c0`; no duplicate check; then Get |
| WEAPON `0x5d9a7c` | `%s\n` | weapon array `+0x2c` / `+0x3c` | record 0xd0; duplicate fatal; `0x48ac30`; then **Get** |
| ARMOR `0x5d9ac8` | `%s\n` | armor array `+0x40` / `+0x50` | record 0xcc; duplicate fatal; `0x48b1a0`; then **WhiteGet** |
| HEALTHDATA `0x5d9b14` | `%i, %i, %i` | `+0x68`, `+0x74`, `+0x80` | perlevel, recovval, recovrate |
| FATIGUEDATA `0x5d9b2c` | `%i, %i, %i` | `+0x6c`, `+0x78`, `+0x84` | |
| MANADATA `0x5d9b44` | `%i, %i, %i` | `+0x70`, `+0x7c`, `+0x88` | |
| POISONDATA `0x5d9b5c` | `%i, %i` | `+0x8c`, `+0x90` | damage val, rate |
| AMMODATA `0x5d9b70` | `%i, %i, %i, %i` with **5** pointers | `+0xdc..+0xe8` (`+0xec` never parsed) | commented out in the shipped rules.def |
| TOHITCENTER `0x5d9b8c` | `%i` | `+0x94` | |
| TOHITRANGECHAR `0x5d9b9c` | `%i` | `+0x98` | |
| TOHITRANGEPLYR `0x5d9bb0` | `%i` | `+0x9c` | |
| TOHITBLOCK `0x5d9bc4` | `%i` | `+0xa0` | |
| TOHITFACE `0x5d9bd4` | `%i` | `+0xa4` | |
| TOHITDAMAGE `0x5d9be4` | WhiteGet, BlockBegin, then 5× { Is("ENTRY") else ok=0/break; Get; WhiteGet; `%i %i`; WhiteGet }, Get, WhiteGet | `+0xa8+8i` (MinValue), `+0xac+8i` (DamagePercent), i<5 | **exactly 5 ENTRY lines**: fewer is fatal (ok=0); more leaves a token behind |
| STATLEVEL `0x5d9c00` | `%s\n` | `+0x58` tables via `0x49c9f0` | fatal "Error loading statlevel data"; then WhiteGet |

Values in the shipped rules.def (resources.rvr): DAYLENGTH 80000; TWILIGHT
60, 8; STEALTH 100, 50, 10; HEALTHDATA 25, 1, 6000; FATIGUEDATA 3, 10, 75;
MANADATA 25, 1, 6000; POISONDATA 6, 100; TOHITCENTER 50; TOHITRANGECHAR 12;
TOHITRANGEPLYR 10; TOHITBLOCK 25; TOHITFACE 25; TOHITDAMAGE {40,50}, {0,0},
{−15,−50}, {−40,−75}, {−50,−90}. Six STATLEVEL tables; CLASS Revenant,
Veteran, Shaman, Assassin. The file's comments document the to-hit formula
(lines 38–62).

Readers of these globals, for C4 (xref only, not analysed): TOHITCENTER
`0x4d17f0`; TOHITRANGECHAR `0x4d72c3`, `0x4d7303`; TOHITRANGEPLYR
`0x51a52e`, `0x51a55e`; TOHITFACE `0x4d1841`; TOHITDAMAGE table
`0x4c64dc`–`0x4c69a7` (ResolveHit region) and `0x4d18d7`–`0x4d1909` (loop
end `0x65d878`); AMMODATA `0x468e50`, `0x4c056f`–`0x4c0612`. TOHITBLOCK
`0x65d848` has no absolute reference.

### 3.3 Lookups and binding

- GetClass (`0x48c730`): linear `_stricmp` over `+0x14`; null when not found.
- GetCharData(objtype, objclass) (`0x48c780`): 0 if not initialized; the
  first entry with `+0xc0==objtype && +0xc4==objclass`; else def `+0x54`
  (null if there is no "Default").
- **Late binding.** Retail loads rules **before** class.def (`0x4861cb`,
  then LoadClasses `0x486202`). SCharData::Load gives every non-"Default"
  name `objtype = objclass = −2`. BindTypes (`0x48cab0`, called at the end
  of LoadClasses `0x47654f`) walks every type of every TObjectClass. It
  finds which class owns the name (`0x475210`) and switches on class id
  (table `0x48ca98`): ids 11/12 (player/character) → the chardata named
  so gets `+0xc0 = type index`, `+0xc4 = class id`. Id 1 is weapon
  (`+0xc4/+0xc8`) and id 2 is armor (`+0xc0/+0xc4`). A CHARACTER whose name
  is no type stays −2 and is never returned. There is no error.

## 4. SClassData (`0x4891c0`, 0x74 bytes, zeroed by malloc `0x482fb0`)

Name `+0x00` (strncpy 31). `+0x70 = 0xffff` is set before parsing. The
block is: SkipBlanks, Is("BEGIN") (else fatal "Char block BEGIN
expected"), LineGet. The loop runs until EOF or Is("END"). The token must
be IDENT ("Class data keyword expected"). Then WhiteGet and:

| Tag | Parse | Lands at |
|---|---|---|
| STATREQS | 6 raw NUMBER tokens (type 8), `,` between them; fatal "Stat requirement value expected" / "',' Expected" | `+0x20..+0x34` |
| SKILLMODS | 11 raw NUMBER tokens, same way ("Skill modifier expected") | `+0x38..+0x60` |
| HEALTHMOD | `%i` | `+0x64` |
| FATIGUEMOD | `%i` | `+0x68` |
| MANAMOD | `%i` | `+0x6c` |
| WEAPONS | `%i` (expression, e.g. `WM_BLUDGEON \| WM_BOW \| WM_CROSSBOW`) | `+0x70` (default 0xffff) |
| anything else | fatal "Invalid class tag %s" | |

Then RETURN is required and LineGet follows. At the end: Is("END") (else
"Class block END expected"), Get.

## 5. SCharData (0x590 bytes)

### 5.1 Layout, tags and defaults

Defaults come from malloc's zero fill plus `0x4895d0`. The tag column
gives the char.def keyword. Counts are uses in the shipped char.def (60
CHARACTER blocks).

| Offset | Field | Tag / format | Default | Uses |
|---|---|---|---|---|
| `+0x000` | name[32] | (strncpy 31 of the block name) | — | |
| `+0x020` | groups[80] | GROUPS `%80s` | "" | 58 |
| `+0x070` | enemies[80] | ENEMIES `%80s` | "" | 58 |
| `+0x0c0` | objtype | ("Default" → −1, else −2, bound later §3.3) | | |
| `+0x0c4` | objclass | (same) | | |
| `+0x0c8` | flags | FLAGS `%i`: **passes the value, not the address** (`0x489ecb`), so it writes through a pointer equal to the current flags (0). A failing BLEEDER/BLEADER parse ORs 0x10 here before the fatal error | 0 | 0 |
| `+0x0cc` | attacks (pointer array: count `+0xcc`, data `+0xdc`, null element `+0xe0` = 800 zero bytes) | ATTACK, FATIGUEATTACK, MAGICATTACK, PLAYANIM | | 889 / 45 / 83 / 0 |
| `+0x0e4` | damagemods[10] | DAMAGEMODS: 10 raw NUMBER tokens, `,` between | 0 (only [0] is written by the ctor; the rest come zeroed from malloc) | 58 |
| `+0x10c` | blocksounds[32] | BLOCKSOUNDS `%31s` | **"block1,block2,block2"** | 51 |
| `+0x12c` | misssounds[32] | MISSSOUNDS `%31s` | "" (BSS `0x6682fc`) | 0 |
| `+0x14c` / `+0x150` / `+0x154` | playerblock min / step / inc | PLAYERBLOCK `%i, %i, %i` | 10 / 5 / 10 | 0 |
| `+0x158` / `+0x15c` | combatrange min / max | COMBATRANGE `%i` [`, %i`]; no second value → max = min+64 (also when the parse fails) | 128 / 192 | 58 |
| `+0x160` | maxattackrange | MAXATTACKRANGE `%i` (on failure set to 32, then fatal) | 32 | 58 |
| `+0x164` | bleeder | BLEEDER or BLEADER `%i` | **1** | 8 (all `0`) |
| `+0x168` | swipecolor | SWIPECOLOR `%b, %b, %b`: r → `+0x16a`, g → `+0x169`, b → `+0x168` (a 0x00RRGGBB dword) | 0 | 26 |
| `+0x16c` | swipefull (byte) | SWIPEFULL `%b` | 0 | 0 |
| `+0x170` | bodytype[32] | BODYTYPE `%30s` | "normal" | 3 |
| `+0x190` | classdata* | CLASS `%30s` → GetClass; **not found is fatal** "Unable to find character class \"%s\"" (`0x481c10`) | null | 4 |
| `+0x194` / `+0x198` / `+0x19c` | block freq / min / max | BLOCK `%i, %i, %i` | 10 / 5 / 15 | 51 |
| `+0x1a0..+0x1ac` | sight min, max, range, angle | SIGHT `%i, %i, %i, %i` | 30, 100, 320, 64 | 59 |
| `+0x1b0..+0x1b8` | hearing min, max, range | HEARING `%i, %i, %i` | 10, 50, 320 | 59 |
| `+0x1bc` | weapontype | WEAPONTYPE `%i` | 0 (WT_HAND) | 54 |
| `+0x1c0` | weapondamage | WEAPONDAMAGE `%i` | 2 | 58 |
| `+0x1c4` | armorvalue | ARMOR `%i` | **1** (no character sets it, so every monster's Armor() is 1) | 0 |
| `+0x1c8` | defensemod | DEFENSEMOD `%i` | 0 | 0 |
| `+0x1cc` | attackmod | ATTACKMOD `%i` | 0 | 0 |
| `+0x1d0` / `+0x1d4` | attackfreq min / max | ATTACKFREQ `%i, %i` | 100 / 250 | 58 |
| `+0x1d8` / `+0x1dc` | magicfreq min / max | MAGICFREQ `%i, %i` | 100 / 250 | 6 |
| `+0x1e0` | mana | MANA `%i` | 0 | 18 |
| `+0x1e4` | fatigue | FATIGUE `%i` | 25 | 54 |
| `+0x1e8` | health | HEALTH `%i` | 25 | 54 |
| `+0x1ec` / `+0x1f0` / `+0x1f4` / `+0x1f8` | walk / run / sneak / combatwalk speed | WALKSPEED / RUNSPEED / SNEAKSPEED / COMBATWALKSPEED `%i` | −1 each | 0 |
| `+0x1fc..+0x204` | arrowpos x, y, z | ARROWPOS `%i, %i, %i` | −1; if still −1 after the block: bayne (−5,−15,80), morganna (−10,−15,50), navarro (−10,−15,30), everyone else (−5,−15,60) (`0x48aa75`; names compared case-blind) | 0 |
| `+0x208` / `+0x20c` / `+0x210` | arrowspeed / bowwait / bowaimspeed | ARROWSPEED / BOWWAIT / BOWAIMSPEED `%i` | 20 / 12 / 8 | 0 / 4 / 0 |
| `+0x214` | numimpacts (CHARIMPACT) | | 0 | |
| `+0x218` | impacts[6] × 0x5c | CHARIMPACT (§6.3) | | 11 |
| `+0x440` | retreatat | RETREATAT `%i` | 0 | 4 |
| `+0x444` | retreatatmana | RETREATATMANA `%i` | 0 | 0 |
| `+0x448` | retreatfor | RETREATFOR `%i` | −1 | 4 |
| `+0x44c` / `+0x450` | runfatigue a / b | RUNFATIGUE `%i, %i` | 1 / 100 | 1 (Locke 10, 100) |
| `+0x454` | noparalyze | NOPARALYZE (no argument) → 1 | 0 | 5 |
| `+0x458 + 0x4c·i`, i<4 | attacheffect[4] {`+0x00`,`+0x04`,`+0x08` int; `+0x0c` str[20]; `+0x20` str[20]; `+0x34` str[20]; `+0x48` used} | ATTACHEFFECT `%20s, %i, %i, %i, %20s, %20s`: field 1 → `+0x34`, fields 2–4 → `+0x00..+0x08`, field 5 → `+0x0c`, field 6 → `+0x20`; "none" → ""; first free slot; all 4 used → nothing parsed | 0 | 0 |
| `+0x588` | poisonchance | POISONCHANCE `%i` | 0 | 0 |
| `+0x58c` | labelheight | LABELHEIGHT `%i` | −1; if still −1: bayne 120, navarro 80, everyone else 100 | 0 |

Block flow (`0x489850`): strncpy name; "Default" check; SkipBlanks;
Is("BEGIN") (fatal "Char block BEGIN expected"); LineGet. Loop until EOF
or Is("END"): IDENT required ("Char data keyword expected"); tag (39
chars); WhiteGet; dispatch (`_stricmp`, in the order of the string list at
`0x5d8fbc`–`0x5d95c0`). **An unknown tag is fatal: "Invalid character tag %s".**
Then: not ok → fatal "Error parsing tag %s"; RETURN required ("Return
expected"); LineGet. After the loop: the arrowpos/labelheight name
defaults; Is("END") (fatal "Char block END expected"); Get. Note that
LineGet is used inside the block, so a `#define` is only honoured between
blocks (DefineGet in §3.2).

## 6. Attacks and impacts

### 6.1 SCharAttackData (0x320 bytes; Add `0x48d230` mallocs and copies it)

| Offset | Field | ATTACK field # | FATIGUEATTACK # | PLAYANIM # |
|---|---|---|---|---|
| `+0x00` | attackname[32] | 1 `%30s` | 1 | 1 |
| `+0x20` | (index: the new slot index is written into the **local** copy *after* Add has copied it, so the stored record keeps 0) | — | — | — |
| `+0x24` | flags | 2 | 2 (then `\|= 0x10000`) | 2 (then `\|= 0x1000000`) |
| `+0x28` | button | 3 | 3 | 3 |
| `+0x2c` | attackpcnt | 22 | 22 | 6 |
| `+0x30` / `+0x34` | mindist / maxdist | 12 / 13 | 12 / 13 | 4 / 5 |
| `+0x38` | responsename[32] | 4 `%30s` | 4 | |
| `+0x58` | blockname[32] | 5 | 5 | |
| `+0x78` | missname[32] | 6 | 6 | |
| `+0x98` | 7th string (port: chainname; retail char.def comment: "death") | 7 | 7 | |
| `+0xb8` | blocktime | 8 | 8 | |
| `+0xbc` | impacttime | 9 | 9 | |
| `+0xc0` | chainexptime | **11** | 11 | |
| `+0xc4` | nextwait | **10** | 10 | |
| `+0xc8` / `+0xcc` / `+0xd0` | hitminrange / hitmaxrange / hitangle | 14 / 15 / 16 | same | |
| `+0xd4` / `+0xd8` / `+0xdc` | damagemod / fatigue / attackskill | 17 / 18 / 19 | same | |
| `+0xe0` / `+0xe4` | weaponmask / weaponskill | 20 / 21 | same | |
| `+0xe8` | numimpacts | | | |
| `+0xec` / `+0xf0` | swipeframeon / swipeframeoff | 23 / 24 | **24 / 25** | |
| `+0xf4` | maxfatigue | — | **23** | |
| `+0xf8` | impacts[6] × 0x5c | IMPACT | IMPACT | |

Formats: ATTACK `0x5d8fc4` (24 fields), FATIGUEATTACK `0x5d9080` (25
fields), PLAYANIM `0x5d90fc` `%30s, %i, %i, %i, %i, %i`. The file header
comments name the fields: ATTACK "…weaponmask, weaponskill, attackpcnt,
swipeframeon, swipeframeoff"; FATIGUEATTACK "…attackpcnt, maxfatigue,
swipeframeon, swipeframeoff". The 10th/11th fields are documented as
"nextwait, chainexptime" and stored at `+0xc4`/`+0xc0`. That is the 1998
member order {…, chainexptime, nextwait} moved up 4 bytes, so the port's
parse order matches.

- **ATTACK only:** if responsename[0] then `flags = (flags & ~1) | 2`
  (`0x489a40`). FATIGUEATTACK, MAGICATTACK and PLAYANIM skip this rule.
- Each kind sets lastattack (`[esp+0x10]`) to the stored record (or to the
  array's null element if the slot is null).
- 0x10000 is not among the CA_ defines in char.def's header (0x8000 →
  0x40000). It marks a fatigue attack.

**MAGICATTACK** (`0x5d903c`, `%30s, %i, %i, %i, %i, %i, %31s, %i, %i, %i, %i, %i`):
name `+0x00`, flags `+0x24` (then `|= 0x800000`), button `+0x28`,
attackpcnt `+0x2c`, mindist `+0x30`, maxdist `+0x34`, spellname[32]
`+0x38`, spellsource x/y/z `+0x58/+0x5c/+0x60`, **condition `+0x64`**
(MASTAT_NONE 1, HEALTHLT 2, HEALTHGT 3, MANALT 4, MANAGT 5, defined in
char.def), **condition value `+0x68`**. Example: `MAGICATTACK "invoke3", …,
"heal3", -1, -1, -1, MASTAT_HEALTHLT, 100`.

### 6.2 Impact record (0x5c bytes; stack copy zeroed, then `rep movsd` ×0x17)

`%30s, %i, %30s, %i, %i, %i, %i, %i` (`0x5d91dc`): impactname[32]
`+0x00`, **index `+0x20`** (its slot number, written before the copy),
flags `+0x24`, loopname[32] `+0x28`, looptime `+0x48`, damagemin `+0x4c`,
damagemax `+0x50`, snapdist `+0x54`, snaptime `+0x58`. CHARIMPACT's comment
names the last four "mindmgpcnt, maxdmgpcnt, pushpos, pushtime".

- If loopname[0] is set and `!(flags & 7)`, the load is fatal: "Used
  'loop' animation for impact with no STUN, KNOCKDOWN, or DEATH flag".
  This check runs before the parse-ok test.
- **IMPACT**: lastattack required ("ATTACK tag must preceed its IMPACT
  tags"), not magic ("IMPACT can not follow a MAGICATTACK tag"),
  `numimpacts < 6` ("Too many IMPACT tags for this ATTACK"). Then
  `flags &= ~0x1000` and the record is copied to `attack+0xf8+0x5c·n`,
  `attack+0xe8`++.
- **CHARIMPACT**: `+0x214 < 6` ("Too many CHARIMPACT tags…"). Then
  `flags |= 0x1000` and the record is copied to `chardata+0x218+0x5c·n`,
  `+0x214`++.

## 7. STATLEVEL (`0x49c9f0`; tables at `*(Rules+0x58)` = int*[6], int[31] each)

The name is matched with **exact case** (`repe cmpsb`, NUL included):
Strength 0, Constitution 1, Agility 2, Reflexes 3, Mind 4, Luck 5. Any
other name gives "Invalid Stat Level Type in Rules.def" (`0x481d10`) and
returns 0, which is fatal in Load. The block is: SkipBlanks, BEGIN, LineGet;
lines `ENTRY %i %i` (any other tag is fatal: "Invalid character tag %s");
`table[level] = value` only when `level < 31`. Negative levels are
**not** checked, so they write out of bounds. Unset entries stay
−2,000,000. Get (`0x49cbf0`) returns 0 when there are no tables, stat ≥ 6
or value ≥ 31; it does not check for negatives.

## 8. WEAPON / ARMOR blocks (brief; gameflow ported them)

`0x48ac30` (record 0xd0): name `+0x00`, DESCRIPTION → `+0x20`, BASICMODS
`%i`×8 → `+0xa0..+0xbc` in file order, STATLINE → malloc'd string at
`+0xcc`. Defaults: `+0xb8`, `+0xc4` and `+0xc8` are −1 (type and class
filled by BindType). Any other tag is fatal ("Invalid character tag %s").
ARMOR `0x48b1a0` has the same tags (record 0xcc). Not re-verified field by
field.

## 9. RNG draws

None. No call to `random` `0x483300`, `rand` `0x58c582` or `srand`
`0x58c575` appears in any function in §1. Data loading takes no part in
the RNG tape.

## 10. Fixture plan (seams)

**D1 kata (parse).** Run `TRules::Initialize` `0x48b690` (ecx
`0x65d7a8`, zeroed) as original code, then BindTypes `0x48cab0`. Dump the
Rules object (0x254 bytes), every class, chardata, attack, impact, weapon
and armor record, and the 6 STATLEVEL tables, by field name using §3–§8.
The port dumps TRules and SCharData by the same names.

| Callee | Run as | Seam records |
|---|---|---|
| `0x4a13f0` zfopen_rel / `0x4a1240` zfopen | **seam**: return a fake ZFILE handle onto the case's bytes for that path | path, mode, looseFirst |
| `0x4a17b0` length, `0x4a15a0` read, `0x4a1540` close | **seam** (on the fake handle) | handle |
| `0x4a1c00` FileExists | **seam**: answer from the case's file set | path |
| `0x481c10` FatalError, `0x481d10` | **seam**: record the formatted message, stop the case (expected for the error cases) | fmt, arg |
| `0x475210` FindObjType (inside BindTypes), class list `0x65a148` / count `0x65a258` | **seam** or a stand-in class table from the case | name |
| WaitForSingleObject / ReleaseMutex (`0x65b8ac`) | no-op seam | — |
| `0x4789c0`, `0x4788d0` (+XOR 0xCC), tokenizer `0x478a10`…, Parse `0x479ad0`, all parsers, `0x41c840` array add, `0x48d230`, malloc `0x482fb0` / free `0x4830f0`, `_stricmp` / `_strnicmp` / `toupper` / `_strupr` (C locale: `[0x676fdc]` = 0) | original | — |

Globals to set: ClassDefPath `0x65bd48` = `.\Resources\`, ImageryPath
`0x65bc44` = `.\Imagery\`, ResourcePath `0x65dde8` = `.\Resources\`,
ModulesPath `0x65d6a4` = `.\Modules\`, workDir `0x65d254` = exeDir
`0x6666cc` = `C:\REVENANT\`, GameSpeed `0x5d79e4` = 3 (also run 5 for
TWILIGHT), `0x65b9f8` = 0 (FatalError re-entry guard).

Cases: (a) the shipped resources.rvr rules.def + imagery.rvi
char/weapon/armor.def + stats.def (golden); (b) GOG loose rules.def and
char.def (the value diffs in §2.5); (c) one per error string (unknown tag,
repeated CHARACTER, CLASS not found, 4 or 6 TOHITDAMAGE entries, FLAGS
tag, BLEEDER with no value, 7 IMPACTs, loop with no flag, STATLEVEL bad
name or level 31); (d) a scrambled (XOR 0xCC) file; (e) `#define` used
inside and outside a block.

**D0 kata (lookup order).** Run `0x4a13f0` / `0x4a1240` / `0x4a0380` /
`0x4a1c30` as original over pack objects built by the original Open
`0x49ee20`. Seam the CRT file calls (`fopen` `0x58b5db`, `fread`, `fseek`,
`ftell`, `fclose`, `_findfirst` `0x58c680`, `_findclose`) onto a per-case
host tree, and record every CRT fopen path. Cases: {loose only, pack only,
both, neither} × looseFirst {0, 1} × path form {`.\Resources\x`,
`Resources\x`, `C:\…`, `..\x`} × workDir == exeDir or not × case
differences in names. On the port side, `REVENANT_VFS_TRACE=1` logs pack /
loose / by-name for each open.

## 11. Port divergences

Port = `worktrees/combat` (feature/combat @ fa8f555); `main` noted where
it differs.

**File resolution**
1. **main still opens loose files first.** `rev_fopen` on `main` tries
   SavePath, the overlay, RunPath, the module dir, the data root, and only
   then the VFS, so it reads the 86,689-byte 1998 `data/Resources/rules.def`.
   feature/combat (`36faa5f`, `src/revutils.cpp:1552-1576`, `1824-1892`)
   searches the packs first in each root, like retail, and its
   `data/Resources/rules.def` is the stock 8,642-byte file (`9bce7c8`).
   The dojo doc §5.6 ("the port reads the 1998 file") is out of date for
   feature/combat.
2. Search roots: the port uses SavePath, the overlay, then RunPath
   (`revutils.cpp:1707-1715`, a REVSYNC-DIVERGENCE note); retail uses
   workDir then exeDir. The port also has fallbacks retail lacks (module
   dir, data root, by-name lookup across packs, `:1865-1891`).
3. `MountModule` (`revutils.cpp:1771-1805`) mounts an unpacked
   `Modules/<name>/` folder **instead of** `<name>.rvm` when the folder
   exists (lab: Deathmatch6, Demo). Retail mounts the .rvm, and the pack
   wins. The port treats a missing base pack as fatal (`revmain.cpp:2628-2631`);
   retail carries on with loose files (§2.2).
4. No XOR-0xCC decode for scrambled .def files (`parse.cpp` has none).
   The shipped data is plain, so there is no effect today.

**rules.def (`TRules::LoadFile`, `src/rules.cpp:683-871`)**
5. **TOHITCENTER, TOHITRANGECHAR, TOHITRANGEPLYR, TOHITBLOCK, TOHITFACE,
   TOHITDAMAGE and AMMODATA are skipped** (`rules.cpp:839-856`, logged as
   warnings in the arena runs). TRules has no fields for them, so the
   port's to-hit is still the 1998 formula (`character.cpp:1659-1675`) and
   ignores the retail table (C4).
6. An unknown tag is skipped (port) but fatal in retail. A repeated
   CHARACTER replaces the earlier one (`:754-772`) but is fatal in retail;
   the same goes for WEAPON/ARMOR (`:812-818`).
7. Missing Initialize defaults: recov rates 1, poison rate 1, TOHIT* and
   AMMODATA. TRules members have no initializers (`rules.h:295-309`). The
   shipped file sets every value the port parses, so this has no data
   effect.
8. `ConvertMinutesToFrames` (`playscreen.cpp:264-268`) is `m·60·30` (kGameFrameRate 30, `playscreen.cpp:244`);
   retail is `m·daylength/1440`. TWILIGHT steps differ when GameSpeed is 5
   (default is 3).
9. After a WEAPON block the port calls WhiteGet (`:820`); retail calls Get.
   This is a tokenizer edge case only.

**CLASS (`SClassData::Load`, `rules.cpp:33-117`)**
10. **WEAPONS (`+0x70`, default 0xffff) is skipped** (`:92-101`), so the
    Shaman's weapon restriction (`WM_BLUDGEON | WM_BOW | WM_CROSSBOW`) is
    missing. An unknown class tag is fatal in retail.

**CHARACTER (`SCharData::Load`, `rules.cpp:242-610`)**
11. **GROUPS / ENEMIES are 48 bytes, `%48s`** (`rules.h:85`,
    `rules.cpp:493-500`); retail uses 80 / `%80s`. 11 of 58 ENEMIES lists
    get cut off at 47 characters:
    - Locke, Bayne, Morganna and Navarro keep
      `player,trainer,human,humanoid,beast,spider,supe`. They lose
      `supernatural,undead`, which are the groups of the golems, dragons,
      Skeleton, Zombie, Wraith, Demon Mage, Dark Revenant, Undead
      Sorcerer, Styxx, Solifuge, Yhagoro and Sidious.
    - The three golems lose White, Blue and Red Dragon.
    - The four dragons lose Zombie and Skeleton.

    IsEnemy (`character.cpp:2459-2467`) reads this list. Its effect on
    targeting is for M4.
12. **ATTACK trailing fields are dropped** (`:299-318`): swipeframeon /
    swipeframeoff (`+0xec/+0xf0`).
13. **FATIGUEATTACK is parsed as an ATTACK** (`:288-293`): maxfatigue
    (`+0xf4`, field 23) is lost, the 0x10000 flag is never set, the
    response rule is applied (retail doesn't apply it here) and the swipe
    fields are lost. All 45 FATIGUEATTACKs are affected.
14. **The MAGICATTACK condition is dropped** (`:340-346`): `+0x64`
    MASTAT_* and its `+0x68` value (83 uses, e.g. "heal3" only when health
    < 100).
15. **Retail tags the port skips** (`:575-591`): MAGICFREQ (default
    100/250; 6 uses, Locke 50,170), BLEEDER/BLEADER (default **1**; 8
    characters set 0), NOPARALYZE (5), RETREATAT/RETREATFOR/RETREATATMANA
    (4/4/0; RETREATFOR default −1), RUNFATIGUE (default 1,100; Locke
    10,100), POISONCHANCE, LABELHEIGHT, SWIPEFULL, ATTACHEFFECT. An
    unknown char tag is fatal in retail.
16. Impacts have no index field (`+0x20`) and no 0x1000 CHARIMPACT/IMPACT
    flag (`:371-418`).
17. Defaults (`rules.cpp:184-233`):
    - blocksounds: retail `"block1,block2,block2"`, port
      `"clang1,clang2,clang3"`. 9 characters have no BLOCKSOUNDS.
    - arrowpos: retail −1, then by name (−5,−15,60), (−5,−15,80) Bayne,
      (−10,−15,50) Morganna, (−10,−15,30) Navarro; port (15,15,60). No
      character sets ARROWPOS.
    - labelheight: the port doesn't have it.
18. Binding: the port looks up objtype while parsing
    (`CharacterClass.FindObjType`, then PlayerClass; fatal "Invalid
    character type", `:246-263`), so class.def has to be loaded first.
    Retail sets −2 and binds by name later; names that aren't types are
    silently inert. The port adds errors retail lacks: "Monsters/NPC's do
    not have character classes" (`:421`) and "Class required for player
    characters" (`:602`). On the other hand, the port turns an unknown
    CLASS into null where retail stops with a fatal error.
    `GetCharData`'s static fallback (`:895-919`, speeds 6/12/3/4) is
    port-only; retail returns def or null.
19. Matches retail: field order of ATTACK fields 1–22 (including
    nextwait → `+0xc4`, chainexptime → `+0xc0`), MAGICATTACK 1–10,
    PLAYANIM, IMPACT, DAMAGEMODS raw-number loop, COMBATRANGE `+64` rule,
    MAXATTACKRANGE 32-on-failure, the FLAGS pass-by-value bug, damagemods
    [0]-only init, `%Ns` semantics (memset N, strncpy N−1: retail
    `0x47a0b0`), STATLEVEL (`playerstats.cpp:32-93`, except that negative
    levels are guarded), experience tables (`rules.cpp:922-958`).

## 12. Open questions

- Which screens go through the loose-first loader `0x4377c0`
  (options/joingame/mpingame.def)? Callers pass the name in a register
  (`0x435120`, `0x436223`); only connect.def and connectsimple.def were
  seen as literals.
- What does the 7th ATTACK string (`+0x98`) do: is it a chain name (port)
  or a death animation (retail char.def comment)? This needs the retail
  attack-choice and chain code (C3).
- Who reads the third experience table at Rules `+0x1e0` (500, +2000i+500)?
- Who reads the new fields: bleeder `+0x164`, swipefull `+0x16c`,
  retreat* `+0x440..`, runfatigue `+0x44c`, noparalyze `+0x454`,
  poisonchance `+0x588`, labelheight `+0x58c`, attacheffect `+0x458`,
  attack `+0xec/+0xf0/+0xf4`, magic `+0x64/+0x68`, impact flag 0x1000,
  attack flag 0x10000? Not traced. They belong to the C/S katas.
- The 1998 loose files and the archive copies: which is older? The
  archive char.def (1999-10-08) is the newest .def in either pack. The
  loose GOG files carry no usable date.
- `0x487300` (the ImageryPath fallback check) and pack `OpenEntry`
  `0x4a06a0` were not traced.
