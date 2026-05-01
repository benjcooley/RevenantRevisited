// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 charanimator.cpp - TCharAnimator module               *
// *************************************************************************

#include <math.h>

#include "character.h"
#include "player.h"
#include "charanimator.h"
#include "effect.h"
#include "math3d.h"
#include "render3d_types.h"

namespace
{
// Float-RGBA (0..1) -> packed ARGB8 uint32 (matches S3DLVertex::diffuse layout).
static inline uint32_t PackARGB(float r, float g, float b, float a)
{
    auto clamp8 = [](float f) -> uint32_t {
        if (f <= 0.0f) return 0u;
        if (f >= 1.0f) return 255u;
        return (uint32_t)(f * 255.0f + 0.5f);
    };
    return (clamp8(a) << 24) | (clamp8(r) << 16) | (clamp8(g) << 8) | clamp8(b);
}
}

REGISTER_3DANIMATOR("CHARACTER", TCharAnimator)
REGISTER_3DANIMATOR("PLAYER", TPlayerAnimator)

TCharAnimator::TCharAnimator(TObjectInstance* oi) : T3DAnimator(oi)
{
    InitPoisonColor();
    InitTransparency();
    InitUtilityImagery();

    visindicator_shiftval = 0.0f;
}

TCharAnimator::~TCharAnimator()
{
    CloseUtilityImagery();
    ClosePoisonColor();

    Close();
}

void TCharAnimator::Animate(bool draw)
{
    T3DAnimator::Animate(draw);
}

bool TCharAnimator::Render()
{
    // Programmers note: it's a good idea to put your functions down below, as
    // these functions here can get to be pretty confusing if you add all your 
    // crap to them.  BEN

    SetPoisonColor();       // Sets green color for character
    UpdateTransparency();   // Updates animator's transparency value based on char
    if (transparency < 0.01f)
        return true;        // Too dim to render

  // Unhide all objects (except for weapon/sword)
    for (int32_t c = 0; c < NumObjects(); c++)
    {
        S3DAnimObj* obj = GetObject(c);
        if (!stricmp(Get3DImagery()->GetObjectName(obj->objnum), "sword") ||
            !stricmp(Get3DImagery()->GetObjectName(obj->objnum), "weapon"))
            obj->flags |= OBJ3D_HIDE;
        else
            obj->flags &= ~OBJ3D_HIDE;
    }

    if (inst->ObjClass() == OBJCLASS_PLAYER)
        HideCharParts();        // Hides char parts replaced by equipment before rendering
    
  // Now reset transparency (if needed)
    if (abs(transparency - 1.0f) > 0.001f)
        SetMaterialTransparency(Get3DImagery());

    RenderShadow();

    T3DAnimator::Render();  // Renders character (minus hidden parts)

    if (inst->ObjClass() == OBJCLASS_CHARACTER)
        if (Player)
            if (Player->IsSneakMode())
                RenderVisionIndicator();

    RenderBloodyChunks();

  // Now reset transparency (if needed)
    if (abs(transparency - 1.0f) > 0.001f)
        ResetMaterialTransparency(Get3DImagery());

    if (inst->ObjClass() == OBJCLASS_PLAYER)
        RenderEquipment();      // Renders equipment geometry

//  RenderCombatFlashes();

    return true;
}


// ********************
// * Poison Functions *
// ********************

void TCharAnimator::InitPoisonColor()
{

  // Get poison material    
    origmatred = new float[MAXMATERIALS];
    origmatgreen = new float[MAXMATERIALS];
    oldpoison = 0;

    S3DMat mat;
    for (int32_t i = 0; i < Get3DImagery()->NumMaterials(); i++)
    {
        memset(&mat, 0, sizeof(S3DMat));
        Get3DImagery()->GetMaterial(i, &mat);

        S3DMaterial &m = mat.matdesc;
        origmatred[i] = m.ambient.r;
        origmatgreen[i] = m.ambient.g;
    }
}

void TCharAnimator::ClosePoisonColor()
{
    S3DMat mat;
    for (int32_t i = 0; i < Get3DImagery()->NumMaterials(); i++)
    {
        memset(&mat, 0, sizeof(S3DMat));
        Get3DImagery()->GetMaterial(i, &mat);

        S3DMaterial &m = mat.matdesc;
        m.ambient.r = origmatred[i];
        m.ambient.b = origmatgreen[i];

        Get3DImagery()->SetMaterial(i, &mat);
    }

    delete [] origmatred;
    delete [] origmatgreen;
}

// Sets poison color for character
void TCharAnimator::SetPoisonColor()
{
    int32_t poison = 9 * ((TCharacter*)inst)->Poisoned();

    if (poison != oldpoison)
    {
        S3DMat mat;

        float amount = ((float)10.0 - (float)poison) / (float)10.0;

        if (poison > 10)
            amount = 0.0;
        else if (poison < 0)
            amount = 1.0;

        for (int32_t i = 0; i < Get3DImagery()->NumMaterials(); i++)
        {
            memset(&mat, 0, sizeof(S3DMat));
            Get3DImagery()->GetMaterial(i, &mat);

            S3DMaterial &m = mat.matdesc;

            if (amount < 0.1)
            {
                m.ambient.r = origmatred[i];
                m.ambient.b = origmatgreen[i];
            }
            else
            {
                m.ambient.r = origmatred[i] * amount;
                m.ambient.b = origmatgreen[i] * amount;
            }

            Get3DImagery()->SetMaterial(i, &mat);
        }
    }
}

// *********************
// * Equipment Methods *
// *********************

#define PROCESSEQUIPMENT_HIDECHARPARTS    1
#define PROCESSEQUIPMENT_RENDEREQUIPPARTS 2

// Finds the weapon in the character object and hides it
void TCharAnimator::ProcessEquipment(int32_t task)
{
    if (inst->ObjClass() != OBJCLASS_PLAYER)
        return;

    S3DAnimObj equipobj;
    memset(&equipobj, 0, sizeof(S3DAnimObj));

    TPlayer* player = (TPlayer*)inst;

    for (int32_t eq = 0; eq < NUM_EQ_SLOTS; eq++)
    {
        TObjectInstance* oi = player->GetEquip(eq);
        if (!oi)
            continue;
            
        if (oi->GetImagery()->GetHeader()->imageryid != OBJIMAGE_MESH3D)
            continue;

        T3DImagery* equipimagery = (T3DImagery*)oi->GetImagery();
        SImageryHeader* equipheader = equipimagery->GetHeader();

      // Find equipment state for player's body type
        int32_t nlen = 0;
        int32_t st = 0;
        for (; st < equipheader->numstates; st++)
        {
          // Objects in this state are for all chars
            if (!stricmp(equipheader->states[st].animname, "still") ||
                !stricmp(equipheader->states[st].animname, "all"))
            {
                nlen = 0;
                break;
            }

          // Has specific state for this body type
            if (!stricmp(equipheader->states[st].animname, player->BodyType()))
            {
                nlen = strlen(player->BodyType());
                break;
            }
        }

      // No geometry for player's body type!  Exit (this should never happen!)
        if (st >= equipheader->numstates)
            continue;

      // Go through equip objects, and processes for matching player objects    
        for (int32_t o = 0; o < equipimagery->NumObjects(); o++)
        {
            if (equipimagery->IsHidden(o, st))
                continue;

            char *n = equipimagery->GetObjectName(o);

            if (nlen > 0 && !strnicmp(n, player->BodyType(), nlen))
                n += nlen;

            int32_t playerobjnum;

            if (eq == EQ_PRIMEHAND)     // If weapon slot
            {   
                if (((TCharacter*)inst)->IsBowMode())   // Not in combat mode.. don't replace
                    playerobjnum = -1;
                else
                {
                    playerobjnum = GetObjectNum("weapon");  // Find weapon or sword object to replace
                    if (playerobjnum < 0)
                        playerobjnum = GetObjectNum("sword");
                    if (playerobjnum < 0)
                        playerobjnum = GetObjectNum(n);     // All else fails, try object name
                }
            }
            else if (eq == EQ_RANGEDWEAPON)     // If bow slot
            {
                if (!((TCharacter*)inst)->IsBowMode())  // Not in bow mode.. don't replace
                    playerobjnum = -1;
                else
                {
                    playerobjnum = GetObjectNum("weapon");  // Find weapon or sword object to replace
                    if (playerobjnum < 0)
                        playerobjnum = GetObjectNum("sword");
                    if (playerobjnum < 0)
                        playerobjnum = GetObjectNum(n); // All else fails, try object name
                }
            }
            else    // Otherwise find the object name specified in the inventory object
                playerobjnum = GetObjectNum(n);

            if (playerobjnum < 0)
                continue;

            S3DAnimObj* playerobj = GetObject(playerobjnum);

          // Do the actual thing now
            if (task == PROCESSEQUIPMENT_HIDECHARPARTS)
            {
                playerobj->flags |= OBJ3D_HIDE;
            }
            else if (task == PROCESSEQUIPMENT_RENDEREQUIPPARTS)
            {
                equipobj.objnum = o;
                equipobj.flags = OBJ3D_MATRIX;

              // Copy the matrix for the weapon in the character, to the inventory item weapon!
                memcpy(&(equipobj.matrix), &(playerobj->matrix), sizeof(hmm_mat4));

              // Set transparency (if needed)
                if (abs(transparency - 1.0f) > 0.001f)
                    SetMaterialTransparency(equipimagery);

             // Render the inventory item weapon via it's own imagery object!!
                equipimagery->RenderObject(&equipobj, 0, 0, &matrix, -1, false);

              // Now reset transparency (if needed)
                if (abs(transparency - 1.0f) > 0.001f)
                    ResetMaterialTransparency(equipimagery);
            }
        }
    }
}

void TCharAnimator::HideCharParts()
{
    ProcessEquipment(PROCESSEQUIPMENT_HIDECHARPARTS);
}

void TCharAnimator::RenderEquipment()
{
    ProcessEquipment(PROCESSEQUIPMENT_RENDEREQUIPPARTS);
}

// ************************
// * Visibility Functions *
// ************************

#define MAX_TRANSPARENCY_GROW (0.05f)

void TCharAnimator::InitTransparency()
{
    float transparency = (float)((TCharacter*)inst)->Transparency() / 100.0f;
}

void TCharAnimator::UpdateTransparency()
{
    float t = (float)((TCharacter*)inst)->Transparency() / 100.0f;

    if (Editor || abs(t - transparency) < MAX_TRANSPARENCY_GROW)
        transparency = t;
    else if (t < transparency)
        transparency -= MAX_TRANSPARENCY_GROW;
    else
        transparency += MAX_TRANSPARENCY_GROW;
}

void TCharAnimator::SetMaterialTransparency(T3DImagery* img)
{
    for (int32_t c = 0; c < img->NumMaterials(); c++)
    {
        S3DMat m;
        img->GetMaterial(0, &m);
        m.matdesc.ambient.a = m.matdesc.diffuse.a =
            m.matdesc.specular.a = m.matdesc.emissive.a = transparency;
        img->SetMaterial(0, &m);
    }
}

void TCharAnimator::ResetMaterialTransparency(T3DImagery* img)
{
    for (int32_t c = 0; c < img->NumMaterials(); c++)
    {
        S3DMat m;
        img->GetMaterial(0, &m);
        m.matdesc.ambient.a = m.matdesc.diffuse.a =
            m.matdesc.specular.a = m.matdesc.emissive.a = 100.0f;
        img->SetMaterial(0, &m);
    }
}

// ***************************************************************
// * Utility stuff (shadow, combat flashes, bloody chunks, etc.) *
// ***************************************************************

void TCharAnimator::InitUtilityImagery()
{
    extern TObjectClass EffectClass;

    SObjectDef def;
    memset(&def, 0, sizeof(SObjectDef));

    def.objclass = OBJCLASS_EFFECT;
    def.objtype = EffectClass.FindObjType("CharUtility");
    SObjectInfo* info = EffectClass.GetObjType(def.objtype);
    if (info)
        utilityimagery = (T3DImagery*)TObjectImagery::LoadImagery(info->imageryid);
}

void TCharAnimator::CloseUtilityImagery()
{
    extern TObjectClass EffectClass;

    TObjectImagery::FreeImagery(utilityimagery);
}

S3DAnimObj* TCharAnimator::GetNewImObject(T3DImagery* imagery, int32_t objnum, int32_t flags)
{
    S3DAnimObj* obj = new S3DAnimObj;
    memset(obj, 0, sizeof(S3DAnimObj));
    obj->objnum = objnum;
    obj->parent = nullptr;

    S3DObj o;
    imagery->GetObject(objnum, &o);

    obj->animtrack = min(objnum, imagery->NumObjects() - 1);
    obj->primtype = ERender3DPrim::TriangleList;
    obj->verttype = ERender3DVertex::Vertex;
    obj->hmaterial = imagery->GetMaterialHandle(o.material);
    for (int32_t c = 0; c < MAXTEXTURES; c++)
        obj->htextures[c] = imagery->GetTextureHandle(c);

    if (flags & OBJ3D_COPYVERTS)
        GetImVerts(imagery, obj, obj->verttype);

    if (flags & OBJ3D_COPYFACES)
        GetImFaces(imagery, obj);

    return obj;
}

void TCharAnimator::GetImVerts(T3DImagery* imagery, S3DAnimObj* obj, ERender3DVertex verttype)
{
    obj->flags |= (OBJ3D_VERTS | OBJ3D_COPYVERTS | OBJ3D_OWNSVERTS);
    obj->verttype = verttype;

    obj->numverts = imagery->NumObjVerts(obj->objnum);
    if (obj->verttype == ERender3DVertex::Vertex)
        obj->verts = new S3DVertex[obj->numverts];
    else if (obj->verttype == ERender3DVertex::LitVertex)
        obj->verts = new S3DLVertex[obj->numverts];
    else if (obj->verttype == ERender3DVertex::TLVertex)
        obj->verts = new S3DTLVertex[obj->numverts];
    imagery->GetObjVerts(obj->objnum, obj->verts, 0, 0, obj->verttype);
}

void TCharAnimator::GetImFaces(T3DImagery* imagery, S3DAnimObj* obj)
{
    obj->flags |= (OBJ3D_FACES | OBJ3D_COPYFACES | OBJ3D_OWNSFACES);

    obj->numfaces = imagery->NumObjFaces(obj->objnum);
    obj->faces = new S3DFace[obj->numfaces];
    imagery->GetObjFaces(obj->objnum, obj->faces);
}

void TCharAnimator::RenderShadow()
{
    S3DAnimObj* obj;
    obj = GetNewImObject(utilityimagery, 0, OBJ3D_COPYVERTS | OBJ3D_COPYFACES);
    /*for(int32_t i = 0; i < obj->numverts; i++)
    {
        ((S3DLVertex*)obj->verts)[i].diffuse = PackARGB(1.0f, 1.0f, 1.0f, 1.0f);
    }*/
    
    hmm_mat4 pos;
    MtxClear(&pos);
    MtxClear(&obj->matrix);

  // Scale shadow based on radius
    hmm_vec3 scl;
    scl.X = scl.Y = scl.Z = (float)((TCharacter*)inst)->Radius() / 24.0f;
    MtxScale(&obj->matrix, &scl);

  // Add in char's position
    hmm_vec3 v;
    S3DPoint charpos;
    inst->GetPos(charpos);

#define SHADOW_OFFSET   5.0f
    v.X = (float)(charpos.x + SHADOW_OFFSET * 2);
    v.Y = (float)(charpos.y + SHADOW_OFFSET * 2);
    v.Z = (float)FIX_Z_VALUE(charpos.z) + (float)SHADOW_OFFSET;
    MtxTranslate(&obj->matrix, &v);
    obj->flags |= OBJ3D_MATRIX | OBJ3D_FACES | OBJ3D_VERTS | OBJ3D_OWNSFACES | OBJ3D_OWNSVERTS;
    
    // Set start (offset, usually 0!!) and number of faces for texture[0 no texture]
    obj->texfaces[0] = 0;
    obj->numtexfaces[0] = 0;//obj->numfaces;
    obj->texfaces[1] = 0;
    obj->numtexfaces[1] = obj->numfaces;

    SaveBlendState();
    SetBlendState();
    utilityimagery->RenderObject(obj, 0, 0, &pos);
    RestoreBlendState();
}

void TCharAnimator::RenderCombatFlashes()
{
    int32_t frame = ((TCharacter*)inst)->GetCombatFlashTicks();
    if (!frame)
        return;

    S3DAnimObj* obj;
    obj = GetNewImObject(utilityimagery, 1, OBJ3D_COPYVERTS | OBJ3D_COPYFACES);
    /*for(int32_t i = 0; i < obj->numverts; i++)
    {
        ((S3DLVertex*)obj->verts)[i].diffuse = PackARGB(1.0f, 1.0f, 1.0f, 1.0f);
    }*/
    
    hmm_mat4 pos;
    MtxClear(&pos);
    MtxClear(&obj->matrix);

    hmm_vec3 scl;
    scl.X = scl.Y = scl.Z = 0.6f;
    MtxScale(&obj->matrix, &scl);
    MtxRotateX(&obj->matrix, -(float)(90 * TORADIAN));
    MtxRotateZ(&obj->matrix, -(float)(M_PI / 4.0f));
    // spin 45 degrees every other frame
    MtxRotateY(&obj->matrix, -(float)(45 * TORADIAN * (frame & 1)));
        
    hmm_vec3 v;
    S3DPoint charpos;
    inst->GetPos(charpos);

#define CF_OFFSET   25.0f
#define CF_HEIGHT   50.0f
    v.X = (float)(charpos.x + CF_OFFSET * 2);
    v.Y = (float)(charpos.y + CF_OFFSET * 2);
    v.Z = (float)FIX_Z_VALUE(charpos.z) + (float)(CF_OFFSET + CF_HEIGHT);
    MtxTranslate(&obj->matrix, &v);
    obj->flags |= OBJ3D_MATRIX | OBJ3D_FACES | OBJ3D_VERTS | OBJ3D_OWNSFACES | OBJ3D_OWNSVERTS;
    
    // Set start (offset, usually 0!!) and number of faces for texture[0]
    // these aren't correct (it's drawing a shadow)
    obj->texfaces[0] = 0;
    obj->numtexfaces[0] = 0;
    obj->texfaces[1] = 0;
    obj->numtexfaces[1] = 0;
    obj->texfaces[2] = 0;
    obj->numtexfaces[2] = obj->numfaces;

    SaveBlendState();
    SetBlendState();
    utilityimagery->RenderObject(obj, 0, 0, &pos);
    RestoreBlendState();
}

void TCharAnimator::RenderBloodyChunks()
{
}

void TCharAnimator::RenderVisionIndicator()
{
    if (!(((TCharacter*)inst)->CanSeeCharacter(Player, -1)))
    {
        S3DAnimObj* obj;
        S3DPoint charpos;
        hmm_vec3 translate_vector, head_pos;
        hmm_vec3 scl;
        hmm_mat4 pos;
        //hmm_mat4 head_matrix;
        float facing_angle;
        float tmp_shiftval;

        obj = GetNewImObject(utilityimagery, 2, OBJ3D_COPYVERTS | OBJ3D_COPYFACES);

        obj->flags |= OBJ3D_MATRIX | OBJ3D_FACES | OBJ3D_VERTS | OBJ3D_OWNSFACES | OBJ3D_OWNSVERTS;
    
#if 1
//      MtxClear(&head_matrix);

        GetObjectMatrix(GetObjectNum("head"), &pos);

        // extract the translation info from the head object's matrix
        head_pos.X = pos.Elements[3][0];
        head_pos.Y = pos.Elements[3][1];
        head_pos.Z = pos.Elements[3][2];

#endif

        MtxClear(&obj->matrix);
        MtxClear(&pos);

        inst->GetPos(charpos);

        // get the angle that the character is facing in radians, converted from a 255-degree range
        facing_angle = ((float)inst->GetFace()) * ((float)M_PI / 127.0f);

        // scale first: base it on the character's maximum sight range
        scl.Y = 128.0f / (float)(((TCharacter*)inst)->GetCharData())->sightangle;
        scl.X = ((float)(((TCharacter*)inst)->GetCharData())->sightmax / 100.0f) + 1.0f;
        MtxScale(&obj->matrix, &scl);


        MtxRotateZ(&obj->matrix, (-90.0f * (float)TORADIAN));


        // translate next: put it in front of the character
        translate_vector.Y = -50.0f;
        translate_vector.X = translate_vector.Z = 0.0f;
        MtxTranslate(&obj->matrix, &translate_vector);


        // rotate last: aim it in the direction they are facing
        MtxRotateZ(&obj->matrix, facing_angle);


        // now place it where the character's head is...
        translate_vector.X = (float)charpos.x + head_pos.X;
        translate_vector.Y = (float)charpos.y + head_pos.Y;
        translate_vector.Z = (float)charpos.z + head_pos.Z;
        MtxTranslate(&obj->matrix, &translate_vector);

        // Set start (not an offset... starts at 1) and number of faces for texture[0]
        obj->texfaces[0] = 0;
        obj->texfaces[1] = 0;
        obj->texfaces[2] = 0;
        obj->texfaces[3] = 0;

        obj->numtexfaces[0] = 0;
        obj->numtexfaces[1] = 0;
        obj->numtexfaces[2] = 0;
        obj->numtexfaces[3] = obj->numfaces;


        // offset the texture u-coordinate so that the visor changes color appropriately
        tmp_shiftval = (float)(((TCharacter*)inst)->LastGlimpse() / 100.0f);
        if (tmp_shiftval > 1.0f) tmp_shiftval = 1.0f;
        if (tmp_shiftval < 0.0f) tmp_shiftval = 0.0f;

        if (visindicator_shiftval < tmp_shiftval) visindicator_shiftval += 0.033f;
        else visindicator_shiftval -= 0.033f;
        if (visindicator_shiftval > 0.999f) visindicator_shiftval = 0.999f;
        if (visindicator_shiftval < 0.001f) visindicator_shiftval = 0.001f;

        for(int32_t k = 0; k < obj->numverts; k++)
        {
            ((S3DLVertex*)obj->verts)[k].tu = visindicator_shiftval;
        }


        SaveBlendState();
        SetBlendState();
        utilityimagery->RenderObject(obj, 0, 0, &pos);
        RestoreBlendState();
    }
}

