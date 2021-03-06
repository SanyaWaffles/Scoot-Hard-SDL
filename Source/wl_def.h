#ifndef WL_DEF_H
#define WL_DEF_H

// Defines which version shall be built and configures supported extra features
#include "version.h"

#include <assert.h>
#include <fcntl.h>
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(_arch_dreamcast)
#	include <string.h>
#	include "dc/dc_main.h"
#elif !defined(_WIN32)
#	include <stdint.h>
#	include <string.h>
#	include <stdarg.h>
#endif
#include <SDL.h>

#if !defined O_BINARY
#	define O_BINARY 0
#endif

#pragma pack(1)

#if defined(_arch_dreamcast)
#define YESBUTTONNAME "A"
#define NOBUTTONNAME  "B"
#elif defined(GP2X)
#define YESBUTTONNAME "Y"
#define NOBUTTONNAME  "B"
#else
#define YESBUTTONNAME "Y"
#define NOBUTTONNAME  "N"
#endif

#include "foreign.h"

#ifndef SPEAR
    #include "audiowl6.h"
    #ifdef UPLOAD
        #include "gfxv_apo.h"
    #else
            #include "gfxv_wl6.h"
    #endif
#else
    #include "audiosod.h"
    #include "gfxv_sod.h"
    #include "f_spear.h"
#endif

typedef uint8_t byte;
typedef uint16_t word;
typedef int32_t fixed;
typedef uint32_t longword;
typedef int8_t boolean;
typedef void * memptr;

typedef struct
{
    int x,y;
} Point;
typedef struct
{
    Point ul,lr;
} Rect;

void Quit(const char *errorStr, ...);

#include "id_pm.h"
#include "asound/asmcomp.h"
#include "id_in.h"
#include "id_vl.h"
#include "id_vh.h"
#include "id_us.h"
#include "id_ca.h"

#include "wl_menu.h"

#define MAPSPOT(x,y,plane) (mapsegs[plane][((y)<<mapshift)+(x)])

#define SIGN(x)         ((x)>0?1:-1)
#define ABS(x)          ((int)(x)>0?(x):-(x))
#define LABS(x)         ((int32_t)(x)>0?(x):-(x))

#define abs(x) ABS(x)

/*
=============================================================================

                            GLOBAL CONSTANTS

=============================================================================
*/

#define MAXTICS 10
#define DEMOTICS        4

#define MAXACTORS       300         // max number of nazis, etc / map
#define MAXSTATS        400         // max number of lamps, bonus, etc
#define MAXDOORS        64          // max number of sliding doors
#define MAXWALLTILES    64          // max number of wall tiles

//
// tile constants
//

#define ICONARROWS      90
#define PUSHABLETILE    98
#define EXITTILE        99          // at end of castle
#define GOODEXITTILE    86          // good end of game
#define AREATILE        107         // first of NUMAREAS floor tiles
#define NUMAREAS        37
#define ELEVATORTILE    21
#define AMBUSHTILE      106
#define ALTELEVATORTILE 107

#define NUMBERCHARS     9


//----------------

#define EXTRAPOINTS     40000

#define PLAYERSPEED     3000
#define RUNSPEED        6000

#define SCREENSEG       0xa000

#define SCREENBWIDE     80

#define HEIGHTRATIO     0.50            // also defined in id_mm.c

#define BORDERCOLOR     3
#define FLASHCOLOR      5
#define FLASHTICS       4

#ifndef SPEAR
    #define LRpack      4       // # of levels to store in endgame
#else
    #define LRpack      20
#endif

#define PLAYERSIZE      MINDIST         // player radius
#define MINACTORDIST    0x10000l        // minimum dist from player center
                                        // to any actor center

#define NUMLATCHPICS    100

#undef M_PI
#define PI              3.141592657
#define M_PI PI

#define GLOBAL1         (1l<<16)
#define TILEGLOBAL      GLOBAL1
#define PIXGLOBAL       (GLOBAL1/64)
#define TILESHIFT       16l
#define UNSIGNEDSHIFT   8

#define ANGLES          360             // must be divisable by 4
#define ANGLEQUAD       (ANGLES/4)
#define FINEANGLES      3600
#define ANG90           (FINEANGLES/4)
#define ANG180          (ANG90*2)
#define ANG270          (ANG90*3)
#define ANG360          (ANG90*4)
#define VANG90          (ANGLES/4)
#define VANG180         (VANG90*2)
#define VANG270         (VANG90*3)
#define VANG360         (VANG90*4)

#define MINDIST         (0x5800l)

#define mapshift        6
#define MAPSIZE         (1<<mapshift)
#define maparea         MAPSIZE*MAPSIZE

#define mapheight       MAPSIZE
#define mapwidth        MAPSIZE

#ifdef USE_HIRES

#define TEXTURESHIFT    7
#define TEXTURESIZE     (1<<TEXTURESHIFT)
#define TEXTUREFROMFIXEDSHIFT 2
#define TEXTUREMASK     (TEXTURESIZE*(TEXTURESIZE-1))

#define SPRITESCALEFACTOR 1

#else

#define TEXTURESHIFT    6
#define TEXTURESIZE     (1<<TEXTURESHIFT)
#define TEXTUREFROMFIXEDSHIFT 4
#define TEXTUREMASK     (TEXTURESIZE*(TEXTURESIZE-1))

#define SPRITESCALEFACTOR 2

#endif

#define NORTH   0
#define EAST    1
#define SOUTH   2
#define WEST    3


#define STATUSLINES     40

#define SCREENSIZE      (SCREENBWIDE*208)
#define PAGE1START      0
#define PAGE2START      (SCREENSIZE)
#define PAGE3START      (SCREENSIZE*2u)
#define FREESTART       (SCREENSIZE*3u)


#define PIXRADIUS       512

#define STARTAMMO       8


// object flag values

typedef enum
{
    FL_SHOOTABLE        = 0x00000001,
    FL_BONUS            = 0x00000002,
    FL_NEVERMARK        = 0x00000004,
    FL_VISABLE          = 0x00000008,
    FL_ATTACKMODE       = 0x00000010,
    FL_FIRSTATTACK      = 0x00000020,
    FL_AMBUSH           = 0x00000040,
    FL_NONMARK          = 0x00000080,
    FL_FULLBRIGHT       = 0x00000100,
#ifdef USE_DIR3DSPR
    // you can choose one of the following values in wl_act1.cpp
    // to make a static sprite a directional 3d sprite
    // (see example at the end of the statinfo array)
    FL_DIR_HORIZ_MID    = 0x00000200,
    FL_DIR_HORIZ_FW     = 0x00000400,
    FL_DIR_HORIZ_BW     = 0x00000600,
    FL_DIR_VERT_MID     = 0x00000a00,
    FL_DIR_VERT_FW      = 0x00000c00,
    FL_DIR_VERT_BW      = 0x00000e00,

    // these values are just used to improve readability of code
    FL_DIR_NONE         = 0x00000000,
    FL_DIR_POS_MID      = 0x00000200,
    FL_DIR_POS_FW       = 0x00000400,
    FL_DIR_POS_BW       = 0x00000600,
    FL_DIR_POS_MASK     = 0x00000600,
    FL_DIR_VERT_FLAG    = 0x00000800,
    FL_DIR_MASK         = 0x00000e00,
#endif
    FL_COMP_TERM        = 0x00001000,
    // next free bit is   0x00001000
} objflag_t;


//
// sprite constants
//

enum
{
    SPR_DEMO,

    SPR_DEATHCAM,

//
// static sprites
//
    SPR_STAT_0,SPR_STAT_1,SPR_STAT_2,SPR_STAT_3,
    
    SPR_STAT_4,
    
    SPR_STAT_5a, SPR_STAT_5b, SPR_STAT_5c, SPR_STAT_5d,
    SPR_STAT_5e,
    
    SPR_STAT_6,SPR_STAT_7,

    SPR_STAT_8,SPR_STAT_9,SPR_STAT_10,SPR_STAT_11,
    SPR_STAT_12,SPR_STAT_13,SPR_STAT_14,SPR_STAT_15,

    SPR_STAT_16,SPR_STAT_17,SPR_STAT_18,SPR_STAT_19,
    SPR_STAT_20,SPR_STAT_21,SPR_STAT_22,SPR_STAT_23,

    SPR_STAT_24,SPR_STAT_25,SPR_STAT_26,SPR_STAT_27,
    SPR_STAT_28,SPR_STAT_29,SPR_STAT_30,SPR_STAT_31,

    SPR_STAT_32,SPR_STAT_33,SPR_STAT_34,SPR_STAT_35,
    SPR_STAT_36,SPR_STAT_37,SPR_STAT_38,SPR_STAT_39,

    SPR_STAT_40,SPR_STAT_41,SPR_STAT_42,SPR_STAT_43,
    SPR_STAT_44,
    
    SPR_STAT_45a, SPR_STAT_45b,
    
    SPR_STAT_46,
    
    SPR_STAT_47a, SPR_STAT_47b, SPR_STAT_47c, SPR_STAT_47d,
    SPR_STAT_47e,
    
    SPR_STAT_48,SPR_STAT_49,SPR_STAT_50,
    
    SPR_GLASS_A,
    
    SPR_GLASS_CRACK_A,
    SPR_GLASS_CRACK_B,
    SPR_GLASS_BROKEN,
    SPR_GLASS_BREAK_A,
    SPR_GLASS_BREAK_B,
    SPR_GLASS_BREAK_C,
    
    SPR_MESH_A,
    SPR_MESH_B,
    SPR_MESH_C,
    
    SPR_AURORA_DAWN,

//
// guard
//
    SPR_GRD_S_1,

    SPR_GRD_W1_1,

    SPR_GRD_W2_1,

    SPR_GRD_W3_1,

    SPR_GRD_W4_1,

    SPR_GRD_PAIN_1,SPR_GRD_DIE_1,SPR_GRD_DIE_2,SPR_GRD_DIE_3,
    SPR_GRD_PAIN_2,SPR_GRD_DEAD,

    SPR_GRD_SHOOT1,SPR_GRD_SHOOT2,SPR_GRD_SHOOT3,



//
// ss
//
    SPR_SS_S_1,

    SPR_SS_W1_1,

    SPR_SS_W2_1,

    SPR_SS_W3_1,

    SPR_SS_W4_1,

    SPR_SS_PAIN_1,SPR_SS_DIE_1,SPR_SS_DIE_2,SPR_SS_DIE_3,
    SPR_SS_PAIN_2,SPR_SS_DEAD,

    SPR_SS_SHOOT1,SPR_SS_SHOOT2,SPR_SS_SHOOT3,

//
// mutant
//
    SPR_MUT_S_1,

    SPR_MUT_W1_1,

    SPR_MUT_W2_1,

    SPR_MUT_W3_1,

    SPR_MUT_W4_1,

    SPR_MUT_PAIN_1,SPR_MUT_DIE_1,SPR_MUT_DIE_2,SPR_MUT_DIE_3,
    SPR_MUT_PAIN_2,SPR_MUT_DIE_4,SPR_MUT_DEAD,

    SPR_MUT_SHOOT1,SPR_MUT_SHOOT2,SPR_MUT_SHOOT3,SPR_MUT_SHOOT4,

//
// officer
//
    SPR_OFC_S_1,

    SPR_OFC_W1_1,

    SPR_OFC_W2_1,

    SPR_OFC_W3_1,

    SPR_OFC_W4_1,

    SPR_OFC_PAIN_1,SPR_OFC_DIE_1,SPR_OFC_DIE_2,SPR_OFC_DIE_3,
    SPR_OFC_PAIN_2,SPR_OFC_DIE_4,SPR_OFC_DEAD,

    SPR_OFC_SHOOT1,SPR_OFC_SHOOT2,SPR_OFC_SHOOT3,
    SPR_OFC_VEND1,

#ifndef SPEAR

//
// hans
//
    SPR_BOSS_W1,SPR_BOSS_W2,SPR_BOSS_W3,SPR_BOSS_W4,
    SPR_BOSS_SHOOT1,SPR_BOSS_SHOOT2,SPR_BOSS_SHOOT3,SPR_BOSS_DEAD,

    SPR_BOSS_DIE1,SPR_BOSS_DIE2,SPR_BOSS_DIE3,

//
// schabbs
//
    SPR_SCHABB_W1,SPR_SCHABB_W2,SPR_SCHABB_W3,SPR_SCHABB_W4,
    SPR_SCHABB_SHOOT1,SPR_SCHABB_SHOOT2,

    SPR_SCHABB_DIE1,SPR_SCHABB_DIE2,SPR_SCHABB_DIE3,SPR_SCHABB_DEAD,
    SPR_HYPO1,SPR_HYPO2,SPR_HYPO3,SPR_HYPO4,
    
    SPR_FIRE1,SPR_FIRE2,

//
// g4 possessed / g3 dash
//
    SPR_G4RDPOSS_W1,SPR_G4RDPOSS_W2,SPR_G4RDPOSS_W3,SPR_G4RDPOSS_W4,
    SPR_G4RDPOSS_SHOOT1,SPR_G4RDPOSS_SHOOT2,SPR_G4RDPOSS_SHOOT3,

    SPR_G4DASH_TELEPORT,

    SPR_G4RDASH_DIE1, SPR_G4RDASH_DIE2, SPR_G4RDASH_DIE3, SPR_G4RDASH_DEAD,
    
    SPR_TELPORT_F1, SPR_TELEPORT_F2,
#endif
//
// Rocket, smoke and small explosion
//
    SPR_ROCKET_1,

    SPR_SMOKE_1,

//
// bj
//
    SPR_BJ_W1, SPR_BJ_W2,SPR_BJ_W3,SPR_BJ_W4,
    SPR_BJ_JUMP1,SPR_BJ_JUMP2,SPR_BJ_JUMP3,SPR_BJ_JUMP4,
    
    SPR_EXPLOSION_A,
    SPR_EXPLOSION_B,
    SPR_EXPLOSION_C,
    SPR_EXPLOSION_D,
    SPR_EXPLOSION_E,

//
// player attack frames
//
    SPR_KNIFEREADY,SPR_KNIFEATK1,SPR_KNIFEATK2,SPR_KNIFEATK3,
    SPR_KNIFEATK4,

    SPR_PISTOLREADY,SPR_PISTOLATK1,SPR_PISTOLATK2,SPR_PISTOLATK3,
    SPR_PISTOLATK4,

    SPR_MACHINEGUNREADY,SPR_MACHINEGUNATK1,SPR_MACHINEGUNATK2,MACHINEGUNATK3,
    SPR_MACHINEGUNATK4,

    SPR_CHAINREADY,SPR_CHAINATK1,SPR_CHAINATK2,SPR_CHAINATK3,
    SPR_CHAINATK4,
    
    SPR_RTHROWREADY, SPR_RTHROWATK1, SPR_RTHROWATK2, SPR_RTHROWATK3,
    SPR_RTHROWATK4,

};


/*
=============================================================================

                               GLOBAL TYPES

=============================================================================
*/

typedef enum {
    di_north,
    di_east,
    di_south,
    di_west
} controldir_t;

typedef enum {
    dr_normal,
    dr_lock1,
    dr_lock2,
    dr_lock3,
    dr_lock4,
    dr_elevator,
    dr_switchopd
} door_t;

typedef enum {
    ac_badobject = -1,
    ac_no,
    ac_yes,
    ac_allways
} activetype;

typedef enum {
    nothing,
    playerobj,
    inertobj,
    guardobj,
    officerobj,
    ssobj,
    dogobj,
    bossobj,
    schabbobj,
    fakeobj,
    g4dashobj,
    mutantobj,
    needleobj,
    fireobj,
    bjobj,
    ghostobj,
    rocketobj,
    compobj,
    p_rainballobj,
    nglassobj,
    glassobj,
    lwireobj,
    pdevobj,            // 1 Hit Point Increase for each switch flipped. Open door tagged 666
                        // when hit points >= 3
    pdevmspotobj,       // Remove walls, spawn explosions (tagged 667)
    pdevsplodeobj,       // explosion object
    auroraobj
} classtype;

typedef enum {
    none,
    block,
    bo_gibs,
    bo_alpo,
    bo_firstaid,
    bo_key1,
    bo_key2,
    bo_key3,
    bo_key4,
    bo_rainbower,
    bo_rbowtank,
    bo_clip,
    bo_clip2,
    bo_machinegun,
    bo_chaingun,
    bo_food,
    bo_fullheal,
    bo_25clip,
    bo_spear,
    bo_money1b,
    bo_money5b,
    bo_money10b,
    bo_moneybill,
    bo_rarechest
} wl_stat_t;

typedef enum {
    east,
    northeast,
    north,
    northwest,
    west,
    southwest,
    south,
    southeast,
    nodir
} dirtype;


#define NUMENEMIES  22
typedef enum {
    en_guard,
    en_officer,
    en_ss,
    en_dog,
    en_boss,
    en_schabbs,
    en_fake,
    en_hitler,
    en_mutant,
    en_blinky,
    en_clyde,
    en_pinky,
    en_inky,
    en_gretel,
    en_gift,
    en_fat,
    en_spectre,
    en_angel,
    en_trans,
    en_uber,
    en_will,
    en_death
} enemy_t;

typedef void (* statefunc) (void *);

typedef struct statestruct
{
    boolean rotate;
    short   shapenum;           // a shapenum of -1 means get from ob->temp1
    short   tictime;
    void    (*think) (void *),(*action) (void *);
    struct  statestruct *next;
} statetype;


//---------------------
//
// trivial actor structure
//
//---------------------

typedef struct statstruct
{
    byte      tilex,tiley;
    short     shapenum;           // if shapenum == -1 the obj has been removed
    byte      *visspot;
    uint32_t  flags;
    byte      itemnumber;
} statobj_t;


//---------------------
//
// door actor structure
//
//---------------------

typedef enum
{
    dr_open,dr_closed,dr_opening,dr_closing
} doortype;

typedef struct doorstruct
{
    byte     tilex,tiley;
    boolean  vertical;
    byte     lock;
    doortype action;
    short    ticcount;
    int      tag;
} doorobj_t;


//--------------------
//
// thinking actor structure
//
//--------------------

typedef struct objstruct
{
    activetype  active;
    short       ticcount;
    classtype   obclass;
    statetype   *state;

    uint32_t    flags;              // FL_SHOOTABLE, etc

    int32_t     distance;           // if negative, wait for that door to open
    dirtype     dir;

    fixed       x,y;
    word        tilex,tiley;
    byte        areanumber;

    short       viewx;
    word        viewheight;
    fixed       transx,transy;      // in global coord

    short       angle;
    short       hitpoints;
    int32_t     speed;

    short       temp1,temp2,hidden;
    struct objstruct *next,*prev;
} objtype;

enum
{
    bt_nobutton=-1,
    bt_attack=0,
    bt_strafe,
    bt_run,
    bt_use,
    bt_readyknife,
    bt_readypistol,
    bt_readymachinegun,
    bt_readychaingun,
    bt_readyrainthrower,
    bt_nextweapon,
    bt_prevweapon,
    bt_esc,
    bt_pause,
    bt_strafeleft,
    bt_straferight,
    bt_moveforward,
    bt_movebackward,
    bt_turnleft,
    bt_turnright,
    NUMBUTTONS
};


#define NUMWEAPONS      5
typedef enum
{
    wp_knife,
    wp_pistol,
    wp_machinegun,
    wp_chaingun,
    wp_rthrower
} weapontype;


enum
{
    gd_baby,
    gd_easy,
    gd_medium,
    gd_hard
};

//---------------
//
// gamestate structure
//
//---------------

typedef struct
{
    short       difficulty;
    short       mapon;
    short       money;       // Currency for machines (bits)
    short       health;
    short       ammo;
    short       ammo2;
    short       keys;
    int         bobber;
    int         bobdir;
    int         bobber2;
    int         bobdir2;
    weapontype  bestweapon,weapon,chosenweapon;
    boolean     gotmachinegun;
    boolean     gotchaingun;
    boolean     gotrainbowergun;
    boolean     atvendingmenu;

    short       faceframe;
    short       attackframe,attackcount,weaponframe;

    short       episode,secretcount,treasurecount,killcount,
                secrettotal,treasuretotal,killtotal;
    int32_t     TimeCount;
    int32_t     killx,killy;
    boolean     victoryflag;            // set during victory animations
    int         endingmode;             // 0 - bad ending #1 (get killed on final level)
                                        // 1 - bad ending #2 (kill rainbow dash on final level)
                                        // 2 - good ending   (escape with aurora from the factory)
                                        // -1 - does not apply (Episodes 1 and 2)
    char        message[48];
    char        comphint[96];
} gametype;


typedef enum
{
    ex_stillplaying,
    ex_completed,
    ex_died,
    ex_warped,
    ex_resetgame,
    ex_loadedgame,
    ex_victorious,
    ex_abort,
    ex_demodone,
    ex_secretlevel
} exit_t;


extern word *mapsegs[MAPPLANES];
extern int mapon;
extern int lhintnum[30];

/*
=============================================================================

                             WL_MAIN DEFINITIONS

=============================================================================
*/
extern  bool     allowwindow;
extern  boolean  loadedgame;
extern  fixed    focallength;
extern  int      viewscreenx, viewscreeny;
extern  int      viewwidth;
extern  int      viewheight;
extern  short    centerx;
extern  int32_t  heightnumerator;
extern  fixed    scale;

extern  int      dirangle[9];

extern  int      mouseadjustment;
extern  int      shootdelta;
extern  unsigned screenofs;

extern  boolean  startgame;
extern  char     str[80];
extern  char     configdir[256];
extern  char     configname[13];
extern  char     achievement_text[128];
extern  boolean  achievements_unlocked[10];
extern  int      episodes_unlocked;
extern  const char * achievement_strings[10];
extern  const char * achievement_defs[10];


//
// Command line parameter variables
//
extern  boolean  param_debugmode;
extern  boolean  param_nowait;
extern  int      param_difficulty;
extern  int      param_tedlevel;
extern  int      param_joystickindex;
extern  int      param_joystickhat;
extern  int      param_samplerate;
extern  int      param_audiobuffer;
extern  int      param_mission;
extern  boolean  param_goodtimes;
extern  boolean  param_ignorenumchunks;


void            NewGame (int difficulty,int episode);
void            CalcProjection (int32_t focal);
void            NewViewSize (int width);
boolean         SetViewSize (unsigned width, unsigned height);
boolean         LoadTheGame(FILE *file,int x,int y);
boolean         SaveTheGame(FILE *file,int x,int y);
void            ShowViewSize (int width);
void            ShutdownId (void);
void            WriteConfig(void);


/*
=============================================================================

                         WL_GAME DEFINITIONS

=============================================================================
*/

extern  gametype        gamestate;
extern  byte            bordercol;
extern  SDL_Surface     *latchpics[NUMLATCHPICS];
extern  char            demoname[13];

void    SetupGameLevel (void);
void    GameLoop (void);
void    DrawPlayBorder (void);
void    DrawStatusBorder (byte color);
void    DrawPlayScreen (void);
void    DrawPlayBorderSides (void);
void    ShowActStatus();

void    PlayDemo (int demonumber);
void    RecordDemo (void);


#ifdef SPEAR
extern  int32_t            spearx,speary;
extern  unsigned        spearangle;
extern  boolean         spearflag;
#endif


#define ClearMemory SD_StopDigitized


// JAB
#define PlaySoundLocTile(s,tx,ty)       PlaySoundLocGlobal(s,(((int32_t)(tx) << TILESHIFT) + (1L << (TILESHIFT - 1))),(((int32_t)ty << TILESHIFT) + (1L << (TILESHIFT - 1))))
#define PlaySoundLocActor(s,ob)         PlaySoundLocGlobal(s,(ob)->x,(ob)->y)
void    PlaySoundLocGlobal(word s,fixed gx,fixed gy);
void UpdateSoundLoc(void);


/*
=============================================================================

                            WL_PLAY DEFINITIONS

=============================================================================
*/

#define BASEMOVE                35
#define RUNMOVE                 70
#define BASETURN                35
#define RUNTURN                 70

#define JOYSCALE                2

extern  byte            tilemap[MAPSIZE][MAPSIZE];      // wall values only
extern  byte            spotvis[MAPSIZE][MAPSIZE];
extern  objtype         *actorat[MAPSIZE][MAPSIZE];

extern  objtype         *player;

extern  unsigned        tics;
extern  int             viewsize;

extern  int             lastgamemusicoffset;

//
// current user input
//
extern  int         controlx,controly;              // range from -100 to 100
extern  boolean     buttonstate[NUMBUTTONS];
extern  objtype     objlist[MAXACTORS];
extern  boolean     buttonheld[NUMBUTTONS];
extern  exit_t      playstate;
extern  boolean     madenoise;
extern  statobj_t   statobjlist[MAXSTATS];
extern  statobj_t   *laststatobj;
extern  objtype     *newobj,*killerobj;
extern  doorobj_t   doorobjlist[MAXDOORS];
extern  doorobj_t   *lastdoorobj;
extern  int         godmode;

extern  boolean     demorecord,demoplayback;
extern  int8_t      *demoptr, *lastdemoptr;
extern  memptr      demobuffer;

//
// control info
//
extern  boolean     mouseenabled,joystickenabled;
extern  int         dirscan[4];
extern  int         buttonscan[NUMBUTTONS];
extern  int         buttonmouse[4];
extern  int         buttonjoy[32];

void    InitActorList (void);
void    GetNewActor (void);
void    PlayLoop (void);

void    CenterWindow(word w,word h);

void    InitRedShifts (void);
void    FinishPaletteShifts (void);

void    RemoveObj (objtype *gone);
void    PollControls (void);
int     StopMusic(void);
void    StartMusic(void);
void    ContinueMusic(int offs);
void    StartDamageFlash (int damage);
void    StartBonusFlash (void);

#ifdef SPEAR
extern  int32_t     funnyticount;           // FOR FUNNY BJ FACE
#endif

extern  objtype     *objfreelist;     // *obj,*player,*lastobj,

extern  boolean     noclip,ammocheat;
extern  int         singlestep, extravbls;

/*
=============================================================================

                                WL_INTER

=============================================================================
*/

void IntroScreen (void);
void PG13(void);
void DrawAchievements(void);
void CheckHighScore (int32_t score,word other);
// boolean DoAnimation(char *afilename);
void Victory (void);
void LevelCompleted (void);
void ClearSplitVWB (void);

void PreloadGraphics(void);


/*
=============================================================================

                                WL_DEBUG

=============================================================================
*/

int DebugKeys (void);

/*
=============================================================================

                            WL_DRAW DEFINITIONS

=============================================================================
*/

//
// math tables
//
extern  short *pixelangle;
extern  int32_t finetangent[FINEANGLES/4];
extern  fixed sintable[];
extern  fixed *costable;
extern  int *wallheight;
extern  word horizwall[],vertwall[];
extern  int32_t    lasttimecount;
extern  int32_t    frameon;

extern  unsigned screenloc[3];

extern  boolean fizzlein, fpscounter;

extern  fixed   viewx,viewy;                    // the focal point
extern  fixed   viewsin,viewcos;

void    ThreeDRefresh (void);
void    CalcTics (void);
void    GetMessage(const char* lastmessage);
void    DrawMessage(void);
void    GetHint(int hintcount);
void    DrawHint(void);
void    GetHintFromMap(int hintcount);
void    DrawVendingMenu(void);
unsigned     GetPatchNum();
boolean ApplyPatch(int wallpic);

typedef struct
{
    word leftpix,rightpix;
    word dataofs[64];
// table data after dataofs[rightpix-leftpix+1]
} t_compshape;

/*
=============================================================================

                             WL_STATE DEFINITIONS

=============================================================================
*/
#define TURNTICS        10
#define SPDPATROL       512
#define SPDDOG          1500


void    InitHitRect (objtype *ob, unsigned radius);
void    SpawnNewObj (unsigned tilex, unsigned tiley, statetype *state);
void    NewState (objtype *ob, statetype *state);

boolean TryWalk (objtype *ob);
void    SelectChaseDir (objtype *ob);
void    SelectDodgeDir (objtype *ob);
void    SelectRunDir (objtype *ob);
void    MoveObj (objtype *ob, int32_t move);
boolean SightPlayer (objtype *ob);

void    KillActor (objtype *ob);
void    DamageActor (objtype *ob, unsigned damage);

boolean CheckLine (objtype *ob);
boolean CheckStatLine (statobj_t *ob);
boolean CheckSight (objtype *ob);

/*
=============================================================================

                             WL_AGENT DEFINITIONS

=============================================================================
*/

extern  short    anglefrac;
extern  int      facecount, facetimes;
extern  word     plux,pluy;         // player coordinates scaled to unsigned
extern  int32_t  thrustspeed;
extern  objtype  *LastAttacker;

void    Thrust (int angle, int32_t speed);
void    SpawnPlayer (int tilex, int tiley, int dir);
void    TakeDamage (int points,objtype *attacker);
// void    GivePoints (int32_t points);
void    GetBonus (statobj_t *check);
void    GiveWeapon (int weapon);
void    GiveAmmo (int ammo);
void    GiveRBAmmo (int ammo);
void    GiveKey (int key);
void    GiveMoney (int bits);
void    DrawMessageBG(void);

//
// player state info
//

void    StatusDrawFace(unsigned picnum);
void    DrawFace (void);
void    DrawHealth (void);
void    HealSelf (int points);
void    DrawLevel (void);
// void    DrawLives (void);
void    GiveExtraMan (void);
// void    DrawScore (void);
void    DrawWeapon (void);
void    DrawKeys (void);
void    DrawAmmo (void);
void    DrawMoney (void);

//
// Achievements System
//
void    SetAchievement(int num);
void    DrawAchievement(int num);


/*
=============================================================================

                             WL_ACT1 DEFINITIONS

=============================================================================
*/

#define MAXPWALLS 64

extern  doorobj_t           doorobjlist[MAXDOORS];
extern  doorobj_t           *lastdoorobj;
extern  short               doornum, pwallnum, opennum[MAXDOORS];

extern  word      doorposition[MAXDOORS];

extern  byte      areaconnect[NUMAREAS][NUMAREAS];

extern  boolean   areabyplayer[NUMAREAS];

typedef struct
{
    bool     active;
    word     state;
    word     pos;        // amount a pushable wall has been moved (0-63)
    word     x;
    word     y;
    byte     dir;
    byte     tile;
    int      tag;
} pwall_t;

extern pwall_t  pwall[MAXPWALLS];
extern short     pw;

extern  pwall_t             pwalls_[MAXPWALLS];
extern  pwall_t             *lastpwall;

// Defines for the purpose of simplifying the draw loop
#define pwallstate  (pwall[pw].state)
#define pwallpos    (pwall[pw].pos)
#define pwallx      (pwall[pw].x)
#define pwally      (pwall[pw].y)
#define pwalldir    (pwall[pw].dir)
#define pwalltile   (pwall[pw].tile)


void InitDoorList (void);
void InitStaticList (void);
void SpawnPatch (int tilex, int tiley);
void SpawnStatic (int tilex, int tiley, int type);
void SpawnDoor (int tilex, int tiley, boolean vertical, int lock);
void GetTagInfoForPWall (int tilex, int tiley, controldir_t dir);
void MoveDoors (void);
void MovePWalls (void);
void OpenDoor (int door);
void PlaceItemType (int itemtype, int tilex, int tiley);
void PushWall (int checkx, int checky, int dir, boolean secret);
void MapSquareChange(int checkx, int checky, int32_t value, int PlaneNumber);
void OperateDoor (int door);
void InitAreas (void);
void ClearTaggingTables(void);

/*
=============================================================================

                             WL_ACT2 DEFINITIONS

=============================================================================
*/

#define s_nakedbody s_static10

extern  statetype s_grddie1;
extern  statetype s_dogdie1;
extern  statetype s_ofcdie1;
extern  statetype s_mutdie1;
extern  statetype s_ssdie1;
extern  statetype s_bossdie1;
extern  statetype s_schabbdie1;
extern  statetype s_mechadie1;
extern  statetype s_hitlerdie1;
extern  statetype s_greteldie1;
extern  statetype s_giftdie1;
extern  statetype s_fatdie1;

extern  statetype s_spectredie1;
extern  statetype s_angeldie1;
extern  statetype s_transdie0;
extern  statetype s_uberdie0;
extern  statetype s_willdie1;
extern  statetype s_deathdie1;


extern  statetype s_grdchase1;
extern  statetype s_dogchase1;
extern  statetype s_ofcchase1;
extern  statetype s_sschase1;
extern  statetype s_mutchase1;
extern  statetype s_bosschase1;
extern  statetype s_schabbchase1;
extern  statetype s_mechachase1;
extern  statetype s_gretelchase1;
extern  statetype s_giftchase1;
extern  statetype s_fatchase1;

extern  statetype s_spectrechase1;
extern  statetype s_angelchase1;
extern  statetype s_transchase1;
extern  statetype s_uberchase1;
extern  statetype s_willchase1;
extern  statetype s_deathchase1;

extern  statetype s_blinkychase1;
// extern  statetype s_hitlerchase1;

extern  statetype s_aurora1;
extern  statetype s_aurora2;
extern  statetype s_aurora3;

extern  statetype s_grdpain;
extern  statetype s_grdpain1;
extern  statetype s_ofcpain;
extern  statetype s_ofcpain1;
extern  statetype s_sspain;
extern  statetype s_sspain1;
extern  statetype s_mutpain;
extern  statetype s_mutpain1;

extern  statetype s_deathcam;

extern  statetype s_schabbdeathcam2;
extern  statetype s_hitlerdeathcam2;

extern  statetype s_glass_stand_n;

extern  statetype s_glass_stand_a;
extern  statetype s_glass_stand_b;
extern  statetype s_glass_die1;
extern  statetype s_glass_die2;
extern  statetype s_glass_die3;
extern  statetype s_glass_dead;

extern  statetype s_wires_live_a;
extern  statetype s_wires_live_b;
extern  statetype s_wires_live_c;
extern  statetype s_wires_live_d;
extern  statetype s_wires_dead;

void SpawnStand (enemy_t which, int tilex, int tiley, int dir);
void SpawnCompTerminal (int tilex, int tiley);
void SpawnPatrol (enemy_t which, int tilex, int tiley, int dir);
void KillActor (objtype *ob);

void SpawnDeadGuard (int tilex, int tiley);
void SpawnBoss (int tilex, int tiley);
void SpawnPegasusDeviceControl (int tilex, int tiley);
void SpawnPegasusDeviceSpots (int tilex, int tiley);
void A_IncrementSwitchCount(objtype *ob);
void A_DestroyPegasusDevice(objtype *ob);
void SpawnAurora (int tilex, int tiley);
void SpawnTrans (int tilex, int tiley);
void SpawnUber (int tilex, int tiley);
void SpawnWill (int tilex, int tiley);
void SpawnDeath (int tilex, int tiley);
void SpawnAngel (int tilex, int tiley);
void SpawnSpectre (int tilex, int tiley);
void SpawnGhosts (int which, int tilex, int tiley);
void SpawnSchabbs (int tilex, int tiley);
void SpawnFakeHitler (int tilex, int tiley);
void SpawnHitler (int tilex, int tiley);
void SpawnRegularGlass (boolean vert, int tilex, int tiley);
void SpawnBreakableGlass (boolean vert, boolean bloody, int tilex, int tiley);
void SpawnLiveWires (int tilex, int tiley);

void A_DeathScream (objtype *ob);
void SpawnBJVictory (void);

/*
=============================================================================

                             WL_TEXT DEFINITIONS

=============================================================================
*/

extern  char    helpfilename[],endfilename[];

extern  void    HelpScreens(void);
extern  void    EndText(void);


/*
=============================================================================

                               GP2X DEFINITIONS

=============================================================================
*/

#if defined(GP2X)

#if defined(GP2X_940)
void GP2X_MemoryInit(void);
void GP2X_Shutdown(void);
#endif
void GP2X_ButtonDown(int button);
void GP2X_ButtonUp(int button);

#endif


/*
=============================================================================

                             MISC DEFINITIONS

=============================================================================
*/

static inline fixed FixedMul(fixed a, fixed b)
{
	return (fixed)(((int64_t)a * b + 0x8000) >> 16);
}

#ifdef PLAYDEMOLIKEORIGINAL
    #define DEMOCHOOSE_ORIG_SDL(orig, sdl) ((demorecord || demoplayback) ? (orig) : (sdl))
    #define DEMOCOND_ORIG                  (demorecord || demoplayback)
    #define DEMOIF_SDL                     if(DEMOCOND_SDL)
#else
    #define DEMOCHOOSE_ORIG_SDL(orig, sdl) (sdl)
    #define DEMOCOND_ORIG                  false
    #define DEMOIF_SDL
#endif
#define DEMOCOND_SDL                   (!DEMOCOND_ORIG)

#define GetTicks() ((SDL_GetTicks()*7)/100)

#define ISPOINTER(x) ((((uintptr_t)(x)) & ~0xffff) != 0)

#define CHECKMALLOCRESULT(x) if(!(x)) Quit("Out of memory at %s:%i", __FILE__, __LINE__)

#ifdef _WIN32
    #define strcasecmp stricmp
    #define strncasecmp strnicmp
    #define snprintf _snprintf
#else
    static inline char* itoa(int value, char* string, int radix)
    {
	    sprintf(string, "%d", value);
	    return string;
    }

    static inline char* ltoa(long value, char* string, int radix)
    {
	    sprintf(string, "%ld", value);
	    return string;
    }
#endif

#define lengthof(x) (sizeof(x) / sizeof(*(x)))
#define endof(x)    ((x) + lengthof(x))

static inline word READWORD(byte *&ptr)
{
    word val = ptr[0] | ptr[1] << 8;
    ptr += 2;
    return val;
}

static inline longword READLONGWORD(byte *&ptr)
{
    longword val = ptr[0] | ptr[1] << 8 | ptr[2] << 16 | ptr[3] << 24;
    ptr += 4;
    return val;
}


/*
=============================================================================

                           FEATURE DEFINITIONS

=============================================================================
*/

#ifdef USE_FEATUREFLAGS
    // The currently available feature flags
    #define FF_STARSKY      0x0001
    #define FF_PARALLAXSKY  0x0002
    #define FF_CLOUDSKY     0x0004
    #define FF_RAIN         0x0010
    #define FF_SNOW         0x0020

    // The ffData... variables contain the 16-bit values of the according corners of the current level.
    // The corners are overwritten with adjacent tiles after initialization in SetupGameLevel
    // to avoid interpretation as e.g. doors.
    extern int ffDataTopLeft, ffDataTopRight, ffDataBottomLeft, ffDataBottomRight;

    /*************************************************************
     * Current usage of ffData... variables:
     * ffDataTopLeft:     lower 8-bit: ShadeDefID
     * ffDataTopRight:    FeatureFlags
     * ffDataBottomLeft:  CloudSkyDefID or ParallaxStartTexture
     * ffDataBottomRight: unused
     *************************************************************/

    // The feature flags are stored as a wall in the upper right corner of each level
    static inline word GetFeatureFlags()
    {
        return ffDataTopRight;
    }

#endif

#ifdef USE_FLOORCEILINGTEX
    void DrawFloorAndCeiling(byte *vbuf, unsigned vbufPitch, int min_wallheight);
#endif

#ifdef USE_PARALLAX
    void DrawParallax(byte *vbuf, unsigned vbufPitch);
#endif

#ifdef USE_DIR3DSPR
    void Scale3DShape(byte *vbuf, unsigned vbufPitch, statobj_t *ob);
    void Scale3DShapeActor(byte *vbuf, unsigned vbufPitch, objtype *ob, short ThreeDimFlag); 
#endif

extern byte soundvol, musicvol;
extern boolean reversestereo;
extern boolean compass;

#endif
