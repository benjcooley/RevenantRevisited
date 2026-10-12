"""Qualified fixed-function RGBA4444 source-alpha device, linear RGB8 target.

Orthographic projected vertices and UVs must come from original draw packets.
This is an independent semantic device, not the retail RGB565 rasterizer.
"""
import math,struct

def rgba4444(word):return tuple(((word>>shift)&15)/15. for shift in (8,4,0,12))
def source_alpha(source,destination):
    alpha=source[3]
    return tuple(min(255,max(0,int(math.floor(source[c]*255*alpha+destination[c]*(1-alpha)+.5))))for c in range(3))
def emissive_saturation(material):
    """Exact shipped material: nonnegative light terms plus white emission.

    D3DMATERIAL: size, diffuse4, ambient4, specular4, emissive4, power,
    texture handle/ramp size. Reject any material needing a general lighting
    implementation rather than replacing its unknown illumination with white.
    """
    values=struct.unpack('<I17f2I',material)
    assert values[0]==80 and values[1:17]==(1.,)*16 and values[17]==0.
    return (1.,1.,1.,1.)

def raster(packets,filtering,width,height,background=(0,0,0),pixel_center=0.):
    assert filtering in ('point','linear') and pixel_center in (0.,.5);frame=bytearray(bytes(background)*(width*height))
    for p in packets:
        diffuse=emissive_saturation(bytes.fromhex(p['material_hex']))
        assert diffuse==(1.,)*4 # actual saturated emission, not supplied vertex colors
        vertices=p['projected'];raw=bytes.fromhex(p['vertices_hex']);uv=[struct.unpack_from('<2f',raw,i*32+24)for i in range(len(vertices))]
        tw,th=p['texture_width'],p['texture_height'];texture=struct.unpack('<'+'H'*(tw*th),bytes.fromhex(p['texture_hex']))
        def sample(u,v):
            if filtering=='point':return rgba4444(texture[(math.floor(v*th)%th)*tw+math.floor(u*tw)%tw])
            x,y=u*tw-.5,v*th-.5;ix,iy=math.floor(x),math.floor(y);fx,fy=x-ix,y-iy
            values=[rgba4444(texture[((iy+dy)%th)*tw+(ix+dx)%tw])for dy in (0,1)for dx in (0,1)]
            return tuple(sum(values[k][c]*w for k,w in enumerate(((1-fx)*(1-fy),fx*(1-fy),(1-fx)*fy,fx*fy)))for c in range(4))
        for tri in zip(*[iter(p['indices'])]*3):
            ids=list(tri);pts=[vertices[i][:2]for i in ids]
            def edge(a,b,x,y):return(b[0]-a[0])*(y-a[1])-(b[1]-a[1])*(x-a[0])
            area=edge(pts[0],pts[1],*pts[2])
            if area==0:continue
            if area<0:ids[1],ids[2]=ids[2],ids[1];pts[1],pts[2]=pts[2],pts[1];area=-area
            edges=[(pts[1],pts[2]),(pts[2],pts[0]),(pts[0],pts[1])];include=[b[1]<a[1]or(b[1]==a[1]and b[0]>a[0])for a,b in edges]
            for y in range(max(0,math.ceil(min(q[1]for q in pts)-pixel_center)),min(height-1,math.floor(max(q[1]for q in pts)-pixel_center))+1):
                for x in range(max(0,math.ceil(min(q[0]for q in pts)-pixel_center)),min(width-1,math.floor(max(q[0]for q in pts)-pixel_center))+1):
                    w=[edge(a,b,x+pixel_center,y+pixel_center)for a,b in edges]
                    if any(e<0 or(e==0 and not flag)for e,flag in zip(w,include)):continue
                    w=[e/area for e in w];z=sum(w[k]*vertices[ids[k]][2]for k in range(3))
                    if not 0<=z<=65535:continue
                    u,v=(sum(w[k]*uv[ids[k]][c]for k in range(3))for c in range(2));source=sample(u,v);offset=(y*width+x)*3
                    frame[offset:offset+3]=bytes(source_alpha(source,frame[offset:offset+3]))
    return bytes(frame)
