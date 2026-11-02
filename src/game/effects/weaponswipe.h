// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *        weaponswipe.h - retail weapon swipe (no engine types)          *
// *************************************************************************
//
// The ribbon a weapon leaves through an attack: retail TWeaponSwipe,
// embedded in TCharAnimator at +0x1a8 (vtable 0x005a7e98). Each drawn
// frame, while the attack is between its swipeframeon and swipeframeoff
// (char.def ATTACK fields 23/24), the weapon's hilt and tip are sampled in
// the character's model space into an eight-sample history; between
// samples every pair closes in on its midpoint; the strip is a Catmull-Rom
// spline through the history, a constant colour fading in alpha, drawn in
// blend mode 2 (alpha) after the character (TCharAnimator::Render
// 0x004d7c23..0x004d7cc3).
//
// Kept free of engine types so the retail A/B probe
// (tools/retail_runtime/weapon_swipe_probe.py) can compile it next to the
// original code in the emulator. TCharAnimator (charanimator.cpp) feeds it
// the weapon matrix and the attack, and draws it. Forensics:
// docs/gameplay/forensics/COMBAT_EFFECTS.md §6.
//
// What shipped differs from the 1998 snapshot (weapontrail.cpp): the
// colour is SWIPECOLOR/255 as is (the snapshot's normalisation has no
// caller), the alpha starts at 0.7 with no brightened hilt, samples shrink
// toward their midpoint, the history resets when the window opens, the
// strip starts at the second sample, and a weapon modelled along y sweeps
// its low end (a retail slip kept as shipped).
//
// *************************************************************************

#pragma once

#include <array>
#include <cstdint>
#include <string>

class TWeaponSwipe
{
  public:
    // SetupWeaponSwipe passes 8 and 40 (0x004d9092 / 0x004d909a).
    static constexpr int32_t kSmooth = 8;      // spline steps between samples (+0x3b0)
    static constexpr int32_t kMaxSegs = 40;    // +0x3ac
    static constexpr int32_t kPoints = kMaxSegs / kSmooth + 3;   // 8 samples (+0x354, 0x004d9199)
    // BuildStrip writes (kPoints - 3) segments of kSmooth vertex pairs.
    static constexpr int32_t kStripVertices = (kPoints - 3) * kSmooth * 2;   // 80

    using SVec3 = std::array<float, 3>;
    // A D3DMATRIX: row vectors, the translation in elements 12..14.
    using SMatrix = std::array<float, 16>;

    // The sampled history, newest first: the weapon's hilt end (+0x35c) and
    // tip end (+0x360), in the character's model space.
    struct SRing
    {
        std::array<SVec3, kPoints> hilt{};
        std::array<SVec3, kPoints> tip{};
    };

    // One strip vertex: retail's D3DLVERTEX position and packed ARGB.
    struct SVertex
    {
        SVec3 pos{};
        uint32_t argb = 0;
    };
    // Even vertices on the hilt side, odd on the tip side; triangle i is
    // (i, i + 1, i + 2).
    using SStrip = std::array<SVertex, kStripVertices>;

    // What TCharAnimator's SetupWeaponSwipe (0x004d8f10) hands Init.
    struct SParams
    {
        SVec3 extent_min{};     // the weapon object's vertex bounds (0x004d8e60)
        SVec3 extent_max{};
        SVec3 color{};          // SWIPECOLOR bytes / 255: not normalised
        bool full = false;      // SWIPEFULL, or a player's weapon's SwipeFull
                                // stat: the strip starts at the hilt end of
                                // the weapon, not at its pivot
        std::string weapon;     // a player's weapon's name (empty otherwise)
    };

    // The owner, as one Update sees it.
    struct SFrame
    {
        bool attacking = false;     // doing ACTION_ATTACK; a player needs a weapon in hand
        int32_t frame = 0;          // the instance's animation frame
        int32_t frame_on = 0;       // the attack's swipeframeon / swipeframeoff
        int32_t frame_off = 0;      // (both 0: no swipe, or no attack data)
        const SMatrix* weapon = nullptr;   // the weapon object's model matrix
        bool player = false;
        const char* in_hand = nullptr;     // the player's weapon's name, or null
    };

    // Init @ 0x004d90b0: a cleared history, then 41 samples of the weapon
    // as it is, then the weapon's long axis picked from its bounds.
    void Init(const SParams& params, const SFrame& now);
    // Close @ 0x004d92d0. The next SetupWeaponSwipe starts it again.
    void Close();
    // Update @ 0x004d95b0: once a drawn frame (TCharAnimator::Render).
    void Update(const SFrame& now);

    [[nodiscard]] bool Initialized() const { return initialized; }
    [[nodiscard]] const SRing& Ring() const { return ring; }
    [[nodiscard]] const SVec3& Color() const { return color; }

    // SetupWeaponSwipe's colour (0x004d9028..0x004d9072): each SWIPECOLOR
    // byte times 0.00392156886f, not normalised.
    [[nodiscard]] static SVec3 ColorOf(uint8_t red, uint8_t green, uint8_t blue);
    // BuildStrip @ 0x004d9380.
    static void BuildStrip(const SRing& ring, const SVec3& color, SStrip& out);
    // Port: the history between two frames' samples (render interpolation).
    [[nodiscard]] static SRing Blend(const SRing& from, const SRing& to, float t);
    // Port: whether any sample pair is apart (an idle swipe has shrunk to
    // nothing, and draws nothing).
    [[nodiscard]] static bool HasWidth(const SRing& ring);

  private:
    void AddPoint(const SFrame& now);   // 0x004d96c0
    void Reset(const SFrame& now);      // 0x004d9c60
    void Shift();                       // 0x004d9ae0
    void Shrink();                      // 0x004d9b60
    void ChooseAxis();                  // 0x004d9820

    SRing ring;
    SVec3 hilt_local{};     // +0x364: the weapon-space points sampled; kept
    SVec3 tip_local{};      // +0x370   across Close, zero until the first Init
    SVec3 extent_min{};     // +0x384
    SVec3 extent_max{};     // +0x390
    SVec3 color{};          // +0x3a0
    std::string weapon;     // +0x3b4
    bool full = false;      // +0x3b8
    bool initialized = false;   // +0x380
};
