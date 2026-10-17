// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 geometry.wgsl - Depth tiles and rigid meshes          *
// *************************************************************************
@group(1) @binding(0) var colors:texture_2d<f32>;
@group(1) @binding(1) var depths:texture_2d<f32>;
struct Vertex {
    @location(0) pos:vec4f,@location(1) normal:vec4f,@location(2) uv:vec4f,
    @location(3) rect:vec4f,@location(4) tint:vec4f,@location(5) tile:vec4f,
    @location(6) extra:vec4f,@location(7) info:vec4u,
};
struct Varying {
    @builtin(position) position:vec4f,
    @location(0) p:vec3f,@location(1) normal:vec3f,@location(2) uv:vec2f,
    @location(3) @interpolate(flat) rect:vec4f,@location(4) tint:vec4f,
    @location(5) @interpolate(flat) tile:vec4f,
    @location(6) @interpolate(flat) extra:vec4f,
    @location(7) @interpolate(flat) info:vec4u,
    @location(8) @interpolate(flat) ambient:vec4f,
    @location(9) @interpolate(flat) specular:vec4f,
    @location(10) @interpolate(flat) emissive:vec4f,
};
fn vertex_output(v:Vertex)->Varying {
    var out:Varying;
    var xy=(v.pos.xy-frame.camera.xy)*frame.viewport.z;
    var w=1.0;
    if frame.viewport.w>0.0 {
        if v.info.y==1u {xy=v.pos.xy-frame.viewport.xy*0.5;}
        else {w=max((frame.viewport.w+v.pos.z-camera_depth())/frame.viewport.w,0.00001);}
    }
    if v.info.y==3u {xy=v.pos.xy-frame.viewport.xy*0.5;w=1.0;}
    out.position=vec4f(xy.x*2.0/frame.viewport.x,-xy.y*2.0/frame.viewport.y,0.5*w,w);
    out.p=v.pos.xyz;out.normal=v.normal.xyz;out.uv=v.uv.xy;
    out.rect=v.rect;out.tint=v.tint;out.tile=v.tile;out.extra=v.extra;out.info=v.info;
    return out;
}
@vertex fn vs(v:Vertex)->Varying {return vertex_output(v);}
struct MeshVertex {
    @location(0) pos:vec3f,@location(1) normal:vec3f,@location(2) uv:vec2f,
    @location(3) w0:vec4f,@location(4) w1:vec4f,@location(5) w2:vec4f,
    @location(6) n0:vec4f,@location(7) n1:vec4f,@location(8) n2:vec4f,
    @location(9) rect:vec4f,@location(10) tint:vec4f,@location(11) info:vec4u,@location(12) offset:vec4f,
    @location(13) ambient:vec4f,@location(14) specular:vec4f,@location(15) emissive:vec4f,
};
@vertex fn vs_mesh(v:MeshVertex)->Varying {
    let ph=vec4f(v.pos,1.0);let p=vec3f(dot(v.w0,ph),dot(v.w1,ph),dot(v.w2,ph));
    let n=safe_normal(vec3f(dot(v.n0.xyz,v.normal),dot(v.n1.xyz,v.normal),dot(v.n2.xyz,v.normal)));
    var out:Vertex;out.pos=vec4f(p.x-p.y,(p.x+p.y)*0.5-p.z*ISO,world_depth(p),1.0);
    out.normal=vec4f(n,0.0);out.uv=vec4f(v.uv+v.offset.xy,0.0,0.0);out.rect=v.rect;out.tint=v.tint;out.info=v.info;
    out.info.z=0u;
    if frame.settings.w<0.5 && frame.sun_color.w>0.5 && v.info.w!=2u {
        out.info.z=v.info.z;
        if v.info.z==1u {
            let source_light=frame.retail_ambient.rgb+frame.retail_directional.rgb*max(dot(n,vec3f(0.0,0.78125,0.625)),0.0);
            out.tint=vec4f(out.tint.rgb*floor(clamp(source_light,vec3f(0.0),vec3f(1.0))*31.0)/31.0,out.tint.a);
        }
    }
    out.extra.z=v.offset.z;
    var result=vertex_output(out);
    result.ambient=v.ambient;result.specular=v.specular;result.emissive=v.emissive;
    return result;
}
fn pixel(v:Varying,uv:vec2f)->vec2i {
    var p=clamp(floor(uv),vec2f(0.0),v.rect.zw-1.0);
    if v.info.y==1u && v.info.z==1u {p.x=v.rect.z-1.0-p.x;}
    return vec2i(v.rect.xy+p);
}
fn tile_depth(v:Varying,uv:vec2f)->f32 {return textureLoad(depths,pixel(v,uv),0).r;}
fn tile_world(v:Varying,uv:vec2f,d:f32)->vec3f {
    let st=v.tile.xy+uv;let z=(-d-2.0*st.y*ISO)/DENOM;let sum=2.0*(st.y+z*ISO);
    return vec3f((sum+st.x)*0.5,(sum-st.x)*0.5,z);
}
struct Surface {color:vec4f,normal:vec3f,depth:f32};
fn surface(v:Varying)->Surface {
    var uv=fract(v.uv)*v.rect.zw;
    var d=v.p.z;var n=safe_normal(v.normal);
    if v.info.y==1u {
        uv=v.uv;
        if frame.viewport.w>0.0 {
            let ray=(v.position.xy-frame.viewport.xy*0.5)/(frame.viewport.z*frame.viewport.w);
            let near=max(v.tile.w,camera_depth()-frame.viewport.w+1.0);
            let far=v.extra.x;
            var hit=false;var previous=near;
            for(var i=0u;i<=128u;i++) {
                let candidate=mix(near,far,f32(i)/128.0);
                let p=frame.camera.xy+ray*(frame.viewport.w+candidate-camera_depth())-v.tile.xy;
                if all(p>=vec2f(0.0)) && all(p<v.rect.zw) {
                    let actual=v.tile.z+tile_depth(v,p);
                    let alpha=textureLoad(colors,pixel(v,p),0).a;
                    if candidate>=actual && alpha>0.0 {
                        var lo=previous;var hi=candidate;
                        for(var j=0;j<7;j++) {
                            let mid=(lo+hi)*0.5;
                            let m=frame.camera.xy+ray*(frame.viewport.w+mid-camera_depth())-v.tile.xy;
                            let md=v.tile.z+tile_depth(v,m);
                            if mid>=md {hi=mid;}else{lo=mid;}
                        }
                        d=hi;uv=frame.camera.xy+ray*(frame.viewport.w+d-camera_depth())-v.tile.xy;
                        hit=true;break;
                    }
                }
                previous=candidate;
            }
            if !hit {discard;}
        }
        d=v.tile.z+tile_depth(v,uv);
        let r=max(frame.normals.x,0.5);
        let center=d;var dz:array<f32,4>;
        dz[0]=v.tile.z+tile_depth(v,uv-vec2f(r,0));dz[1]=v.tile.z+tile_depth(v,uv+vec2f(r,0));
        dz[2]=v.tile.z+tile_depth(v,uv-vec2f(0,r));dz[3]=v.tile.z+tile_depth(v,uv+vec2f(0,r));
        for(var i=0;i<4;i++) {if abs(dz[i]-center)>frame.normals.y {dz[i]=center;}}
        let tu=tile_world(v,uv+vec2f(r,0),dz[1])-tile_world(v,uv-vec2f(r,0),dz[0]);
        let tv=tile_world(v,uv+vec2f(0,r),dz[3])-tile_world(v,uv-vec2f(0,r),dz[2]);
        n=safe_normal(cross(tv,tu));if dot(n,vec3f(ISO,ISO,1.0))<0.0 {n=-n;}
    }
    var color=textureLoad(colors,pixel(v,uv),0)*v.tint;
    if v.info.y==0u && v.info.w==1u && v.info.z==0u && frame.sun_color.w>0.5 {
        let l=safe_normal(frame.sun.xyz);
        let ndl=max(dot(n,l),0.0);
        let half_direction=safe_normal(l+safe_normal(vec3f(ISO,ISO,0.5)));
        let spec=select(0.0,pow(max(dot(n,half_direction),0.0),max(v.extra.z,1.0)),ndl>0.0);
        color=vec4f(color.rgb*(v.ambient.rgb*frame.ambient.rgb+frame.sun_color.rgb*frame.sun.w*ndl)
            +v.specular.rgb*spec*frame.sun.w+v.emissive.rgb,color.a);
    }
    if color.a<0.001 {discard;}
    return Surface(color,n,d);
}
struct GBuffer {
    @location(0) albedo:vec4f,@location(1) normal:vec4f,@location(2) depth:f32,@location(3) id:u32,
    @builtin(frag_depth) z:f32,
};
fn output(v:Varying,s:Surface)->GBuffer {
    var out:GBuffer;
    out.albedo=s.color;out.normal=vec4f(s.normal*0.5+0.5,select(1.0,2.0,v.info.y==0u && (v.info.z>0u || v.info.w>0u)));out.depth=s.depth;
    out.id=v.info.x;out.z=clamp((s.depth-frame.camera.z)/frame.camera.w,0.0,1.0);
    return out;
}
@fragment fn opaque(v:Varying)->GBuffer {
    let s=surface(v);if s.color.a<0.999 {discard;}return output(v,s);
}
@fragment fn pick(v:Varying)->GBuffer {return output(v,surface(v));}
struct Transparent { @location(0) color:vec4f,@builtin(frag_depth) z:f32 };
@fragment fn transparent(v:Varying)->Transparent {
    let s=surface(v);if s.color.a>=0.999 {discard;}
    var out:Transparent;
    var light=shade(world_at(v.position.xy,s.depth),s.normal,1.0,1.0);
    if frame.settings.x==1.0 || (v.info.y==0u && (v.info.z>0u || v.info.w>0u)) {light=vec3f(1.0);}
    out.color=vec4f(s.color.rgb*light,s.color.a);out.z=clamp((s.depth-frame.camera.z)/frame.camera.w,0.0,1.0);
    return out;
}

fn effect_texel(v:Varying,p:vec2f)->vec4f {
    var t=p;
    if (v.info.w & 32u)!=0u {t=((p%v.rect.zw)+v.rect.zw)%v.rect.zw;}else{t=clamp(p,vec2f(0.0),v.rect.zw-1.0);}
    return textureLoad(colors,vec2i(v.rect.xy+t),0);
}
@fragment fn effect(v:Varying)->Transparent {
    let debug=(v.info.w>>1u)&7u;
    var uv=v.uv;
    if debug==2u {uv=v.normal.xy;}
    var color=vec4f(1.0);
    if debug!=1u && debug!=4u {
        let p=uv*v.rect.zw;
        if (v.info.w&16u)!=0u {
            let t=p-0.5;let a=floor(t);let f=fract(t);
            color=mix(mix(effect_texel(v,a),effect_texel(v,a+vec2f(1,0)),f.x),mix(effect_texel(v,a+vec2f(0,1)),effect_texel(v,a+vec2f(1,1)),f.x),f.y);
        }else{color=effect_texel(v,floor(p));}
    }
    if debug==4u && all(v.normal.xy>vec2f(0.025)) && all(v.normal.xy<vec2f(0.975)) {discard;}
    color*=v.tint;
    if color.a<0.001 {discard;}
    if (v.info.w&1u)!=0u && v.info.y!=3u {color=vec4f(color.rgb*shade(world_at(v.position.xy,v.p.z),vec3f(0,0,1),1.0,1.0),color.a);}
    var out:Transparent;out.color=color;out.z=clamp((v.p.z-frame.camera.z)/frame.camera.w,0.0,1.0);return out;
}
