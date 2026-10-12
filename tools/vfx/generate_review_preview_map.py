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
    '0xad92bc14': ('TParalize1_NAMED_NO_TARGET', 'actual named no-target branch; target-bound behavior and enormous full trajectory unreviewed'),
    '0xc24b2a85': ('TFogEffect_BESPOKE', 'authored animated grid; original post-blend RGB565 quantization incomplete'),
    '0x00ab1d1d': ('TAuraEffect_BESPOKE', 'billboard preview; original character bones and attachment missing'),
    '0x0ea34fa3': ('TBlastEffect_BESPOKE', 'authored 56-part animation; original oversized-triangle clipping differs'),
    '0xad92bd20': ('TLabyrinthEffect_BESPOKE', 'single asset billboard; original controller incomplete'),
    '0xaeaeeb32': ('TJhagaAttackEffect_BESPOKE', 'stationary asset billboard; endpoint motion and original controller incomplete'),
    '0x0c05263b': ('TJTeleEffect_BESPOKE', 'single asset billboard; original teleport controller incomplete'),
    '0x0c052639': ('TCEyesEffect_BESPOKE', 'single asset billboard; original controller incomplete'),
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
# Legacy absent bodies include Vortex/Pulp/Dust/geyser/cfire; Strike has no
# draw submission. Icedsparks/Snow, IrisFlare/Teleporter, WFall/WCap/RiverFall
# and rejected dragon overrides do not establish the correct animator binding.
# Better authored replacements retain preference over old placeholder bodies.


# Exact identities declared by the registered factories in vfxtest.cpp and
# their owner constructors. These newer implementations predate their ledger
# preview_candidates updates; they never authorize asset/name association.
SOURCE_MAPPINGS = {
    '0xadbcef18': 'Rift1_QUALIFIED_PARSER',
    '0xad92bc1b': 'MagicShield_PUBLIC_AURA',
    '0x00ab1d1d': 'Aura_AUTHENTIC_ACOLYTE_INTEGER',
    '0x55471bff': 'Burn_AUTHENTIC_ACOLYTE_INTEGER',
    '0xdedbdbed': 'Halo_AUTHORED_PUBLIC60',
    '0xddc4042e': 'Blood_AUTHORED',

    '0xad92bd21': 'Nakrnoth_ORIGINAL',
    '0xbabeface': 'Quicksand_ORIGINAL_TARGET',
    '0x5975abde': 'TDripEffect_LINKED_NO_SPLASH',
    '0xad92bc16': 'Invisible_AUTHORED',
    '0xd000f005': 'HeadFireBall_ORIGINAL',
    '0xba09fead': 'Dragonattack_ORIGINAL',
    '0xaeaeeb32': 'JhagaAttack_ORIGINAL',
    '0x224a5adf': 'BarrierEW_ORIGINAL',
    '0x224b5adc': 'BarrierNS_ORIGINAL',
    '0xdefeca7e': 'EssenceDrain_AUTHORED',
    '0xaeaeeb28': 'YTelekinesis_ORIGINAL',
    '0x224a5ccc': 'Smoke_ORIGINAL_QUALITY',
    '0xad92bd1f': 'ArrowEffect_ORIGINAL',
    '0x120054d1': 'IrisFlare_AUTHORED',
    '0x369a5d2e': 'Teleporter_ORIGINAL_NULL',
    '0x838cffba': 'Createfood_ORIGINAL_CONTEXT',
    '0xad92bd1e': 'QueenArrow_ORIGINAL',
    '0xacabfada': 'TIcedsparks_AUTHORED_STILL',
    '0xad92bc17': 'Funnel_ORIGINAL',
    '0x0c05263b': 'Jtele_AUTHORED',
    '0x0c05263c': 'Jteled_AUTHORED',
    '0xad92bd22': 'RockStorm_ORIGINAL',
    '0xad92fc13': 'YCataclysm_ORIGINAL',
    '0xad92bc19': 'Cataclysm_ORIGINAL',
    '0x452dade0': 'TVortexEffect_BESPOKE',
    '0xded3dbed': 'TWindStrip_BESPOKE_NULLSPELL',
    '0xad92bc1c': 'TEnergySpray_BESPOKE_NULLSPELL',
    '0xad99bd33': 'GVortex_ORIGINAL',
    '0xae5eeb26': 'YMaelstrom_ORIGINAL',
    '0xaeaeeb26': 'Maelstrom_ORIGINAL',
    '0x0c052639': 'CEyes_ORIGINAL',
    '0xad99fdf2': 'MPAppear_ORIGINAL',
    '0xaeaeeb31': 'YLose_AUTHORED',
    '0xaeaeeb34': 'YAttack_AUTHORED',
    '0xad92bd26': 'TFairy2_AUTHORED',
    '0xad92bc12': 'TShriek_BESPOKE_NULLSPELL',
    '0xad92bc1a': 'TDragonFire_BESPOKE_NULLSPELL',
    '0xad92bc11': 'TPuke_BESPOKE_NULLSPELL',
    '0xaeaefb27': 'PowerUp_AUTHORED',
    '0xaeaeeb27': 'YPowerUp_AUTHORED',
    '0xe0a3bc42': 'Poison_AUTHORED',
    '0xfacedbed': 'TIcedEffect_BESPOKE__Snow',
    '0x0c052637': 'Dust_AUTHORED',
    '0xad92bd31': 'FGeyser_AUTHORED',
    '0xad92bd27': 'CFire_INSTALLED_NO_PRESETS',
    '0xad92bd30': 'SGeyser_LOADED_INLINE',
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
    '0xbecffeda': 'TSwamp_AUTHORED_TAGS',
    '0xad92bc14': 'TParalize1_CONFIGURED_TARGET',
    '0xad92bc13': 'TManaDrain_CONFIGURED_TARGET',
    '0xae92bc14': 'TYManaDrain_CONFIGURED_TARGET',
    '0x2784abde': 'TTornado_CONFIGURED_TARGET',
    '0x152dafdd': 'TPulp_CONFIGURED_ACTOR',
    '0xad92bd32': 'StrikeEffect_METEOR_CONFIGURED',
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
IMPLEMENTED.update(dict.fromkeys(
    (name for name in SOURCE_MAPPINGS.values()
     if name not in {partial[0] for partial in PARTIAL.values()}),
    'vfxtest.cpp: explicit retail ID passed to exact owner/profile factory'))


def admitted_candidates(row):
    if row.get("retail_type_id", "").lower()=="0x119f01e3":
        return ["Fire_INSTALLED_FACE0"]
    if row.get("retail_type_id", "").lower()=="0xad92bd27":
        return ["CFire_INSTALLED_NO_PRESETS"]
    row=dict(row,preview_candidates=[n for n in row.get('preview_candidates',[]) if n not in ('Burn_AUTHENTIC_ACOLYTE_INTEGER','Aura_AUTHENTIC_ACOLYTE_INTEGER') and not n.startswith('MagicShield_PUBLIC_') and n!='Rift1_QUALIFIED_PARSER'])
    if row.get("retail_type_id", "").lower()=="0xad92bd30":
        return ["SGeyser_LOADED_INLINE"]
    if row.get('retail_type_id','').lower()=='0xad92bd32':
        return ['StrikeEffect_METEOR_CONFIGURED']
    row=dict(row,preview_candidates=[n for n in row.get('preview_candidates',[]) if not n.startswith('Halo_AUTHORED_PUBLIC')])
    if row.get('retail_type_id','').lower()!='0xddc4042e':
        row=dict(row,preview_candidates=[n for n in row.get('preview_candidates',[]) if n!='Blood_AUTHORED'])

    if row.get('retail_type_id','').lower()=='0xad92bd21':
        return ['Nakrnoth_ORIGINAL']
    if row.get('retail_type_id','').lower()=='0x838cffba':
        return ['Createfood_ORIGINAL_CONTEXT']
    if row.get('retail_type_id','').lower()=='0x369a5d2e':
        return ['Teleporter_ORIGINAL_NULL']
    if row.get('retail_type_id','').lower()=='0x5975abde':
        return ['TDripEffect_LINKED_NO_SPLASH']
    candidates = [name for name in row['preview_candidates'] if name in IMPLEMENTED
                  and name not in ('Nakrnoth_ORIGINAL','TDripEffect_LINKED_NO_SPLASH','Teleporter_ORIGINAL_NULL','Createfood_ORIGINAL_CONTEXT')]
    explicit = SOURCE_MAPPINGS.get(row.get('retail_type_id', '').lower())
    if explicit: candidates.insert(0, explicit)
    candidates.sort(key=lambda name: (0 if name == explicit else 1 if '_AUTHORED_' in name else 2 if '_BESPOKE' in name.upper() else 3))
    partial = PARTIAL.get(row.get('retail_type_id', '').lower())
    if partial and partial[0] in row['preview_candidates']:
        candidates.append(partial[0])
    return list(dict.fromkeys(candidates))


def candidate_metadata(row, name):
    if row.get("retail_type_id", "").lower()=="0x119f01e3" and name=="Fire_INSTALLED_FACE0":
        return True, "Shipped default Fire: exact static quad/frame0, blank facing0; legacy procedural scatter is not this retail implementation, pixel sampling and natural caller remain open"
    if row.get("retail_type_id", "").lower()=="0xad92bd27" and name=="CFire_INSTALLED_NO_PRESETS":
        return False, "Exact installed Misc/cfire authored base sprites after genuine missing cfire/csparks presets; real kill30, intended particle definitions and natural caller remain open"

    if row.get('retail_type_id','').lower()=='0xadbcef18' and name=='Rift1_QUALIFIED_PARSER':
        return False, 'Qualified Rift1 malformed animtex d-as-V repair; actual own quad/row arithmetic/kill100, original unpatched failure retained; natural caller/device not certified'
    if row.get('retail_type_id','').lower()=='0xad92bc1b' and name=='MagicShield_PUBLIC_AURA':
        return False, 'Exact MagicShield public SetSpell(real Acolyte/named Aura), unmatched-name constructor-storage0 control; natural MagicShield tiers/caller/audio not certified'
    if row.get('retail_type_id','').lower()=='0x00ab1d1d' and name=='Aura_AUTHENTIC_ACOLYTE_INTEGER':
        return False, 'Exact Aura public SetSpell(real Acolyte/named Aura), integer pose14/0/facing96 and original zero selector; natural bone initialization, attachment/interpolation/audio not certified'
    if row.get('retail_type_id','').lower()=='0x55471bff' and name=='Burn_AUTHENTIC_ACOLYTE_INTEGER':
        return False, 'Exact Burn public Set(real Acolyte), declared integer pose14/0/facing96, real damage/fire/smoke; natural attachment/interpolation not certified'
    if row.get("retail_type_id", "").lower()=="0xad92bd30" and name=="SGeyser_LOADED_INLINE":
        return False, "Exact loaded sgeyser inline104-slot controller and authored burst, original missing vapor preset omitted; declared empty ground0 and finite45-tick owner, natural trap/map/audio not certified"
    if row.get('retail_type_id','').lower()=='0xad92bd32' and name=='StrikeEffect_METEOR_CONFIGURED':
        return False, 'Named Meteor Storm caller: four delayed Comet-image strikes, duration93 and genuine configured Acolytes; live RNG plus original index reseeds, empty ground0, natural scheduling/damage and other variants not certified'
    if row.get('retail_type_id','').lower()=='0xdedbdbed' and name=='Halo_AUTHORED_PUBLIC60':
        return False, 'Exact Halo authored mesh/public InitParams(60,0.04), explicit test inputs; natural retail caller not certified'
    if row.get('retail_type_id','').lower()=='0xddc4042e' and name=='Blood_AUTHORED':
        return False, 'Exact Blood public SetParams(40,224,3,5,5,1) spray, live global RNG without reseed; natural actor damage/map insertion not certified'

    if row.get('retail_type_id','').lower()=='0xad92bd21' and name=='Nakrnoth_ORIGINAL':
        return False, 'Original null-spell fixed-owner Nakrnoth,63 authored objects/ten-emitter controller and real kill775; natural boss/spell/audio/device and exact pixels not certified'
    if row.get('retail_type_id','').lower()=='0x838cffba' and name=='Createfood_ORIGINAL_CONTEXT':
        return False, 'Createfood quality3 white-light0/ambient32 lookup context, failed food insertion clock0, real kill80; natural lights, food/map service and actors not certified'
    if row.get('retail_type_id','').lower()=='0x369a5d2e' and name=='Teleporter_ORIGINAL_NULL':
        return False, 'Original null-spell Teleporter portal, five cylinders and real kill51; successful teleport, actors and other spell variants not certified'
    if row.get('retail_type_id','').lower()=='0xbabeface' and name=='Quicksand_ORIGINAL_TARGET':
        return False, 'Configured Quicksand stationary target offset(96,0,0), actual56-tick lifetime; natural actors, damage and other variants not certified'
    if row.get('retail_type_id','').lower()=='0x5975abde' and name=='TDripEffect_LINKED_NO_SPLASH':
        return False, 'Persistent Drip SetParams(20,128,1), managed no-splash Ripple children; natural map residency and audio not certified'
    if row.get('retail_type_id','').lower()=='0xba09fead' and name=='Dragonattack_ORIGINAL':
        return False, 'Declared Fighting caller: target offset(280,0), ground0, face96 muzzle(14,14,+100); arbitrary endpoints and natural dragon actors not certified'
    if row.get('retail_type_id','').lower()=='0x152dafdd' and name=='TPulp_CONFIGURED_ACTOR':
        return False, 'Configured genuine Acolyte public Set: velocity(0,0,8), five chunks/drops/splats, empty terrain and failed blood tiles with declared clock0; persistent chunks, natural mapped caller not certified'
    if row.get('retail_type_id','').lower()=='0x2784abde' and name=='TTornado_CONFIGURED_TARGET':
        return False, 'Exact Tornado owner with explicit genuine Acolytes and loaded Tornado variant; natural variant selects Funnel, so no natural-cast or successful-damage claim'
    if row.get('retail_type_id','').lower()=='0xae92bc14' and name=='TYManaDrain_CONFIGURED_TARGET':
        return False, 'Configured genuine Acolyte fighting target: offset(96,0,0), YManaDrain variant; original uncapped mana transfer and finite completion; arbitrary moving actors not certified'
    if row.get('retail_type_id','').lower()=='0xad92bc13' and name=='TManaDrain_CONFIGURED_TARGET':
        return False, 'Configured genuine Acolyte fighting target: offset(96,0,0), Mana Drain variant; actual mana transfer and completion; arbitrary moving actors not certified'
    if row.get('retail_type_id','').lower()=='0xad92bc14' and name=='TParalize1_CONFIGURED_TARGET':
        return False, 'Configured genuine Acolyte target: offset(96,0,0), radius16; original variant1 lifecycle; arbitrary targets and bones not certified'
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
