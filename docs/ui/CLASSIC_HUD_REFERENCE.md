# Classic Revenant HUD — Visual Reference

**Status:** authoritative reference. Drives all per-class reconstruction.

**Reference screenshots** (in this same `docs/ui/` directory):

| File | Scene | Shows |
|---|---|---|
| [sample_screen_1.jpg](sample_screen_1.jpg) | Dungeon with snake boss, Locke Lvl 26 | Full HUD: character panel UL, Stats/Skills + Automap sidebar, quickspell row + potion shelf + game log |
| [sample_screen_2.jpg](sample_screen_2.jpg) | Forest with chest, Locke Lvl 8 | Inventory sidebar mode (paper doll + item grid + currency) |
| [sample_screen_3.jpg](sample_screen_3.jpg) | Low-res dungeon combat | **TWO character panels** (upper-left + upper-right), both with portraits + bars — confirms the two-panel layout |
| [sample_screen_4.jpg](sample_screen_4.jpg) | Keep entrance, Locke Lvl 1 | Sidebar collapsed (only the vertical button strip visible). Extra slot below character panel shows a sword icon (active weapon?). "HINSTEN" overlay top-right is a **fansite watermark, NOT game UI** |
| [spell_list_ui.jpg](spell_list_ui.jpg) | Spell cast scene, scroll reader open | Sidebar showing scroll text ("Tut Hailey") + spell-book-style column of small spells below the scroll |
| [main_menu_ui.jpg](main_menu_ui.jpg) | OOG main menu | Title + character art + vertical menu (New Game / Load Game / Multiplayer / Options / Exit) + Cinematix branding |

This document is the SOURCE OF TRUTH for what Classic Revenant looks like and how each HUD element behaves. Cross-check anything you build against the screenshots; if reconstruction doesn't match, reconstruction is wrong.

If you spot a misread in this doc — fix it. The screenshots are authoritative; this transcription is not.

## Design context — Revenant is a fighting-game-style ARPG

Per the user (original developer): "Revenant is very much that kind of a fighting game." The HUD design follows from that ethos:

- **Two character panels facing off** (Locke vs current target, upper-left vs upper-right) — like Street Fighter / Mortal Kombat health bars facing each other across the screen. The target panel appears the instant the player has a target; it's critical combat real estate, not a peripheral readout.
- **Three bars per character** (health / mana / stamina) — all three need to be readable at a glance during fast combat exchanges.
- **Quickspell + potion shelf bottom-prominent** — combat tools always within reach. Fast-access pattern that fighting games use for special moves and inputs.
- **Game log transparent + multi-color** — combat events stream past without occluding the playfield. Color-coding speeds parsing during action.
- **Sidebar collapsible** — when combat heats up, the player can hide the sidebar to maximize playfield visibility (`sample_screen_4`).

Treat this as the **lens** for every UI reconstruction call: optimize for **fast combat readability + at-a-glance state + bottom-of-screen action shortcuts**, not menu-driven RPG comfort.

### Target-lock orbit combat (key mechanic that drives the HUD)

When the player acquires a target, the game's movement model **shifts into a circular orbit around the target**. Attacks become **linear player→target** — no 2D aiming. The user (original developer) notes Revenant may have been one of the first games to do this (mid/late 90s).

Implications for the HUD that fall out of this:

- **The target panel really IS the opponent in a ring with you** — both characters constantly face each other across the screen, matching the upper-left + upper-right placement (Street Fighter / MK framing).
- **Target is high-commitment** — the player is orbiting one opponent until they explicitly cycle off. The right panel is not a peripheral "current hover info"; it's the opponent the player is engaged with. Its prominence matches that weight.
- **No aim cursor needed in the HUD** — attacks are linear from player to target by virtue of the orbit lock. The HUD doesn't need to surface aim direction / cone-of-attack — the lock itself does that.
- **Smooth target swap matters** — when the player steers to switch targets, the right panel must update without a hard cut so the combat flow isn't broken. Animate the transition.

### Heritage

The user (Revenant's original developer) was clear that creating Revenant was hard and there were "no games to crib from" — most of the design was original. The two explicit references they did borrow from are **Street Fighter and Mortal Kombat** (face-off framing + fast-action input). Expect non-conventional design choices throughout — when something in the UI doesn't match a familiar genre convention, the right call is usually to honor what's in retail rather than "fix" it to match contemporary patterns.

---

## Element catalog (per visual evidence)

### 1. Character panel — upper-left ONLY (single-player)

**Position:** upper-left corner of playfield. Top edge ≈ screen top, left edge ≈ screen left, small inset margins. Fixed size.

**Contents (top→bottom):**
- **Portrait** — small head-and-shoulders character image, framed in gold. Locke's face shown with helmet/headgear.
- **Name label** — "Locke" (centered below portrait).
- **Level label** — "Level NN" (e.g. "Level 26", "Level 8", "Level 1").
- **Three horizontal stat bars** — each bar is a thin horizontal gauge with a colored icon on the LEFT and the **current numeric value** on the right:
  - **Health** (red heart icon, red-magenta bar) — values like 1833, 185, 93, 97.
  - **Mana** (purple gem icon, purple bar) — values like 2174, 28, 105, 105.
  - **Stamina** (yellow / orange icon, yellow-orange bar) — values like 131, 103, 78.
  Bar lengths show proportional fill of current vs max.

**Behavior:** always visible during gameplay. Not collapsible. Updates live as player stats change.

**Two-panel layout** (`sample_screen_3.jpg`): panels in BOTH upper corners with portraits + bars. The **right panel = current targeted enemy** (per user). Like a fighting game — two opponents' stats facing off. Same panel class as §1, just bound to a different stat source.

### 2. Upper-right character panel = current target (when targeting)

Same class / same visual shape as §1 (mirrored right-aligned). Shown **conditionally — only when the player has a current target**. Displays the targeted enemy's portrait + name + level + three bars (health / mana / stamina, with the same icon set).

This retrofits the pre-release `TTextBar::SetHealthDisplay(name, level)` API in [src/textbar.cpp](../../src/textbar.h) — that "show opponent name + health line in the textbar" path was the **primitive predecessor** of what became the dedicated right-side target panel by retail.

**Targeting is player-controlled** — the player explicitly steers toward / away from enemies to cycle the target. The right panel observes the target pointer dynamically: when the player switches targets, the panel updates to display the new opponent (presumably with a smooth transition, not a hard cut, but verify against retail). Two-opponents-facing-off framing reinforced — the target is the opponent the player is currently engaging.

**Implementation: single instance, two-pass draw with stat-source swap (CORRECTED 2026-05-16 by Wave-2B).** The original framing assumed "one class, two instances (left=player, right=target)" but `FindBytes` on TPlyrStatusBar's vtable address (0x5a54e4) returned exactly ONE wire site in the binary — the player instance. There is no second global instance.

The likely real architecture (to be verified in Wave-3 by extracting TPlyrStatusBar's draw methods):
- **Single TPlyrStatusBar instance**
- Draw method called **twice per frame**, once with the player as stat source and once with the current target — same widget, two paint operations, two source-stat pointers
- OR: one paint operation that internally walks both stat sources

The face-off framing IS real (the user's "two opponents facing off" description holds — Locke vs current target). It's just implemented via re-draw, not re-instance. **Don't model the port as two-instance** — verify against the actual draw method then mirror retail.

**Not to be confused with**:
- The **"HINSTEN" overlay** in `sample_screen_4` upper-right — that's a fansite-watermark on the screenshot, not in-game UI.
- The **sword-icon circle** below the character panel in `sample_screen_4` — that's an extra slot below the LEFT character panel, looks like an active-weapon / quick-equip display. Whether this is part of the character panel itself or a separate small pane is uncertain.

### 3. Right sidebar — multifunction panel ("right tab area")

**Position:** runs from near top of screen to near bottom, on the right edge. Wide enough to hold a paper-doll character figure + adjacent gem slot columns (~30% of screen width).

**Modes observed:**

#### 3a. Stats / Skills tab
(screenshot 1, dungeon)
- Scroll-styled gold-bordered panel
- Tab header: "Stats" | "Skills" buttons (only "Stats" shown active in screenshot 1)
- Body text (one line per row):
  - "Name: Locke"
  - "Class: Revenant"
  - "Lvl: 38 Exp: 39169 Nxt: 40350"
  - "Hit:1833/1833 Mana:2174/2255"
  - (gap)
  - "Sun 30", "Cons 30", "+Agil 16", "Rflx 14", "Mind 23", "Luck 14"
  - (gap)
  - "Armor: 40 Dmg: 200", "Atk: 278", "Def: 250"
- Decorative cherub/angel ornament near bottom

#### 3b. Inventory mode (paper doll + grid)
(screenshot 2, forest)
- Top: equipment slots arranged around character body
  - Helmet slot (top center, gold helm shown)
  - Side slots: weapons (left + right), shields, gloves, boots
  - Gem slots: colored gems (red / green / blue / etc.) in side columns
  - Center: full character figure shown wearing equipped items
- Below: small coin icon + currency display ("144754g")
- Bottom: a grid of inventory item slots (~4×4), each slot showing item icon + optional numeric count badge

#### 3c. Book / scroll reader
(screenshot 6, scroll showing "Tut Hailey")
- The sidebar area is replaced by a scroll-styled text panel
- Title at top (e.g. "Tut Hailey")
- Body: paragraph text of the found scroll's content

#### 3d. Spell list / spell book
(referenced by the user, partly visible in screenshot 6 below the scroll — column of spell circles)

#### 3e. Automap (combined with stats in some configurations)
(screenshot 1 bottom of sidebar)
- Circular gold-framed map view at the BOTTOM of the right sidebar
- "+" and "-" zoom controls in upper corners of the map circle
- Map shows dungeon corridors / current floor layout
- **Important:** automap circle and stats panel are BOTH visible simultaneously in screenshot 1 (stats on top, automap on bottom). In screenshot 2 (inventory mode), the inventory takes the full sidebar — no automap.

**Sidebar collapsed state:**
(screenshot 4, Keep)
- Sidebar content hidden
- Only the vertical button strip (§4) visible

### 4. Vertical button strip — sidebar mode switcher

**Position:** between the playfield and the right sidebar (so to the LEFT of the sidebar content), running vertically.

**Contents:** column of ~7-8 small circular gold-framed buttons. Each button is an icon for a sidebar mode:
- Stats / character
- Inventory
- Spell book
- Automap
- (a few more I can't clearly identify from the resolution)

**Behavior:** clicking a button switches the right sidebar to that mode. Always visible (even when the sidebar content is hidden — screenshot 4 shows this).

### 5. Bottom — quickspell row + potion shelf + game log

**Position:** spans the bottom edge of the playfield.

**Contents (left to right):**

#### 5a. Quickspell row (4 spell slots)
- 4 circular spell buttons in a horizontal row
- Each button shows the spell's icon (orb of varying color depending on element)
- Text label **above** each: primary spell name (e.g. "Advanced", "Iron Skin", "Fire", "Ice")
- Text label **below** each: spell sub-type (e.g. "healing", "Flash", "Bolt"). For one-word spells like "IronSkin", no sub-label.
- Examples observed:
  - Screenshot 1: "Advanced healing | IronSkin | Fire Flash | Ice Bolt"
  - Screenshot 2: "Poison Strike | Swift | Fire Flash | Meteor Storm"

#### 5b. Potion shelf
- Row of ~6-7 small slots adjacent to the quickspell row (to its right)
- Each slot shows a colored potion bottle icon (red, green, purple, yellow) with a **numeric count badge** ("70", "93", "76", "49", etc.)
- Far right: one or two additional slots that may hold **keys** or other special items (small key icon visible)

#### 5c. Game log (transparent text overlay)
- Positioned ABOVE the quickspell row, hovering over the playfield
- Multi-line text feed (3+ visible lines), each line in its own color:
  - Red / orange — system / map events ("Greater Mana Picked up.", "Locke entered The Keep")
  - Yellow / white — gameplay events ("Loading Map... Please Wait", "Chest opened.")
  - Blue / cyan — other categories
- Transparent background (playfield shows through)
- Scrolls older messages off as new ones arrive

### 6. Overlays (conditional, atop the playfield)

#### 6a. Dialog
- Appears when conversing with NPCs
- Top game-area overlay and/or bottom game-area overlay
- (No screenshot in this batch shows a dialog active)

#### 6b. Books / scrolls
- Render IN the right sidebar area (§3c), not as a main-game-area overlay
- Found-item content displayed as scroll text

#### 6c. Death screen
- Replaces play screen when player dies
- (No screenshot in this batch shows death)

### 7. Out-of-Game (OOG) — main menu

(screenshot 5 = `main_menu_ui.jpg`)
- Full-screen menu, not a HUD overlay
- Background: large character art of Locke on the left
- Centered title "REVENANT" in stylized gold lettering at top
- Vertical menu (right side): New Game / Load Game / Multiplayer / Options / Exit
- Bottom-right corner: "CINEMATIX" branding
- Bottom-left corner: version label ("Revenant v1.22")

### 8. DEF-driven secondary screens

**Per the user: "The game had its own text-based layout system/UI for the settings, multiplayer, save/load we used."**

A whole category of screens are rendered by a **widget engine** that walks per-screen `.def` files — not by hand-written `TScreen` subclasses. The 22 `.def` files in `data/resources_unzipped/` are this system's content. The engine reads `widgets.def` for the primitive widget definitions and per-screen files (`options.def`, `loadgame.def`, etc.) for the layout + behavior.

Screens in this category:
- **Settings / options** (`options.def`)
- **Multiplayer start** (`joingame.def`, `hostgame.def`, `connect.def`, `connectsimple.def`, `mpingamemenu.def`)
- **Save game** (`savegame.def`)
- **Load game** (`loadgame.def`)
- **Character creation** (`createchar.def`)
- **Character select / start** (`selstart.def`)
- **In-game pause menu** (`ingamemenu.def`)
- **Exit confirmation** (`exit.def`)
- **Popups / modals** (`popup.def`)
- **User info entry** (`userinfo.def`)
- **(possibly) credits** — TBD whether DEF-driven or special-cased

Implication for reconstruction: we **don't hunt per-screen classes** for these. We hunt the **DEF widget engine** (parser + renderer + input dispatcher). That + the existing `.def` files = the whole secondary-screen layer for free.

The engine is roughly: parse `.def` → instantiate widget tree → render via the same primitives the HUD uses → route input to the active widget. Find the parser entry point in the retail decomp by searching for unique strings from `widgets.def` (e.g. widget type names, common attribute names), then trace inward.

This is a Tier of its own in the recovery plan — separate from the hard-coded HUD pane work (Wave-1A territory).

### 9. Special-cased non-HUD screens

A few screens are NOT in either category — they're special-cased TScreen subclasses with their own draw / input:

- **Logo / splash** — `TLogoScreen` (`cls_0x5a5d18`, already partially identified in `recon/discovered/`).
- **Death screen** — `TDeathPane` (`src/death.h`, implementation present). May be a pane on top of the play screen rather than a full screen replacement.
- **Books / scrolls** — `TBookPane` / `TScrollPane` (`src/scroll.h`, implementations present). Per CLASSIC_HUD §3c earlier, the scroll/book reader content renders **in the right sidebar area**, not as a main-area overlay. Whether that's the actual TBookPane/TScrollPane rendered into the sidebar slot, or a different code path that pulls scroll text from a `TScroll` game object, is open.
- **Main menu** (screenshot 5) — unclear if hard-coded or DEF-driven. Has fancy character-art background + version label which suggests at least the chrome is special-cased.

---

## What this corrects in prior documents

| Prior assumption (wrong) | Reality (per screenshots) |
|---|---|
| Two vertical fluid-tube bars on left and right screen edges | **One** character panel upper-left, with **three horizontal bars** (health/mana/stamina) + portrait + name + level |
| Inventory grid at the bottom (382×85) | **Inventory is a right-sidebar mode**, with paper-doll equipment slots + item grid. Bottom area is quickspell + potion shelf, not inventory. |
| Multifunction sidebar holds one tab at a time | Sidebar can hold combinations (stats + automap together; inventory full-height alone). Each mode has its own footprint. |
| Text bar = small 198×14 fixed-position yellow strip | **Game log = transparent multi-line multi-color overlay** above the quickspell row, with scrollback |
| MultiCtrl is the 4-button switcher in a separate small pane | The mode switcher is a **vertical strip of ~7-8 buttons** between playfield and sidebar — more modes than 4 (stats, inventory, spell book, automap, …) |
| Books / scrolls = main-area overlay | Books / scrolls render **in the right sidebar** |
| Single screen aspect | OOG main menu is a wholly separate full-screen layout, not part of HUD |

## Implications for the recovery plan

This visual reference forces revisions to [RETAIL_UI_RECOVERY_PLAN.md](RETAIL_UI_RECOVERY_PLAN.md):

- **Tier 1 (character panel)** — was "two combined panels". Should be **one combined panel** upper-left only (single-player). Right side §2 is a separate uncertain element (active weapon? coop second player?) — separate Tier or sub-tier.
- **Tier 3 (right sidebar)** — much bigger scope than just TMultiCtrlPane + sibling multipanes. The sidebar is a multi-mode container with **at least 5 modes** (stats, skills, inventory, spell book, automap, book reader). Each mode is potentially a distinct retail class. Plus the vertical button strip is a separate pane.
- **Tier 4 (quickspell + shelf)** — adds the **potion shelf with numeric counts** explicitly, plus possible key slot on far right.
- **Tier 5 (overlays)** — books/scrolls move OUT of "main-area overlay" into "sidebar content" (3c). Death stays as overlay.
- **NEW Tier — Game log** — separate pane (was lumped under TTextBar). Multi-line, multi-color, transparent, scrollback. Retail TTextBar decomp at 4× pre-release size makes sense now.
- **NEW Tier — OOG main menu** — already in scope (Phase C in the original plan); this is the visual reference.

## Categorization summary

| Category | Examples | Recovery approach |
|---|---|---|
| Hard-coded HUD panes | TPlyrStatusBar, TSidePane, TSideTabsPane, TBottomPane, TTextBar, TQuickSpellPane, TInventory, TAutoMap, TStatPane, TEquipPane, TSpellPane, TMapPane, TDialogPane | Per-class forensic identification in retail decomp; port via existing src/ impl where available. **Wave-1A territory.** |
| DEF-driven secondary screens | settings, save/load, multiplayer, character create, select start, in-game menu, exit confirm, popups, user info | Identify the widget engine (parser + renderer + dispatcher) in retail decomp. Existing `.def` files in `data/resources_unzipped/` are the content. **New tier.** |
| Special-cased non-HUD screens | TLogoScreen, TDeathPane, TBookPane, TScrollPane, main menu | Per-class identification. Some src impl exists. Cross-check retail layout. |

## Open questions for the user

1. ~~Right character panel role~~ — **ANSWERED:** current targeted enemy (like a fighting game). Same class, two instances; right one shown conditionally on target presence.
2. **Sword-icon circle below character panel** (`sample_screen_4`, below upper-left panel) — is that part of the character panel structure, OR a separate "active weapon / quick equip" pane? If part of the character panel: does it always show (just hidden if no weapon equipped)?
3. **Sidebar modes count** — I count ~7-8 buttons in the vertical mode-switcher strip. I can identify 5 modes confidently (stats, skills?, inventory, spell book, automap). What are the others?
4. **Stats vs Skills tab** — in `sample_screen_1` the stats panel has "Stats | Skills" tab buttons. Are those sub-tabs inside one sidebar mode, or two distinct sidebar modes?
5. **Potion shelf** — fixed slot count (~6-7) or variable? Is the key/extra slot on the far right always there?
6. **Far-right slot** — what goes there (keys, quest items, runes)?
7. **Bar ordering** — health → mana → stamina, or health → stamina → mana? `sample_screen_1` shows the order from screenshots: heart (red), then purple, then yellow — what are the middle and bottom values labeled in retail terms?
8. **Game log scrollback** — can the player scroll up to see older messages, or is it write-only? How many lines visible at once?
9. **Credits screen** — DEF-driven (in the widget engine) or special-cased (scrolling text screen of its own)?
10. **Main menu** (screenshot 5 / `main_menu_ui.jpg`) — DEF-driven or hard-coded? The character-art background and version label feel hand-authored, but the menu items themselves could be widget-engine.

Filling these in dramatically narrows the recon hunt and prevents another off-spec build.
