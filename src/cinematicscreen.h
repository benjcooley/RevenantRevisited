// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   cinematicscreen.h - full-motion-video (.SMK) playback screen         *
// *************************************************************************
//
// Plays a Smacker (SMK2) cinematic full-screen, decoded by the clean-room
// revsmk decoder (thirdparty/revsmk). Used for the intro / inter-mission FMVs
// and the credits roll, and driven directly by `--test=ui-cinematic` for
// verification.
//
// The screen owns the decoder, a streaming GPU texture, and (optionally) the
// decoded audio. Video advances on the wall clock at the file's frame rate, so
// playback speed is independent of render frame rate. Skippable with Esc / a
// click; ends itself when the stream finishes.
#pragma once

#include "renderer.h" // THudDrawable, TTextureHandle
#include "revenant.h" // MAXPATHLEN
#include "screen.h"

#include <memory>
#include <vector>

namespace revsmk { class Decoder; }
namespace audio { struct Source; }

class TCinematicScreen : public TScreen, public THudDrawable
{
  public:
    TCinematicScreen() = default;

    // Set the .SMK to play before entering the screen (production callers).
    // `--test=ui-cinematic` instead reads StartupCinematicPath / a default.
    void SetVideo(const char* path);

    bool Initialize() override;
    void Close() override;
    void Animate(bool draw) override;
    void KeyPress(int32_t key, bool down) override;
    void MouseClick(int32_t button, int32_t x, int32_t y) override;

    // THudDrawable: composite the current frame (inside the swapchain pass).
    void Draw() override;

  private:
    void Advance(); // wall-clock-paced decode + texture upload (no pass open)

    std::unique_ptr<revsmk::Decoder> decoder_;
    TTextureHandle texture_ = kInvalidTexture;
    std::vector<uint8_t> rgba_;
    int32_t dispW_ = 0;
    int32_t dispH_ = 0;
    double fps_ = 15.0;
    double startTime_ = -1.0;
    int32_t shownFrame_ = -1;
    bool registeredHud_ = false;

    audio::Source* audio_ = nullptr;
    std::vector<uint8_t> audioPcm_;

    char path_[MAXPATHLEN] = {};
};

extern TCinematicScreen CinematicScreen;
