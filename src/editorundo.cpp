// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           editorundo.cpp - Dirty-tracking undo / redo                 *
// *************************************************************************

#include "editorundo.h"

#include "node.h"
#include "component.h"
#include "refl.h"

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

// One captured property. SPropValue carries a string *view*, so the
// snapshot owns the characters in `text` and re-points the view before
// use (the vector may move the entry).
struct SSnapEntry
{
    int32_t     slot = -1;          // -1: the node itself; else a component slot
    uint16_t    id = 0;
    SPropValue  value;
    std::string text;

    [[nodiscard]] SPropValue Value() const
    {
        SPropValue v = value;
        if (v.type == EPropType::String) v.str = text;
        return v;
    }
};

using SObjectSnapshot = std::vector<SSnapEntry>;

// The thing a snapshot entry addresses: the node, or one of its
// components (the map object among them), with its reflected type.
void* TargetOf(TNode* node, int32_t slot, const refl::SType*& type)
{
    if (!node) { type = nullptr; return nullptr; }
    if (slot < 0) { type = refl::Of(*node); return node; }
    TComponent* c = node->GetComponent(slot);
    type = c ? refl::Of(*c) : nullptr;
    return c;
}

void SnapshotTarget(SObjectSnapshot& snap, int32_t slot, const refl::SType* type, const void* obj)
{
    if (!type || !obj) return;
    for (const refl::SType* t = type; t; t = t->base)
    {
        for (int32_t i = 0; i < t->prop_count; ++i)
        {
            const refl::SProp& p = t->props[i];
            if (p.read_only || p.type == EPropType::Trigger) continue;
            SSnapEntry e;
            e.slot = slot;
            e.id = p.id;
            if (!refl::Get(*type, obj, p.id, e.value)) continue;
            if (e.value.type == EPropType::String) e.text = std::string(e.value.str);
            snap.push_back(std::move(e));
        }
    }
}

// The node's own properties and every component's.
SObjectSnapshot SnapshotOf(TNode* node)
{
    SObjectSnapshot snap;
    if (!node) return snap;
    for (int32_t slot = -1; slot < node->NumComponents(); ++slot)
    {
        const refl::SType* type = nullptr;
        void* obj = TargetOf(node, slot, type);
        SnapshotTarget(snap, slot, type, obj);
    }
    return snap;
}

void ApplyOnto(TNode* node, const SObjectSnapshot& snap)
{
    if (!node) return;
    for (const SSnapEntry& e : snap)
    {
        const refl::SType* type = nullptr;
        void* obj = TargetOf(node, e.slot, type);
        if (type && obj)
            refl::Set(*type, obj, e.id, e.Value());
    }
}

// Entries of `after` whose value differs from the same (slot, id) in `before`.
SObjectSnapshot Diff(const SObjectSnapshot& before, const SObjectSnapshot& after)
{
    SObjectSnapshot d;
    for (const SSnapEntry& a : after)
    {
        bool changed = true;
        for (const SSnapEntry& b : before)
            if (b.slot == a.slot && b.id == a.id) { changed = !refl::Equals(a.Value(), b.Value()); break; }
        if (changed) d.push_back(a);
    }
    return d;
}

// A single auto-generated undo entry covering EVERY node that changed
// during the frame / lock period. Undo applies the before-snapshot to
// each; Redo applies the after-snapshot. One entry, one menu label.
class BatchSnapshotCommand : public IEditorCommand
{
public:
    struct SEntry
    {
        TSafeRef<TNode> node;
        SObjectSnapshot     before;
        SObjectSnapshot     after;
    };
    BatchSnapshotCommand(std::string name, std::vector<SEntry> entries)
        : name_(std::move(name)), entries_(std::move(entries)) {}
    void Undo() override { for (auto& e : entries_) ApplyOnto(e.node.Get(), e.before); }
    void Redo() override { for (auto& e : entries_) ApplyOnto(e.node.Get(), e.after);  }
    const char* Name() const override { return name_.c_str(); }
private:
    std::string         name_;
    std::vector<SEntry> entries_;
};

struct SDirtyEntry
{
    TSafeRef<TNode> node;
    SObjectSnapshot     before;
};

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

std::unordered_map<TNode*, SDirtyEntry>& DirtySet()
{
    static std::unordered_map<TNode*, SDirtyEntry> s;
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

void EditorUndo::MarkDirty(TNode* node)
{
    if (!node) return;
    auto& d = DirtySet();
    if (d.find(node) != d.end()) return;     // idempotent
    d.emplace(node, SDirtyEntry{ TSafeRef<TNode>(node), SnapshotOf(node) });
}

void EditorUndo::DropDirty(TNode* node)
{
    DirtySet().erase(node);
}

void EditorUndo::ResolveDirty()
{
    if (LockCount() > 0) return;
    auto& d = DirtySet();
    if (d.empty()) return;

    std::vector<BatchSnapshotCommand::SEntry> changed;
    changed.reserve(d.size());
    for (auto& [key, entry] : d)
    {
        TNode* node = entry.node.Get();
        if (!node) continue;                      // deleted mid-transaction
        SObjectSnapshot now   = SnapshotOf(node);
        SObjectSnapshot after = Diff(entry.before, now);
        if (after.empty()) continue;
        // Pair each "after" entry with its "before" value so undo flips back
        // exactly what we changed.
        SObjectSnapshot before_sub;
        before_sub.reserve(after.size());
        for (const SSnapEntry& a : after)
            for (const SSnapEntry& b : entry.before)
                if (a.slot == b.slot && a.id == b.id) { before_sub.push_back(b); break; }
        changed.push_back({ entry.node, std::move(before_sub), std::move(after) });
    }
    d.clear();

    if (changed.empty()) return;

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
    // ResolveDirty() call picks it up so the diff happens at a stable
    // point, after every system has finished mutating the dirty nodes.
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
