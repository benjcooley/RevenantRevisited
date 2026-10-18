// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   intworld.h - integer views of a node's transform for code written   *
// *                against retail's integer world                         *
// *************************************************************************
//
// Retail kept an object's position as an S3DPoint of world units and its
// orientation as three 0..255 angles. The port keeps one spatial store,
// TNode's TTransform, and nothing else. The functions here convert between
// the two for the gameplay code that still thinks in integers: they are
// expressions over the transform, not accessors, and they are deliberately
// free functions with an Int prefix so the legacy unit is visible at every
// call site. New systems take the transform directly.
//
// Rounding: a float holds every integer below 2^24 exactly, far beyond any
// world coordinate, so an integer written through SetIntPos reads back
// unchanged. A node under a parent can sit at a fractional world position;
// IntPos rounds it to the nearest unit.
//
// Angles: 256 units per turn. The transform stores the euler triple
// verbatim (TTransform::SetLocalRotEuler), so the 0..255 values round-trip
// exactly as well.
//
// *************************************************************************

#pragma once

#include "engine/scene/node.h"
#include "engine/core/revtypes.h"
#include "engine/core/saferef.h"

#include <cmath>
#include <cstdint>
#include <memory>

constexpr float kIntAngleToRad = float(M_PI * 2.0 / 256.0);

[[nodiscard]] inline float IntAngleToRad(int32_t angle)
{
    return float(angle & 255) * kIntAngleToRad;
}

[[nodiscard]] inline int32_t RadToIntAngle(float rad)
{
    return int32_t(std::lround(rad / kIntAngleToRad)) & 255;
}

[[nodiscard]] inline hmm_vec3 ToWorld(const S3DPoint& p)
{
    return hmm_vec3{{ float(p.x), float(p.y), float(p.z) }};
}

[[nodiscard]] inline S3DPoint ToIntPos(const hmm_vec3& v)
{
    return S3DPoint(int32_t(std::lround(v.X)), int32_t(std::lround(v.Y)), int32_t(std::lround(v.Z)));
}

// ---- Position --------------------------------------------------------------

[[nodiscard]] inline S3DPoint IntPos(const TNode& node)  { return ToIntPos(node.Transform().Pos()); }
[[nodiscard]] inline S3DPoint IntPos(const TNode* node)  { return IntPos(*node); }
template <class T, class D> [[nodiscard]] inline S3DPoint IntPos(const std::unique_ptr<T, D>& node) { return IntPos(node.get()); }
template <class T> [[nodiscard]] inline S3DPoint IntPos(const TSafeRef<T>& node)        { return IntPos(node.Get()); }

// Writes the world position straight into the transform: no sector,
// walkmap or notification side effects (those are TObjectInstance::SetPos).
inline void SetIntPos(TNode& node, const S3DPoint& p) { node.Transform().SetPos(ToWorld(p)); }
inline void SetIntPos(TNode* node, const S3DPoint& p) { SetIntPos(*node, p); }
template <class T, class D> inline void SetIntPos(const std::unique_ptr<T, D>& node, const S3DPoint& p) { SetIntPos(node.get(), p); }
template <class T> inline void SetIntPos(const TSafeRef<T>& node, const S3DPoint& p)        { SetIntPos(node.Get(), p); }

// ---- Orientation -------------------------------------------------------------

[[nodiscard]] inline int32_t IntRotateX(const TNode& node) { return RadToIntAngle(node.Transform().LocalRotEuler().X); }
[[nodiscard]] inline int32_t IntRotateY(const TNode& node) { return RadToIntAngle(node.Transform().LocalRotEuler().Y); }
[[nodiscard]] inline int32_t IntFacing(const TNode& node)  { return RadToIntAngle(node.Transform().LocalRotEuler().Z); }
[[nodiscard]] inline int32_t IntRotateX(const TNode* node) { return IntRotateX(*node); }
[[nodiscard]] inline int32_t IntRotateY(const TNode* node) { return IntRotateY(*node); }
[[nodiscard]] inline int32_t IntFacing(const TNode* node)  { return IntFacing(*node); }
template <class T, class D> [[nodiscard]] inline int32_t IntFacing(const std::unique_ptr<T, D>& node) { return IntFacing(node.get()); }
template <class T> [[nodiscard]] inline int32_t IntFacing(const TSafeRef<T>& node)        { return IntFacing(node.Get()); }

inline void SetIntRotateX(TNode& node, int32_t angle)
{
    hmm_vec3 e = node.Transform().LocalRotEuler();
    e.X = IntAngleToRad(angle);
    node.Transform().SetLocalRotEuler(e);
}
inline void SetIntRotateY(TNode& node, int32_t angle)
{
    hmm_vec3 e = node.Transform().LocalRotEuler();
    e.Y = IntAngleToRad(angle);
    node.Transform().SetLocalRotEuler(e);
}
inline void SetIntFacing(TNode& node, int32_t angle)
{
    hmm_vec3 e = node.Transform().LocalRotEuler();
    e.Z = IntAngleToRad(angle);
    node.Transform().SetLocalRotEuler(e);
}
inline void SetIntRotateX(TNode* node, int32_t angle) { SetIntRotateX(*node, angle); }
inline void SetIntRotateY(TNode* node, int32_t angle) { SetIntRotateY(*node, angle); }
inline void SetIntFacing(TNode* node, int32_t angle)  { SetIntFacing(*node, angle); }
template <class T, class D> inline void SetIntFacing(const std::unique_ptr<T, D>& node, int32_t angle) { SetIntFacing(node.get(), angle); }
template <class T> inline void SetIntFacing(const TSafeRef<T>& node, int32_t angle)        { SetIntFacing(node.Get(), angle); }
