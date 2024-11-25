// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   Surface.cpp - TSurface Object                       *
// *************************************************************************

#include "surface.h"

#include "display.h"
#include "font.h"

// If this is true, surfaces are unlocked as soon as we have the pointer.  This is great for
// debugging, and seems to work fine on every video card except for the VooDoo and VooDoo 2
//
// If you notice graphics getting screwed up, try setting this to false..  This should always
// be false if _DEBUG is not defined.

TSurface::TSurface() {
    locked = nullptr;
    width = height = stride = 0;
    clipx = clipy = 0;
    clipwidth = width;
    clipheight = height;
    clipmode = CLIP_EDGES;
    flags = 0;
    keycolor = 0;
    originx = originy = 0;
    
    // Initialize buffers
    cpu_buffer = nullptr;
    buffer_size = 0;
    image = {};
    // Initialize image description with defaults
    img_desc = {};
    img_desc.type = SG_IMAGETYPE_2D;
    img_desc.render_target = false;
    img_desc.num_mipmaps = 1;
    img_desc.usage = SG_USAGE_DYNAMIC;
    img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    img_desc.sample_count = 1;
    img_desc.min_filter = SG_FILTER_LINEAR;
    img_desc.mag_filter = SG_FILTER_LINEAR;
    img_desc.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
    img_desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
}

TSurface::TSurface(int32_t w, int32_t h, int32_t bpp) {
    locked = nullptr;
    width = w;
    height = h;
    bitsperpixel = bpp;
    stride = width;
    clipx = clipy = 0;
    clipwidth = width;
    clipheight = height;
    clipmode = CLIP_EDGES;
    flags = 0;
    keycolor = 0;
    originx = originy = 0;
    
    // Initialize buffers
    cpu_buffer = nullptr;
    buffer_size = width * height * sizeof(uint32_t);
    // Create new Sokol image
    img_desc.width = width;
    img_desc.height = height;
    image = sg_make_image(&img_desc);
    
}

TSurface::TSurface(sg_image existing_image, int32_t w, int32_t h, int32_t bpp) {
    locked = nullptr;
    width = w;
    height = h;
    bitsperpixel = bpp;
    stride = width;
    clipx = clipy = 0;
    clipwidth = width;
    clipheight = height;
    clipmode = CLIP_EDGES;
    flags = 0;
    keycolor = 0;
    originx = originy = 0;
    
    // Initialize buffers
    cpu_buffer = nullptr;
    buffer_size = width * height * sizeof(uint32_t);
    image = existing_image;
    
    // Initialize image description with defaults
    img_desc = {};
    img_desc.type = SG_IMAGETYPE_2D;
    img_desc.render_target = true;
    img_desc.width = width;
    img_desc.height = height;
    img_desc.num_mipmaps = 1;
    img_desc.usage = SG_USAGE_DYNAMIC;
    img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    img_desc.sample_count = 1;
    img_desc.min_filter = SG_FILTER_LINEAR;
    img_desc.mag_filter = SG_FILTER_LINEAR;
    img_desc.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
    img_desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
}

TSurface::~TSurface()
{
    if (cpu_buffer) {
        free(cpu_buffer);
        cpu_buffer = nullptr;
        buffer_size = 0;
    }
}

void TSurface::Reset()
{
    SetOrigin(0, 0);
    SetClipRect(0, 0, width, height);
    SetClipMode(CLIP_EDGES);
}

// This function sets up the ParamDraw DrawParam structure.  This function is also
// called in higher level surfaces before being called here, so MAKE SURE that it
// can be called multiple times without continuously adding values like registration
// offsets each time.

bool TSurface::ParamDrawSetup(RSDrawParam dpv, PTBitmap bitmap)
{
  // Set drawmode to bitmap drawmode if DM_USEDEFAULT
    if (dpv.drawmode == DM_USEDEFAULT)
    {
        if (bitmap != nullptr)
            dpv.drawmode = bitmap->drawmode;
        else
            dpv.drawmode = DM_DEFAULT;
    }

  // If surface is wrap clipping and drawmode isn't, set drawmode
    if ((clipmode == CLIP_WRAP) &&
      !(dpv.drawmode & (DM_WRAPCLIP | DM_WRAPCLIPSRC | DM_NOWRAPCLIP)))
        dpv.drawmode |= DM_WRAPCLIP;

  // Adjust for registration point
    if (bitmap && (dpv.drawmode & DM_USEREG))
    {
        dpv.dx -= bitmap->regx;
        dpv.dy -= bitmap->regy;
        dpv.drawmode &= ~DM_USEREG; // Don't do USERGEG again
    }

  // Set up highlight color for selecting
    if (dpv.drawmode & DM_SELECTED)
    {
        SColor color = { 220, 10, 30 };
        dpv.color = TranslateColor(color);
    }

  // Set up cliping and origin
    dpv.originx     = originx;
    dpv.originy     = originy;

    dpv.clipx       = clipx;
    dpv.clipy       = clipy;
    dpv.clipwidth   = clipwidth;
    dpv.clipheight  = clipheight;

    dpv.callback    = GetDrawCallBack();

  // Do a quick clip check before we go to the trouble to lock the buffers
    int32_t dx = dpv.dx + originx;
    int32_t dy = dpv.dy + originy;
    if (dx + dpv.dwidth <= clipx || dy + dpv.dheight <= clipy ||
        dx >= clipx + clipwidth || dy >= clipy + clipheight)
            return false;

    return true;
}

// This function calls the low level graphics Draw function.. It takes care
// of wrap clipping, clipping, and decoding the DrawMode flags and DrawPrimitive
// commands.  This is THE central low level asm code drawing function for all
// graphics.  All low level graphics calls MUST go through this function or they
// WILL NOT WORK on all surface classes!

bool TSurface::ParamDraw(PSDrawParam dp, PTBitmap bitmap)
{
    SDrawParam dpv = *dp;  // Don't mess up the draw param struct passed to us

    if (!ParamDrawSetup(dpv, bitmap))
        return false;

  // Setup drawblock and lock surfaces
    SDrawBlock  db;
    db.dstbitmapflags = flags;

  // If we have a bitmap.. use it
    if (bitmap)  // Get bitmap for blit
    {
        db.sbufwidth      = bitmap->width;
        db.sbufheight     = bitmap->height;
        db.sstride        = bitmap->width;
        db.keycolor       = bitmap->keycolor;

      // Return now if we don't draw anything
        if (dpv.sx >= db.sbufwidth || dpv.sy >= db.sbufheight ||
            dpv.sx + dpv.swidth < 0 || dpv.sy + dpv.sheight < 0)
                return false;

        db.srcbitmapflags = bitmap->flags;
        db.source         = &bitmap->data16;
        db.szbuffer       = (uint16_t *)bitmap->zbuffer.ptr();
        db.szstride       = bitmap->width;

        if (!db.szbuffer)
            db.srcbitmapflags  &= ~BM_ZBUFFER;

        db.snormals       = (uint16_t *)bitmap->normal.ptr();

        if (!db.snormals)
            db.srcbitmapflags  &= ~BM_NORMALS;

        db.alpha          = (uint8_t *)bitmap->alpha.ptr();

        if (!db.alpha)
            db.srcbitmapflags  &= ~BM_ALPHA;

        db.palette        = (uint16_t *)bitmap->palette.ptr();
        db.alias          = (uint8_t *)bitmap->alias.ptr();

        if (!db.alias)
            db.srcbitmapflags  &= ~BM_ALIAS;
    }

    else 
    {
        db.srcbitmapflags = 0;
        db.source         = nullptr;
        db.szbuffer       = nullptr; 
        db.snormals       = nullptr; 
        db.palette        = nullptr;
        db.alpha          = nullptr;
        db.alias          = nullptr;
        db.sbufwidth      = 0;
        db.sbufheight     = 0;
        db.sstride        = 0;
        db.szstride       = 0;
        db.keycolor       = 0;
    }

  // Lock graphics buffer!!
    db.dest = Lock();
    if (db.dest == nullptr) 
        return false;  // Duh!
    if (UnlockImmediately)
        Unlock();

  // Lock z buffer
    if (GetZBuffer() == nullptr || !(dp->drawmode & (DM_ZBUFFER | DM_ZSTATIC)) || 
        (bitmap && !(bitmap->flags & BM_ZBUFFER) && !(dp->drawmode & DM_ZSTATIC)))
    {
        db.dzbuffer = nullptr;
        db.dzstride = 0;
        db.dstbitmapflags  &= ~BM_ZBUFFER;
    }
    else
    {
        // NoVidZBufLock - Means you can't lock the display video surface and the 
        //                 display zbuffer at the same time (voodoo cards can't do this).
        // UseClearZBuffer - Means we are using a sneaky alternate fake ZBuffer for
        //                 the Display object.  An app can get the real zbuffer surface
        //                 in this mode by calling GetRealZBuffer() for the display.
        //                 We use this sneaky buffer so we can get around the simultaneous
        //                 access problems below, and to eliminate any read/write/lock
        //                 problems a non cooperative zbuffer (i.e. voodoo) may have.
        //
        // We can get a simultaneous zbuffer/video buffer if NoVidZBufLock is false,
        // or if UseClearZBuffer is true, or we're not drawing to the display.

        if ((void *)this == (void *)Display && NoVidZBufLock && !UseClearZBuffer)
            db.dzbuffer = nullptr;
        else
            db.dzbuffer  = (uint16_t *)(GetZBuffer()->Lock());

        if (db.dzbuffer == db.dest) // Video card can only lock one buffer at a time (voodoo)
            db.dzbuffer = nullptr;     // If can't lock it, don't do zbuffer draw

        if (UnlockImmediately)
            GetZBuffer()->Unlock();

        if (db.dzbuffer)
        {
            db.dstbitmapflags  |= BM_ZBUFFER;
            db.dzstride = GetZBuffer()->Stride();
        }
        else
        {
            db.dstbitmapflags  &= ~BM_ZBUFFER;
            db.dzstride = 0;
        }
    }

  // Lock normal buffer
/*  if (GetNormalBuffer() == nullptr || !(dp->drawmode & DM_NORMALS) ||
        (bitmap && !(bitmap->flags & BM_NORMALS)))
    {
        db.dnormals = nullptr;
        db.dstbitmapflags  &= ~BM_NORMALS;
    }
            
    else
    {
        db.dnormals  = (uint16_t *)GetNormalBuffer()->Lock();
        GetNormalBuffer()->Unlock();
        if (db.dnormals)
            db.dstbitmapflags  |= BM_NORMALS;   

        else
            db.dstbitmapflags  &= ~BM_NORMALS;  
    } */
    db.dnormals = nullptr;

  // Set width/height stuff
    db.dbufwidth  = width;
    db.dbufheight = height;
    db.dstride    = stride;

  // Okay baby, now do it!
    bool result = Draw(&db, &dpv);

    if (IsLocked())
        Unlock();
    if (GetZBuffer() && GetZBuffer()->IsLocked())
        GetZBuffer()->Unlock();

    return result;
}

bool TSurface::ParamBlitSetup(RSDrawParam tmpdp, TSurface* srcsurface, int32_t flags)
{
  // Set defaults
    if (tmpdp.drawmode == DM_USEDEFAULT)
        tmpdp.drawmode = DM_DEFAULT;

  // If surface is wrap clipping and drawmode isn't, set drawmode
    if ((clipmode == CLIP_WRAP) &&
      !(tmpdp.drawmode & (DM_WRAPCLIP | DM_WRAPCLIPSRC | DM_NOWRAPCLIP)))
        tmpdp.drawmode |= DM_WRAPCLIP;

    int32_t sox, soy;
    if (srcsurface)
        srcsurface->GetOrigin(sox, soy);
    else
        sox = soy = 0;
    tmpdp.sx += sox;
    tmpdp.sy += soy;

    tmpdp.originx    = originx;
    tmpdp.originy    = originy;

    tmpdp.clipx      = clipx;
    tmpdp.clipy      = clipy;
    tmpdp.clipwidth  = clipwidth;
    tmpdp.clipheight = clipheight;
    
    tmpdp.callback   = GetDrawCallBack();

    return true;
}

bool TSurface::BlitHandler(PSDrawParam dp, TSurface* srcsurface, int32_t flags)
{
    SDrawParam tmpdp = *dp;
    
    if (!ParamBlitSetup(tmpdp, srcsurface, flags))
        return false;

    // Setup Sokol render pass
    sg_pass_action pass_action = {};
    pass_action.colors[0] = { .action = SG_ACTION_LOAD };
    
    // Create pipeline if needed
    if (!pipeline.id) {
        sg_pipeline_desc pip_desc = {};
        pip_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
        pip_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT2;
        pip_desc.shader = sg_make_shader(blit_shader_desc()); 
        pipeline = sg_make_pipeline(pip_desc);
        pip_desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
        pip_desc.colors[0].blend.enabled = true;
        pip_desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
        pip_desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
        pipeline = sg_make_pipeline(&pip_desc);
    }

    tmpdb.dbufwidth  = width;
    tmpdb.dbufheight = height;
    tmpdb.dstride = stride;
    tmpdb.dstbitmapflags = flags;

    if (srcsurface)
    {
      // Voodoo can't use zbuffer as a source surface
//      if (NoVidZBufLock && srcsurface == Display->GetZBuffer())
//          return true;

        tmpdb.srcbitmapflags = srcsurface->flags;
        tmpdb.keycolor   = srcsurface->KeyColor();
        tmpdb.sbufwidth  = srcsurface->Width();
        tmpdb.sbufheight = srcsurface->Height();
        tmpdb.sstride = srcsurface->Stride();

      // Clip source
        if (!(tmpdp.drawmode & DM_WRAPCLIPSRC) &&
           (tmpdp.sx >= tmpdb.sbufwidth || tmpdp.sy >= tmpdb.sbufheight ||
            tmpdp.sx + tmpdp.swidth < 0 || tmpdp.sy + tmpdp.sheight < 0))
                return false;

        tmpdb.source = srcsurface->Lock();
        if (!tmpdb.source)
            return false;
        if (UnlockImmediately)
            srcsurface->Unlock();
    }
    else
    {   
        tmpdb.srcbitmapflags = 0;
        tmpdb.sbufwidth = tmpdb.sbufheight = tmpdb.sstride = 0;
        tmpdb.source = nullptr;
        tmpdb.keycolor = 0;
    }

    tmpdb.dest = Lock();
    if (!tmpdb.dest)
        return false;
    if (UnlockImmediately)
        Unlock();

    if (!tmpdb.dest)
    {
        if (srcsurface && srcsurface->IsLocked())
            srcsurface->Unlock();
        return false;
    }

    tmpdb.szbuffer = tmpdb.dzbuffer = nullptr;
    tmpdb.szstride = tmpdb.dzstride = 0;
    tmpdb.snormals = tmpdb.dnormals = nullptr;

    tmpdb.palette  = nullptr;
    tmpdb.alias    = tmpdb.alpha    = nullptr;

    tmpdp.drawmode &= ~(DM_ZBUFFER | DM_NORMALS | DM_ALPHA | DM_ALIAS);

    bool result = Draw(&tmpdb, &tmpdp);

    if (IsLocked())
        Unlock();
    if (srcsurface && srcsurface->IsLocked())
        srcsurface->Unlock();

    return result;
}

bool TSurface::ParamBlit(PSDrawParam dp, TSurface* surface, int32_t flags)
{
    if (surface && surface->UseGetBlit())  // If source surface is complex, call its Get function instead
    {
        return surface->ParamGetBlit(dp, this, flags);
    }

    if (!(dp->drawmode & DM_NODRAW))
    {
        if (!BlitHandler(dp, surface, flags))
            return false;
    }
    if (GetZBuffer() && 
        (!surface || surface->GetZBuffer()) && 
        (surface && (GetZBuffer() != surface->GetZBuffer())) &&
        (dp->drawmode & DM_ZBUFFER))
    {
        TSurface* zbuffer = nullptr;
        if (surface)
            zbuffer = surface->GetZBuffer();
//      if ((void *)this == (void *)Display && NoVidZBufLock)
//      {
//          dp->drawmode |= DM_NOHARDWARE;
//      }
        if (!GetZBuffer()->BlitHandler(dp, zbuffer, flags))
             return false;
    }
    if (GetNormalBuffer() && 
        (!surface || surface->GetNormalBuffer()) &&
        (surface && (GetNormalBuffer() != surface->GetNormalBuffer())) &&
        (dp->drawmode & DM_NORMALS))
    {
        TSurface* normals = nullptr;
        if (surface)
            normals = surface->GetNormalBuffer();
        if (!GetNormalBuffer()->BlitHandler(dp, normals, flags))
            return false;
    }

    return true;
}

bool TSurface::ParamGetBlit(PSDrawParam dp, TSurface* surface, int32_t flags)
{
    return surface->ParamBlit(dp, this, flags);
}

bool TSurface::Put(int32_t x, int32_t y, PTBitmap bitmap, uint32_t drawmode, PSColor color)
{ 
    if (!bitmap)
        return false;

    SDrawParam dp;
    MakeDP(dp, x, y, 0, 0, bitmap->width, bitmap->height, drawmode);

    if (color)
    {
        dp.color = TranslateColor(*color);
        dp.drawmode |= DM_CHANGECOLOR;
    }

    return ParamDraw(&dp, bitmap); 
}

bool TSurface::PutHue(int32_t x, int32_t y, PTBitmap bitmap, uint32_t drawmode, int32_t hue)
{ 
    if (!bitmap)
        return false;

    SDrawParam dp;
    MakeDP(dp, x, y, 0, 0, bitmap->width, bitmap->height, drawmode);

    dp.color = hue;
    dp.drawmode |= DM_CHANGEHUE;

    return ParamDraw(&dp, bitmap); 
}

bool TSurface::PutSV(int32_t x, int32_t y, PTBitmap bitmap, uint32_t drawmode, int32_t saturation, int32_t brightness)
{ 
    if (!bitmap)
        return false;

    SDrawParam dp;
    MakeDP(dp, x, y, 0, 0, bitmap->width, bitmap->height, drawmode);

    saturation &= 0xFF;
    brightness &= 0xFF;

    dp.color = (saturation << 8) | brightness;
    dp.drawmode |= DM_CHANGESV;

    return ParamDraw(&dp, bitmap); 
}

bool TSurface::PutDim(int32_t x, int32_t y, PTBitmap bitmap, uint32_t drawmode, int32_t dim)
{ 
    if (!bitmap)
        return false;

    SDrawParam dp;
    MakeDP(dp, x, y, 0, 0, bitmap->width, bitmap->height, drawmode);

    dp.intensity = 31 - min(31, dim);

    return ParamDraw(&dp, bitmap);
}

bool TSurface::ZPut(int32_t x, int32_t y, int32_t z, PTBitmap bitmap, uint32_t drawmode)
{ 
    SDrawParam dp; 
    MakeDP(dp, x, y, 0, 0, bitmap->width, bitmap->height, drawmode);
    dp.zpos = (uint16_t)z;
    return ParamDraw(&dp, bitmap); 
}

bool TSurface::ZPutDim(int32_t x, int32_t y, int32_t z, PTBitmap bitmap, uint32_t drawmode, int32_t dim, PSColor color)
{ 
    SDrawParam dp; 
    MakeDP(dp, x, y, 0, 0, bitmap->width, bitmap->height, drawmode);
    dp.zpos = (uint16_t)z;
    dp.intensity = 31 - min(31, dim);
    if (color)
        dp.color = TranslateColor(*color);
    else
        dp.color = 0xffff;

    return ParamDraw(&dp, bitmap); 
}

uint32_t TSurface::ZFind(int32_t x, int32_t y, int32_t z, PTBitmap bitmap, uint32_t drawmode)
{ 
    SDrawParam dp; 
    MakeDP(dp, x, y, 0, 0, bitmap->width, bitmap->height, drawmode);
    dp.func = ::ZFind;
    dp.zpos = (uint16_t)z;

    return ParamDraw(&dp, bitmap);
}

bool TSurface::Box(int32_t dx, int32_t dy, int32_t dwidth, int32_t dheight, 
     uint32_t color, uint16_t zpos, uint16_t normal, uint32_t drawmode)
{ 
    if (drawmode & DM_USEDEFAULT)
        drawmode = DM_ZBUFFER | DM_NORMALS | DM_FILL;

    SDrawParam dp; 
    MakeDP(dp, dx, dy, 0, 0, dwidth, dheight, drawmode | DM_FILL);

    dp.func = ::Box;
    
    dp.zpos      = zpos;
    dp.normal    = normal;
    dp.color     = color;

    return ParamDraw(&dp);
}

bool TSurface::Line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, SColor &color, uint32_t drawmode)
{
        SDrawParam dp;
        SLineParam lp;

        MakeDP(dp, x1, y1, x1, y1, x2 - x1 + 1, y2 - y1 + 1, drawmode);
        dp.func = LineDraw;
        dp.color = TranslateColor(color);
        dp.data = (void *)&lp;
        lp.x1 = x1;
        lp.y1 = y1;
        lp.x2 = x2;
        lp.y2 = y2;

        return ParamDraw(&dp); 
}

bool TSurface::Rect(int32_t x, int32_t y, int32_t w, int32_t h, SColor &color, uint32_t drawmode)
{
    if (!Line(x, y, x + w - 1, y, color, drawmode))
        return false;
    if (!Line(x, y + 1, x, y + h - 2, color, drawmode))
        return false;
    if (!Line(x + w - 1, y + 1, x + w - 1, y + h - 2, color, drawmode))
        return false;
    if (!Line(x, y + h - 1, x + w - 1, y + h - 1, color, drawmode))
        return false;
    return true;
}

int32_t TSurface::WriteText(char *text, int32_t x, int32_t y, int32_t numlines, TFont* font, PSColor color,
                         uint32_t drawmode, int32_t wrapwidth, int32_t startline, int32_t justify, int32_t hue, int32_t linespace)
{
    if (!text || !*text)
        return true;

    SDrawParam dp; 
    STextParam tp;

    MakeDP(dp, x, y, 0, 0, width, height, drawmode);
    if (drawmode == DM_USEDEFAULT)
        dp.drawmode = FONT_DRAWMODE;
    else
        dp.drawmode = drawmode;
    dp.dx = x;
    dp.dy = y;
    dp.dwidth  = width;
    dp.dheight = height;
    dp.func = TextDraw;
    dp.data = (void *)&tp;

    tp.text = text;
    tp.numlines = numlines;
    tp.font = font;
    tp.wrapwidth = wrapwidth;
    tp.startline = startline;
    tp.justify = justify;
    tp.draw = true;
    tp.length = 0; // Length of text drawn
    tp.noclip = drawmode & DM_NOCLIP;       // save noclip value
    tp.linespace = linespace;
    dp.drawmode |= DM_NOCLIP;               // then set it temporarily

    if (color)
    {
        dp.drawmode |= DM_CHANGECOLOR;
        dp.color = TranslateColor(*color);
    }
    else if (hue >= 0)
    {
        dp.drawmode |= DM_CHANGEHUE;
        dp.color = hue;
    }
    else
        dp.color = 0;

    ParamDraw(&dp);

    return tp.length;
}

int32_t TSurface::WriteTextShadow(char *text, int32_t x, int32_t y, int32_t numlines, TFont* font, PSColor color,
                               uint32_t drawmode, int32_t wrapwidth, int32_t startline, int32_t justify, int32_t hue, int32_t linespace)
{
    SColor black = { 0, 0, 0 };
    WriteText(text, x + 1, y + 1, numlines, font, &black, drawmode, wrapwidth, startline, justify, linespace);
    int32_t len = WriteText(text, x, y, numlines, font, color, drawmode, wrapwidth, startline, justify, hue, linespace);

    return len;
}
void* TSurface::Lock()
{
    if (!locked) {
        // Allocate staging buffer if needed
        if (!cpu_buffer) {
            buffer_size = width * height * sizeof(uint32_t);
            cpu_buffer = (uint8_t*)malloc(buffer_size);
        }

        // Read back current texture content into staging buffer
        if (image.id) {
            // Setup image data for 2D texture
            sg_image_data img_data = {};
            img_data.subimage[0][0].ptr = cpu_buffer;
            img_data.subimage[0][0].size = buffer_size;

            // Create image if it doesn't exist
            if (!image.id) {
                img_desc.width = width;
                img_desc.height = height;
                img_desc.data = img_data;
                image = sg_make_image(img_desc);
            }

            // Initialize CPU buffer with zeros if no previous content
            memset(cpu_buffer, 0, buffer_size);
        }

        locked = cpu_buffer;
    }
    return locked;
}

bool TSurface::Unlock()
{
    if (locked) {
        // Upload staging buffer to GPU texture
        if (image.id && cpu_buffer) {
            // Create immutable buffer with current CPU data
            sg_buffer_desc buf_desc = {};
            buf_desc.size = buffer_size;
            buf_desc.type = SG_BUFFERTYPE_VERTEXBUFFER;
            buf_desc.usage = SG_USAGE_IMMUTABLE;
            buf_desc.data.ptr = cpu_buffer;
            buf_desc.data.size = buffer_size;
            
            // Update image content via buffer
            sg_image_data img_data = {};
            img_data.subimage[0][0].ptr = cpu_buffer;
            img_data.subimage[0][0].size = buffer_size;
            sg_update_image(image, img_data);
        }
        locked = nullptr;
        return true;
    }
    return false;
}
