// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *     shadowencounters.h - the monsters a walk would meet on the way      *
// *************************************************************************
//
// Travel between two points is not free (docs/shadow/ARCHITECTURE.md §5.5).
// When Locke walks, the monsters along his way find him and come for him;
// a teleport would slip past them. So a leg's walk is checked for every
// monster whose look would find him at some point of it -- alive,
// aggressive, his enemy -- and the leg stops where the first one would, for
// a fight.
//
// The look is the AI's own (TCharacter::FindCharacters with ENEMY, HEAR and
// SEE): a valid target is within the monster's COMBATRANGEMAX, edge to edge
// (IsValidTarget), and it hears him -- any noise carries within its
// HEARINGRANGE of its edge plus 32, with or without a line of sight
// (Hearing, CanHearCharacter). Sight adds nothing on a walk: Sight wants a
// glimpse of 99, and only an attacking Locke gives one off
// (ResetStealthValues).

#pragma once

#include "../revtypes.h"
#include "../saferef.h"

#include <cstdint>
#include <vector>

class TCharacter;

struct SShadowEncounter
{
    TSafeRef<TCharacter> monster;
    size_t               index = 0;     // the first path point it sees
};

// The monsters a walk along `path` on `level` meets, in the order it meets
// them.
[[nodiscard]] std::vector<SShadowEncounter> ShadowLegEncounters(const std::vector<S3DPoint>& path, int32_t level);

// Of `encounters`, how many an evading Locke still has to fight (§5.5): a
// share of them, at least one when there are any.
[[nodiscard]] size_t ShadowEvadeShare(const std::vector<SShadowEncounter>& encounters, double lockerun);
