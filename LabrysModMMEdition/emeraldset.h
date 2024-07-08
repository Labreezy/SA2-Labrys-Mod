#pragma once
#include <vector>
#include <string>
#include <fstream>

typedef unsigned char   undefined;

typedef unsigned int    ImageBaseOffset32;
typedef unsigned char    byte;
typedef unsigned int    dword;
typedef long long    longlong;
typedef unsigned long long    qword;
typedef unsigned char    uchar;
typedef unsigned int    uint;
typedef unsigned long    ulong;
typedef unsigned char    undefined1;
typedef unsigned short    undefined2;
typedef unsigned int    undefined4;
typedef unsigned long long    undefined6;
typedef unsigned long long    undefined8;
typedef unsigned short    ushort;
typedef unsigned short    word;
typedef struct SearchEmeraldsGameManager SearchEmeraldsGameManager, * PSearchEmeraldsGameManager;

typedef struct Emerald Emerald, * PEmerald;

typedef struct Fvec3 Fvec3, * PFvec3;

struct Fvec3 {
    float x;
    float y;
    float z;
};

struct Emerald {
    int id;
    struct Fvec3 position;
};

struct SearchEmeraldsGameManager {
    byte state;
    byte field1_0x1;
    byte field2_0x2;
    byte hints_read;
    byte has_gotten_very_close;
    byte emeralds_spawned;
    byte slot_1_array_len;
    byte slot_2_array_len;
    byte slot_3_array_len;
    byte slot_enemy_array_len;
    byte field10_0xa;
    byte field11_0xb;
    struct Emerald duplicate_emerald_1;
    struct Emerald duplicate_emerald_2;
    struct Emerald piece_1;
    struct Emerald pieec_2;
    struct Emerald piece_3;
    struct Emerald* slot_1_emeralds;
    struct Emerald* slot_2_emeralds;
    struct Emerald* slot_3_emeralds;
    struct Emerald* slot_enemy_emeralds;
    int timer;
    undefined field22_0x70;
    undefined field23_0x71;
    undefined field24_0x72;
    undefined field25_0x73;
};



DataPointer(SearchEmeraldsGameManager*, EmeraldManager, 0x1AF014C);
DataPointer(byte*, LevelComplete, 0x174B001);
DataPointer(byte*, GoToNextStage, 0x1A558A4);
void LoadSetsFromFile(std::string fpath);
void initHooks();
void initResultsSkipHook();
void StartTimeDB(std::string fpath);
static std::ofstream* ildb_outf = NULL;

