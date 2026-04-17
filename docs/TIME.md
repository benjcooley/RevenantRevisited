# Time System Documentation

## Overview

Revenant uses a **fixed-rate timer system** with significant limitations that cause timing issues on modern hardware. The game was designed for 1998-era systems and struggles with variable frame rates and high-performance hardware.

## Core Timing System

### Fixed Frame Rate Design

**Target Frame Rate:**
```cpp
#define FRAMERATE 24
#define RESOLUTION 5
```

**Timer Period:**
```cpp
uint32_t period = (uint32_t)(1000 / FRAMERATE);  // ~41.67ms per frame
```

### Windows Multimedia Timer

**Timer Implementation:**
```cpp
void WINAPI TTimer::TimerCallback(uint32_t wTimerID, uint32_t msg, uint32_t dwUser, uint32_t dw1, uint32_t dw2)
{
    clockticks++;
    TickOccured = true;
    PulseEvent(tickevent);
}
```

**Timer Setup:**
```cpp
timerID = timeSetEvent(period, RESOLUTION, TimerCallback, nullptr, TIME_PERIODIC);
```

## Game Loop Architecture

### Main Game Loop

**WinMain Loop:**
```cpp
// Do game screens
while (NextScreen)
    NextScreen = TScreen::ShowScreen(NextScreen, 0);
```

**Screen Timer Loop:**
```cpp
bool TScreen::TimerLoop(int32_t ticks)
{
    while (true)
    {
        // If clock has ticked, call timertick
        if (!DisableTimer)
            Timer.WaitForTick();
        
        // Process frame...
        if (CurrentScreen)
        {
            static uint32_t lastcount;
            LastFrameTicks = GetTickCount() - lastcount;
            lastcount = GetTickCount();
            
            // Frame rate calculation and frame skipping logic
        }
    }
}
```

## Timing Problems

### No Delta Time System

**Critical Issue:** Revenant has **no delta time system**. All animations and game logic assume a fixed 24 FPS frame rate.

**Animation Timing:**
```cpp
// Animations advance by fixed frame increments
inst->SetFrame(inst->GetFrame() + GetFrameRate());
```

**Texture Animation:**
```cpp
// Texture scrolling based on frame count, not time
float f = (float)PlayScreen.FrameCount();
float newdu = du * f;
float newdv = dv * f;
```

### Frame Rate Calculation Issues

**Problematic Frame Rate Logic:**
```cpp
if (totalframes >= 5)
{
    showrate = (float)1000.0 / (float)(totalticks / totalframes);
    int32_t realframes = totalframes - skipframes;
    if (realframes <= 0)
        realframes = 1; 
    framerate = (float)1000.0 / (float)(totalticks / realframes);
}
```

**Issues:**
- **Inaccurate timing** on modern hardware
- **Frame skipping** causes animation stuttering
- **No compensation** for variable frame rates
- **GetTickCount() precision** limited to ~15ms

### Animation Timing Problems

**Fixed Frame Rate Assumptions:**
- **Character animations** advance by fixed frame increments
- **Particle effects** use frame-based timing
- **Sound synchronization** tied to frame count
- **Combat timing** assumes consistent frame rate

**Example Animation Issue:**
```cpp
// Fire effect timing - frame-based, not time-based
#define FLAME_FRAME_COUNT   17
#define FLAME_CREATE        4
#define FLAME_MIN_LIFE      10
#define FLAME_MAX_LIFE      45
```

## Modern Hardware Issues

### High Performance Problems

**On Fast Systems:**
- **Animation runs too fast** - 24 FPS target becomes 60+ FPS
- **Game logic executes too quickly** - combat, movement, AI
- **Sound synchronization lost** - audio timing drifts
- **Particle effects accelerate** - visual effects become unrealistic

**On Slow Systems:**
- **Frame skipping** causes jerky animation
- **Game becomes unplayable** when dropping below 24 FPS
- **Audio stuttering** due to timing mismatches

### Timer Precision Issues

**Windows Multimedia Timer Limitations:**
- **~15ms minimum resolution** on most systems
- **Inconsistent timing** across different hardware
- **No high-resolution timer** support
- **Limited to 24 FPS** maximum reliable rate

## Animation System Dependencies

### Frame-Based Animation

**Character Animation:**
```cpp
void T3DAnimator::Animate(bool draw)
{
    // Frame advances by fixed increment
    if (flags & ANI3D_UPDATEBOUNDRECT)
    {
        if (inst->CommandDone())
            AnimateResetBoundRect();    
        else
            inst->SetFrame(inst->GetFrame() + GetFrameRate());
    }
}
```

**Texture Animation:**
```cpp
void TAnimTexController::Render()
{
    // Frame-based texture animation
    int32_t f = PlayScreen.FrameCount() % (ugrid * vgrid);
    int32_t u = f % ugrid;
    int32_t v = f - (u * ugrid);
    float newdu = usize * (float)u;
    float newdv = vsize * (float)v;
}
```

### Effect Timing

**Particle Effects:**
```cpp
// All effect timing is frame-based
#define CFD_SMOKETIME2      55      // frames, not milliseconds
#define CFD_GTIME1          10      // frames
#define CFD_GTIME2          20      // frames
```

## Modern Solutions Needed

**High-Resolution Timer:**
- Use QueryPerformanceCounter for microsecond precision
- Maintain the original 24 FPS target
- Ensure consistent timing across modern hardware

**Frame Rate Limiting:**
- Implement explicit frame rate limiting to maintain 24 FPS
- Use sleep-based timing to enforce the original game speed
- Preserve the original timing behavior

**Timer Synchronization:**
- Fix Windows multimedia timer issues on modern hardware
- Ensure `WaitForTick()` properly blocks until timer fires
- Maintain the original timer-driven game loop

**Note:** While it would be technically possible to run the game at higher framerates (60+ FPS) with frame interpolation for smoother animation, this is very low priority for a preservation port. The focus is on maintaining the original 24 FPS timing and authentic gameplay experience.

## Impact on Modern Port

**Critical Issues to Address:**
1. **Animation Speed** - All animations run too fast on modern hardware
2. **Combat Timing** - Attack speeds and cooldowns become unrealistic
3. **Particle Effects** - Visual effects accelerate beyond intended speed
4. **Audio Sync** - Sound effects lose synchronization with visuals
5. **Game Balance** - Difficulty changes due to faster execution

**Required Preservation:**
- **Maintain original 24 FPS timing** for authentic gameplay
- **Fix timer synchronization** issues on modern hardware
- **Preserve frame-based animation** systems as designed
- **Ensure consistent timing** across different hardware
- **Keep original game speed** and pacing intact

The time system represents one of the **most critical preservation challenges** for a Revenant port, requiring fixes to maintain the original timing behavior on modern hardware. 