# Save Game System

## Overview

The save game system in Revenant allows players to save and load their progress. Save games are stored as binary files with the extension `.sav` and follow a specific format that includes player data, automap information, and game state. The system also includes a sophisticated map save mechanism that tracks changes to the game world.

## File Format

### File Structure

Save game files follow this structure:

```
[AutoMap Data] + [Game Data Slots] + [Player Object Data]
```

### File Naming Convention

- Save games are named `gameXX.sav` where XX is a two-digit number (01-99)
- Default save game is `game01.sav`
- Game numbers are zero-padded (e.g., `game01.sav`, `game02.sav`, etc.)

## Data Components

### 1. AutoMap Data

The automap data is written first and contains:
- Explored areas of the map
- Discovered locations
- Map annotations and notes

**Implementation**: `TAutoMap::WriteAutoMapData()` and `TAutoMap::ReadAutoMapData()`

### 2. Game Data Slots (32 integers)

A fixed array of 32 integers for storing game state information:

```cpp
int32_t data[DATA_SLOTS];  // DATA_SLOTS = 32
```

**Current Usage**:
- `data[0]` - Game time (gametime)
- `data[1]` - Active pane (which UI pane was active)
- `data[7]` - Save game version (MAP_VERSION)
- `data[2-6, 8-31]` - Reserved for future use (currently zeroed)

### 3. Player Object Data

The complete player object (Locke) including:
- All object properties (position, stats, flags, etc.)
- Complete inventory with all items
- Equipment and gear
- Script state and variables

**Implementation**: Uses `TObjectInstance::SaveObject()` and `TObjectInstance::LoadObject()`

## Map Save System

### Two-Tier Map Architecture

Revenant uses a sophisticated two-tier map system that separates static base maps from dynamic current maps:

#### Base Map Directory (`BASEMAPDIR = "map"`)
- Contains the original, unmodified sector files
- These are the initial game sectors that never change
- Serves as the "clean slate" for the game world

#### Current Map Directory (`CURMAPDIR = "curmap"`)
- Contains modified sector files that reflect the current game state
- These are the "dirty" sectors that have been altered during gameplay
- Gets updated as the player interacts with the world

### Sector Loading Priority

The sector loading system uses a specific priority order:

```cpp
// First try to load from current map (modified sectors)
strcpy(mappath, CurMapPath);
strcat(mappath, CURMAPDIR "\\");
strcat(mappath, filename);
fp = fopen(mappath, "rb");

// If not found, fall back to base map (original sectors)
if (!fp) {
    strcpy(mappath, BaseMapPath);
    strcat(mappath, BASEMAPDIR "\\");
    strcat(mappath, filename);
    fp = fopen(mappath, "rb");
}
```

### Map Save Process

#### SaveCurMap() Function
```cpp
void TMapPane::SaveCurMap(char *to)
{
    ReloadSectors(); // Forces sectors to be saved
    
    LOCKSECTORS;     // Prevent update thread from accessing sectors
    
    TSector::ClearPreloadSectors();
    
    UNLOCKSECTORS;
    
    if (!to)
        to = BASEMAPDIR;  // Default to base map directory
    
    // Copy all .DAT files from curmap to target directory
    char frompath[MAXPATHLEN], topath[MAXPATHLEN];
    makepath(CURMAPDIR, frompath, MAXPATHLEN);
    strncatz(frompath, "\\*.DAT", MAXPATHLEN);
    makepath(to, topath, MAXPATHLEN);
    strncatz(topath, "\\", MAXPATHLEN);
    
    copyfiles(frompath, topath);
}
```

#### LoadCurMap() Function
```cpp
void TMapPane::LoadCurMap(char *from)
{
    char frompath[MAXPATHLEN], topath[MAXPATHLEN];
    
    ClearCurMap();  // Clear current map state
    
    if (!from)
        return;
    
    // Copy all .DAT files from source to curmap directory
    makepath(from, frompath, MAXPATHLEN);
    strcat(frompath, "\\*.DAT");
    makepath(CURMAPDIR, topath, MAXPATHLEN);
    
    copyfiles(frompath, topath);
}
```

### Differences Between Initial and Saved Sectors

#### Content Changes
- **Initial sectors**: Contain only the original static objects (tiles, buildings, NPCs, etc.)
- **Saved sectors**: Include all changes made during gameplay:
  - Destroyed objects (doors, containers, etc.)
  - Moved objects (items picked up, furniture moved)
  - New objects (items dropped, corpses, etc.)
  - Modified object states (opened doors, activated switches)
  - Changed object properties (damaged items, depleted resources)

#### Object Persistence
- **Initial sectors**: Objects are in their default states
- **Saved sectors**: Objects reflect their current state:
  - Health/damage levels
  - Animation frames and states
  - Inventory contents (for containers)
  - Script variables and flags
  - Light states and intensities

### Save Game Directory Structure

Each save game creates its own directory structure:
```
savegame.001/
├── game01.sav          # Player data, automap, game state
└── *.DAT              # Modified sector files

savegame.002/
├── game02.sav          # Player data, automap, game state  
└── *.DAT              # Modified sector files
```

## Object Serialization

### Object Data Structure

Each object in the save game includes:

```cpp
struct ObjectHeader {
    short objversion;      // Object version ID
    short objclass;        // Class ID (player, monster, item, etc.)
    uint32_t objid;        // Unique object ID
    short blocksize;       // Size of object data block
};
```

### Serialized Object Properties

Objects save the following data:

**Basic Properties**:
- Name (variable length string)
- Flags (object flags)
- Position (x, y, z coordinates)
- Velocity (x, y, z for mobile objects)
- State (current object state)
- Level (for non-map objects like players)
- Inventory number and index
- Shadow, rotation, and map index

**Animation Data**:
- Frame (current animation frame)
- Frame rate (animation speed)
- Group (animation group)

**Statistics**:
- Object-specific stats with unique IDs
- Health, armor, damage, etc.
- Stats are identified by unique 4-character codes (e.g., "AMT", "TYPE", "AC")

**Light Data** (if object is a light source):
- Light flags
- Light position
- Light color (RGB)
- Light intensity and multiplier

**Inventory**:
- Number of inventory items
- Recursive serialization of all inventory objects

## Save Game Class

### TSaveGame Class

```cpp
class TSaveGame {
public:
    TSaveGame() { saved = nullptr; }
    
    bool WriteGame(char *name = nullptr);
    bool WriteGame(int32_t gamenum);
    bool ReadGame(char *name = nullptr);
    bool ReadGame(int32_t gamenum);
    bool IsLoading() const { return loading; }

protected:
    TObjectInstance* saved;     // Saved object (Locke)
    int32_t gametime;          // Game time when saved
    int32_t version;           // Version of savegame
    int32_t pane;              // Which pane was active
    bool loading;              // Loading state flag
};
```

### Save Process

1. **Validation**: Check if player exists and is valid
2. **State Capture**: Store current game time and active pane
3. **File Creation**: Open binary file for writing
4. **AutoMap Write**: Write automap data first
5. **Game Data Write**: Write 32-integer data array
6. **Player Serialization**: Serialize complete player object with inventory
7. **File Close**: Close and validate file

### Load Process

1. **State Reset**: Clear current map and reload game states
2. **Player Clear**: Clear current player list
3. **File Open**: Open save game file for reading
4. **AutoMap Read**: Read and restore automap data
5. **Game Data Read**: Read 32-integer data array
6. **Player Deserialization**: Load and reconstruct player object
7. **State Restoration**: Restore game time, active pane, and player manager
8. **UI Update**: Redraw screen and activate correct pane

## Version Compatibility

### Version History

- **Version 0**: No unique IDs, direct objtype storage
- **Version 1-3**: Unique IDs used instead of objtype
- **Version 4+**: Block size support for object skipping
- **Version 6+**: Level storage for non-map objects
- **Version 8+**: Object version IDs
- **Version 9+**: 16-bit state and level values

### Backward Compatibility

The system includes version checking and backward compatibility:
- Old save games are rejected if version < 3
- Object loading handles missing properties gracefully
- Corrupted objects are skipped rather than causing crashes

## Integration Points

### Play Screen Integration

The save game system integrates with the play screen through:
- `PlayScreen::SaveGame(int32_t game)` - Triggers save operation
- `PlayScreen::SetGameTime(int32_t time)` - Restores game time
- Save/load operations during death and respawn

### Command System Integration

Save games can be triggered via console commands:
- `save game <number>` - Save to specific slot
- `save map` - Save current map state
- `save headers` - Save object headers
- `save classes` - Save object class definitions
- `save exits` - Save exit/portal data

### Death System Integration

When the player dies, the save game system:
- Automatically loads the last save game
- Restores player position and state
- Maintains game continuity

## File I/O Implementation

### Stream Classes

The system uses custom stream classes for serialization:
- `TOutputStream` - For writing save game data
- `TInputStream` - For reading save game data
- `TFileParseStream` - For parsing text-based configuration

### Error Handling

- File I/O errors are caught and reported
- Invalid object data is skipped rather than crashing
- Memory allocation failures are handled gracefully
- Version mismatches trigger appropriate error messages

## Security and Validation

### Data Validation

- Object counts are validated (max 2048 inventory items)
- File sizes are checked for reasonable limits
- Object class and type validation
- Unique ID verification against class definitions

### Corruption Recovery

- Corrupted objects are deleted after loading
- Invalid object classes default to tile objects
- Missing object types use fallback type 0
- Block size allows skipping corrupted data

## Performance Considerations

### Memory Management

- Save games use streaming I/O to minimize memory usage
- Large objects are processed in chunks
- Temporary buffers are allocated and freed appropriately

### Optimization

- Object serialization is optimized for common cases
- Inventory serialization uses recursive but efficient algorithms
- AutoMap data is compressed where possible
- Only modified sectors are saved, not the entire world

## Future Extensibility

### Data Slots

The 32-integer data array provides room for future expansion:
- Additional game state variables
- New system flags and settings
- Extended metadata storage

### Object Versioning

Object version IDs allow for:
- Backward compatibility with old save formats
- New object properties without breaking existing saves
- Gradual migration of save game formats

## Usage Examples

### Saving a Game

```cpp
// Save to slot 1
SaveGame.WriteGame(1);

// Save with custom filename
SaveGame.WriteGame("mygame.sav");
```

### Loading a Game

```cpp
// Load from slot 1
SaveGame.ReadGame(1);

// Load with custom filename
SaveGame.ReadGame("mygame.sav");
```

### Checking Save Status

```cpp
if (SaveGame.IsLoading()) {
    // Handle loading state
}
```

### Map Save Operations

```cpp
// Save current map state
MapPane.SaveCurMap();

// Load map from save game directory
MapPane.LoadCurMap("savegame.001");

// Clear current map (reset to base)
MapPane.ClearCurMap();
```

## Troubleshooting

### Common Issues

1. **Corrupted Save Files**: Delete the corrupted `.sav` file and start a new game
2. **Version Mismatches**: Old save files may not load with newer game versions
3. **Missing Objects**: Objects that no longer exist in the game will be skipped
4. **Map State Issues**: Clear the `curmap` directory to reset to base map state
5. **Sector Loading Failures**: Check file permissions and disk space

### Debug Information

The system provides debug output for:
- Object loading failures
- Invalid inventory sizes
- Corrupted object data
- Version compatibility issues
- Map save/load operations 