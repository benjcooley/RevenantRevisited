// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           transform.h - SafeRef-able hierarchical transform           *
// *************************************************************************
//
// TTransform is a SafeRef-able transform node that can form a parent /
// children hierarchy. It is owned by some other object (TObjectInstance,
// skeleton bone, scene-graph node, ...) -- TTransform itself doesn't
// model ownership, just spatial relationship.
//
// Coordinate convention: row-vector (matches src/math3d.h). Local TRS
// matrix builds as Scale * Rotation * Translation, applied to a row
// vector v as v' = v * S * R * T. World matrix is local * parent.world.
//
// Lazy evaluation: pos / rot / scl are stored as their primitive form;
// the local 4x4 and world 4x4 are caches recomputed on demand. Setters
// compare-before-set: writing the same value back is a no-op (no dirty
// flags raised). Mutations propagate two flags through the tree:
//
//   F_GLOBAL_MATRIX_DIRTY -- pushed DOWN to descendants on a local
//                            change so each child knows its world cache
//                            is stale; cleared on Matrix() recompute.
//   F_CHILDREN_DIRTY      -- pushed UP to ancestors so a top-down
//                            sweep can early-skip clean subtrees;
//                            cleared on Matrix() recompute of self.
//
// F_CHANGED is an observer signal set on self and all descendants on a
// local change; cleared automatically when Matrix() consumes the recompute.
//
// Hierarchy linkage: parent_ is an unsafe raw pointer (hot-path
// traversal); the parent pins the lifetime of the link via its
// children_ vector. Destruction always detaches the transform from its
// parent's children list and orphans its own children (their parent_
// is cleared, they keep their world transform via no-op until next
// SetParent call).
//
// Children iteration is index-based (GetChildCount + GetChildAt). The
// internal vector is intentionally not exposed -- the caller can't hold
// a const reference across an Add/Remove that would realloc and dangle
// it.
// *************************************************************************

#pragma once

#include <HandmadeMath.h>

#include "animsystem.h"
#include "saferef.h"

#include <cstdint>
#include <vector>

class TTransform : public TSafeObjectBase<TTransform>
{
  public:
    enum Flag : uint32_t
    {
        // Storage / value markers.
        F_HAS_POS              = 1u << 0,  // pos    != identity (0, 0, 0)
        F_HAS_ROT              = 1u << 1,  // rot    != identity (quat 0,0,0,1)
        F_HAS_SCALE            = 1u << 2,  // scale  != identity (1, 1, 1)
        F_ROT_IS_QUAT          = 1u << 3,  // rot last set as quat (else euler)

        // Hierarchy / change propagation.
        F_CHILDREN_DIRTY       = 1u << 4,  // a descendant somewhere needs recompute
        F_CHANGED              = 1u << 5,  // self or an ancestor was modified

        // Cached-matrix dirty markers.
        F_LOCAL_MATRIX_DIRTY   = 1u << 6,
        F_GLOBAL_MATRIX_DIRTY  = 1u << 7,
    };

    TTransform();
    ~TTransform();

    // Hierarchy is non-copyable / non-movable: parent_ <-> children_ is
    // a doubly-linked relationship that doesn't survive memcpy.
    TTransform(const TTransform&)            = delete;
    TTransform& operator=(const TTransform&) = delete;
    TTransform(TTransform&&)                 = delete;
    TTransform& operator=(TTransform&&)      = delete;

    // ---- Hierarchy ---------------------------------------------------------

    [[nodiscard]] TTransform* Parent() const { return parent_; }
    void                      SetParent(TTransform* parent);
        // Reparents this transform. Pass nullptr to detach from current parent.
        // No-op if parent is already this transform's parent.

    [[nodiscard]] int32_t     GetChildCount() const;
    [[nodiscard]] TTransform* GetChildAt(int32_t index) const;
        // Returns nullptr if index is out of range. The returned pointer is
        // stable until the next AddChild / RemoveChild / child destruction;
        // do NOT cache it across mutations.

    // ---- Local accessors (parent-relative) --------------------------------

    [[nodiscard]] const hmm_vec3& LocalPos() const { return pos_; }
    void                          SetLocalPos(const hmm_vec3& v);

    // LocalRot() returns the current rotation as a quat regardless of
    // whether it was last set as quat or euler. Reading round-trip
    // representation (see LocalRotEuler) is preserved separately.
    [[nodiscard]] SAnimQuat       LocalRot() const;
    [[nodiscard]] hmm_vec3        LocalRotEuler() const;
        // Returns the last-set euler if F_ROT_IS_QUAT is clear, else
        // (0, 0, 0) -- we don't auto-convert quat to euler since the
        // result is ambiguous (gimbal-lock equivalence classes).
    void                          SetLocalRot(const SAnimQuat& q);
    void                          SetLocalRotEuler(const hmm_vec3& euler_xyz);

    [[nodiscard]] const hmm_vec3& LocalScl() const { return scl_; }
    void                          SetLocalScl(const hmm_vec3& s);

    [[nodiscard]] const hmm_mat4& LocalMatrix() const;

    // ---- World (global) accessors -----------------------------------------

    [[nodiscard]] hmm_vec3        Pos() const;
    void                          SetPos(const hmm_vec3& world_pos);

    [[nodiscard]] SAnimQuat       Rot() const;
    void                          SetRot(const SAnimQuat& world_rot);
    void                          SetRotEuler(const hmm_vec3& euler_xyz);

    [[nodiscard]] hmm_vec3        Scl() const;
        // World-space ("lossy") scale, read-only. Setting world scale is
        // ill-defined under non-uniform scale through rotated parents; use
        // SetLocalScl. Matches Unity's lossyScale convention.

    [[nodiscard]] const hmm_mat4& Matrix() const;

    // ---- Top-down hierarchy refresh ---------------------------------------
    //
    // Visits every dirty node in this transform's subtree and recomputes
    // its local + global matrices to match. This is the only path that
    // also clears F_CHILDREN_DIRTY (the lazy-pull Matrix() leaves it
    // sticky on principle -- a chain query is not a sweep).
    //
    // Visit rule: skip subtrees with neither F_GLOBAL_MATRIX_DIRTY nor
    // F_CHILDREN_DIRTY. For visited nodes:
    //   * Recompute local matrix if F_LOCAL_MATRIX_DIRTY (the node's
    //     own TRS was modified).
    //   * Recompute global matrix if F_GLOBAL_MATRIX_DIRTY (cached
    //     global is stale, either because self changed or because some
    //     ancestor changed).
    //   * Clear F_LOCAL_MATRIX_DIRTY, F_GLOBAL_MATRIX_DIRTY, F_CHANGED,
    //     F_CHILDREN_DIRTY.
    //
    // Must be called on a transform whose parent (if any) is already
    // up-to-date -- typical use is to call it on the root.
    void RefreshHierarchy();

    // ---- Flag / state queries ---------------------------------------------

    [[nodiscard]] bool HasFlag(Flag f) const     { return (flags_ & uint32_t(f)) != 0; }
    [[nodiscard]] uint32_t Flags() const         { return flags_; }
    [[nodiscard]] bool ChildrenDirty() const     { return HasFlag(F_CHILDREN_DIRTY); }
    [[nodiscard]] bool Changed() const           { return HasFlag(F_CHANGED); }
    [[nodiscard]] bool LocalMatrixDirty() const  { return HasFlag(F_LOCAL_MATRIX_DIRTY); }
    [[nodiscard]] bool GlobalMatrixDirty() const { return HasFlag(F_GLOBAL_MATRIX_DIRTY); }
    [[nodiscard]] bool RotIsQuat() const         { return HasFlag(F_ROT_IS_QUAT); }

  private:
    void RecomputeLocalMatrix() const;
    void RecomputeGlobalMatrix() const;
    void MarkLocalMatrixDirty();
    void MarkChildrenDirtyUp();
    void MarkGlobalDirtyAndChangedDown();
        // Pushes F_GLOBAL_MATRIX_DIRTY + F_CHANGED to self + descendants;
        // stops descending into subtrees that are already marked.
    void DetachFromParent();
    void AddChildEntry(TTransform* child);
    void RemoveChildEntry(TTransform* child);

    TTransform*               parent_ = nullptr;
    std::vector<TTransform*>  children_;
        // Eagerly-allocated. Empty for leaves; sizeof(vector) overhead
        // (~24 bytes) is acceptable in exchange for branch-free access.

    // Primitive storage. Both quat_ and euler_ are kept so a SetXxx +
    // GetXxx round-trip preserves the form the user supplied; the
    // F_ROT_IS_QUAT flag selects which is authoritative.
    hmm_vec3  pos_   = { 0.0f, 0.0f, 0.0f };
    SAnimQuat quat_  = { 0.0f, 0.0f, 0.0f, 1.0f };
    hmm_vec3  euler_ = { 0.0f, 0.0f, 0.0f };
    hmm_vec3  scl_   = { 1.0f, 1.0f, 1.0f };

    mutable hmm_mat4 local_matrix_  = {};
    mutable hmm_mat4 global_matrix_ = {};

    // flags_ is mutable so const recompute paths (LocalMatrix / Matrix)
    // can clear cache-dirty bits as they refresh the caches.
    mutable uint32_t flags_ = uint32_t(F_LOCAL_MATRIX_DIRTY)
                            | uint32_t(F_GLOBAL_MATRIX_DIRTY);
};
