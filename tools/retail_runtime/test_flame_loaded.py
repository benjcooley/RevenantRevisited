"""Real loaded base Flame: no double Pulse, complete cycles and fresh-VM replay."""
import hashlib
import unittest

from blood_actor_preflight import BOOT
from flame_loaded_reference import LoadedFlame, ROOT


class LoadedFlameTests(unittest.TestCase):
    def test_original_loaded_owner_and_all_render_phases_repeat(self):
        replays = []
        for _ in range(2):
            f = LoadedFlame(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe', BOOT)
            rows = []
            for tick in range(37):
                if tick:
                    f.step(tick)
                image = f.render()
                self.assertEqual(f.pulses, tick)
                self.assertEqual(f.vm.u32(f.animator + 0xfc), tick % 18)
                self.assertEqual(len(f.draws), 1)
                self.assertEqual(f.draws[0]['fvf'], 0x1e2)
                self.assertNotEqual(image, bytes(len(image)))
                rows.append((hashlib.sha256(image).hexdigest(), f.draws))
            self.assertEqual(rows[:18], rows[18:36])
            self.assertEqual(rows[0], rows[36])
            self.assertEqual(len({r[0] for r in rows}), 8)
            replays.append(rows)
        self.assertEqual(*replays)


if __name__ == '__main__':
    unittest.main()
