// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           editorundo.h - Dirty-tracking undo / redo                   *
// *************************************************************************
//
// Two flavours of undo coexist on one stack:
//
//   (1) Dirty-tracked snapshot diffs (default for property edits + gizmo
//       transforms).  The editor calls EditorUndo::MarkDirty(node) before
//       any edit -- idempotent.  On the first MarkDirty for a node we
//       snapshot all of its annotated properties (refl.h).  On Resolve
//       (end of frame, or mouse-up for an actively-locked drag) we snapshot
//       again, diff against the baseline, and push one undo entry per
//       changed node that captures (before -> after) for the changed
//       properties only.  Edit code never has to construct undo data;
//       it just toggles the dirty flag and writes.
//
//   (2) Explicit IEditorCommand entries for things that aren't a simple
//       property mutation -- create/delete/paste, structure changes,
//       command-pattern actions.  Pushed via Push(); they own their own
//       Undo/Redo logic.
//
// Snapshots hold a soft ref to the node, so undoing an edit on a node
// that has since been deleted is a no-op rather than a dangling write.
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

class TNode;

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

// Mark `node` dirty. Idempotent: the first call this transaction snapshots
// the node's annotated properties; subsequent calls do nothing. Edit code
// calls this BEFORE writing.
void MarkDirty(TNode* node);

// Drop `node` from the dirty set without producing an undo entry.
void DropDirty(TNode* node);

// Take the current snapshot of every dirty node, diff vs its baseline,
// and push one undo entry per node that actually changed. Clears the
// dirty set. No-op while the resolver is locked.
void ResolveDirty();

// Lock/unlock the resolver. Lock counter is a counted semaphore so
// nested locks (e.g. property drag while gizmo drag) compose cleanly.
void LockResolver();
void UnlockResolver();
bool ResolverLocked();

// Optional operation label, surfaced in the Edit > Undo / Redo menu
// items ("Undo move", "Undo position", ...). The next ResolveDirty
// stamps every produced undo entry with this name and clears it.
void SetOperationName(const char* name);

// ---- Explicit command-pattern entries ------------------------------------

// Push a custom command and execute its Redo. Drops any redo-stack
// entries. Resolves any pending dirty state first so the explicit command
// lands AFTER any in-flight edits.
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
