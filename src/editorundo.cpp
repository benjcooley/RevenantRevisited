// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           editorundo.cpp - Dirty-tracking undo / redo                 *
// *************************************************************************

#include "editorundo.h"

#include "iobject.h"

#include <unordered_map>
#include <utility>
#include <vector>

namespace {

class LambdaCommand : public IEditorCommand
{
public:
    LambdaCommand(const char* name,
                  std::function<void()> redo,
                  std::function<void()> undo)
        : name_(name ? name : "edit"),
          redo_(std::move(redo)), undo_(std::move(undo)) {}
    void Undo() override { if (undo_) undo_(); }
    void Redo() override { if (redo_) redo_(); }
    const char* Name() const override { return name_.c_str(); }
private:
    std::string           name_;
    std::function<void()> redo_;
    std::function<void()> undo_;
};

// "Doc tree" of a single object, for the diff-based undo path. We use
// IReflection's GetValue/SetValue as the serialization API: walking all
// properties produces a flat vector of (name, value) pairs, and applying
// a vector calls SetValue for each.
//
// TODO(deep-undo): if we ever need to capture nested state -- sectors
// with their child arrays, instance state with sub-objects, animator
// keyframes, anything that doesn't decompose into a flat property list
// -- swap SObjectSnapshot for a defdoc::Node tree. The public API
// (MarkDirty / Lock / Resolve / SetOperationName) stays the same; only
// SnapshotOf / ApplyOnto / Diff need to grow into a recursive tree walk
// (path-keyed match, array insert/delete detection, etc.). Probably
// unnecessary for this game, but flagged here so future-us doesn't
// re-derive the architecture.
using SObjectSnapshot = std::vector<std::pair<const char*, SPropertyValue>>;

SObjectSnapshot SnapshotOf(IObject* obj)
{
    SObjectSnapshot snap;
    if (!obj) return snap;
    const IReflection& r = obj->GetRefl();
    const int32_t n = r.NumProperties();
    snap.reserve(size_t(n));
    for (int32_t i = 0; i < n; ++i) {
        const SPropertyDesc& p = r.GetProperty(i);
        if (HasFlag(p.flags, EPropertyFlag::ReadOnly)) continue;
        SPropertyValue v;
        r.GetValue(obj, p, v);
        snap.emplace_back(p.name, std::move(v));
    }
    return snap;
}

void ApplyOnto(IObject* obj, const SObjectSnapshot& snap)
{
    if (!obj) return;
    const IReflection& r = obj->GetRefl();
    for (const auto& kv : snap) {
        if (const SPropertyDesc* p = r.FindProperty(kv.first))
            r.SetValue(obj, *p, kv.second);
    }
}

// Apply only the entries in `snap` whose names appear in `keys` -- used
// to push targeted before/after pairs that touch only the diffed props.
SObjectSnapshot Diff(const SObjectSnapshot& before, const SObjectSnapshot& after)
{
    // before and after are produced by the same reflection schema, so the
    // entries are positional. Iterate and keep only differing slots.
    SObjectSnapshot d;
    const size_t n = (before.size() < after.size()) ? before.size() : after.size();
    for (size_t i = 0; i < n; ++i) {
        if (!(before[i].second == after[i].second))
            d.emplace_back(after[i].first, after[i].second);
    }
    return d;
}

// A single auto-generated undo entry covering EVERY object that changed
// during the frame / lock period. Undo applies the before-snapshot to
// each; Redo applies the after-snapshot. One entry, one menu label, no
// matter how many objects the command touched.
class BatchSnapshotCommand : public IEditorCommand
{
public:
    struct SEntry { IObject* obj; SObjectSnapshot before; SObjectSnapshot after; };
    BatchSnapshotCommand(std::string name, std::vector<SEntry> entries)
        : name_(std::move(name)), entries_(std::move(entries)) {}
    void Undo() override { for (auto& e : entries_) ApplyOnto(e.obj, e.before); }
    void Redo() override { for (auto& e : entries_) ApplyOnto(e.obj, e.after);  }
    const char* Name() const override { return name_.c_str(); }
private:
    std::string         name_;
    std::vector<SEntry> entries_;
};

struct SDirtyEntry { IObject* obj; SObjectSnapshot before; };

std::vector<std::unique_ptr<IEditorCommand>>& UndoStack()
{
    static std::vector<std::unique_ptr<IEditorCommand>> s;
    return s;
}
std::vector<std::unique_ptr<IEditorCommand>>& RedoStack()
{
    static std::vector<std::unique_ptr<IEditorCommand>> s;
    return s;
}

std::unordered_map<IObject*, SDirtyEntry>& DirtySet()
{
    static std::unordered_map<IObject*, SDirtyEntry> s;
    return s;
}

int32_t& LockCount()
{
    static int32_t s = 0;
    return s;
}

std::string& PendingName()
{
    static std::string s;
    return s;
}

} // namespace

// ---- Dirty-tracking ------------------------------------------------------

void EditorUndo::MarkDirty(IObject* obj)
{
    if (!obj) return;
    auto& d = DirtySet();
    if (d.find(obj) != d.end()) return;     // idempotent
    d.emplace(obj, SDirtyEntry{ obj, SnapshotOf(obj) });
}

void EditorUndo::DropDirty(IObject* obj)
{
    DirtySet().erase(obj);
}

void EditorUndo::ResolveDirty()
{
    if (LockCount() > 0) return;
    auto& d = DirtySet();
    if (d.empty()) return;

    std::vector<BatchSnapshotCommand::SEntry> changed;
    changed.reserve(d.size());
    for (auto& [obj, entry] : d) {
        SObjectSnapshot now   = SnapshotOf(obj);
        SObjectSnapshot after = Diff(entry.before, now);
        if (after.empty()) continue;
        // Pair each "after" key with its corresponding "before" value
        // so undo flips back exactly what we changed.
        SObjectSnapshot before_sub;
        before_sub.reserve(after.size());
        for (const auto& a : after) {
            for (const auto& b : entry.before)
                if (a.first == b.first) {
                    before_sub.emplace_back(b.first, b.second);
                    break;
                }
        }
        changed.push_back({ obj, std::move(before_sub), std::move(after) });
    }
    d.clear();

    if (changed.empty()) return;

    // One batch entry per Resolve. Name comes from the operation label
    // set during this transaction (gizmo "move" / property "scale" /
    // command name) and is consumed here.
    std::string name = std::move(PendingName());
    if (name.empty()) name = "edit";
    UndoStack().push_back(std::make_unique<BatchSnapshotCommand>(
        std::move(name), std::move(changed)));
    RedoStack().clear();
}

void EditorUndo::SetOperationName(const char* name)
{
    PendingName() = name ? name : "";
}

void EditorUndo::LockResolver()  { ++LockCount(); }
void EditorUndo::UnlockResolver()
{
    // Only decrement the lock; do NOT resolve inline. The end-of-frame
    // ResolveDirty() call (in EditorDrawChrome's chrome) picks it up.
    // That keeps the diff happening at a stable point -- after every
    // system has finished mutating the dirty objects this frame -- and
    // matches the user-facing rule "drag, release, undo entry appears".
    if (LockCount() > 0) --LockCount();
}
bool EditorUndo::ResolverLocked() { return LockCount() > 0; }

// ---- Explicit commands ---------------------------------------------------

void EditorUndo::Push(std::unique_ptr<IEditorCommand> cmd)
{
    if (!cmd) return;
    ResolveDirty();             // flush any pending property/transform diffs first
    cmd->Redo();
    UndoStack().push_back(std::move(cmd));
    RedoStack().clear();
}

bool EditorUndo::CanUndo() { return !UndoStack().empty(); }
bool EditorUndo::CanRedo() { return !RedoStack().empty(); }

void EditorUndo::Undo()
{
    auto& u = UndoStack();
    if (u.empty()) return;
    auto cmd = std::move(u.back());
    u.pop_back();
    cmd->Undo();
    RedoStack().push_back(std::move(cmd));
}

void EditorUndo::Redo()
{
    auto& r = RedoStack();
    if (r.empty()) return;
    auto cmd = std::move(r.back());
    r.pop_back();
    cmd->Redo();
    UndoStack().push_back(std::move(cmd));
}

void EditorUndo::Clear()
{
    UndoStack().clear();
    RedoStack().clear();
}

std::unique_ptr<IEditorCommand> EditorUndo::MakeLambda(
    const char* name,
    std::function<void()> do_fn,
    std::function<void()> undo_fn)
{
    return std::make_unique<LambdaCommand>(name, std::move(do_fn), std::move(undo_fn));
}

const char* EditorUndo::TopUndoName()
{
    return UndoStack().empty() ? "" : UndoStack().back()->Name();
}

const char* EditorUndo::TopRedoName()
{
    return RedoStack().empty() ? "" : RedoStack().back()->Name();
}
