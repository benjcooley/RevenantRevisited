// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           weapontrail.cpp - parked weapon-swipe code                 *
// *************************************************************************
//
// TODO(weapontrail): re-implement as TWeaponTrailComponent picked up by
// the renderer's submit pass. The legacy code below is the 1998-era
// strip renderer (TWeaponSwipe + TCharAnimator weapon-extents helpers)
// pulled out of charanimator.{h,cpp} when TCharAnimator stopped owning
// rendering. It is gated under `#if 0` so this file builds as nothing,
// and the migration target is local: lift the math identified below
// into a TObjectComponent that the character/player attaches, with the
// renderer emitting a strip drawable from its ring buffer.
//
// What carries over to the new TWeaponTrailComponent:
//   * GetWeaponImagery / GetWeaponNum -- find the equipped weapon's
//     mesh + objnum on the character.
//   * GetWeaponVertices / GetWeaponNumVerts -- pull the vertex array
//     of the weapon mesh, used to compute hilt/tip extents.
//   * TWeaponSwipe::GetWeaponExtents -- "scan vertices, take the two
//     with extreme positions along the weapon axis" -- selects hilt
//     vs. tip from the raw mesh.
//   * The (points[0], points[1]) ring-buffer shape (length maxsegs,
//     CycleStrip drops the oldest segment each frame so the trail
//     fades).
//   * TWeaponSwipe::NormalizeColors -- per-channel max-normalization
//     for the swipe color.
//   * Weapon-change detection via primehand string compare (resets
//     the strip when the equipped weapon changes).
//
// What does NOT carry over (the new submit path does these):
//   * GenerateStrip's S3DLVertex / face packing.
//   * Render()'s SaveBlendState/SetBlendState/RenderObject calls.
//   * GetCharsWeaponMatrix's reach into T3DAnimator::MakeMatrix +
//     T3DImagery::CalcObjectMatrix -- replaced by SampleI3DAnimPose +
//     BuildAnimPoseObjectMatrix (see src/meshextract.cpp).
//
// *************************************************************************

// =========================================================================
// Originally in charanimator.h:
// =========================================================================
//
//     _CLASSDEF(TWeaponSwipe)
//
//     _STRUCTDEF(SWeaponSwipeParams)
//     struct SWeaponSwipeParams
//     {
//         S3DLVertex* weaponverts;        // vertices of weapon, used for extents
//         int32_t numverts;               // number of vertices in weaponverts
//         TCharAnimator* charanim;        // animator of character this swipe is for
//         float r, g, b;                  // color (to be normalized) of swipe
//         int32_t maxsegs;                // number of segments / length of swipe
//         int32_t smooth;                 // triangles per frame such that maxsegs % smooth == 0
//         const char* primehand;          // using this weapon (test for weapon change)
//     };
//
//     class TWeaponSwipe
//     {
//       private:
//         S3DAnimObj theobj;
//         S3DAnimObj* obj;
//         int32_t maxpoints, maxverts;    // max points we can have, max verts
//         hmm_vec3 *points[2];            // points[0] = hilt, points[1] = tip
//         hmm_vec3 vweapbeg, vweapend;    // hilt/tip vertex positions on weapon
//         hmm_mat4* weaponmat;            // weapon translation matrix
//         bool initialized;
//         // duplicated from SWeaponSwipeParams
//         S3DLVertex* weaponverts;
//         int32_t numverts;
//         TCharAnimator* charanim;
//         float r, g, b;
//         int32_t maxsegs;
//         int32_t smooth;
//         const char* primehand;
//
//       public:
//         TWeaponSwipe() { initialized = false; maxpoints = 0; primehand = nullptr; }
//         virtual ~TWeaponSwipe() { }
//
//         void Init(SWeaponSwipeParams* p);
//         void GenerateStrip();
//         void Animate();
//         void CycleStrip();
//         void GetWeaponExtents();
//         void NormalizeColors();
//         void ChangeColor(float r, float g, float b);
//         void Render();
//         void Close();
//         hmm_mat4* GetCharsWeaponMatrix();
//         bool GetInitialized() const { return initialized; }
//         TCharAnimator* GetCharAnim() const { return charanim; }
//     };
//
//     // Inside TCharAnimator (public):
//         TWeaponSwipe* GetWeaponSwipe() { return &weaponswipe; }
//         void SetupWeaponSwipe();
//         S3DLVertex* GetWeaponVertices(int32_t len);
//         int32_t GetWeaponNumVerts();
//         T3DImagery* GetWeaponImagery(int32_t objnum);
//         int32_t GetWeaponNum();
//
//     // TCharAnimator member (private/protected):
//         TWeaponSwipe weaponswipe;
//
// =========================================================================
// Integration call sites (original locations):
// =========================================================================
//
// src/charanimator.cpp -- TCharAnimator::~TCharAnimator (after the
// CloseUtilityImagery / ClosePoisonColor calls):
//
//     weaponswipe.Close();
//
// src/charanimator.cpp -- TCharAnimator::Animate(bool draw):
//
//     if (!weaponswipe.GetInitialized())
//         SetupWeaponSwipe();
//     T3DAnimator::Animate(draw);
//
// src/charanimator.cpp -- TCharAnimator::Render() (right before the
// commented-out RenderCombatFlashes()):
//
//     if (weaponswipe.GetInitialized() &&                      // Ready
//       ((TCharacter*)inst)->IsAttack() &&                     // Is an attack
//       !(inst->ObjClass() == OBJCLASS_PLAYER &&               // Not hand-to-hand
//         ((TPlayer*)inst)->PrimeHand() == nullptr ))
//           weaponswipe.Render();
//
// src/character.cpp -- TCharacter::Pulse, just before "This code below
// SHOULD be in ResolveInvoke()":
//
//     TCharAnimator* anim = (TCharAnimator*)GetAnimator();
//     if (anim)
//     {
//         TWeaponSwipe* weaponswipe = anim->GetWeaponSwipe();
//         if (weaponswipe && weaponswipe->GetInitialized())
//             weaponswipe->Animate();
//     }
//
// =========================================================================
// Implementations from charanimator.cpp:
// =========================================================================
//
// (The block below is verbatim from src/charanimator.cpp pre-deletion.
//  Includes the TCharAnimator weapon-extents helpers ABOVE the
//  TWeaponSwipe member definitions.)

#if 0  // attic-only -- never compiled

// *****************************************
// * WeaponSwipe Functions and Attachments *
// *****************************************

T3DImagery* TCharAnimator::GetWeaponImagery(int32_t objnum)
{
    if (objnum == -1)
        return nullptr;
    if (inst->ObjClass() != OBJCLASS_PLAYER)
        return nullptr;

    TPlayer* player = (TPlayer*)inst;
    TObjectInstance* oi = player->GetEquip(EQ_PRIMEHAND);
    if (!oi)
        return nullptr;

//      if (oi->GetImagery()->ImageryId() != OBJIMAGE_MESH3D)
//          return 0;

    T3DImagery* equipimagery = (T3DImagery*)oi->GetImagery();

    return equipimagery;
}

int32_t TCharAnimator::GetWeaponNum()
{
    if (inst->ObjClass() != OBJCLASS_PLAYER)
        return -1;

    S3DAnimObj equipobj;
    memset(&equipobj, 0, sizeof(S3DAnimObj));

    TPlayer* player = (TPlayer*)inst;
    TObjectInstance* oi = player->GetEquip(EQ_PRIMEHAND);
    if (!oi)
        return -1;
            
//      if (oi->GetImagery()->ImageryId() != OBJIMAGE_MESH3D)
//          return 0;

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
        return -1;

  // Go through equip objects, and processes for matching player objects    
    for (int32_t o = 0; o < equipimagery->NumObjects(); o++)
    {
        if (equipimagery->IsHidden(o, st))
            continue;

        char *n = equipimagery->GetObjectName(o);

        if (nlen > 0 && !strnicmp(n, player->BodyType(), nlen))
            n += nlen;

      // Find matching player object
        int32_t playerobjnum = GetObjectNum(n);
        
      // Hack for weapons
        if (playerobjnum < 0 && !stricmp(n, "weapon"))
            playerobjnum = GetObjectNum("sword");

        if (playerobjnum < 0)
            continue;

    // Do the actual thing now (return weapon num)
        return o;
    }

    return -1;
}

int32_t TCharAnimator::GetWeaponNumVerts()
{
    int32_t weaponnum = GetWeaponNum();
    T3DImagery* imagery = GetWeaponImagery(weaponnum);

    if (imagery)
    {
        return imagery->NumVerts();
    }
    else if (inst->ObjClass() != OBJCLASS_PLAYER)
    // this is a character, not a player (no weapon equiped)
    {
        weaponnum = GetObjectNum("weapon");
        if (weaponnum < 0)
            weaponnum = GetObjectNum("sword");
        if (weaponnum < 0)
            weaponnum = GetObjectNum("ogrokaxe");
        if (weaponnum >= 0)
        {
            S3DAnimObj* weaponobj = GetObject(weaponnum);
            GetVerts(weaponobj, ERender3DVertex::LitVertex);
            return weaponobj->numverts;
        }

    }

    return 0;
}

S3DLVertex* TCharAnimator::GetWeaponVertices(int32_t len)
{
    if (len <= 0)
        return nullptr;

    int32_t weaponnum = GetWeaponNum();
    T3DImagery* imagery = GetWeaponImagery(weaponnum);

    if (imagery)
    {
        S3DLVertex* verts;
        verts = new S3DLVertex[len];
        imagery->GetObjVerts(weaponnum, verts, 0, 0, ERender3DVertex::LitVertex);

        return verts;
    }
    else if (inst->ObjClass() != OBJCLASS_PLAYER)
    // this is a character, not a player (no weapon equiped)
    {
        weaponnum = GetObjectNum("weapon");
        if (weaponnum < 0)
            weaponnum = GetObjectNum("sword");
        if (weaponnum < 0)
            weaponnum = GetObjectNum("ogrokaxe");
        if (weaponnum >= 0)
        {
            S3DAnimObj* weaponobj = GetObject(weaponnum);
            GetVerts(weaponobj, ERender3DVertex::LitVertex);
            return (S3DLVertex*)weaponobj->verts;
        }
    }
    return nullptr;
}

void TCharAnimator::SetupWeaponSwipe()
{
    if (!inst)
        return;

    SWeaponSwipeParams p;
    p.numverts = GetWeaponNumVerts();
    p.weaponverts = GetWeaponVertices(p.numverts);
    if (!p.weaponverts)
        return;
    
    if (inst->ObjClass() == OBJCLASS_PLAYER)
    {
        TPlayer* player = (TPlayer*)inst;
        TObjectInstance* oi = player->PrimeHand();
        if (oi)
        {
            p.primehand = oi->GetTypeName();
        }
    }
    else
        p.primehand = nullptr;

    p.charanim = this;

    SCharData* chardata = ((TCharacter*)inst)->GetCharData();
    if (chardata->swipecolor.red == 0 &&
        chardata->swipecolor.green == 0 &&
        chardata->swipecolor.blue == 0)
        return;

    p.r = (float)(chardata->swipecolor.red) / 255.0f;
    p.g = (float)(chardata->swipecolor.green) / 255.0f;
    p.b = (float)(chardata->swipecolor.blue) / 255.0f;

    int32_t length = 5;
    p.smooth = 8;
    p.maxsegs = length * p.smooth;
    
    weaponswipe.Init(&p);
}

void TWeaponSwipe::Init(SWeaponSwipeParams* p)
{
    initialized = true;
    r = p->r;
    g = p->g;
    b = p->b;
    charanim = p->charanim;
    numverts = p->numverts;
    weaponverts = p->weaponverts;
    maxsegs = p->maxsegs;
    smooth = p->smooth;
    primehand = p->primehand;
    //eff = p->eff;
    
    if (!weaponverts)
    {
        initialized = false;
        return;
    }

    obj = &theobj;
    memset(obj, 0, sizeof(S3DAnimObj));
    
    maxpoints = ((maxsegs / smooth) + 3);
    points[0] = new hmm_vec3[maxpoints];
    points[1] = new hmm_vec3[maxpoints];
    memset(points[0], 0, sizeof(points));
    memset(points[1], 0, sizeof(points));

    // Paraphrased GetVerts
    maxverts = (maxpoints - 1) * smooth * 2;
    obj->numverts = maxverts;
    obj->verts = new S3DLVertex[obj->numverts];
    obj->verttype = ERender3DVertex::LitVertex;

    // Paraphrased GetFaces
    obj->numfaces = (maxsegs * 2);
    //if (!obj->faces)
    obj->faces = new S3DFace[obj->numfaces];
    for (int32_t i = 0; i < obj->numfaces; i++)
    {
        obj->faces[i].v1 = i;
        obj->faces[i].v2 = i + 1;
        obj->faces[i].v3 = i + 2;
    }

  // Set start and number of faces for texture 0 (no texture)
    obj->texfaces[0] = 0;
    obj->numtexfaces[0] = obj->numfaces;

    obj->primtype = ERender3DPrim::TriangleList;

    for (int32_t i = 0; i < maxsegs + 1; i++)
    {
        Animate();
    }

    GetWeaponExtents(); // set vweapbeg, vweapend
    //weaponmat = GetCharsWeaponMatrix(); // get a pointer the matrix of the weapon (in the character's animator)!
    NormalizeColors();  // fix ratios on r, g, b
}

void TWeaponSwipe::GenerateStrip()
{
    if (!obj)
        return;
    if (obj->numverts == 0 || !initialized)
        return;

    obj->flags = OBJ3D_MATRIX | OBJ3D_ROT1 | OBJ3D_VERTS | OBJ3D_FACES | OBJ3D_OWNSVERTS | OBJ3D_OWNSFACES;
    MtxClear(&obj->matrix);

    float start = 0.3f, startr = min(r + 0.4f, 1.0f), startg = min(g + 0.4f, 1.0f), startb = min(b + 0.4f, 1.0f);
    float fadeoutstep = (float)(start * 2.0f / maxverts), alpha = start;
    float wfade = (float)(4.0f / maxverts), ratio = 0.0f, smoothinv = (float)(1.0f / smooth);
    hmm_vec3 avert; // your eyes
    int32_t o, k, i, vn = 0;
    S3DLVertex* lverts = (S3DLVertex*)obj->verts;
    for (o = 0; o < maxpoints - 2; o++)
    {
        ratio = 0.0f;
        for (i = 0; i < smooth; i++)
        {
            for (k = 0; k < 2; k++)
            {
                Spline(&avert, ratio, &points[k][max(0, o - 1)], &points[k][o], &points[k][o + 1], &points[k][o + 2]);
                lverts[vn].pos.X = avert.X;
                lverts[vn].pos.Y = avert.Y;
                lverts[vn].pos.Z = avert.Z;
                lverts[vn].diffuse = PackARGB(startr, startg, startb, alpha);
                vn++;
                if (startr > r)
                    startr -= wfade;
                if (startr <= r)
                    startr = r;
                if (startg > g)
                    startg -= wfade;
                if (startg <= g)
                    startg = g;
                if (startb > b)
                    startb -= wfade;
                if (startb <= b)
                    startb = b;
                alpha -= fadeoutstep;
                if (alpha < 0.0f)
                    alpha = 0.0f;
            }
            ratio += smoothinv;
        }
    }
}

void TWeaponSwipe::Animate()
{
    if (!obj)
        return;
    if (obj->numverts == 0 || !initialized)
        return;

    if (charanim->GetObjInst()->ObjClass() == OBJCLASS_PLAYER)
    {
        TPlayer* player = (TPlayer*)charanim->GetObjInst();
        TObjectInstance* oi = player->PrimeHand();
        if (oi)
        {
            const char* name = oi->GetTypeName();
            if (stricmp(primehand, name)) // switched weapons! now reset the weaponswipe and all that
            {
                Close();
                return;
            }
        }
    }

    weaponmat = GetCharsWeaponMatrix(); // get a pointer the matrix of the weapon (in the character's animator)!
    hmm_vec3 beg, end;

    S3DPoint animpos;
        
    MtxTransform(weaponmat, &vweapbeg, &beg);
    MtxTransform(weaponmat, &vweapend, &end);
    
    CycleStrip();
    points[0][0].X = beg.X;
    points[0][0].Y = beg.Y;
    points[0][0].Z = beg.Z;
    points[1][0].X = end.X;
    points[1][0].Y = end.Y;
    points[1][0].Z = end.Z;
}

void TWeaponSwipe::GetWeaponExtents()
{
    if (!obj)
        return;
    if (obj->numverts == 0 || !initialized)
        return;
    
    int32_t ivweapbeg = 0;
    int32_t ivweapend = 0;
    float beg = 100000.0f, end = -100000.0f;
    for (int32_t i = 0; i < numverts; i++)
    {
        if (weaponverts[i].pos.Z < beg)
        {
            beg = weaponverts[i].pos.Z;
            ivweapbeg = i;
        }
        if (weaponverts[i].pos.Z > end)
        {
            end = weaponverts[i].pos.Z;
            ivweapend = i;
        }
    }
    (void)ivweapbeg;
    vweapbeg.X = 0.0f;
    vweapbeg.Y = 0.0f;
    vweapbeg.Z = 0.0f;
    vweapend.X = weaponverts[ivweapend].pos.X;
    vweapend.Y = weaponverts[ivweapend].pos.Y;
    vweapend.Z = weaponverts[ivweapend].pos.Z;
}

void TWeaponSwipe::NormalizeColors()
{
    float most = max(r, max(g, b));
    if (most > 0.0f)
    {
        r /= most;
        g /= most;
        b /= most;
    }
    if (r > 1.0)
        r = 1.0;
    if (g > 1.0)
        g = 1.0;
    if (b > 1.0)
        b = 1.0;
}

void TWeaponSwipe::ChangeColor(float r, float g, float b)
{
    r = r;
    g = g;
    b = b;
    NormalizeColors();
}

void TWeaponSwipe::CycleStrip()
{
    if (!obj)
        return;
    if (obj->numverts == 0 || !initialized)
        return;

    for (int32_t o = 0; o < 2; o++)
    {
        for (int32_t i = maxpoints - 1; i > 0; i--)
        {
            points[o][i].X = points[o][i - 1].X;
            points[o][i].Y = points[o][i - 1].Y;
            points[o][i].Z = points[o][i - 1].Z;
        }
    }
}

void TWeaponSwipe::Render()
{
    if (!obj)
        return;
    if (obj->numverts == 0 || !initialized)
        return;

    SaveBlendState();

    // set the new render flags
    Scene3D.SetRenderState(ERender3DState::Cull,       0 /* none */);
    Scene3D.SetRenderState(ERender3DState::SrcBlend,   (uint32_t)ERender3DBlend::SrcAlpha);
    Scene3D.SetRenderState(ERender3DState::DstBlend,   (uint32_t)ERender3DBlend::InvSrcAlpha);
    Scene3D.SetRenderState(ERender3DState::AlphaBlend, 1);
    Scene3D.SetRenderState(ERender3DState::ZWriteEnable, 0);

    //charanim->ResetExtents();
    GenerateStrip();

    charanim->RenderObject(obj);
    //charanim->UpdateExtents();

    RestoreBlendState();
}

hmm_mat4* TWeaponSwipe::GetCharsWeaponMatrix()
{
    int32_t weaponnum;
    S3DAnimObj* weaponobj;

    weaponnum = charanim->GetObjectNum("weapon");

  // This is a hack here!
    if (weaponnum < 0)
        weaponnum = charanim->GetObjectNum("sword");
    if (weaponnum < 0)
        weaponnum = charanim->GetObjectNum("ogrokaxe");

    if (weaponnum >= 0)
    {
        weaponobj = charanim->GetObject(weaponnum);
        if (weaponobj && charanim->GetObjectMatrix(weaponnum, &weaponobj->matrix))
            return &weaponobj->matrix;
    }
    return nullptr;
}

void TWeaponSwipe::Close()
{
    initialized = false;

    delete points[0];
    delete points[1];
}


#endif // attic-only
