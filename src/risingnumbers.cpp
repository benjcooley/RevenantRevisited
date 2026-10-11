// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *   risingnumbers.cpp - the characters' rising damage numbers, drawn    *
// *************************************************************************

#include "risingnumbers.h"

#include "character.h"
#include "charanimator.h"
#include "font.h"
#include "fonttable.h"
#include "mappane.h"
#include "maprenderer.h"
#include "surface.h"
#include "time.h"

#include <cmath>
#include <string>

namespace
{
// REVSYNC: 0x004da3b0 -- the number is composed once into a 64 x 32
// bitmap (64 x 64 in the display mode 0x00669324 sets, not the Classic
// one): itoa, the "Med" font (0x0065caec), centred, the black 3-pass
// shadow (0x004be2b0 with flags 0x402); drawn with its top centre at the
// point (0x004da750: x - 32).
constexpr int32_t kImageWidth = 64;
constexpr int32_t kImageHeight = 32;
}  // namespace

TRisingNumberLayer::TRisingNumberLayer() = default;
TRisingNumberLayer::~TRisingNumberLayer() = default;

TSurface* TRisingNumberLayer::Image(uint32_t serial, int32_t amount, uint32_t rgb)
{
    SImage& image = images[serial];
    image.live = true;
    if (image.surface)
        return image.surface.get();
    const SFontAtlas* font = FontTable ? FontTable->Atlas("Med") : nullptr;
    if (!font)
        return nullptr;
    image.surface = std::make_unique<TSurface>(kImageWidth, kImageHeight, SG_PIXELFORMAT_RGBA8);
    image.surface->StartPass(0.0f, 0.0f, 0.0f, 0.0f);
    DrawTextShadowedToTarget(font, std::to_string(amount).c_str(), 0, 0, kImageWidth, kImageHeight,
                             ETextAlign::Center, float((rgb >> 16) & 0xff) / 255.0f,
                             float((rgb >> 8) & 0xff) / 255.0f, float(rgb & 0xff) / 255.0f, kImageWidth,
                             kImageHeight);
    image.surface->EndPass();
    return image.surface.get();
}

// REVSYNC: 0x004da750 -- each live number at its point less the rise
// (1/256 px) and the scroll; alpha full until its last kRisingNumberFadeTicks
// ticks, then ticks / kRisingNumberFadeTicks. Retail drew once a tick; between
// ticks the rise and the fade ease toward the next tick's, which a number
// in its last tick doesn't have.
void TRisingNumberLayer::Compose(const TMapRenderer& map)
{
    using SRisingNumber = TCharAnimator::SRisingNumber;
    placed.clear();
    for (auto& [serial, image] : images)
        image.live = false;

    const float tick = float(TTime::LegacyFrameFraction());
    for (TMapIterator i(nullptr, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
    {
        TObjectInstance* inst = i.Item();
        if (!inst || (inst->GetFlags() & OF_INVISIBLE))
            continue;                                   // not drawn, as retail's draw list
        const auto* animator = dynamic_cast<const TCharAnimator*>(inst->GetAnimator());
        if (!animator)
            continue;
        for (const SRisingNumber& n : animator->RisingNumbers())
        {
            if (n.ticks <= 0)
                continue;
            TSurface* surface = Image(n.serial, n.amount, n.rgb);
            const float ease = n.ticks > 1 ? tick : 0.0f;
            const float rise = (float(n.rise) + ease * float(TCharAnimator::kRisingNumberRise)) / 256.0f;
            const float ticks = float(n.ticks) - ease;
            const float alpha = (std::min)(ticks / float(TCharAnimator::kRisingNumberFadeTicks), 1.0f);
            float sx = 0.0f, sy = 0.0f;
            if (!surface || !map.MapScreenToScreen(float(n.x), float(n.y) - rise, sx, sy))
                continue;
            placed.push_back(SPlaced{surface, MapPane.GetPosX() + int32_t(std::lround(sx)) - kImageWidth / 2,
                                     MapPane.GetPosY() + int32_t(std::lround(sy)), alpha});
        }
    }

    for (auto it = images.begin(); it != images.end();)
        it = it->second.live ? std::next(it) : images.erase(it);
}

void TRisingNumberLayer::Draw()
{
    for (const SPlaced& p : placed)
        Renderer->DrawSurfaceTinted(p.surface, p.x, p.y, 1.0f, 1.0f, 1.0f, p.alpha);
}
