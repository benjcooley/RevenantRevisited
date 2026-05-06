// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           transform.cpp - SafeRef-able hierarchical transform         *
// *************************************************************************

#include "transform.h"

#include "math3d.h"

#include <algorithm>
#include <cmath>

namespace
{

constexpr float kEqEps = 1e-6f;

bool VecEqual(const hmm_vec3& a, const hmm_vec3& b)
{
    return std::fabs(a.X - b.X) <= kEqEps &&
           std::fabs(a.Y - b.Y) <= kEqEps &&
           std::fabs(a.Z - b.Z) <= kEqEps;
}

bool QuatEqual(const SAnimQuat& a, const SAnimQuat& b)
{
    return std::fabs(a.x - b.x) <= kEqEps &&
           std::fabs(a.y - b.y) <= kEqEps &&
           std::fabs(a.z - b.z) <= kEqEps &&
           std::fabs(a.w - b.w) <= kEqEps;
}

bool VecIsZero(const hmm_vec3& v)
{
    return std::fabs(v.X) <= kEqEps &&
           std::fabs(v.Y) <= kEqEps &&
           std::fabs(v.Z) <= kEqEps;
}

bool VecIsOne(const hmm_vec3& v)
{
    return std::fabs(v.X - 1.0f) <= kEqEps &&
           std::fabs(v.Y - 1.0f) <= kEqEps &&
           std::fabs(v.Z - 1.0f) <= kEqEps;
}

bool QuatIsIdentity(const SAnimQuat& q)
{
    return std::fabs(q.x) <= kEqEps &&
           std::fabs(q.y) <= kEqEps &&
           std::fabs(q.z) <= kEqEps &&
           std::fabs(q.w - 1.0f) <= kEqEps;
}

SAnimQuat QuatMul(const SAnimQuat& a, const SAnimQuat& b)
{
    return {
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
    };
}

SAnimQuat QuatConjugate(const SAnimQuat& q)
{
    // Unit-quat conjugate == inverse.
    return { -q.x, -q.y, -q.z, q.w };
}

hmm_vec3 QuatRotateVec(const SAnimQuat& q, const hmm_vec3& v)
{
    // v' = q * v * conj(q). Implemented via the Rodrigues form
    //   v' = v + 2 * cross(qxyz, cross(qxyz, v) + qw * v)
    // which is faster than the literal triple-quat-multiply.
    const float qx = q.x, qy = q.y, qz = q.z, qw = q.w;
    const float tx = 2.0f * (qy * v.Z - qz * v.Y);
    const float ty = 2.0f * (qz * v.X - qx * v.Z);
    const float tz = 2.0f * (qx * v.Y - qy * v.X);
    return {
        v.X + qw * tx + (qy * tz - qz * ty),
        v.Y + qw * ty + (qz * tx - qx * tz),
        v.Z + qw * tz + (qx * ty - qy * tx),
    };
}

SAnimQuat QuatFromEulerXYZ(const hmm_vec3& e)
{
    const float cx = std::cos(e.X * 0.5f), sx = std::sin(e.X * 0.5f);
    const float cy = std::cos(e.Y * 0.5f), sy = std::sin(e.Y * 0.5f);
    const float cz = std::cos(e.Z * 0.5f), sz = std::sin(e.Z * 0.5f);
    const SAnimQuat qx = {  sx, 0.0f, 0.0f,  cx };
    const SAnimQuat qy = { 0.0f,  sy, 0.0f,  cy };
    const SAnimQuat qz = { 0.0f, 0.0f,  sz,  cz };
    return NormalizeQuat(QuatMul(QuatMul(qz, qy), qx));
}

// Inverse of QuatFromEulerXYZ. Decomposes a quat into XYZ Tait-Bryan
// angles: rotations applied to a vector in the order rx (around X),
// ry (around Y), rz (around Z) -- the same convention QuatFromEulerXYZ
// produces. Result is non-unique under gimbal lock (|ry| = pi/2);
// in that case rz is pinned to 0 and rx absorbs the freedom.
hmm_vec3 QuatToEulerXYZ(const SAnimQuat& q)
{
    const SAnimQuat n = NormalizeQuat(q);
    // R[2][0] of the row-vector rotation matrix = 2(xz - wy) = -sin(ry).
    // Clamp to [-1, 1] to swallow floating-point drift before asin.
    const float r20      = 2.0f * (n.x * n.z - n.w * n.y);
    const float sin_ry   = -r20;
    const float sin_clip = sin_ry < -1.0f ? -1.0f : (sin_ry > 1.0f ? 1.0f : sin_ry);

    constexpr float kGimbal = 1.0f - 1e-6f;
    if (std::fabs(sin_clip) >= kGimbal)
    {
        // Gimbal lock: ry == +/- pi/2. Decomposition is non-unique;
        // fix rz = 0 and absorb the residual rotation into rx.
        const float ry = sin_clip > 0.0f ? 0.5f * 3.14159265358979323846f
                                         : -0.5f * 3.14159265358979323846f;
        const float rx = std::atan2(-2.0f * (n.y * n.z - n.w * n.x),
                                     1.0f - 2.0f * (n.x * n.x + n.z * n.z));
        return { rx, ry, 0.0f };
    }
    const float rx = std::atan2(2.0f * (n.y * n.z + n.w * n.x),
                                 1.0f - 2.0f * (n.x * n.x + n.y * n.y));
    const float ry = std::asin(sin_clip);
    const float rz = std::atan2(2.0f * (n.x * n.y + n.w * n.z),
                                 1.0f - 2.0f * (n.y * n.y + n.z * n.z));
    return { rx, ry, rz };
}

void QuatToMat4(const SAnimQuat& q, hmm_mat4* out)
{
    // Row-vector convention; matches math3d's MtxRotate* layout (and
    // HandmadeMath's HMM_QuaternionToMat4 byte layout) so the result
    // composes via MtxMultiply with MtxScale / MtxTranslate cleanly.
    const SAnimQuat n = NormalizeQuat(q);
    const float xx = n.x * n.x;
    const float yy = n.y * n.y;
    const float zz = n.z * n.z;
    const float xy = n.x * n.y;
    const float xz = n.x * n.z;
    const float yz = n.y * n.z;
    const float wx = n.w * n.x;
    const float wy = n.w * n.y;
    const float wz = n.w * n.z;

    MtxClear(out);
    out->Elements[0][0] = 1.0f - 2.0f * (yy + zz);
    out->Elements[0][1] =        2.0f * (xy + wz);
    out->Elements[0][2] =        2.0f * (xz - wy);

    out->Elements[1][0] =        2.0f * (xy - wz);
    out->Elements[1][1] = 1.0f - 2.0f * (xx + zz);
    out->Elements[1][2] =        2.0f * (yz + wx);

    out->Elements[2][0] =        2.0f * (xz + wy);
    out->Elements[2][1] =        2.0f * (yz - wx);
    out->Elements[2][2] = 1.0f - 2.0f * (xx + yy);
}

} // namespace

// =====================================================================

TTransform::TTransform() = default;

TTransform::~TTransform()
{
    // Detach from parent's children list, then orphan any of our own
    // children. Each orphaned child's cached global_matrix_ now refers
    // to a parent that's about to disappear, so push a global-dirty
    // flag down its subtree -- next Matrix() call recomputes against
    // their new (rootless) state. Their local TRS values are
    // preserved; effectively they snap to local-space as world.
    DetachFromParent();
    for (TTransform* c : children_)
    {
        if (!c) continue;
        c->parent_ = nullptr;
        c->MarkGlobalDirtyAndChangedDown();
    }
    children_.clear();
}

// ---- Hierarchy ---------------------------------------------------------

void TTransform::SetParent(TTransform* parent)
{
    if (parent == parent_) return;

    // Disallow trivial cycle: parenting to self. Walking the chain to
    // detect deeper cycles is more work than the rare misuse warrants;
    // the immediate self-parent case is the one that's silently
    // catastrophic (infinite recursion in matrix recompute).
    if (parent == this) return;

    DetachFromParent();
    parent_ = parent;
    if (parent_)
        parent_->AddChildEntry(this);

    // Reparenting changes world transform even if local stayed the same.
    MarkGlobalDirtyAndChangedDown();
    MarkChildrenDirtyUp();
}

int32_t TTransform::GetChildCount() const
{
    return int32_t(children_.size());
}

TTransform* TTransform::GetChildAt(int32_t index) const
{
    if (index < 0 || index >= int32_t(children_.size())) return nullptr;
    return children_[size_t(index)];
}

void TTransform::AddChildEntry(TTransform* child)
{
    children_.push_back(child);
}

void TTransform::RemoveChildEntry(TTransform* child)
{
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end())
        children_.erase(it);
}

void TTransform::DetachFromParent()
{
    if (parent_)
    {
        parent_->RemoveChildEntry(this);
        parent_ = nullptr;
    }
}

// ---- Local accessors ---------------------------------------------------

void TTransform::SetLocalPos(const hmm_vec3& v)
{
    if (VecEqual(pos_, v)) return;
    pos_ = v;
    if (VecIsZero(pos_)) flags_ &= ~uint32_t(F_HAS_POS);
    else                 flags_ |= uint32_t(F_HAS_POS);
    MarkLocalMatrixDirty();
    MarkGlobalDirtyAndChangedDown();
    MarkChildrenDirtyUp();
}

SAnimQuat TTransform::LocalRot() const
{
    if (HasFlag(F_ROT_IS_QUAT))
        return quat_;
    return QuatFromEulerXYZ(euler_);
}

hmm_vec3 TTransform::LocalRotEuler() const
{
    // If the rotation was last set as euler, return the user's stored
    // form verbatim (lossless round-trip). Otherwise decompose the
    // stored quat into XYZ Tait-Bryan angles. The decomposition is
    // not unique under gimbal lock; we pin rz = 0 and let rx absorb
    // the residual.
    if (!HasFlag(F_ROT_IS_QUAT))
        return euler_;
    return QuatToEulerXYZ(quat_);
}

void TTransform::SetLocalRot(const SAnimQuat& q)
{
    const SAnimQuat n = NormalizeQuat(q);
    const SAnimQuat current = LocalRot();
    if (HasFlag(F_ROT_IS_QUAT) && QuatEqual(current, n)) return;
    quat_ = n;
    flags_ |= uint32_t(F_ROT_IS_QUAT);
    if (QuatIsIdentity(quat_)) flags_ &= ~uint32_t(F_HAS_ROT);
    else                       flags_ |= uint32_t(F_HAS_ROT);
    MarkLocalMatrixDirty();
    MarkGlobalDirtyAndChangedDown();
    MarkChildrenDirtyUp();
}

void TTransform::SetLocalRotEuler(const hmm_vec3& e)
{
    if (!HasFlag(F_ROT_IS_QUAT) && VecEqual(euler_, e)) return;
    euler_ = e;
    flags_ &= ~uint32_t(F_ROT_IS_QUAT);
    if (VecIsZero(euler_)) flags_ &= ~uint32_t(F_HAS_ROT);
    else                   flags_ |= uint32_t(F_HAS_ROT);
    MarkLocalMatrixDirty();
    MarkGlobalDirtyAndChangedDown();
    MarkChildrenDirtyUp();
}

void TTransform::SetLocalScl(const hmm_vec3& s)
{
    if (VecEqual(scl_, s)) return;
    scl_ = s;
    if (VecIsOne(scl_)) flags_ &= ~uint32_t(F_HAS_SCALE);
    else                flags_ |= uint32_t(F_HAS_SCALE);
    MarkLocalMatrixDirty();
    MarkGlobalDirtyAndChangedDown();
    MarkChildrenDirtyUp();
}

const hmm_mat4& TTransform::LocalMatrix() const
{
    if (HasFlag(F_LOCAL_MATRIX_DIRTY))
        RecomputeLocalMatrix();
    return local_matrix_;
}

// ---- World accessors --------------------------------------------------

hmm_vec3 TTransform::Pos() const
{
    if (!parent_) return pos_;
    const hmm_mat4& m = Matrix();
    // Row-vector convention: translation is the last row.
    return { m.Elements[3][0], m.Elements[3][1], m.Elements[3][2] };
}

void TTransform::SetPos(const hmm_vec3& world_pos)
{
    if (!parent_)
    {
        SetLocalPos(world_pos);
        return;
    }
    // Decompose world -> local via parent's world TRS:
    //   local = inverse(parentScl) * inverse(parentRot) * (world - parentPos)
    // Lossy under non-uniform scale through rotated parents (Unity
    // lossyScale). For the common case (uniform scale or scale-1
    // throughout the chain) this is exact.
    const hmm_vec3  parent_pos = parent_->Pos();
    const SAnimQuat parent_rot = parent_->Rot();
    const hmm_vec3  parent_scl = parent_->Scl();

    const hmm_vec3 rel  = { world_pos.X - parent_pos.X,
                            world_pos.Y - parent_pos.Y,
                            world_pos.Z - parent_pos.Z };
    const hmm_vec3 unrot = QuatRotateVec(QuatConjugate(parent_rot), rel);
    const hmm_vec3 local = {
        parent_scl.X != 0.0f ? unrot.X / parent_scl.X : 0.0f,
        parent_scl.Y != 0.0f ? unrot.Y / parent_scl.Y : 0.0f,
        parent_scl.Z != 0.0f ? unrot.Z / parent_scl.Z : 0.0f,
    };
    SetLocalPos(local);
}

SAnimQuat TTransform::Rot() const
{
    // Trigger the recompute chain so F_CHANGED clears on this and any
    // ancestors that needed refresh. The actual quat is composed by
    // walking parents (matrix-extraction is hairy under non-uniform
    // scale; quat-walk is exact for the rotation component).
    Matrix();
    SAnimQuat q = LocalRot();
    if (parent_)
        q = QuatMul(parent_->Rot(), q);
    return NormalizeQuat(q);
}

void TTransform::SetRot(const SAnimQuat& world_rot)
{
    if (!parent_)
    {
        SetLocalRot(world_rot);
        return;
    }
    const SAnimQuat parent_rot = parent_->Rot();
    const SAnimQuat local = QuatMul(QuatConjugate(parent_rot), world_rot);
    SetLocalRot(local);
}

void TTransform::SetRotEuler(const hmm_vec3& euler_xyz)
{
    SetRot(QuatFromEulerXYZ(euler_xyz));
}

hmm_vec3 TTransform::Scl() const
{
    // Trigger recompute chain so F_CHANGED clears uniformly with the
    // other world-space queries; same lossyScale-style component-wise
    // composition up the chain (lossy under non-uniform scale +
    // rotation, matches Unity).
    Matrix();
    if (!parent_) return scl_;
    const hmm_vec3 ps = parent_->Scl();
    return { scl_.X * ps.X, scl_.Y * ps.Y, scl_.Z * ps.Z };
}

const hmm_mat4& TTransform::Matrix() const
{
    if (HasFlag(F_GLOBAL_MATRIX_DIRTY))
        RecomputeGlobalMatrix();
    return global_matrix_;
}

void TTransform::RefreshHierarchy()
{
    // Skip subtrees that have nothing to refresh. F_GLOBAL_MATRIX_DIRTY
    // means this node itself is stale; F_CHILDREN_DIRTY means some
    // descendant is. If both clear, nothing in the subtree needs work.
    if (!HasFlag(F_GLOBAL_MATRIX_DIRTY) && !HasFlag(F_CHILDREN_DIRTY))
        return;

    // Refresh self in the order the user described: local first
    // (rebuild from TRS if our own values changed), then global
    // (compose with parent's already-up-to-date global). Caller
    // contract is "parent is already refreshed", so reading
    // parent_->global_matrix_ directly is safe and avoids re-entering
    // the recursive lazy path.
    if (HasFlag(F_LOCAL_MATRIX_DIRTY))
        RecomputeLocalMatrix();

    if (HasFlag(F_GLOBAL_MATRIX_DIRTY))
    {
        if (parent_)
            MtxMultiply(&global_matrix_, &local_matrix_, &parent_->global_matrix_);
        else
            global_matrix_ = local_matrix_;
        flags_ &= ~uint32_t(F_GLOBAL_MATRIX_DIRTY);
    }
    flags_ &= ~uint32_t(F_CHANGED);
    flags_ &= ~uint32_t(F_CHILDREN_DIRTY);

    // Recurse. Children whose subtrees are clean (no dirty + no
    // children-dirty) early-out at the top of their own RefreshHierarchy.
    for (TTransform* c : children_)
        if (c) c->RefreshHierarchy();
}

// ---- Recompute paths ---------------------------------------------------

void TTransform::RecomputeLocalMatrix() const
{
    MtxClear(&local_matrix_);
    if (HasFlag(F_HAS_SCALE))
        MtxScale(&local_matrix_, &scl_);
    if (HasFlag(F_HAS_ROT))
    {
        hmm_mat4 rot;
        QuatToMat4(LocalRot(), &rot);
        MtxMultiply(&local_matrix_, &local_matrix_, &rot);
    }
    if (HasFlag(F_HAS_POS))
        MtxTranslate(&local_matrix_, &pos_);
    flags_ &= ~uint32_t(F_LOCAL_MATRIX_DIRTY);
}

void TTransform::RecomputeGlobalMatrix() const
{
    if (HasFlag(F_LOCAL_MATRIX_DIRTY))
        RecomputeLocalMatrix();

    if (parent_)
    {
        const hmm_mat4& p = parent_->Matrix();   // recursive; cached on success
        MtxMultiply(&global_matrix_, &local_matrix_, &p);
    }
    else
    {
        global_matrix_ = local_matrix_;
    }
    flags_ &= ~uint32_t(F_GLOBAL_MATRIX_DIRTY);
    flags_ &= ~uint32_t(F_CHANGED);
    // F_CHILDREN_DIRTY is intentionally NOT cleared here: recomputing
    // self's global tells us nothing about descendants. A top-down
    // sweep that visits the subtree and confirms it clean is the
    // place to clear it. Without a consumer the flag is sticky-on,
    // which is fine -- it's an early-out hint, not a correctness flag.
}

// ---- Flag propagation --------------------------------------------------

void TTransform::MarkLocalMatrixDirty()
{
    flags_ |= uint32_t(F_LOCAL_MATRIX_DIRTY);
}

void TTransform::MarkChildrenDirtyUp()
{
    // Walk parents, set F_CHILDREN_DIRTY; stop at the first ancestor
    // that already has it (the marker is monotonic upward).
    TTransform* p = parent_;
    while (p && !p->HasFlag(F_CHILDREN_DIRTY))
    {
        p->flags_ |= uint32_t(F_CHILDREN_DIRTY);
        p = p->parent_;
    }
}

void TTransform::MarkGlobalDirtyAndChangedDown()
{
    // Tag self if not already; if both flags are already set there's
    // no work for the subtree (a prior mark already covered it).
    const uint32_t both = uint32_t(F_GLOBAL_MATRIX_DIRTY) | uint32_t(F_CHANGED);
    if ((flags_ & both) == both)
        return;
    flags_ |= both;

    for (TTransform* c : children_)
        if (c)
            c->MarkGlobalDirtyAndChangedDown();
}
