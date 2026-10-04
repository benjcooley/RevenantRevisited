# UI reference screenshots

Retail-game reference imagery for the HUD + OOG reconstruction effort.
Used by reconstruction agents as ground-truth visual comparison targets
when implementing / aligning pane code in `src/ui*test.{cpp,h}`.

Folder layout maps 1:1 to panel categories in `docs/ui/forensics/`.

| folder | what's in it | spec ref |
|---|---|---|
| `loading-screen/` | "LOADING GAME" boot/level-load progress screen | `LoadingScreen_SPEC.md` |
| `main-menu/` | Title / `TLogoScreen` (New Game / Load / Multi / Options / Exit) | `MainMenu_SPEC.md` |
| `sidebar-equipment/` | Right-sidebar Equipment paperdoll (`EquipPane_SPEC.md`) | `EquipPane_SPEC.md` |
| `sidebar-map/` | Right-sidebar Automap (`MapPane_SPEC.md`) — dungeon + overworld variants | `MapPane_SPEC.md` |
| `sidebar-spell-create/` | Right-sidebar Spell-construction (talisman composer, `TSpellPane`) | `SpellCreatePane_SPEC.md` |
| `sidebar-tabs/` | The 6-button vertical sidebar tab strip (`TSideTabsPane`) | `TSideTabsPane_SPEC.md` |
| `scroll-pane/` | Item-description + spell-description scroll readers (`TScrollPane`) | `ScrollPane_SPEC.md` |
