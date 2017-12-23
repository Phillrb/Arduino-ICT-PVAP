//
// Copyright (c) 2017, Phillip Riscombe-Burton
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
#include "CPaclandGame.h"

//ROM locations:
//Program ROMs:
//PL5_01B / PL6_01 / PL1-1 (set M) -> 8B
//PL5_02  / PL6_02 / PL1-2 (set M) -> 8D

//Banked ROMs:
//PL1_03  -> 8E
//PL1_04  -> 8F
//PL1_05  -> 8H
//PL1_06 or PL3_6 -> 8J

//NOT ACCESSIBLE FROM CPU
//PL4_10  -> 7E - sprites
//PL4_08  -> 6E - sprites
//PL4_11  -> 7F - sprites
//PL4_09  -> 6F - sprites
//PL1_07  -> 3E - sub program for the mcu
//PL2_12  -> 6N - chars
//PL4_13  -> 6T - chars?
//PL1-3   -> 6L - sprites lookup table
//PL1-5   -> 5T - foreground lookup table
//PL1-4   -> 4N - background lookup table
//PL1-2   -> 1T - red and green component
//PL1-1   -> 1R - blue component

/*
 * There are 5 official ROM variants supported by MAME:
 * Pacland - Namco
 * J - Japan
 * Jo - Japan old
 * Jo2 - Japan older
 * M - Midway Bally
 *
 * Although the ICT supports all 5 variants, Jo and Jo2 are essentially identical from a 'CPU accessible' perspective.
 *
 * There are two highscore mods available for each ROM set; one with and one without the software highscore reset feature.
 * These mods were created by cmonkey on UKVac and permission was given to support them in the ICT.
 */

static const UINT32 s_MPAGE_ROM_SLICE = 0x4000; //Address where ROM slices become accessible by CPU
static const UINT32 s_MPAGE_ROM_SLICE_SIZE = 0x2000; //Size of ROM slice

static const UINT32 s_PL_8B_ROM = 0x8000; //Address of 8B ROM - a000
static const UINT32 s_PL_8D_ROM = 0xC000; //Address of 8D ROM - e000
static const UINT32 s_PL_ROM_SIZE = 0x4000; //Size of ROM


//UINT16 arrays represent the ROM data at power of 2 offset addresses for the address bit check.
//        e.g. [0]===0x001, [1]===0x002, [2]===0x004, [3]===0x008 ...
// For example a ROM length of:-
//  - 0x0400 bytes (max address 0x3FF) has 10 data samples.
//  - 0x1000 bytes (max address 0xFFF) has 12 data samples.
//  - 0x2000 bytes (max address 0x1FFF) has 13 data samples.
//  - 0x4000 bytes (max address 0x3FFF) has 14 data samples.
//
// Use 'rom_idx_dump.sh' to generate these dumps for you!
// If you need a ROM slice first then use 'rom_splitter.sh'

// Default set from MAME.
//
// First 2 ROMs are 0x4000 program ROMs - 14 data samples needed
//                                                   0x01 0x02 0x04 0x08 0x10 0x20 0x40 0x80 x100 x200 x400 x800 1000 2000
 static const UINT16 s_romData_PL5_01B_8B[]   =     {0xff,0x50,0x43,0x4e,0xbd,0x00,0x30,0x8e,0x50,0x83,0xed,0x88,0x00,0x1f};  //0xB0EA7631
 static const UINT16 s_romData_PL5_02_8D[]    =     {0xff,0x50,0x43,0x4e,0x5f,0x5f,0x25,0xfb,0x0f,0x27,0x02,0x8e,0x9a,0x20};  //0xD903E84E, 0x4DDA0854 Hi, 0xFF0B7D1D Hi with Reset

 //Banked ROMs are 0x2000 ROM slices - 13 data samples needed
 static const UINT16 s_romData_PL1_03_8E_Lo[] =     {0x84,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x4f,0x56,0xed,0x3a};  //0xA9EFA48D
 static const UINT16 s_romData_PL1_03_8E_Hi[] =     {0x26,0xe3,0xe4,0xe3,0xe3,0xe3,0xe3,0x20,0xa1,0xb0,0xc0,0x25,0xc3};  //0x3ABCB7CD
 static const UINT16 s_romData_PL1_04_8F_Lo[] =     {0x1e,0x43,0x43,0x43,0x44,0x45,0xc0,0xc0,0xe4,0xc0,0xe4,0xe3,0xe3};  //0x1A11C805
 static const UINT16 s_romData_PL1_04_8F_Hi[] =     {0x08,0xdd,0xc0,0xdd,0xdd,0xc0,0xdd,0xcc,0xc0,0xc0,0xcf,0xdd,0xc3};  //0x517D4DB8
 static const UINT16 s_romData_PL1_05_8H_Lo[] =     {0x07,0x61,0x61,0x66,0x40,0x40,0x40,0x63,0x62,0x6a,0x40,0x40,0x40};  //0x570B72EA
 static const UINT16 s_romData_PL1_05_8H_Hi[] =     {0x04,0x40,0x40,0x4d,0x31,0x49,0x00,0x00,0x00,0x00,0x00,0x00,0x00};  //0x872C56C0
 static const UINT16 s_romData_PL3_06_8J_Lo[] =     {0x3e,0xc1,0xc1,0xc1,0xc2,0xc3,0xc5,0x03,0x00,0x00,0x00,0x0c,0x98};  //0xAF660BE5
 static const UINT16 s_romData_PL3_06_8J_Hi[] =     {0x00,0xe0,0xe0,0xf9,0xe2,0xe5,0xed,0xf7,0x00,0x00,0x01,0x0d,0x30};  //0xC99CC9E9


// Variant sets.
//
//
//variant PL1_6_8J is reused for all variants but not base set above
// static const UINT16 s_romDataAlt_PL1_06_8J_Lo[] = s_romData_PL3_06_8J_Lo; Has same bytes at these locations - diff CRC  //0xAF660BE5
 static const UINT16 s_romDataAlt_PL1_06_8J_Hi[] =  {0x00,0xe0,0xe0,0xf7,0xe2,0xe5,0xed,0xf6,0x00,0x00,0x01,0x0d,0x30};    //0x567CA359

//paclandj
 static const UINT16 s_romDataJ_PL6_01_8B[] =       {0xff,0x50,0x43,0x4e,0xbd,0x00,0x30,0x8e,0x50,0x83,0xed,0x88,0x01,0xa0};    //0x4C96E11C, 0x99812147 Hi, 0x6644C92A RST
 static const UINT16 s_romDataJ_PL6_02_8D[] =       {0xff,0x50,0x43,0x4e,0x5f,0x5f,0x25,0xfb,0x0f,0x27,0x01,0x8e,0x9a,0x20};    //0x8CF5BD8D, 0xD15800B4 Hi, 0x7ED251C6 RST

//paclandjo and paclandjo2
// static const UINT16 s_romDataJo_PL1_1_8B[] =     s_romDataJ_PL6_01_8B; Has same bytes at these locations - diff CRC          //0xF729FB94, 0xB2F7DB47 Hi, 0xB2F7DB47 RST
 static const UINT16 s_romDataJo_PL1_2_8D[] =       {0xff,0x50,0x43,0x4e,0x5f,0x5f,0x25,0xfb,0x0f,0x27,0x01,0x8e,0xd7,0x04};    //0x5C66EB6F, 0x257E58EC Hi, 0xD736C6FC RST

//paclandm
// static const UINT16 s_romDataM_PL1_1[] =         s_romData_PL5_01B_8B; Has same bytes at these locations - diff CRC          //0xA938AE99
 static const UINT16 s_romDataM_PL1_2[] =           {0xff,0x50,0x43,0x4e,0x5f,0x5f,0x25,0xfb,0x0f,0x27,0x02,0x8e,0xd0,0x27};    //0x3FE43BB5, 0x464D170D Hi, 0x5F8E80C3 RST


//Banked ROMS.
//
//
//3c00 Bank and ROM Selector    - i.e. when the correct 8 bit number is written to address 0x3c00 the paging is triggered.
//Banked ROMs at 4000-5FFF      - the paged ROMs are readable in this address space.

//Write the magic number to address 0x3C00 and the new slice of ROM should appear magically between 0x4000 and 0x5FFF
//Treating each ROM as two banks - an upper and a lower bank.

//00000000 0x00 Pages in ROM PL1-3 Lower 8E
//00000001 0x01 Pages in ROM PL1-3 Upper 8E

//00000010 0x02 Pages in ROM PL1-4 Lower 8F
//00000011 0x03 Pages in ROM PL1-4 Upper 8F

//00000100 0x04 Pages in ROM PL1-5 Lower 8H
//00000101 0x05 Pages in ROM PL1-5 Upper 8H

//00000110 0x06 Pages in ROM PL1-6 / PL3-6 Lower 8J
//00000111 0x07 Pages in ROM PL1-6 / PL3-6 Upper 8J

//Pacland - Namco.
//
//
static const ROM_REGION s_romRegion[] PROGMEM =     {
    //Program ROMs
    {NO_BANK_SWITCH,                            s_PL_8B_ROM,        s_PL_ROM_SIZE,         s_romData_PL5_01B_8B,    0xB0EA7631, "8B "},  //0x8000 0xBFFF -> 4000 size
    {NO_BANK_SWITCH,                            s_PL_8D_ROM,        s_PL_ROM_SIZE,         s_romData_PL5_02_8D,     0xD903E84E, "8D "},  //0xC000 0xFFFF -> 4000 size
    
    //Banked ROMs
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Lo,  0xA9EFA48D, "8E0"}, //  Lo - write 0x00 to 0x3c00
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Hi,  0x3ABCB7CD, "8E1"}, //  Hi - write 0x01 to 0x3c00
    
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Lo,  0x1A11C805, "8F0"}, //  Lo - write 0x02 to 0x3c00
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Hi,  0x517D4DB8, "8F1"}, //  Hi - write 0x03 to 0x3c00
    
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Lo,  0x570B72EA, "8H0"}, //  Lo - write 0x04 to 0x3c00
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Hi,  0x872C56C0, "8H1"}, //  Hi - write 0x05 to 0x3c00
    
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL3_06_8J_Lo,  0xAF660BE5, "8J0"}, //  Lo - write 0x06 to 0x3c00
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL3_06_8J_Hi,  0xC99CC9E9, "8J1"}, //  Hi - write 0x07 to 0x3c00

    {0} }; // end of list

//Pacland - Namco - with hiscore mod
static const ROM_REGION s_romRegion_Hi[] PROGMEM =     {
    {NO_BANK_SWITCH,                            s_PL_8B_ROM,        s_PL_ROM_SIZE,         s_romData_PL5_01B_8B,    0xB0EA7631, "8B "},
    {NO_BANK_SWITCH,                            s_PL_8D_ROM,        s_PL_ROM_SIZE,         s_romData_PL5_02_8D,     0x4DDA0854, "8D "}, //Hiscore CRC
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Lo,  0xA9EFA48D, "8E0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Hi,  0x3ABCB7CD, "8E1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Lo,  0x1A11C805, "8F0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Hi,  0x517D4DB8, "8F1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Lo,  0x570B72EA, "8H0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Hi,  0x872C56C0, "8H1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL3_06_8J_Lo,  0xAF660BE5, "8J0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL3_06_8J_Hi,  0xC99CC9E9, "8J1"},
    {0} }; // end of list

//Pacland - Namco - with hiscore mod and software highscore reset feature
static const ROM_REGION s_romRegion_Hi_Rst[] PROGMEM =     {
    {NO_BANK_SWITCH,                            s_PL_8B_ROM,        s_PL_ROM_SIZE,         s_romData_PL5_01B_8B,    0xB0EA7631, "8B "},
    {NO_BANK_SWITCH,                            s_PL_8D_ROM,        s_PL_ROM_SIZE,         s_romData_PL5_02_8D,     0xFF0B7D1D, "8D "}, //Hiscore with Reset CRC
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Lo,  0xA9EFA48D, "8E0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Hi,  0x3ABCB7CD, "8E1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Lo,  0x1A11C805, "8F0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Hi,  0x517D4DB8, "8F1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Lo,  0x570B72EA, "8H0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Hi,  0x872C56C0, "8H1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL3_06_8J_Lo,  0xAF660BE5, "8J0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL3_06_8J_Hi,  0xC99CC9E9, "8J1"},
    {0} }; // end of list

//Pacland - Japan.
//
//
static const ROM_REGION s_romRegionSetJ[] PROGMEM =     {
    {NO_BANK_SWITCH,                            s_PL_8B_ROM,       s_PL_ROM_SIZE,          s_romDataJ_PL6_01_8B,        0x4C96E11C, "8B "},
    {NO_BANK_SWITCH,                            s_PL_8D_ROM,       s_PL_ROM_SIZE,          s_romDataJ_PL6_02_8D,        0x8CF5BD8D, "8D "},
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Lo,      0xA9EFA48D, "8E0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Hi,      0x3ABCB7CD, "8E1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Lo,      0x1A11C805, "8F0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Hi,      0x517D4DB8, "8F1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Lo,      0x570B72EA, "8H0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Hi,      0x872C56C0, "8H1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL3_06_8J_Lo,      0xAF660BE5, "8J0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romDataAlt_PL1_06_8J_Hi,   0x567CA359, "8J1"},
    {0} }; // end of list

static const ROM_REGION s_romRegionSetJ_Hi[] PROGMEM =     {
    {NO_BANK_SWITCH,                            s_PL_8B_ROM,       s_PL_ROM_SIZE,          s_romDataJ_PL6_01_8B,        0x99812147, "8B "}, //Hiscore CRC
    {NO_BANK_SWITCH,                            s_PL_8D_ROM,       s_PL_ROM_SIZE,          s_romDataJ_PL6_02_8D,        0xD15800B4, "8D "}, //Hiscore CRC
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Lo,      0xA9EFA48D, "8E0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Hi,      0x3ABCB7CD, "8E1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Lo,      0x1A11C805, "8F0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Hi,      0x517D4DB8, "8F1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Lo,      0x570B72EA, "8H0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Hi,      0x872C56C0, "8H1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL3_06_8J_Lo,      0xAF660BE5, "8J0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romDataAlt_PL1_06_8J_Hi,   0x567CA359, "8J1"},
    {0} }; // end of list

static const ROM_REGION s_romRegionSetJ_Hi_Rst[] PROGMEM =     {
    {NO_BANK_SWITCH,                            s_PL_8B_ROM,       s_PL_ROM_SIZE,          s_romDataJ_PL6_01_8B,        0x6644C92A, "8B "}, //Hiscore with reset CRC
    {NO_BANK_SWITCH,                            s_PL_8D_ROM,       s_PL_ROM_SIZE,          s_romDataJ_PL6_02_8D,        0x7ED251C6, "8D "}, //Hiscore with reset CRC
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Lo,      0xA9EFA48D, "8E0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Hi,      0x3ABCB7CD, "8E1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Lo,      0x1A11C805, "8F0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Hi,      0x517D4DB8, "8F1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Lo,      0x570B72EA, "8H0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Hi,      0x872C56C0, "8H1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL3_06_8J_Lo,      0xAF660BE5, "8J0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romDataAlt_PL1_06_8J_Hi,   0x567CA359, "8J1"},
    {0} }; // end of list

//Pacland - Japan old and older.
//
//
static const ROM_REGION s_romRegionSetJo[] PROGMEM =     {
    {NO_BANK_SWITCH,                            s_PL_8B_ROM,       s_PL_ROM_SIZE,          s_romDataJ_PL6_01_8B,        0xF729FB94, "8B "},
    {NO_BANK_SWITCH,                            s_PL_8D_ROM,       s_PL_ROM_SIZE,          s_romDataJo_PL1_2_8D,        0x5C66EB6F, "8D "},
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Lo,      0xA9EFA48D, "8E0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Hi,      0x3ABCB7CD, "8E1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Lo,      0x1A11C805, "8F0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Hi,      0x517D4DB8, "8F1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Lo,      0x570B72EA, "8H0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Hi,      0x872C56C0, "8H1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL3_06_8J_Lo,      0xAF660BE5, "8J0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romDataAlt_PL1_06_8J_Hi,   0x567CA359, "8J1"},
    {0} }; // end of list

static const ROM_REGION s_romRegionSetJo_Hi[] PROGMEM =     {
    {NO_BANK_SWITCH,                            s_PL_8B_ROM,       s_PL_ROM_SIZE,          s_romDataJ_PL6_01_8B,        0xB2F7DB47, "8B "}, //Hiscore CRC
    {NO_BANK_SWITCH,                            s_PL_8D_ROM,       s_PL_ROM_SIZE,          s_romDataJo_PL1_2_8D,        0x257E58EC, "8D "}, //Hiscore CRC
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Lo,      0xA9EFA48D, "8E0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Hi,      0x3ABCB7CD, "8E1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Lo,      0x1A11C805, "8F0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Hi,      0x517D4DB8, "8F1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Lo,      0x570B72EA, "8H0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Hi,      0x872C56C0, "8H1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL3_06_8J_Lo,      0xAF660BE5, "8J0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romDataAlt_PL1_06_8J_Hi,   0x567CA359, "8J1"},
    {0} }; // end of list

static const ROM_REGION s_romRegionSetJo_Hi_Rst[] PROGMEM =     {
    {NO_BANK_SWITCH,                            s_PL_8B_ROM,       s_PL_ROM_SIZE,          s_romDataJ_PL6_01_8B,        0xB2F7DB47, "8B "}, //Hiscore with reset CRC
    {NO_BANK_SWITCH,                            s_PL_8D_ROM,       s_PL_ROM_SIZE,          s_romDataJo_PL1_2_8D,        0xD736C6FC, "8D "}, //Hiscore with reset CRC
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Lo,      0xA9EFA48D, "8E0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Hi,      0x3ABCB7CD, "8E1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Lo,      0x1A11C805, "8F0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Hi,      0x517D4DB8, "8F1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Lo,      0x570B72EA, "8H0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Hi,      0x872C56C0, "8H1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL3_06_8J_Lo,      0xAF660BE5, "8J0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romDataAlt_PL1_06_8J_Hi,   0x567CA359, "8J1"},
    {0} }; // end of list

//Pacland - Bally Midway.
//
//
static const ROM_REGION s_romRegionSetM[] PROGMEM =     {
    {NO_BANK_SWITCH,                            s_PL_8B_ROM,       s_PL_ROM_SIZE,          s_romData_PL5_01B_8B,        0xA938AE99, "8B "},
    {NO_BANK_SWITCH,                            s_PL_8D_ROM,       s_PL_ROM_SIZE,          s_romDataM_PL1_2,            0x3FE43BB5, "8D "},
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Lo,      0xA9EFA48D, "8E0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Hi,      0x3ABCB7CD, "8E1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Lo,      0x1A11C805, "8F0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Hi,      0x517D4DB8, "8F1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Lo,      0x570B72EA, "8H0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Hi,      0x872C56C0, "8H1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL3_06_8J_Lo,      0xAF660BE5, "8J0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romDataAlt_PL1_06_8J_Hi,   0x567CA359, "8J1"},
    {0} }; // end of list

static const ROM_REGION s_romRegionSetM_Hi[] PROGMEM =     {
    {NO_BANK_SWITCH,                            s_PL_8B_ROM,       s_PL_ROM_SIZE,          s_romData_PL5_01B_8B,        0xA938AE99, "8B "},
    {NO_BANK_SWITCH,                            s_PL_8D_ROM,       s_PL_ROM_SIZE,          s_romDataM_PL1_2,            0x464D170D, "8D "}, //Hiscore CRC
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Lo,      0xA9EFA48D, "8E0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Hi,      0x3ABCB7CD, "8E1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Lo,      0x1A11C805, "8F0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Hi,      0x517D4DB8, "8F1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Lo,      0x570B72EA, "8H0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Hi,      0x872C56C0, "8H1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL3_06_8J_Lo,      0xAF660BE5, "8J0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romDataAlt_PL1_06_8J_Hi,   0x567CA359, "8J1"},
    {0} }; // end of list

static const ROM_REGION s_romRegionSetM_Hi_Rst[] PROGMEM =     {
    {NO_BANK_SWITCH,                            s_PL_8B_ROM,       s_PL_ROM_SIZE,          s_romData_PL5_01B_8B,        0xA938AE99, "8B "},
    {NO_BANK_SWITCH,                            s_PL_8D_ROM,       s_PL_ROM_SIZE,          s_romDataM_PL1_2,            0x5F8E80C3, "8D "}, //Hiscore with reset CRC
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Lo,      0xA9EFA48D, "8E0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8E_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_03_8E_Hi,      0x3ABCB7CD, "8E1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Lo,      0x1A11C805, "8F0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8F_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_04_8F_Hi,      0x517D4DB8, "8F1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Lo,      0x570B72EA, "8H0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8H_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL1_05_8H_Hi,      0x872C56C0, "8H1"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Lo, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romData_PL3_06_8J_Lo,      0xAF660BE5, "8J0"},
    {CPaclandBaseGame::onBankSwitchMPAGE_8J_Hi, s_MPAGE_ROM_SLICE, s_MPAGE_ROM_SLICE_SIZE, s_romDataAlt_PL1_06_8J_Hi,   0x567CA359, "8J1"},
    {0} }; // end of list


//Namco - MAME default
IGame*
CPaclandGame::createInstance(
)
{
    return (new CPaclandGame(s_romRegion));
}

// with Hiscore mod
IGame*
CPaclandGame::createInstance_Hi(
)
{
    return (new CPaclandGame(s_romRegion_Hi));
}

//with Hiscore and Hiscore reset mod
IGame*
CPaclandGame::createInstance_Hi_Rst(
)
{
    return (new CPaclandGame(s_romRegion_Hi_Rst));
}

//Japan
IGame*
CPaclandGame::createInstance_J(
)
{
    return (new CPaclandGame(s_romRegionSetJ));
}

IGame*
CPaclandGame::createInstance_J_Hi(
)
{
    return (new CPaclandGame(s_romRegionSetJ_Hi));
}

IGame*
CPaclandGame::createInstance_J_Hi_Rst(
)
{
    return (new CPaclandGame(s_romRegionSetJ_Hi_Rst));
}

//Japan Old
IGame*
CPaclandGame::createInstance_Jo(
)
{
    return (new CPaclandGame(s_romRegionSetJo));
}

IGame*
CPaclandGame::createInstance_Jo_Hi(
)
{
    return (new CPaclandGame(s_romRegionSetJo_Hi));
}

IGame*
CPaclandGame::createInstance_Jo_Hi_Rst(
)
{
    return (new CPaclandGame(s_romRegionSetJo_Hi_Rst));
}

//Japan Older
IGame*
CPaclandGame::createInstance_Jo2(
)
{
    //Currently using same set as Jo as CPU accessible ROMs are same
    return (new CPaclandGame(s_romRegionSetJo));
}

IGame*
CPaclandGame::createInstance_Jo2_Hi(
)
{
    return (new CPaclandGame(s_romRegionSetJo_Hi));
}

IGame*
CPaclandGame::createInstance_Jo2_Hi_Rst(
)
{
    return (new CPaclandGame(s_romRegionSetJo_Hi_Rst));
}

//Bally Midway
IGame*
CPaclandGame::createInstance_M(
)
{
    return (new CPaclandGame(s_romRegionSetM));
}

IGame*
CPaclandGame::createInstance_M_Hi(
)
{
    return (new CPaclandGame(s_romRegionSetM_Hi));
}

IGame*
CPaclandGame::createInstance_M_Hi_Rst(
)
{
    return (new CPaclandGame(s_romRegionSetM_Hi_Rst));
}


CPaclandGame::CPaclandGame(
const ROM_REGION *romRegion
) : CPaclandBaseGame( romRegion )
{
}



