# UI Resurrection — Phased Plan

Read [README.md](README.md) and [ARCHITECTURE.md](ARCHITECTURE.md) first. Live status is in [BURNDOWN.md](BURNDOWN.md); this doc explains the **why** behind ordering.

This plan sits inside the master [../PORT_PLAN.md](../PORT_PLAN.md) — its Phase 4 (feature recovery) and Phase 5 (rolling cleanup) for the UI subsystem specifically. The phases below (A–D) are internal to this UI bring-up.

## Sequencing principle

The codebase already has the foundations (overlay/mount, `defdoc`, `TDialogList`, INI, font path, render queue, test harness). So Phase A is small — a few targeted evolutions to existing classes that unblock everything else. Phases B/C are the actual pane work: HUD first (largest unblocking value because it gates gameplay), OOG second (more screens but well-trodden patterns by then). Phase D is the Revisited-toggle features that make 1920×1080 + localization etc. shippable.

Each phase has externally verifiable exit criteria.

---

## Phase A — Foundational evolutions

Small, surgical. Each is an evolution of an existing class or the implementation of a single planned-but-deferred accessor. No new modules.

### A.1 — `Revisited::IsEnabled()` accessor
[revisited/README.md](../../revisited/README.md) documents the model and notes "TBD — add when the first behavioral toggle ships." UI work is the first customer. Single public accessor reading `[Revisited]` section of `<SavePath>/Revenant.ini` via the existing INI API. Default per key = false.

**Done when:** accessor available; default-off behavior verified; a sentinel key flips behavior in a single test.

### A.2 — `TPane` retained-mode evolution (layout + 9-slice + style + scroll/clip + resizable/HiDPI)
Add to existing `TPane`: explicit parent + children, **2-pass measure→layout** (vertical/horizontal containers, margin/padding, fixed/greedy sizing), anchors, dirty propagation, **proper 9-slice rendering**, a single `UIStyle` struct (ImGui / Unity style-asset model — no themable components), **clip rect on the base**, **alpha (soft-edge) clip** variant. Existing [`TScrollPane`](../../src/scroll.h) evolves to viewport+content split using the new clip rect. **UI canvas = live window backing resolution** (resize-responsive, HiDPI-aware), decoupled from game-world framebuffer (game world composites under UI as a textured quad). Vanilla rendering is unchanged: retail DEF coords pass through, layout degenerates to explicit rects, anchors / containers / 9-slice / clip activate only where opted in. (Memory: `feedback-ui-retained-mode`, `project-ui-layout-system`, `project-resolution-modes`.) Worktree branch — touches the pane base class everything else uses.

**Done when:** existing screens still render identically; `--test=ui-anchors` shows correct positions in 640×480, 1920×1080, and HiDPI backing (with live window resize); `--test=ui-layout` shows a synthetic vertical container with a mix of fixed and greedy children sizing correctly; `--test=ui-nineslice` renders a 9-sliced panel at several target sizes; `--test=ui-scroll` shows a scrollable list inside both hard and soft clip rects; pane tree dirty propagation triggers minimal redraw under a synthetic test.

### A.3 — `TPane::LoadFromDef` + `TButtonPane::LoadFromDef`
Add DEF-driven construction. Parses a `defdoc::Node` and instantiates the appropriate pane / button hierarchy. Unknown widget kinds log and skip (don't FATAL).

**Done when:** `--test=ui-loadfromdef` loads `widgets.def` and an OOG `.def` and produces an inspectable pane tree without error.

### A.4 — Anchor metadata extension to retail DEF
Define the anchor block convention inside DEF (extending the format `defdoc` already parses). Document the convention in `revisited/resources/README.md`. Vanilla mode ignores anchor blocks if present.

**Done when:** a sample retail screen's anchor file in `revisited/resources/` is consumed by `LoadFromDef` and positions panes correctly under Revisited 1920×1080.

---

## Phase B — HUD bring-up (`TPlayScreen`)

Un-stub the `TODO(port)` markers in [src/playscreen.cpp](../../src/playscreen.cpp). Each pane is brought online by evolving the existing class — adding retained-mode parenting + Tier-1 modern C++ + connecting it to `TPlayScreen`'s composition.

Order is dependency-respecting + cheap-first:

| # | Pane class evolved | Notes |
|---|---|---|
| B.1 | `TTextBar` | Simplest. First pane brought online end-to-end through the retained-mode tree. Functions as Phase B's vertical slice. |
| B.2 | `THealthBar`, `TStaminaBar` (`TStatusBar` subclasses) | Read-only; trivial coupling to player stats. |
| B.3 | `TCursorHud` | Resolves Win32-clipping `#if 0` in [src/cursor.cpp](../../src/cursor.cpp). Needed before any pointer-driven pane is useful. |
| B.4 | `TQuickSpellPane` | Read-only display first; click logs; bind to existing spell list. |
| B.5 | `TMultiCtrlPane` | Container for B.6–B.9; needs 4-button switcher wired (1/2/3/4 keys). |
| B.6 | `TInventory` | Drag-and-drop; first complex interaction. |
| B.7 | `TEquipPane` | Reuses B.6 drag infrastructure. |
| B.8 | `TAutoMap` | Read-only; consumes existing explored data. |
| B.9 | `TStatPane` | Read-only; consumes player stats. |
| B.10 | `TDialogPane` | Hooks into script/dialog system. |
| B.11 | `GAMECOMMAND` dispatch un-stub in `playscreen.cpp` | Wires keyboard/mouse → game actions. This is what makes the HUD a control surface. |
| B.12 | Panel visibility toggles | B / V / Space keys per [../HUD.md](../HUD.md). |

Each item includes: retained-mode integration, Tier-1 modernization sweep on the file being touched, test mode where applicable.

**Done when:** a full play session can be driven entirely from the HUD without ImGui editor or console. Both vanilla 640×480 and Revisited 1920×1080 visually verified per pane.

---

## Phase C — OOG bring-up

Once the HUD is working, the patterns are well-trodden. OOG is mostly content + flow.

| # | Screen | Notes |
|---|---|---|
| C.1 | `TMainMenuScreen` | Entry point. Drives transitions to other OOG screens. Simplest TScreen subclass. |
| C.2 | `TPopupScreen` / popup mechanism | Modal pattern reused by everything downstream. |
| C.3 | `TExitConfirmScreen` | Trivial; pattern shakedown. |
| C.4 | `TOptionsScreen` | Settings I/O via existing INI API. Surfaces the first user-visible toggles. |
| C.5 | `TLoadGameScreen` | Save format integration; read-only listing first. |
| C.6 | `TSaveGameScreen` | Mirror of C.5. |
| C.7 | `TCreateCharScreen` | More complex; portrait + attributes. |
| C.8 | `TSelStartScreen` | Continues from create-char. |
| C.9 | `TInGameMenuScreen` | Pause flow; bridges HUD and OOG. |
| C.10 | Network setup screens (host / join / connect) | Only if network layer is ready; otherwise stub. |

**Done when:** fresh boot lands on main menu, character create flows into a playable session, in-game menu pauses cleanly.

---

## Phase D — Revisited features

Each item is a toggle under `[Revisited]` in `Revenant.ini`. Default off (= vanilla).

- **D.1 `Visual.Resolution = "1920x1080"`** — anchored layout fully exercised across all HUD + OOG screens. Anchor overlays for every screen ship in `revisited/resources/`.
- **D.2 `Visual.HiDPI`** — variable resolution beyond 1920×1080. Anchor work from A.2/A.4 should mostly absorb this.
- **D.3 `UI.ExpandedHUD`** — optional UI elements per [revisited/README.md](../../revisited/README.md) example. Speculative until specifically requested.
- **D.4 `Loc.AutoTranslate` (or per-locale toggles)** — actual non-English packs. Deferred to here per user direction (defer translations until the game functions). String table API is already in use throughout from Phases A–C; only the packs themselves arrive in D.
  - D.4a German pack (Latin-1, +30% length — stresses anchor layout).
  - D.4b First CJK pack (stresses font fallback / glyph coverage).
- **D.5 Settings UI** — only if hand-edited `Revenant.ini` becomes painful. Default: don't build.

Items added as the user identifies them, not invented speculatively.

---

## Test modes (per `src/testmodes.cpp` convention)

Three categories, all wired into the existing `Initialize`/`Close`/`Render` dispatch:

**Subsystem modes:**
| Phase | Test mode | Verifies |
|---|---|---|
| A.2c | `--test=ui-anchors` | Same pane at correct positions in 640×480 and 1920×1080 |
| A.2b | `--test=ui-layout` | Synthetic vertical container with mixed fixed/greedy children sizes correctly |
| A.2d | `--test=ui-nineslice` | One source 9-slice atom renders at several target sizes |
| A.2f/g | `--test=ui-scroll` | Scrollable list inside a hard clip rect and a soft (alpha) clip rect |
| A.3 | `--test=ui-loadfromdef` | A retail UI DEF loads into an inspectable pane tree |

**Game panel mockup modes** — single `TPane` with synthesized input:
| Phase | Test mode |
|---|---|
| B.1 | `--test=ui-textbar` |
| B.2 | `--test=ui-statusbars` |
| B.4 | `--test=ui-quickspells` |
| B.6 | `--test=ui-inventory` |
| B.8 | `--test=ui-automap` |
| B.9 | `--test=ui-statpane` |
| B.10 | `--test=ui-dialog` |

**Game screen mockup modes** — full `TScreen` with all panels composed, action stubs at the boundary:
| Phase | Test mode | Verifies |
|---|---|---|
| B (after B.1–B.10) | `--test=ui-hud-mockup` | Full HUD composition, both resolutions |
| C.1 | `--test=ui-mainmenu` | Main menu in isolation |
| C.7 | `--test=ui-createchar` | Char create in isolation |
| C.9 | `--test=ui-ingamemenu` | Pause screen in isolation |
| C (after C.1–C.9) | `--test=ui-oog-mockup` | End-to-end OOG flow with stubbed actions |

Skip a test mode only where the system is genuinely inseparable from full game state (B.11 `GAMECOMMAND` dispatch).

---

## Coordination with other agents

The repo is shared. Main tree stays buildable + playable at every commit (memory: `feedback-keep-main-runnable`). Invasive changes — **A.2 `TPane` retained-mode evolution** especially — land in a git worktree branch and merge as one coherent change. Sub-agents spawned for non-trivial work use `isolation: "worktree"`.

## Risks and mitigations

| Risk | Mitigation |
|---|---|
| Retained-mode evolution of TPane breaks existing screens | A.2 in a worktree; existing screens verified to render identically before merge. Vanilla path is a no-op for anchors. |
| `GAMECOMMAND` dispatch turns out to be entangled with game state we haven't wired | B.11 deliberately after panes are visible — gap is bounded and obvious |
| Retail UI DEFs have quirks that break LoadFromDef mid-Phase-C | Phase B exercises `LoadFromDef` early via widgets/textbar; widen as we go rather than build-then-test |
| Editor and game UI input collide on Mac | Memory `project-imgui-mac-modifiers` flags chord caveat; same `kPrimaryMod` pattern for HUD shortcuts |
| Sub-agent work diverges from architecture | Briefs in [briefs/](briefs/) restate constraints; reviewer = coordinating agent |
