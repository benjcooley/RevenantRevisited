#!/usr/bin/env python3
"""Route audited retail type IDs to controls and preserve separate evidence gates."""
import argparse
import hashlib
import json
from pathlib import Path
from controls import Controller

ROOT=Path(__file__).resolve().parents[2]
BASELINE=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
LEDGER=ROOT/'docs/vfx/EFFECT_BURNDOWN.json'


def canonical_id(value):
    return f'0x{int(value,0) if isinstance(value,str) else int(value):08x}'


def sample_actions(profile,type_id,descriptor):
    params={'type_id':type_id} if profile in ('static_mesh','colored_ribbon','fountain','flame') else {}
    if profile=='ripple_no_splash':params={'length':20}
    if descriptor['kind']=='projectile':
        actions=[dict(op='create_entity',id='source',position=[0,0,128]),
            dict(op='create_entity',id='target',position=[1000,0,128],stats=dict(hp=0,enemy=True)),
            dict(op='spawn_effect',id='fx',type=profile,source_id='source',destination_id='target',launch_params=params),
            dict(op='advance_ticks',id='fx',ticks=6),dict(op='validate_execution_contract',id='fx')]
    else:
        actions=[dict(op='create_effect',id='fx',type=profile,params=params)]
        if profile not in ('static_mesh','colored_ribbon'):
            actions.append(dict(op='advance_ticks',id='fx',ticks=6))
    actions.append(dict(op='capture',id='fx',kind='pixels' if 'capture_pixels' in descriptor['actions'] else 'state',
        output='reference.png' if 'capture_pixels' in descriptor['actions'] else 'state.json'))
    return actions


def generate(ledger_path=LEDGER,executable=BASELINE):
    ledger_path=Path(ledger_path).resolve();raw=ledger_path.read_bytes();ledger=json.loads(raw)
    capabilities=Controller(executable).capabilities()
    routes={canonical_id(entry['type_id']):entry for entry in capabilities['verified_retail_types']}
    reports={}
    def report(reference):
        if reference not in reports:
            path=ROOT/reference
            if path.is_file():
                data=path.read_bytes();value=json.loads(data)
                diffs=[]
                def measured_pixels(case):
                    for key in ('differing_rgb565_pixels','pixel_differences','different_pixels','differing_pixels','color_different_bytes'):
                        if type(case.get(key)) is int:diffs.append(case[key])
                    for key in ('cases','poses','pairs'):
                        for child in case.get(key,[]):measured_pixels(child)
                measured_pixels(value)
                total=value.get('differing_rgb565_pixels')
                pixel_checked=bool(diffs) or type(total) is int or value.get('shared_original_raster_pixel_status')=='pass'
                pixel_equal=pixel_checked and (not diffs or max(diffs)==0) and (type(total) is not int or total==0)
                reports[reference]=dict(path=reference,exists=True,sha256=hashlib.sha256(data).hexdigest(),
                    reported_status=value.get('status'),pixel_checked=pixel_checked,pixels_equal=pixel_equal,
                    sampled_pixel_cases=len(diffs),scope=value.get('scope'),
                    actual_metal_compared=value.get('actual_metal_compared',value.get('metal_backend_compared',False)))
            else:reports[reference]=dict(path=reference,exists=False)
        return reports[reference]
    types={};rendered=[]
    for row in ledger['retail_effects']:
        type_id=canonical_id(row['retail_type_id']);route=routes.get(type_id)
        checks=[]
        for check in row.get('thin_emulator_checks',[]):
            summary=report(check['report']) if check.get('report') else {}
            checks.append(dict(name=check['name'],status=check['status'],scope=check.get('scope'),
                report=summary,open_gates=check.get('open',[]),
                modern_gpu_backend_compared=check.get('modern_gpu_backend_compared',False),
                runtime_producer_compared=check.get('runtime_producer_compared',False)))
        passed=[c for c in checks if c['status']=='pass' and c['report'].get('reported_status')=='pass']
        pixel_passed=[c for c in passed if c['report'].get('pixels_equal')]
        if pixel_passed:rendered.append(type_id)
        gpu=[c for c in checks if c['modern_gpu_backend_compared'] or c['report'].get('actual_metal_compared')]
        gpu_status='differences_found' if any('differ' in c['status'] or c['report'].get('reported_status')=='diagnostic' for c in gpu) else ('bounded_pass' if any(c['status']=='pass' for c in gpu) else 'not_checked')
        acceptance=row['acceptance']
        command_evidence={key:row[key] for key in ('command_runtime_validation','runtime_regression','command_lifecycle_validation') if key in row}
        control=None
        if route:
            descriptor=capabilities['profiles'][route['profile']]
            control=dict(profile=route['profile'],kind=descriptor['kind'],parameters=dict(type_id=type_id) if route['profile'] in ('static_mesh','colored_ribbon','fountain','flame') else {},
                actions=descriptor['actions'],named_build_contract_supported=descriptor['named_build_contract_supported'],
                input_fields=descriptor['input_fields'],example_actions=sample_actions(route['profile'],type_id,descriptor),
                scope=descriptor['description'])
        types[type_id]=dict(retail_name=row['retail_name'],ledger_key=row['key'],asset=row['asset'],
            control=control,ledger_acceptance=dict(acceptance),gates=dict(
                state_frontend=dict(status='bounded_pass' if passed else 'not_checked',checks=[c['name'] for c in passed],
                    scope='Only stated kernel/mesh/controller subset, not complete effect behavior'),
                pixel_frontend=dict(status='shared_original_raster_pass' if pixel_passed else 'not_checked',
                    checks=[c['name'] for c in pixel_passed],modern_renderer=False),
                runtime=dict(typed_fixture_available=bool(route),natural_trigger_verified=bool(acceptance.get('runtime_trigger_integration')),
                    natural_context_verified=bool(acceptance.get('map_or_character_context')),command_evidence=command_evidence,
                    scope='Fixture lifecycle, command lifecycle and natural game triggers are distinct'),
                gpu=dict(status=gpu_status,checks=[c['name'] for c in gpu]),
                visual_review=dict(status='pass_for_tested_configuration' if acceptance.get('visual_fidelity') else 'pending',
                    review=row.get('visual_review'),
                    scope='Reviewed actual port appearance against retail; retain the recorded configuration and limitations'),
                full_acceptance=dict(status='accepted' if acceptance.get('accepted') else 'open')),
            evidence=checks,ledger_next_action=row['next_action'],routing_next_step=
                ('Use typed controls for the stated subset; pursue open runtime/map/GPU gates separately' if route else
                 'No direct adapter: reuse evidence/auxiliary kernel only within its scope, then add and validate an exact-type adapter'))
    return dict(schema_version=1,source=dict(ledger=str(ledger_path.relative_to(ROOT)),
        ledger_sha256=hashlib.sha256(raw).hexdigest(),controls_sha256=hashlib.sha256(Path(__file__).with_name('controls.py').read_bytes()).hexdigest(),
        pixel_controls_sha256=hashlib.sha256(Path(__file__).with_name('pixel_controls.py').read_bytes()).hexdigest(),
        executable_sha256=capabilities['build']['executable_sha256']),counts=dict(
            ledger_rows=len(types),exposed_real_type_ids=len(routes),ledger_reported_render_frontend_rows=ledger['counts'].get('thin_bounded_render_frontend_rows'),
            evidence_rows_with_shared_raster_pixel_pass=len(rendered),
            reviewed_visual_pass_rows=sum(bool(r['acceptance'].get('visual_fidelity')) for r in ledger['retail_effects']),
            remaining_visual_review_rows=sum(not r['acceptance'].get('visual_fidelity') for r in ledger['retail_effects']),
            fully_accepted_rows=sum(bool(r['acceptance'].get('accepted')) for r in ledger['retail_effects'])),
        exposed_without_ledger=[type_id for type_id in routes if type_id not in types],
        rendered_without_direct_adapter=[type_id for type_id in rendered if type_id not in routes],
        auxiliary_profiles={name:value for name,value in capabilities['profiles'].items() if not value['catalog']},
        types=types,policy='Controls, bounded original-raster/frontend proof, natural runtime/context, modern GPU and full acceptance are separate. Never promote one from another.')


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output',type=Path,default=ROOT/'recon/retail_asm/runtime/profile-coverage.json')
    parser.add_argument('--type-id',help='Print one exact routing record instead of the full map')
    args=parser.parse_args();coverage=generate()
    if args.type_id:print(json.dumps(coverage['types'][canonical_id(args.type_id)],indent=2));return
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(coverage,indent=2)+'\n')
    print(json.dumps(coverage['counts'],indent=2))


if __name__=='__main__':main()
