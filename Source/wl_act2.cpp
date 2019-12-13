// WL_ACT2.C

#include <stdio.h>
#include <math.h>
#include "wl_def.h"
#pragma hdrstop

/*
=============================================================================

                               LOCAL CONSTANTS

=============================================================================
*/

#define PROJECTILESIZE  0xc000l
#define BJROCKETSIZE    0x6000l    // Poet: adding a missile launcher

#define BJRUNSPEED      2048
#define BJJUMPSPEED     680


/*
=============================================================================

                              GLOBAL VARIABLES

=============================================================================
*/



/*
=============================================================================

                              LOCAL VARIABLES

=============================================================================
*/


dirtype dirtable[9] = {northwest,north,northeast,west,nodir,east,
    southwest,south,southeast};

short starthitpoints[4][NUMENEMIES] =
//
// BABY MODE
//
{
    {
        25,   // guards
        50,   // officer
        100,  // SS
        1,    // dogs
        850,  // Hans
        850,  // Schabbs
        200,  // fake hitler
        1200,  // mecha hitler
        45,   // mutants
        25,   // ghosts
        25,   // ghosts
        25,   // ghosts
        25,   // ghosts

        850,  // Gretel
        850,  // Gift
        850,  // Fat
        5,    // en_spectre,
        1450, // en_angel,
        850,  // en_trans,
        1050, // en_uber,
        950,  // en_will,
        1250  // en_death
    },
    //
    // DON'T HURT ME MODE
    //
    {
        25,   // guards
        50,   // officer
        100,  // SS
        1,    // dogs
        950,  // Hans
        950,  // Schabbs
        300,  // fake hitler
        1920,  // mecha hitler
        55,   // mutants
        25,   // ghosts
        25,   // ghosts
        25,   // ghosts
        25,   // ghosts

        950,  // Gretel
        950,  // Gift
        950,  // Fat
        10,   // en_spectre,
        1550, // en_angel,
        950,  // en_trans,
        1150, // en_uber,
        1050, // en_will,
        1350  // en_death
    },
    //
    // BRING 'EM ON MODE
    //
    {
        25,   // guards
        50,   // officer
        100,  // SS
        1,    // dogs

        1050, // Hans
        1550, // Schabbs
        400,  // fake hitler
        2240, // mecha hitler

        55,   // mutants
        25,   // ghosts
        25,   // ghosts
        25,   // ghosts
        25,   // ghosts

        1050, // Gretel
        1050, // Gift
        1050, // Fat
        15,   // en_spectre,
        1650, // en_angel,
        1050, // en_trans,
        1250, // en_uber,
        1150, // en_will,
        1450  // en_death
    },
    //
    // DEATH INCARNATE MODE
    //
    {
        25,   // guards
        50,   // officer
        100,  // SS
        1,    // dogs

        1200, // Hans
        2400, // Schabbs
        500,  // fake hitler
        2560, // mecha hitler

        65,   // mutants
        25,   // ghosts
        25,   // ghosts
        25,   // ghosts
        25,   // ghosts

        1200, // Gretel
        1200, // Gift
        1200, // Fat
        25,   // en_spectre,
        2000, // en_angel,
        1200, // en_trans,
        1400, // en_uber,
        1300, // en_will,
        1600  // en_death
    }
};

void    A_StartDeathCam (objtype *ob);


void    T_Path (objtype *ob);
void    T_Shoot (objtype *ob);
void    T_Bite (objtype *ob);
void    T_DogChase (objtype *ob);
void    T_Chase (objtype *ob);
void    T_Projectile (objtype *ob);
void    T_Stand (objtype *ob);

void A_DeathScream (objtype *ob);

extern  statetype s_rocket;
extern  statetype s_smoke1;
extern  statetype s_smoke2;
extern  statetype s_smoke3;
extern  statetype s_smoke4;
extern  statetype s_boom2;
extern  statetype s_boom3;

void A_Smoke (objtype *ob);

statetype s_rocket              = {false,SPR_ROCKET_1,3,(statefunc)T_Projectile,NULL,&s_rocket};

statetype s_boom1               = {false,SPR_SMOKE_1,6,NULL,NULL,NULL};

void    T_Schabb (objtype *ob);
void    T_SchabbThrow (objtype *ob);
void    T_Fake (objtype *ob);
void    T_FakeFire (objtype *ob);
void    T_Ghosts (objtype *ob);

void A_Slurpie (objtype *ob);
void A_HitlerMorph (objtype *ob);
void A_UndashMorph (objtype *ob);
void A_MechaSound (objtype *ob);

int lhintnum[30] =
{
    5,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

/*
===================
=
= ProjectileTryMove
=
= returns true if move ok
===================
*/

#define PROJSIZE        0x2000

boolean ProjectileTryMove (objtype *ob)
{
    int      xl,yl,xh,yh,x,y;
    objtype *check;

    xl = (ob->x-PROJSIZE) >> TILESHIFT;
    yl = (ob->y-PROJSIZE) >> TILESHIFT;

    xh = (ob->x+PROJSIZE) >> TILESHIFT;
    yh = (ob->y+PROJSIZE) >> TILESHIFT;

    //
    // check for solid walls
    //
    for (y=yl;y<=yh;y++)
        for (x=xl;x<=xh;x++)
        {
            check = actorat[x][y];
            if (check && !ISPOINTER(check))
                return false;
        }

        return true;
}



/*
=================
=
= T_Projectile
=
=================
*/


void T_Projectile (objtype *ob)
{
    long    deltax,deltay;
    int             damage;
    long    speed;
    objtype *check;
    
    
    
    speed = (long)ob->speed*tics;
    
    deltax = FixedMul(speed,costable[ob->angle]);
    deltay = -FixedMul(speed,sintable[ob->angle]);
    
    if (deltax>0x10000l)
        deltax = 0x10000l;
    if (deltay>0x10000l)
        deltay = 0x10000l;
    
    ob->x += deltax;
    ob->y += deltay;
    
    if (!ProjectileTryMove (ob))
    {
        if (ob->obclass == rocketobj)
        {
            PlaySoundLocActor(MISSILE_EXPLODE_SND,ob);
            ob->state = &s_boom1;
        }
        else if (ob->obclass == needleobj || ob->obclass == p_rainballobj)
        {
            PlaySoundLocActor(MISSILE_EXPLODE_SND,ob);
            ob->state = NULL;
        }
        else
            ob->state = NULL;               // mark for removal
        
        return;
    }
    
    // Poet: adding a missile launcher. New section: the player's rocket
    
    if (ob->obclass == p_rainballobj)
    {
        // Check if player's rocket hit anybody
        check = objlist ;
        while (check)
        {
            if (check->flags & FL_SHOOTABLE)
            {
                deltax = LABS(ob->x - check->x);
                deltay = LABS(ob->y - check->y);
                
                if (deltax < BJROCKETSIZE && deltay < BJROCKETSIZE)
                {
                    if (check->obclass == bossobj ||   // APN: keep going, except for Bosses
                        check->obclass == schabbobj ||
                        check->obclass == g4dashobj)
                    {
                        PlaySoundLocActor(MISSILE_EXPLODE_SND,ob);
                        ob->state = &s_boom1;
                    }
                    
                    DamageActor (check, 80);
                    
                }
            }
            check = check->next ;
        }
    }
    
    else if (ob->obclass == fireobj)
    {
        // Check if player's fire hit anybody
        check = objlist ;
        while (check)
        {
            if (check->flags & FL_SHOOTABLE)
            {
                deltax = LABS(ob->x - check->x);
                deltay = LABS(ob->y - check->y);
                
                if (deltax < BJROCKETSIZE && deltay < BJROCKETSIZE)
                {
                    if (check->obclass == playerobj)
                    {
                        PlaySoundLocActor(MISSILE_EXPLODE_SND,ob);
                        DamageActor (check, 55);  // APN: fire weaker than missile
                        ob->state = &s_boom1;
                    }
                    
                }
            }
            check = check->next ;
        }
    }
    
    else
        
        // Check if player hit by anything
    {
        deltax = LABS(ob->x - player->x);
        deltay = LABS(ob->y - player->y);
        
        if (deltax < PROJECTILESIZE && deltay < PROJECTILESIZE && ob->obclass != p_rainballobj)
        {       // hit the player
            switch (ob->obclass)
            {
                    
                case needleobj:
                    damage = (US_RndT() >>3) + 20;
                    break;
                case rocketobj:
                    damage = (US_RndT() >>3) + 30;
                    break;
                case fireobj:
                    damage = (US_RndT() >>3);
                    break;
                default:
                    break;
            }
            
            TakeDamage (damage,ob);
            if (ob->obclass==needleobj)
            {
                ob->state=&s_boom1;   // DHW: make projectile explode (the_fish)
                PlaySoundLocActor(MISSILE_EXPLODE_SND,ob);
            }
            //                else if (ob->obclass==drocketobj)
            //                {
            //                    ob->state=&s_hboom1;   // DHW: make rocket explode (the_fish)
            //                    PlaySoundLocActor(MISSILEHITSND,ob);
            //                }
            else
                ob->state = NULL;      // mark for removal
            return;
        }
    }
    
    ob->tilex = ob->x >> TILESHIFT;
    ob->tiley = ob->y >> TILESHIFT;
    
}




/*
=============================================================================

GUARD

=============================================================================
*/

//
// guards
//

extern  statetype s_grdstand;

extern  statetype s_grdpath1;
extern  statetype s_grdpath1s;
extern  statetype s_grdpath2;
extern  statetype s_grdpath3;
extern  statetype s_grdpath3s;
extern  statetype s_grdpath4;

extern  statetype s_grdpain;
extern  statetype s_grdpain1;

extern  statetype s_grdgiveup;

extern  statetype s_grdshoot1;
extern  statetype s_grdshoot2;
extern  statetype s_grdshoot3;
extern  statetype s_grdshoot4;

extern  statetype s_grdchase1;
extern  statetype s_grdchase1s;
extern  statetype s_grdchase2;
extern  statetype s_grdchase3;
extern  statetype s_grdchase3s;
extern  statetype s_grdchase4;

extern  statetype s_grddie1;
extern  statetype s_grddie1d;
extern  statetype s_grddie2;
extern  statetype s_grddie3;
extern  statetype s_grddie4;

statetype s_grdstand            = {false,SPR_GRD_S_1,0,(statefunc)T_Stand,NULL,&s_grdstand};

statetype s_grdpath1            = {false,SPR_GRD_W1_1,20,(statefunc)T_Path,NULL,&s_grdpath1s};
statetype s_grdpath1s           = {false,SPR_GRD_W1_1,5,NULL,NULL,&s_grdpath2};
statetype s_grdpath2            = {false,SPR_GRD_W2_1,15,(statefunc)T_Path,NULL,&s_grdpath3};
statetype s_grdpath3            = {false,SPR_GRD_W3_1,20,(statefunc)T_Path,NULL,&s_grdpath3s};
statetype s_grdpath3s           = {false,SPR_GRD_W3_1,5,NULL,NULL,&s_grdpath4};
statetype s_grdpath4            = {false,SPR_GRD_W4_1,15,(statefunc)T_Path,NULL,&s_grdpath1};

statetype s_grdpain             = {2,SPR_GRD_PAIN_1,10,NULL,NULL,&s_grdchase1};
statetype s_grdpain1            = {2,SPR_GRD_PAIN_2,10,NULL,NULL,&s_grdchase1};

statetype s_grdshoot1           = {false,SPR_GRD_SHOOT1,20,NULL,NULL,&s_grdshoot2};
statetype s_grdshoot2           = {false,SPR_GRD_SHOOT2,20,NULL,(statefunc)T_Shoot,&s_grdshoot3};
statetype s_grdshoot3           = {false,SPR_GRD_SHOOT3,20,NULL,NULL,&s_grdchase1};

statetype s_grdchase1           = {false,SPR_GRD_W1_1,10,(statefunc)T_Chase,NULL,&s_grdchase1s};
statetype s_grdchase1s          = {false,SPR_GRD_W1_1,3,NULL,NULL,&s_grdchase2};
statetype s_grdchase2           = {false,SPR_GRD_W2_1,8,(statefunc)T_Chase,NULL,&s_grdchase3};
statetype s_grdchase3           = {false,SPR_GRD_W3_1,10,(statefunc)T_Chase,NULL,&s_grdchase3s};
statetype s_grdchase3s          = {false,SPR_GRD_W3_1,3,NULL,NULL,&s_grdchase4};
statetype s_grdchase4           = {false,SPR_GRD_W4_1,8,(statefunc)T_Chase,NULL,&s_grdchase1};

statetype s_grddie1             = {false,SPR_GRD_DIE_1,15,NULL,(statefunc)A_DeathScream,&s_grddie2};
statetype s_grddie2             = {false,SPR_GRD_DIE_2,15,NULL,NULL,&s_grddie3};
statetype s_grddie3             = {false,SPR_GRD_DIE_3,15,NULL,NULL,&s_grddie4};
statetype s_grddie4             = {false,SPR_GRD_DEAD,0,NULL,NULL,&s_grddie4};


#ifndef SPEAR
#endif

//
// dogs
//

extern  statetype s_dogpath1;
extern  statetype s_dogpath1s;
extern  statetype s_dogpath2;
extern  statetype s_dogpath3;
extern  statetype s_dogpath3s;
extern  statetype s_dogpath4;

extern  statetype s_dogjump1;
extern  statetype s_dogjump2;
extern  statetype s_dogjump3;
extern  statetype s_dogjump4;
extern  statetype s_dogjump5;

extern  statetype s_dogchase1;
extern  statetype s_dogchase1s;
extern  statetype s_dogchase2;
extern  statetype s_dogchase3;
extern  statetype s_dogchase3s;
extern  statetype s_dogchase4;

extern  statetype s_dogdie1;
extern  statetype s_dogdie1d;
extern  statetype s_dogdie2;
extern  statetype s_dogdie3;
extern  statetype s_dogdead;


//
// officers
//

extern  statetype s_ofcstand;

extern  statetype s_ofcpath1;
extern  statetype s_ofcpath1s;
extern  statetype s_ofcpath2;
extern  statetype s_ofcpath3;
extern  statetype s_ofcpath3s;
extern  statetype s_ofcpath4;

extern  statetype s_ofcpain;
extern  statetype s_ofcpain1;

extern  statetype s_ofcgiveup;

extern  statetype s_ofcshoot1;
extern  statetype s_ofcshoot2;
extern  statetype s_ofcshoot3;
extern  statetype s_ofcshoot4;

extern  statetype s_ofcchase1;
extern  statetype s_ofcchase1s;
extern  statetype s_ofcchase2;
extern  statetype s_ofcchase3;
extern  statetype s_ofcchase3s;
extern  statetype s_ofcchase4;

extern  statetype s_ofcdie1;
extern  statetype s_ofcdie2;
extern  statetype s_ofcdie3;
extern  statetype s_ofcdie4;
extern  statetype s_ofcdie5;

statetype s_ofcstand            = {false,SPR_OFC_S_1,0,(statefunc)T_Stand,NULL,&s_ofcstand};

statetype s_ofcpath1            = {false,SPR_OFC_W1_1,20,(statefunc)T_Path,NULL,&s_ofcpath1s};
statetype s_ofcpath1s           = {false,SPR_OFC_W1_1,5,NULL,NULL,&s_ofcpath2};
statetype s_ofcpath2            = {false,SPR_OFC_W2_1,15,(statefunc)T_Path,NULL,&s_ofcpath3};
statetype s_ofcpath3            = {false,SPR_OFC_W3_1,20,(statefunc)T_Path,NULL,&s_ofcpath3s};
statetype s_ofcpath3s           = {false,SPR_OFC_W3_1,5,NULL,NULL,&s_ofcpath4};
statetype s_ofcpath4            = {false,SPR_OFC_W4_1,15,(statefunc)T_Path,NULL,&s_ofcpath1};

statetype s_ofcpain             = {2,SPR_OFC_PAIN_1,10,NULL,NULL,&s_ofcchase1};
statetype s_ofcpain1            = {2,SPR_OFC_PAIN_2,10,NULL,NULL,&s_ofcchase1};

statetype s_ofcshoot1           = {false,SPR_OFC_SHOOT1,6,NULL,NULL,&s_ofcshoot2};
statetype s_ofcshoot2           = {false,SPR_OFC_SHOOT2,20,NULL,(statefunc)T_Shoot,&s_ofcshoot3};
statetype s_ofcshoot3           = {false,SPR_OFC_SHOOT3,10,NULL,NULL,&s_ofcchase1};

statetype s_ofcchase1           = {false,SPR_OFC_W1_1,10,(statefunc)T_Chase,NULL,&s_ofcchase1s};
statetype s_ofcchase1s          = {false,SPR_OFC_W1_1,3,NULL,NULL,&s_ofcchase2};
statetype s_ofcchase2           = {false,SPR_OFC_W2_1,8,(statefunc)T_Chase,NULL,&s_ofcchase3};
statetype s_ofcchase3           = {false,SPR_OFC_W3_1,10,(statefunc)T_Chase,NULL,&s_ofcchase3s};
statetype s_ofcchase3s          = {false,SPR_OFC_W3_1,3,NULL,NULL,&s_ofcchase4};
statetype s_ofcchase4           = {false,SPR_OFC_W4_1,8,(statefunc)T_Chase,NULL,&s_ofcchase1};

statetype s_ofcdie1             = {false,SPR_OFC_DIE_1,11,NULL,(statefunc)A_DeathScream,&s_ofcdie2};
statetype s_ofcdie2             = {false,SPR_OFC_DIE_2,11,NULL,NULL,&s_ofcdie3};
statetype s_ofcdie3             = {false,SPR_OFC_DIE_3,11,NULL,NULL,&s_ofcdie4};
statetype s_ofcdie4             = {false,SPR_OFC_DIE_4,11,NULL,NULL,&s_ofcdie5};
statetype s_ofcdie5             = {false,SPR_OFC_DEAD,0,NULL,NULL,&s_ofcdie5};


//
// mutant
//

extern  statetype s_mutstand;

extern  statetype s_mutpath1;
extern  statetype s_mutpath1s;
extern  statetype s_mutpath2;
extern  statetype s_mutpath3;
extern  statetype s_mutpath3s;
extern  statetype s_mutpath4;

extern  statetype s_mutpain;
extern  statetype s_mutpain1;

extern  statetype s_mutgiveup;

extern  statetype s_mutshoot1;
extern  statetype s_mutshoot2;
extern  statetype s_mutshoot3;
extern  statetype s_mutshoot4;

extern  statetype s_mutchase1;
extern  statetype s_mutchase1s;
extern  statetype s_mutchase2;
extern  statetype s_mutchase3;
extern  statetype s_mutchase3s;
extern  statetype s_mutchase4;

extern  statetype s_mutdie1;
extern  statetype s_mutdie2;
extern  statetype s_mutdie3;
extern  statetype s_mutdie4;
extern  statetype s_mutdie5;

statetype s_mutstand            = {false,SPR_MUT_S_1,0,(statefunc)T_Stand,NULL,&s_mutstand};

statetype s_mutpath1            = {false,SPR_MUT_W1_1,20,(statefunc)T_Path,NULL,&s_mutpath1s};
statetype s_mutpath1s           = {false,SPR_MUT_W1_1,5,NULL,NULL,&s_mutpath2};
statetype s_mutpath2            = {false,SPR_MUT_W2_1,15,(statefunc)T_Path,NULL,&s_mutpath3};
statetype s_mutpath3            = {false,SPR_MUT_W3_1,20,(statefunc)T_Path,NULL,&s_mutpath3s};
statetype s_mutpath3s           = {false,SPR_MUT_W3_1,5,NULL,NULL,&s_mutpath4};
statetype s_mutpath4            = {false,SPR_MUT_W4_1,15,(statefunc)T_Path,NULL,&s_mutpath1};

statetype s_mutpain             = {2,SPR_MUT_PAIN_1,10,NULL,NULL,&s_mutchase1};
statetype s_mutpain1            = {2,SPR_MUT_PAIN_2,10,NULL,NULL,&s_mutchase1};

statetype s_mutshoot1           = {false,SPR_MUT_SHOOT1,6,NULL,(statefunc)T_Shoot,&s_mutshoot2};
statetype s_mutshoot2           = {false,SPR_MUT_SHOOT2,20,NULL,NULL,&s_mutshoot3};
statetype s_mutshoot3           = {false,SPR_MUT_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_mutshoot4};
statetype s_mutshoot4           = {false,SPR_MUT_SHOOT4,20,NULL,NULL,&s_mutchase1};

statetype s_mutchase1           = {false,SPR_MUT_W1_1,10,(statefunc)T_Chase,NULL,&s_mutchase1s};
statetype s_mutchase1s          = {false,SPR_MUT_W1_1,3,NULL,NULL,&s_mutchase2};
statetype s_mutchase2           = {false,SPR_MUT_W2_1,8,(statefunc)T_Chase,NULL,&s_mutchase3};
statetype s_mutchase3           = {false,SPR_MUT_W3_1,10,(statefunc)T_Chase,NULL,&s_mutchase3s};
statetype s_mutchase3s          = {false,SPR_MUT_W3_1,3,NULL,NULL,&s_mutchase4};
statetype s_mutchase4           = {false,SPR_MUT_W4_1,8,(statefunc)T_Chase,NULL,&s_mutchase1};

statetype s_mutdie1             = {false,SPR_MUT_DIE_1,7,NULL,(statefunc)A_DeathScream,&s_mutdie2};
statetype s_mutdie2             = {false,SPR_MUT_DIE_2,7,NULL,NULL,&s_mutdie3};
statetype s_mutdie3             = {false,SPR_MUT_DIE_3,7,NULL,NULL,&s_mutdie4};
statetype s_mutdie4             = {false,SPR_MUT_DIE_4,7,NULL,NULL,&s_mutdie5};
statetype s_mutdie5             = {false,SPR_MUT_DEAD,0,NULL,NULL,&s_mutdie5};


//
// SS
//

extern  statetype s_ssstand;

extern  statetype s_sspath1;
extern  statetype s_sspath1s;
extern  statetype s_sspath2;
extern  statetype s_sspath3;
extern  statetype s_sspath3s;
extern  statetype s_sspath4;

extern  statetype s_sspain;
extern  statetype s_sspain1;

extern  statetype s_ssshoot1;
extern  statetype s_ssshoot2;
extern  statetype s_ssshoot3;
extern  statetype s_ssshoot4;
extern  statetype s_ssshoot5;
extern  statetype s_ssshoot6;
extern  statetype s_ssshoot7;
extern  statetype s_ssshoot8;
extern  statetype s_ssshoot9;

extern  statetype s_sschase1;
extern  statetype s_sschase1s;
extern  statetype s_sschase2;
extern  statetype s_sschase3;
extern  statetype s_sschase3s;
extern  statetype s_sschase4;

extern  statetype s_ssdie1;
extern  statetype s_ssdie2;
extern  statetype s_ssdie3;
extern  statetype s_ssdie4;

statetype s_ssstand             = {false,SPR_SS_S_1,0,(statefunc)T_Stand,NULL,&s_ssstand};

statetype s_sspath1             = {false,SPR_SS_W1_1,20,(statefunc)T_Path,NULL,&s_sspath1s};
statetype s_sspath1s            = {false,SPR_SS_W1_1,5,NULL,NULL,&s_sspath2};
statetype s_sspath2             = {false,SPR_SS_W2_1,15,(statefunc)T_Path,NULL,&s_sspath3};
statetype s_sspath3             = {false,SPR_SS_W3_1,20,(statefunc)T_Path,NULL,&s_sspath3s};
statetype s_sspath3s            = {false,SPR_SS_W3_1,5,NULL,NULL,&s_sspath4};
statetype s_sspath4             = {false,SPR_SS_W4_1,15,(statefunc)T_Path,NULL,&s_sspath1};

statetype s_sspain              = {2,SPR_SS_PAIN_1,10,NULL,NULL,&s_sschase1};
statetype s_sspain1             = {2,SPR_SS_PAIN_2,10,NULL,NULL,&s_sschase1};

statetype s_ssshoot1            = {false,SPR_SS_SHOOT1,20,NULL,NULL,&s_ssshoot2};
statetype s_ssshoot2            = {false,SPR_SS_SHOOT2,20,NULL,(statefunc)T_Shoot,&s_ssshoot3};
statetype s_ssshoot3            = {false,SPR_SS_SHOOT3,10,NULL,NULL,&s_ssshoot4};
statetype s_ssshoot4            = {false,SPR_SS_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_ssshoot5};
statetype s_ssshoot5            = {false,SPR_SS_SHOOT3,10,NULL,NULL,&s_ssshoot6};
statetype s_ssshoot6            = {false,SPR_SS_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_ssshoot7};
statetype s_ssshoot7            = {false,SPR_SS_SHOOT3,10,NULL,NULL,&s_ssshoot8};
statetype s_ssshoot8            = {false,SPR_SS_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_ssshoot9};
statetype s_ssshoot9            = {false,SPR_SS_SHOOT3,10,NULL,NULL,&s_sschase1};

statetype s_sschase1            = {false,SPR_SS_W1_1,10,(statefunc)T_Chase,NULL,&s_sschase1s};
statetype s_sschase1s           = {false,SPR_SS_W1_1,3,NULL,NULL,&s_sschase2};
statetype s_sschase2            = {false,SPR_SS_W2_1,8,(statefunc)T_Chase,NULL,&s_sschase3};
statetype s_sschase3            = {false,SPR_SS_W3_1,10,(statefunc)T_Chase,NULL,&s_sschase3s};
statetype s_sschase3s           = {false,SPR_SS_W3_1,3,NULL,NULL,&s_sschase4};
statetype s_sschase4            = {false,SPR_SS_W4_1,8,(statefunc)T_Chase,NULL,&s_sschase1};

statetype s_ssdie1              = {false,SPR_SS_DIE_1,15,NULL,(statefunc)A_DeathScream,&s_ssdie2};
statetype s_ssdie2              = {false,SPR_SS_DIE_2,15,NULL,NULL,&s_ssdie3};
statetype s_ssdie3              = {false,SPR_SS_DIE_3,15,NULL,NULL,&s_ssdie4};
statetype s_ssdie4              = {false,SPR_SS_DEAD,0,NULL,NULL,&s_ssdie4};

//
// hans
//
extern  statetype s_bossstand;

extern  statetype s_bosschase1;
extern  statetype s_bosschase1s;
extern  statetype s_bosschase2;
extern  statetype s_bosschase3;
extern  statetype s_bosschase3s;
extern  statetype s_bosschase4;

extern  statetype s_bossdie1;
extern  statetype s_bossdie2;
extern  statetype s_bossdie3;
extern  statetype s_bossdie4;

extern  statetype s_bossshoot1;
extern  statetype s_bossshoot2;
extern  statetype s_bossshoot3;
extern  statetype s_bossshoot4;
extern  statetype s_bossshoot5;
extern  statetype s_bossshoot6;
extern  statetype s_bossshoot7;
extern  statetype s_bossshoot8;

extern  statetype s_compterm;

extern  statetype s_aurora1;
extern  statetype s_aurora2;
extern  statetype s_aurora3;


statetype s_bossstand           = {false,SPR_BOSS_W1,0,(statefunc)T_Stand,NULL,&s_bossstand};

statetype s_bosschase1          = {false,SPR_BOSS_W1,10,(statefunc)T_Chase,NULL,&s_bosschase1s};
statetype s_bosschase1s         = {false,SPR_BOSS_W1,3,NULL,NULL,&s_bosschase2};
statetype s_bosschase2          = {false,SPR_BOSS_W2,8,(statefunc)T_Chase,NULL,&s_bosschase3};
statetype s_bosschase3          = {false,SPR_BOSS_W3,10,(statefunc)T_Chase,NULL,&s_bosschase3s};
statetype s_bosschase3s         = {false,SPR_BOSS_W3,3,NULL,NULL,&s_bosschase4};
statetype s_bosschase4          = {false,SPR_BOSS_W4,8,(statefunc)T_Chase,NULL,&s_bosschase1};

statetype s_bossdie1            = {false,SPR_BOSS_DIE1,15,NULL,(statefunc)A_DeathScream,&s_bossdie2};
statetype s_bossdie2            = {false,SPR_BOSS_DIE2,15,NULL,NULL,&s_bossdie3};
statetype s_bossdie3            = {false,SPR_BOSS_DIE3,15,NULL,NULL,&s_bossdie4};
statetype s_bossdie4            = {false,SPR_BOSS_DEAD,0,NULL,NULL,&s_bossdie4};

statetype s_bossshoot1          = {false,SPR_BOSS_SHOOT1,30,NULL,NULL,&s_bossshoot2};
statetype s_bossshoot2          = {false,SPR_BOSS_SHOOT2,10,NULL,(statefunc)T_FakeFire,&s_bossshoot3};
statetype s_bossshoot3          = {false,SPR_BOSS_SHOOT3,10,NULL,(statefunc)T_FakeFire,&s_bossshoot4};
statetype s_bossshoot4          = {false,SPR_BOSS_SHOOT2,10,NULL,(statefunc)T_FakeFire,&s_bossshoot5};
statetype s_bossshoot5          = {false,SPR_BOSS_SHOOT3,10,NULL,(statefunc)T_FakeFire,&s_bossshoot6};
statetype s_bossshoot6          = {false,SPR_BOSS_SHOOT2,10,NULL,(statefunc)T_FakeFire,&s_bossshoot7};
statetype s_bossshoot7          = {false,SPR_BOSS_SHOOT3,10,NULL,(statefunc)T_FakeFire,&s_bossshoot8};
statetype s_bossshoot8          = {false,SPR_BOSS_SHOOT1,10,NULL,NULL,&s_bosschase1};

statetype s_compterm            = {false,SPR_STAT_46,10,NULL,NULL,&s_compterm};

statetype s_aurora1             = {false,SPR_AURORA_DAWN,10,(statefunc)T_Stand,NULL,&s_aurora1};
statetype s_aurora2             = {false,SPR_AURORA_DAWN,10,NULL,NULL,&s_aurora3};
statetype s_aurora3             = {false,SPR_AURORA_DAWN,10,NULL,NULL,NULL};

extern statetype s_pegcontrol_a;
extern statetype s_pegcontrol_b;

statetype s_pegcontrol_a   = {false,SPR_STAT_10,10,NULL,NULL,&s_pegcontrol_a};
statetype s_pegcontrol_b   = {false,SPR_STAT_10,10,NULL,NULL,&s_pegcontrol_b};

extern statetype s_pegmapspot_a;
extern statetype s_pegmapspot_b;
extern statetype s_pegmapspot_c;
extern statetype s_pegmapspot_d;
extern statetype s_pegmapspot_e;
extern statetype s_pegmapspot_f;

statetype s_pegmapspot_a = {false,SPR_HYPO1,10,NULL,NULL,&s_pegmapspot_a};
statetype s_pegmapspot_b = {false,SPR_EXPLOSION_A,10,NULL,(statefunc)A_DestroyPegasusDevice,&s_pegmapspot_c};
statetype s_pegmapspot_c = {false,SPR_EXPLOSION_B,10,NULL,NULL,&s_pegmapspot_d};
statetype s_pegmapspot_d = {false,SPR_EXPLOSION_C,10,NULL,NULL,&s_pegmapspot_e};
statetype s_pegmapspot_e = {false,SPR_EXPLOSION_D,10,NULL,NULL,&s_pegmapspot_f};
statetype s_pegmapspot_f = {false,SPR_EXPLOSION_E,10,NULL,NULL,NULL};


/*
===============
=
= SpawnStand
=
===============
*/

void SpawnStand (enemy_t which, int tilex, int tiley, int dir)
{
    word *map;
    word tile;

    switch (which)
    {
        case en_guard:
            SpawnNewObj (tilex,tiley,&s_grdstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_officer:
            SpawnNewObj (tilex,tiley,&s_ofcstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_mutant:
            SpawnNewObj (tilex,tiley,&s_mutstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_ss:
            SpawnNewObj (tilex,tiley,&s_ssstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        default:
            break;
    }


    map = mapsegs[0]+(tiley<<mapshift)+tilex;
    tile = *map;
    if (tile == AMBUSHTILE)
    {
        tilemap[tilex][tiley] = 0;

        if (*(map+1) >= AREATILE)
            tile = *(map+1);
        if (*(map-mapwidth) >= AREATILE)
            tile = *(map-mapwidth);
        if (*(map+mapwidth) >= AREATILE)
            tile = *(map+mapwidth);
        if ( *(map-1) >= AREATILE)
            tile = *(map-1);

        *map = tile;
        newobj->areanumber = tile-AREATILE;

        newobj->flags |= FL_AMBUSH;
    }

    newobj->obclass = (classtype)(guardobj + which);
    newobj->hitpoints = starthitpoints[gamestate.difficulty][which];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE;
}

/*
= SpawnCompTerminal
*/
void SpawnCompTerminal(int tilex, int tiley)
{
    word *map;
    word tile;
    
    SpawnNewObj (tilex,tiley,&s_compterm);
    newobj->speed = 0;
    
    
    map = mapsegs[0]+(tiley<<mapshift)+tilex;
    tile = *map;
    
    newobj->obclass = compobj;
    newobj->hitpoints = 0;
    newobj->dir = nodir;
    
    newobj->flags |= FL_COMP_TERM;
    // newobj->flags |= FL_SHOOTABLE;
}


/*
===============
=
= SpawnDeadGuard
=
===============
*/

void SpawnDeadGuard (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_grddie4);
    DEMOIF_SDL
    {
        newobj->flags |= FL_NONMARK;    // walk through moving enemy fix
    }
    newobj->obclass = inertobj;
}



#ifndef SPEAR
/*
===============
=
= SpawnBoss
=
===============
*/

void SpawnBoss (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_bossstand);
    newobj->speed = SPDPATROL;

    newobj->obclass = bossobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_boss];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}

void SpawnAurora (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_aurora1);
    newobj->speed = 0;
    
    newobj->obclass = auroraobj;
    newobj->hitpoints = 1000;
    newobj->dir = nodir;
}


void SpawnPegasusDeviceControl (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_pegcontrol_a);
    newobj->speed = 0;
    
    newobj->obclass = pdevobj;
    newobj->hitpoints = 1;  // once it's >= 4, do the action
    newobj->dir = nodir;
    newobj->flags |= FL_AMBUSH;
}

void SpawnPegasusDeviceSpots (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_pegcontrol_a);
    newobj->speed = 0;
    
    newobj->obclass = pdevmspotobj;
    newobj->hitpoints = 30;
    newobj->dir = nodir;
    newobj->flags |= FL_AMBUSH |  FL_SHOOTABLE;
}
#endif

/*
===============
=
= SpawnPatrol
=
===============
*/

void SpawnPatrol (enemy_t which, int tilex, int tiley, int dir)
{
    switch (which)
    {
        case en_guard:
            SpawnNewObj (tilex,tiley,&s_grdpath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_officer:
            SpawnNewObj (tilex,tiley,&s_ofcpath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_ss:
            SpawnNewObj (tilex,tiley,&s_sspath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_mutant:
            SpawnNewObj (tilex,tiley,&s_mutpath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;
        default:
            break;
    }

    newobj->obclass = (classtype)(guardobj+which);
    newobj->dir = (dirtype)(dir*2);
    newobj->hitpoints = starthitpoints[gamestate.difficulty][which];
    newobj->distance = TILEGLOBAL;
    newobj->flags |= FL_SHOOTABLE;
    newobj->active = ac_yes;

    actorat[newobj->tilex][newobj->tiley] = NULL;           // don't use original spot

    switch (dir)
    {
        case 0:
            newobj->tilex++;
            break;
        case 1:
            newobj->tiley--;
            break;
        case 2:
            newobj->tilex--;
            break;
        case 3:
            newobj->tiley++;
            break;
    }

    actorat[newobj->tilex][newobj->tiley] = newobj;
}

/*
==================
=
= A_DeathScream
=
==================
*/

void A_DeathScream (objtype *ob)
{
    int gd_snds[3]={ GUARD_DIE1_SND,
        GUARD_DIE2_SND, GUARD_DIE3_SND
    };
    
    int en_snds[2]={ ENGIE_DIE1_SND,
        ENGIE_DIE2_SND
    };

    switch (ob->obclass)
    {
        case mutantobj:
            PlaySoundLocActor(DERELLE_NOISE_SND,ob);
            break;

        case guardobj:
            PlaySoundLocActor(gd_snds[US_RndT()%3],ob);
            break;
        case officerobj:
            PlaySoundLocActor(en_snds[US_RndT()%2],ob);
            break;
        case ssobj:
            PlaySoundLocActor(PEGPOL_DIE_SND,ob);
            break;
        case bossobj:
            SD_PlaySound(BOSS1_DIE_SND);                         // JAB
            break;
        case schabbobj:
            SD_PlaySound(ATMOS_DIE_SND);
            break;
        case g4dashobj:
            SD_PlaySound(RDASH_DIE_SND);
            break;
        case glassobj:
            PlaySoundLocActor(GLASS_BREAK_SND, ob);
            break;
        default:
            break;
    }
}

void A_OpenDestDoor(void)
{
    for (int i = 0; i < MAXDOORS; i++)
    {
        if (doorobjlist[i].tag == 667)
        {
            OpenDoor(i);
            break;
        }
    }
}

void A_IncrementSwitchCount (objtype *ob)
{
    switch (ob->obclass)
    {
        case pdevobj:
            ob->hitpoints++;
            if (ob->hitpoints >= 4)
                A_OpenDestDoor();
            break;
        default:
            break;
    }
}

void A_DestroyPegasusDevice (objtype *ob)
{
    if (ob->obclass == pdevmspotobj)
    SD_PlaySound(MISSILE_EXPLODE_SND);
}

/*
=============================================================================

                            SCHABBS / GIFT / FAT

=============================================================================
*/

#ifndef SPEAR

//
// schabb
//
extern  statetype s_schabbstand;

extern  statetype s_schabbchase1;
extern  statetype s_schabbchase1s;
extern  statetype s_schabbchase2;
extern  statetype s_schabbchase3;
extern  statetype s_schabbchase3s;
extern  statetype s_schabbchase4;

extern  statetype s_schabbdie1;
extern  statetype s_schabbdie2;
extern  statetype s_schabbdie3;
extern  statetype s_schabbdie4;
extern  statetype s_schabbdie5;
extern  statetype s_schabbdie6;

extern  statetype s_schabbshoot1;
extern  statetype s_schabbshoot2;

extern  statetype s_needle1;
extern  statetype s_needle2;
extern  statetype s_needle3;
extern  statetype s_needle4;

extern  statetype s_schabbdeathcam;

statetype s_schabbstand         = {false,SPR_SCHABB_W1,0,(statefunc)T_Stand,NULL,&s_schabbstand};

statetype s_schabbchase1        = {false,SPR_SCHABB_W1,10,(statefunc)T_Schabb,NULL,&s_schabbchase1s};
statetype s_schabbchase1s       = {false,SPR_SCHABB_W1,3,NULL,NULL,&s_schabbchase2};
statetype s_schabbchase2        = {false,SPR_SCHABB_W2,8,(statefunc)T_Schabb,NULL,&s_schabbchase3};
statetype s_schabbchase3        = {false,SPR_SCHABB_W3,10,(statefunc)T_Schabb,NULL,&s_schabbchase3s};
statetype s_schabbchase3s       = {false,SPR_SCHABB_W3,3,NULL,NULL,&s_schabbchase4};
statetype s_schabbchase4        = {false,SPR_SCHABB_W4,8,(statefunc)T_Schabb,NULL,&s_schabbchase1};

statetype s_schabbdeathcam      = {false,SPR_SCHABB_W1,1,NULL,NULL,&s_schabbdie1};

statetype s_schabbdie1          = {false,SPR_SCHABB_W1,10,NULL,(statefunc)A_DeathScream,&s_schabbdie2};
statetype s_schabbdie2          = {false,SPR_SCHABB_W1,10,NULL,NULL,&s_schabbdie3};
statetype s_schabbdie3          = {false,SPR_SCHABB_DIE1,10,NULL,NULL,&s_schabbdie4};
statetype s_schabbdie4          = {false,SPR_SCHABB_DIE2,10,NULL,NULL,&s_schabbdie5};
statetype s_schabbdie5          = {false,SPR_SCHABB_DIE3,10,NULL,NULL,&s_schabbdie6};
statetype s_schabbdie6          = {false,SPR_SCHABB_DEAD,20,NULL,(statefunc)A_StartDeathCam,&s_schabbdie6};

statetype s_schabbshoot1        = {false,SPR_SCHABB_SHOOT1,30,NULL,NULL,&s_schabbshoot2};
statetype s_schabbshoot2        = {false,SPR_SCHABB_SHOOT2,10,NULL,(statefunc)T_SchabbThrow,&s_schabbchase1};

statetype s_needle1             = {false,SPR_HYPO1,6,(statefunc)T_Projectile,NULL,&s_needle2};
statetype s_needle2             = {false,SPR_HYPO2,6,(statefunc)T_Projectile,NULL,&s_needle3};
statetype s_needle3             = {false,SPR_HYPO3,6,(statefunc)T_Projectile,NULL,&s_needle4};
statetype s_needle4             = {false,SPR_HYPO4,6,(statefunc)T_Projectile,NULL,&s_needle1};

extern  statetype s_needle1;
extern  statetype s_needle2;
extern  statetype s_needle3;
extern  statetype s_needle4;

extern  statetype s_fatdeathcam;

/*
===============
=
= SpawnSchabbs
=
===============
*/

void SpawnSchabbs (int tilex, int tiley)
{
    s_schabbdie2.tictime = 140;

    SpawnNewObj (tilex,tiley,&s_schabbstand);
    newobj->speed = SPDPATROL;

    newobj->obclass = schabbobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_schabbs];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
=================
=
= T_SchabbThrow
=
=================
*/

void T_SchabbThrow (objtype *ob)
{
    int32_t deltax,deltay;
    float   angle;
    int     iangle;

    deltax = player->x - ob->x;
    deltay = ob->y - player->y;
    angle = (float) atan2((float) deltay, (float) deltax);
    if (angle<0)
        angle = (float) (M_PI*2+angle);
    iangle = (int) (angle/(M_PI*2)*ANGLES);

    GetNewActor ();
    newobj->state = &s_needle1;
    newobj->ticcount = 1;

    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->obclass = needleobj;
    newobj->dir = nodir;
    newobj->angle = iangle;
    newobj->speed = 0x2000l;

    newobj->flags = FL_NEVERMARK;
    newobj->active = ac_yes;

    PlaySoundLocActor (MISSILE_SHOOT_SND,newobj);
}


/*
=================
=
= T_Schabb
=
=================
*/

void T_Schabb (objtype *ob)
{
    int32_t move;
    int     dx,dy,dist;
    boolean dodge;

    dodge = false;
    dx = abs(ob->tilex - player->tilex);
    dy = abs(ob->tiley - player->tiley);
    dist = dx>dy ? dx : dy;

    if (CheckLine(ob))                                              // got a shot at player?
    {
        ob->hidden = false;
        if ( (unsigned) US_RndT() < (tics<<3) && objfreelist)
        {
            //
            // go into attack frame
            //
            NewState (ob,&s_schabbshoot1);
            return;
        }
        dodge = true;
    }
    else
        ob->hidden = true;

    if (ob->dir == nodir)
    {
        if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        if (ob->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (-ob->distance-1);
            if (doorobjlist[-ob->distance-1].action != dr_open)
                return;
            ob->distance = TILEGLOBAL;      // go ahead, the door is now open
            TryWalk(ob);
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        if (dist <4)
            SelectRunDir (ob);
        else if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}


/*
=============================================================================

                                    HITLERS

=============================================================================
*/


//
// fake
//

extern  statetype s_fire1;
extern  statetype s_fire2;

statetype s_fire1               = {false,SPR_FIRE1,6,NULL,(statefunc)T_Projectile,&s_fire2};
statetype s_fire2               = {false,SPR_FIRE2,6,NULL,(statefunc)T_Projectile,&s_fire1};

//
// hitler
//
extern  statetype s_mechachase1;
extern  statetype s_mechachase1s;
extern  statetype s_mechachase2;
extern  statetype s_mechachase3;
extern  statetype s_mechachase3s;
extern  statetype s_mechachase4;

extern  statetype s_mechadie1;
extern  statetype s_mechadie1a;
extern  statetype s_mechadie2;
extern  statetype s_mechadie3;
extern  statetype s_mechadie4;

extern  statetype s_mechashoot1;
extern  statetype s_mechashoot2;
extern  statetype s_mechashoot3;
extern  statetype s_mechashoot4;
extern  statetype s_mechashoot5;
extern  statetype s_mechashoot6;

extern  statetype s_g4dashcam;


extern  statetype s_hitlerdeathcam;

statetype s_mechastand          = {false,SPR_G4RDPOSS_W1,0,(statefunc)T_Stand,NULL,&s_mechastand};

statetype s_mechachase1         = {false,SPR_G4RDPOSS_W1,10,(statefunc)T_Chase,NULL,&s_mechachase1s};
statetype s_mechachase1s        = {false,SPR_G4RDPOSS_W1,6,NULL,NULL,&s_mechachase2};
statetype s_mechachase2         = {false,SPR_G4RDPOSS_W2,8,(statefunc)T_Chase,NULL,&s_mechachase3};
statetype s_mechachase3         = {false,SPR_G4RDPOSS_W3,10,(statefunc)T_Chase,NULL,&s_mechachase3s};
statetype s_mechachase3s        = {false,SPR_G4RDPOSS_W3,6,NULL,NULL,&s_mechachase4};
statetype s_mechachase4         = {false,SPR_G4RDPOSS_W4,8,(statefunc)T_Chase,NULL,&s_mechachase1};

statetype s_mechadie1           = {false,SPR_G4RDPOSS_W1,35,NULL,(statefunc)A_DeathScream,&s_mechadie1a};
statetype s_mechadie1a          = {false,SPR_G4RDASH_DIE1,10,NULL,NULL,&s_mechadie2};
statetype s_mechadie2           = {false,SPR_G4RDASH_DIE2,10,NULL,NULL,&s_mechadie3};
statetype s_mechadie3           = {false,SPR_G4RDASH_DIE3,10,NULL,NULL,&s_mechadie4};
statetype s_mechadie4           = {false,SPR_G4RDASH_DEAD,10,NULL,(statefunc)A_StartDeathCam,&s_mechadie4};

statetype s_mechashoot1         = {false,SPR_G4RDPOSS_SHOOT1,30,NULL,NULL,&s_mechashoot2};
statetype s_mechashoot2         = {false,SPR_G4RDPOSS_SHOOT2,10,(statefunc)T_FakeFire,(statefunc)T_Shoot,&s_mechashoot3};
statetype s_mechashoot3         = {false,SPR_G4RDPOSS_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_mechashoot4};
statetype s_mechashoot4         = {false,SPR_G4RDPOSS_SHOOT2,10,(statefunc)T_FakeFire,(statefunc)T_Shoot,&s_mechashoot5};
statetype s_mechashoot5         = {false,SPR_G4RDPOSS_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_mechashoot6};
statetype s_mechashoot6         = {false,SPR_G4RDPOSS_SHOOT2,10,(statefunc)T_FakeFire,(statefunc)T_Shoot,&s_mechachase1};

statetype s_g4dashcam           = {false,SPR_SCHABB_W1,1,NULL,NULL,&s_mechadie1};

extern statetype s_glass_stand_n;
extern statetype s_glass_stand_a;
extern statetype s_glass_stand_b;
extern statetype s_glass_die1;
extern statetype s_glass_die2;
extern statetype s_glass_die3;
extern statetype s_glass_dead;

statetype s_glass_stand_n           = {false,SPR_GLASS_A,5,NULL,NULL, &s_glass_stand_n};

statetype s_glass_stand_a           = {false,SPR_GLASS_CRACK_A,5,NULL,NULL, &s_glass_stand_a};
statetype s_glass_stand_b           = {false,SPR_GLASS_CRACK_B,5,NULL,NULL, &s_glass_stand_b};
statetype s_glass_die1              = {false,SPR_GLASS_BREAK_A,5,NULL,(statefunc)A_DeathScream, &s_glass_die2};
statetype s_glass_die2              = {false,SPR_GLASS_BREAK_B,5,NULL,NULL, &s_glass_die3};
statetype s_glass_die3              = {false,SPR_GLASS_BREAK_C,5,NULL,NULL, &s_glass_dead};
statetype s_glass_dead              = {false,SPR_GLASS_BROKEN,10,NULL,NULL, &s_glass_dead};

void SpawnRegularGlass (boolean vert, int tilex, int tiley)
{
    SpawnNewObj(tilex, tiley, &s_glass_stand_n);
    
    if (vert)
        newobj->speed = -1;
    else
        newobj->speed = -2;
    
    newobj->obclass = nglassobj;
    newobj->hitpoints = 1024;
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH|FL_VISABLE;
    
}

void SpawnBreakableGlass (boolean vert, boolean bloody, int tilex, int tiley)
{
    if (bloody)
        SpawnNewObj (tilex,tiley,&s_glass_stand_b);
    else
        SpawnNewObj (tilex,tiley,&s_glass_stand_a);
        
    if (vert)
        newobj->speed = -1;
    else
        newobj->speed = -2;
        
    newobj->obclass = glassobj;
    newobj->hitpoints = 20;
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
}

void T_Burn(objtype *ob)
{
    if (player->tilex == ob->tilex &&
        player->tiley == ob->tiley)
    {
        TakeDamage(US_RndT()>>4,ob);
        SD_PlaySound(ELECTRIC_ARC_SND);
    }
}

extern  statetype s_wires_live_a;
extern  statetype s_wires_live_b;
extern  statetype s_wires_live_c;
extern  statetype s_wires_live_d;
extern  statetype s_wires_dead;

statetype s_wires_live_a        = {false,SPR_STAT_47a,5,NULL,(statefunc)T_Burn, &s_wires_live_b};
statetype s_wires_live_b        = {false,SPR_STAT_47b,5,NULL,NULL, &s_wires_live_c};
statetype s_wires_live_c        = {false,SPR_STAT_47c,5,NULL,NULL, &s_wires_live_d};
statetype s_wires_live_d        = {false,SPR_STAT_47d,5,NULL,NULL, &s_wires_live_a};

statetype s_wires_dead          = {false,SPR_STAT_47e,10,NULL,NULL, &s_wires_dead};

void SpawnLiveWires (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_wires_live_a);
    
    newobj->speed = MAPSPOT(tilex, tiley, 3);
    
    newobj->obclass = lwireobj;
    newobj->hitpoints = 100;
    newobj->dir = nodir;
    newobj->flags |= FL_AMBUSH | FL_FULLBRIGHT;
}

/*
===============
=
= SpawnHitler
=
===============
*/

void SpawnHitler (int tilex, int tiley)
{
    // s_hitlerdie2.tictime = 140;


    SpawnNewObj (tilex,tiley,&s_mechastand);
    newobj->speed = SPDPATROL;

    newobj->obclass = g4dashobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_hitler];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
===============
=
= A_HitlerMorph
=
===============
*/

void A_HitlerMorph (objtype *ob)
{
    short hitpoints[4]={500,700,800,900};

    // SpawnNewObj (ob->tilex,ob->tiley,&s_hitlerchase1);
    newobj->speed = SPDPATROL;

    newobj->x = ob->x;
    newobj->y = ob->y;

    newobj->distance = ob->distance;
    newobj->dir = ob->dir;
    newobj->flags = ob->flags | FL_SHOOTABLE;
    newobj->flags &= ~FL_NONMARK;   // hitler stuck with nodir fix

    // newobj->obclass = g3dashobj;
    newobj->hitpoints = hitpoints[gamestate.difficulty];
}

////////////////////////////////////////////////////////
//
// A_MechaSound
// A_Slurpie
//
////////////////////////////////////////////////////////
void A_MechaSound (objtype *ob)
{
    // if (areabyplayer[ob->areanumber])
        // PlaySoundLocActor (MECHSTEPSND,ob);
}

void A_Slurpie (objtype *)
{
    // SD_PlaySound(SLURPIESND);
}

/*
=================
=
= T_FakeFire
=
=================
*/

void T_FakeFire (objtype *ob)
{
    int32_t deltax,deltay;
    float   angle;
    int     iangle;

    if (!objfreelist)       // stop shooting if over MAXACTORS
    {
        // NewState (ob,&s_fakechase1);
        return;
    }

    deltax = player->x - ob->x;
    deltay = ob->y - player->y;
    angle = (float) atan2((float) deltay, (float) deltax);
    if (angle<0)
        angle = (float)(M_PI*2+angle);
    iangle = (int) (angle/(M_PI*2)*ANGLES);

    GetNewActor ();
    newobj->state = &s_fire1;
    newobj->ticcount = 2;

    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->dir = nodir;
    newobj->angle = iangle;
    newobj->obclass = fireobj;
    newobj->speed = 0x3000l;
    newobj->flags = FL_NEVERMARK;
    newobj->active = ac_yes;

    PlaySoundLocActor (FLAMETHROWERSND,newobj);
}



/*
=================
=
= T_Fake
=
=================
*/

void T_Fake (objtype *ob)
{
    int32_t move;

    if (CheckLine(ob))                      // got a shot at player?
    {
        ob->hidden = false;
        if ( (unsigned) US_RndT() < (tics<<1) && objfreelist)
        {
            //
            // go into attack frame
            //
            // NewState (ob,&s_fakeshoot1);
            return;
        }
    }
    else
        ob->hidden = true;

    if (ob->dir == nodir)
    {
        SelectDodgeDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        SelectDodgeDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}

#endif
/*
============================================================================

STAND

============================================================================
*/


/*
===============
=
= T_Stand
=
===============
*/

void T_Stand (objtype *ob)
{
    SightPlayer (ob);
}


/*
============================================================================

CHASE

============================================================================
*/

/*
=================
=
= T_Chase
=
=================
*/

void T_Chase (objtype *ob)
{
    int32_t move,target;
    int     dx,dy,dist,chance;
    boolean dodge;

    if (gamestate.victoryflag)
        return;

    dodge = false;
    if (CheckLine(ob))      // got a shot at player?
    {
        ob->hidden = false;
        dx = abs(ob->tilex - player->tilex);
        dy = abs(ob->tiley - player->tiley);
        dist = dx>dy ? dx : dy;

#ifdef PLAYDEMOLIKEORIGINAL
        if(DEMOCOND_ORIG)
        {
            if(!dist || (dist == 1 && ob->distance < 0x4000))
                chance = 300;
            else
                chance = (tics<<4)/dist;
        }
        else
#endif
        {
            if (dist)
                chance = (tics<<4)/dist;
            else
                chance = 300;

            if (dist == 1)
            {
                target = abs(ob->x - player->x);
                if (target < 0x14000l)
                {
                    target = abs(ob->y - player->y);
                    if (target < 0x14000l)
                        chance = 300;
                }
            }
        }

        if ( US_RndT()<chance)
        {
            //
            // go into attack frame
            //
            switch (ob->obclass)
            {
                case guardobj:
                    NewState (ob,&s_grdshoot1);
                    break;
                case officerobj:
                    NewState (ob,&s_ofcshoot1);
                    break;
                case mutantobj:
                    NewState (ob,&s_mutshoot1);
                    break;
                case ssobj:
                    NewState (ob,&s_ssshoot1);
                    break;
                case bossobj:
                    NewState (ob,&s_bossshoot1);
                    break;
                case g4dashobj:
                    NewState (ob,&s_mechashoot1);
                    break;
                default:
                    break;
            }
            return;
        }
        dodge = true;
    }
    else
        ob->hidden = true;

    if (ob->dir == nodir)
    {
        if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        if (ob->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (-ob->distance-1);
            if (doorobjlist[-ob->distance-1].action != dr_open)
                return;
            ob->distance = TILEGLOBAL;      // go ahead, the door is now open
            DEMOIF_SDL
            {
                TryWalk(ob);
            }
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}


/*
=================
=
= T_Ghosts
=
=================
*/

void T_Ghosts (objtype *ob)
{
    int32_t move;

    if (ob->dir == nodir)
    {
        SelectChaseDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        SelectChaseDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}

/*
=================
=
= T_DogChase
=
=================
*/

void T_DogChase (objtype *ob)
{
    int32_t    move;
    int32_t    dx,dy;


    if (ob->dir == nodir)
    {
        SelectDodgeDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        //
        // check for byte range
        //
        dx = player->x - ob->x;
        if (dx<0)
            dx = -dx;
        dx -= move;
        if (dx <= MINACTORDIST)
        {
            dy = player->y - ob->y;
            if (dy<0)
                dy = -dy;
            dy -= move;
            if (dy <= MINACTORDIST)
            {
                // NewState (ob,&s_dogjump1);
                return;
            }
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        SelectDodgeDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}



/*
============================================================================

                                    PATH

============================================================================
*/


/*
===============
=
= SelectPathDir
=
===============
*/

void SelectPathDir (objtype *ob)
{
    unsigned spot;

    spot = MAPSPOT(ob->tilex,ob->tiley,1)-ICONARROWS;

    if (spot<8)
    {
        // new direction
        ob->dir = (dirtype)(spot);
    }

    ob->distance = TILEGLOBAL;

    if (!TryWalk (ob))
        ob->dir = nodir;
}


/*
===============
=
= T_Path
=
===============
*/

void T_Path (objtype *ob)
{
    int32_t    move;

    if (SightPlayer (ob))
        return;

    if (ob->dir == nodir)
    {
        SelectPathDir (ob);
        if (ob->dir == nodir)
            return;                                 // all movement is blocked
    }


    move = ob->speed*tics;

    while (move)
    {
        if (ob->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (-ob->distance-1);
            if (doorobjlist[-ob->distance-1].action != dr_open)
                return;
            ob->distance = TILEGLOBAL;      // go ahead, the door is now open
            DEMOIF_SDL
            {
                TryWalk(ob);
            }
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        if (ob->tilex>MAPSIZE || ob->tiley>MAPSIZE)
        {
            sprintf (str, "T_Path hit a wall at %u,%u, dir %u",
                ob->tilex,ob->tiley,ob->dir);
            Quit (str);
        }

        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
        move -= ob->distance;

        SelectPathDir (ob);

        if (ob->dir == nodir)
            return;                                 // all movement is blocked
    }
}


/*
=============================================================================

                                    FIGHT

=============================================================================
*/


/*
===============
=
= T_Shoot
=
= Try to damage the player, based on skill level and player's speed
=
===============
*/

void T_Shoot (objtype *ob)
{
    int     dx,dy,dist;
    int     hitchance,damage;

    hitchance = 128;

    if (!areabyplayer[ob->areanumber])
        return;

    if (CheckLine (ob))                    // player is not behind a wall
    {
        dx = abs(ob->tilex - player->tilex);
        dy = abs(ob->tiley - player->tiley);
        dist = dx>dy ? dx:dy;

        if (ob->obclass == ssobj || ob->obclass == bossobj)
            dist = dist*2/3;                                        // ss are better shots

        if (thrustspeed >= RUNSPEED)
        {
            if (ob->flags&FL_VISABLE)
                hitchance = 160-dist*16;                // player can see to dodge
            else
                hitchance = 160-dist*8;
        }
        else
        {
            if (ob->flags&FL_VISABLE)
                hitchance = 256-dist*16;                // player can see to dodge
            else
                hitchance = 256-dist*8;
        }

        // see if the shot was a hit

        if (US_RndT()<hitchance)
        {
            if (dist<2)
                damage = US_RndT()>>2;
            else if (dist<4)
                damage = US_RndT()>>3;
            else
                damage = US_RndT()>>4;

            TakeDamage (damage,ob);
        }
    }

    switch(ob->obclass)
    {
        case ssobj:
            PlaySoundLocActor(PEGPOL_ATK_SND,ob);
            break;
#ifndef SPEAR
        case g4dashobj:
        case bossobj:
            PlaySoundLocActor(BOSS_CHAINGUN_SND,ob);
            break;
        case schabbobj:
            PlaySoundLocActor(MISSILE_SHOOT_SND,ob);
            break;
#endif
        default:
            PlaySoundLocActor(WEAPON_PISTOL_SND,ob);
    }
}


/*
===============
=
= T_Bite
=
===============
*/

void T_Bite (objtype *ob)
{
    int32_t    dx,dy;

    // PlaySoundLocActor(DOGATTACKSND,ob);     // JAB

    dx = player->x - ob->x;
    if (dx<0)
        dx = -dx;
    dx -= TILEGLOBAL;
    if (dx <= MINACTORDIST)
    {
        dy = player->y - ob->y;
        if (dy<0)
            dy = -dy;
        dy -= TILEGLOBAL;
        if (dy <= MINACTORDIST)
        {
            if (US_RndT()<180)
            {
                TakeDamage (US_RndT()>>4,ob);
                return;
            }
        }
    }
}


#ifndef SPEAR
/*
============================================================================

                                    BJ VICTORY

============================================================================
*/


//
// BJ victory
//

void T_BJRun (objtype *ob);
void T_BJJump (objtype *ob);
void T_BJDone (objtype *ob);
void T_BJYell (objtype *ob);

extern  statetype s_bjrun1;
extern  statetype s_bjrun1s;
extern  statetype s_bjrun2;
extern  statetype s_bjrun3;
extern  statetype s_bjrun3s;
extern  statetype s_bjrun4;

extern  statetype s_bjjump1;
extern  statetype s_bjjump2;
extern  statetype s_bjjump3;
extern  statetype s_bjjump4;


statetype s_bjrun1              = {false,SPR_BJ_W1,12,(statefunc)T_BJRun,NULL,&s_bjrun1s};
statetype s_bjrun1s             = {false,SPR_BJ_W1,3, NULL,NULL,&s_bjrun2};
statetype s_bjrun2              = {false,SPR_BJ_W2,8,(statefunc)T_BJRun,NULL,&s_bjrun3};
statetype s_bjrun3              = {false,SPR_BJ_W3,12,(statefunc)T_BJRun,NULL,&s_bjrun3s};
statetype s_bjrun3s             = {false,SPR_BJ_W3,3, NULL,NULL,&s_bjrun4};
statetype s_bjrun4              = {false,SPR_BJ_W4,8,(statefunc)T_BJRun,NULL,&s_bjrun1};


statetype s_bjjump1             = {false,SPR_BJ_JUMP1,14,(statefunc)T_BJJump,NULL,&s_bjjump2};
statetype s_bjjump2             = {false,SPR_BJ_JUMP2,14,(statefunc)T_BJJump,(statefunc)T_BJYell,&s_bjjump3};
statetype s_bjjump3             = {false,SPR_BJ_JUMP3,14,(statefunc)T_BJJump,NULL,&s_bjjump4};
statetype s_bjjump4             = {false,SPR_BJ_JUMP4,300,NULL,(statefunc)T_BJDone,&s_bjjump4};


statetype s_deathcam            = {false,0,0,NULL,NULL,NULL};


/*
===============
=
= SpawnBJVictory
=
===============
*/

void SpawnBJVictory (void)
{
    SpawnNewObj (player->tilex,player->tiley+1,&s_bjrun1);
    newobj->x = player->x;
    newobj->y = player->y;
    newobj->obclass = bjobj;
    newobj->dir = south;
    newobj->temp1 = 6;                      // tiles to run forward
}



/*
===============
=
= T_BJRun
=
===============
*/

void T_BJRun (objtype *ob)
{
    int32_t    move;

    move = BJRUNSPEED*tics;

    while (move)
    {
        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }


        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
        move -= ob->distance;

        SelectPathDir (ob);

        if ( !(--ob->temp1) )
        {
            NewState (ob,&s_bjjump1);
            return;
        }
    }
}


/*
===============
=
= T_BJJump
=
===============
*/

void T_BJJump (objtype *ob)
{
    int32_t    move;

    move = BJJUMPSPEED*tics;
    MoveObj (ob,move);
}


/*
===============
=
= T_BJYell
=
===============
*/

void T_BJYell (objtype *ob)
{
    // PlaySoundLocActor(YEAHSND,ob);  // JAB
}


/*
===============
=
= T_BJDone
=
===============
*/

void T_BJDone (objtype *)
{
    playstate = ex_victorious;                              // exit castle tile
}



//===========================================================================


/*
===============
=
= CheckPosition
=
===============
*/

boolean CheckPosition (objtype *ob)
{
    int     x,y,xl,yl,xh,yh;
    objtype *check;

    xl = (ob->x-PLAYERSIZE) >> TILESHIFT;
    yl = (ob->y-PLAYERSIZE) >> TILESHIFT;

    xh = (ob->x+PLAYERSIZE) >> TILESHIFT;
    yh = (ob->y+PLAYERSIZE) >> TILESHIFT;

    //
    // check for solid walls
    //
    for (y=yl;y<=yh;y++)
    {
        for (x=xl;x<=xh;x++)
        {
            check = actorat[x][y];
            if (check && !ISPOINTER(check))
                return false;
        }
    }

    return true;
}


/*
===============
=
= A_StartDeathCam
=
===============
*/

void    A_StartDeathCam (objtype *ob)
{
    int32_t dx,dy;
    float   fangle;
    int32_t xmove,ymove;
    int32_t dist;

    FinishPaletteShifts ();

    VW_WaitVBL (100);

    if (gamestate.victoryflag)
    {
        playstate = ex_victorious;
        return;
    }

    if(usedoublebuffering) VH_UpdateScreen();
    
    gamestate.victoryflag = true;
        
    if (gamestate.mapon == 4 && gamestate.episode == 2)
        gamestate.endingmode = 1;
    else
        gamestate.endingmode = -1;
    
    unsigned fadeheight = viewsize != 21 ? screenHeight-scaleFactor*STATUSLINES : screenHeight;
    VL_BarScaledCoord (0, 0, screenWidth, fadeheight, bordercol);
    FizzleFade(screenBuffer, 0, 0, screenWidth, fadeheight, 70, false);

    if (bordercol != VIEWCOLOR)
    {
        CA_CacheGrChunk (STARTFONT+1);
        fontnumber = 1;
        SETFONTCOLOR(15,bordercol);
        PrintX = 68; PrintY = 45;
        US_Print (STR_SEEAGAIN);
        UNCACHEGRCHUNK(STARTFONT+1);
    }
    else
    {
        CacheLump(LEVELEND_LUMP_START,LEVELEND_LUMP_END);
        Write(0,7,STR_SEEAGAIN);
    }

    VW_UpdateScreen ();
    if(usedoublebuffering) VH_UpdateScreen();

    IN_UserInput(300);

    //
    // line angle up exactly
    //
    NewState (player,&s_deathcam);

    player->x = gamestate.killx;
    player->y = gamestate.killy;

    dx = ob->x - player->x;
    dy = player->y - ob->y;

    fangle = (float) atan2((float) dy, (float) dx);          // returns -pi to pi
    if (fangle<0)
        fangle = (float) (M_PI*2+fangle);

    player->angle = (short) (fangle/(M_PI*2)*ANGLES);

    //
    // try to position as close as possible without being in a wall
    //
    dist = 0x14000l;
    do
    {
        xmove = FixedMul(dist,costable[player->angle]);
        ymove = -FixedMul(dist,sintable[player->angle]);

        player->x = ob->x - xmove;
        player->y = ob->y - ymove;
        dist += 0x1000;

    } while (!CheckPosition (player));
    plux = (word)(player->x >> UNSIGNEDSHIFT);                      // scale to fit in unsigned
    pluy = (word)(player->y >> UNSIGNEDSHIFT);
    player->tilex = (word)(player->x >> TILESHIFT);         // scale to tile values
    player->tiley = (word)(player->y >> TILESHIFT);

    //
    // go back to the game
    //

    DrawPlayBorder ();

    fizzlein = true;

    switch (ob->obclass)
    {
        case schabbobj:
            NewState (ob,&s_schabbdeathcam);
            break;
        case g4dashobj:
            NewState (ob,&s_g4dashcam);
            break;
        default:
            break;
    }
}

#endif
