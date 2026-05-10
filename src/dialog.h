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

// ****************************************************************************
// * TDialogList - Stores language specific dialog and message lines for game *
// ****************************************************************************

_STRUCTDEF(SDialogLine)
struct SDialogLine
{
    char *tag;
    char *line;
    SDialogLine() { tag = line = nullptr; }
    ~SDialogLine() { if (tag) free(tag); if (line) free(line); }
    void Set(char *t, char *l) { tag = strdup(t); line = strdup(l); }
};
typedef TPointerArray<SDialogLine, 64, 64> TDialogLineArray;

class TDialogList
{
  public:
    bool Initialize();
      // Loads lines from LANGUAGE.DEF file (i.e. ENGLISH.DEF for english).
      // Idempotent.
    void Close();
      // Closes dialog file. Idempotent — leaves `lines` empty so the
      // trivial default dtor only walks zeroed state.
    int32_t FindLine(char *tag);
      // Finds the dialog line for the given tag and returns id
    char *GetLine(int32_t id);
      // Finds the dialog line for the given id and returns it
    char *GetTag(int32_t id);
      // Finds the dialog line for the given id and returns it
    char *GetLine(char *tag);
      // Finds the dialog line for the given tag and returns it

  private:
    TDialogLineArray lines;         // Dialog line list
    bool initialized = false;
};

// ************************************************************
// * TDialogPane - Shows the dialog options for the character *
// ************************************************************

#define MAXCHOICES      4

// Global function for translating dialog lines

void SetDialogContext(TObjectInstance* context); // The script context
char *DialogLine(char *line, char *buf, int32_t buflen);

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