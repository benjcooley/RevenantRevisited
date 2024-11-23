// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     helper.cpp - THelper module                       *
// *************************************************************************

#include "helper.h"

#include "3dimage.h"
#include "mappane.h"

REGISTER_BUILDER(THelper)

TObjectClass HelperClass("HELPER", OBJCLASS_HELPER, 0);

// *******************
// * Helper Animator *
// *******************

_CLASSDEF(THelperAnimator)
class THelperAnimator : public T3DAnimator
{
  public:
    THelperAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~THelperAnimator() { Close(); }
      // Call close function

    virtual void SetupObjects();
      // Sets up objects
    virtual bool Render();
      // Renders objects
};

REGISTER_3DANIMATOR("Box", THelperAnimator)

// Sets up the objects to animate (simply uses whatever objects are in the imagery)
// Override this function to set up whatever objects you need for your effect, etc.
void THelperAnimator::SetupObjects()
{
    PS3DAnimObj o = NewObject(0, OBJ3D_VERTS | OBJ3D_COPYVERTS | OBJ3D_POS1);
    AddObject(o);
}

bool THelperAnimator::Render()
{
    PS3DAnimObj obj = GetObject(0);

    ResetExtents();

    int32_t width, length, height;
    image->GetWorldBoundBox(state, width, length, height);

    obj->pos.X = -(float)((((float)width / 2.0) - (float)image->GetWorldRegX(state)) * (float)GRIDSIZE);
    obj->pos.Y = -(float)((((float)length / 2.0) - (float)image->GetWorldRegY(state)) * (float)GRIDSIZE);
    obj->pos.Z = -(float)((((float)height / 2.0) - (float)image->GetWorldRegZ(state)) * (float)GRIDSIZE);

    for (int32_t i = 0; i < obj->numverts; i++)
    {
        hmm_vec3 *v = &(((hmm_vec3 *)obj->verts)[i]);

        if (v->X)
        {
            v->X = (float)((float)width / 2.0 * (float)GRIDSIZE);
            if (v->X < 0)
                v->X *= -1;
        }
        if (v->Y)
        {
            v->Y = (float)((float)length / 2.0 * (float)GRIDSIZE);
            if (v->Y < 0)
                v->Y *= -1;
        }
        if (v->Z)
        {
            v->Z = (float)((float)height / 2.0 * (float)GRIDSIZE);
            if (v->Z < 0)
                v->Z *= -1;
        }
    }

    RenderObject(obj);

    UpdateExtents();
    
    return true;
}
