// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  monstergen.h - TMonsterGen object                    *
// *************************************************************************
//
// A monster generator: the HELPER type "MonsterGen" (class.def), a marker
// the designers placed in the wilds so they don't run out of monsters. The
// shipped game's behaviour (retail vtable 0x5b4cd4) is documented in
// docs/playtest/GRIND.md §3.1; this is a port of it.
//
// - Up to five slots, each a CHARACTER type with a frequency, a treasure
//   tier and the game time its clock started. The editor and scripts set
//   them with `addmonstertype`, `delmonstertype` and `maxmonsters`.
// - While the generator is off screen, a slot whose clock has run past its
//   frequency, but not past twice it, restarts the clock and makes one
//   monster 50-150 units away when fewer characters than the cap stand
//   within 300 units.
// - Sector and save files keep the slots, their clocks and the cap
//   (docs/gameflow/forensics/SAVE_GAME.md §11.3).

#pragma once

#include "helper.h"

#include <array>
#include <optional>
#include <string>

// A generator's slot (retail: a 16-byte block the generator owns). At
// namespace scope, not nested: a nested struct's member initializers are
// only complete with the enclosing class, and libstdc++ decides
// std::optional<SSlot>'s constructibility before then.
struct SMonsterGenSlot
{
    std::string type;           // +0x00 the CHARACTER type it makes
    int32_t     frequency = 0;  // +0x04 game time between monsters, 1/100 s
    int32_t     treasure  = 0;  // +0x08 treasure tier (GiveSpawnTreasure)
    int32_t     start     = 0;  // +0x0c game time its clock started; 0: not yet
};

_CLASSDEF(TMonsterGen)
class TMonsterGen : public THelper
{
    friend class TLegacyDeserialize;    // the retail stream (legacystream.h)
    friend class TLegacySerialize;
  public:
    // REVSYNC: Build @ 0x005176a0 -- a generator is an editor object that
    // never moves and always pulses (flags 0x8101).
    TMonsterGen(TObjectImagery* newim) : THelper(newim) { flags |= kGenFlags; }
    TMonsterGen(SObjectDef* def, TObjectImagery* newim) : THelper(def, newim) { flags |= kGenFlags; }

    static constexpr int32_t kMaxTypes = 5;     // slots (+0xd8..+0xe8)

    // Its sector layout (slots, cap) is TLegacyDeserialize::ReadMonsterGen.

    // REVSYNC: 0x00516c30 (vtable 0x110) -- run the slots' clocks and make
    // the monsters that are due. No script runs: the base Pulse isn't called.
    void Pulse() override;

    // REVSYNC: AddType @ 0x00517050 -- the first free slot makes `type`
    // every `frequency` hundredths of a second (at most 19999), with treasure
    // tier `treasure`; its clock starts now. False when every slot is used.
    bool AddType(const char* type, int32_t frequency, int32_t treasure);
    // REVSYNC: 0x00517110 -- frees the first slot making `type` (exactly,
    // case included). False when none does.
    bool DelType(const char* type);
    // REVSYNC: 0x005171d0 -- the used slots.
    [[nodiscard]] int32_t NumTypes() const;
    // REVSYNC: 0x005171f0 -- slot `index`'s type, "" for a free slot.
    [[nodiscard]] const char* TypeName(int32_t index) const;

    [[nodiscard]] int32_t MaxMonsters() const { return maxmonsters; }
    void SetMaxMonsters(int32_t count) { maxmonsters = count; }
        // The cap (`maxmonsters`): a slot makes a monster only while fewer
        // characters than this stand within 300 units

  private:
    static constexpr uint32_t kGenFlags = OF_IMMOBILE | OF_EDITOR | OF_PULSE;

    using SSlot = SMonsterGenSlot;

    // REVSYNC: 0x00517240 -- a monster of slot `index`'s type on a walkable
    // spot 50-150 units away, facing a random way, with the slot's treasure.
    void Spawn(int32_t index);

    std::array<std::optional<SSlot>, kMaxTypes> slots{};   // +0xd8
    int32_t maxmonsters = 2;                                // +0xec
    bool    offscreen   = true;                             // +0xf0, OnScreen/OffScreen
};

DEFINE_BUILDER("MonsterGen", TMonsterGen)
