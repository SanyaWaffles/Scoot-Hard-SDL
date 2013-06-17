/*
VODKA-INDUCED ENTERTAINMENT ADVANCED SOUND MANAGER v0.9.1
COMPATIBILITY LAYER FOR WOLF4SDL v1.6 - HEADER FILE
BY GERARD 'ALUMIUN' WATSON
*/

#ifndef _ASMCOMP_H_
#define _ASMCOMP_H_

#include "vieasm.h"

#define NUMAMBIENTS     ASM_ABSMAXCHANNELS  // Number of ambient sound slots

#define TickBase        70      // 70Hz per tick - used as a base for timer 0

#define GetTimeCount()  ((SDL_GetTicks()*TickBase)/1000)

inline void Delay(int wolfticks)
{
    if (wolfticks>0) 
        SDL_Delay(wolfticks * 1000 / TickBase);
}

#define SD_SoundPlaying() 0
#define UNCACHEAUDIOCHUNK(chunk) ASM_Uncache(ASM_Audiosegs[chunk]);
#define SD_WaitSoundDone()
#define SD_PrepareSound(which) int nullps = which
#define SD_AbortIfMusOff if (!SD_MusIsOn()) return
#define SD_AbortIfSndOff if (!SD_SndIsOn()) return
#define SD_StopDigitized() SD_StopSound()
#define SD_ContinueMusic(chunk, startoffs) SD_StartMusic(chunk)
#define SD_ChangeVolume(snd, mus) ASM_ChangeVolume(snd, mus)
#define SD_GetVolume(snd, mus) ASM_ReturnVolume(snd, mus)
#define SD_Reverse(reverse) ASM_ReverseStereo(reverse)

extern sample ASM_Audiosegs[NUMSOUNDS];

// These enums are now treated differently. Because there are no seperate devices,
// any value other than sxm_Off will turn the device on, and SDSMode is no longer used.

typedef enum
{
    sdm_Off,sdm_AdLib
} SDMode;

typedef enum
{
    smm_Off,smm_AdLib
} SMMode;

typedef struct
{
    int valid;
    fixed globalsoundx, globalsoundy;
} globalsoundpos;

extern globalsoundpos channelSoundPos[];

extern  SDMode          SoundMode;
extern  SMMode          MusicMode;

// Functions

extern  void    SD_Startup(void),                                           // DONE
                SD_Shutdown(void);                                          // DONE

extern  void    SD_PositionSound(int leftvol, int rightvol);                // DONE, basically the same
extern  byte    SD_PlaySound(soundnames sound);                             // DONE, just calls SD_PlayDigitized
extern  void    SD_SetPosition(int channel, int leftpos, int rightpos);     // DONE, basically the same
extern  void    SD_StopSound(void);                                         // DONE, just calls ASM_HaltSound()

extern  void    SD_StartMusic(int chunk);                                   // DONE
extern  void    SD_ContinueMusic(int chunk, int startoffs);                 // DONE, but always starts at beginning (Sorry WSJ :p)
extern  void    SD_MusicOn(void),                                           // DONE, just resumes
                SD_FadeOutMusic(int fade);                                  // DONE
extern  int     SD_MusicOff(void);                                          // DONE, just pauses

extern  bool    SD_MusicPlaying(void);                                      // DONE
extern  bool    SD_SetSoundMode(SDMode mode);                               // DONE
extern  bool    SD_SetMusicMode(SMMode mode);                               // DONE

extern  int     SD_PlayDigitized(word which, int leftpos, int rightpos, bool amb);   // DONE
extern  void    CA_LoadAllSounds(void);

// Ambient sound prototypes

extern  int     SD_AmbientSound(void);
extern  void    SD_StopAmbient(int ambindex);

#endif  // _ASMCOMP_H_
