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

**Two-panel layout confirmed by `sample_screen_3.jpg`** — that screenshot shows panels in BOTH upper corners with portraits + bars. The right-side portrait looks distinct from Locke's (possibly companion / pet / coop second player). My earlier "single-player only has one" guess based on sample_screen_1/2 was wrong; the upper-right just isn't always populated.

### 2. Upper-right character panel (when shown)

Mirror of §1 (right-aligned). Visible in `sample_screen_3`. Shows another character's portrait + bars in the same shape. Whether this is a coop partner, a pet/companion, or a targeted-NPC info popup is uncertain — needs user confirmation.

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

(screenshot 5)
- Full-screen menu, not a HUD overlay
- Background: large character art of Locke on the left
- Centered title "REVENANT" in stylized gold lettering at top
- Vertical menu (right side): New Game / Load Game / Multiplayer / Options / Exit
- Bottom-right corner: "CINEMATIX" branding
- Bottom-left corner: version label ("Revenant v1.22")

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

## Open questions for the user

1. **Right character panel role** — confirmed in `sample_screen_3` (two panels upper-left + upper-right). Who's the second panel? Coop partner? Pet / companion? Targeted NPC info popup?
2. **Sword-icon circle below character panel** (`sample_screen_4`, below upper-left panel) — is that part of the character panel structure, OR a separate "active weapon / quick equip" pane? If part of the character panel: does it always show (just hidden if no weapon equipped)?
3. **Sidebar modes count** — I count ~7-8 buttons in the vertical mode-switcher strip. I can identify 5 modes confidently (stats, skills?, inventory, spell book, automap). What are the others?
4. **Stats vs Skills tab** — in `sample_screen_1` the stats panel has "Stats | Skills" tab buttons. Are those sub-tabs inside one sidebar mode, or two distinct sidebar modes?
5. **Potion shelf** — fixed slot count (~6-7) or variable? Is the key/extra slot on the far right always there?
6. **Far-right slot** — what goes there (keys, quest items, runes)?
7. **Bar ordering** — health → mana → stamina, or health → stamina → mana? `sample_screen_1` shows the order from screenshots: heart (red), then purple, then yellow — what are the middle and bottom values labeled in retail terms?
8. **Game log scrollback** — can the player scroll up to see older messages, or is it write-only? How many lines visible at once?

Filling these in dramatically narrows the recon hunt and prevents another off-spec build.
