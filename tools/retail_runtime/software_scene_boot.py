"""Boot retail's original SOFTWARE3D configuration for natural scene references.

The HUD slot's default fixture intentionally selects a hybrid hardware-style
texture overlay with the internal software raster. Its postcondition rejects
NoTexOverlay. This fixture changes only WinMain's command-line input and verifies
the original genuine software configuration instead; no guest flags are patched.
"""
import argparse
import json
from pathlib import Path

from character_native_preflight import ROOT, sha
import hudworld
from native_store_loop import install

COMMAND_LINE = b'NOSOUND SOFTWARE3D'


class SoftwareSceneWorld(hudworld.HudWorld):
    def _install_hooks(self):
        super()._install_hooks()
        self.store_acceleration = install(self.vm)

    def __init__(self, executable):
        previous = hudworld.COMMAND_LINE
        hudworld.COMMAND_LINE = COMMAND_LINE
        try:
            try:
                super().__init__(executable)
            except RuntimeError as error:
                # All real boot, allocation, APIs and game-init postconditions
                # still apply. Only the HUD slot's intentional hybrid mode
                # assertion is replaced by this fixture's software assertion.
                if str(error) != 'Boot did not reach the texture-overlay / internal-raster mode':
                    raise
                if self.vm.u32(hudworld.G_NOTEXOVERLAY) != 1 or self.vm.u32(hudworld.G_INTERNAL_RASTER) != 1:
                    raise RuntimeError('Original SOFTWARE3D did not select NoTexOverlay/internal raster') from error
            else:
                raise RuntimeError('Original SOFTWARE3D unexpectedly retained hybrid texture overlays')
        finally:
            hudworld.COMMAND_LINE = previous


def run(output):
    output.mkdir(parents=True, exist_ok=False)
    executable = ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
    world = SoftwareSceneWorld(executable)
    # The basename keeps the original boot-source fingerprint for the existing
    # authentic-owner constructor check. A private output directory separates
    # this different configuration from the default HUD fixture's cache.
    checkpoint = output/hudworld.HudWorld.image_path(executable).name
    world.save(checkpoint)
    report = dict(status='original_software3d_boot_complete', accepted=False,
        command_line=COMMAND_LINE.decode('ascii'), image=str(checkpoint), image_sha256=sha(checkpoint),
        executable_sha256=sha(executable), source_sha256=sha(__file__),
        boot_source_sha256={str(p):sha(p) for p in hudworld.BOOT_SOURCES},
        notexoverlay=world.vm.u32(hudworld.G_NOTEXOVERLAY),
        internal_raster=world.vm.u32(hudworld.G_INTERNAL_RASTER),
        store_acceleration={k:value for k,value in world.store_acceleration.items() if not k.endswith('hook')},
        scope=__doc__)
    (output/'manifest.json').write_text(json.dumps(report, indent=2)+'\n')
    print(report['status'], checkpoint, flush=True)
    return report


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--output', type=Path, required=True)
    a = p.parse_args()
    run(a.output)
