// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   post.wgsl - AO, shadows and deferred light          *
// *************************************************************************
@group(1) @binding(0) var albedo:texture_2d<f32>;
@group(1) @binding(1) var normals:texture_2d<f32>;
@group(1) @binding(2) var scene_depth:texture_2d<f32>;
@group(1) @binding(3) var occlusion:texture_2d<f32>;
@group(1) @binding(4) var visibility:texture_2d<f32>;
struct Quad {@builtin(position) position:vec4f,@location(0) uv:vec2f};
@vertex fn vs(@builtin(vertex_index) id:u32)->Quad {
    let uv=vec2f(f32((id<<1u)&2u),f32(id&2u));
    return Quad(vec4f(uv.x*2.0-1.0,1.0-uv.y*2.0,0.0,1.0),uv);
}
fn load(t:texture_2d<f32>,uv:vec2f)->vec4f {
    let dims=vec2i(textureDimensions(t));return textureLoad(t,clamp(vec2i(uv*vec2f(dims)),vec2i(0),dims-1),0);
}
fn empty(uv:vec2f)->bool {return load(albedo,uv).a<0.001;}
@fragment fn ao(q:Quad)->@location(0) f32 {
    if empty(q.uv) || frame.ao.y<=0.0 {return 1.0;}
    let w=world_at(q.uv*frame.viewport.xy,load(scene_depth,q.uv).r);
    let n=safe_normal(load(normals,q.uv).xyz*2.0-1.0);
    let h=fract(sin(dot(q.uv,vec2f(12.9898,78.233)))*43758.5453);
    var occ=0.0;var weight=0.0;
    for(var i=0;i<8;i++) {
        let a=h*6.2831853+f32(i)*(6.2831853/8.0);
        let ring=max(frame.ao.x,1.0)*(0.35+0.65*fract(h+f32(i)*0.6180339));
        let uv=q.uv+vec2f(cos(a),sin(a))*ring/frame.viewport.xy;
        if any(uv<vec2f(0.0)) || any(uv>=vec2f(1.0)) || empty(uv) {continue;}
        let delta=world_at(uv*frame.viewport.xy,load(scene_depth,uv).r)-w;
        let dist=length(delta);if dist<0.0001 || dist>frame.ao.w {continue;}
        let hemi=pow(max(dot(n,delta/dist)-frame.ao.z,0.0),1.35);
        let attn=pow(1.0-dist/frame.ao.w,2.0);occ+=hemi*attn;weight+=attn;
    }
    if weight<=0.00001 {return 1.0;}
    return clamp(1.0-frame.ao.y*pow(clamp(occ/weight,0.0,1.0),0.75),0.0,1.0);
}
@fragment fn shadow(q:Quad)->@location(0) f32 {
    if empty(q.uv) || frame.settings.z<0.5 {return 1.0;}
    let w=world_at(q.uv*frame.viewport.xy,load(scene_depth,q.uv).r);
    let end=w+safe_normal(frame.sun.xyz)*frame.shadow.x*frame.shadow.z;
    let end_uv=project(end)/frame.viewport.xy;
    let d0=world_depth(w);let d1=world_depth(end);
    for(var i=1u;i<=min(u32(frame.shadow.z),512u);i++) {
        let a=(f32(i)-0.5)/frame.shadow.z;let uv=mix(q.uv,end_uv,a);
        if any(uv<vec2f(0.0)) || any(uv>=vec2f(1.0)) {break;}
        if empty(uv) {continue;}
        var ray_depth=mix(d0,d1,a);
        if frame.viewport.w>0.0 {
            let offset=frame.viewport.w-camera_depth();
            ray_depth=1.0/mix(1.0/max(d0+offset,1.0),1.0/max(d1+offset,1.0),a)-offset;
        }
        let delta=load(scene_depth,uv).r-ray_depth;
        if delta>=-frame.shadow.w && delta<=frame.shadow_extra.x {return 0.0;}
    }
    return 1.0;
}
fn blur(uv:vec2f,axis:vec2f)->f32 {
    let step=axis*frame.shadow.y/vec2f(textureDimensions(visibility));
    var value=0.38774*load(visibility,uv).r;
    value+=0.24477*(load(visibility,uv+step).r+load(visibility,uv-step).r);
    value+=0.06136*(load(visibility,uv+step*2.0).r+load(visibility,uv-step*2.0).r);
    return clamp(value,0.0,1.0);
}
@fragment fn blur_x(q:Quad)->@location(0) f32 {return blur(q.uv,vec2f(1.0,0.0));}
@fragment fn blur_y(q:Quad)->@location(0) f32 {return blur(q.uv,vec2f(0.0,1.0));}
@fragment fn light(q:Quad)->@location(0) vec4f {
    let color=load(albedo,q.uv);
    if color.a<0.001 {return vec4f(17.0/255.0,22.0/255.0,27.0/255.0,1.0);}
    let depth=load(scene_depth,q.uv).r;
    let normal=load(normals,q.uv).xyz;
    let n=safe_normal(normal*2.0-1.0);
    let w=world_at(q.uv*frame.viewport.xy,depth);
    let ao=load(occlusion,q.uv).r;let shadow=load(visibility,q.uv).r;
    let view=u32(frame.settings.x);
    if view==1u {return vec4f(color.rgb,1.0);}
    if view==2u {let a=fract(depth/1024.0);return vec4f(fract(depth/128.0),a,1.0-a,1.0);}
    if view==3u {return vec4f(normal,1.0);}
    if view==4u {return vec4f(point_light(w,n),1.0);}
    if view==5u {return vec4f(fract(w/128.0),1.0);}
    if view==6u {return vec4f(vec3f(shadow),1.0);}
    if view==7u {return vec4f(vec3f(ao),1.0);}
    if view==8u {let e=clamp((abs(dpdx(depth))+abs(dpdy(depth)))/48.0,0.0,1.0);return vec4f(e,e*0.05,0.0,1.0);}
    if view==9u {let h=clamp((w.z+256.0)/2048.0,0.0,1.0);let band=select(0.0,1.0,fract(abs(w.z)/128.0)<0.04);return vec4f(max(h,band),h*(1.0-0.5*band),1.0-h,1.0);}
    if load(normals,q.uv).a>1.5 {return vec4f(color.rgb,1.0);}
    return vec4f(color.rgb*shade(w,n,ao,shadow),1.0);
}
