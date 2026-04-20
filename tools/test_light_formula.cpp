// Verify the Metal shader's inline light-falloff math matches the 1998
// colortable.cpp LUT exactly. Ground truth is MakeColorTables() +
// GetLightBrightness(); shader side is the analytical expression from
// display.cpp's kLightFsMetal. Run:
//   clang++ -std=c++17 -O2 tools/test_light_formula.cpp -o /tmp/test_light
//   /tmp/test_light

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>

// --- colortable.cpp ground truth (verbatim constants + math) -----------
namespace retail {
static constexpr double maxbrightness   = 50.0;
static constexpr double attrange        = 0.0;
static constexpr double attmin          = 256.0 - attrange;
static constexpr double exponent        = -1.1;
static constexpr double multiplierscale = 20.0;

double BrightnessTable[256];

void MakeColorTables()
{
    const double minpower = std::pow(256.0, exponent);
    const double maxpower = std::pow(1.0, exponent) - minpower;
    for (int d = 0; d < 256; d++) {
        double p = std::pow(double(d + 1), exponent) - minpower;
        double b = std::min(p / maxpower * maxbrightness, 1.0);
        if (double(d) > attmin)
            b *= std::max((attrange - (double(d) - attmin)) / attrange, 0.0);
        BrightnessTable[d] = b;
    }
}

double GetLightBrightness(int dist, int intensity, int multiplier)
{
    intensity = std::min(254, std::max(intensity, 1));
    dist = std::max(0, dist) * 254 / intensity;
    if (dist > 254) return 0.0;
    return BrightnessTable[dist] * double(multiplier) / (multiplierscale / 2.0);
}
}  // namespace retail

// --- Shader analytical form (literal transcription of kLightFsMetal) ---
namespace shader {
// const float kMinPower = 0.0019064f;          // pow(256, -1.1)
// const float kScale    = 50.0 / (1.0 - kMinPower);
// float normd = dist * 254.0 / rad;
// float pw    = pow(normd + 1.0, -1.1) - kMinPower;
// float attn  = clamp(pw * kScale, 0.0, 1.0);
// light += col * col_w * attn * ndotl;   <-- col_w is multiplier/10.0 from testscreen
double Attn(double dist_wu, double rad_wu)
{
    if (rad_wu <= 0.0 || dist_wu >= rad_wu) return 0.0;
    const float kMinPower = 0.0022436f;
    const float kScale    = 50.0f / (1.0f - kMinPower);
    float normd = float(dist_wu) * 254.0f / float(rad_wu);
    if (normd >= 254.0f) return 0.0;
    float pw    = std::pow(normd + 1.0f, -1.1f) - kMinPower;
    float attn  = std::max(0.0f, std::min(pw * kScale, 1.0f));
    return double(attn);
}
double PointLightBrightness(double dist_wu, double rad_wu, int multiplier)
{
    return Attn(dist_wu, rad_wu) * (double(multiplier) / 10.0);
}
}  // namespace shader

int main()
{
    retail::MakeColorTables();

    // 1. Raw LUT vs analytical, no intensity normalization. Feed the shader
    //    dist=d wu, rad=254 wu so normd == d — direct comparison to LUT[d].
    printf("d    lut                shader             diff\n");
    double worst = 0.0;
    int worst_d = -1;
    for (int d = 0; d < 256; d += 1) {
        double lut  = retail::BrightnessTable[d];
        double shd  = shader::Attn(double(d), 254.0);
        double diff = std::abs(lut - shd);
        if (diff > worst) { worst = diff; worst_d = d; }
        if (d < 16 || d % 32 == 0 || d >= 250)
            printf("%-4d %-18.10f %-18.10f %.3e\n", d, lut, shd, diff);
    }
    printf("\nworst |lut - shader| = %.3e at d=%d\n\n", worst, worst_d);

    // 2. Full function: GetLightBrightness(dist_wu, intensity_wu, multiplier)
    //    vs shader's PointLightBrightness(dist_wu, rad_wu, multiplier),
    //    sampling a grid of radii (intensities) and multipliers. Pick sample
    //    distances that stress normalization rounding.
    printf("full GetLightBrightness comparison\n");
    printf("rad   mult dist  lut          shader       diff\n");
    const int rads[]  = { 32, 64, 100, 128, 200, 254 };
    const int mults[] = { 10, 28, 50, 63 };
    double full_worst = 0.0;
    for (int rad : rads) {
        for (int mult : mults) {
            for (int dist = 0; dist <= rad + 4; dist += std::max(rad / 16, 1)) {
                double lut = retail::GetLightBrightness(dist, rad, mult);
                double shd = shader::PointLightBrightness(double(dist),
                                                          double(rad),
                                                          mult);
                double diff = std::abs(lut - shd);
                full_worst = std::max(full_worst, diff);
                if (diff > 1e-3 || dist == 0 || dist == rad)
                    printf("%-5d %-4d %-5d %-12.6f %-12.6f %.3e\n",
                           rad, mult, dist, lut, shd, diff);
            }
        }
    }
    printf("\nworst full |lut - shader| = %.3e\n", full_worst);

    return 0;
}
