//////////////////////////////////////
//
// Graphics .H file for .WL6
// IGRAB-ed on Wed Apr 13 06:58:44 1994
//
//////////////////////////////////////

typedef enum {
    // Lump Start
    H_CASTLEPIC=3,                       // 3
    H_TOPWINDOWPIC,                      // 4
    H_LEFTWINDOWPIC,                     // 5
    H_RIGHTWINDOWPIC,                    // 6
    H_BOTTOMINFOPIC,                     // 7
    // Lump Start
    C_OPTIONSPIC,                        // 8
    C_CURSOR1PIC,                        // 9
    C_CURSOR2PIC,                        // 10
    C_NOTSELECTEDPIC,                    // 11
    C_SELECTEDPIC,                       // 12
    C_FXTITLEPIC,                        // 13
    C_DIGITITLEPIC,                      // 14
    C_MUSICTITLEPIC,                     // 15
    C_MOUSELBACKPIC,                     // 16
    C_BABYMODEPIC,                       // 17
    C_EASYPIC,                           // 18
    C_NORMALPIC,                         // 19
    C_HARDPIC,                           // 20
    C_LOADSAVEDISKPIC,                   // 21
    C_DISKLOADING1PIC,                   // 22
    C_DISKLOADING2PIC,                   // 23
    C_CONTROLPIC,                        // 24
    C_CUSTOMIZEPIC,                      // 25
    C_LOADGAMEPIC,                       // 26
    C_SAVEGAMEPIC,                       // 27
    C_EPISODE1PIC,                       // 28
    C_EPISODE2PIC,                       // 29
    C_EPISODE3PIC,                       // 30
    C_LEVELPIC,                          // 31
    C_NAMEPIC,                           // 32
    C_SCOREPIC,                          // 33
    C_JOY1PIC,                           // 34
    C_JOY2PIC,                           // 35
    // BEGIN README HELP
    RM_HELP1UP,                          // 36
    RM_HELPBITS,                         // 37
    RM_HELPWORKER,                       // 38
    RM_HELPPEGPOL,                       // 39
    RM_HELPENGIE,                        // 40
    RM_HELPDERELLE,                      // 41
    RM_HELPAMMO,                         // 42
    RM_HELPSUBMAC,                       // 43
    RM_HELPCHAINGUN,                     // 44
    RM_HELPKEYS,                         // 45
    RM_HELPSWITCH,                       // 46
    RM_HELPVENDING,                      // 47
    // BEGIN
    // Lump Start
    L_COLONPIC,                          // 48
    L_NUM0PIC,                           // 49
    L_NUM1PIC,                           // 50
    L_NUM2PIC,                           // 51
    L_NUM3PIC,                           // 52
    L_NUM4PIC,                           // 53
    L_NUM5PIC,                           // 54
    L_NUM6PIC,                           // 55
    L_NUM7PIC,                           // 56
    L_NUM8PIC,                           // 57
    L_NUM9PIC,                           // 58
    L_PERCENTPIC,                        // 59
    L_APIC,                              // 60
    L_BPIC,                              // 61
    L_CPIC,                              // 62
    L_DPIC,                              // 63
    L_EPIC,                              // 64
    L_FPIC,                              // 65
    L_GPIC,                              // 66
    L_HPIC,                              // 67
    L_IPIC,                              // 68
    L_JPIC,                              // 69
    L_KPIC,                              // 70
    L_LPIC,                              // 71
    L_MPIC,                              // 72
    L_NPIC,                              // 73
    L_OPIC,                              // 74
    L_PPIC,                              // 75
    L_QPIC,                              // 76
    L_RPIC,                              // 77
    L_SPIC,                              // 78
    L_TPIC,                              // 79
    L_UPIC,                              // 80
    L_VPIC,                              // 81
    L_WPIC,                              // 82
    L_XPIC,                              // 83
    L_YPIC,                              // 84
    L_ZPIC,                              // 85
    L_EXPOINTPIC,                        // 86
    L_APOSTROPHEPIC,                     // 87
    STATUSBARPIC,                        // 88
    TITLEPIC,                            // 89
    PG13PIC,                             // 90
    CREDITSPIC,                          // 91
    HIGHSCORESPIC,                       // 92
    // Lump Start
    NOKEYPIC,                            // 97
    GOLDKEYPIC,                          // 98
    SILVERKEYPIC,                        // 99
    N_BLANKPIC,                          // 100
    N_0PIC,                              // 101
    N_1PIC,                              // 102
    N_2PIC,                              // 103
    N_3PIC,                              // 104
    N_4PIC,                              // 105
    N_5PIC,                              // 106
    N_6PIC,                              // 107
    N_7PIC,                              // 108
    N_8PIC,                              // 109
    N_9PIC,                              // 110
    FACE1APIC,                           // 111
    FACE1BPIC,                           // 112
    FACE1CPIC,                           // 113
    FACE2APIC,                           // 114
    FACE2BPIC,                           // 115
    FACE2CPIC,                           // 116
    FACE3APIC,                           // 117
    FACE3BPIC,                           // 118
    FACE3CPIC,                           // 119
    FACE4APIC,                           // 120
    FACE4BPIC,                           // 121
    FACE4CPIC,                           // 122
    FACE5APIC,                           // 123
    FACE5BPIC,                           // 124
    FACE5CPIC,                           // 125
    FACE6APIC,                           // 126
    FACE6BPIC,                           // 127
    FACE6CPIC,                           // 128
    FACE7APIC,                           // 129
    FACE7BPIC,                           // 130
    FACE7CPIC,                           // 131
    FACE8APIC,                           // 132
    GOTGATLINGPIC,                       // 129
    MUTANTBJPIC,                         // 130
    PAUSEDPIC,                           // 131
    GETPSYCHEDPIC,                       // 132
    MESSAGE_BGPIC,
    RM_HELPBONUSCHEST,
    RM_HELPHEALTHSTUFF,
    RM_VOUCHERHELP,
    TILE8,                               // 133
    ORDERSCREEN,                         // 138
    ERRORSCREEN,                         // 139
    T_HELPART,                           // 140
    T_ENDART1,                           // 145
    T_ENDART2,                           // 146
    T_ENDART3A,                           // 147
    T_ENDART3B,
    T_ENDART3C,
    ENUMEND
} graphicnums;

//
// Data LUMPs
//
#define README_LUMP_START		(H_CASTLEPIC)
#define README_LUMP_END			(H_BOTTOMINFOPIC)

#define CONTROLS_LUMP_START		(C_OPTIONSPIC)
#define CONTROLS_LUMP_END		(C_JOY2PIC)

#define LEVELEND_LUMP_START		(L_COLONPIC)
#define LEVELEND_LUMP_END		(L_APOSTROPHEPIC)

#define LATCHPICS_LUMP_START	(NOKEYPIC)
#define LATCHPICS_LUMP_END		(MESSAGE_BGPIC)



//
// Amount of each data item
//
#define NUMCHUNKS    ENUMEND
#define NUMFONT      3
#define NUMFONTM     0
#define NUMPICS      132
#define NUMPICM      0
#define NUMSPRITES   0
#define NUMTILE8     72
#define NUMTILE8M    0
#define NUMTILE16    0
#define NUMTILE16M   0
#define NUMTILE32    0
#define NUMTILE32M   0
#define NUMEXTERNS   13
//
// File offsets for data items
//
#define STRUCTPIC    0

#define STARTFONT    1
#define STARTFONTM   3
#define STARTPICS    3
#define STARTPICM    TILE8
#define STARTSPRITES TILE8
#define STARTTILE8   TILE8
#define STARTTILE8M  TILE8+1
#define STARTTILE16  TILE8+1
#define STARTTILE16M TILE8+1
#define STARTTILE32  TILE8+1
#define STARTTILE32M TILE8+1
#define STARTEXTERNS TILE8+1

//
// Thank you for using IGRAB!
//
