// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 charanimator.cpp - TCharAnimator module               *
// *************************************************************************

#include <cmath>

#include "character.h"
#include "player.h"
#include "charanimator.h"
#include "effect.h"
#include "math3d.h"
#include "render3d_types.h"
#include "time.h"

namespace
{
// REVSYNC: TCharAnimator::Render @ 0x004d7a50 -- the drawn alpha.
constexpr float kTransparencyScale   = 0.01f;   // Transparency() 0..100 to alpha (@0x005a350c)
constexpr float kTransparencyStep    = 0.05f;   // alpha moved per drawn frame (@0x005a7e9c)
constexpr float kMinDrawTransparency = 0.01f;   // below this nothing draws (@0x005a3524)
// Retail drew at most one frame per 24 Hz timer tick, so its 0.05 a frame is
// 1.2 a second. The port moves the alpha by time, at that rate, whatever the
// frame rate: a full fade takes 1/1.2 = 0.83 s, as the 20-pulse script fade
// (TCharacter::UpdateFade, 5 a pulse) does.
constexpr float kTransparencyPerSecond = kTransparencyStep * float(TTime::LegacyFramerate);

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

// REVSYNC: TCharAnimator::Animate @ 0x004d7a00 ticks the rising numbers
// (0x004da6f0) once a frame, paralysed or iced too; the port's per-tick
// animator call is Pulse. Each live number loses a tick: with none left it
// goes, else it rises kRisingNumberRise.
void TCharAnimator::Pulse()
{
    T3DAnimator::Pulse();
    for (SRisingNumber& n : numbers)
    {
        if (n.ticks <= 0)
            continue;
        if (--n.ticks <= 0)
            n = SRisingNumber{};
        else
            n.rise += kRisingNumberRise;
    }
}

void TCharAnimator::ShowNumber(int32_t amount, int32_t x, int32_t y, uint32_t rgb)
{
    static uint32_t serials = 0;
    for (size_t i = 1; i < numbers.size(); ++i)
        if (numbers[i].ticks <= 0)
        {
            numbers[i] = SRisingNumber{amount, rgb, x, y, kRisingNumberTicks, 0, ++serials};
            return;
        }
}

bool TCharAnimator::Render()
{
    // Programmers note: it's a good idea to put your functions down below, as
    // these functions here can get to be pretty confusing if you add all your 
    // crap to them.  BEN

    SetPoisonColor();       // Sets green color for character
    UpdateDrawState(TTime::LegacyFrameSeconds);   // this path draws once a 24 Hz frame;
                                                  // hides the replaced objects
    if (DrawAlpha() <= 0.0f)
        return true;        // Invisible, or too dim to render

  // Now reset transparency (if needed)
    if (std::fabs(transparency - 1.0f) > 0.001f)
        SetMaterialTransparency(Get3DImagery());

    RenderShadow();

    T3DAnimator::Render();  // Renders character (minus hidden parts)

    if (inst->ObjClass() == OBJCLASS_CHARACTER)
        if (Player)
            if (Player->IsSneakMode())
                RenderVisionIndicator();

    RenderBloodyChunks();

  // Now reset transparency (if needed)
    if (std::fabs(transparency - 1.0f) > 0.001f)
        ResetMaterialTransparency(Get3DImagery());

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

// REVSYNC: TCharAnimator::Render @ 0x004d7a50 (0x004d7b0b..0x004d7b87) and
// ProcessEquipment @ 0x004d7f20: its task 1 (hide), and the choice its
// task 2 draws by.
//
// Render first shows every object of the character but its own "sword" and
// "weapon". For a player, every equipped item whose imagery is a mesh
// (header type 1), and every object of that imagery:
//   - a '*' or '@' first in the name is dropped; '@' leaves the character's
//     object showing under the item's;
//   - the player's body type first in the name is dropped (_strnicmp), so
//     "*normalchest" is "chest", and the other body types' objects
//     ("*largechest") match nothing;
//   - the prime hand draws outside bow mode, the ranged weapon only in it,
//     both on the character's "weapon", else its "sword", else the object
//     the name gives; any other slot on the object the name gives;
//   - with no such character object, the item's object doesn't draw.
// The item's object draws with that character object's matrix
// (OBJ3D_MATRIX: its own keys and states play no part) and the
// character's alpha. Retail's paperdoll draw (+0x1a4) also leaves the
// ranged weapon out; the map never draws that way.
void TCharAnimator::UpdateEquipment()
{
    T3DImagery* img = Get3DImagery();
    for (int32_t c = 0; c < NumObjects(); c++)
        if (S3DAnimObj* obj = GetObject(c))
        {
            const char* name = img->GetObjectName(obj->objnum);
            if (!stricmp(name, "sword") || !stricmp(name, "weapon"))
                obj->flags |= OBJ3D_HIDE;
            else
                obj->flags &= ~OBJ3D_HIDE;
        }

    equipment.clear();
    if (inst->ObjClass() != OBJCLASS_PLAYER)
        return;

    TPlayer* player = static_cast<TPlayer*>(inst);
    const char* bodytype = player->BodyType();
    const size_t bodylen = strlen(bodytype);
    const bool bowmode = player->IsBowMode();
    for (int32_t slot = 0; slot < NUM_EQ_SLOTS; slot++)
    {
        TObjectInstance* item = player->GetEquip(slot);
        if (!item || (slot == EQ_PRIMEHAND && bowmode) || (slot == EQ_RANGEDWEAPON && !bowmode))
            continue;
        TObjectImagery* imagery = item->GetImagery();
        const SImageryHeader* header = imagery ? imagery->GetHeader() : nullptr;
        if (!header || header->imageryid != OBJIMAGE_MESH3D)
            continue;
        T3DImagery* mesh = static_cast<T3DImagery*>(imagery);

        for (int32_t o = 0; o < mesh->NumObjects(); o++)
        {
            const char* name = mesh->GetObjectName(o);
            bool replaces = true;
            if (name[0] == '*' || name[0] == '@')
                replaces = *name++ == '*';
            if (!strnicmp(name, bodytype, bodylen))
                name += bodylen;

            int32_t bone = -1;
            if (slot == EQ_PRIMEHAND || slot == EQ_RANGEDWEAPON)
            {
                bone = GetObjectNum("weapon");
                if (bone < 0)
                    bone = GetObjectNum("sword");
            }
            if (bone < 0)
                bone = GetObjectNum(name);
            if (bone < 0)
                continue;

            if (replaces)
                GetObject(bone)->flags |= OBJ3D_HIDE;
            equipment.push_back({TSafeRef<TObjectInstance>(item), o, bone});
        }
    }
}

// REVSYNC: ProcessEquipment @ 0x004d7f20, task 2 -- the legacy draw: each
// item object through its own imagery, state 0 frame 0, with the matrix of
// the character object it rides.
void TCharAnimator::RenderEquipment()
{
    const bool translucent = std::fabs(transparency - 1.0f) > 0.001f;
    S3DAnimObj object;
    for (const SEquipAttachment& attachment : equipment)
    {
        TObjectInstance* item = attachment.item.Get();
        const S3DAnimObj* bone = GetObject(attachment.bone);
        if (!item || !bone)
            continue;
        T3DImagery* mesh = static_cast<T3DImagery*>(item->GetImagery());

        object.objnum = attachment.object;
        object.flags = OBJ3D_MATRIX;
        object.matrix = bone->matrix;

        if (translucent)
            SetMaterialTransparency(mesh);
        mesh->RenderObject(&object, 0, 0, &matrix, -1, false);
        if (translucent)
            ResetMaterialTransparency(mesh);
    }
}

// ************************
// * Visibility Functions *
// ************************

// REVSYNC: TCharAnimator::Render @ 0x004d7a50 tests OF_INVISIBLE (flags bit
// 0x80) unless the editor (DAT_00668154) is running.
bool TCharAnimator::IsHidden() const
{
    return (inst->GetFlags() & OF_INVISIBLE) && !Editor;
}

// REVSYNC: TCharAnimator::TCharAnimator @ 0x004d76b0 (at 0x004d77b2) seeds
// the drawn alpha from Transparency() * 0.01 and resets the materials' alpha
// to 1. (1998 assigned a local, leaving the member unset; retail's reset loop
// writes material 0 every pass, which the port doesn't copy.)
void TCharAnimator::InitTransparency()
{
    transparency = float(static_cast<TCharacter*>(inst)->Transparency()) * kTransparencyScale;
    ResetMaterialTransparency(Get3DImagery());
}

// REVSYNC: TCharAnimator::Render @ 0x004d7a50, the head. An OF_INVISIBLE
// character isn't drawn and its alpha holds; otherwise the alpha steps
// toward Transparency(). Retail runs it once per drawn frame.
//
// Retail gives a character no animator while it is OF_INVISIBLE:
// TMapPane::AnimateObjects (0x00458750) skips invisible objects, so the
// animator, and its alpha seeded from Transparency(), comes into being on
// the first frame the character shows. Port characters keep a permanent
// animator (TCharacter::IsAnimatorPermanent), so the alpha is seeded again
// on that frame instead. That's what lets `player.toggle invisible` +
// `fadecharacterin player` fade Locke in from nothing.
//
// A drawn frame also settles which objects show and what the equipment
// draws (UpdateEquipment), as Render does next in retail.
void TCharAnimator::UpdateDrawState(double dt_seconds)
{
    if (IsHidden())
    {
        reseed_transparency = true;
        return;
    }
    if (reseed_transparency)
    {
        InitTransparency();
        reseed_transparency = false;
    }
    else
        UpdateTransparency(dt_seconds);
    UpdateEquipment();
}

float TCharAnimator::DrawAlpha() const
{
    if (IsHidden() || transparency < kMinDrawTransparency)
        return 0.0f;
    return transparency;
}

// REVSYNC: TCharAnimator::Render @ 0x004d7a50 -- steps 0.05 toward
// Transparency() * 0.01, or lands on it once within a step; the editor
// lands on it at once. Time-based here: kTransparencyPerSecond.
void TCharAnimator::UpdateTransparency(double dt_seconds)
{
    const float target = float(static_cast<TCharacter*>(inst)->Transparency()) * kTransparencyScale;
    const float step = kTransparencyPerSecond * float(dt_seconds);

    if (Editor || std::fabs(target - transparency) <= step)
        transparency = target;
    else if (target < transparency)
        transparency -= step;
    else
        transparency += step;
}

// REVSYNC: TCharAnimator::SetMaterialTransparency @ 0x004d82a0 -- every
// material's ambient/diffuse/specular/emissive alpha (1998 wrote material 0
// on every pass). Retail also picks the blend state here; the port's
// renderer picks it from the alpha the character is submitted with.
void TCharAnimator::SetMaterialTransparency(T3DImagery* img)
{
    for (int32_t c = 0; c < img->NumMaterials(); c++)
    {
        S3DMat m;
        img->GetMaterial(c, &m);
        m.matdesc.ambient.a = m.matdesc.diffuse.a =
            m.matdesc.specular.a = m.matdesc.emissive.a = transparency;
        img->SetMaterial(c, &m);
    }
}

// Every material's alpha back to 1 (1998 wrote 100 to material 0 only).
void TCharAnimator::ResetMaterialTransparency(T3DImagery* img)
{
    for (int32_t c = 0; c < img->NumMaterials(); c++)
    {
        S3DMat m;
        img->GetMaterial(c, &m);
        m.matdesc.ambient.a = m.matdesc.diffuse.a =
            m.matdesc.specular.a = m.matdesc.emissive.a = 1.0f;
        img->SetMaterial(c, &m);
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

