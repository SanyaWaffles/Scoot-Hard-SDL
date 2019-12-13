// WL_AGENT.C

#include "wl_def.h"
#pragma hdrstop

/*
=============================================================================

                                LOCAL CONSTANTS

=============================================================================
*/

#define MAXMOUSETURN    10


#define MOVESCALE       150l
#define BACKMOVESCALE   100l
#define ANGLESCALE      20

const char * STR_EWWWWWWW = "...eww";
const char * STR_BADAPPLE = "Bad Apple";
const char * STR_APPLE = "Dole Apple";
const char * STR_FIRST_AID = "First Aid";
const char * STR_GOLDKEY = "Gold Pass";
const char * STR_SILVKEY = "Silver Pass";
const char * STR_10BULLET = "Ammo";
const char * STR_USEDCLIP = "Used Ammo";
const char * STR_SUBMACGUN = "Machine Gun";
const char * STR_SUPERGUN = "Chaingun";
const char * STR_RBOWGUN = "Rainbower";
const char * STR_RBOWAMMO = "Spectratank";
const char * STR_PEGAORB = "Pegaorb";
const char * STR_BRONZEBIT = "1 Bit";
const char * STR_SILVERBIT = "5 Bit";
const char * STR_GOLDENBIT = "10 Bit";
const char * STR_VOUCHERBIT = "20 Bit";
const char * STR_RARECHEST = "Rare Chest";

const char * STR_CANDY_BAR_VEND = "Candy Bar";
const char * STR_APPLE_VEND = "Apple";
const char * STR_APPLE_PIE_VEND = "Apple Pie";
const char * STR_AMMO_VEND = "Bulk Ammo";
const char * STR_AMMO2_VEND = "Spectratank";

const char * STR_DONT_NEED = "Not needed";
const char * STR_NO_FUNDS = "You're broke";

/*
=============================================================================

                                GLOBAL VARIABLES

=============================================================================
*/



//
// player state info
//
int32_t         thrustspeed;

word            plux,pluy;          // player coordinates scaled to unsigned

short           anglefrac;

objtype        *LastAttacker;

/*
=============================================================================

                                                 LOCAL VARIABLES

=============================================================================
*/


void    T_Player (objtype *ob);
void    T_Attack (objtype *ob);
void    RainbowThrowerAttack (objtype *ob);

statetype   s_player = {false,0,0,(statefunc) T_Player,NULL,NULL};
statetype   s_attack = {false,0,0,(statefunc) T_Attack,NULL,NULL};

struct atkinf
{
    int8_t    tics,attack,frame;              // attack is 1 for gun, 2 for knife
} attackinfo[5][14] =
{
    { {6,0,1},{6,2,2},{6,0,3},{6,-1,4} },
    { {6,0,1},{6,1,2},{6,0,3},{6,-1,4} },
    { {6,0,1},{6,1,2},{6,3,3},{6,-1,4} },
    { {6,0,1},{6,1,2},{6,4,3},{6,-1,4} },
    { {6,0,1},{6,1,2},{6,3,3},{6,-1,4} },
};

//===========================================================================

//----------

void Attack (void);
void Use (void);
void Search (objtype *ob);
void SelectWeapon (void);
void SelectItem (void);
void UseVendingMachine (void);

//----------

boolean TryMove (objtype *ob);
void T_Player (objtype *ob);

void ClipMove (objtype *ob, int32_t xmove, int32_t ymove);

/*
=============================================================================

                                CONTROL STUFF

=============================================================================
*/

boolean CheckAttackAmmo (int weapon)
{
    switch (weapon)
    {
        case wp_knife: return true;
            break;
        case wp_pistol:
            if (gamestate.ammo > 0) return true;
            break;
        case wp_machinegun:
            if (gamestate.gotmachinegun && gamestate.ammo > 0) return true;
            break;
        case wp_chaingun:
            if (gamestate.gotchaingun && gamestate.ammo > 0) return true;
            break;
        case wp_rthrower:
            if (gamestate.gotrainbowergun && gamestate.ammo2 > 0) return true;
            break;
    }
    
    return false;
}

/*
======================
=
= CheckWeaponChange
=
= Keys 1-4 change weapons
=
======================
*/

void CheckWeaponChange (void)
{
    int j;
    if(Keyboard[sc_5] && gamestate.gotrainbowergun && (gamestate.ammo2 > 0))
    {
        gamestate.chosenweapon = gamestate.weapon = wp_rthrower;
        DrawAmmo();
        return;
    }
    
    if ((gamestate.weapon == wp_rthrower) && (gamestate.ammo2 < 1))
    {
        for (j = gamestate.bestweapon; j >= 0; j--)
        {
            if (CheckAttackAmmo(j))
            {
                gamestate.weapon = (weapontype)j;
                break;
            }
        }
    }
    else if ((gamestate.weapon <= wp_chaingun) &&
              (gamestate.chosenweapon != wp_knife) &&
              (gamestate.ammo < 1))
    {
        for (j = gamestate.bestweapon; j >= 0; j--)
        {
            if (CheckAttackAmmo(j))
            {
                gamestate.weapon = (weapontype)j;
                break;
            }
        }
    }
    else if ((gamestate.weapon == wp_knife) && (gamestate.chosenweapon
            != wp_knife) && (gamestate.ammo < 1))
    {
        for (j = gamestate.bestweapon; j>= 0; j--)
        {
            if (CheckAttackAmmo(j))
            {
                gamestate.chosenweapon = gamestate.weapon = (weapontype)j;
                break;
            }
        }
    }
    
    else if (!gamestate.ammo && !gamestate.ammo2)
        gamestate.weapon = wp_knife;  // APN: knife is not "chosen" here
    else if (Keyboard[sc_1] && gamestate.bestweapon >= wp_knife && CheckAttackAmmo(0))
        gamestate.weapon = gamestate.chosenweapon = wp_knife;
    else if (Keyboard[sc_2] && gamestate.bestweapon >= wp_pistol && CheckAttackAmmo(1))
        gamestate.weapon = gamestate.chosenweapon = wp_pistol;
    else if (Keyboard[sc_3] && gamestate.bestweapon >= wp_machinegun && CheckAttackAmmo(2))
        gamestate.weapon = gamestate.chosenweapon = wp_machinegun;
    else if (Keyboard[sc_4] && gamestate.bestweapon >= wp_chaingun && CheckAttackAmmo(3))
        gamestate.weapon = gamestate.chosenweapon = wp_chaingun;
    else if (Keyboard[sc_5] && gamestate.bestweapon >= wp_rthrower && CheckAttackAmmo(4))
        gamestate.weapon = gamestate.chosenweapon = wp_rthrower;
    
    DrawAmmo();
    
    // int newWeapon = -1;

    /* if (!gamestate.ammo)            // must use knife with no ammo
    {
        if (!gamestate.ammo2)
        {
            return;
        }
    }

    if(buttonstate[bt_nextweapon] && !buttonheld[bt_nextweapon])
    {
        newWeapon = gamestate.weapon + 1;
        if(newWeapon > gamestate.bestweapon) newWeapon = 0;
    }
    else if(buttonstate[bt_prevweapon] && !buttonheld[bt_prevweapon])
    {
        newWeapon = gamestate.weapon - 1;
        if(newWeapon < 0) newWeapon = gamestate.bestweapon;
    }
    else
    {
        for(int i = wp_knife; i <= gamestate.bestweapon; i++)
        {
            if (buttonstate[bt_readyknife + i - wp_knife])
            {
                newWeapon = i;
                break;
            }
        }
    }

    if(newWeapon != -1)
    {
        gamestate.weapon = gamestate.chosenweapon = (weapontype) newWeapon;
        // DrawWeapon();
        DrawAmmo();
    } */
}


/*
=======================
=
= ControlMovement
=
= Takes controlx,controly, and buttonstate[bt_strafe]
=
= Changes the player's angle and position
=
= There is an angle hack because when going 70 fps, the roundoff becomes
= significant
=
=======================
*/

void ControlMovement (objtype *ob)
{
    int32_t oldx,oldy;
    int     angle;
    int     angleunits;

    thrustspeed = 0;

    oldx = player->x;
    oldy = player->y;

    if(buttonstate[bt_strafeleft])
    {
        angle = ob->angle + ANGLES/4;
        if(angle >= ANGLES)
            angle -= ANGLES;
        if(buttonstate[bt_run])
            Thrust(angle, RUNMOVE * MOVESCALE * tics);
        else
            Thrust(angle, BASEMOVE * MOVESCALE * tics);
    }

    if(buttonstate[bt_straferight])
    {
        angle = ob->angle - ANGLES/4;
        if(angle < 0)
            angle += ANGLES;
        if(buttonstate[bt_run])
            Thrust(angle, RUNMOVE * MOVESCALE * tics );
        else
            Thrust(angle, BASEMOVE * MOVESCALE * tics);
    }

    //
    // side to side move
    //
    if (buttonstate[bt_strafe])
    {
        //
        // strafing
        //
        //
        if (controlx > 0)
        {
            angle = ob->angle - ANGLES/4;
            if (angle < 0)
                angle += ANGLES;
            Thrust (angle,controlx*MOVESCALE);      // move to left
        }
        else if (controlx < 0)
        {
            angle = ob->angle + ANGLES/4;
            if (angle >= ANGLES)
                angle -= ANGLES;
            Thrust (angle,-controlx*MOVESCALE);     // move to right
        }
    }
    else
    {
        //
        // not strafing
        //
        anglefrac += controlx;
        angleunits = anglefrac/ANGLESCALE;
        anglefrac -= angleunits*ANGLESCALE;
        ob->angle -= angleunits;

        if (ob->angle >= ANGLES)
            ob->angle -= ANGLES;
        if (ob->angle < 0)
            ob->angle += ANGLES;

    }

    //
    // forward/backwards move
    //
    if (controly < 0)
    {
        Thrust (ob->angle,-controly*MOVESCALE); // move forwards
    }
    else if (controly > 0)
    {
        angle = ob->angle + ANGLES/2;
        if (angle >= ANGLES)
            angle -= ANGLES;
        Thrust (angle,controly*BACKMOVESCALE);          // move backwards
    }

    if (gamestate.victoryflag)              // watching the BJ actor
        return;
}

/*
=============================================================================

                            STATUS WINDOW STUFF

=============================================================================
*/


/*
==================
=
= StatusDrawPic
=
==================
*/

void StatusDrawPic (unsigned x, unsigned y, unsigned picnum)
{
    LatchDrawPicScaledCoord ((screenWidth-scaleFactor*320)/16 + scaleFactor*x,
        screenHeight-scaleFactor*(STATUSLINES-y),picnum);
}

void DrawMessageBG (void)
{
    StatusDrawPic (6, 4, MESSAGE_BGPIC);
}

void StatusDrawFace(unsigned picnum)
{
    StatusDrawPic(17, 4, picnum);

#ifdef _arch_dreamcast
    DC_StatusDrawLCD(picnum);
#endif
}


/*
==================
=
= DrawFace
=
==================
*/

void DrawFace (void)
{
    if(viewsize == 21 && ingame) return;

    if (gamestate.health)
    {
        if (gamestate.health < 0)
            gamestate.health = -1;
            
        StatusDrawFace(FACE1APIC+3*((100-gamestate.health)/16)+gamestate.faceframe);
    }
    else
    {
#ifndef SPEAR
        if (LastAttacker && LastAttacker->obclass == needleobj)
            StatusDrawFace(MUTANTBJPIC);
        else
#endif
            StatusDrawFace(FACE8APIC);
    }
}

/*
===============
=
= UpdateFace
=
= Calls draw face if time to change
=
===============
*/

int facecount = 0;
int facetimes = 0;

void UpdateFace (void)
{
    
    // don't make demo depend on sound playback
    if(demoplayback || demorecord)
    {
        if(facetimes > 0)
        {
            facetimes--;
            return;
        }
    }
    
    facecount += tics;
    if (facecount > US_RndT())
    {
        gamestate.faceframe = (US_RndT()>>6);
        if (gamestate.faceframe==3)
            gamestate.faceframe = 1;

        facecount = 0;
        DrawFace ();
    }
}



/*
===============
=
= LatchNumber
=
= right justifies and pads with blanks
=
===============
*/

static void LatchNumber (int x, int y, unsigned width, int32_t number)
{
    unsigned length,c;
    char    str[20];

    ltoa (number,str,10);

    length = (unsigned) strlen (str);

    while (length<width)
    {
        StatusDrawPic (x,y,N_BLANKPIC);
        x++;
        width--;
    }

    c = length <= width ? 0 : length-width;

    while (c<length)
    {
        StatusDrawPic (x,y,str[c]-'0'+ N_0PIC);
        x++;
        c++;
    }
}


/*
===============
=
= DrawHealth
=
===============
*/

void DrawHealth (void)
{
    if(viewsize == 21 && ingame) return;
    LatchNumber (21,16,3,gamestate.health);
}


/*
===============
=
= TakeDamage
=
===============
*/

void TakeDamage (int points,objtype *attacker)
{
    LastAttacker = attacker;

    if (gamestate.victoryflag)
        return;
    if (gamestate.difficulty==gd_baby)
        points>>=2;

    if (!godmode)
        gamestate.health -= points;

    if (gamestate.health<=0)
    {
        gamestate.health = 0;
        playstate = ex_died;
        killerobj = attacker;
    }

    if (godmode != 2)
        StartDamageFlash (points);

    DrawHealth ();
    DrawFace ();

    //
    // MAKE BJ'S EYES BUG IF MAJOR DAMAGE!
    //
#ifdef SPEAR
    if (points > 30 && gamestate.health!=0 && !godmode && viewsize != 21)
    {
        StatusDrawFace(BJOUCHPIC);
        facecount = 0;
    }
#endif
}

/*
===============
=
= HealSelf
=
===============
*/

void HealSelf (int points)
{
    gamestate.health += points;
    if (gamestate.health>100)
        gamestate.health = 100;

    DrawHealth ();
    DrawFace ();
}


//===========================================================================


/*
===============
=
= DrawLevel
=
===============
*/

void DrawLevel (void)
{
    if(viewsize == 21 && ingame) return;
#ifdef SPEAR
    if (gamestate.mapon == 20)
        LatchNumber (2,16,2,18);
    else
#endif
        LatchNumber (2,16,2,gamestate.mapon+1);
}

//===========================================================================


/*
===============
=
= GiveExtraMan
=
===============
*/

/* void GiveExtraMan (void)
{
    if (gamestate.lives<9)
        gamestate.lives++;
    DrawLives ();
    SD_PlaySound (BONUS1UPSND);
} */

//===========================================================================

/*
===============
=
= DrawScore
=
===============
*/

/* void DrawScore (void)
{
    if(viewsize == 21 && ingame) return;
    LatchNumber (6,16,6,gamestate.score);
} */

/*
===============
=
= GivePoints
=
===============
*/

/* void GivePoints (int32_t points)
{
    gamestate.score += points;
    while (gamestate.score >= gamestate.nextextra)
    {
        gamestate.nextextra += EXTRAPOINTS;
        GiveExtraMan ();
    }
    DrawScore ();
} */

//===========================================================================

/*
==================
=
= DrawWeapon
=
==================
*/

void DrawWeapon (void)
{
    if(viewsize == 21 && ingame) return;
    // StatusDrawPic (32,8,KNIFEPIC+gamestate.weapon);
}


/*
==================
=
= DrawKeys
=
==================
*/

void DrawKeys (void)
{
    if(viewsize == 21 && ingame) return;
    if (gamestate.keys & 1)
        StatusDrawPic (30,4,GOLDKEYPIC);
    else
        StatusDrawPic (30,4,NOKEYPIC);

    if (gamestate.keys & 2)
        StatusDrawPic (30,20,SILVERKEYPIC);
    else
        StatusDrawPic (30,20,NOKEYPIC);
}

/*
==================
=
= GiveWeapon
=
==================
*/

void GiveWeapon (int weapon)
{
    /* if (weapon != (int)wp_rthrower)
        GiveAmmo (10);
    else
        GiveRBAmmo (20);

    if (gamestate.bestweapon<weapon)
        gamestate.weapon
        = gamestate.chosenweapon = (weapontype) weapon; */
    
    if (weapon != wp_rthrower)
    {
        switch (weapon)
        {
            case wp_machinegun:
                GiveAmmo(10);
                if (((!gamestate.gotmachinegun && !gamestate.gotchaingun
                     && gamestate.weapon != wp_rthrower) ||
                     (gamestate.gotmachinegun && gamestate.weapon <= wp_pistol)))
                {
                    gamestate.weapon = gamestate.chosenweapon = (weapontype)weapon;
                }
                gamestate.gotmachinegun = true;
                break;
            case wp_chaingun:
                GiveAmmo(20);
                if (((!gamestate.gotmachinegun && !gamestate.gotchaingun
                      && gamestate.weapon != wp_rthrower) ||
                     (gamestate.gotmachinegun && gamestate.weapon <= wp_pistol)))
                {
                    gamestate.weapon = gamestate.chosenweapon = (weapontype)weapon;
                }
                gamestate.gotchaingun = true;
                break;
        }
    }
    else if (weapon == wp_rthrower)
    {
        GiveRBAmmo(20);
        if (!gamestate.gotrainbowergun)
        {
            gamestate.weapon = gamestate.chosenweapon = (weapontype)weapon;
            gamestate.gotrainbowergun = true;
        }
    }
    
    if (gamestate.bestweapon < weapon)
        gamestate.bestweapon = gamestate.weapon
        = gamestate.chosenweapon = (weapontype)weapon;
    
    DrawAmmo();

}

//===========================================================================

void DrawMoney (void)
{
    if(viewsize == 21 && ingame) return;
    LatchNumber (32,16,3,gamestate.money);
}

void GiveMoney(int bits)
{
    gamestate.money += bits;
    if (gamestate.money >= 150)
        gamestate.money = 150;
    
    DrawMoney();
    
    if (achievements_unlocked[4] == false)
    {
        if (gamestate.money >= 100)
        {
            SetAchievement(4);
        }
    }
}

/*
===============
=
= DrawAmmo
=
===============
*/

void DrawAmmo (void)
{
    if(viewsize == 21 && ingame) return;
    
    if(gamestate.weapon != wp_rthrower)
        LatchNumber (27,16,3,gamestate.ammo);
    else
        LatchNumber (27,16,3,gamestate.ammo2);
}

/*
===============
=
= GiveAmmo
=
===============
*/

void GiveAmmo (int ammo)
{
    if (!gamestate.ammo)                            // knife was out
    {
        if (!gamestate.attackframe)
        {
            gamestate.weapon = gamestate.chosenweapon;
        }
    }
    gamestate.ammo += ammo;
    if (gamestate.ammo > 250)
        gamestate.ammo = 250;
    DrawAmmo ();
}

void GiveRBAmmo (int ammo2)
{
    if (!gamestate.ammo2)                            // knife was out
    {
        if (!gamestate.attackframe)
        {
            gamestate.weapon = gamestate.chosenweapon;
        }
    }
    gamestate.ammo2 += ammo2;
    if (gamestate.ammo2 > 150)
        gamestate.ammo2 = 150;
    DrawAmmo ();
}

//===========================================================================

/*
==================
=
= GiveKey
=
==================
*/

void GiveKey (int key)
{
    gamestate.keys |= (1<<key);
    DrawKeys ();
}



/*
=============================================================================

                                MOVEMENT

=============================================================================
*/


/*
===================
=
= GetBonus
=
===================
*/
void GetBonus (statobj_t *check)
{
    switch (check->itemnumber)
    {
        case    bo_firstaid:
            if (gamestate.health == 100)
                return;

            SD_PlaySound (GENERIC_ITEMUPSND);
            HealSelf (25);
            GetMessage(STR_FIRST_AID);
            break;
            
        case    bo_money1b:
            if (gamestate.money == 150)
                return;
            
            SD_PlaySound (GENERIC_ITEMUPSND);
            GiveMoney(1);
            GetMessage(STR_BRONZEBIT);
            break;
            
        case    bo_money5b:
            if (gamestate.money == 150)
                return;
            
            SD_PlaySound (GENERIC_ITEMUPSND);
            GiveMoney(5);
            GetMessage(STR_SILVERBIT);
            break;
            
        case    bo_money10b:
            if (gamestate.money == 150)
                return;
            
            SD_PlaySound (GENERIC_ITEMUPSND);
            GiveMoney(10);
            GetMessage(STR_GOLDENBIT);
            break;

        case    bo_key1:
            GetMessage(STR_GOLDKEY);
            GiveKey (check->itemnumber - bo_key1);
            SD_PlaySound (KEY_PICKUP_SND);
            break;
            
        case    bo_key2:
            GetMessage(STR_SILVKEY);
            GiveKey (check->itemnumber - bo_key1);
            SD_PlaySound (KEY_PICKUP_SND);
            break;

        case    bo_clip:
            if (gamestate.ammo == 250)
                return;

            SD_PlaySound (AMMO_PICKUP_SND);
            GetMessage(STR_10BULLET);
            GiveAmmo (10);
            break;
            
        case    bo_clip2:
            if (gamestate.ammo == 250)
                return;

            SD_PlaySound (AMMO_PICKUP_SND);
            GetMessage(STR_USEDCLIP);
            GiveAmmo (5);
            break;

        case    bo_machinegun:
            SD_PlaySound (AMMO_PICKUP_SND);
            GetMessage(STR_SUBMACGUN);
            GiveWeapon (wp_machinegun);
            if (achievements_unlocked[2] == false)
            {
                SetAchievement(2);
            }
            break;
            
        case    bo_chaingun:
            SD_PlaySound (AMMO_PICKUP_SND);
            GetMessage(STR_SUPERGUN);
            GiveWeapon (wp_chaingun);
            break;
            
        case    bo_rainbower:
            SD_PlaySound (AMMO_PICKUP_SND);
            GetMessage(STR_RBOWGUN);
            GiveWeapon (wp_rthrower);
            break;
            
        case    bo_rbowtank:
            if (gamestate.ammo2 == 150)
                return;
            
            SD_PlaySound(AMMO_PICKUP_SND);
            GetMessage(STR_RBOWAMMO);
            GiveRBAmmo(20);
            break;
            
        case    bo_fullheal:
            SD_PlaySound (GENERIC_ITEMUPSND);
            HealSelf (99);
            GiveAmmo (25);
            GetMessage(STR_PEGAORB);
            gamestate.treasurecount++;
            break;
            
        case    bo_rarechest:
            SD_PlaySound (GENERIC_ITEMUPSND);
            GiveAmmo(40);
            GiveRBAmmo(25);
            GiveMoney(15);
            GetMessage(STR_RARECHEST);
            gamestate.treasurecount++;
            break;
            
        case    bo_moneybill:
            SD_PlaySound (GENERIC_ITEMUPSND);
            GiveMoney (20);
            GetMessage(STR_VOUCHERBIT);
            break;

        case    bo_food:
            if (gamestate.health == 100)
                return;

            SD_PlaySound (GENERIC_ITEMUPSND);
            HealSelf (10);
            GetMessage(STR_APPLE);
            break;

        case    bo_alpo:
            if (gamestate.health == 100)
                return;

            SD_PlaySound (GENERIC_ITEMUPSND);
            HealSelf (5);
            GetMessage(STR_BADAPPLE);
            break;
    }

    StartBonusFlash ();
    check->shapenum = -1;                   // remove from list
}

int pdirs[4][2]={{0,-1},{1,0},{0,1},{-1,0}}; 

/*
===================
=
= TryMove
=
= returns true if move ok
= debug: use pointers to optimize
===================
*/

boolean TryMove (objtype *ob)
{
    int         xl,yl,xh,yh,x,y;
    objtype    *check;
    int32_t     deltax,deltay;

    xl = (ob->x-PLAYERSIZE) >>TILESHIFT;
    yl = (ob->y-PLAYERSIZE) >>TILESHIFT;

    xh = (ob->x+PLAYERSIZE) >>TILESHIFT;
    yh = (ob->y+PLAYERSIZE) >>TILESHIFT;

#define PUSHWALLMINDIST PLAYERSIZE

    //
    // check for solid walls
    //
    for (y=yl;y<=yh;y++)
    {
        for (x=xl;x<=xh;x++)
        {
            check = actorat[x][y];
            if (check && !ISPOINTER(check))
            {
                for (int i = 0; i < MAXPWALLS; i++)
                    if (pwall[i].x == x && pwall[i].y == y)
                    {
                        pw = i;
                        break;
                    }
                if(tilemap[x][y]==64 && x==pwallx && y==pwally)   // back of moving pushwall?
                {
                    switch(pwalldir)
                    {
                        case di_north:
                            if(ob->y-PUSHWALLMINDIST<=(pwally<<TILESHIFT)+((63-pwallpos)<<10))
                                return false;
                            break;
                        case di_west:
                            if(ob->x-PUSHWALLMINDIST<=(pwallx<<TILESHIFT)+((63-pwallpos)<<10))
                                return false;
                            break;
                        case di_east:
                            if(ob->x+PUSHWALLMINDIST>=(pwallx<<TILESHIFT)+(pwallpos<<10))
                                return false;
                            break;
                        case di_south:
                            if(ob->y+PUSHWALLMINDIST>=(pwally<<TILESHIFT)+(pwallpos<<10))
                                return false;
                            break;
                    }
                }
                else return false;
            }
        }
    }

    //
    // check for actors
    //
    if (yl>0)
        yl--;
    if (yh<MAPSIZE-1)
        yh++;
    if (xl>0)
        xl--;
    if (xh<MAPSIZE-1)
        xh++;

    for (y=yl;y<=yh;y++)
    {
        for (x=xl;x<=xh;x++)
        {
            check = actorat[x][y];
            if (ISPOINTER(check) && check != player && (check->flags & FL_SHOOTABLE) )
            {
                deltax = ob->x - check->x;
                if (deltax < -MINACTORDIST || deltax > MINACTORDIST)
                    continue;
                deltay = ob->y - check->y;
                if (deltay < -MINACTORDIST || deltay > MINACTORDIST)
                    continue;

                return false;
            }
        }
    }

    return true;
}


/*
===================
=
= ClipMove
=
===================
*/

void ClipMove (objtype *ob, int32_t xmove, int32_t ymove)
{
    int32_t    basex,basey;

    basex = ob->x;
    basey = ob->y;

    ob->x = basex+xmove;
    ob->y = basey+ymove;
    if (TryMove (ob))
        return;

#ifndef REMDEBUG
    if (noclip && ob->x > 2*TILEGLOBAL && ob->y > 2*TILEGLOBAL
        && ob->x < (((int32_t)(mapwidth-1))<<TILESHIFT)
        && ob->y < (((int32_t)(mapheight-1))<<TILESHIFT) )
        return;         // walk through walls
#endif

    ob->x = basex+xmove;
    ob->y = basey;
    if (TryMove (ob))
        return;

    ob->x = basex;
    ob->y = basey+ymove;
    if (TryMove (ob))
        return;

    ob->x = basex;
    ob->y = basey;
}

//==========================================================================

/*
===================
=
= VictoryTile
=
===================
*/

void VictoryTile (void)
{
#ifndef SPEAR
    SpawnBJVictory ();
#endif
    
    gamestate.endingmode = -1;

    gamestate.victoryflag = true;
}

void EndGameGood (void)
{
    if (achievements_unlocked[9] == false)
        SetAchievement(9);
    
    gamestate.endingmode = 2;
    
    gamestate.victoryflag = true;
    
    playstate = ex_victorious;
}

/*
===================
=
= Thrust
=
===================
*/

// For player movement in demos exactly as in the original Wolf3D v1.4 source code
static fixed FixedByFracOrig(fixed a, fixed b)
{
    int sign = 0;
    if(b == 65536) b = 65535;
    else if(b == -65536) b = 65535, sign = 1;
    else if(b < 0) b = (-b), sign = 1;

    if(a < 0)
    {
        a = -a;
        sign = !sign;
    }
    fixed res = (fixed)(((int64_t) a * b) >> 16);
    if(sign)
        res = -res;
    return res;
}

void Thrust (int angle, int32_t speed)
{
    int32_t xmove,ymove;
    unsigned offset;
    int tag;
    
    if (gamestate.bobdir != 2)
    {gamestate.bobber++;}
    if (gamestate.bobdir == 2)
    {gamestate.bobber--;}
    if (gamestate.bobber == 10)
    {gamestate.bobdir = 2;}
    if (gamestate.bobber == 0)
    {gamestate.bobdir = 1;}
    if (gamestate.bobdir2 != 2)
    {gamestate.bobber2++;}
    if (gamestate.bobdir2 == 2)
    {gamestate.bobber2--;}
    if (gamestate.bobber2 == 20)
    {gamestate.bobdir2 = 2;}
    if (gamestate.bobber2 == 0)
    {gamestate.bobdir2 = 1;}


    //
    // ZERO FUNNY COUNTER IF MOVED!
    //
#ifdef SPEAR
    if (speed)
        funnyticount = 0;
#endif

    thrustspeed += speed;
    //
    // moving bounds speed
    //
    if (speed >= MINDIST*2)
        speed = MINDIST*2-1;

    xmove = DEMOCHOOSE_ORIG_SDL(
                FixedByFracOrig(speed, costable[angle]),
                FixedMul(speed,costable[angle]));
    ymove = DEMOCHOOSE_ORIG_SDL(
                -FixedByFracOrig(speed, sintable[angle]),
                -FixedMul(speed,sintable[angle]));

    ClipMove(player,xmove,ymove);

    player->tilex = (short)(player->x >> TILESHIFT);                // scale to tile values
    player->tiley = (short)(player->y >> TILESHIFT);

    offset = (player->tiley<<mapshift)+player->tilex;
    player->areanumber = *(mapsegs[0] + offset) -AREATILE;

    if (*(mapsegs[1] + offset) == EXITTILE)
        VictoryTile ();
    else if (*(mapsegs[1] + offset) == GOODEXITTILE)
        EndGameGood ();
    else if (*(mapsegs[1] + offset) == 300)
    {
        buttonheld[bt_use] = true;
        SD_PlaySound(SWITCH_SND);
        tag = MAPSPOT(player->tilex,player->tiley,3);
        if (tag > 0 && tag < 64)
        {
            for (int i = 0; i < MAXDOORS; i++)
            {
                if (doorobjlist[i].tag == tag)
                {
                    OpenDoor(i);
                }
            }
            for (int p = 0; p < MAXPWALLS; p++)
            {
                if(pwalls_[p].tag == tag)
                {
                    PushWall(pwalls_[p].x, pwalls_[p].y, pwalls_[p].dir, false);
                }
            }
            for (objtype *check=player->next; check; check=check->next)
            {
                if(check->obclass == lwireobj)
                {
                    if(check->speed == tag)
                    {
                        check->state = &s_wires_dead;
                    }
                }
            }
            
            MapSquareChange(player->tilex, player->tiley, 0, 1);
        }
    }
}


/*
=============================================================================

                                ACTIONS

=============================================================================
*/

extern statetype s_pegmapspot_b;


/*
===============
=
= Cmd_Fire
=
===============
*/

void Cmd_Fire (void)
{
    buttonheld[bt_attack] = true;

    gamestate.weaponframe = 0;

    player->state = &s_attack;

    gamestate.attackframe = 0;
    gamestate.attackcount =
        attackinfo[gamestate.weapon][gamestate.attackframe].tics;
    gamestate.weaponframe =
        attackinfo[gamestate.weapon][gamestate.attackframe].frame;
}

//===========================================================================

void ChangeSwitchNum (int cx, int cy, int cnum)
{
    int tag;
    short floorcode;
    
    buttonheld[bt_use] = true;
    SD_PlaySound(SWITCH_SND);
    tag = MAPSPOT(cx,cy,3);
    if (tag > 0 && tag < 64)
    {
        for (int i = 0; i < MAXDOORS; i++)
        {
            if (doorobjlist[i].tag == tag)
            {
                OpenDoor(i);
            }
        }
        for (int p = 0; p < MAXPWALLS; p++)
        {
            if(pwalls_[p].tag == tag)
            {
                PushWall(pwalls_[p].x, pwalls_[p].y, pwalls_[p].dir, false);
            }
        }
        for (objtype *check=player->next; check; check=check->next)
        {
            if(check->obclass == lwireobj)
            {
                if(check->speed == tag)
                {
                    check->state = &s_wires_dead;
                }
            }
        }
        
        MapSquareChange(cx, cy, cnum, 1);
    }
    else
    {
        if (tag == 666)
        {
            for (objtype *check=player->next; check; check=check->next)
            {
                if(check->obclass == pdevobj)
                {
                    A_IncrementSwitchCount(check);
                }
            }
            MapSquareChange(cx, cy, cnum, 1);
        }
        else if (tag == 668)
        {
            for (objtype *check=player->next; check; check=check->next)
            {
                if(check->obclass == pdevmspotobj)
                {
                    tilemap[check->tilex][check->tiley] = 0;
                    floorcode = *(mapsegs[0]+(check->tiley<<mapshift)+check->tilex+1);
                    *(mapsegs[0]+(check->tiley<<mapshift)+check->tilex) = floorcode;
                    check->state = &s_pegmapspot_b;
                }
            }
            MapSquareChange(cx, cy, cnum, 1);
        }
    }
}

/*
===============
=
= Cmd_Use
=
===============
*/

void Cmd_Use (void)
{
    int     checkx,checky,doornum,dir;
    boolean elevatorok;

    //
    // find which cardinal direction the player is facing
    //
    if (player->angle < ANGLES/8 || player->angle > 7*ANGLES/8)
    {
        checkx = player->tilex + 1;
        checky = player->tiley;
        dir = di_east;
        elevatorok = true;
    }
    else if (player->angle < 3*ANGLES/8)
    {
        checkx = player->tilex;
        checky = player->tiley-1;
        dir = di_north;
        elevatorok = false;
    }
    else if (player->angle < 5*ANGLES/8)
    {
        checkx = player->tilex - 1;
        checky = player->tiley;
        dir = di_west;
        elevatorok = true;
    }
    else
    {
        checkx = player->tilex;
        checky = player->tiley + 1;
        dir = di_south;
        elevatorok = false;
    }

    doornum = tilemap[checkx][checky];
    if (*(mapsegs[1]+(checky<<mapshift)+checkx) == PUSHABLETILE)
    {
        //
        // pushable wall
        //

        PushWall(checkx,checky,dir,true);
        return;
    }
    if (!buttonheld[bt_use])
    {
        switch(*(mapsegs[1]+(checky<<mapshift)+checkx))
        {
            case 500:
            {
                ChangeSwitchNum(checkx, checky, 701);
                break;
            }
            case 501:
            {
                ChangeSwitchNum(checkx, checky, 700);
                break;
            }
            case 502:
            {
                buttonheld[bt_use] = true;
                gamestate.atvendingmenu = true;
                break;
            }
            case 503:
            {
                buttonheld[bt_use] = true;
                if (gamestate.ammo < 250)
                {
                    GiveAmmo(60);
                    SD_PlaySound(AMMO_PICKUP_SND);
                    MapSquareChange(checkx, checky, 505, 1);
                }
                break;
            }
            case 504:
            {
                buttonheld[bt_use] = true;
                SD_PlaySound (AMMO_PICKUP_SND);
                GetMessage(STR_SUBMACGUN);
                GiveWeapon (wp_machinegun);
                if (achievements_unlocked[2] == false)
                {
                    SetAchievement(2);
                }
                MapSquareChange(checkx, checky, 505, 1);
                break;
            }
            case 511:
            {
                ChangeSwitchNum(checkx, checky, 712);
                break;
            }
            case 512:
            {
                ChangeSwitchNum(checkx, checky, 711);
                break;
            }
            case 513:
            {
                ChangeSwitchNum(checkx, checky, 714);
                break;
            }
            case 514:
            {
                ChangeSwitchNum(checkx, checky, 713);
                break;
            }
        }
    }
    if (!buttonheld[bt_use] && doornum == ELEVATORTILE && elevatorok)
    {
        //
        // use elevator
        //
        buttonheld[bt_use] = true;

        tilemap[checkx][checky]++;              // flip switch
        if (*(mapsegs[0]+(player->tiley<<mapshift)+player->tilex) == ALTELEVATORTILE)
            playstate = ex_secretlevel;
        else
            playstate = ex_completed;
        GetMessage("");
        SD_PlaySound (SWITCH_SND);
        SD_WaitSoundDone();
    }
    else if (!buttonheld[bt_use] && doornum & 0x80)
    {
        buttonheld[bt_use] = true;
        OperateDoor (doornum & ~0x80);
    }
}

/*
=============================================================================

                                PLAYER CONTROL

=============================================================================
*/



/*
===============
=
= SpawnPlayer
=
===============
*/

void SpawnPlayer (int tilex, int tiley, int dir)
{
    player->obclass = playerobj;
    player->active = ac_yes;
    player->tilex = tilex;
    player->tiley = tiley;
    player->areanumber = (byte) *(mapsegs[0]+(player->tiley<<mapshift)+player->tilex);
    player->x = ((int32_t)tilex<<TILESHIFT)+TILEGLOBAL/2;
    player->y = ((int32_t)tiley<<TILESHIFT)+TILEGLOBAL/2;
    player->state = &s_player;
    player->angle = (1-dir)*90;
    if (player->angle<0)
        player->angle += ANGLES;
    player->flags = FL_NEVERMARK;
    Thrust (0,0);                           // set some variables

    InitAreas ();
}


//===========================================================================

/*
===============
=
= T_KnifeAttack
=
= Update player hands, and try to do damage when the proper frame is reached
=
===============
*/

void    KnifeAttack (objtype *ob)
{
    objtype *check,*closest;
    int32_t  dist;

    SD_PlaySound (WEAPON_TAZE_SND);
    // actually fire
    dist = 0x7fffffff;
    closest = NULL;
    for (check=ob->next; check; check=check->next)
    {
        if ( (check->flags & FL_SHOOTABLE) && (check->flags & FL_VISABLE)
            && abs(check->viewx-centerx) < shootdelta)
        {
            if (check->transx < dist)
            {
                dist = check->transx;
                closest = check;
            }
        }
    }

    if (!closest || dist > 0x18000l)
    {
        // missed
        return;
    }

    // hit something
    DamageActor (closest,US_RndT() >> 4);
}


void    GunAttack (objtype *ob)
{
    objtype *check,*closest,*oldclosest;
    int      damage;
    int      dx,dy,dist;
    int32_t  viewdist;

    switch (gamestate.weapon)
    {
        case wp_pistol:
            SD_PlaySound (WEAPON_PISTOL_SND);
            break;
        case wp_machinegun:
            SD_PlaySound (WEAPON_SUBMAC_SND);
            break;
        case wp_chaingun:
            SD_PlaySound (WEAPON_CHAINGUN_SND);
            break;
        default:
            break;
    }
    
    if (gamestate.weapon == wp_rthrower)
    {
        RainbowThrowerAttack(ob);
        return;
    }
    else if (gamestate.weapon > wp_knife && gamestate.weapon <= wp_chaingun)
    {
        madenoise = true;
        
        //
        // find potential targets
        //
        viewdist = 0x7fffffffl;
        closest = NULL;
        
        GiveAmmo(-1);
        
        while (1)
        {
            oldclosest = closest;
            
            for (check=ob->next; check ; check=check->next)
            {
                if ((check->flags & FL_SHOOTABLE) && (check->flags & FL_VISABLE)
                    && abs(check->viewx-centerx) < shootdelta)
                {
                    if (check->transx < viewdist)
                    {
                        viewdist = check->transx;
                        closest = check;
                    }
                }
            }
            
            if (closest == oldclosest)
                return;                                // no more targets, all missed
            
            //
            // trace a line from player to enemey
            //
            if (CheckLine(closest))
                break;
        }

        
        //
        // hit something
        //
        dx = ABS(closest->tilex - player->tilex);
        dy = ABS(closest->tiley - player->tiley);
        dist = dx>dy ? dx:dy;
        
        if (dist<2)
            damage = US_RndT() / 4;
        else if (dist<4)
            damage = US_RndT() / 6;
        else
        {
            if ((US_RndT() / 12) < dist)           // missed
                return;
            damage = US_RndT() / 6;
        }
        
        DamageActor (closest,damage);
    }
}

extern statetype s_fire1;

void    RainbowThrowerAttack (objtype *ob)
{
    int32_t deltax,deltay;
    
    if (!objfreelist)       // stop shooting if over MAXACTORS
    {
        return;
    }
    
    madenoise = true;
    
    deltax = player->x - ob->x;
    deltay = ob->y - player->y;

    GetNewActor ();
    newobj->state = &s_fire1;
    newobj->ticcount = 2;
    
    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->dir = nodir;
    newobj->angle = ob->angle;
    newobj->obclass = p_rainballobj;
    newobj->speed = 0x4200l;
    newobj->flags = FL_FULLBRIGHT | FL_VISABLE | FL_BONUS;
    newobj->active = ac_yes;
    
    PlaySoundLocActor (FLAMETHROWERSND,newobj);
    
    GiveRBAmmo(-1);
}

//===========================================================================

/*
===============
=
= VictorySpin
=
===============
*/

void VictorySpin (void)
{
    int32_t    desty;

    if (player->angle > 90)
    {
        player->angle -= (short)(tics * 3);
        if (player->angle < 90)
            player->angle = 90;
    }
    else if (player->angle < 90)
    {
        player->angle += (short)(tics * 3);
        if (player->angle > 90)
            player->angle = 90;
    }

    desty = (((int32_t)player->tiley+12)<<TILESHIFT)+0x3000;

    if (player->y < desty)
    {
        player->y += tics*4096;
        if (player->y > desty)
            player->y = desty;
    }
}


//===========================================================================

/*
===============
=
= T_Attack
=
===============
*/

void    T_Attack (objtype *ob)
{
    struct  atkinf  *cur;

    UpdateFace ();

    if (gamestate.victoryflag)              // watching the BJ actor
    {
        VictorySpin ();
        return;
    }

    if ( buttonstate[bt_use] && !buttonheld[bt_use] )
        buttonstate[bt_use] = false;

    if ( buttonstate[bt_attack] && !buttonheld[bt_attack])
        buttonstate[bt_attack] = false;

    ControlMovement (ob);
    if (gamestate.victoryflag)              // watching the BJ actor
        return;

    plux = (word) (player->x >> UNSIGNEDSHIFT);                     // scale to fit in unsigned
    pluy = (word) (player->y >> UNSIGNEDSHIFT);
    player->tilex = (short)(player->x >> TILESHIFT);                // scale to tile values
    player->tiley = (short)(player->y >> TILESHIFT);

    //
    // change frame and fire
    //
    gamestate.attackcount -= (short) tics;
    while (gamestate.attackcount <= 0)
    {
        cur = &attackinfo[gamestate.weapon][gamestate.attackframe];
        switch (cur->attack)
        {
            case -1:
                ob->state = &s_player;
                if (!gamestate.ammo)
                {
                    if (!gamestate.ammo2)
                        gamestate.weapon = wp_knife;
                    else if (gamestate.gotrainbowergun)
                        gamestate.weapon = wp_rthrower;
                    // DrawWeapon ();
                }
                else
                {
                    if (gamestate.weapon != gamestate.chosenweapon)
                    {
                        gamestate.weapon = gamestate.chosenweapon;
                        // DrawWeapon ();
                    }
                }
                gamestate.attackframe = gamestate.weaponframe = 0;
                return;

            case 4:
                if (!gamestate.ammo2)
                    if (!gamestate.ammo)
                        break;
                if (buttonstate[bt_attack])
                    gamestate.attackframe -= 2;
            case 1:
                if (!gamestate.ammo2)
                {       // can only happen with chain gun
                    if (!gamestate.ammo)
                    {
                        gamestate.attackframe++;
                        break;
                    }
                }
                
                GunAttack (ob);
                DrawAmmo ();
                break;

            case 2:
                KnifeAttack (ob);
                break;


            case 3:
                if ((gamestate.ammo || gamestate.ammo2) && buttonstate[bt_attack])
                    gamestate.attackframe -= 2;
                break;
        }

        gamestate.attackcount += cur->tics;
        gamestate.attackframe++;
        gamestate.weaponframe =
            attackinfo[gamestate.weapon][gamestate.attackframe].frame;
    }
}



//===========================================================================

/*
===============
=
= T_Player
=
===============
*/

void    T_Player (objtype *ob)
{
    if (gamestate.victoryflag)              // watching the BJ actor
    {
        VictorySpin ();
        return;
    }
    
    if (gamestate.atvendingmenu)
    {
        UseVendingMachine();
        return;
    }
    
    UpdateFace ();
    CheckWeaponChange ();

    if ( buttonstate[bt_use] )
        Cmd_Use ();

    if ( buttonstate[bt_attack] && !buttonheld[bt_attack])
        Cmd_Fire ();
    
    ControlMovement (ob);
    
    if (gamestate.victoryflag)              // watching the BJ actor
        return;

    plux = (word) (player->x >> UNSIGNEDSHIFT);                     // scale to fit in unsigned
    pluy = (word) (player->y >> UNSIGNEDSHIFT);
    player->tilex = (short)(player->x >> TILESHIFT);                // scale to tile values
    player->tiley = (short)(player->y >> TILESHIFT);
}

void    UseVendingMachine ()
{
    if (Keyboard[sc_1])
    {
        if (gamestate.health < 100 && gamestate.money >= 1)
        {
            GetMessage(STR_CANDY_BAR_VEND);
            HealSelf(5);
            GiveMoney(-1);
			SD_PlaySound(AMMO_PICKUP_SND);
        }
        else
        {
            if (gamestate.money < 1)
                GetMessage(STR_NO_FUNDS);
            else
                GetMessage(STR_DONT_NEED);
        }
    }
    if (Keyboard[sc_2])
    {
        if (gamestate.health < 100 && gamestate.money >= 5)
        {
            GetMessage(STR_APPLE);
            HealSelf(10);
            GiveMoney(-5);
			SD_PlaySound(AMMO_PICKUP_SND);
            if (achievements_unlocked[3] == false)
            {
                SetAchievement(3);
            }
        }
        else
        {
            if (gamestate.money < 5)
                GetMessage(STR_NO_FUNDS);
            else
                GetMessage(STR_DONT_NEED);
        }
    }
    if (Keyboard[sc_3])
    {
        if (gamestate.health < 100 && gamestate.money >= 10)
        {
            GetMessage(STR_APPLE_PIE_VEND);
            HealSelf(20);
            GiveMoney(-10);
			SD_PlaySound(AMMO_PICKUP_SND);
        }
        else
        {
            if (gamestate.money < 10)
                GetMessage(STR_NO_FUNDS);
            else
                GetMessage(STR_DONT_NEED);
        }
    }
    if (Keyboard[sc_4])
    {
        if (gamestate.ammo < 250 && gamestate.money >= 15)
        {
            GetMessage(STR_AMMO_VEND);
            GiveAmmo(40);
            GiveMoney(-15);
			SD_PlaySound(AMMO_PICKUP_SND);
        }
        else
        {
            if (gamestate.money < 15)
                GetMessage(STR_NO_FUNDS);
            else
                GetMessage(STR_DONT_NEED);
        }
    }
    if (Keyboard[sc_5])
    {
        if (gamestate.ammo2 < 150 && gamestate.money >= 25 && gamestate.gotrainbowergun == true)
        {
            GetMessage(STR_AMMO2_VEND);
            GiveRBAmmo(60);
            GiveMoney(-25);
			SD_PlaySound(AMMO_PICKUP_SND);
        }
        else
        {
            if (gamestate.money < 25)
                GetMessage(STR_NO_FUNDS);
            else
                GetMessage(STR_DONT_NEED);
        }
    }
    if (Keyboard[sc_Q])
    {
        gamestate.atvendingmenu = false;
    }
    if (Keyboard[sc_Escape])
    {
        gamestate.atvendingmenu = false;
    }
}
