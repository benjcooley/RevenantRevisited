# Retail game image goes here

This folder holds a Revenant install for development: the folder with
`Revenant.exe`, `imagery.rvi`, `resources.rvr` and `Modules/`, from GOG or
the original CD. Copy the install's contents here, or make `data` a symlink
to it. Git ignores everything here but this file: the game is not ours to
redistribute, so it never goes into the repository.

Run from the repository root, the engine finds the game here (`./data`).
To keep it elsewhere, set `REVENANT_DATA_PATH` to the install folder
instead. docs/DATA_LAYOUT.md describes how the engine reads it.
