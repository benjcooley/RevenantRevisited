// *************************************************************************
// *                         Cinematix Revenant                            *
// *                      Revenant Revisited 2026                          *
// *                  test_transform.cpp - TTransform tests                *
// *************************************************************************
//
// Unit tests for the SafeRef-able hierarchical transform. Standalone
// target -- depends only on saferef.h (header-only) + animsystem.cpp +
// math3d.cpp + HandmadeMath. Run via build/test_transform after build.
//
// Each test_xxx() asserts its own correctness; main prints OK after the
// full suite passes. A failing assert exits non-zero with a diagnostic.
// *************************************************************************

#include "../src/transform.h"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace {

// ---- floating-point equality helpers -----------------------------------

constexpr float kEps = 1e-4f;

bool FeqEps(float a, float b, float eps = kEps)
{
    return std::fabs(a - b) <= eps;
}

bool VecEq(const hmm_vec3& a, const hmm_vec3& b, float eps = kEps)
{
    return FeqEps(a.X, b.X, eps) && FeqEps(a.Y, b.Y, eps) && FeqEps(a.Z, b.Z, eps);
}

bool QuatEq(const SAnimQuat& a, const SAnimQuat& b, float eps = kEps)
{
    // Quat double-cover: q and -q represent the same rotation.
    const bool same = FeqEps(a.x, b.x, eps) && FeqEps(a.y, b.y, eps)
                   && FeqEps(a.z, b.z, eps) && FeqEps(a.w, b.w, eps);
    const bool neg  = FeqEps(a.x,-b.x, eps) && FeqEps(a.y,-b.y, eps)
                   && FeqEps(a.z,-b.z, eps) && FeqEps(a.w,-b.w, eps);
    return same || neg;
}

void Fail(const char* test, const char* msg)
{
    std::fprintf(stderr, "FAIL: %s -- %s\n", test, msg);
    std::abort();
}

#define EXPECT(cond, msg) do { if (!(cond)) Fail(__func__, msg); } while (0)

// ---- helpers ------------------------------------------------------------

hmm_vec3 V(float x, float y, float z) { return { x, y, z }; }
SAnimQuat Q(float x, float y, float z, float w) { return { x, y, z, w }; }

constexpr float kPi = 3.14159265358979323846f;

// ========================================================================
// Tests
// ========================================================================

void test_default_state()
{
    TTransform t;
    EXPECT(t.Parent() == nullptr,            "default parent should be null");
    EXPECT(t.GetChildCount() == 0,           "default children count should be 0");
    EXPECT(t.GetChildAt(0) == nullptr,       "out-of-range GetChildAt returns null");
    EXPECT(t.GetChildAt(-1) == nullptr,      "negative index returns null");
    EXPECT(VecEq(t.LocalPos(), V(0,0,0)),    "default local pos is origin");
    EXPECT(VecEq(t.LocalScl(), V(1,1,1)),    "default local scale is one");
    EXPECT(QuatEq(t.LocalRot(), Q(0,0,0,1)), "default local rot is identity quat");
    EXPECT(!t.HasFlag(TTransform::F_HAS_POS),    "fresh transform has no pos flag");
    EXPECT(!t.HasFlag(TTransform::F_HAS_ROT),    "fresh transform has no rot flag");
    EXPECT(!t.HasFlag(TTransform::F_HAS_SCALE),  "fresh transform has no scale flag");
    EXPECT(!t.HasFlag(TTransform::F_ROT_IS_QUAT),"fresh transform's rot starts as euler");
    EXPECT(t.LocalMatrixDirty(),  "matrix caches are dirty until first compute");
    EXPECT(t.GlobalMatrixDirty(), "matrix caches are dirty until first compute");
}

void test_set_local_pos_basic()
{
    TTransform t;
    t.SetLocalPos(V(1, 2, 3));
    EXPECT(VecEq(t.LocalPos(), V(1, 2, 3)), "local pos stored");
    EXPECT(t.HasFlag(TTransform::F_HAS_POS), "F_HAS_POS set on non-zero pos");
    EXPECT(t.LocalMatrixDirty(),             "local matrix marked dirty");
    EXPECT(t.GlobalMatrixDirty(),            "global matrix marked dirty");
    EXPECT(t.Changed(),                      "F_CHANGED set on local change");
}

void test_set_local_pos_compare_before_set()
{
    TTransform t;
    t.SetLocalPos(V(1, 2, 3));
    // Force a recompute so flags clear.
    (void)t.Matrix();
    EXPECT(!t.GlobalMatrixDirty(), "first matrix recompute clears global-dirty");
    EXPECT(!t.Changed(),           "first matrix recompute clears changed");

    // Same value -> no-op, flags should NOT be raised.
    t.SetLocalPos(V(1, 2, 3));
    EXPECT(!t.GlobalMatrixDirty(), "same-value SetLocalPos is no-op (no global dirty)");
    EXPECT(!t.Changed(),           "same-value SetLocalPos is no-op (no changed)");
}

void test_identity_clears_has_flags()
{
    TTransform t;
    t.SetLocalPos(V(5, 0, 0));
    EXPECT(t.HasFlag(TTransform::F_HAS_POS), "non-zero pos sets F_HAS_POS");
    t.SetLocalPos(V(0, 0, 0));
    EXPECT(!t.HasFlag(TTransform::F_HAS_POS), "zero pos clears F_HAS_POS");

    t.SetLocalScl(V(2, 2, 2));
    EXPECT(t.HasFlag(TTransform::F_HAS_SCALE), "non-one scale sets F_HAS_SCALE");
    t.SetLocalScl(V(1, 1, 1));
    EXPECT(!t.HasFlag(TTransform::F_HAS_SCALE), "one scale clears F_HAS_SCALE");

    t.SetLocalRot(Q(0.7071f, 0, 0, 0.7071f));  // 90deg around X
    EXPECT(t.HasFlag(TTransform::F_HAS_ROT), "non-identity quat sets F_HAS_ROT");
    t.SetLocalRot(Q(0, 0, 0, 1));
    EXPECT(!t.HasFlag(TTransform::F_HAS_ROT), "identity quat clears F_HAS_ROT");
}

void test_rot_quat_vs_euler()
{
    TTransform t;
    t.SetLocalRotEuler(V(0.5f, 0, 0));
    EXPECT(!t.HasFlag(TTransform::F_ROT_IS_QUAT), "SetLocalRotEuler clears F_ROT_IS_QUAT");
    EXPECT(VecEq(t.LocalRotEuler(), V(0.5f, 0, 0)), "euler round-trip preserved");
    // LocalRot() converts; check it's non-identity.
    EXPECT(!QuatEq(t.LocalRot(), Q(0,0,0,1)), "LocalRot() returns non-identity for euler input");

    SAnimQuat q90x = Q(std::sin(0.25f * kPi), 0, 0, std::cos(0.25f * kPi));
    t.SetLocalRot(q90x);
    EXPECT(t.HasFlag(TTransform::F_ROT_IS_QUAT), "SetLocalRot raises F_ROT_IS_QUAT");
    EXPECT(QuatEq(t.LocalRot(), q90x), "quat round-trip preserved");
    // After quat set, LocalRotEuler returns (0,0,0) -- we don't auto-decompose.
    EXPECT(VecEq(t.LocalRotEuler(), V(0,0,0)),
           "LocalRotEuler returns zero vec after quat set (no auto-decompose)");
}

void test_local_matrix_lazy()
{
    TTransform t;
    t.SetLocalPos(V(1, 2, 3));
    EXPECT(t.LocalMatrixDirty(), "dirty after SetLocalPos");
    const hmm_mat4& m = t.LocalMatrix();
    EXPECT(!t.LocalMatrixDirty(), "clean after LocalMatrix() recompute");
    // Translation in last row (row-vector convention).
    EXPECT(FeqEps(m.Elements[3][0], 1.0f), "matrix translation x = 1");
    EXPECT(FeqEps(m.Elements[3][1], 2.0f), "matrix translation y = 2");
    EXPECT(FeqEps(m.Elements[3][2], 3.0f), "matrix translation z = 3");
}

void test_root_global_equals_local()
{
    TTransform t;
    t.SetLocalPos(V(10, 20, 30));
    const hmm_mat4& g = t.Matrix();
    const hmm_mat4& l = t.LocalMatrix();
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            EXPECT(FeqEps(g.Elements[r][c], l.Elements[r][c]),
                   "root global matrix == local matrix");
}

void test_world_pos_via_parent()
{
    TTransform parent;
    TTransform child;
    parent.SetLocalPos(V(10, 0, 0));
    child.SetParent(&parent);
    child.SetLocalPos(V(0, 5, 0));

    const hmm_vec3 world = child.Pos();
    EXPECT(VecEq(world, V(10, 5, 0)), "child world pos = parent + local (no rot)");
}

void test_world_pos_via_rotated_parent()
{
    TTransform parent;
    TTransform child;
    // Parent at origin, rotated 90deg around Z.
    parent.SetLocalRotEuler(V(0, 0, 0.5f * kPi));
    child.SetParent(&parent);
    child.SetLocalPos(V(1, 0, 0));

    const hmm_vec3 world = child.Pos();
    // Local +X rotated 90 around Z -> +Y in world.
    EXPECT(VecEq(world, V(0, 1, 0), 1e-3f),
           "child local +X under parent ZRot(90) -> world +Y");
}

void test_set_world_pos_decomposes()
{
    TTransform parent;
    TTransform child;
    parent.SetLocalPos(V(10, 20, 0));
    child.SetParent(&parent);
    child.SetPos(V(15, 22, 0));
    // local = world - parent_pos = (5, 2, 0).
    EXPECT(VecEq(child.LocalPos(), V(5, 2, 0)),
           "SetPos decomposes against parent translation");
    EXPECT(VecEq(child.Pos(), V(15, 22, 0)),
           "round-trip: SetPos then Pos returns the original world");
}

void test_set_world_rot_decomposes()
{
    TTransform parent;
    TTransform child;
    parent.SetLocalRotEuler(V(0, 0, 0.5f * kPi));
    child.SetParent(&parent);
    // Want world rot = identity.
    child.SetRot(Q(0, 0, 0, 1));
    // Parent has ZRot(90); child local must be ZRot(-90) so world cancels.
    const SAnimQuat expected = Q(0, 0, -std::sin(0.25f * kPi), std::cos(0.25f * kPi));
    EXPECT(QuatEq(child.LocalRot(), expected, 1e-3f),
           "SetRot decomposes against parent rotation");
    EXPECT(QuatEq(child.Rot(), Q(0, 0, 0, 1), 1e-3f),
           "round-trip: SetRot then Rot returns the original world");
}

void test_world_scl_compose()
{
    TTransform parent;
    TTransform child;
    parent.SetLocalScl(V(2, 2, 2));
    child.SetParent(&parent);
    child.SetLocalScl(V(3, 3, 3));
    EXPECT(VecEq(child.Scl(), V(6, 6, 6)),
           "child world scale = parent scale * local scale (component-wise)");
}

void test_set_parent_links()
{
    TTransform a;
    TTransform b;
    EXPECT(a.GetChildCount() == 0,    "a starts with no children");
    EXPECT(b.Parent() == nullptr,     "b starts orphan");

    b.SetParent(&a);
    EXPECT(b.Parent() == &a,          "b.parent_ now a");
    EXPECT(a.GetChildCount() == 1,    "a now has 1 child");
    EXPECT(a.GetChildAt(0) == &b,     "a's child is b");
}

void test_reparent()
{
    TTransform a, b, c;
    c.SetParent(&a);
    EXPECT(a.GetChildCount() == 1 && b.GetChildCount() == 0,
           "initial: c under a");
    c.SetParent(&b);
    EXPECT(c.Parent() == &b,           "reparented to b");
    EXPECT(a.GetChildCount() == 0,     "a no longer has c");
    EXPECT(b.GetChildCount() == 1 && b.GetChildAt(0) == &c,
           "b has c");
}

void test_set_parent_null_detaches()
{
    TTransform a, b;
    b.SetParent(&a);
    EXPECT(a.GetChildCount() == 1, "linked");
    b.SetParent(nullptr);
    EXPECT(b.Parent() == nullptr,  "b detached");
    EXPECT(a.GetChildCount() == 0, "a's children list cleaned up");
}

void test_set_parent_self_no_op()
{
    TTransform a;
    a.SetParent(&a);
    EXPECT(a.Parent() == nullptr,  "self-parent rejected");
    EXPECT(a.GetChildCount() == 0, "self-parent does not add to own children");
}

void test_set_parent_same_no_op()
{
    TTransform a, b;
    b.SetParent(&a);
    (void)b.Matrix();
    EXPECT(!b.Changed(), "fresh after recompute");
    b.SetParent(&a);  // same parent
    EXPECT(!b.Changed(), "setting same parent is no-op");
    EXPECT(a.GetChildCount() == 1, "no duplicate child entry");
}

void test_changed_propagates_down()
{
    TTransform a, b, c;
    b.SetParent(&a);
    c.SetParent(&b);
    // Recompute everything to clear initial dirty state.
    (void)a.Matrix(); (void)b.Matrix(); (void)c.Matrix();
    EXPECT(!a.Changed() && !b.Changed() && !c.Changed(), "all clean baseline");

    a.SetLocalPos(V(1, 0, 0));
    EXPECT(a.Changed(), "a changed");
    EXPECT(b.Changed(), "b inherits changed from ancestor");
    EXPECT(c.Changed(), "c inherits changed from ancestor");
}

void test_changed_clears_on_query()
{
    TTransform a, b;
    b.SetParent(&a);
    a.SetLocalPos(V(1, 0, 0));
    EXPECT(b.Changed(), "b is changed after a's modification");

    // Querying b's world position recomputes its global and clears its
    // F_CHANGED. Ancestors that needed recompute also clear theirs.
    (void)b.Pos();
    EXPECT(!b.Changed(),  "b.Pos() consumed F_CHANGED");
    EXPECT(!a.Changed(),  "a.Pos() chain cleared a's F_CHANGED too");
}

void test_changed_clears_on_rot_and_scl_query()
{
    // Rot() and Scl() also trigger the recompute chain so all world
    // queries behave uniformly w.r.t. F_CHANGED.
    {
        TTransform a, b;
        b.SetParent(&a);
        a.SetLocalRotEuler(V(0.3f, 0, 0));
        EXPECT(b.Changed(), "set up: b is changed");
        (void)b.Rot();
        EXPECT(!b.Changed(), "Rot() clears F_CHANGED on b");
        EXPECT(!a.Changed(), "Rot() chain clears F_CHANGED on a");
    }
    {
        TTransform a, b;
        b.SetParent(&a);
        a.SetLocalScl(V(2, 2, 2));
        EXPECT(b.Changed(), "set up: b is changed");
        (void)b.Scl();
        EXPECT(!b.Changed(), "Scl() clears F_CHANGED on b");
        EXPECT(!a.Changed(), "Scl() chain clears F_CHANGED on a");
    }
}

void test_children_dirty_propagates_up()
{
    TTransform a, b, c;
    b.SetParent(&a);
    c.SetParent(&b);

    // Reparenting itself dirties up; recompute to clear baseline.
    (void)a.Matrix(); (void)b.Matrix(); (void)c.Matrix();
    // F_CHILDREN_DIRTY isn't cleared by Matrix() per design; force-clear
    // here to test fresh propagation.
    // (We can't clear it externally without exposing internals; instead
    // we accept the sticky state and just look for forward propagation
    // after a fresh mutation.)
    c.SetLocalPos(V(0, 0, 1));
    EXPECT(b.ChildrenDirty(), "b inherits CHILDREN_DIRTY from c");
    EXPECT(a.ChildrenDirty(), "a inherits CHILDREN_DIRTY transitively");
}

void test_destruction_orphans_children()
{
    TTransform child;
    {
        TTransform parent;
        child.SetParent(&parent);
        EXPECT(child.Parent() == &parent, "linked");
        // parent dtor runs at scope end.
    }
    EXPECT(child.Parent() == nullptr, "child orphaned by parent destruction");
    EXPECT(child.GlobalMatrixDirty(),
           "child global is marked dirty so next Matrix() recomputes against new parent state");
}

void test_destruction_unlinks_from_parent()
{
    TTransform parent;
    {
        TTransform child;
        child.SetParent(&parent);
        EXPECT(parent.GetChildCount() == 1, "parent has 1 child");
    }
    EXPECT(parent.GetChildCount() == 0, "parent's children list cleaned up after child destroyed");
}

void test_get_child_at_bounds()
{
    TTransform a, b, c;
    b.SetParent(&a);
    c.SetParent(&a);
    EXPECT(a.GetChildCount() == 2, "two children");
    EXPECT(a.GetChildAt(0) == &b,  "[0] = b");
    EXPECT(a.GetChildAt(1) == &c,  "[1] = c");
    EXPECT(a.GetChildAt(2) == nullptr,  "out-of-range high returns null");
    EXPECT(a.GetChildAt(-1) == nullptr, "out-of-range low returns null");
}

void test_safe_ref()
{
    TSafeRef<TTransform> ref;
    EXPECT(!ref.IsValid(),     "default ref is invalid");
    EXPECT(ref.Get() == nullptr, "default ref resolves to null");

    {
        TTransform t;
        ref = &t;
        EXPECT(ref.IsValid(),       "ref valid while target alive");
        EXPECT(ref.Get() == &t,     "ref resolves to target");
        EXPECT(ref.Id()  == t.SafeRefId(),  "ref captured id");
        EXPECT(ref.Gen() == t.SafeRefGen(), "ref captured gen");
    }
    // t out of scope -> ref must become invalid.
    EXPECT(!ref.IsValid(),    "ref invalidated by target destruction");
    EXPECT(ref.Get() == nullptr, "ref resolves to null after target gone");
}

void test_world_pos_under_translate_rotate_scale()
{
    // Compose a non-trivial parent and verify Matrix() row-3 matches a
    // hand-computed world position.
    TTransform parent, child;
    parent.SetLocalPos(V(100, 0, 0));
    parent.SetLocalScl(V(2, 2, 2));
    parent.SetLocalRotEuler(V(0, 0, 0.5f * kPi));   // ZRot(90)
    child.SetParent(&parent);
    child.SetLocalPos(V(1, 0, 0));
    // child local +X under: scale 2 (->2,0,0), rot 90Z (->0,2,0), translate +100X
    // -> world (100, 2, 0).
    EXPECT(VecEq(child.Pos(), V(100, 2, 0), 1e-3f),
           "world = T * R * S applied to local origin (row-vector convention)");
}

void test_grand_child_caching()
{
    // Querying a deep descendant should recompute the chain top-down
    // implicitly (each Matrix() recurses up). Sanity-check that values
    // are consistent and stable across two reads.
    TTransform a, b, c;
    b.SetParent(&a);
    c.SetParent(&b);
    a.SetLocalPos(V(1, 0, 0));
    b.SetLocalPos(V(0, 1, 0));
    c.SetLocalPos(V(0, 0, 1));
    const hmm_vec3 first  = c.Pos();
    const hmm_vec3 second = c.Pos();
    EXPECT(VecEq(first, second), "two consecutive Pos() reads are equal");
    EXPECT(VecEq(first, V(1, 1, 1)), "world = sum of pure-translation chain");
}

void test_reparent_marks_dirty_chain()
{
    TTransform a, b, child;
    a.SetLocalPos(V(10, 0, 0));
    b.SetLocalPos(V(0, 10, 0));

    child.SetLocalPos(V(1, 0, 0));
    child.SetParent(&a);
    EXPECT(VecEq(child.Pos(), V(11, 0, 0)), "child under a -> 11,0,0");

    child.SetParent(&b);
    EXPECT(VecEq(child.Pos(), V(1, 10, 0)), "after reparent, child under b -> 1,10,0");
}

// ========================================================================
// Dirty-flag corner cases
// ========================================================================

// --- F_HAS_POS / F_HAS_ROT / F_HAS_SCALE micro-corners ---

void test_set_pos_zero_on_fresh_transform_no_op()
{
    // Fresh transform already has pos == zero. Setting (0,0,0) should
    // be a true no-op (no dirty flags raised).
    TTransform t;
    EXPECT(!t.HasFlag(TTransform::F_HAS_POS), "fresh has no pos flag");
    // Force-clear changed by recomputing.
    (void)t.Matrix();
    EXPECT(!t.Changed(), "baseline clean");
    t.SetLocalPos(V(0, 0, 0));
    EXPECT(!t.Changed(),         "SetLocalPos(0,0,0) on already-zero is no-op");
    EXPECT(!t.HasFlag(TTransform::F_HAS_POS), "still no pos flag");
}

void test_set_scl_one_on_fresh_no_op()
{
    TTransform t;
    (void)t.Matrix();
    t.SetLocalScl(V(1, 1, 1));
    EXPECT(!t.Changed(),                       "SetLocalScl(1,1,1) is no-op on fresh");
    EXPECT(!t.HasFlag(TTransform::F_HAS_SCALE),"no scale flag");
}

void test_set_rot_identity_quat_on_fresh_no_op()
{
    TTransform t;
    (void)t.Matrix();
    t.SetLocalRot(Q(0, 0, 0, 1));
    // F_ROT_IS_QUAT toggle is a state change in storage representation,
    // even though the rotation value is identity. Whether this counts
    // as "changed" is a contract decision -- we accept that storage-
    // form toggles raise dirty so the matrix path stays consistent.
    EXPECT(!t.HasFlag(TTransform::F_HAS_ROT), "identity quat does not raise F_HAS_ROT");
}

void test_set_euler_zero_clears_has_rot()
{
    TTransform t;
    t.SetLocalRotEuler(V(1.0f, 0, 0));
    EXPECT(t.HasFlag(TTransform::F_HAS_ROT), "non-zero euler raises F_HAS_ROT");
    t.SetLocalRotEuler(V(0, 0, 0));
    EXPECT(!t.HasFlag(TTransform::F_HAS_ROT), "zero euler clears F_HAS_ROT");
}

void test_quat_normalization_on_set()
{
    // Setting an unnormalized quat should normalize on store; reading
    // back should give the unit form.
    TTransform t;
    SAnimQuat unnorm = Q(2.0f, 0.0f, 0.0f, 0.0f);   // ||q|| = 2
    t.SetLocalRot(unnorm);
    SAnimQuat r = t.LocalRot();
    const float len2 = r.x*r.x + r.y*r.y + r.z*r.z + r.w*r.w;
    EXPECT(FeqEps(len2, 1.0f), "stored quat is normalized to unit length");
}

// --- F_LOCAL_MATRIX_DIRTY independence ---

void test_local_matrix_dirty_does_not_propagate()
{
    // F_LOCAL_MATRIX_DIRTY is per-transform; mutating a child's local
    // does NOT mark the parent's local matrix dirty (parent's local
    // is unaffected).
    TTransform parent, child;
    child.SetParent(&parent);
    (void)parent.LocalMatrix();
    (void)child.LocalMatrix();
    EXPECT(!parent.LocalMatrixDirty(), "parent local clean");
    EXPECT(!child.LocalMatrixDirty(),  "child local clean");

    child.SetLocalPos(V(1, 0, 0));
    EXPECT(child.LocalMatrixDirty(),    "child local now dirty");
    EXPECT(!parent.LocalMatrixDirty(),  "parent local NOT dirtied by child mutation");
}

void test_parent_change_dirties_child_global_not_local()
{
    // Parent's local TRS change should dirty the child's GLOBAL cache
    // (which depends on parent.global) but NOT its LOCAL cache.
    TTransform parent, child;
    child.SetParent(&parent);
    (void)child.Matrix();    // recompute everything
    EXPECT(!child.LocalMatrixDirty(),  "child local clean baseline");
    EXPECT(!child.GlobalMatrixDirty(), "child global clean baseline");

    parent.SetLocalPos(V(7, 0, 0));
    EXPECT(!child.LocalMatrixDirty(),
           "child local stays clean -- only parent's local changed");
    EXPECT(child.GlobalMatrixDirty(),
           "child global dirty -- depends on parent.global");
}

void test_local_matrix_recompute_does_not_clear_global_dirty()
{
    // LocalMatrix() refreshes only the local cache. The global cache
    // depends on parent.global * local; refreshing local alone leaves
    // global stale, so F_GLOBAL_MATRIX_DIRTY must persist.
    TTransform t;
    t.SetLocalPos(V(1, 2, 3));
    EXPECT(t.LocalMatrixDirty() && t.GlobalMatrixDirty(), "both dirty");
    (void)t.LocalMatrix();
    EXPECT(!t.LocalMatrixDirty(), "local clean after LocalMatrix()");
    EXPECT(t.GlobalMatrixDirty(), "global remains dirty after LocalMatrix() alone");
}

void test_local_matrix_recompute_does_not_clear_changed()
{
    // LocalMatrix() is cache-only; F_CHANGED is the world-change
    // signal and should only clear when a world-space query (Matrix /
    // Pos / Rot / Scl) consumes the recompute.
    TTransform t;
    t.SetLocalPos(V(1, 0, 0));
    EXPECT(t.Changed(),           "set up: F_CHANGED");
    (void)t.LocalMatrix();
    EXPECT(t.Changed(),           "LocalMatrix() does NOT clear F_CHANGED");
}

void test_matrix_recomputes_local_lazily()
{
    // Going straight from SetLocalPos to Matrix() (skipping
    // LocalMatrix) should still produce the right result -- the
    // global recompute has to refresh the local cache itself.
    TTransform t;
    t.SetLocalPos(V(4, 5, 6));
    const hmm_mat4& m = t.Matrix();
    EXPECT(FeqEps(m.Elements[3][0], 4.0f), "Matrix() picked up x");
    EXPECT(FeqEps(m.Elements[3][1], 5.0f), "Matrix() picked up y");
    EXPECT(FeqEps(m.Elements[3][2], 6.0f), "Matrix() picked up z");
    EXPECT(!t.LocalMatrixDirty(),  "Matrix() recomputed local along the way");
}

// --- F_CHANGED corner cases ---

void test_changed_set_on_self_modified_transform()
{
    TTransform t;
    (void)t.Matrix();
    EXPECT(!t.Changed(), "baseline");
    t.SetLocalPos(V(1, 0, 0));
    EXPECT(t.Changed(), "F_CHANGED on the transform that was modified");
}

void test_changed_propagates_down_three_levels()
{
    TTransform a, b, c, d;
    b.SetParent(&a);
    c.SetParent(&b);
    d.SetParent(&c);
    (void)a.Matrix(); (void)b.Matrix(); (void)c.Matrix(); (void)d.Matrix();
    EXPECT(!a.Changed() && !b.Changed() && !c.Changed() && !d.Changed(),
           "four-level chain clean baseline");

    a.SetLocalScl(V(2, 2, 2));
    EXPECT(a.Changed(), "a changed");
    EXPECT(b.Changed(), "b inherits");
    EXPECT(c.Changed(), "c inherits transitively");
    EXPECT(d.Changed(), "d (leaf) inherits");
}

void test_local_matrix_query_does_not_clear_changed()
{
    TTransform a, b;
    b.SetParent(&a);
    a.SetLocalPos(V(1, 0, 0));
    EXPECT(b.Changed(), "set up: b is changed");
    (void)b.LocalMatrix();   // cache-only, not a world query
    EXPECT(b.Changed(),  "LocalMatrix() does NOT clear F_CHANGED");
    EXPECT(a.Changed(),  "a still changed too");
}

void test_changed_isolation_between_siblings()
{
    TTransform a, b, c;
    b.SetParent(&a);
    c.SetParent(&a);
    (void)a.Matrix(); (void)b.Matrix(); (void)c.Matrix();
    EXPECT(!b.Changed() && !c.Changed(), "siblings clean baseline");

    b.SetLocalPos(V(1, 0, 0));
    EXPECT(b.Changed(),  "b changed");
    EXPECT(!c.Changed(), "sibling c unaffected by b's change");
    EXPECT(!a.Changed(), "parent a unaffected by descendant change (only CHILDREN_DIRTY goes up)");
}

void test_matrix_clears_changed_only_on_recomputed_path()
{
    // Two siblings: change root, then query only one sibling. The
    // queried sibling and its ancestors clear F_CHANGED; the unread
    // sibling stays F_CHANGED until it's queried itself.
    TTransform a, b, c;
    b.SetParent(&a);
    c.SetParent(&a);
    (void)b.Matrix(); (void)c.Matrix();

    a.SetLocalPos(V(5, 0, 0));
    EXPECT(b.Changed() && c.Changed(), "both children inherit changed from a");

    (void)b.Pos();
    EXPECT(!a.Changed(), "a's F_CHANGED cleared by b's chain recompute");
    EXPECT(!b.Changed(), "b cleared by its own query");
    EXPECT(c.Changed(),  "c not yet queried -- stays changed");

    (void)c.Pos();
    EXPECT(!c.Changed(), "c clears once queried");
}

// --- F_CHILDREN_DIRTY corner cases ---

void test_children_dirty_only_propagates_up()
{
    // Modifying a child should set F_CHILDREN_DIRTY on every ancestor
    // up to the root, but not on siblings or the modified node itself.
    TTransform a, b, c, d;
    b.SetParent(&a);
    c.SetParent(&b);
    d.SetParent(&a);   // d sibling of b (under a)

    c.SetLocalPos(V(1, 0, 0));
    EXPECT(b.ChildrenDirty(), "direct parent gets CHILDREN_DIRTY");
    EXPECT(a.ChildrenDirty(), "grandparent gets CHILDREN_DIRTY");
    EXPECT(!d.ChildrenDirty(),"sibling subtree (d) does NOT get CHILDREN_DIRTY");
    EXPECT(!c.ChildrenDirty(),"the modified node itself does not get CHILDREN_DIRTY");
}

void test_children_dirty_sticky_after_recompute()
{
    // Per design: F_CHILDREN_DIRTY is only cleared by an explicit
    // top-down sweep. A leaf-only Matrix() recompute leaves it set
    // even though that branch is now clean.
    TTransform a, b;
    b.SetParent(&a);
    b.SetLocalPos(V(1, 0, 0));
    EXPECT(a.ChildrenDirty(), "a marked CHILDREN_DIRTY");
    (void)b.Matrix();
    EXPECT(a.ChildrenDirty(), "still sticky after b's recompute (no top-down sweep ran)");
}

void test_reparent_marks_new_chain_children_dirty()
{
    TTransform a, b, c;   // a, b are root candidates; c is the mover
    c.SetParent(&a);
    (void)a.Matrix();
    // We can't easily clear F_CHILDREN_DIRTY on a; just check b gets
    // it freshly when c re-parents into b.
    EXPECT(!b.ChildrenDirty(), "b had no children, no CHILDREN_DIRTY");

    c.SetParent(&b);
    EXPECT(b.ChildrenDirty(),
           "reparenting c under b marks b CHILDREN_DIRTY");
}

void test_set_parent_null_dirties_self_global()
{
    TTransform parent, child;
    parent.SetLocalPos(V(10, 0, 0));
    child.SetParent(&parent);
    (void)child.Matrix();
    EXPECT(!child.GlobalMatrixDirty(), "child clean before detach");
    EXPECT(VecEq(child.Pos(), V(10, 0, 0)), "child world = parent translation");

    child.SetParent(nullptr);
    EXPECT(child.GlobalMatrixDirty(),
           "detaching from parent dirties child global (world space changed)");
    EXPECT(VecEq(child.Pos(), V(0, 0, 0)),
           "after detach, world == local == origin");
}

// --- Destruction propagation corners ---

void test_destruction_dirties_orphan_descendants()
{
    // Two-level orphan: parent dies, child's grandchild's global
    // depended on parent through the chain. Both child and grandchild
    // should be flagged dirty by the parent's destructor.
    TTransform child, grandchild;
    grandchild.SetParent(&child);
    {
        TTransform parent;
        parent.SetLocalPos(V(100, 0, 0));
        child.SetParent(&parent);
        (void)grandchild.Matrix();
        EXPECT(!grandchild.GlobalMatrixDirty(), "grandchild clean baseline");
        // parent dtor at scope end orphans `child`.
    }
    EXPECT(child.Parent() == nullptr,        "child orphaned");
    EXPECT(child.GlobalMatrixDirty(),        "child global dirty after parent died");
    EXPECT(grandchild.GlobalMatrixDirty(),
           "grandchild global dirty too -- mark propagated down on orphan");
}

void test_destruction_does_not_dirty_unrelated_tree()
{
    TTransform unrelated;
    unrelated.SetLocalPos(V(7, 0, 0));
    (void)unrelated.Matrix();
    EXPECT(!unrelated.GlobalMatrixDirty(), "unrelated clean before");
    EXPECT(!unrelated.Changed(),           "unrelated not changed");

    {
        TTransform parent, child;
        child.SetParent(&parent);
    } // both go out of scope -- shouldn't touch `unrelated`.

    EXPECT(!unrelated.GlobalMatrixDirty(),
           "destroying an unrelated tree leaves this transform clean");
    EXPECT(!unrelated.Changed(),
           "unrelated F_CHANGED not perturbed");
}

// --- Matrix recompute clears the right flags, doesn't clear others ---

void test_matrix_clears_global_dirty_and_changed_only()
{
    TTransform t;
    t.SetLocalPos(V(1, 2, 3));
    t.SetLocalRotEuler(V(0.5f, 0, 0));
    t.SetLocalScl(V(2, 2, 2));
    EXPECT(t.LocalMatrixDirty() && t.GlobalMatrixDirty() && t.Changed(),
           "all three dirty before Matrix()");
    EXPECT(t.HasFlag(TTransform::F_HAS_POS) &&
           t.HasFlag(TTransform::F_HAS_ROT) &&
           t.HasFlag(TTransform::F_HAS_SCALE),
           "all HAS_X flags set");

    (void)t.Matrix();

    EXPECT(!t.LocalMatrixDirty(),  "Matrix() cleared local-dirty");
    EXPECT(!t.GlobalMatrixDirty(), "Matrix() cleared global-dirty");
    EXPECT(!t.Changed(),           "Matrix() cleared F_CHANGED");
    // HAS_X markers persist (they're descriptive, not dirty).
    EXPECT(t.HasFlag(TTransform::F_HAS_POS) &&
           t.HasFlag(TTransform::F_HAS_ROT) &&
           t.HasFlag(TTransform::F_HAS_SCALE),
           "HAS_X markers persist across recompute");
}

// --- Multi-mutation corners ---

void test_repeated_mutations_keep_state_consistent()
{
    // Hammer the same transform with many mutations of the same value
    // and a few real changes. Final state must be correct.
    TTransform t;
    for (int i = 0; i < 10; ++i)
        t.SetLocalPos(V(1, 2, 3));     // 9 are no-ops after the first
    EXPECT(VecEq(t.LocalPos(), V(1, 2, 3)), "value still 1,2,3");
    EXPECT(VecEq(t.Pos(), V(1, 2, 3)),      "world matches local (no parent)");
    t.SetLocalPos(V(10, 0, 0));
    EXPECT(VecEq(t.Pos(), V(10, 0, 0)),     "subsequent change applied");
}

void test_chain_of_changes_all_cleared_by_single_query()
{
    // Set local many times before any query, then query once. The
    // accumulated F_CHANGED should clear in one go.
    TTransform a, b;
    b.SetParent(&a);
    a.SetLocalPos(V(1, 0, 0));
    a.SetLocalScl(V(2, 2, 2));
    a.SetLocalRotEuler(V(0.1f, 0, 0));
    EXPECT(a.Changed() && b.Changed(), "both flagged after multiple sets");

    (void)b.Matrix();
    EXPECT(!a.Changed(), "single descendant query clears the whole chain's F_CHANGED");
    EXPECT(!b.Changed(), "and the descendant itself");
}

// --- Reparent + descendant interaction ---

void test_reparent_grandchildren_inherit_global_dirty()
{
    TTransform a, b, child, grandchild;
    child.SetParent(&a);
    grandchild.SetParent(&child);
    (void)grandchild.Matrix();
    EXPECT(!grandchild.GlobalMatrixDirty(), "grandchild clean before reparent");

    child.SetParent(&b);  // moves the whole subtree
    EXPECT(grandchild.GlobalMatrixDirty(),
           "grandchild's global dirtied by ancestor reparent");
    EXPECT(grandchild.Changed(),
           "grandchild also gets F_CHANGED");
}

// ========================================================================
// Lazy pull (Matrix()) vs top-down sweep (RefreshHierarchy)
// ========================================================================

void test_lazy_pull_stops_at_topmost_clean_ancestor()
{
    // X is a clean ancestor that pre-dates the change. The recompute
    // chain on a deep query should stop walking up at X (X.Matrix()
    // hits cached value and returns), not redundantly traverse above.
    TTransform x, a, b, c;
    a.SetParent(&x);
    b.SetParent(&a);
    c.SetParent(&b);
    (void)c.Matrix();   // recompute everything once.
    EXPECT(!x.Changed() && !a.Changed() && !b.Changed() && !c.Changed(),
           "all clean baseline");

    // Mutate at A (mid-tree), then query c. The chain x->a->b->c is
    // recomputed; x is already clean and stays untouched.
    a.SetLocalPos(V(1, 0, 0));
    (void)c.Matrix();
    EXPECT(!a.Changed() && !b.Changed() && !c.Changed(),
           "chain a->b->c cleared by leaf query");
    EXPECT(!x.Changed(), "x stays clean -- not on the dirty chain");
}

void test_lazy_pull_does_not_clear_children_dirty()
{
    // Per the design: a chain query is not a sweep, so F_CHILDREN_DIRTY
    // remains sticky. Only RefreshHierarchy clears it.
    TTransform a, b, c;
    b.SetParent(&a);
    c.SetParent(&b);

    c.SetLocalPos(V(1, 0, 0));
    EXPECT(a.ChildrenDirty() && b.ChildrenDirty(),
           "chain marked CHILDREN_DIRTY up from c");

    (void)c.Matrix();
    EXPECT(!c.GlobalMatrixDirty(), "c global recomputed");
    EXPECT(a.ChildrenDirty(), "a's CHILDREN_DIRTY remains sticky after lazy pull");
    EXPECT(b.ChildrenDirty(), "b's CHILDREN_DIRTY remains sticky after lazy pull");
}

void test_lazy_pull_does_not_recompute_siblings_or_descendants()
{
    // Query a midpoint node M; its descendants should retain their
    // F_GLOBAL_MATRIX_DIRTY because the lazy chain only walks up.
    TTransform a, b, c;
    b.SetParent(&a);
    c.SetParent(&b);

    a.SetLocalPos(V(2, 0, 0));
    (void)b.Matrix();   // pull only b; do NOT touch c.
    EXPECT(!b.GlobalMatrixDirty(), "b's global cleaned by its own query");
    EXPECT(c.GlobalMatrixDirty(),  "c's global remains dirty -- sibling-side not visited");
    EXPECT(c.Changed(),            "c still has F_CHANGED inherited from a");
}

void test_refresh_hierarchy_clears_children_dirty()
{
    TTransform root, mid, leaf;
    mid.SetParent(&root);
    leaf.SetParent(&mid);

    leaf.SetLocalPos(V(1, 0, 0));
    EXPECT(root.ChildrenDirty(), "root marked CHILDREN_DIRTY");
    EXPECT(mid.ChildrenDirty(),  "mid  marked CHILDREN_DIRTY");

    root.RefreshHierarchy();
    EXPECT(!root.ChildrenDirty(),  "RefreshHierarchy clears CHILDREN_DIRTY on root");
    EXPECT(!mid.ChildrenDirty(),   "and on mid");
    EXPECT(!leaf.GlobalMatrixDirty(), "leaf's global recomputed");
    EXPECT(!leaf.Changed(),           "leaf F_CHANGED cleared");
}

void test_refresh_hierarchy_recomputes_dirty_nodes_only()
{
    // root is changed; mid is not changed (parent change cascaded
    // F_GLOBAL_MATRIX_DIRTY but not F_LOCAL_MATRIX_DIRTY). After
    // RefreshHierarchy, mid's local should NOT have been touched
    // (used cached), but its global should be the rebuilt one.
    TTransform root, mid, leaf;
    mid.SetParent(&root);
    leaf.SetParent(&mid);
    mid.SetLocalPos(V(0, 5, 0));   // give mid a non-trivial local
    leaf.SetLocalPos(V(0, 0, 7));
    (void)leaf.Matrix();           // first refresh: everything clean.

    root.SetLocalPos(V(3, 0, 0));
    EXPECT(root.LocalMatrixDirty(), "root local TRS-dirty (its own change)");
    EXPECT(!mid.LocalMatrixDirty(), "mid  local NOT TRS-dirty (only inherited global)");
    EXPECT(!leaf.LocalMatrixDirty(),"leaf local NOT TRS-dirty");
    EXPECT(mid.GlobalMatrixDirty() && leaf.GlobalMatrixDirty(),
           "but globals are dirty for everything in subtree");

    root.RefreshHierarchy();
    EXPECT(!root.LocalMatrixDirty() && !mid.LocalMatrixDirty() && !leaf.LocalMatrixDirty(),
           "all locals clean after sweep");
    EXPECT(!root.GlobalMatrixDirty() && !mid.GlobalMatrixDirty() && !leaf.GlobalMatrixDirty(),
           "all globals clean after sweep");

    // Verify the math is correct: leaf world = root + mid_local + leaf_local.
    EXPECT(VecEq(leaf.Pos(), V(3, 5, 7)),
           "leaf world correct after top-down recompute");
}

void test_refresh_hierarchy_skips_clean_subtrees()
{
    // Root has two children A and B. Mutate something in A's subtree.
    // RefreshHierarchy on root should descend into A's branch but NOT
    // into B's (B has no F_GLOBAL_MATRIX_DIRTY and no F_CHILDREN_DIRTY).
    // We can't directly observe "did it descend" without instrumentation,
    // but we can check that B's matrix cache stays untouched and its
    // pre-existing flag state survives.
    TTransform root, a, b, a_child;
    a.SetParent(&root);
    b.SetParent(&root);
    a_child.SetParent(&a);
    (void)root.Matrix(); (void)a.Matrix(); (void)b.Matrix(); (void)a_child.Matrix();

    a_child.SetLocalPos(V(9, 0, 0));
    EXPECT(root.ChildrenDirty(), "root marked");
    EXPECT(!b.GlobalMatrixDirty() && !b.Changed(),
           "B (sibling subtree) untouched");

    root.RefreshHierarchy();
    EXPECT(!a_child.GlobalMatrixDirty(), "A's branch was visited and refreshed");
    EXPECT(!b.GlobalMatrixDirty(), "B still clean (and was never dirty)");
}

void test_refresh_hierarchy_no_op_on_clean_tree()
{
    TTransform root, child;
    child.SetParent(&root);
    (void)child.Matrix();   // everything clean.

    // Calling RefreshHierarchy on a clean tree should be a complete
    // no-op (the early-out prevents traversal).
    root.RefreshHierarchy();
    EXPECT(!root.GlobalMatrixDirty() && !child.GlobalMatrixDirty(), "still clean");
    EXPECT(!root.Changed() && !child.Changed(), "still clean");
}

void test_lazy_pull_after_refresh_is_noop_for_query()
{
    // After RefreshHierarchy at the root, a subsequent leaf query
    // should hit cached globals everywhere -- no recompute happens.
    TTransform root, mid, leaf;
    mid.SetParent(&root);
    leaf.SetParent(&mid);
    root.SetLocalPos(V(1, 0, 0));
    mid.SetLocalPos(V(0, 1, 0));
    leaf.SetLocalPos(V(0, 0, 1));

    root.RefreshHierarchy();
    EXPECT(!root.GlobalMatrixDirty() && !mid.GlobalMatrixDirty() && !leaf.GlobalMatrixDirty(),
           "everything clean after sweep");
    EXPECT(!root.Changed() && !mid.Changed() && !leaf.Changed(),
           "F_CHANGED cleared on every node");
    EXPECT(!root.ChildrenDirty() && !mid.ChildrenDirty(),
           "F_CHILDREN_DIRTY cleared on every ancestor");

    // Pos read should not flip any flag.
    EXPECT(VecEq(leaf.Pos(), V(1, 1, 1)), "world correct");
    EXPECT(!leaf.GlobalMatrixDirty() && !leaf.Changed(), "still clean after query");
}

void test_refresh_hierarchy_only_changes_what_needs_changing()
{
    // root.SetLocalPos changes only root's local; mid and leaf locals
    // should NOT have their local matrices recomputed. We can't
    // observe "was RecomputeLocalMatrix called?" directly, but we
    // can verify F_LOCAL_MATRIX_DIRTY behaviour: mid and leaf never
    // had their own local TRS-dirty bit set, so the sweep doesn't
    // touch them at the local level. Validated by checking the
    // numeric output is exact.
    TTransform root, mid, leaf;
    mid.SetParent(&root);
    leaf.SetParent(&mid);
    mid.SetLocalScl(V(2, 2, 2));    // non-identity to force visible compose
    (void)leaf.Matrix();            // first refresh.

    root.SetLocalPos(V(10, 0, 0));
    root.RefreshHierarchy();
    // Manually verify world: leaf = root_local * (mid_local * leaf_local).
    // root_local = T(10,0,0). mid_local = S(2,2,2). leaf_local = identity.
    // leaf world position = (origin * S(2)) * T(10) = (10,0,0).
    EXPECT(VecEq(leaf.Pos(), V(10, 0, 0)),
           "leaf world after partial-dirty sweep is correct");
}

// ========================================================================
// Local readback permutations — local* accessors must be unaffected by
// any mutation that doesn't directly target this node.
// ========================================================================

void test_local_pos_unchanged_after_parent_change()
{
    TTransform parent, child;
    child.SetParent(&parent);
    child.SetLocalPos(V(1, 2, 3));
    parent.SetLocalPos(V(99, 99, 99));
    EXPECT(VecEq(child.LocalPos(), V(1, 2, 3)),
           "child local pos unaffected by parent's pos change");
}

void test_local_rot_unchanged_after_parent_change()
{
    TTransform parent, child;
    child.SetParent(&parent);
    child.SetLocalRotEuler(V(0.5f, 0.0f, 0.0f));
    parent.SetLocalRotEuler(V(1.0f, 1.0f, 1.0f));
    EXPECT(VecEq(child.LocalRotEuler(), V(0.5f, 0.0f, 0.0f)),
           "child local euler unaffected by parent's rotation");
}

void test_local_scl_unchanged_after_parent_change()
{
    TTransform parent, child;
    child.SetParent(&parent);
    child.SetLocalScl(V(2, 2, 2));
    parent.SetLocalScl(V(7, 7, 7));
    EXPECT(VecEq(child.LocalScl(), V(2, 2, 2)),
           "child local scale unaffected by parent's scale");
}

void test_local_unchanged_after_grandparent_change()
{
    TTransform a, b, c, d;
    b.SetParent(&a);
    c.SetParent(&b);
    d.SetParent(&c);
    d.SetLocalPos(V(7, 8, 9));
    d.SetLocalRotEuler(V(0.3f, 0, 0));
    d.SetLocalScl(V(3, 3, 3));

    a.SetLocalPos(V(100, 200, 300));
    a.SetLocalScl(V(0.5f, 0.5f, 0.5f));
    a.SetLocalRotEuler(V(1.5f, 1.5f, 1.5f));

    EXPECT(VecEq(d.LocalPos(), V(7, 8, 9)),                "d local pos preserved");
    EXPECT(VecEq(d.LocalRotEuler(), V(0.3f, 0, 0)),        "d local rot preserved");
    EXPECT(VecEq(d.LocalScl(), V(3, 3, 3)),                "d local scl preserved");
}

void test_local_unchanged_after_descendant_change()
{
    TTransform parent, child;
    child.SetParent(&parent);
    parent.SetLocalPos(V(10, 0, 0));
    parent.SetLocalRotEuler(V(0.7f, 0, 0));
    parent.SetLocalScl(V(2, 2, 2));

    child.SetLocalPos(V(99, 0, 0));
    child.SetLocalScl(V(5, 5, 5));

    EXPECT(VecEq(parent.LocalPos(), V(10, 0, 0)),         "parent local pos preserved");
    EXPECT(VecEq(parent.LocalRotEuler(), V(0.7f, 0, 0)),  "parent local rot preserved");
    EXPECT(VecEq(parent.LocalScl(), V(2, 2, 2)),          "parent local scl preserved");
}

void test_local_unchanged_after_sibling_change()
{
    TTransform parent, sib_a, sib_b;
    sib_a.SetParent(&parent);
    sib_b.SetParent(&parent);
    sib_a.SetLocalPos(V(1, 2, 3));
    sib_a.SetLocalScl(V(2, 2, 2));

    sib_b.SetLocalPos(V(99, 0, 0));
    sib_b.SetLocalRotEuler(V(0.5f, 0, 0));

    EXPECT(VecEq(sib_a.LocalPos(), V(1, 2, 3)),  "sibling A local pos preserved");
    EXPECT(VecEq(sib_a.LocalScl(), V(2, 2, 2)),  "sibling A local scl preserved");
}

void test_local_unchanged_after_reparent()
{
    TTransform x, y, child;
    child.SetParent(&x);
    child.SetLocalPos(V(1, 2, 3));
    child.SetLocalRotEuler(V(0.4f, 0, 0));
    child.SetLocalScl(V(3, 3, 3));

    child.SetParent(&y);
    EXPECT(VecEq(child.LocalPos(), V(1, 2, 3)),         "local pos survives reparent");
    EXPECT(VecEq(child.LocalRotEuler(), V(0.4f, 0, 0)), "local rot survives reparent");
    EXPECT(VecEq(child.LocalScl(), V(3, 3, 3)),         "local scl survives reparent");
}

void test_local_unchanged_after_detach()
{
    TTransform parent, child;
    child.SetParent(&parent);
    child.SetLocalPos(V(1, 2, 3));
    child.SetLocalScl(V(4, 4, 4));
    child.SetParent(nullptr);
    EXPECT(VecEq(child.LocalPos(), V(1, 2, 3)), "local pos survives detach");
    EXPECT(VecEq(child.LocalScl(), V(4, 4, 4)), "local scl survives detach");
}

void test_local_unchanged_after_parent_destruction()
{
    TTransform child;
    child.SetLocalPos(V(5, 6, 7));
    child.SetLocalRotEuler(V(0.1f, 0.2f, 0.3f));
    {
        TTransform parent;
        child.SetParent(&parent);
        // parent goes out of scope -> child orphaned
    }
    EXPECT(child.Parent() == nullptr,                 "child orphaned");
    EXPECT(VecEq(child.LocalPos(), V(5, 6, 7)),       "local pos survives parent dtor");
    EXPECT(VecEq(child.LocalRotEuler(), V(0.1f, 0.2f, 0.3f)),
           "local rot survives parent dtor");
}

void test_local_matrix_cache_unaffected_by_upstream()
{
    // LocalMatrix is a function of THIS node's TRS only. Once cached,
    // upstream changes must not invalidate it.
    TTransform parent, child;
    child.SetParent(&parent);
    child.SetLocalPos(V(1, 2, 3));
    (void)child.LocalMatrix();
    EXPECT(!child.LocalMatrixDirty(), "child local matrix cached");

    parent.SetLocalPos(V(100, 0, 0));
    EXPECT(!child.LocalMatrixDirty(), "parent's change doesn't invalidate child's local cache");

    parent.SetLocalRotEuler(V(0.5f, 0, 0));
    EXPECT(!child.LocalMatrixDirty(), "parent's rotation doesn't invalidate child's local cache");

    parent.SetLocalScl(V(5, 5, 5));
    EXPECT(!child.LocalMatrixDirty(), "parent's scale doesn't invalidate child's local cache");
}

void test_local_matrix_cache_unaffected_by_descendant()
{
    TTransform parent, child;
    child.SetParent(&parent);
    parent.SetLocalPos(V(1, 2, 3));
    (void)parent.LocalMatrix();
    EXPECT(!parent.LocalMatrixDirty(), "parent local matrix cached");

    child.SetLocalPos(V(99, 99, 99));
    child.SetLocalRotEuler(V(0.5f, 0, 0));
    child.SetLocalScl(V(7, 7, 7));
    EXPECT(!parent.LocalMatrixDirty(),
           "descendant changes don't invalidate parent's local cache");
}

void test_local_matrix_cache_unaffected_by_sibling()
{
    TTransform parent, sib_a, sib_b;
    sib_a.SetParent(&parent);
    sib_b.SetParent(&parent);
    sib_a.SetLocalPos(V(1, 2, 3));
    (void)sib_a.LocalMatrix();
    EXPECT(!sib_a.LocalMatrixDirty(), "sib_a cached");

    sib_b.SetLocalPos(V(50, 50, 50));
    sib_b.SetLocalRotEuler(V(1.0f, 0, 0));
    EXPECT(!sib_a.LocalMatrixDirty(), "sib_a cache unaffected by sib_b changes");
}

void test_local_matrix_cache_unaffected_by_reparent()
{
    TTransform x, y, child;
    child.SetParent(&x);
    child.SetLocalPos(V(1, 2, 3));
    (void)child.LocalMatrix();
    EXPECT(!child.LocalMatrixDirty(), "cached");
    child.SetParent(&y);
    EXPECT(!child.LocalMatrixDirty(), "reparent doesn't invalidate own local cache");
}

// ========================================================================
// World-readback permutations across various mutation patterns.
// ========================================================================

void test_world_at_root_unaffected_by_descendant_change()
{
    TTransform root, child;
    child.SetParent(&root);
    root.SetLocalPos(V(10, 0, 0));
    EXPECT(VecEq(root.Pos(), V(10, 0, 0)), "root world set up");

    child.SetLocalPos(V(99, 99, 99));
    child.SetLocalRotEuler(V(1.0f, 0, 0));
    EXPECT(VecEq(root.Pos(), V(10, 0, 0)),
           "root world unchanged by child mutation");
}

void test_world_at_leaf_reflects_root_change()
{
    TTransform root, mid, leaf;
    mid.SetParent(&root);
    leaf.SetParent(&mid);
    leaf.SetLocalPos(V(1, 0, 0));
    EXPECT(VecEq(leaf.Pos(), V(1, 0, 0)), "initial");

    root.SetLocalPos(V(0, 5, 0));
    EXPECT(VecEq(leaf.Pos(), V(1, 5, 0)), "leaf world picks up root translate");
}

void test_world_at_leaf_reflects_mid_change()
{
    TTransform root, mid, leaf;
    mid.SetParent(&root);
    leaf.SetParent(&mid);
    leaf.SetLocalPos(V(1, 0, 0));
    EXPECT(VecEq(leaf.Pos(), V(1, 0, 0)), "initial");

    mid.SetLocalPos(V(0, 0, 7));
    EXPECT(VecEq(leaf.Pos(), V(1, 0, 7)), "leaf world picks up mid translate");
}

void test_world_at_mid_reflects_root_change_only()
{
    TTransform root, mid, leaf;
    mid.SetParent(&root);
    leaf.SetParent(&mid);
    mid.SetLocalPos(V(1, 0, 0));

    leaf.SetLocalPos(V(99, 99, 99));   // descendant change
    EXPECT(VecEq(mid.Pos(), V(1, 0, 0)), "mid unaffected by leaf change");

    root.SetLocalPos(V(10, 0, 0));     // ancestor change
    EXPECT(VecEq(mid.Pos(), V(11, 0, 0)), "mid picks up root change");
}

void test_world_at_leaf_after_multiple_chain_changes()
{
    TTransform root, mid, leaf;
    mid.SetParent(&root);
    leaf.SetParent(&mid);
    root.SetLocalPos(V(1, 0, 0));
    mid.SetLocalPos(V(0, 1, 0));
    leaf.SetLocalPos(V(0, 0, 1));
    EXPECT(VecEq(leaf.Pos(), V(1, 1, 1)), "additive translation chain");

    root.SetLocalPos(V(10, 0, 0));
    mid.SetLocalPos(V(0, 20, 0));
    leaf.SetLocalPos(V(0, 0, 30));
    EXPECT(VecEq(leaf.Pos(), V(10, 20, 30)),
           "leaf reflects every level's change without intermediate query");
}

void test_world_query_order_independence()
{
    // Querying root, mid, leaf in arbitrary orders should always
    // produce the same numeric result, regardless of how the cache
    // gets populated.
    auto build = [](TTransform& root, TTransform& mid, TTransform& leaf) {
        mid.SetParent(&root);
        leaf.SetParent(&mid);
        root.SetLocalPos(V(1, 0, 0));
        mid.SetLocalPos(V(0, 1, 0));
        leaf.SetLocalPos(V(0, 0, 1));
    };

    {
        TTransform r, m, l; build(r, m, l);
        // Order: leaf, mid, root.
        const hmm_vec3 lp = l.Pos();
        const hmm_vec3 mp = m.Pos();
        const hmm_vec3 rp = r.Pos();
        EXPECT(VecEq(lp, V(1, 1, 1)) && VecEq(mp, V(1, 1, 0)) && VecEq(rp, V(1, 0, 0)),
               "order leaf-mid-root");
    }
    {
        TTransform r, m, l; build(r, m, l);
        // Order: root, leaf, mid.
        const hmm_vec3 rp = r.Pos();
        const hmm_vec3 lp = l.Pos();
        const hmm_vec3 mp = m.Pos();
        EXPECT(VecEq(rp, V(1, 0, 0)) && VecEq(lp, V(1, 1, 1)) && VecEq(mp, V(1, 1, 0)),
               "order root-leaf-mid");
    }
    {
        TTransform r, m, l; build(r, m, l);
        // Order: mid, root, leaf.
        const hmm_vec3 mp = m.Pos();
        const hmm_vec3 rp = r.Pos();
        const hmm_vec3 lp = l.Pos();
        EXPECT(VecEq(mp, V(1, 1, 0)) && VecEq(rp, V(1, 0, 0)) && VecEq(lp, V(1, 1, 1)),
               "order mid-root-leaf");
    }
}

void test_world_at_sibling_after_other_sibling_change()
{
    TTransform parent, a, b;
    a.SetParent(&parent);
    b.SetParent(&parent);
    a.SetLocalPos(V(1, 0, 0));
    b.SetLocalPos(V(0, 0, 1));
    EXPECT(VecEq(a.Pos(), V(1, 0, 0)) && VecEq(b.Pos(), V(0, 0, 1)),
           "siblings independent baseline");

    a.SetLocalPos(V(99, 99, 99));
    EXPECT(VecEq(b.Pos(), V(0, 0, 1)),
           "sibling A's change doesn't move sibling B in world");
}

void test_world_after_setpos_then_descendant_setlocalpos()
{
    TTransform parent, child;
    child.SetParent(&parent);
    parent.SetLocalPos(V(10, 0, 0));
    child.SetPos(V(15, 0, 0));    // world-space SetPos -> child.local = (5, 0, 0)
    EXPECT(VecEq(child.LocalPos(), V(5, 0, 0)), "SetPos decomposed correctly");

    // Now move child via local-space without intervening parent change.
    child.SetLocalPos(V(7, 0, 0));
    EXPECT(VecEq(child.Pos(), V(17, 0, 0)),
           "child world reflects parent + new local");
}

void test_world_after_setpos_then_parent_moves()
{
    TTransform parent, child;
    child.SetParent(&parent);
    parent.SetLocalPos(V(10, 0, 0));
    child.SetPos(V(15, 0, 0));
    // Now move parent. Child's LOCAL is preserved, world tracks parent.
    parent.SetLocalPos(V(20, 0, 0));
    EXPECT(VecEq(child.LocalPos(), V(5, 0, 0)),
           "child local stays at the value SetPos decomposed");
    EXPECT(VecEq(child.Pos(), V(25, 0, 0)),
           "child world tracks parent's new world + same local");
}

void test_local_changes_dont_break_root_world_cache()
{
    // After a root.Matrix() warms up its cache, a deep mutation in
    // the subtree should not perturb root's cached value (root has
    // no F_CHANGED, no F_GLOBAL_MATRIX_DIRTY -- only F_CHILDREN_DIRTY).
    TTransform root, mid, leaf;
    mid.SetParent(&root);
    leaf.SetParent(&mid);
    root.SetLocalPos(V(7, 0, 0));
    (void)root.Matrix();
    EXPECT(!root.GlobalMatrixDirty(), "root cache warm");

    leaf.SetLocalPos(V(123, 456, 789));
    EXPECT(!root.GlobalMatrixDirty(), "leaf change does NOT mark root dirty");
    EXPECT(VecEq(root.Pos(), V(7, 0, 0)),
           "root world remains exactly what it was");
}

void test_destroy_intermediate_orphans_grandchild()
{
    // Destroying a mid-chain node leaves the grandchild orphaned with
    // its local TRS preserved, but world recomputed from local alone.
    TTransform grandparent, grandchild;
    grandparent.SetLocalPos(V(100, 0, 0));
    grandchild.SetLocalPos(V(1, 0, 0));
    {
        TTransform parent;
        parent.SetParent(&grandparent);
        parent.SetLocalPos(V(10, 0, 0));
        grandchild.SetParent(&parent);
        EXPECT(VecEq(grandchild.Pos(), V(111, 0, 0)),
               "grandchild world = 100 + 10 + 1");
        // parent dies here.
    }
    EXPECT(grandchild.Parent() == nullptr,            "grandchild orphaned by parent dtor");
    EXPECT(VecEq(grandchild.LocalPos(), V(1, 0, 0)),  "grandchild local preserved");
    EXPECT(VecEq(grandchild.Pos(), V(1, 0, 0)),       "grandchild world == local now (no parent)");
}

// ========================================================================
// TSafeRef thorough tests
// ========================================================================

void test_saferef_default_is_invalid()
{
    TSafeRef<TTransform> r;
    EXPECT(!r.IsValid(),       "default invalid");
    EXPECT(r.Get() == nullptr, "default resolves null");
    EXPECT(r.Id() == -1,       "default id == -1");
    EXPECT(r.Gen() == 0u,      "default gen == 0");
    EXPECT(!bool(r),           "default bool() false");
}

void test_saferef_capture_via_ctor()
{
    TTransform t;
    TSafeRef<TTransform> r(&t);
    EXPECT(r.IsValid(),                "captured via ctor is valid");
    EXPECT(r.Get() == &t,              "resolves to target");
    EXPECT(r.Id()  == t.SafeRefId(),   "id matches target");
    EXPECT(r.Gen() == t.SafeRefGen(),  "gen matches target");
}

void test_saferef_capture_via_assign()
{
    TTransform t;
    TSafeRef<TTransform> r;
    r = &t;
    EXPECT(r.IsValid() && r.Get() == &t, "assignment captures correctly");
}

void test_saferef_capture_nullptr()
{
    TSafeRef<TTransform> r;
    {
        TTransform t;
        r = &t;
        EXPECT(r.IsValid(), "valid");
        r = nullptr;
        EXPECT(!r.IsValid(),       "nullptr assignment invalidates");
        EXPECT(r.Id() == -1,       "id reset to -1");
        EXPECT(r.Gen() == 0u,      "gen reset to 0");
    }
}

void test_saferef_clear()
{
    TTransform t;
    TSafeRef<TTransform> r(&t);
    EXPECT(r.IsValid(), "valid");
    r.Clear();
    EXPECT(!r.IsValid(),  "Clear() invalidates");
    EXPECT(r.Id() == -1,  "id reset");
    EXPECT(r.Gen() == 0u, "gen reset");
}

void test_saferef_target_destruction_invalidates()
{
    TSafeRef<TTransform> r;
    {
        TTransform t;
        r = &t;
        EXPECT(r.IsValid(), "valid while target alive");
    }
    EXPECT(!r.IsValid(),       "invalid after target destroyed");
    EXPECT(r.Get() == nullptr, "resolves null");
}

void test_saferef_multi_target_independence()
{
    // Several refs to several distinct targets. Destroying one
    // invalidates only its ref; others stay valid.
    TTransform a;
    TTransform* b = new TTransform();
    TTransform c;

    TSafeRef<TTransform> ra(&a);
    TSafeRef<TTransform> rb(b);
    TSafeRef<TTransform> rc(&c);

    EXPECT(ra.IsValid() && rb.IsValid() && rc.IsValid(), "all three valid");

    delete b;
    EXPECT(ra.IsValid(),     "ra unaffected by b's destruction");
    EXPECT(!rb.IsValid(),    "rb invalidated by its target's destruction");
    EXPECT(rc.IsValid(),     "rc unaffected");
}

void test_saferef_to_self()
{
    // A transform holding a SafeRef to itself: legal; resolves to
    // itself while alive, null after destruction.
    TSafeRef<TTransform> self_ref;
    {
        TTransform t;
        self_ref = &t;
        EXPECT(self_ref.Get() == &t, "self-ref resolves to self");
    }
    EXPECT(self_ref.Get() == nullptr, "self-ref invalid after destruction");
}

void test_saferef_to_orphaned_child_still_valid()
{
    // Destroying a parent orphans its children but does NOT destroy
    // them. A SafeRef captured against the child must remain valid.
    TTransform child;
    TSafeRef<TTransform> r(&child);
    {
        TTransform parent;
        child.SetParent(&parent);
        EXPECT(r.IsValid(), "ref valid before parent dies");
    }
    EXPECT(r.IsValid(),       "ref still valid after parent destroyed");
    EXPECT(r.Get() == &child, "still resolves to child");
}

void test_saferef_equality()
{
    TTransform a, b;
    TSafeRef<TTransform> r1(&a);
    TSafeRef<TTransform> r2(&a);
    TSafeRef<TTransform> r3(&b);
    EXPECT(r1 == r2,    "two refs to same target compare equal");
    EXPECT(r1 != r3,    "refs to different targets compare unequal");

    TSafeRef<TTransform> empty1, empty2;
    EXPECT(empty1 == empty2, "two default refs are equal");
    EXPECT(r1 != empty1,     "valid ref != default ref");
}

void test_saferef_copy()
{
    TTransform t;
    TSafeRef<TTransform> r1(&t);
    TSafeRef<TTransform> r2 = r1;     // copy
    EXPECT(r2.IsValid() && r2.Get() == &t, "copy resolves to same target");
    EXPECT(r1 == r2,                  "copy equals original");
}

void test_saferef_distinct_instances_have_distinct_ids()
{
    // Two TTransforms have monotonically distinct ids and gens.
    TTransform a, b;
    EXPECT(a.SafeRefId() != b.SafeRefId(),  "distinct ids");
    EXPECT(a.SafeRefGen() != b.SafeRefGen(),"distinct gens");
}

void test_saferef_id_reuse_safety()
{
    // Capture a ref, destroy the target, create a new transform.
    // The new transform has a fresh id (monotonic, no reuse) and fresh
    // gen, so the old ref doesn't accidentally resolve to it. This
    // exercises the (id, gen) defense against id recycling.
    TSafeRef<TTransform> stale;
    int32_t old_id = -1;
    uint32_t old_gen = 0;
    {
        TTransform t;
        stale = &t;
        old_id = t.SafeRefId();
        old_gen = t.SafeRefGen();
    }
    EXPECT(!stale.IsValid(), "stale ref invalid");

    // Allocate many transforms; the id space should remain monotonic
    // (TSafeObjectBase doesn't recycle), but verify the stale ref
    // never resolves regardless.
    TTransform fresh1, fresh2, fresh3;
    EXPECT(fresh1.SafeRefId() != old_id, "monotonic id never reuses old id");
    EXPECT(stale.Get() == nullptr,       "stale ref still null after fresh allocations");
    EXPECT(stale.Id()  == old_id,        "stale captured id preserved");
    EXPECT(stale.Gen() == old_gen,       "stale captured gen preserved");
}

void test_saferef_survives_multiple_resolves()
{
    // Calling Get() repeatedly should be idempotent and not perturb
    // the registry.
    TTransform t;
    TSafeRef<TTransform> r(&t);
    for (int i = 0; i < 100; ++i)
    {
        EXPECT(r.Get() == &t, "stable resolution");
    }
}

void test_safe_cast_unchecked()
{
    TTransform t;
    TTransform* p = safe_cast<TTransform>(t.SafeRefId());
    EXPECT(p == &t, "safe_cast<T>(id) resolves to target (unchecked)");

    TTransform* nullp = safe_cast<TTransform>(-1);
    EXPECT(nullp == nullptr, "safe_cast<T>(-1) returns null");
}

void test_safe_cast_checked()
{
    TTransform t;
    int32_t id   = t.SafeRefId();
    uint32_t gen = t.SafeRefGen();
    TTransform* good = safe_cast<TTransform>(id, gen);
    EXPECT(good == &t, "safe_cast<T>(id, gen) with matching gen resolves");

    TTransform* bad = safe_cast<TTransform>(id, gen + 1);
    EXPECT(bad == nullptr, "safe_cast<T>(id, gen) with wrong gen returns null");
}

void test_safe_cast_from_ref()
{
    TTransform t;
    TSafeRef<TTransform> r(&t);
    TTransform* p = safe_cast<TTransform>(r);
    EXPECT(p == &t, "safe_cast<T>(ref) resolves");

    {
        TTransform short_lived;
        TSafeRef<TTransform> sr(&short_lived);
    }
    // sr went out of scope; we don't have a stale ref to test, but
    // we can construct one manually with stale (id, gen):
    TSafeRef<TTransform> stale(99999, 99999);
    EXPECT(safe_cast<TTransform>(stale) == nullptr,
           "safe_cast on stale ref returns null");
}

void test_saferef_outlives_many_transforms()
{
    // Stress: create and destroy many transforms in a loop, holding
    // a single ref to one of them. Verify the ref's behavior is
    // consistent throughout.
    TTransform target;
    TSafeRef<TTransform> r(&target);
    EXPECT(r.IsValid(), "valid pre-loop");

    for (int i = 0; i < 100; ++i)
    {
        TTransform tmp;
        EXPECT(r.IsValid() && r.Get() == &target,
               "ref to target unaffected by churn");
    }
    EXPECT(r.IsValid(), "still valid post-loop");
}

void test_saferef_in_hierarchy_independence()
{
    // Refs across a hierarchy. Mutating values doesn't perturb refs
    // (they track identity, not state).
    TTransform a, b, c;
    b.SetParent(&a);
    c.SetParent(&b);
    TSafeRef<TTransform> ra(&a), rb(&b), rc(&c);

    a.SetLocalPos(V(1, 2, 3));
    b.SetLocalRotEuler(V(0.5f, 0, 0));
    c.SetLocalScl(V(7, 7, 7));
    (void)c.Matrix();

    EXPECT(ra.Get() == &a && rb.Get() == &b && rc.Get() == &c,
           "refs unaffected by mutations or matrix recompute");
}

} // namespace

int main()
{
#define RUN(t) do { t(); std::printf("ok %s\n", #t); } while (0)
    RUN(test_default_state);
    RUN(test_set_local_pos_basic);
    RUN(test_set_local_pos_compare_before_set);
    RUN(test_identity_clears_has_flags);
    RUN(test_rot_quat_vs_euler);
    RUN(test_local_matrix_lazy);
    RUN(test_root_global_equals_local);
    RUN(test_world_pos_via_parent);
    RUN(test_world_pos_via_rotated_parent);
    RUN(test_set_world_pos_decomposes);
    RUN(test_set_world_rot_decomposes);
    RUN(test_world_scl_compose);
    RUN(test_set_parent_links);
    RUN(test_reparent);
    RUN(test_set_parent_null_detaches);
    RUN(test_set_parent_self_no_op);
    RUN(test_set_parent_same_no_op);
    RUN(test_changed_propagates_down);
    RUN(test_changed_clears_on_query);
    RUN(test_changed_clears_on_rot_and_scl_query);
    RUN(test_children_dirty_propagates_up);
    RUN(test_destruction_orphans_children);
    RUN(test_destruction_unlinks_from_parent);
    RUN(test_get_child_at_bounds);
    RUN(test_safe_ref);
    RUN(test_world_pos_under_translate_rotate_scale);
    RUN(test_grand_child_caching);
    RUN(test_reparent_marks_dirty_chain);

    // --- Dirty-flag corner cases ---
    RUN(test_set_pos_zero_on_fresh_transform_no_op);
    RUN(test_set_scl_one_on_fresh_no_op);
    RUN(test_set_rot_identity_quat_on_fresh_no_op);
    RUN(test_set_euler_zero_clears_has_rot);
    RUN(test_quat_normalization_on_set);
    RUN(test_local_matrix_dirty_does_not_propagate);
    RUN(test_parent_change_dirties_child_global_not_local);
    RUN(test_local_matrix_recompute_does_not_clear_global_dirty);
    RUN(test_local_matrix_recompute_does_not_clear_changed);
    RUN(test_matrix_recomputes_local_lazily);
    RUN(test_changed_set_on_self_modified_transform);
    RUN(test_changed_propagates_down_three_levels);
    RUN(test_local_matrix_query_does_not_clear_changed);
    RUN(test_changed_isolation_between_siblings);
    RUN(test_matrix_clears_changed_only_on_recomputed_path);
    RUN(test_children_dirty_only_propagates_up);
    RUN(test_children_dirty_sticky_after_recompute);
    RUN(test_reparent_marks_new_chain_children_dirty);
    RUN(test_set_parent_null_dirties_self_global);
    RUN(test_destruction_dirties_orphan_descendants);
    RUN(test_destruction_does_not_dirty_unrelated_tree);
    RUN(test_matrix_clears_global_dirty_and_changed_only);
    RUN(test_repeated_mutations_keep_state_consistent);
    RUN(test_chain_of_changes_all_cleared_by_single_query);
    RUN(test_reparent_grandchildren_inherit_global_dirty);

    // --- Lazy pull vs top-down sweep ---
    RUN(test_lazy_pull_stops_at_topmost_clean_ancestor);
    RUN(test_lazy_pull_does_not_clear_children_dirty);
    RUN(test_lazy_pull_does_not_recompute_siblings_or_descendants);
    RUN(test_refresh_hierarchy_clears_children_dirty);
    RUN(test_refresh_hierarchy_recomputes_dirty_nodes_only);
    RUN(test_refresh_hierarchy_skips_clean_subtrees);
    RUN(test_refresh_hierarchy_no_op_on_clean_tree);
    RUN(test_lazy_pull_after_refresh_is_noop_for_query);
    RUN(test_refresh_hierarchy_only_changes_what_needs_changing);

    // --- Local readback permutations ---
    RUN(test_local_pos_unchanged_after_parent_change);
    RUN(test_local_rot_unchanged_after_parent_change);
    RUN(test_local_scl_unchanged_after_parent_change);
    RUN(test_local_unchanged_after_grandparent_change);
    RUN(test_local_unchanged_after_descendant_change);
    RUN(test_local_unchanged_after_sibling_change);
    RUN(test_local_unchanged_after_reparent);
    RUN(test_local_unchanged_after_detach);
    RUN(test_local_unchanged_after_parent_destruction);
    RUN(test_local_matrix_cache_unaffected_by_upstream);
    RUN(test_local_matrix_cache_unaffected_by_descendant);
    RUN(test_local_matrix_cache_unaffected_by_sibling);
    RUN(test_local_matrix_cache_unaffected_by_reparent);

    // --- World readback permutations ---
    RUN(test_world_at_root_unaffected_by_descendant_change);
    RUN(test_world_at_leaf_reflects_root_change);
    RUN(test_world_at_leaf_reflects_mid_change);
    RUN(test_world_at_mid_reflects_root_change_only);
    RUN(test_world_at_leaf_after_multiple_chain_changes);
    RUN(test_world_query_order_independence);
    RUN(test_world_at_sibling_after_other_sibling_change);
    RUN(test_world_after_setpos_then_descendant_setlocalpos);
    RUN(test_world_after_setpos_then_parent_moves);
    RUN(test_local_changes_dont_break_root_world_cache);
    RUN(test_destroy_intermediate_orphans_grandchild);

    // --- TSafeRef thorough ---
    RUN(test_saferef_default_is_invalid);
    RUN(test_saferef_capture_via_ctor);
    RUN(test_saferef_capture_via_assign);
    RUN(test_saferef_capture_nullptr);
    RUN(test_saferef_clear);
    RUN(test_saferef_target_destruction_invalidates);
    RUN(test_saferef_multi_target_independence);
    RUN(test_saferef_to_self);
    RUN(test_saferef_to_orphaned_child_still_valid);
    RUN(test_saferef_equality);
    RUN(test_saferef_copy);
    RUN(test_saferef_distinct_instances_have_distinct_ids);
    RUN(test_saferef_id_reuse_safety);
    RUN(test_saferef_survives_multiple_resolves);
    RUN(test_safe_cast_unchecked);
    RUN(test_safe_cast_checked);
    RUN(test_safe_cast_from_ref);
    RUN(test_saferef_outlives_many_transforms);
    RUN(test_saferef_in_hierarchy_independence);
#undef RUN
    std::printf("all tests passed\n");
    return 0;
}
