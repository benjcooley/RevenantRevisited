// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           editorundo.h - Dirty-tracking undo / redo                   *
// *************************************************************************
//
// Two flavours of undo coexist on one stack:
//
//   (1) Dirty-tracked snapshot diffs (default for property edits + gizmo
//       transforms).  The editor calls EditorUndo::MarkDirty(obj) before
//       any edit -- idempotent.  On the first MarkDirty for an object we
//       snapshot all of its reflected properties.  On Resolve (end of
//       frame, or mouse-up for an actively-locked drag) we snapshot
//       again, diff against the baseline, and push one undo entry per
//       changed object that captures (before -> after) for the changed
//       properties only.  Edit code never has to construct undo data;
//       it just toggles the dirty flag and writes.
//
//   (2) Explicit IEditorCommand entries for things that aren't a simple
//       property mutation -- create/delete/paste, structure changes,
//       command-pattern actions.  Pushed via Push(); they own their own
//       Undo/Redo logic.
//
// During a drag (mouse-down through mouse-up) the editor calls
// LockResolver()/UnlockResolver() so end-of-frame Resolve calls turn into
// no-ops; the actual diff fires on Unlock when the drag ends, producing
// a single undo entry instead of one per pixel of motion.
//
// *************************************************************************

#pragma once

#include <functional>
#include <memory>
#include <string>

class IObject;

class IEditorCommand
{
public:
    virtual ~IEditorCommand() = default;
    virtual void Undo() = 0;
    virtual void Redo() = 0;
    virtual const char* Name() const { return "edit"; }
};

namespace EditorUndo {

// ---- Dirty-tracking snapshot diffs ---------------------------------------

// Mark `obj` dirty. Idempotent: the first call this transaction snapshots
// the object's reflected properties; subsequent calls do nothing. Edit
// code calls this BEFORE writing.
void MarkDirty(IObject* obj);

// Drop `obj` from the dirty set without producing an undo entry. Use for
// the rare case where you marked something dirty speculatively.
void DropDirty(IObject* obj);

// Take the current snapshot of every dirty object, diff vs its baseline,
// and push one undo entry per object that actually changed. Clears the
// dirty set. No-op while the resolver is locked.
void ResolveDirty();

// Lock/unlock the resolver. Lock counter is a counted semaphore so
// nested locks (e.g. property drag while gizmo drag) compose cleanly.
// When the counter returns to zero, ResolveDirty fires automatically.
void LockResolver();
void UnlockResolver();
bool ResolverLocked();

// Optional operation label, surfaced in the Edit > Undo / Redo menu
// items ("Undo move", "Undo position", ...). The next ResolveDirty
// stamps every produced undo entry with this name and clears it.
// Falls back to "edit" if unset.
void SetOperationName(const char* name);

// ---- Explicit command-pattern entries ------------------------------------

// Push a custom command and execute its Redo. Drops any redo-stack
// entries (the new edit branches off the previous undo state). Resolves
// any pending dirty state first so the explicit command lands AFTER any
// in-flight edits.
void Push(std::unique_ptr<IEditorCommand> cmd);

// One-line helper for ad-hoc commands. `do_fn` is invoked once at Push
// time and again on Redo; `undo_fn` is invoked on Undo.
std::unique_ptr<IEditorCommand> MakeLambda(
    const char* name,
    std::function<void()> do_fn,
    std::function<void()> undo_fn);

// ---- Standard undo / redo -----------------------------------------------

bool CanUndo();
bool CanRedo();
void Undo();
void Redo();
void Clear();

// Description of the next undo/redo action (for menu labels).
const char* TopUndoName();
const char* TopRedoName();

} // namespace EditorUndo
