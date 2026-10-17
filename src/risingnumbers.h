// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *   risingnumbers.h - the characters' rising damage numbers, drawn      *
// *************************************************************************
//
// A blow's number floats up over the character it hurt and fades
// (TCharAnimator's rising numbers hold them; TCharacter::Damage shows
// one). Retail's map pass drew them after the scene, character by
// character (0x004141f2 -> TCharAnimator 0x004da750): this layer does,
// over the world and under every HUD layer.

#pragma once

#include "renderer.h"   // THudDrawable

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

class TMapRenderer;
class TSurface;

class TRisingNumberLayer final : public THudDrawable
{
  public:
    static constexpr float kHudZ = -10.0f;      // over the world, under the HUD panels (z 0..10)

    TRisingNumberLayer();
    ~TRisingNumberLayer() override;
    TRisingNumberLayer(const TRisingNumberLayer&) = delete;
    TRisingNumberLayer& operator=(const TRisingNumberLayer&) = delete;

    void Compose(const TMapRenderer& map);
      // Outside any pass, after the world's render: a new number's image,
      // and where each live one is this frame (its rise and fade eased
      // between ticks)
    void Draw() override;
      // The numbers Compose placed

  private:
    struct SImage
    {
        std::unique_ptr<TSurface> surface;
        bool live = false;              // still shown (Compose's sweep)
    };
    struct SPlaced
    {
        TSurface* surface = nullptr;
        int32_t x = 0, y = 0;           // the image's top left, screen pixels
        float alpha = 1.0f;
    };

    TSurface* Image(uint32_t serial, int32_t amount, uint32_t rgb);

    std::unordered_map<uint32_t, SImage> images;    // by the number's serial
    std::vector<SPlaced> placed;                    // this frame's
};
