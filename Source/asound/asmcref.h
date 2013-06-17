/*
VODKA-INDUCED ENTERTAINMENT ADVANCED SOUND MANAGER v0.9
COMPATIBILITY LAYER FOR WOLF4SDL v1.6 - SOUND NAMES REFERENCE FILE
BY GERARD 'ALUMIUN' WATSON
*/

#ifndef _ASMCREF_H_
#define _ASMCREF_H_

// This table holds the file names for the sounds defined in AUDIOWL6.H.
// Change these to suit your sounds and your AUDIOHED.WL6 or SOD file.
// Currently these are set up for Wolfenstein, not SoD.

const char *sounddir = "sounds/";

const char *ASM_Soundnames[NUMSOUNDS] = {
        "keypkup.wav",          // 12
        "door.wav",                // 18
        "guard/g_see1.wav",          // 21
        "guard/g_see2.wav",
        "guard/g_see3.wav",
        "guard/g_die1.wav",
        "guard/g_die2.wav",          // 22
        "guard/g_die3.wav",
        "weapons/taser.wav",         // 23
        "weapons/pistfire.wav",       // 24
        "weapons/smg_fire.wav",       // 26
        "weapons/cgunfire.wav",
        "genpkup.wav",       // 33
        "switch.wav",              // 40
        "achnoise.wav",          // 44
        "pushwall.wav",             // 46
        "surprise/b1_see.wav",           // 49
        "surprise/b1_die.wav",           // 50
        "pegpol/pp_see1.wav",         // 51
        "pegpol/pp_see2.wav",
        "pegpol/pp_die.wav",
        "derelle/der_die.wav",       // 52
        "engie/engie_see1.wav",
        "engie/engie_see2.wav",
        "engie/engie_die1.wav",
        "engie/engie_die2.wav",
        "rdash/b3_see.wav",          // 53
        "rdash/b3_swell.wav",          // 57
        "rdash/b3_bitch.wav",
        "bossgun.wav",       // 59
        "pegpol/pp_attk.wav",          // 60
        "atmos/b2_die.wav",           // 63
        "atmos/b2_see.wav",           // 64
        "weapons/flamfire.wav",         // 69
        "weapons/rockfire.wav",       // 85
        "weapons/explode.wav",     // 86
        "ammoup.wav",
        "thud.wav",
        "glassbrk.wav",
        "electric.wav",
};

// This table holds the file names for the music defined in AUDIOWL6.H.
// Change these to suit your music and your AUDIOHED.WL6 or SOD file.
// Currently these are set up for Wolfenstein, not SoD.
// Make sure you put the right format on the end here
// Compatible extentions:
//  -- WAV (If you're using WAVs for music, you're mad.)
//  -- OGG
//  -- MP3
//  -- MID
//  -- MOD, XM, IT, 669, S3M, MED

const char *musicdir = "music/";

/*

 typedef enum {
 MARIDIA2_MUS,               // 1
 NORFAIR2_MUS,               // 2
 MARCHING_MUS,               // 3
 SIMCITYC_MUS,               // 4
 RFACTORY_MUS,               // 5
 BADDER4_MUS,                // 6
 NORFAIR_MUS,                // 7
 K5HALPM_MUS,                // 8
 HHEROES_MUS,                // 9
 HSMOOTH_MUS,                // 10
 MBRAINF_MUS,                // 11
 POWRGRS_MUS,                // 12
 HCAROL_MUS,                 // 13
 LASTMUSIC
 } musicnames;
 
*/

const char *ASM_Musicnames[NUMMUSICS] = {
	"TRACK01.OGG",              // 0
	"TRACK02.OGG",             // 1
	"TRACK03.OGG",            // 2
	"TRACK04.OGG",             // 3
	"RFACTORY.OGG",            // 7
	"TRACK05.OGG",            // 8
	"TRACK06.OGG",                 // 9
	"TRACK07.OGG",              // 10
	"TRACK08.OGG",             // 11
	"TRACK09.OGG",            // 12
	"TRACK10.OGG",             // 13
	"TRACK11.OGG",            // 14
	"TRACK12.OGG",          // 15
    "TRACK13.OGG",
    "TRACK14.OGG",
    "TRACK15.OGG",
    "MARCHING.OGG"
};

#endif  // _ASMCREF_H_
