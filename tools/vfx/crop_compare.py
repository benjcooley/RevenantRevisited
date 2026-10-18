# crop_compare.py OUT x0 y0 w h scale FRAME.png... -- magnified side-by-side crops of the same region (pure python PNG).
import struct, zlib, sys, glob
def read_png(p):
    d=open(p,'rb').read(); pos=8; w=h=0; idat=b''; ct=0
    while pos<len(d):
        l=struct.unpack('>I',d[pos:pos+4])[0]; t=d[pos+4:pos+8]; c=d[pos+8:pos+8+l]; pos+=12+l
        if t==b'IHDR': w,h,bd,ct=struct.unpack('>IIBB',c[:10])
        elif t==b'IDAT': idat+=c
    raw=zlib.decompress(idat); bpp={2:3,6:4}[ct]; stride=w*bpp; rows=[]; prev=bytearray(stride); i=0
    for y in range(h):
        f=raw[i]; i+=1; line=bytearray(raw[i:i+stride]); i+=stride
        for x in range(stride):
            a=line[x-bpp] if x>=bpp else 0; b=prev[x]; c=prev[x-bpp] if x>=bpp else 0
            if f==1: line[x]=(line[x]+a)&255
            elif f==2: line[x]=(line[x]+b)&255
            elif f==3: line[x]=(line[x]+(a+b)//2)&255
            elif f==4:
                pa=abs(b-c); pb=abs(a-c); pc=abs(a+b-2*c)
                pr=a if pa<=pb and pa<=pc else (b if pb<=pc else c); line[x]=(line[x]+pr)&255
        rows.append(bytes(line)); prev=line
    return w,h,bpp,rows
def write_png(out,w,h,o):
    def chunk(t,c): return struct.pack('>I',len(c))+t+c+struct.pack('>I',zlib.crc32(t+c)&0xffffffff)
    open(out,'wb').write(b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('>IIBBBBB',w,h,8,2,0,0,0))+chunk(b'IDAT',zlib.compress(bytes(o)))+chunk(b'IEND',b''))
# montage: for each input file, crop (x0,y0,cw,ch) magnified s, side by side
x0,y0,cw,ch,s=int(sys.argv[2]),int(sys.argv[3]),int(sys.argv[4]),int(sys.argv[5]),int(sys.argv[6])
files=sys.argv[7:]
crops=[]
for f in files:
    w,h,bpp,rows=read_png(f)
    crops.append([rows[y0+y][(x0)*bpp:(x0+cw)*bpp] for y in range(ch)])
W=cw*s*len(files); H=ch*s; o=bytearray()
for y in range(ch):
    line=bytearray()
    for cr in crops:
        row=cr[y]
        for x in range(cw):
            px=row[x*bpp:x*bpp+3]; line+=px*s
    for _ in range(s): o+=b'\x00'+bytes(line)
write_png(sys.argv[1],W,H,o)
