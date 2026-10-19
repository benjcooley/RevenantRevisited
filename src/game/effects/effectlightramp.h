// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *        effectlightramp.h - a spell effect's light, in and out          *
// *************************************************************************

#pragma once

#include <cstdint>

// REVSYNC: the spell light of a TEffect (TEffect::Pulse 0x004de860..
// 0x004de9c4; SetSpell 0x004de7d0 clears it): the LIGHT's intensity reached
// over its fadein ticks, held, and from the effect's kill faded over its
// fadeout ticks from the value it had. (Proved against a native owner trace
// by the VFX track's Regeneration pass, docs/vfx/forensics/
// GENERIC_EFFECT_OWNER_REGENERATION_20261011.md.)
struct SEffectLightRamp
{
    int32_t in = 0;         // ticks faded in
    int32_t out = 0;        // ticks of fading out left
    int32_t value = 0;      // the intensity now
    int32_t peak = 0;       // the intensity the fade out starts from

    void Reset() { in = out = value = peak = 0; }
    void StartFadeOut(int32_t fadeout) { out = fadeout; }
      // From the effect's kill, when the light has a fade out
    int32_t Step(int32_t intensity, int32_t fadein, int32_t fadeout)
      // One tick: the intensity to light at
    {
        if (out > 0 && fadeout > 0)
            value = peak * (--out) / fadeout;
        else
        {
            value = (in < fadein && fadein > 0) ? intensity * (++in) / fadein : intensity;
            peak = value;
        }
        return value;
    }
};
