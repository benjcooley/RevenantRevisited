// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               parse.h - Low-level file parsing functions              *
// *************************************************************************

#pragma once

#include "revenant.h"

#include <stdio.h>

// *******************************************
// * TParseStream - Parser stream base class *
// *******************************************

#define ENDOFSTREAM -1

_CLASSDEF(TParseStream)
class TParseStream
{
  public:
    virtual const char *Name() const { return ""; }
    virtual int32_t GetChar() = 0;
    virtual void Reset() = 0;
    virtual uint32_t GetPos() const = 0;
    virtual void SetPos(uint32_t newpos) = 0;
};

// ********************************************************
// * TStringParseStream - ASCII string parse stream class *
// ********************************************************

_CLASSDEF(TStringParseStream)
class TStringParseStream : public TParseStream
{
  public:
    TStringParseStream(char *buffer, int32_t len = 0x7FFFFFFF)
        { buf = buffer; end = buf + len; ptr = buffer; }
    virtual const char *Name() const { return "String"; }
    virtual int32_t GetChar() { if (!*ptr || ptr == end) return ENDOFSTREAM; else return *ptr++; }
    virtual void Reset() { ptr = buf; }
    virtual uint32_t GetPos() const { return (uint32_t)(ptr - buf); }
    virtual void SetPos(uint32_t newpos) { ptr = (char *)(buf + newpos); }

  private:

    char *buf;
    char *end;
    char *ptr;
};

// **********************************************
// * TFileParseStream - FILE parse stream class *
// **********************************************

_CLASSDEF(TFileParseStream)
class TFileParseStream : public TParseStream
{
  public:
    TFileParseStream(FILE *f, const char *n = "File") { file = f; name = n; }
    virtual const char *Name() const { return name; }
    virtual int32_t GetChar()
        { int32_t ch = getc(file); if (ch == EOF) return ENDOFSTREAM; else return ch; }
    virtual void Reset() { fseek(file, 0, SEEK_SET); }
    virtual uint32_t GetPos() const { return (uint32_t)ftell(file); }
    virtual void SetPos(uint32_t newpos) { fseek(file, newpos, SEEK_SET); }

  private:

    FILE *file;
    const char *name;
};

// *****************************
// * TIdent - Itentifier class *
// *****************************

struct Ident
{
    char ident[MAXIDENTLEN];
    int32_t type;
    int32_t value;
};

#define ISDEFINE -1
#define MAXIDENTS 128

_CLASSDEF(TIdent)
class TIdent
{
  public:
    TIdent();
    ~TIdent();
    int32_t NumIdents() const { return numidents; }
    Ident *Find(const char *text, int32_t type);
    Ident *Find(const char *text) { return Find(text, ISDEFINE); }
    int32_t FindIndex(const char *text, int32_t type);
    int32_t FindIndex(const char *text) { return FindIndex(text, ISDEFINE); }
    Ident *Get(int32_t index) const { return &(idents[index]); }
    int32_t Add(const char *ident, int32_t type, int32_t value);
    int32_t Add(const char *ident, int32_t value) { return Add(ident, ISDEFINE, value); }

  private:
    int32_t numidents;
    Ident *idents;
};

// ************************
// * TToken - Token class *
// ************************

#define TKN_ERROR       0
#define TKN_WHITESPACE  1
#define TKN_TEXT        2
#define TKN_KEYWORD     3
#define TKN_IDENT       4
#define TKN_DEFINE      5
#define TKN_SCENEID     6
#define TKN_SYMBOL      7
#define TKN_NUMBER      8
#define TKN_RETURN      9
#define TKN_EOF         10

#define KEY_BEGIN       0
#define KEY_END         1
#define KEY_DEFINE      2
#define KEY_INCLUDE     3

#define MAXTOKENTEXT    8192

class TToken
{
  public:
    TToken() { stream = nullptr; type = TKN_ERROR;
      index = 0; code = 0; number = 0; text[0] = 0; lastch = 0; linenum = 1; }
    TToken(TParseStream &s) { stream = &s; type = TKN_ERROR;
      index = 0; code = 0; number = 0; text[0] = 0; lastch = 0; linenum = 1; }
    
    void SetStream(TParseStream &s) { stream = &s; }

    void Get();         // Gets next token
    void WhiteGet();    // Gets next non-whitespace token
    void LineGet();     // Gets next non-newline/whitespace token
    bool DefineGet();   // Gets all whitespace/newline/and #define lines up to next token
                        // for each define, creates a new identifier in idents list.  Returns
                        // false if syntax error in #define or too many defines.

    void SkipLine();    // Skips a line
    void SkipBlanks();  // Skips whitespace and returns
    bool SkipBlock();   // Skips a BEGIN/END block (returns false if EOF before END)

    int32_t Type() const { return type; }
    int32_t Index() const { return index; }
    int32_t Code() const { return code; }
    const char *Text() const { return text; }
    double Number() const { return number; }

    bool Is(char *istext, int32_t abbrevlen = 0) const;
    bool IsBegin() const { return type == TKN_KEYWORD && code == KEY_BEGIN; }
    bool IsEnd() const { return type == TKN_KEYWORD && code == KEY_END; }
    void DoBegin();                 // Call to compile BEGIN
    void DoEnd();                   // Call to compile END
    int32_t LineNum() const { return linenum; }

    uint32_t GetPos() const { return stream->GetPos(); }
    void SetPos(uint32_t pos) { stream->SetPos(pos); }

    void Error(const char *err, const char *extra = nullptr);
      // Fatal error at line number

  private:
    PTParseStream stream;
    int32_t type;
    int32_t index;
    int32_t code;
    double number;
    char text[MAXTOKENTEXT];
    char lastch;
    int32_t linenum;
    TIdent idents;
};

// ***************************************************
// * TScriptObject - Script parsing and interpreting *
// ***************************************************

#define SCRIPT_ERROR    0       // Script has encountered an error
#define SCRIPT_DONE     1       // Script is finished executing/parsing
#define SCRIPT_WAITING  2       // Script is waiting, call Parse() again next frame

class TScriptObject
{
  public:
    TScriptObject(TParseStream &stream) { t.SetStream(stream); }

    virtual void Reset() { s->Reset(); }        // Redefine to reset script state
    virtual int32_t Parse();                    // Redefine to implement script processing

  protected:
    PTParseStream s;
    TToken t;
};

// *********************
// * Parsing Functions *
// *********************

int32_t abbrevcmp(const char *abbrev, const char *string);

bool Parse(TToken &t, const char *format, ...);
bool ParseString(const char *string, const char *format, ...);

