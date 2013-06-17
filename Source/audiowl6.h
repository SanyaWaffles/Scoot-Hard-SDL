/////////////////////////////////////////////////
//
// MUSE Header for .WL6
// Created Tue Jul 14 15:04:53 1992
//
/////////////////////////////////////////////////

//
// Sound names & indexes
//
typedef enum
{
	KEY_PICKUP_SND,          // 12
	DOOR_SND,                // 18
	GUARD_SEE1_SND,          // 21
    GUARD_SEE2_SND,
    GUARD_SEE3_SND,
    GUARD_DIE1_SND,
	GUARD_DIE2_SND,          // 22
    GUARD_DIE3_SND,
	WEAPON_TAZE_SND,         // 23
	WEAPON_PISTOL_SND,       // 24
	WEAPON_SUBMAC_SND,       // 26
    WEAPON_CHAINGUN_SND,
	GENERIC_ITEMUPSND,       // 33
	SWITCH_SND,              // 40
	ACHIEVEMENTSND,          // 44
	PUSHWALLSND,             // 46
	BOSS1_SEE_SND,           // 49
	BOSS1_DIE_SND,           // 50
	PEGPOL_SEE1_SND,         // 51
    PEGPOL_SEE2_SND,
    PEGPOL_DIE_SND,
	DERELLE_NOISE_SND,       // 52
    ENGIE_SEE1_SND,
    ENGIE_SEE2_SND,
    ENGIE_DIE1_SND,
    ENGIE_DIE2_SND,
	RDASH_SEE1_SND,          // 53
	RDASH_SEE2_SND,          // 57
    RDASH_DIE_SND,
	BOSS_CHAINGUN_SND,       // 59
	PEGPOL_ATK_SND,          // 60
	ATMOS_DIE_SND,           // 63
	ATMOS_SEE_SND,           // 64
	FLAMETHROWERSND,         // 69
	MISSILE_SHOOT_SND,       // 85
	MISSILE_EXPLODE_SND,     // 86
    AMMO_PICKUP_SND,
    THUD_NOPE_SND,
    GLASS_BREAK_SND,
    ELECTRIC_ARC_SND,
	LASTSOUND
} soundnames;

//
// Base offsets
//
#define STARTPCSOUNDS		0
#define STARTADLIBSOUNDS	0
#define STARTDIGISOUNDS		0
#define STARTMUSIC		    0

//
// Music names & indexes
//
typedef enum {
	TRACK_01,               // 1
	TRACK_02,               // 2
	TRACK_03,               // 3
	TRACK_04,               // 4
	RFACTORY_MUS,               // 5
	TRACK_05,                // 6
	TRACK_06,                // 7
	TRACK_07,                // 8
	TRACK_08,                // 9
	TRACK_09,                // 10
	TRACK_10,                // 11
	TRACK_11,                // 12
	TRACK_12,                 // 13
    TRACK_13,
    TRACK_14,
    TRACK_15,
    MARCH_MUS,
	LASTMUSIC
} musicnames;

#define NUMSOUNDS		    LASTSOUND
#define NUMSNDCHUNKS		(STARTMUSIC + LASTMUSIC)
#define NUMMUSICS           LASTMUSIC

/////////////////////////////////////////////////
//
// Thanks for playing with MUSE!
//
/////////////////////////////////////////////////
