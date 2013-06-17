// WL_GAME.C

#include <math.h>
#include "wl_def.h"
#include <SDL_mixer.h>
#pragma hdrstop

#ifdef MYPROFILE
#include <TIME.H>
#endif


/*
=============================================================================

                             LOCAL CONSTANTS

=============================================================================
*/


/*
=============================================================================

                             GLOBAL VARIABLES

=============================================================================
*/

boolean         ingame,fizzlein;
gametype        gamestate;
byte            bordercol=VIEWCOLOR;        // color of the Change View/Ingame border

#ifdef SPEAR
int32_t         spearx,speary;
unsigned        spearangle;
boolean         spearflag;
#endif

#ifdef USE_FEATUREFLAGS
int ffDataTopLeft, ffDataTopRight, ffDataBottomLeft, ffDataBottomRight;
#endif

//
// ELEVATOR BACK MAPS - REMEMBER (-1)!!
//
int ElevatorBackTo[]={2,2,2};

void SetupGameLevel (void);
void DrawPlayScreen (void);
void LoadLatchMem (void);
void LoadVendMem (void);
void GameLoop (void);

/*
=============================================================================

                             LOCAL VARIABLES

=============================================================================
*/



//===========================================================================
//===========================================================================


/*
==========================
=
= SetSoundLoc - Given the location of an object (in terms of global
=       coordinates, held in globalsoundx and globalsoundy), munges the values
=       for an approximate distance from the left and right ear, and puts
=       those values into leftchannel and rightchannel.
=
= JAB
=
==========================
*/

int leftchannel, rightchannel;
#define ATABLEMAX 15
byte righttable[ATABLEMAX][ATABLEMAX * 2] = {
{ 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 6, 0, 0, 0, 0, 0, 1, 3, 5, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 6, 4, 0, 0, 0, 0, 0, 2, 4, 6, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 4, 1, 0, 0, 0, 1, 2, 4, 6, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 6, 5, 4, 2, 1, 0, 1, 2, 3, 5, 7, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 5, 4, 3, 2, 2, 3, 3, 5, 6, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 6, 5, 4, 4, 4, 4, 5, 6, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 6, 5, 5, 5, 6, 6, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8}
};
byte lefttable[ATABLEMAX][ATABLEMAX * 2] = {
{ 8, 8, 8, 8, 8, 8, 8, 8, 5, 3, 1, 0, 0, 0, 0, 0, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 6, 4, 2, 0, 0, 0, 0, 0, 4, 6, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 6, 4, 2, 1, 0, 0, 0, 1, 4, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 7, 5, 3, 2, 1, 0, 1, 2, 4, 5, 6, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 6, 5, 3, 3, 2, 2, 3, 4, 5, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 6, 5, 4, 4, 4, 4, 5, 6, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 6, 6, 5, 5, 5, 6, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 6, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8}
};

void
SetSoundLoc(fixed gx,fixed gy)
{
    fixed   xt,yt;
    int     x,y;

//
// translate point to view centered coordinates
//
    gx -= viewx;
    gy -= viewy;

//
// calculate newx
//
    xt = FixedMul(gx,viewcos);
    yt = FixedMul(gy,viewsin);
    x = (xt - yt) >> TILESHIFT;

//
// calculate newy
//
    xt = FixedMul(gx,viewsin);
    yt = FixedMul(gy,viewcos);
    y = (yt + xt) >> TILESHIFT;

    if (y >= ATABLEMAX)
        y = ATABLEMAX - 1;
    else if (y <= -ATABLEMAX)
        y = -ATABLEMAX;
    if (x < 0)
        x = -x;
    if (x >= ATABLEMAX)
        x = ATABLEMAX - 1;
    leftchannel  =  lefttable[x][y + ATABLEMAX];
    rightchannel = righttable[x][y + ATABLEMAX];

#if 0
    CenterWindow(8,1);
    US_PrintSigned(leftchannel);
    US_Print(",");
    US_PrintSigned(rightchannel);
    VW_UpdateScreen();
#endif
}

/*
==========================
=
= SetSoundLocGlobal - Sets up globalsoundx & globalsoundy and then calls
=       UpdateSoundLoc() to transform that into relative channel volumes. Those
=       values are then passed to the Sound Manager so that they'll be used for
=       the next sound played (if possible).
=
= JAB
=
==========================
*/
void PlaySoundLocGlobal(word s,fixed gx,fixed gy)
{
    SetSoundLoc(gx, gy);
    SD_PositionSound(leftchannel, rightchannel);

    int channel = SD_PlaySound((soundnames) s);
    if(channel)
    {
        channelSoundPos[channel - 1].globalsoundx = gx;
        channelSoundPos[channel - 1].globalsoundy = gy;
        channelSoundPos[channel - 1].valid = 1;
    }
}

void UpdateSoundLoc(void)
{
/*    if (SoundPositioned)
    {
        SetSoundLoc(globalsoundx,globalsoundy);
        SD_SetPosition(leftchannel,rightchannel);
    }*/

    for(int i = 0; i < ASM_CurChannels(); i++)
    {
        if(channelSoundPos[i].valid)
        {
            SetSoundLoc(channelSoundPos[i].globalsoundx,
                channelSoundPos[i].globalsoundy);
            SD_SetPosition(i, leftchannel, rightchannel);
        }
    }
}

/*
**      JAB End
*/

/*
==========================
=
= ScanInfoPlane
=
= Spawn all actors and mark down special places
=
==========================
*/

static void ScanInfoPlane(void)
{
    unsigned x,y;
    int      tile;
    boolean  auroraspawned;
    int      numaurorasspots;
    word     *start;

    start = mapsegs[1];
    for (y=0;y<mapheight;y++)
    {
        for (x=0;x<mapwidth;x++)
        {
            tile = *start++;
            if (!tile)
                continue;

            switch (tile)
            {
                case 19:
                case 20:
                case 21:
                case 22:
                    SpawnPlayer(x,y,NORTH+tile-19);
                    break;

                case 23:
                case 24:
                case 25:
                case 26:
                case 27:
                case 28:
                case 29:
                case 30:

                case 31:
                case 32:
                case 33:
                case 34:
                case 35:
                case 36:
                case 37:
                case 38:

                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:

                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                    
                case 52:
                case 53:
                case 54:
                case 55:

                case 56:
                case 57:
                case 58:
                case 59:
                case 60:
                case 61:
                case 62:

                case 63:
                case 64:
                case 65:
                case 66:
                case 67:
                case 68:
                case 69:
                case 71:
                case 72:
                case 73:
                    SpawnStatic(x,y,tile-23);
                    break;
                    
                case 70:
                    SpawnLiveWires(x, y);
                    break;
                    
                case 80:
                    if (gamestate.difficulty < gd_hard)
                        SpawnAurora(x, y);
                    else
                    {
                        if (!auroraspawned)
                        {
                            if (US_RndT() % 2 || numaurorasspots == 7)
                            {
                                SpawnAurora(x, y);
                                auroraspawned = true;
                            }
                            else
                            {
                                numaurorasspots++;
                            }
                        }
                    }
                    break;
                    
                case 85:
                    if (!auroraspawned)
                    {
                        if (US_RndT() % 2 || numaurorasspots == 7)
                        {
                            SpawnAurora(x, y);
                            auroraspawned = true;
                        }
                        else
                        {
                            numaurorasspots++;
                        }
                    }
                    break;


//
// P wall
//
                case 98:
                    if (!loadedgame)
                        gamestate.secrettotal++;
                    break;

//
// guard
//
                case 180:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 36;
                case 144:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 36;
                case 108:
                    SpawnStand(en_guard,x,y,tile-108);
                    break;

                case 124:
                    SpawnDeadGuard (x,y);
                    break;
//
// officer
//
                case 188:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 36;
                case 152:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 36;
                case 116:
                    SpawnStand(en_officer,x,y,tile-116);
                    break;



//
// ss
//
                case 198:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 36;
                case 162:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 36;
                case 126:
                    SpawnStand(en_ss,x,y,tile-126);
                    break;

//
// boss
//
                case 214:
                    SpawnBoss (x,y);
                    break;
                case 196:
                    SpawnSchabbs (x,y);
                    break;
                case 178:
                    SpawnHitler (x,y);
                    break;

//
// mutants
//
                case 252:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 18;
                case 234:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 18;
                case 216:
                    SpawnStand(en_mutant,x,y,tile-216);
                    break;
                    
                    
                case 350:

                case 351:
                    
                case 352:
                case 353:
                case 354:
                case 355:
                    SpawnStatic(x, y, tile-299);
                    break;
                    
                case 358:
                case 359:
                    SpawnStatic(x, y, tile-301);
                    break;
                    
                case 360:
                    SpawnBreakableGlass(false, false, x, y);
                    break;
                case 361:
                    SpawnBreakableGlass(true, false, x, y);
                    break;
                case 362:
                    SpawnBreakableGlass(false, true, x, y);
                    break;
                case 363:
                    SpawnBreakableGlass(true, true, x, y);
                    break;
                    
                case 364:
                case 365:
                case 366:
                    SpawnStatic(x, y, tile-305);
                    break;
                    
                    
                    
                case 800:
                {
                    GetTagInfoForPWall(x, y, di_north);
                    break;
                }
                case 801:
                {
                    GetTagInfoForPWall(x, y, di_east);
                    break;
                }
                case 802:
                {
                    GetTagInfoForPWall(x, y, di_south);
                    break;
                }
                case 803:
                {
                    GetTagInfoForPWall(x, y, di_west);
                    break;
                }
                    
                case 850:
                {
                    SpawnPegasusDeviceControl(x, y);
                    break;
                }
                    
                case 851:
                {
                    SpawnPegasusDeviceSpots(x, y);
                    break;
                }
            }
        }
    }
}

/*
// Lowbyte is the effect the object on the channel has
// Highbyte is the tagging channel
// EFFECTS:
// 0 - NONE
// 1 - DOOR_OPEN
// 2 - DOOR_CLOSE
// 3 - PUSHWALL_NORTH
// 4 - PUSHWALL_EAST
// 5 - PUSHWALL_SOUTH
// 6 - PUSHWALL_WEST
void ApplyTaggingInfo (unsigned lowbyte, unsigned highbyte, unsigned tilespot)
{
    if (lowbyte == 
}

void ScanTaggingPlane (void)
{
    unsigned x,y;
    unsigned tile;
    word     *start;
    
    start = mapsegs[3];
    for (y=0;y<mapheight;y++)
    {
        for (x=0;x<mapwidth;x++)
        {
            tile = *start++;
            if (!tile)
                continue;
            
            unsigned ltilebyte = tile >> 8;
            unsigned utilebyte = tile & 0xff;
            
            if (ltilebyte && utilebyte)
                ApplyTaggingInfo(ltilebyte, htilebyte, tile);
            
        }
    }
} */

void ClearTaggingTables (void)
{
    for (int i = 0; i < MAXPWALLS; i++)
    {
        pwalls_[i].x = 0;
        pwalls_[i].y = 0;
        pwalls_[i].tag = 0;
    }
    
    lastpwall = &pwalls_[0];
    pwallnum = 0;
}

void GetTagInfoForPWall (int x, int y, controldir_t dir)
{
    int tag;
    
    tag = MAPSPOT(x, y, 3);
    if(tag > 0)
    {
        lastpwall->dir = dir;
        lastpwall->tag = tag;
        lastpwall->x = x;
        lastpwall->y = y;
        
        pwallnum++;
        lastpwall++;
    }
}

//==========================================================================

/*
==================
=
= SetupGameLevel
=
==================
*/

void SetupGameLevel (void)
{
    int  x,y;
    word *map;
    word tile;


    if (!loadedgame)
    {
        gamestate.TimeCount
            = gamestate.secrettotal
            = gamestate.killtotal
            = gamestate.treasuretotal
            = gamestate.secretcount
            = gamestate.killcount
            = gamestate.treasurecount
            = facetimes = 0;
            for (int i = 0; i < MAXPWALLS; i++)
            {
                pwall[i].active = false;
                pwall[i].state = 0;
                pwall[i].pos = 0;
            }
        LastAttacker = NULL;
        killerobj = NULL;
    }

    if (demoplayback || demorecord)
        US_InitRndT (false);
    else
        US_InitRndT (true);

//
// load the level
//
    CA_CacheMap (gamestate.mapon+6*gamestate.episode);
    mapon-=gamestate.episode*6;

#ifdef USE_FEATUREFLAGS
    // Temporary definition to make things clearer
    #define MXX MAPSIZE - 1

    // Read feature flags data from map corners and overwrite corners with adjacent tiles
    ffDataTopLeft     = MAPSPOT(0,   0,   0); MAPSPOT(0,   0,   0) = MAPSPOT(1,       0,       0);
    ffDataTopRight    = MAPSPOT(MXX, 0,   0); MAPSPOT(MXX, 0,   0) = MAPSPOT(MXX,     1,       0);
    ffDataBottomRight = MAPSPOT(MXX, MXX, 0); MAPSPOT(MXX, MXX, 0) = MAPSPOT(MXX - 1, MXX,     0);
    ffDataBottomLeft  = MAPSPOT(0,   MXX, 0); MAPSPOT(0,   MXX, 0) = MAPSPOT(0,       MXX - 1, 0);

    #undef MXX
#endif

//
// copy the wall data to a data segment array
//
    memset (tilemap,0,sizeof(tilemap));
    memset (actorat,0,sizeof(actorat));
    map = mapsegs[0];
    for (y=0;y<mapheight;y++)
    {
        for (x=0;x<mapwidth;x++)
        {
            tile = *map++;
            if (tile<AREATILE)
            {
                // solid wall
                tilemap[x][y] = (byte) tile;
                actorat[x][y] = (objtype *)(uintptr_t) tile;
            }
            else
            {
                // area floor
                tilemap[x][y] = 0;
                actorat[x][y] = 0;
            }
        }
    }

//
// spawn doors
//
    InitActorList ();                       // start spawning things with a clean slate
    InitDoorList ();
    InitStaticList ();
    ClearTaggingTables();

    map = mapsegs[0];
    for (y=0;y<mapheight;y++)
    {
        for (x=0;x<mapwidth;x++)
        {
            tile = *map++;
            if (tile >= 90 && tile <= 101)
            {
                // door
                switch (tile)
                {
                    case 90:
                    case 92:
                    case 94:
                    case 96:
                    case 98:
                    case 100:
                        SpawnDoor (x,y,1,(tile-90)/2);
                        break;
                    case 91:
                    case 93:
                    case 95:
                    case 97:
                    case 99:
                    case 101:
                        SpawnDoor (x,y,0,(tile-91)/2);
                        break;
                }
            }
        }
    }

//
// spawn actors
//
    ScanInfoPlane ();
    
    // ScanTaggingPlane();


//
// take out the ambush markers
//
    map = mapsegs[0];
    for (y=0;y<mapheight;y++)
    {
        for (x=0;x<mapwidth;x++)
        {
            tile = *map++;
            if (tile == AMBUSHTILE)
            {
                tilemap[x][y] = 0;
                if ( (unsigned)(uintptr_t)actorat[x][y] == AMBUSHTILE)
                    actorat[x][y] = NULL;

                if (*map >= AREATILE)
                    tile = *map;
                if (*(map-1-mapwidth) >= AREATILE)
                    tile = *(map-1-mapwidth);
                if (*(map-1+mapwidth) >= AREATILE)
                    tile = *(map-1+mapwidth);
                if ( *(map-2) >= AREATILE)
                    tile = *(map-2);

                *(map-1) = tile;
            }
        }
    }


//
// have the caching manager load and purge stuff to make sure all marks
// are in memory
//
    CA_LoadAllSounds ();
}


//==========================================================================


/*
===================
=
= DrawPlayBorderSides
=
= To fix window overwrites
=
===================
*/
void DrawPlayBorderSides(void)
{
    if(viewsize == 21) return;

	const int sw = screenWidth;
	const int sh = screenHeight;
	const int vw = viewwidth;
	const int vh = viewheight;
	const int px = scaleFactor; // size of one "pixel"

	const int h  = sh - px * STATUSLINES;
	const int xl = sw / 2 - vw / 2;
	const int yl = (h - vh) / 2;

    if(xl != 0)
    {
	    VWB_BarScaledCoord(0,            0, xl - px,     h, bordercol);                 // left side
	    VWB_BarScaledCoord(xl + vw + px, 0, xl - px * 2, h, bordercol);                 // right side
    }

    if(yl != 0)
    {
	    VWB_BarScaledCoord(0, 0,            sw, yl - px, bordercol);                    // upper side
	    VWB_BarScaledCoord(0, yl + vh + px, sw, yl - px, bordercol);                    // lower side
    }

    if(xl != 0)
    {
        // Paint game view border lines
	    VWB_BarScaledCoord(xl - px, yl - px, vw + px, px,          0);                      // upper border
	    VWB_BarScaledCoord(xl,      yl + vh, vw + px, px,          bordercol - 2);          // lower border
	    VWB_BarScaledCoord(xl - px, yl - px, px,      vh + px,     0);                      // left border
	    VWB_BarScaledCoord(xl + vw, yl - px, px,      vh + px * 2, bordercol - 2);          // right border
	    VWB_BarScaledCoord(xl - px, yl + vh, px,      px,          bordercol - 3);          // lower left highlight
    }
    else
    {
        // Just paint a lower border line
        VWB_BarScaledCoord(0, yl+vh, vw, px, bordercol-2);       // lower border
    }
}


/*
===================
=
= DrawStatusBorder
=
===================
*/

void DrawStatusBorder (byte color)
{
    int statusborderw = (screenWidth-scaleFactor*320)/2;

    VWB_BarScaledCoord (0,0,screenWidth,screenHeight-scaleFactor*(STATUSLINES-3),color);
    VWB_BarScaledCoord (0,screenHeight-scaleFactor*(STATUSLINES-3),
        statusborderw+scaleFactor*8,scaleFactor*(STATUSLINES-4),color);
    VWB_BarScaledCoord (0,screenHeight-scaleFactor*2,screenWidth,scaleFactor*2,color);
    VWB_BarScaledCoord (screenWidth-statusborderw-scaleFactor*8, screenHeight-scaleFactor*(STATUSLINES-3),
        statusborderw+scaleFactor*8,scaleFactor*(STATUSLINES-4),color);

    VWB_BarScaledCoord (statusborderw+scaleFactor*9, screenHeight-scaleFactor*3,
        scaleFactor*97, scaleFactor*1, color-1);
    VWB_BarScaledCoord (statusborderw+scaleFactor*106, screenHeight-scaleFactor*3,
        scaleFactor*161, scaleFactor*1, color-2);
    VWB_BarScaledCoord (statusborderw+scaleFactor*267, screenHeight-scaleFactor*3,
        scaleFactor*44, scaleFactor*1, color-3);
    VWB_BarScaledCoord (screenWidth-statusborderw-scaleFactor*9, screenHeight-scaleFactor*(STATUSLINES-4),
        scaleFactor*1, scaleFactor*20, color-2);
    VWB_BarScaledCoord (screenWidth-statusborderw-scaleFactor*9, screenHeight-scaleFactor*(STATUSLINES/2-4),
        scaleFactor*1, scaleFactor*14, color-3);
}


/*
===================
=
= DrawPlayBorder
=
===================
*/

void DrawPlayBorder (void)
{
	const int px = scaleFactor; // size of one "pixel"

    if (bordercol != VIEWCOLOR)
        DrawStatusBorder(bordercol);
    else
    {
        const int statusborderw = (screenWidth-px*320)/2;
        VWB_BarScaledCoord (0, screenHeight-px*STATUSLINES,
            statusborderw+px*8, px*STATUSLINES, bordercol);
        VWB_BarScaledCoord (screenWidth-statusborderw-px*8, screenHeight-px*STATUSLINES,
            statusborderw+px*8, px*STATUSLINES, bordercol);
    }

    if((unsigned) viewheight == screenHeight) return;

    VWB_BarScaledCoord (0,0,screenWidth,screenHeight-px*STATUSLINES,bordercol);

    const int xl = screenWidth/2-viewwidth/2;
    const int yl = (screenHeight-px*STATUSLINES-viewheight)/2;
    VWB_BarScaledCoord (xl,yl,viewwidth,viewheight,0);

    if(xl != 0)
    {
        // Paint game view border lines
        VWB_BarScaledCoord(xl-px, yl-px, viewwidth+px, px, 0);                      // upper border
        VWB_BarScaledCoord(xl, yl+viewheight, viewwidth+px, px, bordercol-2);       // lower border
        VWB_BarScaledCoord(xl-px, yl-px, px, viewheight+px, 0);                     // left border
        VWB_BarScaledCoord(xl+viewwidth, yl-px, px, viewheight+2*px, bordercol-2);  // right border
        VWB_BarScaledCoord(xl-px, yl+viewheight, px, px, bordercol-3);              // lower left highlight
    }
    else
    {
        // Just paint a lower border line
        VWB_BarScaledCoord(0, yl+viewheight, viewwidth, px, bordercol-2);       // lower border
    }
}


/*
===================
=
= DrawPlayScreen
=
===================
*/

void DrawPlayScreen (void)
{
    VWB_DrawPicScaledCoord ((screenWidth-scaleFactor*320)/2,screenHeight-scaleFactor*STATUSLINES,STATUSBARPIC);
    DrawPlayBorder ();

    DrawFace ();
    DrawHealth ();
    DrawLevel ();
    DrawAmmo ();
    DrawKeys ();
    // DrawWeapon ();
    DrawMoney();
}

// Uses LatchDrawPic instead of StatusDrawPic
void LatchNumberHERE (int x, int y, unsigned width, int32_t number)
{
    unsigned length,c;
    char str[20];

    ltoa (number,str,10);

    length = (unsigned) strlen (str);

    while (length<width)
    {
        LatchDrawPic (x,y,N_BLANKPIC);
        x++;
        width--;
    }

    c = length <= width ? 0 : length-width;

    while (c<length)
    {
        LatchDrawPic (x,y,str[c]-'0'+ N_0PIC);
        x++;
        c++;
    }
}

void ShowActStatus()
{
    // Draw status bar without borders
    byte *source = grsegs[STATUSBARPIC];
    int	picnum = STATUSBARPIC - STARTPICS;
    int width = pictable[picnum].width;
    int height = pictable[picnum].height;
    int destx = (screenWidth-scaleFactor*320)/2 + 9 * scaleFactor;
    int desty = screenHeight - (height - 4) * scaleFactor;
    VL_MemToScreenScaledCoord(source, width, height, 9, 4, destx, desty, width - 18, height - 7);

    ingame = false;
    DrawFace ();
    DrawHealth ();
    DrawLevel ();
    DrawAmmo ();
    DrawKeys ();
    // DrawWeapon ();
    DrawMoney();
    ingame = true;
}


//==========================================================================

/*
==================
=
= StartDemoRecord
=
==================
*/

char    demoname[13] = "DEMO?.";

#ifndef REMDEBUG
#define MAXDEMOSIZE     8192

void StartDemoRecord (int levelnumber)
{
    demobuffer=malloc(MAXDEMOSIZE);
    CHECKMALLOCRESULT(demobuffer);
    demoptr = (int8_t *) demobuffer;
    lastdemoptr = demoptr+MAXDEMOSIZE;

    *demoptr = levelnumber;
    demoptr += 4;                           // leave space for length
    demorecord = true;
}


/*
==================
=
= FinishDemoRecord
=
==================
*/

void FinishDemoRecord (void)
{
    int32_t    length,level;

    demorecord = false;

    length = (int32_t) (demoptr - (int8_t *)demobuffer);

    demoptr = ((int8_t *)demobuffer)+1;
    demoptr[0] = (int8_t) length;
    demoptr[1] = (int8_t) (length >> 8);
    demoptr[2] = 0;

    VW_FadeIn();
    CenterWindow(24,3);
    PrintY+=6;
    fontnumber=0;
    SETFONTCOLOR(0,15);
    US_Print(" Demo number (0-9): ");
    VW_UpdateScreen();

    if (US_LineInput (px,py,str,NULL,true,1,0))
    {
        level = atoi (str);
        if (level>=0 && level<=9)
        {
            demoname[4] = (char)('0'+level);
            CA_WriteFile (demoname,demobuffer,length);
        }
    }

    free(demobuffer);
}

//==========================================================================

/*
==================
=
= RecordDemo
=
= Fades the screen out, then starts a demo.  Exits with the screen faded
=
==================
*/

void RecordDemo (void)
{
    int level,esc,maps;

    CenterWindow(26,3);
    PrintY+=6;
    CA_CacheGrChunk(STARTFONT);
    fontnumber=0;
    SETFONTCOLOR(0,15);
#ifndef SPEAR
#ifdef UPLOAD
    US_Print("  Demo which level(1-10): "); maps = 10;
#else
    US_Print("  Demo which level(1-60): "); maps = 60;
#endif
#else
    US_Print("  Demo which level(1-21): "); maps = 21;
#endif
    VW_UpdateScreen();
    VW_FadeIn ();
    esc = !US_LineInput (px,py,str,NULL,true,2,0);
    if (esc)
        return;

    level = atoi (str);
    level--;

    if (level >= maps || level < 0)
        return;

    VW_FadeOut ();

#ifndef SPEAR
    NewGame (gd_hard,level/10);
    gamestate.mapon = level%10;
#else
    NewGame (gd_hard,0);
    gamestate.mapon = level;
#endif

    StartDemoRecord (level);

    DrawPlayScreen ();
    VW_FadeIn ();

    startgame = false;
    demorecord = true;

    SetupGameLevel ();
    StartMusic ();

    if(usedoublebuffering) VH_UpdateScreen();
    fizzlein = true;

    PlayLoop ();

    demoplayback = false;

    StopMusic ();
    VW_FadeOut ();
    ClearMemory ();

    FinishDemoRecord ();
}
#else
void FinishDemoRecord (void) {return;}
void RecordDemo (void) {return;}
#endif



//==========================================================================

/*
==================
=
= PlayDemo
=
= Fades the screen out, then starts a demo.  Exits with the screen unfaded
=
==================
*/

void PlayDemo (int demonumber)
{
    return;
    /* int length;
#ifdef DEMOSEXTERN
// debug: load chunk
#ifndef SPEARDEMO
    // int dems[4]={T_DEMO0,T_DEMO1,T_DEMO2,T_DEMO3};
#else
    int dems[1]={T_DEMO0};
#endif

    // CA_CacheGrChunk(dems[demonumber]);
    // demoptr = (int8_t *) grsegs[dems[demonumber]];
#else
    demoname[4] = '0'+demonumber;
    CA_LoadFile (demoname,&demobuffer);
    demoptr = (int8_t *)demobuffer;
#endif

    NewGame (1,0);
    gamestate.mapon = *demoptr++;
    gamestate.difficulty = gd_hard;
    length = READWORD(*(uint8_t **)&demoptr);
    // TODO: Seems like the original demo format supports 16 MB demos
    //       But T_DEM00 and T_DEM01 of Wolf have a 0xd8 as third length size...
    demoptr++;
    lastdemoptr = demoptr-4+length;

    VW_FadeOut ();

    SETFONTCOLOR(0,15);
    DrawPlayScreen ();

    startgame = false;
    demoplayback = true;

    SetupGameLevel ();
    StartMusic ();

    PlayLoop ();

#ifdef DEMOSEXTERN
    // UNCACHEGRCHUNK(dems[demonumber]);
#else
    MM_FreePtr (&demobuffer);
#endif

    demoplayback = false;

    StopMusic ();
    ClearMemory ();
     */
}

//==========================================================================

/*
==================
=
= Died
=
==================
*/

#define DEATHROTATE 2

void Died (void)
{
    float   fangle;
    int32_t dx,dy;
    int     iangle,curangle,clockwise,counter,change;

    if (screenfaded)
    {
        ThreeDRefresh ();
        VW_FadeIn ();
    }

    gamestate.weapon = (weapontype) -1;                     // take away weapon
    // SD_PlaySound (PLAYERDEATHSND);

    //
    // swing around to face attacker
    //
    if(killerobj)
    {
        dx = killerobj->x - player->x;
        dy = player->y - killerobj->y;

        fangle = (float) atan2((float) dy, (float) dx);     // returns -pi to pi
        if (fangle<0)
            fangle = (float) (M_PI*2+fangle);

        iangle = (int) (fangle/(M_PI*2)*ANGLES);
    }
    else
    {
        iangle = player->angle + ANGLES / 2;
        if(iangle >= ANGLES) iangle -= ANGLES;
    }

    if (player->angle > iangle)
    {
        counter = player->angle - iangle;
        clockwise = ANGLES-player->angle + iangle;
    }
    else
    {
        clockwise = iangle - player->angle;
        counter = player->angle + ANGLES-iangle;
    }

    curangle = player->angle;

    if (clockwise<counter)
    {
        //
        // rotate clockwise
        //
        if (curangle>iangle)
            curangle -= ANGLES;
        do
        {
            change = tics*DEATHROTATE;
            if (curangle + change > iangle)
                change = iangle-curangle;

            curangle += change;
            player->angle += change;
            if (player->angle >= ANGLES)
                player->angle -= ANGLES;

            ThreeDRefresh ();
            CalcTics ();
        } while (curangle != iangle);
    }
    else
    {
        //
        // rotate counterclockwise
        //
        if (curangle<iangle)
            curangle += ANGLES;
        do
        {
            change = -(int)tics*DEATHROTATE;
            if (curangle + change < iangle)
                change = iangle-curangle;

            curangle += change;
            player->angle += change;
            if (player->angle < 0)
                player->angle += ANGLES;

            ThreeDRefresh ();
            CalcTics ();
        } while (curangle != iangle);
    }

    //
    // fade to red
    //
    FinishPaletteShifts ();

    if(usedoublebuffering) VH_UpdateScreen();

    VL_BarScaledCoord (viewscreenx,viewscreeny,viewwidth,viewheight,4);

    IN_ClearKeysDown ();

    FizzleFade(screenBuffer,viewscreenx,viewscreeny,viewwidth,viewheight,70,false);

    IN_UserInput(100);
    SD_WaitSoundDone ();
    ClearMemory();
    
        gamestate.health = 100;
        gamestate.weapon = gamestate.bestweapon
            = gamestate.chosenweapon = wp_pistol;
        gamestate.ammo = STARTAMMO;
        gamestate.ammo2 = 0;
        gamestate.keys = 0;
        gamestate.money = 0;
        for (int i = 0; i < MAXPWALLS; i++)
        {
            pwall[i].active = false;
            pwall[i].state = 0;
            pwall[i].pos = 0;
        }
        gamestate.attackframe = gamestate.attackcount =
            gamestate.weaponframe = 0;
    
    gamestate.gotchaingun = false;
    gamestate.gotmachinegun = false;
    gamestate.gotrainbowergun = false;

        /* if(viewsize != 21)
        {
            DrawKeys ();
            // DrawWeapon ();
            DrawMoney();
            DrawAmmo ();
            DrawHealth ();
            DrawFace ();
        } */
}

//==========================================================================

/*
===================
=
= GameLoop
=
===================
*/

void GameLoop (void)
{
    boolean died;
#ifdef MYPROFILE
    clock_t start,end;
#endif

restartgame:
    ClearMemory ();
    SETFONTCOLOR(0,15);
    // VW_FadeOut();
    // DrawPlayScreen ();
    died = false;
    do
    {
        startgame = false;
        if (!loadedgame)
            SetupGameLevel ();

#ifdef SPEAR
        if (gamestate.mapon == 20)      // give them the key allways
        {
            gamestate.keys |= 1;
            DrawKeys ();
        }
#endif

        ingame = true;
        if(loadedgame)
        {
            ContinueMusic(lastgamemusicoffset);
            loadedgame = false;
        }
        else StartMusic ();

        PreloadGraphics ();
        //   if (!died)
        //            PreloadGraphics ();             // TODO: Let this do something useful!
        //        else
        if (died)
        {
            died = false;
            fizzlein = true;
        }

        DrawLevel ();

#ifdef SPEAR
startplayloop:
#endif
        PlayLoop ();

#ifdef SPEAR
        if (spearflag)
        {
            SD_StopSound();
            SD_PlaySound(GETSPEARSND);
            if (DigiMode != sds_Off)
            {
                Delay(150);
            }
            else
                SD_WaitSoundDone();

            ClearMemory ();
            gamestate.oldscore = gamestate.score;
            gamestate.mapon = 20;
            SetupGameLevel ();
            StartMusic ();
            player->x = spearx;
            player->y = speary;
            player->angle = (short)spearangle;
            spearflag = false;
            Thrust (0,0);
            goto startplayloop;
        }
#endif

        StopMusic ();
        ingame = false;

        if (demorecord && playstate != ex_warped)
            FinishDemoRecord ();

        if (startgame || loadedgame)
            goto restartgame;

        switch (playstate)
        {
            case ex_completed:
            case ex_secretlevel:
                if(viewsize == 21) DrawPlayScreen();
                gamestate.keys = 0;
                DrawKeys ();
                VW_FadeOut ();

                ClearMemory ();

                LevelCompleted ();              // do the intermission
                if(viewsize == 21) DrawPlayScreen();

#ifdef SPEARDEMO
                if (gamestate.mapon == 1)
                {
                    died = true;                    // don't "get psyched!"

                    VW_FadeOut ();

                    ClearMemory ();

                    CheckHighScore (gamestate.score,gamestate.mapon+1);
#ifndef JAPAN
                    strcpy(MainMenu[viewscores].string,STR_VS);
#endif
                    MainMenu[viewscores].routine = CP_ViewScores;
                    return;
                }
#endif

#ifdef JAPDEMO
                if (gamestate.mapon == 3)
                {
                    died = true;                    // don't "get psyched!"

                    VW_FadeOut ();

                    ClearMemory ();

                    CheckHighScore (gamestate.score,gamestate.mapon+1);
#ifndef JAPAN
                    strcpy(MainMenu[viewscores].string,STR_VS);
#endif
                    MainMenu[viewscores].routine = CP_ViewScores;
                    return;
                }
#endif


#ifndef SPEAR
                //
                // COMING BACK FROM SECRET LEVEL
                //
                if (gamestate.mapon == 5)
                    gamestate.mapon = ElevatorBackTo[gamestate.episode];    // back from secret
                else
                    //
                    // GOING TO SECRET LEVEL
                    //
                    if (playstate == ex_secretlevel)
                        gamestate.mapon = 5;
#else

#define FROMSECRET1             3
#define FROMSECRET2             11

                //
                // GOING TO SECRET LEVEL
                //
                if (playstate == ex_secretlevel)
                    switch(gamestate.mapon)
                {
                    case FROMSECRET1: gamestate.mapon = 18; break;
                    case FROMSECRET2: gamestate.mapon = 19; break;
                }
                else
                    //
                    // COMING BACK FROM SECRET LEVEL
                    //
                    if (gamestate.mapon == 18 || gamestate.mapon == 19)
                        switch(gamestate.mapon)
                    {
                        case 18: gamestate.mapon = FROMSECRET1+1; break;
                        case 19: gamestate.mapon = FROMSECRET2+1; break;
                    }
#endif
                    else
                        //
                        // GOING TO NEXT LEVEL
                        //
                        gamestate.mapon++;
                break;

            case ex_died:
                Died ();
                died = true;                    // don't "get psyched!"
                
                
                if (gamestate.mapon == 4 && gamestate.episode == 2)
                {
                    gamestate.endingmode = 0;
                    ClearMemory ();
                    
                    Victory ();
                    
                    ClearMemory ();
                    
#ifndef JAPAN
                    strcpy(MainMenu[viewachievements].string,STR_VS);
#endif
                    MainMenu[viewachievements].routine = CP_ViewAchievements;
                    return;
                }
                    
                
                    break;                          // more lives left

                VW_FadeOut ();
                if(screenHeight % 200 != 0)
                    VL_ClearScreen(0);

#ifdef _arch_dreamcast
                DC_StatusClearLCD();
#endif

                ClearMemory ();

#ifndef JAPAN
                strcpy(MainMenu[viewachievements].string,STR_VS);
#endif
                MainMenu[viewachievements].routine = CP_ViewAchievements;
                return;

            case ex_victorious:
                if(viewsize == 21) DrawPlayScreen();
#ifndef SPEAR
                VW_FadeOut ();
#else
                VL_FadeOut (0,255,0,17,17,300);
#endif
                ClearMemory ();
                
                Victory ();

                ClearMemory ();

#ifndef JAPAN
                strcpy(MainMenu[viewachievements].string,STR_VS);
#endif
                MainMenu[viewachievements].routine = CP_ViewAchievements;
                return;
                
            default:
                if(viewsize == 21) DrawPlayScreen();
                ClearMemory ();
                break;
        }
    } while (1);
}
