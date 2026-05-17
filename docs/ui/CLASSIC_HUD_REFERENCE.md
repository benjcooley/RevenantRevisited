# Classic Revenant HUD — Visual Reference

**Status:** authoritative reference. Drives all per-class reconstruction.
**Reference screenshots:** [`../../ui_examples/`](../../ui_examples/) (the user is placing the originals here; this doc transcribes what I read from them).

This is the SOURCE OF TRUTH for what Classic Revenant looks like and how each HUD element behaves on screen. Cross-check anything you build against this; if your reconstruction doesn't match, the reconstruction is wrong.

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

**Open question:** the user's earlier "two character panels upper-left + upper-right" description suggests there IS an upper-right one too in some mode (coop multiplayer?). All six screenshots are single-player and only show the upper-left panel. The upper-right area in screenshots is either empty, or contains a different element (see §2).

### 2. Upper-right — active-weapon / quick indicator (provisional reading)

**Position:** upper-right corner, mirrored against the character panel's upper-left position.

**Contents** (only visible in screenshot 4 — "Keep entrance"):
- A small circular gold-framed slot showing a sword icon (active weapon?).
- Possibly more slots stacked below; hard to tell at the resolution.

**Behavior:** appears in screenshot 4; absent or hidden in screenshots 1, 2, 6. This may be a **toggleable HUD element**, OR conditional on the game state (e.g. only shown right after a weapon equip / map enter), OR it's the multiplayer second-character panel and screenshot 4 is from coop.

**Uncertain.** Needs user clarification or recon-side confirmation.

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

1. **Upper-right element** — what is it actually? Active weapon? Companion / pet info? Coop second player? Quick-equip menu?
2. **Single-player only** — confirm that the second character panel only appears in coop multiplayer.
3. **Sidebar modes count** — how many distinct modes does the vertical button strip cycle through? I count ~7-8 buttons in the strip; I can identify 5 modes confidently (stats, skills/talents?, inventory, spell book, automap). What are the others?
4. **Potion shelf size** — exactly how many potion slots? Are the slots fixed (always 6-7) or variable?
5. **Key / extra-slot on far right of potion shelf** — what goes there (keys, quest items, runes)?
6. **Bars** — are health/mana/stamina the canonical order, or does the user character have a different ordering (e.g. health first then stamina then mana)?
7. **Tab buttons on Stats panel** ("Stats" / "Skills" in screenshot 1) — is "Skills" a sub-tab inside the Stats sidebar mode, or a top-level sidebar mode in its own right? (Affects whether it's a sub-state or a sidebar mode count.)

Filling these in dramatically narrows the recon hunt and prevents another off-spec build.
