import json
import os
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import jev  # noqa: E402

DECISION_RESPONSE = {
    'id': 'gen-dec-1', 'model': 'typesafe/jev-1.13-20260917', 'provider': 'TypeSafe',
    'answers': {
        'move': {'type': 'choice', 'choice': 'block', 'confidence': 0.39,
                 'probabilities': {'swing': 0.01, 'block': 0.47, 'dodge_back': 0.52}},
        'heal': {'type': 'noul', 'noul': 0.75},
        'threat': {'type': 'score', 'score': 1.6, 'confidence': 0.8,
                   'probabilities': {'0': 0.0, '1': 0.4, '2': 0.6},
                   'legend': {'0': 'none', '1': 'some', '2': 'deadly'}}},
    'usage': {'input_tokens': 500, 'output_tokens': 70, 'cost': 2e-05}}


class Recorder:
    def __init__(self, response):
        self.response = response
        self.calls = []

    def __call__(self, url, body, headers):
        self.calls.append((url, body, headers))
        return self.response


class Decisions(unittest.TestCase):
    def setUp(self):
        os.environ.setdefault('OPEN_ROUTER_KEY', 'test-key')

    def test_request_shape_answers_and_ledger(self):
        post = Recorder(DECISION_RESPONSE)
        with tempfile.TemporaryDirectory() as tmp:
            ledger = jev.Ledger(Path(tmp) / 'session.rvlog')
            client = jev.Jev(ledger=ledger, seed=7, post=post)
            answers = client.ask(
                {'enemy': {'action': 'winding up'}}, tag='combat',
                move=jev.Choice('Which move?', {'swing': 'fast', 'block': 'guard', 'dodge_back': 'away'}),
                heal=jev.Noul('Heal now?', true='low health', false='fine'),
                threat=jev.Score('How dangerous?', ['none', 'some', 'deadly']))
            logged = [json.loads(line) for line in ledger.path.read_text().splitlines()]

        url, body, headers = post.calls[0]
        self.assertEqual(url, jev.DECISIONS_URL)
        self.assertEqual(body['model'], 'typesafe/jev-1.13')
        self.assertEqual(headers['Authorization'], f"Bearer {os.environ['OPEN_ROUTER_KEY']}")
        self.assertEqual(set(body['questions']['move']['criteria']), {'swing', 'block', 'dodge_back'})
        self.assertEqual(body['questions']['heal']['criteria'], {'true': 'low health', 'false': 'fine'})
        self.assertEqual(body['questions']['threat']['criteria'], ['none', 'some', 'deadly'])

        self.assertEqual(answers['move'].choice, 'block')
        self.assertAlmostEqual(answers['move'].margin, 0.05)
        self.assertTrue(answers['heal'].yes(0.7))
        self.assertEqual(answers['threat'].level, 2)

        self.assertEqual(ledger.calls, 1)
        self.assertAlmostEqual(ledger.cost, 2e-05)
        self.assertEqual(logged[0]['tag'], 'combat')
        self.assertEqual(logged[0]['answers']['heal'], {'type': 'noul', 'noul': 0.75})

    def test_choice_order_is_shuffled_reproducibly(self):
        options = {name: name for name in 'abcdefgh'}
        orders = []
        for _ in range(2):
            post = Recorder({'answers': {'q': {'type': 'choice', 'choice': 'a', 'probabilities': {}}}})
            client = jev.Jev(seed=3, post=post)
            for _ in range(4):
                client.ask({}, q=jev.Choice('pick', options))
            orders.append([list(body['questions']['q']['criteria']) for _, body, _ in post.calls])
        self.assertEqual(orders[0], orders[1])                       # same seed, same orders
        self.assertGreater(len({tuple(o) for o in orders[0]}), 1)    # but they do vary
        unshuffled = Recorder({'answers': {'q': {'type': 'choice', 'choice': 'a', 'probabilities': {}}}})
        jev.Jev(post=unshuffled).ask({}, q=jev.Choice('pick', options, shuffle=False))
        self.assertEqual(list(unshuffled.calls[0][1]['questions']['q']['criteria']), list(options))

    def test_missing_answer_is_an_error(self):
        client = jev.Jev(post=Recorder({'answers': {}}))
        with self.assertRaises(jev.JevError):
            client.ask({}, q=jev.Noul('yes?'))


class RouterCalls(unittest.TestCase):
    def setUp(self):
        os.environ.setdefault('OPEN_ROUTER_KEY', 'test-key')

    def test_images_tools_and_reply(self):
        response = {'model': 'openai/gpt-6.1-sol', 'usage': {'cost': 0.0045},
                    'choices': [{'message': {'content': 'A dialog with three choices.', 'tool_calls': [
                        {'type': 'function', 'function': {'name': 'click', 'arguments': '{"x": 145, "y": 301}'}}]}}]}
        post = Recorder(response)
        with tempfile.TemporaryDirectory() as tmp:
            png = Path(tmp) / 'f.png'
            png.write_bytes(b'\x89PNG fake')
            router = jev.Router(post=post)
            reply = router.ask('What now?', images=[png],
                               tools=[jev.tool('click', 'Click', {'x': {'type': 'integer'}, 'y': {'type': 'integer'}}, ['x', 'y'])])
        url, body, _ = post.calls[0]
        self.assertEqual(url, jev.CHAT_URL)
        self.assertEqual(body['model'], 'typesafe/jev-router')
        self.assertEqual(body['plugins'], [{'id': 'jev-router', 'cost_tier': 'medium'}])
        parts = body['messages'][0]['content']
        self.assertTrue(parts[1]['image_url']['url'].startswith('data:image/png;base64,'))
        self.assertEqual(reply.tool_calls, [jev.ToolCall('click', {'x': 145, 'y': 301})])
        self.assertEqual(reply.model, 'openai/gpt-6.1-sol')
        self.assertEqual(router.ledger.calls, 1)


@unittest.skipUnless(os.environ.get('PLAYTEST_LIVE') == '1', 'set PLAYTEST_LIVE=1 to call OpenRouter')
class Live(unittest.TestCase):
    """One real decision: the model and the request shape still agree."""

    def test_one_decision(self):
        answers = jev.Jev().ask(
            {'locke': {'health': 'low'}, 'enemy': {'distance': 'melee reach', 'action': 'winding up a heavy swing'}},
            heal=jev.Noul('Should Locke step back and drink a healing potion now?'))
        self.assertGreaterEqual(answers['heal'].p, 0.0)


if __name__ == '__main__':
    unittest.main()
