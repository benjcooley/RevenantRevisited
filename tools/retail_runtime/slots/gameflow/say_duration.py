#!/usr/bin/env python3
"""Gameflow A/B target 2, retail side: how long TCharacter::Say holds a line.

Runs the original `TCharacter::Say` (`0x004d0610`) for each case, once with
the voice found (its length given) and once without, and reports the ticks
it sets (the say action's `wait`, `+0x28`, and what it passes to
`DialogPane.AddSpeech`). Schema `gameflow.sayduration.v1`, shared with the
port's `Revenant --retail-ab=say-duration`.

Original code: Say, `DialogLine` (`0x00533dd0`), the action block
constructor (`0x004da9f0`), strdup and the x87 duration arithmetic.

Fixture: the speaker is a zeroed 0x300-byte object with a synthetic vtable:
Health (slot `0x1c0`) answers 100, TryCommand (slot `0x218`) records the
action block, any other slot fails the case. Single player
(`0x0066829c` = 0), PlaySpeech on (`0x005d7a60` = 1, the ini default),
ShowDialog on (`0x00668188` = 1, the shipped ini). Recorded boundaries: the
sound player's FindSound `0x0049c430` (the case says whether the voice is
found), Mount `0x0049b650` (succeeds), Play `0x0049b990` (succeeds), the
sample length `0x0049c640` (the case's milliseconds; Miles does not run
here), and `TDialogPane::AddSpeech` `0x00535b90`.

    say_duration.py EXE --serve     # JSONL: {"id":..,"case":{name,text_hex,frames,sound,voice_ms}}
"""
from __future__ import annotations

import argparse
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from fixture import Boundaries, s32, serve, start  # noqa: E402

SCHEMA = 'gameflow.sayduration.v1'
SAY = 0x4d0610
FIND_SOUND, MOUNT, PLAY, LENGTH = 0x49c430, 0x49b650, 0x49b990, 0x49c640
ADD_SPEECH = 0x535b90
PLAY_SPEECH, SHOW_DIALOG, MULTIPLAYER = 0x5d7a60, 0x668188, 0x66829c
VOICE_ID = 7


class SayFixture:
    def __init__(self, executable):
        self.vm, self.sha = start(executable)
        vm = self.vm
        self.b = Boundaries(vm)
        self.state = {}
        b = self.b
        b.add(FIND_SOUND, 'FindSound', 4, self._find)
        b.add(MOUNT, 'Mount', 4, lambda a, c: 1)
        b.add(PLAY, 'Play', 0x18, lambda a, c: 1)
        b.add(LENGTH, 'Length', 4, lambda a, c: self.state['voice_ms'])
        b.add(ADD_SPEECH, 'AddSpeech', 0xc, self._add_speech)
        vtable = vm.allocate(0x400)
        unexpected = b.add_stub('unexpected', 0, self._unexpected)
        for slot in range(0x100):
            vm.put_u32(vtable + 4 * slot, unexpected)
        vm.put_u32(vtable + 0x1c0, b.add_stub('Health', 0, lambda a, c: 100))
        vm.put_u32(vtable + 0x218, b.add_stub('TryCommand', 0xc, self._try_command))
        self.speaker = vm.allocate(0x300)
        vm.put_u32(self.speaker, vtable)
        vm.put_u32(PLAY_SPEECH, 1)
        vm.put_u32(SHOW_DIALOG, 1)
        vm.put_u32(MULTIPLAYER, 0)
        vm.checkpoint()

    def _unexpected(self, args, ecx):
        raise RuntimeError('Say called a speaker method the fixture does not provide')

    def _find(self, args, ecx):
        self.state['asked'] = self.vm.string(args[0])
        return VOICE_ID if self.state['found'] else 0xffffffff

    def _try_command(self, args, ecx):
        ab = args[0]
        self.state['action'] = dict(wait=s32(self.vm.u32(ab + 0x28)),
                                    text=bool(self.vm.u32(ab + 0x5c)),
                                    flags=self.vm.u32(ab + 0x60))
        return 1

    def _add_speech(self, args, ecx):
        self.state['speech'] = dict(line=self.vm.string(args[1]), ticks=s32(args[2]))
        return 1

    def say(self, text: bytes, frames, sound, found, voice_ms):
        vm = self.vm
        vm.restore()
        self.b.calls = []
        self.state = dict(found=found, voice_ms=voice_ms)
        tp = vm.allocate(len(text) + 2)
        vm.write(tp, text + b'\0\0')      # DialogLine reads two bytes at a time
        sp = 0
        if sound is not None:
            sp = vm.allocate(len(sound) + 1)
            vm.write(sp, sound.encode('cp1252') + b'\0')
        result = vm.call(SAY, (tp, frames & 0xffffffff, 0, sp), this=self.speaker, instruction_limit=10_000_000)
        action, speech = self.state.get('action'), self.state.get('speech')
        return dict(result=result,
                    wait=action['wait'] if action else None,
                    ticks=speech['ticks'] if speech else None,
                    line_len=len(speech['line'].encode('cp1252')) if speech else None,
                    action_text=action['text'] if action else None,
                    sound_calls=[c for c in self.b.calls if c in ('FindSound', 'Mount', 'Play', 'Length')])

    def run(self, case):
        text = bytes.fromhex(case['text_hex'])
        out = dict(schema=SCHEMA, side='retail', case=case['name'])
        out['novoice'] = self.say(text, case['frames'], case.get('sound'), False, 0)
        if case.get('sound') is not None and case.get('voice_ms') is not None:
            out['voice'] = self.say(text, case['frames'], case['sound'], True, case['voice_ms'])
        if case.get('sweep'):
            # The voice's length swept over [first, last] (ms): Say's wait for each.
            first, last, step = case['sweep']
            out['sweep'] = [self.say(text, case['frames'], case.get('sound') or 'SWEEP', True, ms)['wait']
                            for ms in range(first, last + 1, step)]
        return out


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable', type=Path)
    parser.add_argument('--serve', action='store_true', required=True)
    args = parser.parse_args()
    started = time.perf_counter()
    fixture = SayFixture(args.executable)
    serve(fixture.sha, SCHEMA, (time.perf_counter() - started) * 1000, fixture.run)


if __name__ == '__main__':
    main()
