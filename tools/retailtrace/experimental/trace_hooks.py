#!/usr/bin/env python3
"""The gameflow retail trace: hook spec + handlers for function_hook.py.

Every hook target, the field offsets the handlers read, and the retail-vs-
port line format it reproduces are documented at the call site. The handler
C is freestanding and reads game memory only; it writes nothing but the log.

Trace lines (RETAIL_TRACE.md §2), each prefixed with the game tick so two
runs line up by simulation time:

    <tick> [dialog] <speaker> says: <text>
    <tick> [script] <object>: <line>
    <tick> [script] <object>: trigger <N> of '<proto>' starts
    <tick> [script] <object>: trigger <N> ends

They match the port's own trace (src/dialog.cpp, src/script.cpp) so the two
diff by line sequence.

Addresses (retail Revenant.exe, Ghidra project RevenantDev):

    TCharacter::Say          0x004d0610  (this, text, frames, anim, sound)
    CommandInterpreter       0x0041e8e0  (context, token, abbrevlen, script)  cdecl
    TScript::Start           0x00492440  (this=script, startproto, pos, priority)
    TScript::End             0x00493e40  (this=script)

Object/struct fields used (forensics in docs/gameflow/forensics/):

    object  +0x38  name (char*)                         EXITS.md, SCRIPT_ENGINE.md
    script  +0x08  curproto    +0x0c owner (context)     SCRIPT_ENGINE.md §2
    script  +0x1c  running trigger type   +0x48 ip
    proto   +0x00  name (char*)   +0x1c trigger ptr-array   +0x20 fallback   +0x44 count
    trigger +0x00  type  +0x04 pos  +0x38 priority        script.h SScriptTrigger
    token   +0x0c  stream    (TToken::Get 0x00478a10 reads [this+0x0c])
    stream  +0x08  buf  +0x10 ptr  (TStringParseStream, built in Continue 0x004933d0)
    global  0x00667fd0  current TScreen*;  TScreen +0x48 frame counter (24 Hz)
    CRT     0x0058b100  sprintf(buf, fmt, ...)  -> length (used widely in-game)
"""

# kernel32 imports the runtime needs (IAT slots resolved by function_hook.py).
IAT_IMPORTS = ('CreateFileA', 'WriteFile', 'SetFilePointer', 'CloseHandle')


def runtime_c(iat):
    """Shared C runtime: log file, tick, and one line emitter."""
    return f'''
typedef unsigned int U32;
typedef int (__cdecl  *Sprintf)(char *, const char *, ...);
typedef U32 (__stdcall *CreateFileA_t)(const char *, U32, U32, void *, U32, U32, U32);
typedef int (__stdcall *WriteFile_t)(U32, const void *, U32, U32 *, void *);

/* Fixed retail addresses. */
#define GAME_SPRINTF   ((Sprintf)0x0058b100u)
#define CUR_SCREEN_PTR (*(U32 *)0x00667fd0u)
#define SCREEN_FRAMES  0x48u

#define IAT_CREATEFILE (*(U32 *)0x{iat['CreateFileA']:08x}u)
#define IAT_WRITEFILE  (*(U32 *)0x{iat['WriteFile']:08x}u)

static U32 g_handle = 0;
static int g_opened = 0;
static const char g_logpath[] = "C:\\\\REVENANT\\\\retail-trace.log";

/* Current simulation tick: the current screen's 24 Hz frame counter. 0 before
   a screen exists. Read-only. */
static U32 trace_tick(void)
{{
    U32 screen = CUR_SCREEN_PTR;
    if (!screen)
        return 0;
    return *(U32 *)(screen + SCREEN_FRAMES);
}}

/* Lazily (re)create the log beside the exe, truncating on the first line of a
   run, and keep the handle open; one WriteFile per line so a crash keeps what
   was written. */
static void trace_emit(const char *line, int len)
{{
    U32 written = 0;
    volatile WriteFile_t wf;
    if (len <= 0)
        return;
    if (!g_opened)
    {{
        volatile CreateFileA_t cf = (CreateFileA_t)IAT_CREATEFILE;
        g_handle = cf(g_logpath, 0x40000000u /*GENERIC_WRITE*/, 1 /*SHARE_READ*/,
                      0, 2 /*CREATE_ALWAYS*/, 0x80 /*NORMAL*/, 0);
        g_opened = 1;
    }}
    if (g_handle == 0xffffffffu)
        return;
    wf = (WriteFile_t)IAT_WRITEFILE;
    wf(g_handle, line, (U32)len, &written, 0);
}}

/* The name field of a game object (+0x38), guarded. */
static const char *obj_name(U32 obj)
{{
    const char *n;
    if (!obj)
        return "?";
    n = *(const char **)(obj + 0x38);
    return n ? n : "?";
}}
'''


# ---- Handlers ----------------------------------------------------------------
# Each handler takes the pushad block pointer `f` (dword array). In pushad
# order f[6]=ECX (this) and f[7]=EAX; the function's stack args begin at f[10]
# (f[8]=EFLAGS, f[9]=return address).

SAY_C = r'''
/* TCharacter::Say(text, frames, anim, sound): this=ECX, text=arg0.
   -> "<tick> [dialog] <name> says: <text>" (src/dialog.cpp AddSpeech). */
void h_say(const U32 *f)
{
    U32 self = f[6];
    const char *text = (const char *)f[10];
    char buf[600];
    int n;
    if (!text)
        return;
    n = GAME_SPRINTF(buf, "%u [dialog] %s says: %s\r\n",
                     trace_tick(), obj_name(self), text);
    trace_emit(buf, n);
}
'''

CMD_C = r'''
/* CommandInterpreter(context, token, abbrevlen, script) cdecl: args at f[10..].
   The line is reconstructed from the token's stream: back up to the start of
   the current line, trim leading blanks, stop at end of line (src/script.cpp
   TraceLine). -> "<tick> [script] <object>: <line>". */
void h_cmd(const U32 *f)
{
    U32 context = f[10];
    U32 token = f[11];
    char line[512];
    char buf[700];
    int i = 0, n;
    if (token)
    {
        U32 stream = *(U32 *)(token + 0x0c);
        if (stream)
        {
            const char *buf0 = *(const char **)(stream + 0x08);
            const char *ptr  = *(const char **)(stream + 0x10);
            const char *s = ptr;
            while (s > buf0 && s[-1] != '\n' && s[-1] != '\r')
                --s;
            while (*s == ' ' || *s == '\t')
                ++s;
            while (i < (int)sizeof(line) - 1 && *s && *s != '\n' && *s != '\r')
                line[i++] = *s++;
        }
    }
    line[i] = 0;
    n = GAME_SPRINTF(buf, "%u [script] %s: %s\r\n",
                     trace_tick(), obj_name(context), line);
    trace_emit(buf, n);
}
'''

START_C = r'''
/* TScript::Start(startproto, pos, priority): this=script=ECX, args at f[10..].
   Context = owner (script+0x0c). N = the firing trigger's type, found in the
   proto's trigger table by its pos (the type field is written to script+0x1c
   only after Start returns, so it is read from the record here).
   -> "<tick> [script] <object>: trigger <N> of '<proto>' starts". */
void h_start(const U32 *f)
{
    U32 script = f[6];
    U32 proto = f[10];
    U32 pos = f[11];
    U32 context = *(U32 *)(script + 0x0c);
    const char *pname = proto ? (*(const char **)proto ? *(const char **)proto : "?") : "?";
    int n_type = -1;
    char buf[600];
    int n;
    if (proto)
    {
        int count = *(int *)(proto + 0x44);
        U32 *arr = *(U32 **)(proto + 0x1c);
        int i;
        for (i = 0; i < count; ++i)
        {
            U32 rec = arr ? arr[i] : 0;
            if (!rec)
                rec = *(U32 *)(proto + 0x20);
            if (rec && *(U32 *)(rec + 0x04) == pos)
            {
                n_type = *(int *)rec;
                break;
            }
        }
    }
    n = GAME_SPRINTF(buf, "%u [script] %s: trigger %d of '%s' starts\r\n",
                     trace_tick(), obj_name(context), n_type, pname);
    trace_emit(buf, n);
}
'''

END_C = r'''
/* TScript::End(): this=script=ECX. Callers only invoke it on a running script
   (ip at +0x48 set); guard on that so resets of idle scripts do not log.
   Context = owner (+0x0c), N = running trigger type (+0x1c).
   -> "<tick> [script] <object>: trigger <N> ends". */
void h_end(const U32 *f)
{
    U32 script = f[6];
    U32 context;
    int n_type;
    char buf[600];
    int n;
    if (*(U32 *)(script + 0x48) == 0)
        return;
    context = *(U32 *)(script + 0x0c);
    n_type = *(int *)(script + 0x1c);
    n = GAME_SPRINTF(buf, "%u [script] %s: trigger %d ends\r\n",
                     trace_tick(), obj_name(context), n_type);
    trace_emit(buf, n);
}
'''


HOOKS = [
    dict(name='say',   target=0x004d0610, handler='h_say',   c=SAY_C,
         capture='this=speaker, arg0=text'),
    dict(name='cmd',   target=0x0041e8e0, handler='h_cmd',   c=CMD_C,
         capture='arg0=context, arg1=token, arg3=script'),
    dict(name='start', target=0x00492440, handler='h_start', c=START_C,
         capture='this=script, arg0=proto, arg1=pos'),
    dict(name='end',   target=0x00493e40, handler='h_end',   c=END_C,
         capture='this=script'),
]
