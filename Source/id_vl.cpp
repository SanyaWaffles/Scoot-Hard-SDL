// ID_VL.C

#include <string.h>
#include "wl_def.h"
#pragma hdrstop

// Uncomment the following line, if you get destination out of bounds
// assertion errors and want to ignore them during debugging
//#define IGNORE_BAD_DEST

#ifdef IGNORE_BAD_DEST
#undef assert
#define assert(x) if(!(x)) return
#define assert_ret(x) if(!(x)) return 0
#else
#define assert_ret(x) assert(x)
#endif

boolean fullscreen = false;
#if defined(_arch_dreamcast)
boolean usedoublebuffering = false;
unsigned screenWidth = 320;
unsigned screenHeight = 200;
unsigned screenBits = 8;
#elif defined(GP2X)
boolean usedoublebuffering = true;
unsigned screenWidth = 320;
unsigned screenHeight = 240;
#if defined(GP2X_940)
unsigned screenBits = 8;
#else
unsigned screenBits = 16;
#endif
#else
boolean usedoublebuffering = true;
unsigned screenWidth = 960;
unsigned screenHeight = 600;
unsigned screenBits = -1;      // use "best" color depth according to libSDL
#endif

SDL_Rect rct;

SDL_Surface *screen = NULL;
unsigned screenPitch;

SDL_Surface *screenBuffer = NULL;
unsigned bufferPitch;

SDL_Surface *curSurface = NULL;
SDL_Surface *lastGameSurface = NULL;
unsigned curPitch;

unsigned scaleFactor;

boolean	 screenfaded;
unsigned bordercolor;

SDL_Color palette1[256], palette2[256];
SDL_Color curpal[256];


#define CASSERT(x) extern int ASSERT_COMPILE[((x) != 0) * 2 - 1];
#define RGB(r, g, b) {(r)*255/63, (g)*255/63, (b)*255/63, 0}

SDL_Color gamepal[]={
#ifdef SPEAR
    #include "sodpal.inc"
#else
    #include "wolfpal.inc"
#endif
};

CASSERT(lengthof(gamepal) == 256)

//===========================================================================


/*
=======================
=
= VL_Shutdown
=
=======================
*/

void	VL_Shutdown (void)
{
	//VL_SetTextMode ();
}

void DrawPixel(SDL_Surface *surface, short x, short y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    
    switch(bpp) {
        case 1:
            *p = pixel;
            break;
            
        case 2:
            *(Uint16 *)p = pixel;
            break;
            
        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;
            
        case 4:
            *(Uint32 *)p = pixel;
            break;
    }
}


Uint32 ReadPixel(SDL_Surface *surface, short x, short y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    
    switch(bpp) {
        case 1:
            return *p;
            break;
            
        case 2:
            return *(Uint16 *)p;
            break;
            
        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
            break;
            
        case 4:
            return *(Uint32 *)p;
            break;
            
        default:
            return 0;       /* shouldn't happen, but avoids warnings */
    }
}

SDL_Surface *ScaleSurface(SDL_Surface *Surface, Uint16 Width, Uint16 Height)
{
    if(!Surface || !Width || !Height)
        return 0;
    
    SDL_Surface *_ret = SDL_CreateRGBSurface(Surface->flags, Width, Height, Surface->format->BitsPerPixel,
                                             Surface->format->Rmask, Surface->format->Gmask, Surface->format->Bmask, Surface->format->Amask);
    
    double    _stretch_factor_x = (static_cast<double>(Width)  / static_cast<double>(Surface->w)),
    _stretch_factor_y = (static_cast<double>(Height) / static_cast<double>(Surface->h));
    
    for(Sint32 y = 0; y < Surface->h; y++)
        for(Sint32 x = 0; x < Surface->w; x++)
            for(Sint32 o_y = 0; o_y < _stretch_factor_y; ++o_y)
                for(Sint32 o_x = 0; o_x < _stretch_factor_x; ++o_x)
                    DrawPixel(_ret, static_cast<Sint32>(_stretch_factor_x * x) + o_x,
                              static_cast<Sint32>(_stretch_factor_y * y) + o_y, ReadPixel(Surface, x, y));
    
    return _ret;
}

/*
=======================
=
= VL_SetVGAPlaneMode
=
=======================
*/

void	VL_SetVGAPlaneMode (void)
{
#ifdef SPEAR
    SDL_WM_SetCaption("Spear of Destiny", NULL);
#else
    SDL_WM_SetCaption("Scoot Hard", NULL);
#endif

    if(screenBits == -1)
    {
        const SDL_VideoInfo *vidInfo = SDL_GetVideoInfo();
        screenBits = vidInfo->vfmt->BitsPerPixel;
    }

        screen = SDL_SetVideoMode(screenWidth, screenHeight, screenBits,
                         (usedoublebuffering ? SDL_HWSURFACE | SDL_DOUBLEBUF : 0)
                         | (screenBits == 8 ? SDL_HWPALETTE : 0)
                         | (fullscreen ? SDL_FULLSCREEN : 0));
        if(!screen)
        {
            printf("Unable to set %ix%ix%i video mode: %s\n", screenWidth,
                   screenHeight, screenBits, SDL_GetError());
            exit(1);
        }
        if((screen->flags & SDL_DOUBLEBUF) != SDL_DOUBLEBUF)
            usedoublebuffering = false;
        SDL_ShowCursor(SDL_DISABLE);
        
        SDL_SetColors(screen, gamepal, 0, 256);
        memcpy(curpal, gamepal, sizeof(SDL_Color) * 256);
        
        
        screenBuffer = SDL_CreateRGBSurface(SDL_SWSURFACE, screenWidth,
                                            screenHeight, 8, 0, 0, 0, 0);
        if(!screenBuffer)
        {
            printf("Unable to create screen buffer surface: %s\n", SDL_GetError());
            exit(1);
        }
        SDL_SetColors(screenBuffer, gamepal, 0, 256);
        
        screenPitch = screen->pitch;
        bufferPitch = screenBuffer->pitch;
        
        curSurface = screenBuffer;
        curPitch = bufferPitch;
        
        scaleFactor = screenWidth/320;
        if(screenHeight/200 < scaleFactor) scaleFactor = screenHeight/200;
        
        pixelangle = (short *) malloc(screenWidth * sizeof(short));
        CHECKMALLOCRESULT(pixelangle);
        wallheight = (int *) malloc(screenWidth * sizeof(int));
        CHECKMALLOCRESULT(wallheight);
    
}

/*
=============================================================================

						PALETTE OPS

		To avoid snow, do a WaitVBL BEFORE calling these

=============================================================================
*/

/*
=================
=
= VL_ConvertPalette
=
=================
*/

void VL_ConvertPalette(byte *srcpal, SDL_Color *destpal, int numColors)
{
    for(int i=0; i<numColors; i++)
    {
        destpal[i].r = *srcpal++ * 255 / 63;
        destpal[i].g = *srcpal++ * 255 / 63;
        destpal[i].b = *srcpal++ * 255 / 63;
    }
}

/*
=================
=
= VL_FillPalette
=
=================
*/

void VL_FillPalette (int red, int green, int blue)
{
    int i;
    SDL_Color pal[256];

    for(i=0; i<256; i++)
    {
        pal[i].r = red;
        pal[i].g = green;
        pal[i].b = blue;
    }

    VL_SetPalette(pal, true);
}

//===========================================================================

/*
=================
=
= VL_SetColor
=
=================
*/

void VL_SetColor	(int color, int red, int green, int blue)
{
    SDL_Color col = { red, green, blue };
    curpal[color] = col;

    if(screenBits == 8)
        SDL_SetPalette(screen, SDL_PHYSPAL, &col, color, 1);
    else
    {
        SDL_SetPalette(curSurface, SDL_LOGPAL, &col, color, 1);
        SDL_BlitSurface(screenBuffer, NULL, screen, NULL);
        SDL_Flip(screen);
    }
}

//===========================================================================

/*
=================
=
= VL_GetColor
=
=================
*/

void VL_GetColor	(int color, int *red, int *green, int *blue)
{
    SDL_Color *col = &curpal[color];
    *red = col->r;
    *green = col->g;
    *blue = col->b;
}

//===========================================================================

/*
=================
=
= VL_SetPalette
=
=================
*/

void VL_SetPalette (SDL_Color *palette, bool forceupdate)
{
    memcpy(curpal, palette, sizeof(SDL_Color) * 256);

    if(screenBits == 8)
        SDL_SetPalette(screen, SDL_PHYSPAL, palette, 0, 256);
    else
    {
        SDL_SetPalette(curSurface, SDL_LOGPAL, palette, 0, 256);
        if(forceupdate)
        {
            SDL_BlitSurface(curSurface, NULL, screen, NULL);
                SDL_Flip(screen);
        }
    }
}


//===========================================================================

/*
=================
=
= VL_GetPalette
=
=================
*/

void VL_GetPalette (SDL_Color *palette)
{
    memcpy(palette, curpal, sizeof(SDL_Color) * 256);
}


//===========================================================================

/*
=================
=
= VL_FadeOut
=
= Fades the current palette to the given color in the given number of steps
=
=================
*/

void VL_FadeOut (int start, int end, int red, int green, int blue, int steps)
{
	int		    i,j,orig,delta;
	SDL_Color   *origptr, *newptr;

    red = red * 255 / 63;
    green = green * 255 / 63;
    blue = blue * 255 / 63;

	VL_WaitVBL(1);
	VL_GetPalette(palette1);
	memcpy(palette2, palette1, sizeof(SDL_Color) * 256);

//
// fade through intermediate frames
//
	for (i=0;i<steps;i++)
	{
		origptr = &palette1[start];
		newptr = &palette2[start];
		for (j=start;j<=end;j++)
		{
			orig = origptr->r;
			delta = red-orig;
			newptr->r = orig + delta * i / steps;
			orig = origptr->g;
			delta = green-orig;
			newptr->g = orig + delta * i / steps;
			orig = origptr->b;
			delta = blue-orig;
			newptr->b = orig + delta * i / steps;
			origptr++;
			newptr++;
		}

		if(!usedoublebuffering || screenBits == 8) VL_WaitVBL(1);
		VL_SetPalette (palette2, true);
	}

//
// final color
//
	VL_FillPalette (red,green,blue);

	screenfaded = true;
}


/*
=================
=
= VL_FadeIn
=
=================
*/

void VL_FadeIn (int start, int end, SDL_Color *palette, int steps)
{
	int i,j,delta;

	VL_WaitVBL(1);
	VL_GetPalette(palette1);
	memcpy(palette2, palette1, sizeof(SDL_Color) * 256);

//
// fade through intermediate frames
//
	for (i=0;i<steps;i++)
	{
		for (j=start;j<=end;j++)
		{
			delta = palette[j].r-palette1[j].r;
			palette2[j].r = palette1[j].r + delta * i / steps;
			delta = palette[j].g-palette1[j].g;
			palette2[j].g = palette1[j].g + delta * i / steps;
			delta = palette[j].b-palette1[j].b;
			palette2[j].b = palette1[j].b + delta * i / steps;
		}

		if(!usedoublebuffering || screenBits == 8) VL_WaitVBL(1);
		VL_SetPalette(palette2, true);
	}

//
// final color
//
	VL_SetPalette (palette, true);
	screenfaded = false;
}

/*
=============================================================================

							PIXEL OPS

=============================================================================
*/

byte *VL_LockSurface(SDL_Surface *surface)
{
    if(SDL_MUSTLOCK(surface))
    {
        if(SDL_LockSurface(surface) < 0)
            return NULL;
    }
    return (byte *) surface->pixels;
}

void VL_UnlockSurface(SDL_Surface *surface)
{
    if(SDL_MUSTLOCK(surface))
    {
        SDL_UnlockSurface(surface);
    }
}

/*
=================
=
= VL_Plot
=
=================
*/

void VL_Plot (int x, int y, int color)
{
    assert(x >= 0 && (unsigned) x < screenWidth
            && y >= 0 && (unsigned) y < screenHeight
            && "VL_Plot: Pixel out of bounds!");

    VL_LockSurface(curSurface);
	((byte *) curSurface->pixels)[y * curPitch + x] = color;
    VL_UnlockSurface(curSurface);
}

/*
=================
=
= VL_GetPixel
=
=================
*/

byte VL_GetPixel (SDL_Surface *surface, int x, int y)
{
    byte *ptr;
    byte col;
    
    assert_ret(x >= 0 && (unsigned) x < screenWidth
            && y >= 0 && (unsigned) y < screenHeight
            && "VL_GetPixel: Pixel out of bounds!");

    ptr = VL_LockSurface(surface);
    if (ptr == NULL) return 0;
    
	col = ((byte *) surface->pixels)[y * curPitch + x];
    
    VL_UnlockSurface(surface);
	return col;
}

/*
 ==============================
 =
 = SDL_DuplicateSurface
 =
 = Deep copies SDL_Surface
 =
 ==============================
 */
SDL_Surface* SDL_DuplicateSurface (SDL_Surface* surf)
{
    SDL_Surface* cpy;
    cpy = (SDL_Surface *)malloc(sizeof(SDL_Surface));
    memcpy((SDL_Surface *)cpy, (SDL_Surface *)surf, sizeof(SDL_Surface));
    cpy->format = (SDL_PixelFormat *)malloc(sizeof(SDL_PixelFormat));
    memcpy((SDL_PixelFormat *)cpy->format, (SDL_PixelFormat *)surf->format,
           sizeof(SDL_PixelFormat));
    size_t size = surf->pitch * surf->h;
    cpy->pixels = malloc(size);
    memcpy((Uint8 *)cpy->pixels, (Uint8 *)surf->pixels, size * sizeof(Uint8));
    return cpy;
}

/*
 ==============================
 =
 = DrawPixel
 =
 = Draws the pixel onto the surface
 =
 ==============================
 */
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    
    switch(bpp) {
        case 1:
            *p = pixel;
            break;
            
        case 2:
            *(Uint16 *)p = pixel;
            break;
            
        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;
            
        case 4:
            *(Uint32 *)p = pixel;
            break;
    }
}

/*
 =================================
 =
 = SDL_ScaleSurface
 =
 = Creates a surface to scaled width, then scales it accordingly
 = Width and Height can be any size, not multiples of 320x200
 =
 =================================
 */
SDL_Surface *SDL_ScaleSurface(SDL_Surface *Surface, Uint16 Width, Uint16 Height)
{
    if(!Surface || !Width || !Height)
        return 0;
    SDL_Surface *_ret = SDL_CreateRGBSurface(Surface->flags, Width, Height, 8, 0, 0, 0, 0);
    SDL_SetColors(_ret, gamepal, 0, 256);
    
    double    _stretch_factor_x = (static_cast<double>(Width)  / static_cast<double>(Surface->w)),
    _stretch_factor_y = (static_cast<double>(Height) / static_cast<double>(Surface->h));
    
    for(Sint32 y = 0; y < Surface->h; y++)
        for(Sint32 x = 0; x < Surface->w; x++)
            for(Sint32 o_y = 0; o_y < _stretch_factor_y; ++o_y)
                for(Sint32 o_x = 0; o_x < _stretch_factor_x; ++o_x)
                    DrawPixel(_ret, static_cast<Sint32>(_stretch_factor_x * x) + o_x,
                              static_cast<Sint32>(_stretch_factor_y * y) + o_y, VL_GetPixel(Surface, x, y));
    return _ret;
}

/*
 =================
 =
 = VL_SetSaveGameSlot
 =
 =================
 */
void VL_SetSaveGameSlot()
{
    lastGameSurface = SDL_ScaleSurface(SDL_DuplicateSurface(curSurface), 128, 80);
}


/*
=================
=
= VL_Hlin
=
=================
*/

void VL_Hlin (unsigned x, unsigned y, unsigned width, int color)
{
    assert(x >= 0 && x + width <= screenWidth
            && y >= 0 && y < screenHeight
            && "VL_Hlin: Destination rectangle out of bounds!");

    VL_LockSurface(curSurface);
    Uint8 *dest = ((byte *) curSurface->pixels) + y * curPitch + x;
    memset(dest, color, width);
    VL_UnlockSurface(curSurface);
}


/*
=================
=
= VL_Vlin
=
=================
*/

void VL_Vlin (int x, int y, int height, int color)
{
	assert(x >= 0 && (unsigned) x < screenWidth
			&& y >= 0 && (unsigned) y + height <= screenHeight
			&& "VL_Vlin: Destination rectangle out of bounds!");

	VL_LockSurface(curSurface);
	Uint8 *dest = ((byte *) curSurface->pixels) + y * curPitch + x;

	while (height--)
	{
		*dest = color;
		dest += curPitch;
	}
	VL_UnlockSurface(curSurface);
}


/*
=================
=
= VL_Bar
=
=================
*/

void VL_BarScaledCoord (int scx, int scy, int scwidth, int scheight, int color)
{
	assert(scx >= 0 && (unsigned) scx + scwidth <= screenWidth
			&& scy >= 0 && (unsigned) scy + scheight <= screenHeight
			&& "VL_BarScaledCoord: Destination rectangle out of bounds!");

	VL_LockSurface(curSurface);
	Uint8 *dest = ((byte *) curSurface->pixels) + scy * curPitch + scx;

	while (scheight--)
	{
		memset(dest, color, scwidth);
		dest += curPitch;
	}
	VL_UnlockSurface(curSurface);
}

/*
============================================================================

							MEMORY OPS

============================================================================
*/

/*
=================
=
= VL_MemToLatch
=
=================
*/

void VL_MemToLatch(byte *source, int width, int height,
    SDL_Surface *destSurface, int x, int y)
{
    assert(x >= 0 && (unsigned) x + width <= screenWidth
            && y >= 0 && (unsigned) y + height <= screenHeight
            && "VL_MemToLatch: Destination rectangle out of bounds!");

    VL_LockSurface(destSurface);
    int pitch = destSurface->pitch;
    byte *dest = (byte *) destSurface->pixels + y * pitch + x;
    for(int ysrc = 0; ysrc < height; ysrc++)
    {
        for(int xsrc = 0; xsrc < width; xsrc++)
        {
            dest[ysrc * pitch + xsrc] = source[(ysrc * (width >> 2) + (xsrc >> 2))
                + (xsrc & 3) * (width >> 2) * height];
        }
    }
    VL_UnlockSurface(destSurface);
}

//===========================================================================


/*
=================
=
= VL_MemToScreenScaledCoord
=
= Draws a block of data to the screen with scaling according to scaleFactor.
=
=================
*/

void VL_MemToScreenScaledCoord (byte *source, int width, int height, int destx, int desty)
{
    assert(destx >= 0 && destx + width * scaleFactor <= screenWidth
            && desty >= 0 && desty + height * scaleFactor <= screenHeight
            && "VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");

    VL_LockSurface(curSurface);
    byte *vbuf = (byte *) curSurface->pixels;
    for(int j=0,scj=0; j<height; j++, scj+=scaleFactor)
    {
        for(int i=0,sci=0; i<width; i++, sci+=scaleFactor)
        {
            byte col = source[(j*(width>>2)+(i>>2))+(i&3)*(width>>2)*height];
            for(unsigned m=0; m<scaleFactor; m++)
            {
                for(unsigned n=0; n<scaleFactor; n++)
                {
                    vbuf[(scj+m+desty)*curPitch+sci+n+destx] = col;
                }
            }
        }
    }
    VL_UnlockSurface(curSurface);
}

/*
=================
=
= VL_MemToScreenScaledCoord
=
= Draws a part of a block of data to the screen.
= The block has the size origwidth*origheight.
= The part at (srcx, srcy) has the size width*height
= and will be painted to (destx, desty) with scaling according to scaleFactor.
=
=================
*/

void VL_MemToScreenScaledCoord (byte *source, int origwidth, int origheight, int srcx, int srcy,
                                int destx, int desty, int width, int height)
{
    assert(destx >= 0 && destx + width * scaleFactor <= screenWidth
            && desty >= 0 && desty + height * scaleFactor <= screenHeight
            && "VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");

    VL_LockSurface(curSurface);
    byte *vbuf = (byte *) curSurface->pixels;
    for(int j=0,scj=0; j<height; j++, scj+=scaleFactor)
    {
        for(int i=0,sci=0; i<width; i++, sci+=scaleFactor)
        {
            byte col = source[((j+srcy)*(origwidth>>2)+((i+srcx)>>2))+((i+srcx)&3)*(origwidth>>2)*origheight];
            for(unsigned m=0; m<scaleFactor; m++)
            {
                for(unsigned n=0; n<scaleFactor; n++)
                {
                    vbuf[(scj+m+desty)*curPitch+sci+n+destx] = col;
                }
            }
        }
    }
    VL_UnlockSurface(curSurface);
}

//==========================================================================

/*
=================
=
= VL_LatchToScreen
=
=================
*/

void VL_LatchToScreenScaledCoord(SDL_Surface *source, int xsrc, int ysrc,
    int width, int height, int scxdest, int scydest)
{
	assert(scxdest >= 0 && scxdest + width * scaleFactor <= screenWidth
			&& scydest >= 0 && scydest + height * scaleFactor <= screenHeight
			&& "VL_LatchToScreenScaledCoord: Destination rectangle out of bounds!");

	if(scaleFactor == 1)
    {
        // HACK: If screenBits is not 8 and the screen is faded out, the
        //       result will be black when using SDL_BlitSurface. The reason
        //       is that the logical palette needed for the transformation
        //       to the screen color depth is not equal to the logical
        //       palette of the latch (the latch is not faded). Therefore,
        //       SDL tries to map the colors...
        //       The result: All colors are mapped to black.
        //       So, we do the blit on our own...
        if(screenBits != 8)
        {
            VL_LockSurface(source);
            byte *src = (byte *) source->pixels;
            unsigned srcPitch = source->pitch;

            VL_LockSurface(curSurface);
            byte *vbuf = (byte *) curSurface->pixels;
            for(int j=0,scj=0; j<height; j++, scj++)
            {
                for(int i=0,sci=0; i<width; i++, sci++)
                {
                    byte col = src[(ysrc + j)*srcPitch + xsrc + i];
                    vbuf[(scydest+scj)*curPitch+scxdest+sci] = col;
                }
            }
            VL_UnlockSurface(curSurface);
            VL_UnlockSurface(source);
        }
        else
        {
            SDL_Rect srcrect = { xsrc, ysrc, width, height };
            SDL_Rect destrect = { scxdest, scydest, 0, 0 }; // width and height are ignored
            SDL_BlitSurface(source, &srcrect, curSurface, &destrect);
        }
    }
    else
    {
        VL_LockSurface(source);
        byte *src = (byte *) source->pixels;
        unsigned srcPitch = source->pitch;

        VL_LockSurface(curSurface);
        byte *vbuf = (byte *) curSurface->pixels;
        for(int j=0,scj=0; j<height; j++, scj+=scaleFactor)
        {
            for(int i=0,sci=0; i<width; i++, sci+=scaleFactor)
            {
                byte col = src[(ysrc + j)*srcPitch + xsrc + i];
                for(unsigned m=0; m<scaleFactor; m++)
                {
                    for(unsigned n=0; n<scaleFactor; n++)
                    {
                        vbuf[(scydest+scj+m)*curPitch+scxdest+sci+n] = col;
                    }
                }
            }
        }
        VL_UnlockSurface(curSurface);
        VL_UnlockSurface(source);
    }
}

//===========================================================================

/*
=================
=
= VL_ScreenToScreen
=
=================
*/

void VL_ScreenToScreen (SDL_Surface *source, SDL_Surface *dest)
{
    SDL_BlitSurface(source, NULL, dest, NULL);
}
