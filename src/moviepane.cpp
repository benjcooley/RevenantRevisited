// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   moviepane.cpp - TMoviePane, a Smacker movie playing full screen     *
// *************************************************************************
//
// See moviepane.h.

#include "moviepane.h"

#include "audio_backend.h"
#include "logging.h"
#include "revsmk.h"
#include "revutils.h"
#include "time.h"
#include "wavedata.h"   // WAVEFORMATEX

#include <string>

TMoviePane::TMoviePane() : TPane(0, 0, 640, 480) {}

TMoviePane::~TMoviePane()
{
    Close();
}

bool TMoviePane::Open(const char* path)
{
    Close();
    finished = false;

    std::vector<uint8_t> bytes;
    std::string err;
    if (path && rev_read_file(path, bytes))
        decoder = revsmk::Decoder::OpenMemory(std::move(bytes), &err);
    else
        err = "file not found";
    if (!decoder)
    {
        log_error("[movie] can't play '%s': %s", path ? path : "", err.c_str());
        Finish();
        return false;
    }

    const revsmk::Info& info = decoder->GetInfo();
    dispw = info.width;
    disph = info.displayHeight();
    fps   = info.fps > 0.0 ? info.fps : 15.0;
    rgba.assign(size_t(dispw) * disph * 4, 0);

    texture = Renderer->CreateDynamicTexture(dispw, disph);
    if (texture == kInvalidTexture)
        log_error("[movie] can't create a %dx%d stream texture", dispw, disph);

    // The sound track: decoded whole up front into one PCM buffer (Smacker
    // front-loads ~1 s of audio in frame 0 and decoding is cheap), started
    // with the first frame so picture and sound begin together.
    const revsmk::AudioInfo& ai = info.audio[0];
    if (audio::Functioning() && ai.present && ai.channels > 0 && ai.bytesPerSample > 0)
    {
        while (decoder->DecodeNextFrame())
        {
            const std::vector<uint8_t>& chunk = decoder->AudioData(0);
            soundpcm.insert(soundpcm.end(), chunk.begin(), chunk.end());
        }
        decoder->Rewind();

        if (!soundpcm.empty())
        {
            WAVEFORMATEX fmt = {};
            fmt.wFormatTag      = 1;    // WAVE_FORMAT_PCM
            fmt.nChannels       = uint16_t(ai.channels);
            fmt.nSamplesPerSec  = uint32_t(ai.sampleRate);
            fmt.wBitsPerSample  = uint16_t(ai.bytesPerSample * 8);
            fmt.nBlockAlign     = uint16_t(ai.channels * ai.bytesPerSample);
            fmt.nAvgBytesPerSec = fmt.nSamplesPerSec * fmt.nBlockAlign;
            sound = audio::CreateSourceFromPCM(&fmt, soundpcm.data(), uint32_t(soundpcm.size()), false);
            if (!sound)
                log_warn("[movie] sound source creation failed");
        }
    }

    starttime  = -1.0;
    shownframe = -1;
    log_info("[movie] playing '%s' (%dx%d, %d frames @ %.2f fps)", path, dispw, disph,
             info.frameCount, fps);
    return true;
}

void TMoviePane::Close()
{
    if (texture != kInvalidTexture)
    {
        Renderer->DestroyDynamicTexture(texture);
        texture = kInvalidTexture;
    }
    if (sound)
    {
        audio::StopSource(sound);
        audio::DestroySource(sound);
        sound = nullptr;
    }
    soundpcm.clear();
    soundpcm.shrink_to_fit();
    rgba.clear();
    decoder.reset();
}

void TMoviePane::Finish()
{
    if (finished)
        return;
    finished = true;
    if (onFinished)
        onFinished();
}

void TMoviePane::Compose()
{
    if (!decoder || finished)
        return;

    const double now = TTime::Time();
    if (starttime < 0.0)
    {
        starttime = now;
        if (sound)
            audio::PlaySource(sound, /*volume_ds=*/0, /*freq_hz=*/0, /*pan_ds=*/0);
    }

    // Decode forward to the frame the wall clock has reached.
    const int32_t target = int32_t((now - starttime) * fps);
    while (decoder->CurrentFrame() < target)
        if (!decoder->DecodeNextFrame())
            break;

    const int32_t cur = decoder->CurrentFrame();
    if (cur >= 0 && cur != shownframe && texture != kInvalidTexture)
    {
        decoder->BlitRGBA(rgba.data(), dispw * 4);
        Renderer->UpdateDynamicTexture(texture, rgba.data(), rgba.size());
        shownframe = cur;
    }

    if (decoder->AtEnd())
        Finish();
}

void TMoviePane::Draw()
{
    Renderer->FillScreen(0.0f, 0.0f, 0.0f, 1.0f);
    if (texture != kInvalidTexture && shownframe >= 0)
        Renderer->DrawTextureFit(texture);
}

void TMoviePane::KeyPress(int32_t key, bool down)
{
    if (down && key == VK_ESCAPE)
        Finish();
}

void TMoviePane::MouseClick(int32_t button, int32_t x, int32_t y)
{
    (void)button; (void)x; (void)y;
    Finish();
}
