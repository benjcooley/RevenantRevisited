// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   dialog.h - TDialogPane module                       *
// *************************************************************************

#ifndef _DIALOG_H
#define _DIALOG_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _SCREEN_H
#include "screen.h"
#endif

#include <string>
#include <unordered_map>
#include <vector>

// ****************************************************************************
// * TDialogList - Stores language specific dialog and message lines for game *
// ****************************************************************************

// REVSYNC: TDialogList @ 0x0065d4d0 (DIALOG.md §3.6). Two tables of
// {tag, line}, each sorted by upper-cased tag: the game-wide base table
// (<ClassDefPath><Language>.def: UI text, item names, messages), loaded at
// boot, and the module's (its dialog), loaded when the module is mounted.
// Ids run through the base table, then the module's. Lookups never fail: a
// miss answers "[TAG]" or "[badid]", as retail's do.
class TDialogList
{
  public:
    // REVSYNC: 0x0049ceb0 -- the base table. Idempotent.
    bool Initialize();
    // REVSYNC: 0x0049d2a0 -- the active module's table: <Language>_dialog.def,
    // else <Language>.def, else english.def. Replaces any earlier module's.
    bool LoadModule();
    void Close();

    // REVSYNC: 0x0049d6d0 -- the id of `tag`, base table first; -1 if absent.
    [[nodiscard]] int32_t FindLine(const char *tag) const;
    // REVSYNC: 0x0049d780 / 0x0049d7c0 -- "[badid]" out of range.
    [[nodiscard]] const char *GetLine(int32_t id) const;
    [[nodiscard]] const char *GetTag(int32_t id) const;
    // REVSYNC: 0x0049d800 -- "[TAG]" when absent.
    [[nodiscard]] const char *GetLine(const char *tag) const;

  private:
    struct SLine
    {
        std::string tag;            // upper case
        std::string line;
    };
    using TTable = std::vector<SLine>;

    static bool LoadTable(const char *path, TTable &table);
    static int32_t Find(const TTable &table, const std::string &tag);

    TTable base;
    TTable module;
    bool initialized = false;
    mutable std::unordered_map<std::string, std::string> misses;    // "[TAG]" answers
};

// ************************************************************
// * TDialogPane - Shows the dialog options for the character *
// ************************************************************

#define MAXCHOICES      4

// Global function for translating dialog lines

void SetDialogContext(TObjectInstance* context); // The script context
char *DialogLine(const char *line, char *buf, int32_t buflen);

// Dialog pane, for interacting with NPCs in conversation

_CLASSDEF(TDialogPane)
class TDialogPane : public TPane
{
  public:
    TDialogPane() : TPane(0, INVENTORYPANEY - 6, 404, 97, true) { character = nullptr; }
    
    virtual bool Initialize();
    virtual void Close();
    virtual void Show();
    virtual void Hide();
    virtual void DrawBackground();
    virtual void Animate(bool draw);
    virtual void KeyPress(int32_t key, bool down);
    virtual void MouseClick(int32_t button, int32_t x, int32_t y);
    virtual void MouseMove(int32_t button, int32_t x, int32_t y);

    void SetCharacter(PTCharacter inst) { character = inst; }
        // Set the character that appears in the pane
    PTCharacter GetCharacter() { return character; }
        // Get the current character
    void AddChoice(char *lab, char *tag);
        // Add a dialog choice for the player to choose
    void SetChoice(int32_t c) { choice = c; freshresponse = true; highlighted = true; SetDirty(true); }
        // Set a choice as selected
    void Skip();
        // Skip current say command

    bool HasResponded() { return freshresponse; }
    char *GetResponseLabel() { if (choice >= 0) return label[choice]; else return nullptr; }
    char *GetResponse() { if (choice >= 0) return choices[choice]; else return nullptr; }
    void ResetResponses();

  protected:
    int32_t OnSlot(int32_t x, int32_t y);
        // Find which dialog choice x, y is over

    TMulti* dialogdata;             // Background and misc
    PTCharacter character;          // Character doing the blabing

    char *choices[MAXCHOICES];      // Choice text
    char *label[MAXCHOICES];        // Label for each choice
    int32_t numchoices;                 // Number of current choices
    int32_t choice;                     // Their choice - update next frame
    bool freshresponse;             // Whether response has been handled yet

    int32_t grabslot;                   // Slot they clicked on
    bool highlighted;               // For mouse stuff
};

#endif