// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 charanimator.cpp - TCharAnimator module               *
// *************************************************************************

#include <cmath>
#include <cstdint>
#include <cstring>

#include "character.h"
#include "player.h"
#include "charanimator.h"
#include "effect.h"
#include "logging.h"
#include "math3d.h"
#include "render3d_types.h"
#include "renderer.h"
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
    ++swipe_ticks;
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

    // Retail's combat flash is a second, additive render of the character
    // (DrawsCombatFlash), not the snapshot's RenderCombatFlashes quad.

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
    UpdateWeaponSwipe();
}

float TCharAnimator::DrawAlpha() const
{
    if (IsHidden() || transparency < kMinDrawTransparency)
        return 0.0f;
    return transparency;
}

// REVSYNC: TCharAnimator::Render @ 0x004d7bd8..0x004d7c0c -- while the
// combat flash ticks (TCharacter +0x224, five from a blow) run, the
// character renders again in blend mode 16 (litadd): its lit colour added
// over itself, the white-hot flash of a struck body. The map renderer
// submits that second pass (SMeshSubmit::additive). The snapshot's
// RenderCombatFlashes (a utility quad) is not what shipped.
bool TCharAnimator::DrawsCombatFlash() const
{
    return static_cast<TCharacter*>(inst)->GetCombatFlashTicks() >= 1;
}

// *****************
// * Weapon swipe  *
// *****************

// REVSYNC: TCharAnimator::Animate @ 0x004d7a20 starts the swipe whenever it
// isn't running; Render @ 0x004d7c47 updates it each drawn frame, after the
// character (so not while hidden, nor below the drawn-alpha floor). Retail
// drew at most once a tick: the port updates once a tick, on the first
// drawn frame after it, at that tick's key -- and keeps the history the
// next key's update leads to, to draw the frames between toward it.
void TCharAnimator::UpdateWeaponSwipe()
{
    if (!swipe.Initialized())
        SetupWeaponSwipe();
    if (!swipe.Initialized() || swipe_ticks_seen == swipe_ticks || DrawAlpha() <= 0.0f)
        return;
    swipe_ticks_seen = swipe_ticks;
    TWeaponSwipe::SMatrix weapon{};
    swipe.Update(WeaponSwipeFrame(false, weapon));
    TWeaponSwipe next = swipe;
    next.Update(WeaponSwipeFrame(true, weapon));
    swipe_next = next.Initialized() ? next.Ring() : swipe.Ring();
}

// REVSYNC: TCharAnimator::SetupWeaponSwipe @ 0x004d8f10 -- nothing without a
// SWIPECOLOR. The colour bytes / 255 (0x005a3830), not normalised; SWIPEFULL
// for a character, the SwipeFull stat of a player's weapon in hand (vtable
// +0xd4), whose name the swipe keeps to notice a change. Retail measures
// the weapon first; nothing reads the bounds without a colour.
void TCharAnimator::SetupWeaponSwipe()
{
    auto* character = static_cast<TCharacter*>(inst);
    const SCharData* data = character ? character->GetCharData() : nullptr;
    if (!data)
        return;
    const SColor& color = data->swipecolor;
    if (!color.red && !color.green && !color.blue)
        return;

    TWeaponSwipe::SParams params;
    WeaponExtents(params.extent_min, params.extent_max);
    if (inst->ObjClass() == OBJCLASS_PLAYER)
    {
        if (TObjectInstance* in_hand = static_cast<TPlayer*>(inst)->PrimeHand())
        {
            if (const char* name = in_hand->GetName())
                params.weapon = name;
            params.full = in_hand->GetStat("SwipeFull") != 0;
        }
    }
    else
        params.full = data->swipefull != 0;
    params.color = TWeaponSwipe::ColorOf(color.red, color.green, color.blue);
    TWeaponSwipe::SMatrix weapon{};
    swipe.Init(params, WeaponSwipeFrame(false, weapon));
    swipe_next = swipe.Ring();
    const int32_t object = SwipeObjectNum();
    log_info("[weapon-swipe] %s: object %d '%s', colour %u,%u,%u, full %d, bounds (%.1f,%.1f,%.1f)-(%.1f,%.1f,%.1f)",
             inst->GetTypeName(), object,
             object >= 0 ? Get3DImagery()->GetObjectName(GetObject(object)->objnum) : "-",
             color.red, color.green, color.blue, int(params.full),
             double(params.extent_min[0]), double(params.extent_min[1]), double(params.extent_min[2]),
             double(params.extent_max[0]), double(params.extent_max[1]), double(params.extent_max[2]));
}

// REVSYNC: TWeaponSwipe::Update @ 0x004d95b0 reads the owner: doing
// ACTION_ATTACK (a player only with a weapon in hand, +0x2b0), the frame
// (+0x5c), the attack's swipeframeon/off (+0xec/+0xf0); AddPoint 0x004d96c0
// and Reset 0x004d9c60 the weapon object's matrix (+0x58).
TWeaponSwipe::SFrame TCharAnimator::WeaponSwipeFrame(bool next, TWeaponSwipe::SMatrix& weapon)
{
    TWeaponSwipe::SFrame frame;
    auto* character = static_cast<TCharacter*>(inst);
    TObjectInstance* in_hand = nullptr;
    if (inst->ObjClass() == OBJCLASS_PLAYER)
    {
        frame.player = true;
        in_hand = static_cast<TPlayer*>(inst)->PrimeHand();
        frame.in_hand = in_hand ? in_hand->GetName() : nullptr;
    }
    const TActionBlock* doing = character->DoingBlock();
    frame.attacking = doing && doing->action == ACTION_ATTACK && (!frame.player || in_hand);
    frame.frame = inst->GetFrame();
    if (next)
    {
        // The next key, while the state holds; past it the window is shut.
        const SLegacyRenderKey key = NextKey();
        frame.frame = key.state == inst->GetState() ? key.frame : INT32_MIN;
    }
    if (frame.attacking && doing->attack)
    {
        frame.frame_on = doing->attack->swipeframeon;
        frame.frame_off = doing->attack->swipeframeoff;
    }
    hmm_mat4 model;
    const int32_t object = SwipeObjectNum();
    if (object >= 0 && ObjectModelMatrix(object, next ? 1.0f : 0.0f, model))
    {
        static_assert(sizeof(model) == sizeof(weapon), "a D3D matrix");
        std::memcpy(weapon.data(), &model, sizeof(weapon));
        frame.weapon = &weapon;
    }
    return frame;
}

int32_t TCharAnimator::SwipeObjectNum()
{
    for (const char* name : {"weapon", "sword", "ogrokaxe"})
        if (const int32_t object = GetObjectNum(name); object >= 0)
            return object;
    return -1;
}

// REVSYNC: TCharAnimator::GetWeaponNum @ 0x004d8bd0 -- the weapon's state for
// this body ("still" or "all" for every body, else the body type, a prefix
// on its object names), then its first shown object this body has a place
// for ("weapon" falls back to "sword").
int32_t TCharAnimator::WeaponObjectNum()
{
    if (inst->ObjClass() != OBJCLASS_PLAYER)
        return -1;
    auto* player = static_cast<TPlayer*>(inst);
    TObjectInstance* in_hand = player->PrimeHand();
    auto* imagery = in_hand ? dynamic_cast<T3DImagery*>(in_hand->GetImagery()) : nullptr;
    const SImageryHeader* header = imagery ? imagery->GetHeader() : nullptr;
    if (!header)
        return -1;
    const char* body = player->BodyType();
    size_t prefix = 0;
    int32_t state = 0;
    for (; state < header->numstates; ++state)
    {
        const char* name = header->states[state].animname;
        if (!stricmp(name, "still") || !stricmp(name, "all"))
            break;
        if (!stricmp(name, body))
        {
            prefix = strlen(body);
            break;
        }
    }
    if (state >= header->numstates)
        return -1;
    for (int32_t object = 0; object < imagery->NumObjects(); ++object)
    {
        if (imagery->IsHidden(object, state))
            continue;
        const char* name = imagery->GetObjectName(object);
        if (prefix > 0 && !strnicmp(name, body, prefix))
            name += prefix;
        int32_t own = GetObjectNum(name);
        if (own < 0 && !stricmp(name, "weapon"))
            own = GetObjectNum("sword");
        if (own >= 0)
            return object;
    }
    return -1;
}

// REVSYNC: TCharAnimator::GetWeaponExtents @ 0x004d8e60 -- a player's weapon
// object on the weapon's own imagery, else this character's weapon object,
// at state 0, frame 0. Zero without an object.
void TCharAnimator::WeaponExtents(TWeaponSwipe::SVec3& low, TWeaponSwipe::SVec3& high)
{
    low = {};
    high = {};
    T3DImagery* imagery = nullptr;
    int32_t object = WeaponObjectNum();
    if (object >= 0)
        if (TObjectInstance* in_hand = static_cast<TPlayer*>(inst)->PrimeHand())
            imagery = dynamic_cast<T3DImagery*>(in_hand->GetImagery());
    if (!imagery)
    {
        imagery = Get3DImagery();
        const int32_t own = SwipeObjectNum();
        object = own >= 0 ? GetObject(own)->objnum : -1;
    }
    if (!imagery || object < 0)
        return;
    hmm_vec3 bound_low, bound_high;
    imagery->GetObjectBounds(object, bound_low, bound_high);
    low = {bound_low.X, bound_low.Y, bound_low.Z};
    high = {bound_high.X, bound_high.Y, bound_high.Z};
}

// REVSYNC: TCharAnimator::Render @ 0x004d7c6f..0x004d7cc3 -- the strip
// (BuildStrip 0x004d9380) in blend mode 2: SRCALPHA/INVSRCALPHA, depth
// tested without writes, no culling (0x00417f46), prelit, in the model
// space of the character under its animator matrix (RenderObject
// 0x0040a8f0). Its alpha is its own: the character's fade doesn't reach it.
//
// REVSYNC-DIVERGENCE: retail's 80 faces run two past the 80 vertices
// BuildStrip writes; the last two index the zero-filled vertices 80 and 81,
// a faint black wedge from the strip's tail to the character's feet. The
// port draws the 78 strip triangles. Between ticks it draws the history
// eased toward the next tick's (UpdateWeaponSwipe).
void TCharAnimator::SubmitFx(TRenderer& renderer) const
{
    if (!inst || !swipe.Initialized() || DrawAlpha() <= 0.0f)
        return;
    const TWeaponSwipe::SRing ring =
        TWeaponSwipe::Blend(swipe.Ring(), swipe_next, float(TTime::LegacyFrameFraction()));
    if (!TWeaponSwipe::HasWidth(ring))
        return;
    TWeaponSwipe::SStrip strip;
    TWeaponSwipe::BuildStrip(ring, swipe.Color(), strip);
    SubmitSwipeStrip(renderer, strip, inst->Transform().Matrix());
}

void TCharAnimator::SubmitSwipeStrip(TRenderer& renderer, const TWeaponSwipe::SStrip& strip,
                                     const hmm_mat4& model_to_world)
{
    std::array<hmm_vec3, TWeaponSwipe::kStripVertices> world;
    for (size_t i = 0; i < strip.size(); ++i)
    {
        const hmm_vec3 model = {strip[i].pos[0], strip[i].pos[1], strip[i].pos[2]};
        MtxTransform(&model_to_world, &model, &world[i]);
    }
    // Triangles (2j, 2j+1, 2j+2) and (2j+1, 2j+2, 2j+3) as one quad: the
    // quad splits on corners 0-3, which carry their shared edge.
    constexpr int32_t kCornerVertex[4] = {1, 3, 0, 2};
    for (int32_t first = 0; first + 3 < TWeaponSwipe::kStripVertices; first += 2)
    {
        SQuadDrawItem quad;
        quad.key.blend = uint8_t(EFxBlend::Alpha);
        quad.key.depth_mode = uint8_t(EFxDepthMode::TestNoWrite);
        quad.light_mode = EFxLightMode::Unlit;
        quad.per_corner_color = true;
        for (int32_t corner = 0; corner < 4; ++corner)
        {
            const size_t vertex = size_t(first + kCornerVertex[corner]);
            quad.world_pos[corner][0] = world[vertex].X;
            quad.world_pos[corner][1] = world[vertex].Y;
            quad.world_pos[corner][2] = world[vertex].Z;
            const uint32_t argb = strip[vertex].argb;
            quad.corner_color_rgba[corner][0] = float((argb >> 16) & 0xff) / 255.0f;
            quad.corner_color_rgba[corner][1] = float((argb >> 8) & 0xff) / 255.0f;
            quad.corner_color_rgba[corner][2] = float(argb & 0xff) / 255.0f;
            quad.corner_color_rgba[corner][3] = float(argb >> 24) / 255.0f;
        }
        renderer.SubmitFxQuad(quad);
    }
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

