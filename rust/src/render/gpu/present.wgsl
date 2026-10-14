// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   present.wgsl - Cropped GPU surface presentation     *
// *************************************************************************
override SURFACE_SRGB: bool = false;
struct Crop {origin_size:vec4u};
@group(0) @binding(0) var image:texture_2d<f32>;
@group(0) @binding(1) var<uniform> crop:Crop;
struct Quad {@builtin(position) position:vec4f,@location(0) uv:vec2f};
@vertex fn vs(@builtin(vertex_index) id:u32)->Quad {
    let uv=vec2f(f32((id<<1u)&2u),f32(id&2u));
    return Quad(vec4f(uv.x*2.0-1.0,1.0-uv.y*2.0,0.0,1.0),uv);
}
@fragment fn fs(q:Quad)->@location(0) vec4f {
    let p=crop.origin_size.xy+min(vec2u(q.uv*vec2f(crop.origin_size.zw)),crop.origin_size.zw-1u);
    var color=textureLoad(image,vec2i(p),0).rgb;
    // The offscreen target stores display-encoded retail colors. An sRGB
    // swapchain will encode again, so undo that transfer before writing.
    if SURFACE_SRGB {
        color=select(pow((color+0.055)/1.055,vec3f(2.4)),color/12.92,color<=vec3f(0.04045));
    }
    return vec4f(color,1.0);
}
