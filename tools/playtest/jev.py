"""Jev on OpenRouter: typed decisions (System One) and the router (System Two).

    jev = Jev(ledger=Ledger(bundle / 'session.rvlog'))
    answers = jev.ask({'enemy': {...}, 'locke': {...}},
                      move=Choice('Which move should Locke make now against `enemy`?',
                                  {'block': 'Raise guard ...', 'thrust': 'Average attack ...'}),
                      heal=Noul('Should Locke drink a healing potion now?',
                                true='Health is low enough that ...', false='...'))
    answers['move'].choice, answers['move'].confidence, answers['heal'].p

    router = Router(ledger=...)
    reply = router.ask('What is on this screen?', images=[png], tools=[...])

`typesafe/jev-1.13` answers Choice, Noul and Score questions about a text
state, with probabilities, in about 350 ms; it is text only, weak at
arithmetic and literal-minded, and leans toward a Choice's first option
(docs/playtest/PLAN.md §2). So this client shuffles each Choice's options
(seeded, recorded in the ledger) and leaves numbers, distances and paths to
the caller. `typesafe/jev-router` routes a chat request with images and
tools to a model it picks.

The key comes from $OPEN_ROUTER_KEY (or $OPENROUTER_API_KEY). Every call is
appended to the ledger with its latency and cost.
"""
from __future__ import annotations

import base64
import json
import os
import random
import time
import urllib.error
import urllib.request
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any, Callable, Mapping, Sequence

DECISIONS_URL = 'https://openrouter.ai/api/alpha/decisions'
CHAT_URL = 'https://openrouter.ai/api/v1/chat/completions'
DECISION_MODEL = 'typesafe/jev-1.13'          # pinned: thresholds are tuned to a version
ROUTER_MODEL = 'typesafe/jev-router'
RETRY_STATUS = {408, 429, 500, 502, 503, 504}


class JevError(RuntimeError):
    pass


def api_key() -> str:
    key = os.environ.get('OPEN_ROUTER_KEY') or os.environ.get('OPENROUTER_API_KEY')
    if not key:
        raise JevError('set OPEN_ROUTER_KEY to an OpenRouter API key')
    return key


# ---- the ledger ----------------------------------------------------------

@dataclass
class Ledger:
    """Every call: kind, model, latency, cost, and what was asked. Appended
    to a JSON Lines file when one is given."""
    path: Path | None = None
    calls: int = 0
    cost: float = 0.0
    seconds: float = 0.0
    context: dict = field(default_factory=dict)  # merged into each entry: the caller's tick, say

    def record(self, entry: dict) -> None:
        entry = {**self.context, **entry}
        self.calls += 1
        self.cost += entry.get('cost') or 0.0
        self.seconds += entry.get('seconds') or 0.0
        if self.path:
            with Path(self.path).open('a', encoding='utf-8') as f:
                f.write(json.dumps(entry, separators=(',', ':')) + '\n')

    def summary(self) -> str:
        return f'{self.calls} calls, ${self.cost:.5f}, {self.seconds:.1f} s'


# ---- transport -----------------------------------------------------------

Post = Callable[[str, dict, dict], dict]


def http_post(url: str, body: dict, headers: dict, timeout: float = 120.0,
              attempts: int = 4) -> dict:
    """POST JSON and return the decoded response, retrying rate limits and
    server errors with exponential backoff."""
    data = json.dumps(body).encode('utf-8')
    for attempt in range(attempts):
        request = urllib.request.Request(url, data, {'Content-Type': 'application/json', **headers})
        try:
            with urllib.request.urlopen(request, timeout=timeout) as response:
                return json.loads(response.read())
        except urllib.error.HTTPError as error:
            detail = error.read().decode('utf-8', 'replace')[:500]
            if error.code not in RETRY_STATUS or attempt == attempts - 1:
                raise JevError(f'{url}: HTTP {error.code}: {detail}') from None
        except (urllib.error.URLError, TimeoutError) as error:
            if attempt == attempts - 1:
                raise JevError(f'{url}: {error}') from None
        time.sleep(0.5 * 2 ** attempt)
    raise AssertionError('unreachable')


# ---- questions -----------------------------------------------------------

Guidance = str | Mapping[str, Any] | Sequence[Any]


@dataclass(frozen=True)
class Choice:
    """Pick one option. `options` maps each option's name to what it means."""
    instructions: Guidance
    options: Mapping[str, Guidance]
    shuffle: bool = True

    def request(self, rng: random.Random) -> dict:
        names = list(self.options)
        if self.shuffle:
            rng.shuffle(names)
        return {'type': 'choice', 'instructions': self.instructions,
                'criteria': {name: self.options[name] for name in names}}


@dataclass(frozen=True)
class Noul:
    """A yes/no question; the answer is the probability of yes."""
    instructions: Guidance
    true: Guidance | None = None
    false: Guidance | None = None

    def request(self, rng: random.Random) -> dict:
        question = {'type': 'noul', 'instructions': self.instructions}
        if self.true is not None or self.false is not None:
            question['criteria'] = {'true': self.true or 'Yes.', 'false': self.false or 'No.'}
        return question


@dataclass(frozen=True)
class Score:
    """Place the state on ordered levels, lowest first."""
    instructions: Guidance
    levels: Sequence[Guidance]

    def request(self, rng: random.Random) -> dict:
        return {'type': 'score', 'instructions': self.instructions, 'criteria': list(self.levels)}


Question = Choice | Noul | Score


@dataclass(frozen=True)
class ChoiceAnswer:
    choice: str
    probabilities: dict[str, float]
    confidence: float

    def ranked(self) -> list[tuple[str, float]]:
        return sorted(self.probabilities.items(), key=lambda kv: -kv[1])

    @property
    def margin(self) -> float:
        """How far the winner leads the runner-up."""
        top = self.ranked()
        return top[0][1] - (top[1][1] if len(top) > 1 else 0.0)


@dataclass(frozen=True)
class NoulAnswer:
    p: float

    def yes(self, threshold: float = 0.5) -> bool:
        return self.p >= threshold


@dataclass(frozen=True)
class ScoreAnswer:
    score: float
    probabilities: dict[str, float]
    confidence: float
    legend: dict[str, str] = field(default_factory=dict)

    @property
    def level(self) -> int:
        return round(self.score)


Answer = ChoiceAnswer | NoulAnswer | ScoreAnswer


def parse_answer(raw: dict) -> Answer:
    kind = raw.get('type')
    if kind == 'choice':
        return ChoiceAnswer(raw['choice'], dict(raw.get('probabilities', {})),
                            float(raw.get('confidence', 0.0)))
    if kind == 'noul':
        return NoulAnswer(float(raw['noul']))
    if kind == 'score':
        return ScoreAnswer(float(raw['score']), dict(raw.get('probabilities', {})),
                           float(raw.get('confidence', 0.0)), dict(raw.get('legend', {})))
    raise JevError(f'unknown answer type: {raw}')


# ---- System One ----------------------------------------------------------

class Jev:
    """Typed decisions. All questions in one `ask` see the same state and are
    answered independently, in one request."""

    def __init__(self, model: str = DECISION_MODEL, ledger: Ledger | None = None,
                 seed: int = 0, session: str | None = None, post: Post | None = None):
        self.model = model
        self.ledger = ledger or Ledger()
        self.rng = random.Random(seed)
        self.session = session
        self._post = post or (lambda url, body, headers: http_post(url, body, headers))

    def ask(self, state: Any, tag: str = '', **questions: Question) -> dict[str, Answer]:
        if not questions:
            raise ValueError('ask needs at least one question')
        body = {'model': self.model, 'state': state,
                'questions': {name: q.request(self.rng) for name, q in questions.items()}}
        if self.session:
            body['session_id'] = self.session
        started = time.monotonic()
        response = self._post(DECISIONS_URL, body, {'Authorization': f'Bearer {api_key()}'})
        seconds = time.monotonic() - started
        try:
            answers = {name: parse_answer(response['answers'][name]) for name in questions}
        except KeyError as error:
            raise JevError(f'answer {error} missing from {response}') from None
        usage = response.get('usage', {})
        self.ledger.record({
            'kind': 'decision', 'tag': tag, 'model': response.get('model', self.model),
            'id': response.get('id'), 'seconds': round(seconds, 3), 'cost': usage.get('cost'),
            'input_tokens': usage.get('input_tokens'), 'state': state,
            'questions': body['questions'],
            'answers': {name: response['answers'][name] for name in questions}})
        return answers


# ---- System Two ----------------------------------------------------------

@dataclass(frozen=True)
class ToolCall:
    name: str
    arguments: dict


@dataclass(frozen=True)
class RouterReply:
    content: str
    tool_calls: list[ToolCall]
    model: str
    cost: float | None


def image_part(path: Path) -> dict:
    data = base64.b64encode(Path(path).read_bytes()).decode('ascii')
    return {'type': 'image_url', 'image_url': {'url': f'data:image/png;base64,{data}'}}


def tool(name: str, description: str, properties: dict, required: Sequence[str] = ()) -> dict:
    """A function tool in the Chat Completions shape."""
    return {'type': 'function', 'function': {
        'name': name, 'description': description,
        'parameters': {'type': 'object', 'properties': properties, 'required': list(required)}}}


class Router:
    """Chat requests through `typesafe/jev-router`: screenshots, menus,
    planning, recovery. Slow (seconds) and costlier; ask it rarely."""

    def __init__(self, model: str = ROUTER_MODEL, cost_tier: str = 'medium',
                 ledger: Ledger | None = None, session: str | None = None, post: Post | None = None):
        self.model = model
        self.cost_tier = cost_tier
        self.ledger = ledger or Ledger()
        self.session = session
        self._post = post or (lambda url, body, headers: http_post(url, body, headers, timeout=300.0))

    def ask(self, prompt: str, images: Sequence[Path] = (), tools: Sequence[dict] = (),
            system: str | None = None, tag: str = '', tool_choice: str | None = None) -> RouterReply:
        content: list[dict] = [{'type': 'text', 'text': prompt}]
        content += [image_part(path) for path in images]
        messages = ([{'role': 'system', 'content': system}] if system else []) + \
                   [{'role': 'user', 'content': content}]
        body: dict[str, Any] = {'model': self.model, 'messages': messages,
                                'plugins': [{'id': 'jev-router', 'cost_tier': self.cost_tier}]}
        if tools:
            body['tools'] = list(tools)
            if tool_choice:
                body['tool_choice'] = tool_choice
        if self.session:
            body['session_id'] = self.session
        started = time.monotonic()
        response = self._post(CHAT_URL, body, {'Authorization': f'Bearer {api_key()}'})
        seconds = time.monotonic() - started
        try:
            message = response['choices'][0]['message']
        except (KeyError, IndexError):
            raise JevError(f'no message in {str(response)[:500]}') from None
        calls = []
        for call in message.get('tool_calls') or []:
            function = call.get('function', {})
            try:
                arguments = json.loads(function.get('arguments') or '{}')
            except json.JSONDecodeError:
                arguments = {'_unparsed': function.get('arguments')}
            calls.append(ToolCall(function.get('name', ''), arguments))
        usage = response.get('usage', {})
        reply = RouterReply(message.get('content') or '', calls, response.get('model', self.model),
                            usage.get('cost'))
        self.ledger.record({
            'kind': 'router', 'tag': tag, 'model': reply.model, 'seconds': round(seconds, 3),
            'cost': reply.cost, 'prompt': prompt, 'images': [str(p) for p in images],
            'content': reply.content, 'tool_calls': [vars(c) for c in calls]})
        return reply
