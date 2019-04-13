//
// Copyright (c) 2019, Phillip Riscombe-Burton
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
#include "CPunchOutGame.h"

static const UINT32 s_ROM_2764_SIZE = 0x2000; //8KB - 8192 bytes - 0x2000 in hex
static const UINT32 s_ROM_27128_SIZE = 0x4000; //16KB - 16384 bytes - 0x4000 in hex

// 0000-bfff ROM:
// 0000 0000 0000 0000 - 0x0000 to 0x1FFF - 2764  - 8k ROM  - 0x2000 - 8L
// 0010 0000 0000 0000 - 0x2000 to 0x3FFF - 2764  - 8k ROM  - 0x2000 - 8K 
// 0100 0000 0000 0000 - 0x4000 to 0x5FFF - 2764  - 8k ROM  - 0x2000 - 8J
// 0110 0000 0000 0000 - 0x6000 to 0x7FFF - 2764  - 8k ROM  - 0x2000 - 8H
// 1000 0000 0000 0000 - 0x8000 to 0xBFFF - 27128 - 16k ROM - 0x4000 - 8F


// 2n test of ROMS                                01   02   04   08   10   20   40   80  100  200  400  800
static const UINT16 s_romData_B_8L[]    =       {0x00,0x08,0x00,0x11,0x3e,0xff,0x00,0x01,0xc3,0x00,0xcd,0x02,0xff};
static const UINT16 s_romData_B_8K[]    =       {0xd0,0x7e,0x08,0x34,0xcd,0x23,0x20,0xd9,0xe6,0x00,0x2e,0xb6,0xe6};
static const UINT16 s_romData_B_8J[]    =       {0x82,0x20,0xfe,0x3f,0x05,0x81,0x24,0xff,0xf8,0x8e,0x10,0xff,0x83};
static const UINT16 s_romData_B_8H[]    =       {0x03,0x7b,0x00,0xab,0x0a,0xf4,0x81,0x83,0x02,0x12,0x83,0x04,0x82};
static const UINT16 s_romData_B_8F[]    =       {0x26,0x89,0x23,0x00,0x83,0x91,0x89,0x26,0x00,0x6a,0x26,0x26,0xb9,0x41};

static const UINT16 s_romData_J_8L[]    =       {0x00,0x08,0x00,0x11,0x3e,0xff,0x00,0x01,0xc3,0x00,0xcd,0x02,0xff};
static const UINT16 s_romData_J_8K[]    =       {0xd0,0x7e,0x08,0x34,0xcd,0x23,0x20,0xd9,0xe6,0x00,0x2e,0xb6,0xe6};
static const UINT16 s_romData_J_8F[]    =       {0x26,0x89,0x23,0x00,0x83,0x91,0x89,0x26,0x00,0x6a,0x26,0x26,0xb9,0x20};

static const UINT16 s_romData_I_8L[]    =       {0x00,0x08,0x00,0x11,0x3e,0xff,0x00,0xf2,0xc3,0x00,0x08,0xff,0x1a};
static const UINT16 s_romData_I_8K[]    =       {0x23,0x11,0x22,0xcd,0x23,0xca,0xcd,0x0a,0x43,0x47,0x1e,0xdd,0xe6};
static const UINT16 s_romData_I_8J[]    =       {0xca,0x7f,0xd6,0xd6,0x03,0x91,0xd9,0x00,0x0f,0x91,0x17,0x60,0x06};
static const UINT16 s_romData_I_8H[]    =       {0x02,0x17,0x91,0x15,0x1b,0x13,0x81,0x2f,0xe5,0x03,0x04,0x00,0x71};
static const UINT16 s_romData_I_8F[]    =       {0x00,0x25,0x83,0x24,0x10,0x00,0x83,0x25,0x81,0x25,0x04,0x51,0xa2,0x00};

static const UINT16 s_romData_S_8L[]    =       {0x00,0x08,0x81,0x11,0x3e,0xff,0x00,0xf2,0xc3,0x00,0x0e,0x0a,0x09};
static const UINT16 s_romData_S_8K[]    =       {0x23,0x56,0xed,0x7e,0x94,0x79,0xa7,0x3e,0xc6,0xcb,0xb0,0x16,0x23};
static const UINT16 s_romData_S_8J[]    =       {0x2b,0x08,0x0f,0x74,0x89,0x0f,0x8f,0x07,0xea,0x00,0x2b,0x0a,0x46};
static const UINT16 s_romData_S_8H[]    =       {0x85,0x11,0x00,0x83,0x85,0x15,0x00,0x85,0x16,0x0a,0x00,0x81,0x14};
static const UINT16 s_romData_S_8F[]    =       {0xfd,0x81,0xcf,0x06,0x0f,0x82,0x04,0x83,0xcf,0x62,0x83,0x20,0xc0,0x00};

static const UINT16 s_romData_SJ_8L[]    =      {0x00,0x08,0x81,0x11,0x3e,0xff,0x00,0xf2,0xc3,0x00,0x0e,0x0a,0x09};
static const UINT16 s_romData_SJ_8K[]    =      {0x23,0x56,0xed,0x7e,0x94,0x79,0xa7,0x3e,0xc6,0xcb,0xb0,0x2b,0xdd};
static const UINT16 s_romData_SJ_8F[]    =      {0xfd,0x81,0xcf,0x06,0x0f,0x82,0x04,0x83,0xcf,0x62,0x83,0x20,0xc0,0xd0};

static const UINT16 s_romData_AR_8L[]    =      {0x00,0x08,0xb5,0x11,0x3e,0x6f,0x00,0xa6,0xc2,0x3e,0xd3,0x74,0xd5};
static const UINT16 s_romData_AR_8K[]    =      {0x0f,0xf6,0x77,0x87,0xd9,0x36,0x07,0x3a,0x54,0xd0,0xaa,0xe9,0x34};
static const UINT16 s_romData_AR_8J[]    =      {0x91,0x09,0xa1,0x0a,0xa1,0xf8,0xa1,0x1a,0x56,0x01,0xe0,0xe8,0xca};
static const UINT16 s_romData_AR_8H[]    =      {0x3c,0x83,0xeb,0xe8,0x25,0xe7,0x43,0xfd,0x81,0x74,0x01,0x6c,0x01};
static const UINT16 s_romData_AR_8F[]    =      {0x57,0x82,0x80,0x44,0x01,0xb6,0x84,0x72,0x2f,0x81,0x85,0x81,0xc1,0x11};

// Addresses
static const UINT32 s_ROM_ADDR_8L =  0x0000;
static const UINT32 s_ROM_ADDR_8K =  0x2000;
static const UINT32 s_ROM_ADDR_8J =  0x4000;
static const UINT32 s_ROM_ADDR_8H =  0x6000;
static const UINT32 s_ROM_ADDR_8F =  0x8000;

//
// Set B from MAME - 'punchout'
//
static const ROM_REGION s_romRegionSetB[] PROGMEM = {
    {NO_BANK_SWITCH, s_ROM_ADDR_8L, s_ROM_2764_SIZE,  s_romData_B_8L,  0xA4003ADC, "8L "}, // chp1-c.8l
    {NO_BANK_SWITCH, s_ROM_ADDR_8K, s_ROM_2764_SIZE,  s_romData_B_8K,  0x745ECF40, "8K "}, // chp1-c.8k
    {NO_BANK_SWITCH, s_ROM_ADDR_8J, s_ROM_2764_SIZE,  s_romData_B_8J,  0x7A7F870E, "8J "}, // chp1-c.8j
    {NO_BANK_SWITCH, s_ROM_ADDR_8H, s_ROM_2764_SIZE,  s_romData_B_8H,  0x5D8123D7, "8H "}, // chp1-c.8h
    {NO_BANK_SWITCH, s_ROM_ADDR_8F, s_ROM_27128_SIZE, s_romData_B_8F,  0xC8A55DDB, "8F "}, // chp1-c.8f
	{0} }; // end of list

//
// Set A from MAME - 'punchouta'
//
static const ROM_REGION s_romRegionSetA[] PROGMEM = *s_romRegionSetB;

//
// Set J from MAME - 'punchout J' - Japanese
//
static const ROM_REGION s_romRegionSetJ[] PROGMEM = {
    {NO_BANK_SWITCH, s_ROM_ADDR_8L, s_ROM_2764_SIZE,  s_romData_J_8L,  0x9735EB5A, "8L "}, // chp1-c_8l_a.8l
    {NO_BANK_SWITCH, s_ROM_ADDR_8K, s_ROM_2764_SIZE,  s_romData_J_8K,  0x98BABA41, "8K "}, // chp1-c_8k_a.8k
    {NO_BANK_SWITCH, s_ROM_ADDR_8J, s_ROM_2764_SIZE,  s_romData_B_8J,  0x7A7F870E, "8J "}, // chp1-c.8j
    {NO_BANK_SWITCH, s_ROM_ADDR_8H, s_ROM_2764_SIZE,  s_romData_B_8H,  0x5D8123D7, "8H "}, // chp1-c.8h
    {NO_BANK_SWITCH, s_ROM_ADDR_8F, s_ROM_27128_SIZE, s_romData_J_8F,  0xEA52CDA1, "8F "}, // chp1-c_8f_a.8f
	{0} }; // end of list

//
// Set I from MAME - 'punchita' - Italian boot
//
static const ROM_REGION s_romRegionSetI[] PROGMEM = {
    {NO_BANK_SWITCH, s_ROM_ADDR_8L, s_ROM_2764_SIZE,  s_romData_I_8L,  0x1D595CE2, "8L "}, // chp1-c.8l
    {NO_BANK_SWITCH, s_ROM_ADDR_8K, s_ROM_2764_SIZE,  s_romData_I_8K,  0xC062FA5C, "8K "}, // chp1-c.8k
    {NO_BANK_SWITCH, s_ROM_ADDR_8J, s_ROM_2764_SIZE,  s_romData_I_8J,  0x48D453EF, "8J "}, // chp1-c.8j
    {NO_BANK_SWITCH, s_ROM_ADDR_8H, s_ROM_2764_SIZE,  s_romData_I_8H,  0x67F5AEDC, "8H "}, // chp1-c.8h
    {NO_BANK_SWITCH, s_ROM_ADDR_8F, s_ROM_27128_SIZE, s_romData_I_8F,  0x761DE4F3, "8F "}, // chp1-c.8f
	{0} }; // end of list

//
// Set S from MAME - 'spnchout' - Super Punch-Out!!!
//
static const ROM_REGION s_romRegionSetS[] PROGMEM = {
    {NO_BANK_SWITCH, s_ROM_ADDR_8L, s_ROM_2764_SIZE,  s_romData_S_8L,  0x703B9780, "8L "}, // chs1-c.8l
    {NO_BANK_SWITCH, s_ROM_ADDR_8K, s_ROM_2764_SIZE,  s_romData_S_8K,  0xE13719F6, "8K "}, // chs1-c.8k
    {NO_BANK_SWITCH, s_ROM_ADDR_8J, s_ROM_2764_SIZE,  s_romData_S_8J,  0x1FA629E8, "8J "}, // chs1-c.8j
    {NO_BANK_SWITCH, s_ROM_ADDR_8H, s_ROM_2764_SIZE,  s_romData_S_8H,  0x15A6C068, "8H "}, // chs1-c.8h
    {NO_BANK_SWITCH, s_ROM_ADDR_8F, s_ROM_27128_SIZE, s_romData_S_8F,  0x4FF3CDD9, "8F "}, // chs1-c.8f
	{0} }; // end of list

//
// Set SA from MAME - 'spnchouta'
//
static const ROM_REGION s_romRegionSetSA[] PROGMEM = *s_romRegionSetS;

//
// Set SJ from MAME - 'spnchoutj' - Japanese
//
static const ROM_REGION s_romRegionSetSJ[] PROGMEM = {
    {NO_BANK_SWITCH, s_ROM_ADDR_8L, s_ROM_2764_SIZE,  s_romData_SJ_8L,  0xDC2A592B, "8L "}, // chs1c8la.bin
    {NO_BANK_SWITCH, s_ROM_ADDR_8K, s_ROM_2764_SIZE,  s_romData_SJ_8K,  0xCE687182, "8K "}, // chs1c8ka.bin
    {NO_BANK_SWITCH, s_ROM_ADDR_8J, s_ROM_2764_SIZE,  s_romData_S_8J,   0x1FA629E8, "8J "}, // chs1-c.8j
    {NO_BANK_SWITCH, s_ROM_ADDR_8H, s_ROM_2764_SIZE,  s_romData_S_8H,   0x15A6C068, "8H "}, // chs1-c.8h
    {NO_BANK_SWITCH, s_ROM_ADDR_8F, s_ROM_27128_SIZE, s_romData_SJ_8F,  0xF745B5D5, "8F "}, // chs1c8fa.bin
	{0} }; // end of list

//
// Set AR from MAME - 'armwrest' - Arm Wrestling
//
static const ROM_REGION s_romRegionSetAR[] PROGMEM = {
    {NO_BANK_SWITCH, s_ROM_ADDR_8L, s_ROM_2764_SIZE,  s_romData_AR_8L,  0xB09764C1, "8L "}, // chv1-c.8l
    {NO_BANK_SWITCH, s_ROM_ADDR_8K, s_ROM_2764_SIZE,  s_romData_AR_8K,  0x0E147FF7, "8K "}, // chv1-c.8k
    {NO_BANK_SWITCH, s_ROM_ADDR_8J, s_ROM_2764_SIZE,  s_romData_AR_8J,  0xE7365289, "8J "}, // chv1-c.8j
    {NO_BANK_SWITCH, s_ROM_ADDR_8H, s_ROM_2764_SIZE,  s_romData_AR_8H,  0xA2118EEC, "8H "}, // chv1-c.8h
    {NO_BANK_SWITCH, s_ROM_ADDR_8F, s_ROM_27128_SIZE, s_romData_AR_8F,  0x664A07C4, "8F "}, // chpv-c.8f
	{0} }; // end of list


//
// Input region is the same for all ROM versions?
//
static const INPUT_REGION s_inputRegion[] PROGMEM = {
{0}
};


//
// Output region is the same for all ROM versions?
//
static const OUTPUT_REGION s_outputRegion[] PROGMEM = {
{0}
};




IGame*
CPunchOutGame::createInstanceSetB(
)
{
    return (new CPunchOutGame(s_romRegionSetB));
}

IGame*
CPunchOutGame::createInstanceSetA(
)
{
    return (new CPunchOutGame(s_romRegionSetA));
}

IGame*
CPunchOutGame::createInstanceSetJ(
)
{
    return (new CPunchOutGame(s_romRegionSetJ));
}

IGame*
CPunchOutGame::createInstanceSetI(
)
{
    return (new CPunchOutGame(s_romRegionSetI));
}

IGame*
CPunchOutGame::createInstanceSetS(
)
{
    return (new CPunchOutGame(s_romRegionSetS));
}

IGame*
CPunchOutGame::createInstanceSetSA(
)
{
    return (new CPunchOutGame(s_romRegionSetSA));
}

IGame*
CPunchOutGame::createInstanceSetSJ(
)
{
    return (new CPunchOutGame(s_romRegionSetSJ));
}

IGame*
CPunchOutGame::createInstanceSetAR(
)
{
    return (new CPunchOutGame(s_romRegionSetAR));
}

CPunchOutGame::CPunchOutGame(
    const ROM_REGION *romRegion
) : CPunchOutBaseGame( romRegion )
{
}


