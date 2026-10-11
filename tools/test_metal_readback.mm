// Actual Metal/Sokol render-to-readback ordering, including queued async reads.
#define SOKOL_METAL
#define SOKOL_IMPL
#include "sokol_gfx.h"
#undef SOKOL_IMPL
#undef SOKOL_GFX_IMPL
#include "../src/renderer_readback.h"
#import <Metal/Metal.h>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <thread>
#include <chrono>
#include <atomic>
#include <cstdlib>

static const void* NoSurface() { return nullptr; }

int main() {
    std::atomic<bool> finished{false};
    std::thread watchdog([&] {
        for (int i=0;i<600 && !finished.load();++i)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (!finished.load()) { std::fputs("Metal readback test timed out\n",stderr); std::abort(); }
    });
    @autoreleasepool {
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        assert(device);
        sg_desc desc{};
        desc.context.color_format = SG_PIXELFORMAT_RGBA8;
        desc.context.depth_format = SG_PIXELFORMAT_NONE;
        desc.context.sample_count = 1;
        desc.context.metal.device = (__bridge const void*)device;
        desc.context.metal.renderpass_descriptor_cb = NoSurface;
        desc.context.metal.drawable_cb = NoSurface;
        sg_setup(&desc);
        id<MTLCommandQueue> queue = (__bridge id<MTLCommandQueue>)sg_mtl_query_command_queue();
        assert(queue && queue.device == device && !sg_mtl_has_pending_commands());
        sg_image_desc image_desc{};
        image_desc.render_target = true;
        image_desc.width = image_desc.height = 8;
        image_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
        sg_image image = sg_make_image(&image_desc);
        sg_pass_desc pass_desc{};
        pass_desc.color_attachments[0].image = image;
        sg_pass pass = sg_make_pass(&pass_desc);
        assert(sg_query_pass_state(pass) == SG_RESOURCESTATE_VALID);
        id<MTLSharedEvent> gate = [device newSharedEvent];
        assert(gate);
        constexpr int Frames = 256;
        for (int frame = 1; frame <= Frames; ++frame) {
            // Hold the render queue so an unrelated read queue would see old
            // pixels. The producer and copy must remain ordered at every frame.
            id<MTLCommandBuffer> blocker = [queue commandBuffer];
            [blocker encodeWaitForEvent:gate value:uint64_t(frame)];
            [blocker commit];
            uint8_t expected[4] = {uint8_t(frame), uint8_t(frame * 73), uint8_t(frame * 151), 255};
            sg_pass_action action{};
            action.colors[0].action = SG_ACTION_CLEAR;
            action.colors[0].value = {expected[0]/255.f, expected[1]/255.f, expected[2]/255.f, 1.f};
            sg_begin_pass(pass, &action);
            sg_end_pass();
            assert(sg_mtl_has_pending_commands());
            uint8_t pixel[4];
            assert(!RendererReadback::ReadPixel(image, 0, 0, pixel));
            assert(!RendererReadback::ReadRect(image, 0, 0, 1, 1, pixel));
            int slot = frame % RendererReadback::kAsyncReadSlots;
            assert(RendererReadback::BeginReadRect(slot, image, 0, 0, 1, 1));
            assert(!RendererReadback::BeginReadRect(slot, image, 0, 0, 1, 1));
            assert(!RendererReadback::PollReadRect(slot, pixel, 4));
            sg_commit();
            assert(!sg_mtl_has_pending_commands());
            // Delay release even after all commands have been submitted.
            std::thread release([gate, frame] {
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
                gate.signaledValue = uint64_t(frame);
            });
            assert(RendererReadback::ReadPixel(image, 0, 0, pixel));
            assert(std::memcmp(pixel, expected, 4) == 0);
            uint8_t rectangle[8 * 8 * 4];
            assert(RendererReadback::ReadRect(image, 0, 0, 8, 8, rectangle));
            for (int i = 0; i < 64; ++i) assert(std::memcmp(rectangle + i * 4, expected, 4) == 0);
            release.join();
            assert(!RendererReadback::PollReadRect(slot, pixel, 3));
            assert(RendererReadback::PollReadRect(slot, pixel, 4));
            assert(std::memcmp(pixel, expected, 4) == 0);
            assert(!RendererReadback::PollReadRect(slot, pixel, 4));
        }
        assert(!RendererReadback::ReadRect(image, 8, 0, 1, 1, nullptr));
        sg_destroy_pass(pass);
        sg_destroy_image(image);
        // Swapchain-style BGRA remains exposed as RGBA through every API.
        image_desc.pixel_format = SG_PIXELFORMAT_BGRA8;
        image = sg_make_image(&image_desc);
        pass_desc.color_attachments[0].image = image;
        pass = sg_make_pass(&pass_desc);
        sg_pass_action bgra{};
        bgra.colors[0].action = SG_ACTION_CLEAR;
        bgra.colors[0].value = {17/255.f, 93/255.f, 211/255.f, 1.f};
        sg_begin_pass(pass, &bgra); sg_end_pass(); sg_commit();
        const uint8_t rgba[4] = {17,93,211,255};
        uint8_t pixel[4];
        assert(RendererReadback::ReadPixel(image, 0, 0, pixel));
        assert(std::memcmp(pixel, rgba, 4) == 0);
        assert(RendererReadback::ReadRect(image, 0, 0, 1, 1, pixel));
        assert(std::memcmp(pixel, rgba, 4) == 0);
        assert(RendererReadback::BeginReadRect(0, image, 0, 0, 1, 1));
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
        while (!RendererReadback::PollReadRect(0, pixel, 4)) {
            assert(std::chrono::steady_clock::now() < deadline);
            std::this_thread::yield();
        }
        assert(std::memcmp(pixel, rgba, 4) == 0);
        sg_destroy_pass(pass); sg_destroy_image(image);
        sg_shutdown();
        assert(!sg_mtl_query_command_queue());
        std::printf("PASS %d actual GPU frames: delayed render, pending-frame rejection, sync pixels/rects and async queue ordering\n", Frames);
    }
    finished.store(true);
    watchdog.join();
}
