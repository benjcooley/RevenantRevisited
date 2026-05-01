#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace {

constexpr int kW = 640;
constexpr int kH = 480;
constexpr float kPerspectiveTileCoverageScale = 1.02f;

float focal_from_fov(float fov_deg) {
    constexpr float kPi = 3.14159265358979323846f;
    const float fov_rad = fov_deg * (kPi / 180.0f);
    return (float(kH) * 0.5f) / std::tan(fov_rad * 0.5f);
}

struct float2 { float x = 0.0f, y = 0.0f; };
struct float4 { float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f; };

struct Params {
    float4 rect;
    float4 zparams;
    float4 tile_root;
    float4 tile_sprite;
    float4 filter;
    float4 obj_id;
    float4 camera;
    float4 proj;
    float4 tile_rect;
    float4 cam_tile;
};

struct Rgba {
    uint8_t r = 0, g = 0, b = 0, a = 255;
};

struct SampleMode {
    bool linear_color = false;
    bool linear_depth = false;
};

struct TileBitmap {
    int w = 0, h = 0;
    std::vector<Rgba> color;
    std::vector<float> depth;
    float zmin = 0.0f, zmax = 0.0f;

    bool sample(float2 uv, Rgba& out_color, float& out_z) const {
        if (uv.x < 0.0f || uv.x > 1.0f || uv.y < 0.0f || uv.y > 1.0f) return false;
        int x = std::clamp(int(uv.x * float(w)), 0, w - 1);
        int y = std::clamp(int(uv.y * float(h)), 0, h - 1);
        const int idx = y * w + x;
        out_color = color[idx];
        out_z = depth[idx];
        return out_color.a >= 3;
    }

    bool sample(float2 uv, SampleMode mode, Rgba& out_color, float& out_z) const {
        if (!mode.linear_color && !mode.linear_depth)
            return sample(uv, out_color, out_z);
        if (uv.x < 0.0f || uv.x > 1.0f || uv.y < 0.0f || uv.y > 1.0f) return false;
        const int xn = std::clamp(int(uv.x * float(w)), 0, w - 1);
        const int yn = std::clamp(int(uv.y * float(h)), 0, h - 1);
        const float fx = std::clamp(uv.x * float(w) - 0.5f, 0.0f, float(w - 1));
        const float fy = std::clamp(uv.y * float(h) - 0.5f, 0.0f, float(h - 1));
        const int x0 = int(std::floor(fx));
        const int y0 = int(std::floor(fy));
        const int x1 = std::min(x0 + 1, w - 1);
        const int y1 = std::min(y0 + 1, h - 1);
        const float tx = fx - float(x0);
        const float ty = fy - float(y0);
        auto idx = [&](int x, int y) { return size_t(y) * size_t(w) + size_t(x); };
        if (mode.linear_color) {
            const Rgba c00 = color[idx(x0, y0)], c10 = color[idx(x1, y0)];
            const Rgba c01 = color[idx(x0, y1)], c11 = color[idx(x1, y1)];
            auto bilerp = [&](int a00, int a10, int a01, int a11) -> uint8_t {
                const float a0 = float(a00) * (1.0f - tx) + float(a10) * tx;
                const float a1 = float(a01) * (1.0f - tx) + float(a11) * tx;
                return uint8_t(std::clamp(a0 * (1.0f - ty) + a1 * ty, 0.0f, 255.0f));
            };
            out_color = {
                bilerp(c00.r, c10.r, c01.r, c11.r),
                bilerp(c00.g, c10.g, c01.g, c11.g),
                bilerp(c00.b, c10.b, c01.b, c11.b),
                bilerp(c00.a, c10.a, c01.a, c11.a)
            };
        } else {
            out_color = color[idx(xn, yn)];
        }
        if (mode.linear_depth) {
            const float z00 = depth[idx(x0, y0)], z10 = depth[idx(x1, y0)];
            const float z01 = depth[idx(x0, y1)], z11 = depth[idx(x1, y1)];
            const float z0 = z00 * (1.0f - tx) + z10 * tx;
            const float z1 = z01 * (1.0f - tx) + z11 * tx;
            out_z = z0 * (1.0f - ty) + z1 * ty;
        } else {
            const int xn = std::clamp(int(uv.x * float(w)), 0, w - 1);
            const int yn = std::clamp(int(uv.y * float(h)), 0, h - 1);
            out_z = depth[idx(xn, yn)];
        }
        return out_color.a >= 3;
    }

    bool depth_valid(float2 uv) const {
        if (uv.x < 0.0f || uv.x > 1.0f || uv.y < 0.0f || uv.y > 1.0f) return false;
        int x = std::clamp(int(uv.x * float(w)), 0, w - 1);
        int y = std::clamp(int(uv.y * float(h)), 0, h - 1);
        return depth[size_t(y) * size_t(w) + size_t(x)] != 0.0f;
    }

    float sample_depth(float2 uv, bool linear_depth) const {
        if (!linear_depth) {
            int x = std::clamp(int(uv.x * float(w)), 0, w - 1);
            int y = std::clamp(int(uv.y * float(h)), 0, h - 1);
            return depth[size_t(y) * size_t(w) + size_t(x)];
        }

        const float fx = std::clamp(uv.x * float(w) - 0.5f, 0.0f, float(w - 1));
        const float fy = std::clamp(uv.y * float(h) - 0.5f, 0.0f, float(h - 1));
        const int x0 = int(std::floor(fx));
        const int y0 = int(std::floor(fy));
        const int x1 = std::min(x0 + 1, w - 1);
        const int y1 = std::min(y0 + 1, h - 1);
        const float tx = fx - float(x0);
        const float ty = fy - float(y0);
        auto idx = [&](int x, int y) { return size_t(y) * size_t(w) + size_t(x); };
        const float z00 = depth[idx(x0, y0)], z10 = depth[idx(x1, y0)];
        const float z01 = depth[idx(x0, y1)], z11 = depth[idx(x1, y1)];
        const float z0 = z00 * (1.0f - tx) + z10 * tx;
        const float z1 = z01 * (1.0f - tx) + z11 * tx;
        return z0 * (1.0f - ty) + z1 * ty;
    }

    Rgba sample_color(float2 uv, bool linear_color) const {
        float unused_z = 0.0f;
        Rgba out;
        sample(uv, SampleMode{linear_color, false}, out, unused_z);
        return out;
    }
};

struct Tile {
    Params p;
    TileBitmap bm;
    Rgba debug_color = {255,255,255,255};
};

float2 logical_tile_dims(const TileBitmap& bm) {
    constexpr float kIsoCos30 = 0.867f;
    // Large terrain supertiles include a small art/depth bleed around the
    // logical diamond. Foroooo is 260x130 pixels but occupies a 256x128
    // camera-space footprint (2x2 128x64 cells).
    if (bm.w == 260 && bm.h == 130) return {256.0f, 128.0f};
    if (bm.zmax > bm.zmin) {
        // For a flat 30-degree tile, zspan = 2*cos30*(logical_h - 1)
        // because z samples live at texel centers.
        const float h = (bm.zmax - bm.zmin) / (2.0f * kIsoCos30) + 1.0f;
        return {2.0f * h, h};
    }
    return {float(bm.w), float(bm.h)};
}

uint32_t crc32_bytes(const uint8_t* data, size_t len) {
    static uint32_t table[256] = {};
    static bool init = false;
    if (!init) {
        for (uint32_t i = 0; i < 256; ++i) {
            uint32_t c = i;
            for (int k = 0; k < 8; ++k)
                c = (c & 1u) ? (0xEDB88320u ^ (c >> 1u)) : (c >> 1u);
            table[i] = c;
        }
        init = true;
    }
    uint32_t c = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; ++i)
        c = table[(c ^ data[i]) & 0xFFu] ^ (c >> 8u);
    return c ^ 0xFFFFFFFFu;
}

uint32_t adler32_bytes(const uint8_t* data, size_t len) {
    uint32_t a = 1, b = 0;
    for (size_t i = 0; i < len; ++i) {
        a = (a + data[i]) % 65521u;
        b = (b + a) % 65521u;
    }
    return (b << 16u) | a;
}

void append_be32(std::vector<uint8_t>& out, uint32_t v) {
    out.push_back(uint8_t((v >> 24u) & 0xFFu));
    out.push_back(uint8_t((v >> 16u) & 0xFFu));
    out.push_back(uint8_t((v >> 8u) & 0xFFu));
    out.push_back(uint8_t(v & 0xFFu));
}

void append_chunk(std::vector<uint8_t>& png, const char type[4], const std::vector<uint8_t>& payload) {
    append_be32(png, uint32_t(payload.size()));
    const size_t type_off = png.size();
    png.push_back(uint8_t(type[0]));
    png.push_back(uint8_t(type[1]));
    png.push_back(uint8_t(type[2]));
    png.push_back(uint8_t(type[3]));
    png.insert(png.end(), payload.begin(), payload.end());
    append_be32(png, crc32_bytes(png.data() + type_off, 4 + payload.size()));
}

bool write_png_rgba(const std::filesystem::path& path, int width, int height, const std::vector<Rgba>& pixels) {
    std::vector<uint8_t> raw;
    raw.reserve(size_t(height) * (size_t(width) * 4 + 1));
    for (int y = 0; y < height; ++y) {
        raw.push_back(0);
        for (int x = 0; x < width; ++x) {
            const Rgba& p = pixels[size_t(y) * width + x];
            raw.push_back(p.r); raw.push_back(p.g); raw.push_back(p.b); raw.push_back(p.a);
        }
    }

    std::vector<uint8_t> z;
    z.push_back(0x78);
    z.push_back(0x01);
    size_t off = 0;
    while (off < raw.size()) {
        const uint16_t n = uint16_t(std::min<size_t>(65535, raw.size() - off));
        const bool final = (off + n) == raw.size();
        z.push_back(final ? 1 : 0);
        z.push_back(uint8_t(n & 0xFFu));
        z.push_back(uint8_t((n >> 8u) & 0xFFu));
        const uint16_t nn = uint16_t(~n);
        z.push_back(uint8_t(nn & 0xFFu));
        z.push_back(uint8_t((nn >> 8u) & 0xFFu));
        z.insert(z.end(), raw.begin() + ptrdiff_t(off), raw.begin() + ptrdiff_t(off + n));
        off += n;
    }
    append_be32(z, adler32_bytes(raw.data(), raw.size()));

    std::vector<uint8_t> png = { 137,80,78,71,13,10,26,10 };
    std::vector<uint8_t> ihdr;
    append_be32(ihdr, uint32_t(width));
    append_be32(ihdr, uint32_t(height));
    ihdr.push_back(8); ihdr.push_back(6); ihdr.push_back(0); ihdr.push_back(0); ihdr.push_back(0);
    append_chunk(png, "IHDR", ihdr);
    append_chunk(png, "IDAT", z);
    append_chunk(png, "IEND", {});

    std::ofstream f(path, std::ios::binary);
    if (!f) return false;
    f.write(reinterpret_cast<const char*>(png.data()), std::streamsize(png.size()));
    return f.good();
}

bool write_png(const std::filesystem::path& path, const std::vector<Rgba>& pixels) {
    return write_png_rgba(path, kW, kH, pixels);
}

void put(std::vector<Rgba>& img, int x, int y, Rgba c) {
    if (x >= 0 && y >= 0 && x < kW && y < kH) img[size_t(y) * kW + x] = c;
}

void blend(std::vector<Rgba>& img, int x, int y, Rgba c) {
    if (x < 0 || y < 0 || x >= kW || y >= kH) return;
    Rgba& d = img[size_t(y) * kW + x];
    const float a = c.a / 255.0f;
    const float ia = 1.0f - a;
    d.r = uint8_t(c.r * a + d.r * ia);
    d.g = uint8_t(c.g * a + d.g * ia);
    d.b = uint8_t(c.b * a + d.b * ia);
    d.a = 255;
}

void line(std::vector<Rgba>& img, float x0, float y0, float x1, float y1, Rgba c) {
    const int steps = std::max(1, int(std::max(std::fabs(x1 - x0), std::fabs(y1 - y0))));
    for (int i = 0; i <= steps; ++i) {
        const float t = float(i) / float(steps);
        blend(img, int(std::lround(x0 + (x1 - x0) * t)), int(std::lround(y0 + (y1 - y0) * t)), c);
    }
}

float flat_iso_square_depth(float /*cam_x*/, float cam_y) {
    // Revenant camera-space depth for a flat world-z=0 square under the
    // original 30-degree isometric camera. Since camera-plane Y is:
    //   y = 0.5 * (world_x + world_y) - world_z * cos30
    // and camera depth changes as:
    //   depth = -cos30 * (world_x + world_y) - 0.5 * world_z
    // a flat square has local depth:
    //   depth_delta = -2 * cos30 * camera_y
    constexpr float kIsoCos30 = 0.867f;
    return -2.0f * kIsoCos30 * cam_y;
}

TileBitmap make_bitmap(float cx, float cy, Rgba base) {
    constexpr int tw = 160;
    constexpr int th = 80;
    TileBitmap bm;
    bm.w = tw; bm.h = th;
    bm.color.resize(tw * th);
    bm.depth.resize(tw * th);
    bm.zmin = 1e30f;
    bm.zmax = -1e30f;
    const float anchor_depth = flat_iso_square_depth(cx, cy);
    for (int y = 0; y < th; ++y) {
        for (int x = 0; x < tw; ++x) {
            const float lx = x + 0.5f - tw * 0.5f;
            const float ly = y + 0.5f - th * 0.5f;
            const bool inside = (std::fabs(lx) / (tw * 0.5f) + std::fabs(ly) / (th * 0.5f)) <= 1.0f;
            const size_t idx = size_t(y) * tw + x;
            if (!inside) {
                bm.color[idx] = {0,0,0,0};
                bm.depth[idx] = 0.0f;
                continue;
            }
            // Depth textures are tile-local. The tile anchor carries the
            // camera-space depth of the registration point; zraw carries only
            // the per-texel offset from that anchor.
            const float z = flat_iso_square_depth(cx + lx, cy + ly) - anchor_depth;
            bm.depth[idx] = z;
            bm.zmin = std::min(bm.zmin, z);
            bm.zmax = std::max(bm.zmax, z);
            const float shade = (((x / 8) + (y / 8)) & 1) ? 0.82f : 1.0f;
            bm.color[idx] = { uint8_t(base.r * shade), uint8_t(base.g * shade), uint8_t(base.b * shade), 255 };
        }
    }
    return bm;
}

bool load_ztile(const std::filesystem::path& path, TileBitmap& bm) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return false;
    uint32_t magic = 0;
    int32_t w = 0, h = 0;
    f.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    f.read(reinterpret_cast<char*>(&w), sizeof(w));
    f.read(reinterpret_cast<char*>(&h), sizeof(h));
    if (!f || magic != 0x455A5452u || w <= 0 || h <= 0) return false;
    std::vector<uint8_t> rgba(size_t(w) * size_t(h) * 4);
    std::vector<float> depth(size_t(w) * size_t(h));
    f.read(reinterpret_cast<char*>(rgba.data()), std::streamsize(rgba.size()));
    f.read(reinterpret_cast<char*>(depth.data()), std::streamsize(depth.size() * sizeof(float)));
    if (!f) return false;
    bm.w = w;
    bm.h = h;
    bm.color.resize(size_t(w) * size_t(h));
    bm.depth = std::move(depth);
    bm.zmin = 1e30f;
    bm.zmax = -1e30f;
    for (int i = 0; i < w * h; ++i) {
        bm.color[i] = { rgba[size_t(i) * 4 + 0], rgba[size_t(i) * 4 + 1],
                        rgba[size_t(i) * 4 + 2], rgba[size_t(i) * 4 + 3] };
        if (bm.color[i].a > 0) {
            bm.zmin = std::min(bm.zmin, bm.depth[i]);
            bm.zmax = std::max(bm.zmax, bm.depth[i]);
        }
    }
    if (bm.zmin > bm.zmax) bm.zmin = bm.zmax = 0.0f;
    return true;
}

void write_bitmap_diagnostics() {
    std::filesystem::create_directories("build/cpu_tile_perspective");
    TileBitmap bm = make_bitmap(0.0f, 0.0f, {70, 180, 220, 255});

    write_png_rgba("build/cpu_tile_perspective/synthetic_tile_color.png", bm.w, bm.h, bm.color);

    std::vector<Rgba> alpha(size_t(bm.w) * bm.h);
    std::vector<Rgba> depth(size_t(bm.w) * bm.h);
    const float span = std::max(bm.zmax - bm.zmin, 1.0f);
    for (int y = 0; y < bm.h; ++y) {
        for (int x = 0; x < bm.w; ++x) {
            const size_t idx = size_t(y) * bm.w + x;
            const bool valid = bm.color[idx].a > 0;
            alpha[idx] = valid ? Rgba{255,255,255,255} : Rgba{0,0,0,255};
            const float t = valid ? (bm.depth[idx] - bm.zmin) / span : 0.0f;
            depth[idx] = valid
                ? Rgba{uint8_t(255.0f * t), uint8_t(128.0f + 127.0f * t), uint8_t(255.0f * (1.0f - t)), 255}
                : Rgba{0,0,0,255};
        }
    }
    write_png_rgba("build/cpu_tile_perspective/synthetic_tile_alpha.png", bm.w, bm.h, alpha);
    write_png_rgba("build/cpu_tile_perspective/synthetic_tile_depth.png", bm.w, bm.h, depth);

    std::ofstream report("build/cpu_tile_perspective/synthetic_tile_report.txt");
    report << "synthetic tile bitmap\n";
    report << "size " << bm.w << "x" << bm.h << "\n";
    report << "registration " << (bm.w * 0.5f) << "," << (bm.h * 0.5f) << "\n";
    report << "diamond corners local-camera-space:\n";
    report << "  top    0," << -(bm.h * 0.5f) << "\n";
    report << "  right  " << (bm.w * 0.5f) << ",0\n";
    report << "  bottom 0," << (bm.h * 0.5f) << "\n";
    report << "  left   " << -(bm.w * 0.5f) << ",0\n";
    report << "zmin " << bm.zmin << "\n";
    report << "zmax " << bm.zmax << "\n";
    report << "depth model flat world-z=0 iso square: zraw=-2*cos30*local_camera_y\n";
    report << "corner z samples from flat 30-degree source:\n";
    report << "  top    " << flat_iso_square_depth(0.0f, -(bm.h * 0.5f)) << "\n";
    report << "  right  " << flat_iso_square_depth(bm.w * 0.5f, 0.0f) << "\n";
    report << "  bottom " << flat_iso_square_depth(0.0f, bm.h * 0.5f) << "\n";
    report << "  left   " << flat_iso_square_depth(-(bm.w * 0.5f), 0.0f) << "\n";
}

bool is_dark_bg(Rgba c) {
    return c.a > 0 && (int(c.r) + int(c.g) + int(c.b)) < 24;
}

void write_tile_analysis(const TileBitmap& bm, const char* label) {
    std::vector<Rgba> out(size_t(bm.w) * bm.h, {0, 0, 0, 255});
    int transparent = 0;
    int valid_dark = 0;
    int edge = 0;
    int z_outlier = 0;
    int stable = 0;

    auto valid_at = [&](int x, int y) -> bool {
        if (x < 0 || y < 0 || x >= bm.w || y >= bm.h) return false;
        return bm.color[size_t(y) * bm.w + x].a > 0;
    };

    for (int y = 0; y < bm.h; ++y) {
        for (int x = 0; x < bm.w; ++x) {
            const size_t idx = size_t(y) * bm.w + x;
            const Rgba c = bm.color[idx];
            if (c.a == 0) {
                ++transparent;
                out[idx] = {0, 0, 0, 255};
                continue;
            }

            float samples[25];
            int n = 0;
            for (int oy = -2; oy <= 2; ++oy) {
                for (int ox = -2; ox <= 2; ++ox) {
                    const int nx = x + ox;
                    const int ny = y + oy;
                    if (!valid_at(nx, ny)) continue;
                    samples[n++] = bm.depth[size_t(ny) * bm.w + nx];
                }
            }
            std::sort(samples, samples + n);
            const float median = n > 0 ? samples[n / 2] : bm.depth[idx];
            const float zerr = std::fabs(bm.depth[idx] - median);

            const bool edge_px =
                !valid_at(x - 1, y) || !valid_at(x + 1, y) ||
                !valid_at(x, y - 1) || !valid_at(x, y + 1);
            const bool dark = is_dark_bg(c);
            const bool outlier = n >= 5 && zerr > 24.0f;

            if (dark) {
                ++valid_dark;
                out[idx] = {255, 0, 0, 255};          // valid but visually black
            } else if (outlier) {
                ++z_outlier;
                out[idx] = {255, 0, 255, 255};        // local z disagrees
            } else if (edge_px) {
                ++edge;
                out[idx] = {255, 220, 0, 255};        // valid silhouette edge
            } else {
                ++stable;
                const float t = (bm.zmax > bm.zmin) ? (bm.depth[idx] - bm.zmin) / (bm.zmax - bm.zmin) : 0.5f;
                out[idx] = {0, uint8_t(96 + 120 * t), 255, 255}; // stable interior
            }
        }
    }

    std::filesystem::create_directories("build/cpu_tile_perspective");
    const std::string stem = std::string("tile_analysis_") + label;
    write_png_rgba(std::filesystem::path("build/cpu_tile_perspective") / (stem + ".png"), bm.w, bm.h, out);
    std::ofstream report(std::filesystem::path("build/cpu_tile_perspective") / (stem + ".txt"));
    report << "tile analysis " << label << "\n";
    report << "size " << bm.w << "x" << bm.h << "\n";
    report << "zmin " << bm.zmin << "\n";
    report << "zmax " << bm.zmax << "\n";
    report << "transparent " << transparent << "\n";
    report << "valid_dark " << valid_dark << "\n";
    report << "edge " << edge << "\n";
    report << "z_outlier " << z_outlier << "\n";
    report << "stable " << stable << "\n";
    report << "legend black=transparent red=valid-dark magenta=z-outlier yellow=edge blue=stable\n";
}

void write_tile_validity_diagnostics(const TileBitmap& bm, const char* label) {
    std::vector<Rgba> variance(size_t(bm.w) * bm.h, {0, 0, 0, 255});
    std::vector<Rgba> validity(size_t(bm.w) * bm.h, {20, 20, 20, 255});
    int transparent = 0;
    int z_invalid = 0;
    int both = 0;
    int high_variance = 0;
    int valid = 0;
    int holes4 = 0;
    int holes8 = 0;

    auto alpha_valid = [&](int x, int y) -> bool {
        if (x < 0 || y < 0 || x >= bm.w || y >= bm.h) return false;
        return bm.color[size_t(y) * bm.w + x].a > 0;
    };
    auto z_valid = [&](int x, int y) -> bool {
        if (x < 0 || y < 0 || x >= bm.w || y >= bm.h) return false;
        return bm.depth[size_t(y) * bm.w + x] != 0.0f;
    };

    for (int y = 0; y < bm.h; ++y) {
        for (int x = 0; x < bm.w; ++x) {
            const size_t idx = size_t(y) * bm.w + x;
            const bool aok = alpha_valid(x, y);
            const bool zok = z_valid(x, y);
            if (!aok && !zok) {
                ++both;
                validity[idx] = {255, 0, 255, 255};      // both transparent and z-invalid
            } else if (!aok) {
                ++transparent;
                validity[idx] = {255, 0, 0, 255};        // transparent only
            } else if (!zok) {
                ++z_invalid;
                validity[idx] = {0, 80, 255, 255};       // z-invalid only
            } else {
                ++valid;
                validity[idx] = {0, 180, 60, 255};       // fully valid
            }

            if (!aok && !zok) {
                const bool l = alpha_valid(x - 1, y) && z_valid(x - 1, y);
                const bool r = alpha_valid(x + 1, y) && z_valid(x + 1, y);
                const bool u = alpha_valid(x, y - 1) && z_valid(x, y - 1);
                const bool d = alpha_valid(x, y + 1) && z_valid(x, y + 1);
                const int n4 = (l?1:0) + (r?1:0) + (u?1:0) + (d?1:0);
                int n8 = n4;
                n8 += (alpha_valid(x - 1, y - 1) && z_valid(x - 1, y - 1)) ? 1 : 0;
                n8 += (alpha_valid(x + 1, y - 1) && z_valid(x + 1, y - 1)) ? 1 : 0;
                n8 += (alpha_valid(x - 1, y + 1) && z_valid(x - 1, y + 1)) ? 1 : 0;
                n8 += (alpha_valid(x + 1, y + 1) && z_valid(x + 1, y + 1)) ? 1 : 0;
                if (n4 == 4) {
                    ++holes4;
                    validity[idx] = {255, 255, 0, 255};  // strict 4-neighbor hole
                } else if (n8 >= 7) {
                    ++holes8;
                    validity[idx] = {255, 128, 0, 255};  // near-hole
                }
            }

            if (!aok || !zok)
                continue;

            float min_z = bm.depth[idx];
            float max_z = bm.depth[idx];
            int n = 0;
            for (int oy = -1; oy <= 1; ++oy) {
                for (int ox = -1; ox <= 1; ++ox) {
                    if (ox == 0 && oy == 0) continue;
                    const int nx = x + ox;
                    const int ny = y + oy;
                    if (!alpha_valid(nx, ny) || !z_valid(nx, ny)) continue;
                    const float z = bm.depth[size_t(ny) * bm.w + nx];
                    min_z = std::min(min_z, z);
                    max_z = std::max(max_z, z);
                    ++n;
                }
            }
            const float dz = max_z - min_z;
            if (dz > 48.0f) ++high_variance;
            const float t = std::min(dz / 96.0f, 1.0f);
            // Height-neighbor variation heatmap: blue=low, yellow=medium, red=high.
            if (t < 0.5f) {
                const float u = t * 2.0f;
                variance[idx] = {uint8_t(255.0f * u), uint8_t(255.0f * u), 255, 255};
            } else {
                const float u = (t - 0.5f) * 2.0f;
                variance[idx] = {255, uint8_t(255.0f * (1.0f - u)), 0, 255};
            }
        }
    }

    std::filesystem::create_directories("build/cpu_tile_perspective");
    const std::string stem = std::string("tile_validity_") + label;
    write_png_rgba(std::filesystem::path("build/cpu_tile_perspective") / (stem + ".png"), bm.w, bm.h, validity);
    write_png_rgba(std::filesystem::path("build/cpu_tile_perspective") / (std::string("tile_height_variance_") + label + ".png"), bm.w, bm.h, variance);
    std::ofstream report(std::filesystem::path("build/cpu_tile_perspective") / (stem + ".txt"));
    report << "tile validity diagnostics " << label << "\n";
    report << "size " << bm.w << "x" << bm.h << "\n";
    report << "valid " << valid << "\n";
    report << "transparent_only " << transparent << "\n";
    report << "z_invalid_only " << z_invalid << "\n";
    report << "both_transparent_and_z_invalid " << both << "\n";
    report << "holes_4_neighbor " << holes4 << "\n";
    report << "holes_8_neighbor_7plus " << holes8 << "\n";
    report << "high_3x3_height_variance_over_48 " << high_variance << "\n";
    report << "validity legend green=valid red=transparent-only blue=z-invalid-only magenta=both yellow=4-hole orange=8-near-hole\n";
    report << "height variance legend blue=low yellow=medium red=high\n";
}

bool tile_fragment_perspective(const Tile& tile, float2 frag_px, Rgba& out_color, float& out_depth,
                               SampleMode mode = {},
                               float2* out_hit_uv = nullptr, float* out_hit_zraw = nullptr) {
    // CPU transcription of the current Metal perspective branch.
    const Params& p = tile.p;
    const float zoom = std::max(p.zparams.w, 0.0001f);
    const float tile_scale = std::max(p.zparams.z, 0.0001f);
    const float2 ray_cam_xy = {
        (frag_px.x - p.proj.x) / std::max(p.camera.z * zoom, 1.0f),
        (frag_px.y - p.proj.y) / std::max(p.camera.z * zoom, 1.0f)
    };
    float z0 = p.cam_tile.z + std::min(p.filter.z, p.filter.w) * p.cam_tile.w;
    float z1 = p.cam_tile.z + std::max(p.filter.z, p.filter.w) * p.cam_tile.w;
    if (z1 < z0) std::swap(z0, z1);
    z0 = std::max(z0, 1.0f);
    z1 = std::max(z1, z0 + 1.0f);

    float prev_z = z0;
    float prev_diff = 0.0f;
    float2 prev_uv = {};
    bool prev_valid = false;
    constexpr int kVolumeSteps = 32;
    for (int i = 0; i < kVolumeSteps; ++i) {
        const float cam_z = z0 + (z1 - z0) * (float(i) / float(kVolumeSteps - 1));
        const float2 cam_xy = { ray_cam_xy.x * cam_z, ray_cam_xy.y * cam_z };
        const float sx = (cam_xy.x - p.cam_tile.x) / tile_scale;
        const float sy = (cam_xy.y - p.cam_tile.y) / tile_scale;
        const float2 uv = {
            (sx + p.tile_sprite.x) / std::max(p.tile_sprite.z, 1.0f),
            (sy + p.tile_sprite.y) / std::max(p.tile_sprite.w, 1.0f)
        };
        if (!tile.bm.depth_valid(uv)) {
            prev_valid = false;
            continue;
        }
        float zraw = tile.bm.sample_depth(uv, mode.linear_depth);
        const float surf_z = p.cam_tile.z + zraw * p.cam_tile.w;
        const float diff = surf_z - cam_z;
        if (prev_valid && prev_diff >= 0.0f && diff <= 0.0f) {
            float lo_z = prev_z, hi_z = cam_z;
            float2 lo_uv = prev_uv, hi_uv = uv;
            for (int r = 0; r < 5; ++r) {
                const float mid_z = 0.5f * (lo_z + hi_z);
                const float2 mid_xy = { ray_cam_xy.x * mid_z, ray_cam_xy.y * mid_z };
                const float mid_sx = (mid_xy.x - p.cam_tile.x) / tile_scale;
                const float mid_sy = (mid_xy.y - p.cam_tile.y) / tile_scale;
                const float2 mid_uv = {
                    (mid_sx + p.tile_sprite.x) / std::max(p.tile_sprite.z, 1.0f),
                    (mid_sy + p.tile_sprite.y) / std::max(p.tile_sprite.w, 1.0f)
                };
                if (!tile.bm.depth_valid(mid_uv)) {
                    hi_z = mid_z;
                    hi_uv = mid_uv;
                    continue;
                }
                float mz = tile.bm.sample_depth(mid_uv, mode.linear_depth);
                const float mid_diff = (p.cam_tile.z + mz * p.cam_tile.w) - mid_z;
                if (mid_diff >= 0.0f) { lo_z = mid_z; lo_uv = mid_uv; }
                else { hi_z = mid_z; hi_uv = mid_uv; }
            }
            const float2 hit_uv = { 0.5f * (lo_uv.x + hi_uv.x), 0.5f * (lo_uv.y + hi_uv.y) };
            if (!tile.bm.depth_valid(hit_uv)) return false;
            float hit_zraw = tile.bm.sample_depth(hit_uv, mode.linear_depth);
            out_color = tile.bm.sample_color(hit_uv, mode.linear_color);
            out_depth = p.cam_tile.z + hit_zraw * p.cam_tile.w;
            if (out_hit_uv) *out_hit_uv = hit_uv;
            if (out_hit_zraw) *out_hit_zraw = hit_zraw;
            return true;
        }
        prev_valid = true;
        prev_z = cam_z;
        prev_diff = diff;
        prev_uv = uv;
    }
    return false;
}

bool tile_fragment_perspective_uv(const Tile& tile, float2 frag_px, Rgba& out_color, float& out_depth, float2& out_uv) {
    if (!tile_fragment_perspective(tile, frag_px, out_color, out_depth, {})) return false;
    // Re-run a lightweight final mapping by following the same accepted color
    // is hard without instrumenting the function; this wrapper is just a
    // placeholder for fringe picking where image output is the first target.
    out_uv = {-1.0f, -1.0f};
    return true;
}

bool tile_fragment_orthographic(const Tile& tile, float2 frag_px, Rgba& out_color, float& out_depth) {
    const Params& p = tile.p;
    const float sx = frag_px.x - p.proj.x - p.cam_tile.x;
    const float sy = frag_px.y - p.proj.y - p.cam_tile.y;
    const float2 uv = {
        (sx + p.tile_sprite.x) / std::max(p.tile_sprite.z, 1.0f),
        (sy + p.tile_sprite.y) / std::max(p.tile_sprite.w, 1.0f)
    };
    float zraw = 0.0f;
    if (!tile.bm.sample(uv, out_color, zraw)) return false;
    out_depth = p.cam_tile.z + zraw * p.cam_tile.w;
    return true;
}

float2 project(float cam_x, float cam_y, float cam_z, float focal) {
    return { kW * 0.5f + cam_x * focal / cam_z, kH * 0.5f + cam_y * focal / cam_z };
}

float2 project_case(float cam_x, float cam_y, float cam_z, float focal, bool perspective) {
    if (!perspective) return { kW * 0.5f + cam_x, kH * 0.5f + cam_y };
    return project(cam_x, cam_y, cam_z, focal);
}

void render(float focal, bool perspective, const char* suffix, const TileBitmap* source_bitmap = nullptr,
            float base_z = 1800.0f, SampleMode sample_mode = {}) {
    std::vector<Tile> tiles;
    const Rgba palette[] = {
        {210,70,55,255}, {70,200,80,255}, {70,120,220,255},
        {220,190,70,255}, {70,200,210,255}, {190,90,220,255}
    };
    const float2 logical = source_bitmap ? logical_tile_dims(*source_bitmap) : float2{160.0f, 80.0f};
    const float tile_w = logical.x;
    const float tile_h = logical.y;
    constexpr int radius = 4;
    for (int gy = -radius; gy <= radius; ++gy) {
        for (int gx = -radius; gx <= radius; ++gx) {
            const float cx = (gx - gy) * tile_w * 0.5f;
            const float cy = (gx + gy) * tile_h * 0.5f;
            Tile t;
            t.bm = source_bitmap ? *source_bitmap : make_bitmap(cx, cy, palette[(gx + gy + 64) % 6]);
            t.debug_color = palette[(gx + gy + 64) % 6];
            t.p.zparams = {0,0,perspective ? kPerspectiveTileCoverageScale : 1.0f,1};
            const float bw = float(t.bm.w);
            const float bh = float(t.bm.h);
            t.p.tile_sprite = { bw * 0.5f, bh * 0.5f, bw, bh };
            t.p.filter = {1.5f, 64.0f, t.bm.zmin, t.bm.zmax};
            t.p.camera = {0,0,focal,perspective ? 1.0f : 0.0f};
            t.p.proj = {kW * 0.5f, kH * 0.5f, kW, kH};
            t.p.tile_rect = {0,0,tile_w,tile_h};
            t.p.cam_tile = {cx, cy, base_z + flat_iso_square_depth(cx, cy), 1.0f};
            tiles.push_back(std::move(t));
        }
    }

    std::vector<Rgba> img(size_t(kW) * kH, {16,20,28,255});
    std::vector<Rgba> depth_img(size_t(kW) * kH, {16,20,28,255});
    std::vector<Rgba> uv_img(size_t(kW) * kH, {16,20,28,255});
    std::vector<float> zbuf(size_t(kW) * kH, 1e30f);
    for (const Tile& t : tiles) {
        float min_x = 1e30f, min_y = 1e30f, max_x = -1e30f, max_y = -1e30f;
        for (float zraw : {t.bm.zmin, t.bm.zmax}) {
            const float z = t.p.cam_tile.z + zraw * t.p.cam_tile.w;
            const float sx_min = -t.p.tile_sprite.x * (perspective ? kPerspectiveTileCoverageScale : 1.0f);
            const float sx_max = (t.p.tile_sprite.z - t.p.tile_sprite.x) * (perspective ? kPerspectiveTileCoverageScale : 1.0f);
            const float sy_min = -t.p.tile_sprite.y * (perspective ? kPerspectiveTileCoverageScale : 1.0f);
            const float sy_max = (t.p.tile_sprite.w - t.p.tile_sprite.y) * (perspective ? kPerspectiveTileCoverageScale : 1.0f);
            for (float sx : {sx_min, sx_max}) {
                for (float sy : {sy_min, sy_max}) {
                    const float2 q = project_case(t.p.cam_tile.x + sx, t.p.cam_tile.y + sy, z, focal, perspective);
                    min_x = std::min(min_x, q.x); min_y = std::min(min_y, q.y);
                    max_x = std::max(max_x, q.x); max_y = std::max(max_y, q.y);
                }
            }
        }
        const int x0 = std::clamp(int(std::floor(min_x - 2.0f)), 0, kW - 1);
        const int y0 = std::clamp(int(std::floor(min_y - 2.0f)), 0, kH - 1);
        const int x1 = std::clamp(int(std::ceil (max_x + 2.0f)), 0, kW - 1);
        const int y1 = std::clamp(int(std::ceil (max_y + 2.0f)), 0, kH - 1);
        for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            Rgba c;
            float d = 0.0f;
            float2 hit_uv = {};
            float hit_zraw = 0.0f;
            const bool hit = perspective
                ? tile_fragment_perspective(t, {x + 0.5f, y + 0.5f}, c, d, sample_mode, &hit_uv, &hit_zraw)
                : tile_fragment_orthographic(t, {x + 0.5f, y + 0.5f}, c, d);
            if (!hit) continue;
            const size_t idx = size_t(y) * kW + x;
            if (d >= zbuf[idx]) continue;
            zbuf[idx] = d;
            img[idx] = source_bitmap ? t.debug_color : c;
            if (perspective) {
                const float zt = (t.bm.zmax > t.bm.zmin)
                    ? std::clamp((hit_zraw - t.bm.zmin) / (t.bm.zmax - t.bm.zmin), 0.0f, 1.0f)
                    : 0.5f;
                depth_img[idx] = {uint8_t(255.0f * zt), uint8_t(128.0f + 127.0f * zt),
                                  uint8_t(255.0f * (1.0f - zt)), 255};
                uv_img[idx] = {uint8_t(255.0f * std::clamp(hit_uv.x, 0.0f, 1.0f)),
                               uint8_t(255.0f * std::clamp(hit_uv.y, 0.0f, 1.0f)), 64, 255};
            }
        }
        }
    }

    for (const Tile& t : tiles) {
        const float cx = t.p.cam_tile.x, cy = t.p.cam_tile.y;
        if (std::fabs(cx) > tile_w * 3 || std::fabs(cy) > tile_h * 3) continue;
        const float pts[4][2] = {
            {cx, cy - tile_h * 0.5f}, {cx + tile_w * 0.5f, cy},
            {cx, cy + tile_h * 0.5f}, {cx - tile_w * 0.5f, cy}
        };
        float2 pp[4];
        for (int i = 0; i < 4; ++i) pp[i] = project_case(pts[i][0], pts[i][1], base_z + flat_iso_square_depth(pts[i][0], pts[i][1]), focal, perspective);
        for (int i = 0; i < 4; ++i) line(img, pp[i].x, pp[i].y, pp[(i + 1) & 3].x, pp[(i + 1) & 3].y, {255,255,255,95});
    }

    std::filesystem::create_directories("build/cpu_tile_perspective");
    const std::filesystem::path path = std::filesystem::path("build/cpu_tile_perspective") /
        (std::string("cpu_tile_perspective_cpp_") + suffix + ".png");
    write_png(path, img);
    if (perspective) {
        write_png(std::filesystem::path("build/cpu_tile_perspective") /
                      (std::string("cpu_tile_perspective_cpp_") + suffix + "_depth.png"),
                  depth_img);
        write_png(std::filesystem::path("build/cpu_tile_perspective") /
                      (std::string("cpu_tile_perspective_cpp_") + suffix + "_uv.png"),
                  uv_img);
    }
    std::printf("wrote %s\n", path.string().c_str());
}

float2 tile_surface_project(const Tile& t, float lx, float ly, float focal) {
    const float u = (lx + t.p.tile_sprite.x) / std::max(t.p.tile_sprite.z, 1.0f);
    const float v = (ly + t.p.tile_sprite.y) / std::max(t.p.tile_sprite.w, 1.0f);
    Rgba c;
    float zraw = 0.0f;
    if (!t.bm.sample({u, v}, c, zraw)) return {-1.0f, -1.0f};
    const float cam_x = t.p.cam_tile.x + lx;
    const float cam_y = t.p.cam_tile.y + ly;
    const float cam_z = t.p.cam_tile.z + zraw * t.p.cam_tile.w;
    return project(cam_x, cam_y, cam_z, focal);
}

void render_seam_proof(float focal, const char* suffix, const TileBitmap* source_bitmap = nullptr) {
    const float2 logical = source_bitmap ? logical_tile_dims(*source_bitmap) : float2{160.0f, 80.0f};
    const float tile_w = logical.x;
    const float tile_h = logical.y;
    constexpr float base_z = 1800.0f;

    Tile a;
    a.bm = source_bitmap ? *source_bitmap : make_bitmap(0.0f, 0.0f, {60, 210, 80, 255});
    a.p.tile_sprite = {float(a.bm.w) * 0.5f, float(a.bm.h) * 0.5f, float(a.bm.w), float(a.bm.h)};
    a.p.cam_tile = {0.0f, 0.0f, base_z, 1.0f};

    Tile b;
    b.bm = source_bitmap ? *source_bitmap : make_bitmap(tile_w * 0.5f, tile_h * 0.5f, {230, 60, 60, 255});
    b.p.tile_sprite = {float(b.bm.w) * 0.5f, float(b.bm.h) * 0.5f, float(b.bm.w), float(b.bm.h)};
    b.p.cam_tile = {tile_w * 0.5f, tile_h * 0.5f, base_z, 1.0f};

    std::vector<Rgba> img(size_t(kW) * kH, {16, 20, 28, 255});

    double max_err = 0.0;
    double sum_err = 0.0;
    int samples = 0;

    float2 last_a{-1, -1}, last_b{-1, -1};
    constexpr int kSamples = 128;
    for (int i = 0; i <= kSamples; ++i) {
        const float t = float(i) / float(kSamples);
        // Tile A edge: right -> bottom.
        const float ax = (1.0f - t) * (tile_w * 0.5f) + t * 0.0f;
        const float ay = (1.0f - t) * 0.0f + t * (tile_h * 0.5f);
        // Tile B edge: top -> left. These are the same global camera-space points.
        const float bx = (1.0f - t) * 0.0f + t * (-tile_w * 0.5f);
        const float by = (1.0f - t) * (-tile_h * 0.5f) + t * 0.0f;

        const float2 pa = tile_surface_project(a, ax, ay, focal);
        const float2 pb = tile_surface_project(b, bx, by, focal);
        if (pa.x >= 0.0f && pb.x >= 0.0f) {
            const double err = std::hypot(double(pa.x - pb.x), double(pa.y - pb.y));
            max_err = std::max(max_err, err);
            sum_err += err;
            ++samples;
            if (last_a.x >= 0.0f) line(img, last_a.x, last_a.y, pa.x, pa.y, {0, 255, 0, 255});
            if (last_b.x >= 0.0f) line(img, last_b.x, last_b.y, pb.x, pb.y, {255, 0, 0, 210});
            for (int oy = -2; oy <= 2; ++oy) {
                for (int ox = -2; ox <= 2; ++ox) {
                    blend(img, int(std::lround(pa.x)) + ox, int(std::lround(pa.y)) + oy, {0, 255, 0, 180});
                    blend(img, int(std::lround(pb.x)) + ox, int(std::lround(pb.y)) + oy, {255, 0, 0, 180});
                }
            }
        }
        last_a = pa;
        last_b = pb;
    }

    // Draw the two tile outlines faintly for context.
    for (const Tile* t : {&a, &b}) {
        const float cx = t->p.cam_tile.x, cy = t->p.cam_tile.y;
        const float pts[4][2] = {
            {cx, cy - tile_h * 0.5f}, {cx + tile_w * 0.5f, cy},
            {cx, cy + tile_h * 0.5f}, {cx - tile_w * 0.5f, cy}
        };
        float2 pp[4];
        for (int j = 0; j < 4; ++j) pp[j] = project(pts[j][0], pts[j][1], base_z + flat_iso_square_depth(pts[j][0], pts[j][1]), focal);
        for (int j = 0; j < 4; ++j) line(img, pp[j].x, pp[j].y, pp[(j + 1) & 3].x, pp[(j + 1) & 3].y, {255, 255, 255, 80});
    }

    std::filesystem::create_directories("build/cpu_tile_perspective");
    const std::filesystem::path path = std::filesystem::path("build/cpu_tile_perspective") /
        (std::string("cpu_tile_seam_proof_") + suffix + ".png");
    write_png(path, img);

    const std::filesystem::path report_path = std::filesystem::path("build/cpu_tile_perspective") /
        (std::string("cpu_tile_seam_proof_") + suffix + ".txt");
    std::ofstream report(report_path);
    report << "shared edge seam proof\n";
    report << "green tile A right-bottom edge\n";
    report << "red tile B top-left edge\n";
    report << "samples " << samples << "\n";
    report << "mean_screen_error_px " << (samples ? (sum_err / samples) : 0.0) << "\n";
    report << "max_screen_error_px " << max_err << "\n";
    std::printf("wrote %s\n", path.string().c_str());
}

Tile make_tile_for_center(float cx, float cy, float base_z, float focal, const TileBitmap* source_bitmap, Rgba synthetic_color) {
    Tile t;
    t.bm = source_bitmap ? *source_bitmap : make_bitmap(cx, cy, synthetic_color);
    const float bw = float(t.bm.w);
    const float bh = float(t.bm.h);
    t.p.zparams = {0,0,kPerspectiveTileCoverageScale,1};
    t.p.tile_sprite = {bw * 0.5f, bh * 0.5f, bw, bh};
    t.p.filter = {1.5f, 64.0f, t.bm.zmin, t.bm.zmax};
    t.p.camera = {0,0,focal,1};
    t.p.proj = {kW * 0.5f, kH * 0.5f, kW, kH};
    t.p.tile_rect = {0,0,bw,bh};
    t.p.cam_tile = {cx, cy, base_z + flat_iso_square_depth(cx, cy), 1.0f};
    return t;
}

void render_two_tile_fill_proof(float focal, const char* suffix, const TileBitmap* source_bitmap = nullptr) {
    const float2 logical = source_bitmap ? logical_tile_dims(*source_bitmap) : float2{160.0f, 80.0f};
    const float tile_w = logical.x;
    const float tile_h = logical.y;
    constexpr float base_z = 1800.0f;
    std::vector<Tile> tiles;
    tiles.push_back(make_tile_for_center(0.0f, 0.0f, base_z, focal, source_bitmap, {40, 220, 60, 255}));
    tiles.push_back(make_tile_for_center(tile_w * 0.5f, tile_h * 0.5f, base_z, focal, source_bitmap, {230, 50, 45, 255}));

    std::vector<Rgba> img(size_t(kW) * kH, {16, 20, 28, 255});
    std::vector<float> zbuf(size_t(kW) * kH, 1e30f);
    for (int ti = 0; ti < int(tiles.size()); ++ti) {
        const Tile& t = tiles[ti];
        float min_x = 1e30f, min_y = 1e30f, max_x = -1e30f, max_y = -1e30f;
        for (float zraw : {t.bm.zmin, t.bm.zmax}) {
            const float z = t.p.cam_tile.z + zraw * t.p.cam_tile.w;
            const float sx_min = -t.p.tile_sprite.x * kPerspectiveTileCoverageScale;
            const float sx_max = (t.p.tile_sprite.z - t.p.tile_sprite.x) * kPerspectiveTileCoverageScale;
            const float sy_min = -t.p.tile_sprite.y * kPerspectiveTileCoverageScale;
            const float sy_max = (t.p.tile_sprite.w - t.p.tile_sprite.y) * kPerspectiveTileCoverageScale;
            for (float sx : {sx_min, sx_max}) {
                for (float sy : {sy_min, sy_max}) {
                    const float2 q = project(t.p.cam_tile.x + sx, t.p.cam_tile.y + sy, z, focal);
                    min_x = std::min(min_x, q.x); min_y = std::min(min_y, q.y);
                    max_x = std::max(max_x, q.x); max_y = std::max(max_y, q.y);
                }
            }
        }
        const int x0 = std::clamp(int(std::floor(min_x - 2.0f)), 0, kW - 1);
        const int y0 = std::clamp(int(std::floor(min_y - 2.0f)), 0, kH - 1);
        const int x1 = std::clamp(int(std::ceil (max_x + 2.0f)), 0, kW - 1);
        const int y1 = std::clamp(int(std::ceil (max_y + 2.0f)), 0, kH - 1);
        for (int y = y0; y <= y1; ++y) {
            for (int x = x0; x <= x1; ++x) {
                Rgba c;
                float d = 0.0f;
                if (!tile_fragment_perspective(t, {x + 0.5f, y + 0.5f}, c, d)) continue;
                const size_t idx = size_t(y) * kW + x;
                if (d >= zbuf[idx]) continue;
                zbuf[idx] = d;
                img[idx] = (ti == 0) ? Rgba{40, 230, 50, 255} : Rgba{240, 45, 45, 255};
            }
        }
    }

    const std::filesystem::path path = std::filesystem::path("build/cpu_tile_perspective") /
        (std::string("cpu_tile_seam_fill_") + suffix + ".png");
    write_png(path, img);
    std::printf("wrote %s\n", path.string().c_str());
}

void render_single_tile_fringe_probe(float focal, const char* suffix, const TileBitmap& bm) {
    constexpr float base_z = 1800.0f;
    Tile t = make_tile_for_center(-260.0f, -130.0f, base_z, focal, &bm, {40, 220, 60, 255});
    std::vector<Rgba> img(size_t(kW) * kH, {16, 20, 28, 255});
    std::vector<Rgba> fringe(size_t(kW) * kH, {16, 20, 28, 255});
    std::vector<Rgba> depth_img(size_t(kW) * kH, {16, 20, 28, 255});
    std::vector<Rgba> uv_img(size_t(kW) * kH, {16, 20, 28, 255});

    float min_x = 1e30f, min_y = 1e30f, max_x = -1e30f, max_y = -1e30f;
    for (float zraw : {t.bm.zmin, t.bm.zmax}) {
        const float z = t.p.cam_tile.z + zraw * t.p.cam_tile.w;
        const float sx_min = -t.p.tile_sprite.x * kPerspectiveTileCoverageScale;
        const float sx_max = (t.p.tile_sprite.z - t.p.tile_sprite.x) * kPerspectiveTileCoverageScale;
        const float sy_min = -t.p.tile_sprite.y * kPerspectiveTileCoverageScale;
        const float sy_max = (t.p.tile_sprite.w - t.p.tile_sprite.y) * kPerspectiveTileCoverageScale;
        for (float sx : {sx_min, sx_max}) {
            for (float sy : {sy_min, sy_max}) {
                const float2 q = project(t.p.cam_tile.x + sx, t.p.cam_tile.y + sy, z, focal);
                min_x = std::min(min_x, q.x); min_y = std::min(min_y, q.y);
                max_x = std::max(max_x, q.x); max_y = std::max(max_y, q.y);
            }
        }
    }
    const int x0 = std::clamp(int(std::floor(min_x - 2.0f)), 0, kW - 1);
    const int y0 = std::clamp(int(std::floor(min_y - 2.0f)), 0, kH - 1);
    const int x1 = std::clamp(int(std::ceil (max_x + 2.0f)), 0, kW - 1);
    const int y1 = std::clamp(int(std::ceil (max_y + 2.0f)), 0, kH - 1);

    int rendered = 0;
    int outside_ortho = 0;
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            Rgba c;
            float d = 0.0f;
            float2 hit_uv = {};
            float hit_zraw = 0.0f;
            if (!tile_fragment_perspective(t, {x + 0.5f, y + 0.5f}, c, d, {}, &hit_uv, &hit_zraw)) continue;
            ++rendered;
            img[size_t(y) * kW + x] = c;
            const float zt = (bm.zmax > bm.zmin) ? std::clamp((hit_zraw - bm.zmin) / (bm.zmax - bm.zmin), 0.0f, 1.0f) : 0.5f;
            depth_img[size_t(y) * kW + x] = {
                uint8_t(255.0f * zt),
                uint8_t(128.0f + 127.0f * zt),
                uint8_t(255.0f * (1.0f - zt)),
                255
            };
            uv_img[size_t(y) * kW + x] = {
                uint8_t(255.0f * std::clamp(hit_uv.x, 0.0f, 1.0f)),
                uint8_t(255.0f * std::clamp(hit_uv.y, 0.0f, 1.0f)),
                64,
                255
            };
            const float ortho_lx = (x + 0.5f) - kW * 0.5f - t.p.cam_tile.x;
            const float ortho_ly = (y + 0.5f) - kH * 0.5f - t.p.cam_tile.y;
            const bool in_ortho_diamond =
                std::fabs(ortho_lx) / (float(bm.w) * 0.5f) +
                std::fabs(ortho_ly) / (float(bm.h) * 0.5f) <= 1.0f;
            if (!in_ortho_diamond) {
                ++outside_ortho;
                fringe[size_t(y) * kW + x] = {255, 0, 0, 255};
            } else {
                fringe[size_t(y) * kW + x] = c;
            }
        }
    }

    std::filesystem::create_directories("build/cpu_tile_perspective");
    const std::string stem = std::string("single_tile_fringe_") + suffix;
    write_png(std::filesystem::path("build/cpu_tile_perspective") / (stem + ".png"), img);
    write_png(std::filesystem::path("build/cpu_tile_perspective") / (stem + "_marked.png"), fringe);
    write_png(std::filesystem::path("build/cpu_tile_perspective") / (stem + "_depth.png"), depth_img);
    write_png(std::filesystem::path("build/cpu_tile_perspective") / (stem + "_uv.png"), uv_img);
    std::ofstream report(std::filesystem::path("build/cpu_tile_perspective") / (stem + ".txt"));
    report << "single tile fringe probe " << suffix << "\n";
    report << "rendered_pixels " << rendered << "\n";
    report << "outside_ortho_diamond_pixels " << outside_ortho << "\n";
    report << "red pixels in marked image are rendered outside the ortho diamond footprint\n";
    std::printf("wrote build/cpu_tile_perspective/%s.png\n", stem.c_str());
}

} // namespace

int main() {
    write_bitmap_diagnostics();
    render(1.0f, false, "ortho");
    render(3200.0f, true, "small_fov");
    render(1800.0f, true, "medium_fov");
    render(900.0f, true, "large_fov");
    render_seam_proof(900.0f, "synthetic_large_fov");
    render_two_tile_fill_proof(900.0f, "synthetic_large_fov");
    TileBitmap real_ocean;
    if (load_ztile("build/real_tile_diagnostics_fit/1015_Foroooo.ztile", real_ocean) ||
        load_ztile("build/real_tile_diagnostics/1015_Foroooo.ztile", real_ocean)) {
        render(1800.0f, true, "real_Foroooo_medium_fov", &real_ocean);
        render(900.0f, true, "real_Foroooo_large_fov", &real_ocean);
        render_seam_proof(900.0f, "real_Foroooo_large_fov", &real_ocean);
        render_two_tile_fill_proof(900.0f, "real_Foroooo_large_fov", &real_ocean);
    } else {
        std::printf("real Foroooo ztile not found; run ./build/Revenant --dumptiles=build/real_tile_diagnostics_fit\n");
    }
    TileBitmap grass;
    if (load_ztile("build/real_tile_diagnostics_fit/0996_Forgggg.ztile", grass) ||
        load_ztile("build/real_tile_diagnostics/0996_Forgggg.ztile", grass)) {
        write_tile_analysis(grass, "0996_Forgggg");
        write_tile_validity_diagnostics(grass, "0996_Forgggg");
        render_single_tile_fringe_probe(900.0f, "0996_Forgggg_large_fov", grass);
        render_single_tile_fringe_probe(450.0f, "0996_Forgggg_extreme_fov", grass);
        render(focal_from_fov(20.0f), true, "real_Forgggg_fov20_near", &grass, 900.0f);
        render(focal_from_fov(20.0f), true, "real_Forgggg_fov20_linear_depth", &grass, 900.0f,
               SampleMode{false, true});
        render(focal_from_fov(20.0f), true, "real_Forgggg_fov20_linear_color", &grass, 900.0f,
               SampleMode{true, false});
        render(focal_from_fov(20.0f), true, "real_Forgggg_fov20_linear_rgb_depth_validity", &grass, 900.0f,
               SampleMode{true, false});
        render(focal_from_fov(20.0f), true, "real_Forgggg_fov20_linear_both", &grass, 900.0f,
               SampleMode{true, true});
    } else {
        std::printf("forest ground transition ztile not found; run ./build/Revenant --dumptiles=build/real_tile_diagnostics_fit\n");
    }
    return 0;
}
