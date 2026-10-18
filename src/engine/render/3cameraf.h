#ifndef __CAMERAFIX__
#define __CAMERAFIX__

class C3DCamera
{
public :
    C3DCamera();
    ~C3DCamera();

    void DrawGoraudZbuffer(sAGU_GORAUD p, unsigned short zvalue);

    void DrawTextureAndModulationGoraudZbuffer(sAGU_TEXTUREANDGORAUD p, short *tmap, int32_t tpowu, int32_t tumask, int32_t tvmask, unsigned short zvalue);
    void DrawTextureAndModulationGoraudZbufferWOff(sAGU_TEXTUREANDGORAUD p, short *tmap, int32_t tpowu, int32_t tumask, int32_t tvmask, unsigned short zvalue);
    void DrawTextureAndModulationGoraudZbufferROff(sAGU_TEXTUREANDGORAUD p, short *tmap, int32_t tpowu, int32_t tumask, int32_t tvmask, unsigned short zvalue);
    void DrawTextureAndModulationGoraud(sAGU_TEXTUREANDGORAUD p, short *tmap, int32_t tpowu, int32_t tumask, int32_t tvmask);

    void DrawTextureZbuffer4444(sAGU_TEXTURE p, short *tmap, int32_t tpowu, int32_t tumask, int32_t tvmask, 
                                unsigned short zvalue, unsigned short zwritevalue);
    void DrawTextureZbuffer4444WOff(sAGU_TEXTURE p, short *tmap, int32_t tpowu, int32_t tumask, int32_t tvmask, 
                                       unsigned short zvalue);
    void DrawTextureZbuffer4444ROff(sAGU_TEXTURE p, short *tmap, int32_t tpowu, int32_t tumask, int32_t tvmask, 
                                       unsigned short zvalue);
    void DrawTexture4444(sAGU_TEXTURE p, short *tmap, int32_t tpowu, int32_t tumask, int32_t tvmask);

    void SetScreenCenter(int32_t x, int32_t y);
    void SetStartEnd(int32_t startx, int32_t endx, int32_t starty, int32_t endy, int32_t screenx);
    void SetVideoBuffer(short* video, short* zbuffer);

    void SetAlphaTable(unsigned short* alphatable, int32_t alphacount, int32_t alphashift);
    void SetDivTable(int32_t* divtable);
    void SetAddlightTable(uint32_t* savecarrytable, unsigned short* addlighttable);
    void Set4444Table(unsigned short* fortable);

    void InitScreen();
    void InitLens(float fov, float aspect);
    int32_t GetCenterX() { return m_centerx; };
    int32_t GetCenterY() { return m_centery; };
    int32_t GetStartX() { return m_startx; };
    int32_t GetEndX() { return m_endx; };
    int32_t GetStartY() { return m_starty; };
    int32_t GetEndY() { return m_endy; };

    // it will be used as internal variable.
    int32_t         m_originx;
    int32_t         m_originy;
    int32_t         m_clippx;
    int32_t         m_clippy;
    short*      pm_videoorigin;
    short*      pm_zbufferorigin;
    // argument will be saved on here.
    int32_t         m_startx;
    int32_t         m_endx;
    int32_t         m_starty;
    int32_t         m_endy;
    int32_t         m_screenx;
    int32_t         m_centerx;
    int32_t         m_centery;
    short*      pm_video;
    short*      pm_zbuffer;

    uint32_t*   pm_savecarrytable;
    unsigned short* pm_addlighttable;
    int32_t*            pm_divtable;
    unsigned short* pm_alphatable;
    int32_t             m_alphacount;
    int32_t             m_alphashift;
    unsigned short* pm_fortable;
};
#endif