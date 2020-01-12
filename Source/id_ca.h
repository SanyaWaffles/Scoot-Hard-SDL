#ifndef __ID_CA__
#define __ID_CA__

//===========================================================================

#define NUMMAPS             18


#ifdef USE_FLOORCEILINGTEX
#ifdef USE_FLOORTAGS
    #define MAPPLANES       4
#else
    #define MAPPLANES       3
#endif
#else
    #define MAPPLANES       2
#endif

#define UNCACHEGRCHUNK(chunk) {if(grsegs[chunk]) {free(grsegs[chunk]); grsegs[chunk]=NULL;}}

typedef struct
{
    int32_t planestart[MAPPLANES];
    int32_t planelength[MAPPLANES];
    word    width,height;
    char    name[32];
} maptype;

//===========================================================================

extern  int   mapon;

extern  word *mapsegs[MAPPLANES];
extern  byte *grsegs[NUMCHUNKS];

extern  char  extension[5];
extern  char  graphext[5];

//===========================================================================

boolean CA_LoadFile (const char *filename, memptr *ptr);
boolean CA_WriteFile (const char *filename, void *ptr, int32_t length);

int32_t CA_RLEWCompress (word *source, int32_t length, word *dest, word rlewtag);

void CA_RLEWexpand (word *source, word *dest, int32_t length, word rlewtag);

void CA_Startup (void);
void CA_Shutdown (void);

int32_t CA_CacheAudioChunk (int chunk);
void CA_LoadAllSounds (void);

void CA_CacheGrChunk (int chunk);
void CA_CacheMap (int mapnum);

void CA_CacheScreen (int chunk);

void CA_CannotOpen(const char *name);

#endif
