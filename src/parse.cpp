// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               parse.cpp - Low-level file parsing functions            *
// *************************************************************************

#include "parse.h"

#include <cctype>
#include <cstdarg>

// **************************************
// * Identifiers - Array of identifiers *
// **************************************

TIdent::TIdent()
{
    idents = nullptr;
    numidents = 0;
}

TIdent::~TIdent()
{
    if (idents)
        delete idents;
}

Ident *TIdent::Find(const char *text, int32_t type)
{
    for (int32_t c = 0; c < numidents; c++)
    {
        if (!strcmp(text, idents[c].ident) && type == idents[c].type)
        return &(idents[c]);
    }
    return nullptr;
}

int32_t TIdent::FindIndex(const char *text, int32_t type)
{
    for (int32_t c = 0; c < numidents; c++)
    {
        if (!strcmp(text, idents[c].ident) && type == idents[c].type)
        return c;
    }
    return -1;
}

int32_t TIdent::Add(const char *ident, int32_t type, int32_t value)
{
    if (!idents)
        idents = new Ident[MAXIDENTS];
    if (!idents)
        FatalError("FATAL ERROR: Couldn't allocate identifier buffer\n");

    if (numidents + 1 > MAXIDENTS)
        return -1;

    strcpy(idents[numidents].ident, ident);
    idents[numidents].type = type;
    idents[numidents].value = value;

    numidents++;

    return numidents - 1;
}

// ************************
// * TToken - Token class *
// ************************

// Defs for parsing the object scripts
#define NUMKEYWORDS     4

char *keywords[NUMKEYWORDS] =
{
    "BEGIN",
    "END",
    "#define",
    "#include"
};

// REVSYNC: 0x00478a10 reads every character this way: carriage returns are
// skipped wherever they are, and a line feed counts a line as it is read.
// So the character a token holds after it is never a CR: after an identifier
// that ends a CRLF line the stream has passed the line feed, and the line is
// already counted. (The 1998 tokenizer skipped a CR only at a token's start
// and counted a line when it returned the line break.) Script stepping
// depends on it: where Jump resumes, what a token held across a SetPos reads
// next (Continue's `ELSE IF`), the line numbers of script errors.
int32_t TToken::ReadChar()
{
    int32_t ch;
    do
        ch = stream->GetChar();
    while (ch == '\r');
    if (ch == '\n')
        linenum++;
    return ch;
}

void TToken::Get()
{
    char ch;
    int32_t c;
    int32_t pos;

    if (lastch)
    {
        ch = lastch;
        lastch = 0;
    }
    else ch = ReadChar();

  // Line continuation char
    if (ch == '\\')
    {
        while (ch != '\n' && ch != EOF)
            ch = ReadChar();
        if (ch == '\n')
            ch = ReadChar();
    }

  // End of file
    if (ch == EOF)
    {
        text[0] = 0;
        type = TKN_EOF;
        return;
    }

  // Remark
    else if (ch == '/')
    {
        lastch = ReadChar();
        if (lastch == '/')
        {
            while (lastch != '\n' && lastch != EOF)
                lastch = ReadChar();
            lastch = 0;
            type = TKN_RETURN;
            text[0] = '\n';
            text[1] = 0;
            return;
        }
        else if (lastch == '*')
        {
            lastch = ReadChar();
            do
            {
              while (lastch != '*' && lastch != EOF)
                lastch = ReadChar();
              lastch = ReadChar();
            } while (lastch != '/' && lastch != EOF);
            lastch = ReadChar();
        }
        else
        {
            type = TKN_SYMBOL;
            code = ch;
            text[0] = ch;
            text[1] = 0;
            type = TKN_SYMBOL;
            return;
        }
        type = TKN_WHITESPACE;
        index = 0;
        pos = 0;
        return;
    }

  // Whitespace
    else if ((ch == ' ') || (ch == '\t'))
    {
        type = TKN_WHITESPACE;
        index = 0;
        pos = 0;
        text[pos++] = ch;
        do
        {
            ch = ReadChar();
            if (pos < MAXTOKENTEXT - 1)
                text[pos++] = ch;
        }
        while ((ch == ' ') || (ch == '\t'));
        text[pos] = 0;
        lastch = ch;
        return;
    }

  // Return
    else if (ch == '\n')
    {
        type = TKN_RETURN;
        text[0] = '\n';
        text[1] = 0;
        return;
    }


  // Text
    else if (ch == '\"')
    {
        pos = 0;
moretext:
        ch = ReadChar();
        while (ch != EOF && ch != '\"' && ch != '\n')
        {
            if (pos >= MAXTOKENTEXT - 1)
                Error("String too long");
            if (ch == '\\' && !literalbackslash)
            {
                // REVSYNC: 0x00478c9c -- \n, \r, \t, \\ and \xXX (two hex
                // digits, either case); after any other character the
                // backslash stays and the character is read as itself.
                ch = ReadChar();
                switch (ch)
                {
                  case 'n':  text[pos++] = '\n'; ch = ReadChar(); break;
                  case 'r':  text[pos++] = '\r'; ch = ReadChar(); break;
                  case 't':  text[pos++] = '\t'; ch = ReadChar(); break;
                  case '\\': text[pos++] = '\\'; ch = ReadChar(); break;
                  case 'x':
                  {
                    int32_t value = 0;
                    for (int32_t digit = 0; digit < 2; digit++)
                    {
                        const int32_t c = toupper(ReadChar());
                        if (c >= '0' && c <= '9')
                            value = value * 16 + (c - '0');
                        else if (c >= 'A' && c <= 'F')
                            value = value * 16 + (c - 'A' + 10);
                        else
                            Error("Illegal \\xXX character in string");
                    }
                    text[pos++] = char(value);
                    ch = ReadChar();
                    break;
                  }
                  default:   text[pos++] = '\\'; break;
                }
            }
            else
            {
                text[pos++] = ch;
                ch = ReadChar();
            }
        }
        /*
        if (ch != '\"')
            Error("ERROR: Unterminated string");
        */
        lastch = ReadChar();
        if (lastch == '\\')
        {
            while (lastch != '\n' && lastch != EOF)
                lastch = ReadChar();
            do
            {
                ch = ReadChar();  // Skip whitespace (note, will not return a whitespace token)
            }
            while ((ch == ' ') || (ch == '\t'));
            lastch = ch;
            if (ch == '\"')                  // Skip
                goto moretext;
        }
        text[pos] = 0;
        type = TKN_TEXT;
        index = pos;
        return;
    }

  // Identifier or keyword
    else if ((toupper(ch) >= 'A' && toupper(ch) <= 'Z') || ch == '#')
    {
        char upch;
        pos = 0;
        if (ch == '#')
        {
            text[pos++] = ch;
            ch = ReadChar();
            if (toupper(ch) < 'A' || toupper(ch) > 'Z')
            {
                type = TKN_SYMBOL;
                code = '#';
                text[1] = 0;
                lastch = ch;
                return;
            }
        }
        do
        {
            text[pos++] = ch;
            ch = ReadChar();
            upch = toupper(ch);
        } while ((upch >= 'A' && upch <= 'Z') ||
          (ch >= '0' && ch <= '9') ||
          (ch == '_'));
        text[pos] = 0;
        lastch = ch;

      // Find keyword
        for (c = 0; c < NUMKEYWORDS; c++)
        {
            if (!stricmp(text, keywords[c]))
            {
                type = TKN_KEYWORD;
                code = c;
                return;
            }
        }

      // Find identifer
        c = idents.FindIndex(text);
        if (c >= 0)
        {
            type = TKN_DEFINE;
            code = idents.Get(c)->value;
            number = code;
            index = c;
            return;
        }
        else
        {
            type = TKN_IDENT;
            code = -1;
            number = 0;
            index = -1;
            return;
        }
    }

  // Number
    else if ((ch >= '0' && ch <= '9') || (ch == '-'))
    {
        pos = 0;
        int32_t numdec = 0;
        bool ishex = false;

      // Check for HEX
        if (ch == '0')
        {
            text[pos++] = ch;
            ch = ReadChar();
            if (ch == 'x')
            {
                ishex = true;
                text[pos++] = ch;
                ch = ReadChar();
                number = 0;
                while ((ch >= '0' && ch <= '9') ||
                    (ch >= 'A' && ch <= 'F') ||
                    (ch >= 'a' && ch <= 'f'))
                {
                    ch = toupper(ch);
                    if (ch <=  '9') number = number * 16 + ch - '0';
                      else number = number * 16 + 10 + ch - 'A';
                    text[pos++] = ch;
                    ch = ReadChar();
                }
                if (pos <= 2)
                    Error("Invalid HEX constant"); 
                text[pos] = 0;
            }
        }

      // Hey.. it's not HEX
        if (!ishex)
        {
            if (ch == '-')
            {
                text[pos++] = ch;
                ch = ReadChar();
                if (!(ch >= '0' && ch <= '9'))
                {
                    lastch = ch;
                    type = TKN_SYMBOL;
                    text[1] = 0;
                    return;
                }
            }
            while ((ch >= '0' && ch <= '9') || (ch == '.' && numdec <= 1))
            {
                text[pos++] = ch;
                ch = ReadChar();
                if (ch == '.') numdec++;
            }
            text[pos] = 0;
            number = atof(text);
        }

        lastch = ch;
        type = TKN_NUMBER;
        index = (int32_t)number;
        return;
    }

  // Symbol
    else if ((ch > ' ') && (ch <= '~'))
    {
        type = TKN_SYMBOL;
        code = ch;
        text[0] = ch;
        text[1] = 0;
        return;
    }

  // Invalid Character
    else
    {
        char chr[2];
        chr[0] = ch;
        chr[1] = 0;
        Error("Invalid character '%s'", chr);

        return;
    }
}

bool TToken::DefineGet()
{
    LineGet();
    while (type == TKN_KEYWORD && code == KEY_DEFINE)
    {
        WhiteGet();
        char name[MAXIDENTLEN];
        int32_t val;
        if (!Parse(*this, "%32t %d", name, &val))
            return false;

        if (idents.Add(name, val) < 0)
            return false;

        LineGet();
    }

    return true;
}

void TToken::WhiteGet()
{
    Get();
    if (type == TKN_WHITESPACE) Get();
}

void TToken::LineGet()
{
    Get();
    while (type == TKN_WHITESPACE || type == TKN_RETURN)
        Get();
}

void TToken::SkipLine()
{
    while (type != TKN_RETURN && type != TKN_EOF)
        Get();
    Get();
}

bool TToken::SkipBlock()
{
    int32_t skipdepth = 0;

    if (Type() == TKN_EOF)
        return false;

    if (Type() == TKN_KEYWORD)
    {
        if (Code() == KEY_BEGIN)
            skipdepth++;
        else if (Code() == KEY_END)
            skipdepth--;
    }

    while (skipdepth)
    {
        Get();
        SkipBlanks();

        if (Type() == TKN_EOF)
        {
            if (skipdepth)
                return false;
            else
                break;
        }

        if (Type() == TKN_KEYWORD)
        {
            if (Code() == KEY_BEGIN)
                skipdepth++;
            else if (Code() == KEY_END)
                skipdepth--;
        }

        while (Type() != TKN_RETURN && Type() != TKN_EOF)
            Get();

        if (Type() == TKN_EOF)
        {
            if (skipdepth)
                return false;
            else
                break;
        }
    }

    return true;
}

void TToken::SkipBlanks()
{
    while (type == TKN_WHITESPACE || type == TKN_RETURN)
        Get();
}

int32_t abbrevcmp(const char *abbrev, const char *string)
// A string compare which allows abbreviation, ie abbrev = "fors" and
// string = "forsaken" would return 4.
{
    int32_t i;

    for (i = 0; *abbrev && *string; string++, abbrev++, i++)
        if (tolower(*abbrev) != tolower(*string))
            return 0;

    if (*abbrev && *string == 0)
        return 0;

    return i;
}

bool TToken::Is(const char *istext, int32_t abbrevlen) const
{
    if (abbrevlen)
        return (abbrevcmp(text, istext) >= abbrevlen);

    return (stricmp(istext, text) == 0);
}

void TToken::DoBegin()
{
    if (Type() != TKN_KEYWORD || Code() != KEY_BEGIN)
        Error("BEGIN Expected!");
    WhiteGet();
    if (Type() != TKN_RETURN)
        Error("RETURN Expected!");
    LineGet();
}

void TToken::DoEnd()
{
    if (Type() != TKN_KEYWORD || Code() != KEY_END)
        Error("END Expected!");
    WhiteGet();
    if (Type() != TKN_RETURN)
        Error("RETURN Expected!");
    LineGet();
}

// REVSYNC: TToken::GetRestOfLine @ 0x004799b0 -- the current token's text and
// the rest of its line into buf (len bytes, always terminated): words,
// strings and symbols as written, numbers as their integer value, each run
// of blanks as one space; the return is consumed. False when len < 0.
bool TToken::GetRestOfLine(char *buf, int32_t len)
{
    if (len < 0)
        return false;
    strcpy(buf, text);
    Get();
    while (type != TKN_RETURN && type != TKN_EOF)
    {
        const char *piece = nullptr;
        char number[16];
        if (type == TKN_IDENT || type == TKN_TEXT || type == TKN_SYMBOL)
            piece = text;
        else if (type == TKN_NUMBER)
        {
            snprintf(number, sizeof(number), "%d", index);
            piece = number;
        }
        else if (type == TKN_WHITESPACE)
            piece = " ";
        if (piece)
        {
            const size_t used = strlen(buf);
            strncpy(buf + used, piece, len - used - 1);
            buf[len - 1] = 0;
        }
        Get();
    }
    Get();
    return true;
}

void TToken::Error(const char *err, const char *extra)
{
    char buf[100];
    sprintf(buf, "Error in file %s line %d: ", stream->Name(), linenum);
    int32_t len = strlen(buf);
    sprintf(buf + len, err, extra);
    FatalError(buf);
}

// **************************************************
// * Parse - Parses and sets variables (like scanf) *
// **************************************************

bool ParseAnything(bool stack, TToken &t, const char *format, va_list ap)
{
    const char *f = format;
    char keyword[41];
    int32_t i, len;
    char *p;
    char **from = &p;

    if (!stack)
        from = va_arg(ap, char **);

    while (*f)
    {
        switch (*f)
        {

        // Spaces
          case ' ':
          case '\t':
          {
            f++;
            break;
          }

        // Continue to next line without using backslash (WHITESPACE INCLUDING RETURNS)
          case '\\':
          {
            f++;
            while (t.Type() == TKN_RETURN)
                t.LineGet();
            break;
          }

        // Return
          case '\n':
          {
            if (t.Type() != TKN_RETURN)
                return false;
            f++;
            t.LineGet();
            break;
          }

        // Optional
          case '<':
          {
            f++;
            i = 0;
            while (*f && isalnum(*f))
                keyword[i++] = toupper(*f++);
            keyword[i] = 0;
            if ((t.Type() != TKN_KEYWORD && t.Type() != TKN_IDENT)
              || stricmp(keyword, t.Text()))
            {
                if (*f == '>') // If no parameters to keyword, treat it as a boolean
                {
                    if (stack)
                        *va_arg(ap,bool *) = (bool)false;
                    else
                        *((bool *)(*from)++) = (bool)false;

                    f++;
                }
                else
                {
                    while (*f && *f != '>')
                    {
                        if (*f == '%')
                        {

                          // Get Len
                            f++;
                            if (isdigit(*f))
                            {
                                len = 0;
                                while (isdigit(*f))
                                    len = len * 10 + *f++ - '0';
                            } else len = 0;

                            switch (*f)
                            {
                              case 'b':
                                // REVSYNC: 0x00479ca5 -- an absent field's
                                // byte is skipped, not written (as the others)
                                if (stack)
                                    va_arg(ap, unsigned char *);
                                else
                                    *(unsigned char **)from += 1;
                                break;
                              case 'h':
                                if (stack)
                                    va_arg(ap,short *);
                                else
                                    *(short **)from += 1;
                                break;
                              case 'w':
                                if (stack)
                                    va_arg(ap, uint16_t *);
                                else
                                    *(uint16_t **)from += 1;
                                break;
                              case 'i':
                                if (stack)
                                    va_arg(ap, int32_t *);
                                else
                                    *(int32_t **)from += 1;
                                break;
                              case 'd':
                                if (stack)
                                    va_arg(ap, uint32_t *);
                                else
                                    *(uint32_t **)from += 1;
                                break;
                              case 'f':
                                if (stack)
                                    va_arg(ap,double *);
                                else
                                    *(double **)from += 1;
                                break;
                              case 's':
                              case 't':
                                if (stack)
                                    va_arg(ap,char *);
                                else
                                    *from += len + 1;
                                break;
                              case '#':
                                if (stack)
                                    va_arg(ap,uint8_t *);
                                else
                                    *from += 4;
                                break;
                              case '$':
                                if (stack)
                                    va_arg(ap,uint8_t *);
                                else
                                    *from += 2;
                                break;
                            }
                        }
                        f++;
                    }
                }
            }
            else
            {
                if (*f == '>') // If no parameters to keyword, treat it as a boolean
                {
                    if (stack)
                        *va_arg(ap,bool *) = (bool)true;
                    else
                    {
                        **(bool **)from = (bool)true;
                        *(bool **)from += 1;
                    }
                    f++;
                }

                t.WhiteGet();
            }

            break;
          }

        // End Optional
          case '>':
          {
            f++; // Ignore end optional
            break;
          }

        // Value
          case '%':
          {
            f++;
            // REVSYNC: 0x00479d89 -- `%\` before the conversion: the tokens
            // read while the field is taken (the one after it) keep their
            // backslashes as written; the token's setting comes back after.
            const bool literal = (*f == '\\');
            const bool wasliteral = t.literalbackslash;
            if (literal)
            {
                f++;
                t.literalbackslash = true;
            }
            if (isdigit(*f))
            {
                len = 0;
                while (isdigit(*f))
                    len = len * 10 + *f++ - '0';
            } else len = 0;

            char *str;
            switch (*f)
            {

            // Number
              case 'b':
              case 'h':
              case 'w':
              case 'i':
              case 'd':
              case 'f':
              {
                if (t.Type() != TKN_NUMBER && t.Type() != TKN_DEFINE && t.Type() != TKN_SCENEID)
                    return false;
                double n = t.Number();
                t.WhiteGet();

                // Cast double->uint32_t losslessly across the full range
                // we care about. (uint32_t)(double) is UB for negatives,
                // so route negatives through int32_t first; positives can
                // exceed INT32_MAX (e.g. 0xFFFFFFFF flag masks) so leave
                // those on the unsigned path. The bit patterns join up at
                // the int32_t/uint32_t output cast below.
                auto to_u32 = [](double v) -> uint32_t {
                    return v < 0.0 ? (uint32_t)(int32_t)v : (uint32_t)v;
                };
                uint32_t d;
                if (*f != 'f')
                {
                    d = to_u32(n);
                    while (t.Type() == TKN_SYMBOL && (t.Text())[0] == '|')
                    {
                        t.WhiteGet();
                        if (t.Type() != TKN_NUMBER && t.Type() != TKN_DEFINE && t.Type() != TKN_SCENEID)
                            return false;
                        d |= to_u32(t.Number());
                        t.WhiteGet();
                    }
                }

                switch (*f)
                {
                  case 'c':
                    if (stack)
                        *va_arg(ap,signed char *) = (signed char)d;
                    else
                    {
                        **(signed char **)from = (signed char)d;
                        *(signed char **)from += 1;
                    }
                    break;
                  case 'b':
                    if (stack)
                        *va_arg(ap,unsigned char *) = (unsigned char)d;
                    else
                    {
                        **(unsigned char **)from = (unsigned char)d;
                        *(unsigned char **)from += 1;
                    }
                    break;
                  case 'h':
                    if (stack)
                        *va_arg(ap,short *) = (short)d;
                    else
                    {
                        **(short **)from = (short)d;
                        *(short **)from += 1;
                    }
                    break;
                  case 'w':
                    if (stack)
                        *va_arg(ap,uint16_t *) = (uint16_t)d;
                    else
                    {
                        **(uint16_t **)from = (uint16_t)d;
                        *(uint16_t **)from += 1;
                    }
                    break;
                  case 'i':
                    if (stack)
                        *va_arg(ap,int32_t *) = (int32_t)d;
                    else
                    {
                        **(int32_t **)from = (int32_t)d;
                        *(int32_t **)from += 1;
                    }
                    break;
                  case 'd':
                    if (stack)
                        *va_arg(ap,uint32_t *) = (uint32_t)d;
                    else
                    {
                        **(uint32_t **)from = (uint32_t)d;
                        *(uint32_t **)from += 1;
                    }
                    break;
                  case 'f':
                    if (stack)
                        *va_arg(ap,double *) = n;
                    else
                    {
                        **(double **)from = n;
                        *(double **)from += 1;
                    }
                    break;
                }
                f++;
                break;
              }

              // Text
              case 't':
              {
                f++;
                if (t.Type() != TKN_IDENT && t.Type() != TKN_DEFINE && t.Type() != TKN_SCENEID)
                    return false;
                if (stack)
                    str = va_arg(ap,char *);
                else
                    str = (*from);
                if (len)
                {
                    memset(str, 0, len);
                    strncpy(str, t.Text(), len - 1);
                }
                else
                {
                    strcpy(str, t.Text());
                    len = strlen(t.Text()) + 1;
                }
                if (!stack) (*from) += len;
                t.WhiteGet();
                break;
              }

              // String Constant
              case 's':
              {
                f++;
                if (t.Type() != TKN_TEXT)
                    return false;
                if (stack)
                    str = va_arg(ap,char *);
                else
                    str = (*from);
                if (len)
                {
                    memset(str, 0, len);
                    strncpy(str, t.Text(), len - 1);
                }
                else
                {
                    strcpy(str, t.Text());
                    len = strlen(t.Text()) + 1;
                }
                if (!stack) (*from) += len;
                t.WhiteGet();
                break;
              }

              default:
                Error("Invalid '%' field in GetLine()");
                break;

            } // End of '%' value parse
            if (literal)
                t.literalbackslash = wasliteral;
            break;
          }

        // Keyword
          default:
            if (isalpha(*f))
            {
                i = 0;
                while (*f && isalnum(*f))
                    keyword[i++] = toupper(*f++);
                keyword[i] = 0;
                if ((t.Type() != TKN_KEYWORD && t.Type() != TKN_IDENT)
                  || stricmp(keyword, t.Text()))
                    return false;
                t.WhiteGet();
            }
            else // Default
            {
                if (t.Type() != TKN_SYMBOL || t.Code() != *f)
                {
                    return false;
                }
                f++;
                t.WhiteGet();
            }
            break;
        }
    }

    return true;
}

bool Parse(TToken &t, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    bool retval = ParseAnything(true, t, format, ap);
    va_end(ap);
    return retval;
}

bool ParseString(const char *string, const char *format, ...)
{
    TStringParseStream s((char*)string);
    TToken t(s);
    t.WhiteGet();   // prime first non-whitespace token (real Parse() callers
                    // already advance the stream past the tag before calling)

    va_list ap;
    va_start(ap, format);
    bool retval = ParseAnything(true, t, format, ap);
    va_end(ap);
    return retval;
}
    
