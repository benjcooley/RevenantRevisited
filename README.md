# RevenantRevisited

A modern port of the classic 1998 PC game **Revenant** by EIDOS Interactive, originally developed by Benjamin Cooley.

## About the Game

Revenant is an action RPG that was released in 1998 for Windows, built with DirectX 5. The game features:

- Real-time 3D graphics with Direct3D
- Action-based combat system
- RPG elements including character progression, inventory, and spells
- Multiplayer support via DirectPlay
- Level editor and modding capabilities

## About the Project

This repository contains the complete source code for Revenant, originally developed in 1998-1999. The goal of this project is to modernize the codebase and make it run on contemporary platforms while preserving the original gameplay experience.

### Current Status

- **Original Platform**: Windows with DirectX 5
- **Target Platforms**: Modern Windows, macOS, Linux
- **Graphics**: Transitioning from Direct3D to modern graphics APIs (Metal, Vulkan, DirectX 12)
- **Build System**: CMake for cross-platform compilation

## Development

This is the original source code from the game's development. The codebase is being modernized to:

1. Remove Windows/DirectX dependencies
2. Implement cross-platform graphics and input systems
3. Update the build system for modern development environments
4. Preserve the original game mechanics and content

## Building

See [ARCHITECTURE.md](ARCHITECTURE.md) for detailed information about the codebase structure and porting strategy.

## License

[Add your license information here]

## Author

**Benjamin Cooley** - Original developer of Revenant (1998-1999)

---

*Revenant was originally published by EIDOS Interactive in 1998. EIDOS is now owned by Square Enix.*
