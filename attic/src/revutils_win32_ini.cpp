// Retired from src/revutils.cpp during Subsystem 5 port (Win32 INI →
// simpleini). This is the pre-release 1999 code that called
// GetPrivateProfileString / WritePrivateProfileString / GetPrivateProfileInt
// directly. Replaced by the CSimpleIniA-based bodies in src/revutils.cpp.
//
// Kept here as reference for the original data-flow: every INIGet*
// re-writes the key back via INISet*, so the .ini file on disk ends up as a
// fully materialised record of every defaulted value the game queried. The
// modern port preserves that behaviour by calling SaveFile() after each
// Set.

// *************** Settings Functions *****************

static TString INIPath;
static TString INISection;

void INISetSection(const char* newsection)
{
    INISection = newsection;
}

void INISetPath(const char *runpath)
{
    char *ininame = "Revenant.ini";

    INIPath = SavePath;
    INIPath.Append(ininame);

  // Make sure INI file is in writable (SavePath) directory
    if (stricmp(RunPath, SavePath) != 0) // Run/Save path are different
    {
        FILE *fp = fopen(INIPath.CStr(), "r");
        if (!fp)    // INI file not in SavePath
        {
            char from[MAXPATHLEN];
            strcpy(from, RunPath);
            strcat(from, ininame);
            copyfiles(from, INIPath.CStr());
        }
    }
}

int32_t INIGetInt(const char *key, int32_t def, const char *format)
{
    int32_t i = GetPrivateProfileInt(INISection, key, def, INIPath);

    INISetInt(key, i, format);

    return i;
}

void INISetInt(const char *key, int32_t i, char *format)
{
    if (!format)
        format = "%d";

    char buf[20];
    sprintf(buf, format, i);

    WritePrivateProfileString(INISection, key, buf, INIPath);
}

char *INIGetText(const char *key, char *def, char *buf, int32_t buflen)
{
    static char s[128];
    if (!buf)
    {
        buf = s;
        buflen = 128;
    }

    if (!def)
        def = "";

  // Quote the string
    char qdef[128];
    strncpyz(qdef, "\"", 128);
    strncatz(qdef, def, 128);
    strncatz(qdef, "\"", 128);

    GetPrivateProfileString(INISection, key, qdef, buf, buflen, INIPath);

    if (buf[0] == '\"')
    {
        int32_t l = strlen(buf);
        memmove(buf, buf + 1, l - 2);
        buf[l - 2] = nullptr;
    }

    INISetText(key, buf);

    return buf;
}

void INISetText(char *key, char *str)
{
  // Quote the string
    char qstr[128];
    strncpyz(qstr, "\"", 128);
    strncatz(qstr, str, 128);
    strncatz(qstr, "\"", 128);

    WritePrivateProfileString(INISection, key, qstr, INIPath);
}

char *INIGetStr(char *key, char *def, char *buf, int32_t buflen)
{
    static char s[128];
    if (!buf)
    {
        buf = s;
        buflen = 128;
    }

    if (!def)
        def = "";

    GetPrivateProfileString(INISection, key, def, buf, buflen, INIPath);

    INISetStr(key, buf);

    return buf;
}

void INISetStr(char *key, char *str)
{
    WritePrivateProfileString(INISection, key, str, INIPath);
}

int32_t INIGetArray(char *key, int32_t size, int32_t *ary, int32_t defsize, int32_t *defary, char *format)
{
    char buf[128];

    if (ary != defary)
        memset(ary, 0, sizeof(int32_t) * size);

    GetPrivateProfileString(INISection, key, "", buf, 128, INIPath);

    int32_t newsize = 0;
    if (!buf[0])
    {
        if (defary)
        {
            while (newsize < defsize && newsize < size)
            {
                ary[newsize] = defary[newsize];
                newsize++;
            }
        }
    }
    else
    {
        char *tok = nullptr;
        do
        {
            tok = strtok((newsize < 1) ? buf : nullptr, ",");
            if (tok)
            {
                ary[newsize] = atol(tok);
                newsize++;
            }
        } while (tok && newsize < size);
    }

    INISetArray(key, newsize, ary, format);

    return newsize;
}

void INISetArray(char *key, int32_t size, int32_t ary[], char *format)
{
    char buf[128];

    if (!format)
        format = "%d";

    buf[0] = nullptr;
    for (int32_t c = 0; c < size; c++)
    {
        if (c >= 1)
            strcat(buf, ",");
        int32_t len = strlen(buf);
        sprintf(buf + len, format, ary[c]);
    }

    WritePrivateProfileString(INISection, key, buf, INIPath);
}

bool INIGetBool(char *key, bool def, char *yes, char *no)
{
    char buf[30];
    char getyes[30], getno[30];

    if (!yes)
        strcpy(getyes,"yes on true 1");
    else
    {
        strcpy(getyes, yes);
        strlwr(getyes);
    }

    if (!no)
        strcpy(getno, "no off false 0");
    else
    {
        strcpy(getno, no);
        strlwr(getno);
    }

    GetPrivateProfileString(INISection, key, "", buf, 30, INIPath);
    strlwr(buf);

    bool b;
    if (!strstr(yes, buf))
        b = true;
    else if (!strstr(no, buf))
        b = false;
    else
        b = def;

    INISetBool(key, b, yes, no);

    return b;
}

void INISetBool(char *key, bool on, char *yes, char *no)
{
    if (!yes)
        yes = "1";
    if (!no)
        no = "0";

    WritePrivateProfileString(INISection, key, on ? yes : no, INIPath);
}

bool INIGetYesNo(const char *key, bool def)
{
    return INIGetBool(key, def, "Yes", "No");
}

void INISetYesNo(const char *key, bool on)
{
    INISetBool(key, on, "Yes", "No");
}

bool INIGetTrueFalse(const char *key, bool def)
{
    return INIGetBool(key, def, "True", "False");
}

void INISetTrueFalse(const char *key, bool on)
{
    INISetBool(key, on, "True", "False");
}

bool INIGetOnOff(const char *key, bool def)
{
    return INIGetBool(key, def, "On", "Off");
}

void INISetOnOff(const char *key, bool on)
{
    INISetBool(key, on, "On", "Off");
}

// Grab the ParseAnything function from PARSE.CPP
bool ParseAnything(bool stack, TToken &t, const char *format, va_list ap);

bool INIParse(const char *key, cons char *def, const char *format, ...)
{
    char buf[128];

    INIGetStr(key, def, buf, 128);

    va_list ap;
    va_start(ap, format);

    TStringParseStream s(buf, strlen(buf));
    TToken t(s);
    t.Get();

    bool retval = ParseAnything(true, t, format, ap);

    va_end(ap);

    return retval;
}

void INIPrint(const char *key, const char *format, ...)
{
    char buf[128];

    va_list marker;
    va_start(marker, format);

    vsprintf(buf, format, marker);

    va_end(marker);

    INISetStr(key, buf);
}
