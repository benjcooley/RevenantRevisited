// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   moviepane.h - TMoviePane, a Smacker movie playing full screen       *
// *************************************************************************
//
// Retail plays its movies through one blocking player (Movies.cpp,
// 0x004bc470): the intro before the title, and the `playmovie` script
// command mid-game. The port can't block its frame loop, so the player is a
// pane: TCinematicScreen hosts it before the title, and the PlayScreen pushes
// it as a modal for `playmovie`. The movie is decoded by the clean-room
// revsmk decoder (thirdparty/revsmk), paced by the wall clock at the file's
// frame rate, with its sound track; Esc or a click ends it.
#pragma once

#include "renderer.h"   // TTextureHandle
#include "screen.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace revsmk { class Decoder; }
namespace audio { struct Source; }

class TMoviePane : public TPane
{
  public:
    TMoviePane();                   // out of line: the decoder type is
    ~TMoviePane() override;         // complete only in moviepane.cpp

    // Opens `path` (resolved like any game file). False if it can't be
    // played; the movie then counts as finished at once.
    bool Open(const char* path);
    // Stops and frees the movie.
    void Close() override;

    // Called once when the movie ends or is skipped.
    void SetOnFinished(std::function<void()> fn) { onFinished = std::move(fn); }
    [[nodiscard]] bool Finished() const { return finished; }

    // Decode to the frame the wall clock has reached and upload it (no
    // render pass is open during Compose).
    void Compose() override;
    // The frame fitted to the window, over black.
    void Draw() override;

    void KeyPress(int32_t key, bool down) override;
    void MouseClick(int32_t button, int32_t x, int32_t y) override;

  private:
    void Finish();

    std::unique_ptr<revsmk::Decoder> decoder;
    TTextureHandle       texture    = kInvalidTexture;
    std::vector<uint8_t> rgba;
    int32_t              dispw      = 0;
    int32_t              disph      = 0;
    double               fps        = 15.0;
    double               starttime  = -1.0;
    int32_t              shownframe = -1;
    audio::Source*       sound      = nullptr;
    std::vector<uint8_t> soundpcm;
    bool                 finished   = false;
    std::function<void()> onFinished;
};
