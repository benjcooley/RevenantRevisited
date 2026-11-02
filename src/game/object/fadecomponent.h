// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   fadecomponent.h - a character's fade and invisibility               *
// *************************************************************************
//
// The visibility a character draws with (retail TCharacter +0x194..+0x1a4):
// the fade, 0 to 100, that Transparency reports to the animator; the step
// each pulse takes it by and the limit it stops at; which way it is going;
// whether the invisibility spell holds it at 30; and whether MakeInvisible
// was called. Every character has one from construction, reached through
// Fader(); Fade, SetFade, UpdateFade and SetInvisibleSpell drive it. None
// of it is in the character's record: a loaded character starts fully
// visible, as in retail.
//
// *************************************************************************

#pragma once

#include "engine/scene/component.h"

#include <cstdint>

class TFadeComponent final : public TComponent
{
  public:
    DEF_COMPONENT("fade")
    [[nodiscard]] const char* ComponentName() const override { return "fade"; }

    PROP_NOSAVE(1, "Visibility, 0 to 100")                        int32_t fade           = 100;   // retail +0x194
    PROP_NOSAVE(2, "Taken off the fade each pulse")               int32_t fade_step      = 0;     // retail +0x198
    PROP_NOSAVE(3, "Where the fade stops; -1 at 0 or 100")        int32_t fade_limit     = 100;   // retail +0x19c
    PROP_NOSAVE(4, "-1 fading out, 1 in, 0 still")                int32_t fade_direction = 0;     // retail +0x1a0; a corpse fades once it is 0
    PROP_NOSAVE(5, "The invisibility spell holds it at 30")       bool    invisible_spell = false; // retail +0x1a4
    PROP_NOSAVE(6, "MakeInvisible was called; nothing reads it")  bool    is_invisible    = false;
};
