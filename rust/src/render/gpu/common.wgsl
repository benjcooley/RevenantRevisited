// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  common.wgsl - Camera and lighting data               *
// *************************************************************************
struct Frame {
    viewport: vec4f,
    camera: vec4f,
    ambient: vec4f,
    sun: vec4f,
    sun_color: vec4f,
    ao: vec4f,
    shadow: vec4f,
    normals: vec4f,
    settings: vec4f,
    shadow_extra: vec4f,
    retail_ambient: vec4f,
    retail_directional: vec4f,
    light_pos: array<vec4f,16>,
    light_color: array<vec4f,16>,
};
@group(0) @binding(0) var<uniform> frame: Frame;
const ISO = 0.867;
const DENOM = 2.003378;
fn camera_depth() -> f32 { return -2.0 * frame.camera.y * ISO; }
fn world_at(p:vec2f, d:f32)->vec3f {
    var st=frame.camera.xy+(p-frame.viewport.xy*0.5)/frame.viewport.z;
    if frame.viewport.w>0.0 {st=frame.camera.xy+(st-frame.camera.xy)*(frame.viewport.w+d-camera_depth())/frame.viewport.w;}
    let z=(-d-2.0*st.y*ISO)/DENOM;
    let sum=2.0*(st.y+z*ISO);
    return vec3f((sum+st.x)*0.5,(sum-st.x)*0.5,z);
}
fn world_depth(w:vec3f)->f32 {return -(w.x+w.y)*ISO-w.z*0.5;}
fn project(w:vec3f)->vec2f {
    var scale=frame.viewport.z;
    if frame.viewport.w>0.0 {scale*=frame.viewport.w/max(frame.viewport.w+world_depth(w)-camera_depth(),1.0);}
    return (vec2f(w.x-w.y,(w.x+w.y)*0.5-w.z*ISO)-frame.camera.xy)*scale+frame.viewport.xy*0.5;
}
fn safe_normal(n:vec3f)->vec3f {return n/max(length(n),0.00001);}
fn point_light(w:vec3f,n:vec3f)->vec3f {
    var color=vec3f(0.0);
    for(var i=0u;i<min(u32(frame.settings.y),16u);i++) {
        let p=frame.light_pos[i];let delta=p.xyz-w;let d=length(delta);
        if d<p.w && p.w>0.0 {
            let attn=clamp((pow(d*254.0/p.w+1.0,-1.1)-0.0022436)*50.0/(1.0-0.0022436),0.0,1.0);
            let term=mix(1.0,max(dot(n,delta/max(d,0.00001)),0.0),select(0.0,frame.normals.z,frame.settings.w>0.5));
            color+=frame.light_color[i].rgb*frame.light_color[i].w*attn*term;
        }
    }
    return color;
}
fn shade(w:vec3f,n:vec3f,ao:f32,shadow:f32)->vec3f {
    if frame.sun_color.w<0.5 {return vec3f(1.0);}
    var light=frame.ambient.rgb;
    if frame.settings.w>0.5 {
        let ndl=dot(n,safe_normal(frame.sun.xyz));
        light*=ao;
        light+=frame.sun_color.rgb*frame.sun.w*mix(1.0,max(ndl,0.0),frame.normals.z)*shadow*select(0.0,1.0,ndl>0.0);
    }
    return min(light+point_light(w,n),vec3f(frame.normals.w));
}
