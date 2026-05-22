// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   cinematicscreen.cpp - full-motion-video (.SMK) playback screen       *
// *************************************************************************
//
// See cinematicscreen.h.

#include "cinematicscreen.h"

#include "audio_backend.h"
#include "display.h"
#include "logging.h"
#include "testconfig.h"
#include "time.h"
#include "wavedata.h" // WAVEFORMATEX

#include "revsmk.h"

#include <cstring>

TCinematicScreen CinematicScreen;

// Drawn at the very bottom of the HUD stack so anything else (cursor, debug
// overlays) lands on top of the video.
static constexpr float kCinematicHudZ = -100.0f;

void TCinematicScreen::SetVideo(const char* path)
{
    if (path)
        strncpyz(path_, path, sizeof(path_));
}

bool TCinematicScreen::Initialize()
{
    const char* path = path_[0]               ? path_
                     : StartupCinematicPath[0] ? StartupCinematicPath
                                               : "data/Disk2/MIX_FMV1.SMK";

    std::string err;
    decoder_ = revsmk::Decoder::OpenFile(path, &err);
    if (!decoder_)
    {
        log_error("[cinematic] failed to open '%s': %s", path, err.c_str());
        SetDone();
        return true; // don't wedge the boot; just end the screen
    }

    const revsmk::Info& info = decoder_->GetInfo();
    dispW_ = info.width;
    dispH_ = info.displayHeight();
    fps_ = info.fps > 0.0 ? info.fps : 15.0;
    rgba_.assign(size_t(dispW_) * dispH_ * 4, 0);

    texture_ = Renderer->CreateDynamicTexture(dispW_, dispH_);
    if (texture_ == kInvalidTexture)
        log_error("[cinematic] could not create %dx%d stream texture", dispW_, dispH_);

    Renderer->AddHud(this, kCinematicHudZ);
    registeredHud_ = true;

    // Audio: pre-decode track 0 into one contiguous PCM buffer and hand it to
    // miniaudio. (Smacker front-loads ~1s of audio in frame 0 and decoding is
    // cheap, so a single decode pass + Rewind is simplest; streaming PCM
    // playback is the audio worktree's concern.) Started in sync with the first
    // video frame, not here, so A/V begin together.
    const revsmk::AudioInfo& ai = info.audio[0];
    if (audio::Functioning() && ai.present && ai.channels > 0 && ai.bytesPerSample > 0)
    {
        audioPcm_.clear();
        while (decoder_->DecodeNextFrame())
        {
            const std::vector<uint8_t>& chunk = decoder_->AudioData(0);
            if (!chunk.empty())
                audioPcm_.insert(audioPcm_.end(), chunk.begin(), chunk.end());
        }
        decoder_->Rewind();

        if (!audioPcm_.empty())
        {
            WAVEFORMATEX fmt = {};
            fmt.wFormatTag      = 1; // WAVE_FORMAT_PCM
            fmt.nChannels       = uint16_t(ai.channels);
            fmt.nSamplesPerSec  = uint32_t(ai.sampleRate);
            fmt.wBitsPerSample  = uint16_t(ai.bytesPerSample * 8);
            fmt.nBlockAlign     = uint16_t(ai.channels * ai.bytesPerSample);
            fmt.nAvgBytesPerSec = fmt.nSamplesPerSec * fmt.nBlockAlign;
            audio_ = audio::CreateSourceFromPCM(&fmt, audioPcm_.data(),
                                                uint32_t(audioPcm_.size()), false);
            if (!audio_)
                log_warn("[cinematic] audio source creation failed");
        }
    }

    startTime_ = -1.0;
    shownFrame_ = -1;
    log_info("[cinematic] playing '%s' (%dx%d, %d frames @ %.2f fps)",
             path, dispW_, dispH_, info.frameCount, fps_);
    return true;
}

void TCinematicScreen::Close()
{
    if (registeredHud_)
    {
        Renderer->RemoveHud(this);
        registeredHud_ = false;
    }
    if (texture_ != kInvalidTexture)
    {
        Renderer->DestroyDynamicTexture(texture_);
        texture_ = kInvalidTexture;
    }
    if (audio_)
    {
        audio::StopSource(audio_);
        audio::DestroySource(audio_);
        audio_ = nullptr;
    }
    audioPcm_.clear();
    audioPcm_.shrink_to_fit();
    decoder_.reset();
}

void TCinematicScreen::Advance()
{
    if (!decoder_)
        return;

    const double now = TTime::Time();
    if (startTime_ < 0.0)
    {
        startTime_ = now;
        if (audio_)
            audio::PlaySource(audio_, /*volume_ds=*/0, /*freq_hz=*/0, /*pan_ds=*/0);
    }

    // Decode forward to the frame the wall clock has reached.
    const int32_t target = int32_t((now - startTime_) * fps_);
    while (decoder_->CurrentFrame() < target)
        if (!decoder_->DecodeNextFrame())
            break;

    const int32_t cur = decoder_->CurrentFrame();
    if (cur >= 0 && cur != shownFrame_ && texture_ != kInvalidTexture)
    {
        decoder_->BlitRGBA(rgba_.data(), dispW_ * 4);
        Renderer->UpdateDynamicTexture(texture_, rgba_.data(), rgba_.size());
        shownFrame_ = cur;
    }

    if (decoder_->AtEnd())
        SetDone();
}

void TCinematicScreen::Animate(bool draw)
{
    if (!draw)
        return;

    // Decode + upload first (no render pass is open during Animate, so the
    // streaming-texture update is legal here), then clear the backbuffer to
    // black so letterbox bars and any stale 3D are covered. The frame itself
    // is composited later by Draw() inside the swapchain pass.
    Advance();

    if (Display.IsActive() && Display.BackBuffer())
    {
        Display.BackBuffer()->StartPass(0.0f, 0.0f, 0.0f, 1.0f);
        Display.BackBuffer()->EndPass();
    }
}

void TCinematicScreen::Draw()
{
    if (texture_ != kInvalidTexture && shownFrame_ >= 0)
        Renderer->DrawTextureFit(texture_);
}

void TCinematicScreen::KeyPress(int32_t key, bool down)
{
    if (down && key == VK_ESCAPE)
        SetDone();
}

void TCinematicScreen::MouseClick(int32_t button, int32_t x, int32_t y)
{
    (void)button; (void)x; (void)y;
    SetDone();
}
