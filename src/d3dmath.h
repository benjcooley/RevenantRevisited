/*==========================================================================
 *
 *  Copyright (C) 1995, 1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File: d3dmath.h
 *
 ***************************************************************************/

#pragma once

#if 0

#ifndef __D3DMATH_H__
#define __D3DMATH_H__

#include <math.h>

/*
 * Normalises the vector v
 */
LPD3DVECTOR D3DVECTORNormalise(LPD3DVECTOR v);

/*
 * Calculates cross product of a and b.
 */
LPD3DVECTOR D3DVECTORCrossProduct(LPD3DVECTOR lpd, LPD3DVECTOR lpa, LPD3DVECTOR lpb);

/*
 * Clears matrix to the identity matrix.
 */
hmm_mat4* D3DMATRIXClear(hmm_mat4* d);

/*
 * lpDst = lpSrc1 * lpSrc2
 * lpDst can be equal to lpSrc1 or lpSrc2
 */
hmm_mat4* MultiplyD3DMATRIX(hmm_mat4* lpDst, hmm_mat4* lpSrc1, 
                              hmm_mat4* lpSrc2);


/*
 * Transform 3D vector by matrix
 */
void D3DMATRIXTransform(hmm_mat4* lpm, LPD3DVECTOR lpV, LPD3DVECTOR lpD);

/*
 * -1 d = a
 */
hmm_mat4* D3DMATRIXInvert(hmm_mat4* d, hmm_mat4* a);

/*
 * Set the rotation part of a matrix such that the vector lpD is the new
 * z-axis and lpU is the new y-axis.
 */
hmm_mat4* D3DMATRIXSetRotation(hmm_mat4* lpM, LPD3DVECTOR lpD, LPD3DVECTOR lpU);

/*
 * Translates points relative origin.
 */
hmm_mat4* D3DMATRIXTranslate(hmm_mat4* lpDst, LPD3DVECTOR lpV);

/*
 * Moves points relative to origin.  Inverse of translate
 * x, y, and z orign.
 */
hmm_mat4* D3DMATRIXMove(hmm_mat4* lpDst, LPD3DVECTOR lpV);

/*
 * Set the scale part of a matrix such that the vector lpV is the new
 * x, y, and z orign.
 */
hmm_mat4* D3DMATRIXScale(hmm_mat4* lpDst, LPD3DVECTOR lpV);

/*
 * Set the X rotation components of a matrix given the angle in radians.
 */
hmm_mat4* D3DMATRIXRotateX(hmm_mat4* lpDst, float a);

/*
 * Set the Y rotation components of a matrix given the angle in radians.
 */
hmm_mat4* D3DMATRIXRotateY(hmm_mat4* lpDst, float a);

/*
 * Set the Z rotation components of a matrix given the angle in radians.
 */
hmm_mat4* D3DMATRIXRotateZ(hmm_mat4* lpDst, float a);

/*
 * Calculates a point along a B-Spline curve defined by four points. p
 * n output, contain the point. t                                Position
 * along the curve between p2 and p3.  This position is a float between 0
 * and 1. p1, p2, p3, p4    Points defining spline curve. p, at parameter
 * t along the spline curve
 */
void spline(LPD3DVECTOR p, float t, LPD3DVECTOR p1, LPD3DVECTOR p2,
            LPD3DVECTOR p3, LPD3DVECTOR p4);

#endif
