// *************************************************************************
// *                         Cinematix Revenant                            *
// *                      Reveneant Revisited 2026                         *
// *             test_render_metadata.cpp - Render Metadata Tests           *
// *************************************************************************

#include "../src/render_metadata.h"

#include <cassert>
#include <filesystem>
#include <iostream>

int main()
{
    const std::filesystem::path path =
        std::filesystem::current_path() / ".." / "data" / "Resources" / "render_metadata.def";

    SRenderMetadata meta;
    std::string error;
    if (!LoadRenderMetadataFile(path.string().c_str(), meta, &error))
    {
        std::cerr << "LoadRenderMetadataFile failed: " << error << "\n";
        return 1;
    }

    assert(meta.effects.size() >= 14);
    assert(meta.helpers.size() >= 2);

    const SRenderPolicy* water = meta.FindEffect("Water");
    assert(water);
    assert(water->drawable == ERenderMetaDrawable::WaterParticles);
    assert(water->pass == ERenderMetaPass::TransparentWorld);
    assert(water->blend == ERenderMetaBlend::Additive);
    assert(water->lighting == ERenderMetaLighting::VertexLit);
    assert(water->ztest == true);
    assert(water->zwrite == false);
    assert(water->texture_anim == ERenderMetaTextureAnim::FrameByInstance);

    const SRenderPolicy* wave = meta.FindEffect("WaveM");
    assert(wave);
    assert(wave->drawable == ERenderMetaDrawable::Mesh);
    assert(wave->blend == ERenderMetaBlend::Alpha);
    assert(wave->lighting == ERenderMetaLighting::Fullbright);

    const SRenderObjectPolicy* box = meta.FindHelper("Box");
    assert(box);
    assert(box->submeshes.size() == 2);
    assert(box->submeshes[0].name == "hedra01");
    assert(box->submeshes[1].name == "rectangle0");
    assert(box->submeshes[1].lighting == ERenderMetaLighting::UnlitShadow);

    std::cout << "render metadata tests passed\n";
    return 0;
}
