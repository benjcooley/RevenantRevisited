#!/usr/bin/env python3
"""Freeze exact retail ID -> existing preview candidates from the source ledger."""
import argparse
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

# Source-body audit, 2026-10-10. This certifies an implemented preview, NOT
# original-renderer fidelity or a natural map caller. Unknown registrations fail
# closed even when their name contains BESPOKE. Old group STUB banners can be
# stale: FireFlash/FireCone/MeteorStorm/Sparks have real replacement bodies.
# See each named Spawn/TickAndSubmit in effect.cpp and its vfxtest.cpp factory.
IMPLEMENTED = {
    **dict.fromkeys("""
TBloodEffect_BESPOKE TBloodEffect
TSparksEffect_BESPOKE TSparkEffect TFireEffect_BESPOKE TFireEffect
TFountainAnimator_BESPOKE TCyanFountainAnimator_BESPOKE
TRedFountainAnimator_BESPOKE TGreenFountainAnimator_BESPOKE TBlueFountainAnimator_BESPOKE
TRibbonAnimator_BESPOKE TFlameEffect_BESPOKE TFlameEffect
TFlameEffect_BESPOKE__FlameB TFlameEffect_BESPOKE__FlameG TFireWindEffect_BESPOKE TFireWindEffect_Bespoke__YFireWind_BESPOKE
TFireBallEffect LightStrip TStripEffect TIceBoltEffect_BESPOKE TPhotonEffect_BESPOKE
TSymGlowAnimator_BESPOKE TFlyEffect_BESPOKE TFairyEffect_BESPOKE
TCureEffect_BESPOKE TFireFlashEffect_BESPOKE TFireConeEffect_BESPOKE TBurnEffect_BESPOKE
TIcedEffect_BESPOKE TStreamerEffect_BESPOKE TFireSwarmEffect_BESPOKE TFaultFireEffect_BESPOKE
TQuicksandEffect_BESPOKE TSandswirlEffect_BESPOKE TWindStripAnimator_BESPOKE THaloEffect
TDripEffect TRippleEffect TMistEffect_BESPOKE TMistEffect TWaterEffect_BESPOKE
TWaterFallEffect_BESPOKE TWaterFallEffect_BESPOKE__Waterfall
TFogEffect_Bespoke__MistFog_BESPOKE TPixieEffect_BESPOKE TFizzleEffect TFizzleEffect_SINGLE_BURST
TSetVortexEffect_BESPOKE THealEffect_BESPOKE TRockStormEffect_Bespoke_BESPOKE
TBuffEffect_Bespoke__charm_BESPOKE
""".split(), 'effect.cpp: active controller Spawn/Advance/Submit body'),
    **dict.fromkeys("""
TGlobeEffect_BESPOKE TKinSecretDoor_AUTHORED_STILL TMight_AUTHORED_TAGS
TImmortalmight_AUTHORED_TAGS TFmastery_AUTHORED_TAGS TShadowfist_AUTHORED_OWNER
TWarriorborn_AUTHORED_OWNER TTeleportation_AUTHORED_OWNER TMPAppear_AUTHORED_START
TCombatFlashEffect_AUTHORED_START1 TGoldEffect_AUTHORED_TAGS TPunchAndJudy_AUTHORED_OWNER
TWaterEffect_BESPOKE__StillWater TWaterEffect_BESPOKE__FlowWater TWaterEffect_BESPOKE__SewerWater
""".split(), 'vfxtest.cpp: exact authored mesh/tag/owner factory'),
    **{f'TRibbonAnimator_BESPOKE__Ribbon{color}':
       'vfxtest.cpp: AuthoredStaticMeshVariantSpawn, not historical ribbon alias'
       for color in 'BGOPRWY'},
    **{f'TFountainAnimator_BESPOKE__{name}Sign':
       'vfxtest.cpp: AuthoredStaticMeshVariantSpawn, not historical fountain alias'
       for name in ('Olihoot', 'Ogrok', 'Camp', 'Tower', 'Druhg', 'Ruins', 'Bone',
                    'Yard', 'Ancient', 'Village', 'Mist', 'Haven')},
    **{f'TAuthoredStaticMeshEffect_BESPOKE__ogrokwatcher{i}':
       'vfxtest.cpp: AuthoredTextureMeshVariantSpawn' for i in (1, 2, 3)},
}

# Partial display is explicitly requested by the gallery user. These exact
# retail identities have audited non-null source bodies that submit geometry,
# but do NOT implement the complete original effect. The ID key is essential:
# neither a shared asset nor a similarly named registration authorizes aliases.
# Spawn/TickAndSubmit and each factory's own asset binding were audited in
# effect.cpp/vfxtest.cpp against the shipped class identity, 2026-10-10.
PARTIAL = {
    '0xc24b2a85': ('TFogEffect_BESPOKE', 'authored animated grid; original post-blend RGB565 quantization incomplete'),
    '0x00ab1d1d': ('TAuraEffect_BESPOKE', 'billboard preview; original character bones and attachment missing'),
    '0x0ea34fa3': ('TBlastEffect_BESPOKE', 'authored 56-part animation; original oversized-triangle clipping differs'),
    '0xad92bd20': ('TLabyrinthEffect_BESPOKE', 'single asset billboard; original controller incomplete'),
    '0xaeaeeb32': ('TJhagaAttackEffect_BESPOKE', 'stationary asset billboard; endpoint motion and original controller incomplete'),
    '0x0c05263b': ('TJTeleEffect_BESPOKE', 'single asset billboard; original teleport controller incomplete'),
    '0x0c052639': ('TCEyesEffect_BESPOKE', 'single asset billboard; original controller incomplete'),
    '0xad92bd21': ('TNakrnothEffect_BESPOKE', 'single asset billboard; original controller incomplete'),
    '0xad92bc19': ('TCataclysmEffect_BESPOKE', 'single asset billboard; original controller incomplete'),
    '0xad92bc17': ('TFunnelEffect_BESPOKE', 'single asset billboard; original controller incomplete'),
    '0xaeaeeb26': ('TMaelstromEffect_BESPOKE', 'ground-flat asset billboard; original controller incomplete'),
    '0xad92bc1b': ('TMagicShieldEffect_BESPOKE', 'tinted asset billboard; original shield geometry incomplete'),
    '0xad92bd33': ('TBuffEffect_Bespoke__Stoneskin_BESPOKE', 'tinted asset billboard; original buff controller incomplete'),
    '0xad92bc16': ('TInvisibleEffect_Bespoke_BESPOKE', 'tinted asset billboard; original invisibility controller incomplete'),
    '0xad92fc13': ('TCataclysmEffect_Bespoke__YCataclysm_BESPOKE', 'own YCataclysm asset billboard; original controller incomplete'),
    '0xae92bc14': ('TManadrainEffect_Bespoke__YManadrain_BESPOKE', 'own YManadrain asset billboard; original controller incomplete'),
    '0xae5eeb26': ('TMaelstromEffect_Bespoke__ymaelstrom_BESPOKE', 'own ymaelstrom asset billboard; original controller incomplete'),
}
# Still absent: Tornado/Vortex/Pulp/Dust/geyser/cfire null bodies; Strike has no
# draw submission. Icedsparks/Snow, IrisFlare/Teleporter, WFall/WCap/RiverFall
# and rejected dragon overrides do not establish the correct animator binding.
# Better authored replacements retain preference over old placeholder bodies.


# Exact identities declared by the registered factories in vfxtest.cpp and
# their owner constructors. These newer implementations predate their ledger
# preview_candidates updates; they never authorize asset/name association.
SOURCE_MAPPINGS = {
    '0x0c052639': 'CEyes_ORIGINAL',
    '0xad99fdf2': 'MPAppear_ORIGINAL',
    '0xaeaeeb31': 'YLose_AUTHORED',
    '0xaeaeeb34': 'YAttack_AUTHORED',
    '0xad92bd26': 'TFairy2_AUTHORED',
    '0xad92bc12': 'TShriek_BESPOKE_NULLSPELL',
    '0xad92bc11': 'TPuke_BESPOKE_NULLSPELL',
    '0xaeaefb27': 'PowerUp_AUTHORED',
    '0xaeaeeb27': 'YPowerUp_AUTHORED',
    '0xe0a3bc42': 'Poison_AUTHORED',
    '0xfacedbed': 'TIcedEffect_BESPOKE__Snow',
    '0x0c052637': 'Dust_AUTHORED',
    '0xad92bd31': 'FGeyser_AUTHORED',
    '0xad92bd34': 'Tironskin_AUTHORED_TAGS',
    '0xad92bd33': 'Tstoneskin_AUTHORED_TAGS',
    '0xd3ae043a': 'TAntimagic_AUTHORED_TAGS',
    '0x8cd3ea0f': 'TDexterity_AUTHORED_TAGS',
    '0xb3ae0fed': 'TRestorelife_AUTHORED_TAGS',
    '0xf32bcfac': 'MeteorStorm_AUTHORED',
    '0xbabfface': 'TQuick_AUTHORED_TAGS',
    '0xadbcef13': 'TCharUtility_AUTHORED_STILL',
    '0xad92bd23': 'TKinSecretDoor_NATIVE_STILL',
    '0xdcc4011d': 'TLabback_AUTHORED_TAGS',
    '0x42e0fcd0': 'TOgrestrength_AUTHORED_TAGS',
    '0xad92bd39': 'TTrollblood_AUTHORED_TAGS',
    '0x10ac03de': 'TRegeneration_AUTHORED_TAGS',
    '0xe0a3bc43': 'TSwiftStrike_AUTHORED_TAGS',
    '0x0c052638': 'Tfspray_AUTHORED_TAGS',
    '0xaeaeeb30': 'TYEnergy_AUTHORED_TAGS',
    '0xaeaeeb33': 'TYEnergyLose_AUTHORED_TAGS',
    '0xaeaeeb29': 'TYAbsorb_AUTHORED_TAGS',
    '0xad92bd38': 'Tnullifier_AUTHORED_TAGS',
    '0xad92bd36': 'TSpeed_AUTHORED_TAGS',
    '0xad92bd35': 'TQuicksilver_AUTHORED_TAGS',
}
IMPLEMENTED.update(dict.fromkeys(SOURCE_MAPPINGS.values(),
    'vfxtest.cpp: explicit retail ID passed to exact owner/profile factory'))


def admitted_candidates(row):
    candidates = [name for name in row['preview_candidates'] if name in IMPLEMENTED]
    explicit = SOURCE_MAPPINGS.get(row.get('retail_type_id', '').lower())
    if explicit: candidates.insert(0, explicit)
    candidates.sort(key=lambda name: (0 if name == explicit else 1 if '_AUTHORED_' in name else 2 if '_BESPOKE' in name.upper() else 3))
    partial = PARTIAL.get(row.get('retail_type_id', '').lower())
    if partial and partial[0] in row['preview_candidates']:
        candidates.append(partial[0])
    return list(dict.fromkeys(candidates))


def candidate_metadata(row, name):
    partial = PARTIAL.get(row.get('retail_type_id', '').lower())
    if partial and name == partial[0]:
        return True, 'Partial preview: ' + partial[1] + '; not complete original implementation'
    return False, 'Implemented preview controller; map caller not certified'



def generate(ledger):
    raw = ledger.read_bytes()
    rows = json.loads(raw)['retail_effects']
    lines = ['// Generated by tools/vfx/generate_review_preview_map.py; do not infer aliases.',
             '// Ledger SHA-256: ' + hashlib.sha256(raw).hexdigest(),
             '// Admission: IMPLEMENTED source audit or exact-ID PARTIAL display; null/invalid aliases excluded.']
    for row in sorted(rows, key=lambda r: int(r['retail_type_id'], 16)):
        for name in admitted_candidates(row):
            partial, description = candidate_metadata(row, name)
            lines.append('{' + row['retail_type_id'] + 'u, ' + json.dumps(name) + ', ' +
                         str(partial).lower() + ', ' + json.dumps(description) + '}, // ' + row['retail_name'])
    return '\n'.join(lines) + '\n'


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--ledger', type=Path, default=ROOT/'docs/vfx/EFFECT_BURNDOWN.json')
    p.add_argument('--output', type=Path, default=ROOT/'src/vfxreviewpreviewmap.inc')
    p.add_argument('--check', action='store_true')
    a = p.parse_args(); result = generate(a.ledger)
    if a.check:
        if a.output.read_text() != result:
            raise SystemExit('Review preview mapping differs from source ledger')
    else:
        a.output.write_text(result)
