//
// Copyright (c) 2019 Warren Ondras
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
#include "CSpaceDuelGame.h"

static const UINT32 s_ROM_2716_SIZE = 0x0800; // 2 KB
static const UINT32 s_ROM_2732_SIZE = 0x1000; // 4 KB

//2n test of ROMS                        01   02   04   08   10   20   40   80   100  200  400  800

// Set 1
//   program ROMs
static const UINT16 s_romData_1_R1[]  = {0x02,0xbb,0x30,0xa8,0xc0,0x00,0x02,0x64,0x5b,0xa3,0x4a,0x03}; // 136006-101.r1  - 0xcd239e6c
static const UINT16 s_romData_1_NP1[] = {0x07,0x09,0x08,0x03,0xb0,0x04,0xf0,0x7d,0xf0,0x4a,0x02,0x18}; // 136006-102.np1 - 0x4c451e8a
static const UINT16 s_romData_1_M1[]  = {0x22,0x18,0xa9,0xa9,0x02,0x01,0xc2,0xa9,0x06,0x91,0xb9,0x4d}; // 136006-103.m1  - 0xee72da63
static const UINT16 s_romData_1_KL1[] = {0xbd,0xdd,0xa6,0x60,0xe6,0xd6,0x41,0x68,0x00,0x10,0x03,0xa2}; // 136006-104.kl1 - 0xe41b38a3
static const UINT16 s_romData_1_J1[]  = {0x00,0x00,0x00,0x00,0x00,0x00,0xa2,0x8d,0xd7,0x08,0xd2,0x91}; // 136006-105.j1  - 0x5652710f
//   vector ROMs
static const UINT16 s_romData_1_NP7[] = {0xaf,0xf8,0x02,0xfe,0x02,0xfc,0x00,0x00,0x3a,0x20,0xb8,0x06}; // 136006-107.np7 - 0xd8dd0461
static const UINT16 s_romData_1_R7[]  = {0x28,0x7a,0xb0,0x36,0x36,0x36,0x96,0x06,0x10,0x08,0xf6};      // 136006-106.r7  - 0x691122fe

// Set 2 changes
static const UINT16 s_romData_2_R1[]  = {0x02,0xbb,0x30,0xa8,0xc0,0x00,0x02,0x64,0x5b,0xa3,0x4a,0x03}; // 136006-201.r1  - 0xf4037b6e

// Set "0" (prototype)
static const UINT16 s_romData_0_R1[]  = {0x4c,0x13,0x20,0x91,0x29,0x33,0x20,0xf0,0x9b,0xa9,0xff,0x29}; // 136006-001.r1  - 0x8f993ac8
static const UINT16 s_romData_0_NP1[] = {0xa9,0x61,0xaf,0xad,0x03,0xa8,0x77,0x23,0x20,0x95,0xf5,0x8d}; // 136006-002.np1 - 0x32cca051
static const UINT16 s_romData_0_M1[]  = {0x0c,0x86,0xa9,0x20,0xa2,0x20,0x5f,0xcd,0x18,0x04,0x04,0x2a}; // 136006-003.m1  - 0x36624d57
static const UINT16 s_romData_0_KL1[] = {0xc8,0x60,0xf1,0xa9,0x40,0x9d,0x02,0xdd,0x65,0x98,0xc9,0x3c}; // 136006-004.kl1 - 0xb322bf0b
static const UINT16 s_romData_0_J1[]  = {0x00,0x00,0x19,0x1c,0x16,0x9d,0x27,0xa2,0xd0,0x08,0x06,0xa9}; // 136006-005.j1  - 0x0edb1242


// Program ROM addresses
static const UINT32 s_ROM_ADDR_R1  = 0x4000; // 0x8000-8fff (4K)
static const UINT32 s_ROM_ADDR_NP1 = 0x5000; // 0x7000-7fff (4K)
static const UINT32 s_ROM_ADDR_M1  = 0x6000; // 0x6000-6fff (4K)
static const UINT32 s_ROM_ADDR_KL1 = 0x7000; // 0x5000-5fff (4K)
static const UINT32 s_ROM_ADDR_J1  = 0x8000; // 0x4000-4fff (4K)

// Vector ROM addresses
static const UINT32 s_ROM_ADDR_R7  = 0x2800; // 0x2800-2fff (2K)
static const UINT32 s_ROM_ADDR_NP7 = 0x3000; // 0x3000-3fff (4K)


//
// Set 1 from MAME
//
static const ROM_REGION s_romRegionSet1[] PROGMEM = {
    // program ROMs
    {NO_BANK_SWITCH, s_ROM_ADDR_R1,  s_ROM_2732_SIZE, s_romData_1_R1,  0xcd239e6c, "R1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_NP1, s_ROM_2732_SIZE, s_romData_1_NP1, 0x4c451e8a, "NP1"},
    {NO_BANK_SWITCH, s_ROM_ADDR_M1,  s_ROM_2732_SIZE, s_romData_1_M1,  0xee72da63, "M1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_KL1, s_ROM_2732_SIZE, s_romData_1_KL1, 0xe41b38a3, "KL1"},
    {NO_BANK_SWITCH, s_ROM_ADDR_J1,  s_ROM_2732_SIZE, s_romData_1_J1,  0x5652710f, "J1 "},
    // vector ROMs
    {NO_BANK_SWITCH, s_ROM_ADDR_R7,  s_ROM_2716_SIZE, s_romData_1_R7,  0x691122fe, "R7 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_NP7, s_ROM_2732_SIZE, s_romData_1_NP7, 0xd8dd0461, "NP7"},
    {0}  // end of list
};

//
// Set 2 from MAME
//
static const ROM_REGION s_romRegionSet2[] PROGMEM = {
    // program ROMs
    {NO_BANK_SWITCH, s_ROM_ADDR_R1,  s_ROM_2732_SIZE, s_romData_2_R1,  0xf4037b6e, "R1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_NP1, s_ROM_2732_SIZE, s_romData_1_NP1, 0x4c451e8a, "NP1"},
    {NO_BANK_SWITCH, s_ROM_ADDR_M1,  s_ROM_2732_SIZE, s_romData_1_M1,  0xee72da63, "M1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_KL1, s_ROM_2732_SIZE, s_romData_1_KL1, 0xe41b38a3, "KL1"},
    {NO_BANK_SWITCH, s_ROM_ADDR_J1,  s_ROM_2732_SIZE, s_romData_1_J1,  0x5652710f, "J1 "},
    // vector ROMs
    {NO_BANK_SWITCH, s_ROM_ADDR_R7,  s_ROM_2716_SIZE, s_romData_1_R7,  0x691122fe, "R7 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_NP7, s_ROM_2732_SIZE, s_romData_1_NP7, 0xd8dd0461, "NP7"},
    {0}  // end of list
};

//
// Set 0 (prototype) from MAME
//
static const ROM_REGION s_romRegionSet0[] PROGMEM = {
    // program ROMs
    {NO_BANK_SWITCH, s_ROM_ADDR_R1,  s_ROM_2732_SIZE, s_romData_0_R1,  0x8f993ac8, "R1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_NP1, s_ROM_2732_SIZE, s_romData_0_NP1, 0x32cca051, "NP1"},
    {NO_BANK_SWITCH, s_ROM_ADDR_M1,  s_ROM_2732_SIZE, s_romData_0_M1,  0x36624d57, "M1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_J1,  s_ROM_2732_SIZE, s_romData_0_J1,  0x0edb1242, "J1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_KL1, s_ROM_2732_SIZE, s_romData_0_KL1, 0xb322bf0b, "KL1"},
    // vector ROMs
    {NO_BANK_SWITCH, s_ROM_ADDR_R7,  s_ROM_2716_SIZE, s_romData_1_R7,  0x691122fe, "R7 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_NP7, s_ROM_2732_SIZE, s_romData_1_NP7, 0xd8dd0461, "NP7"},
    {0}  // end of list
};

//
// RAM regions
//
static const RAM_REGION s_ramRegion[] PROGMEM = {
    //                                                  "012", "012345"
    {NO_BANK_SWITCH, 0x0000,      0x03FF,      1, 0x0F, "H1 ", "Progrm"}, // Program RAM, 2114 (1024 x 4) lower
    {NO_BANK_SWITCH, 0x0000,      0x03FF,      1, 0xF0, "E1 ", "Progrm"}, // Program RAM, 2114 (1024 x 4) upper
    {NO_BANK_SWITCH, 0x2000,      0x23FF,      1, 0x0F, "K7 ", "Vector"}, // Vector RAM, 2114 (1024 x 4) lower
    {NO_BANK_SWITCH, 0x2000,      0x23FF,      1, 0xF0, "M7 ", "Vector"}, // Vector RAM, 2114 (1024 x 4) upper
    {NO_BANK_SWITCH, 0x2400,      0x27FF,      1, 0x0F, "J7 ", "Vector"}, // Vector RAM, 2114 (1024 x 4) lower
    {NO_BANK_SWITCH, 0x2400,      0x27FF,      1, 0xF0, "L7 ", "Vector"}, // Vector RAM, 2114 (1024 x 4) upper
    {0}  // end of list
};

//
// RAM regions for the byte-wide intensive random access memory test
//
static const RAM_REGION s_ramRegionByteOnly[] PROGMEM = {
    //                                                  "012", "012345"
    {NO_BANK_SWITCH, 0x0000,      0x03FF,      1, 0xFF, "1EH", "Progrm"}, // Program RAM H1/E1
    {NO_BANK_SWITCH, 0x2000,      0x23FF,      1, 0xFF, "7KM", "Vector"}, // Vector RAM K7/M7
    {NO_BANK_SWITCH, 0x2400,      0x27FF,      1, 0xFF, "7JL", "Vector"}, // Vector RAM J7/L7
    {0}  // end of list
};

//
// No write-only RAM
//
static const RAM_REGION s_ramRegionWriteOnly[] PROGMEM = {
    {0}  // end of list
};

//
// Hardware addresses
//
static const UINT16 s_IRQ_RESET_ADDR = 0x0e00;
static const UINT16 s_POKEY0_ADDR = 0x1000;
static const UINT16 s_POKEY1_ADDR = 0x1400;
static const UINT16 s_EAROM_WRITE_ADDR = 0x0f00;
static const UINT16 s_EAROM_CONTROL_ADDR = 0x0e80;
static const UINT16 s_EAROM_READ_ADDR = 0x0a00;
static const UINT16 s_CONF_ADDRESS = 0x0904;  // address for START button to confirm destructive EAROM operations
static const UINT8  s_CONF_MASK = (UINT8)CSpaceDuelBaseGame::s_MSK_D6; // bitmask for START button
static const bool   s_CONF_ACTIVE_LOW =  false; // START button is active high

//
// Input region
//
static const INPUT_REGION s_inputRegion[] PROGMEM = { //     "012", "012345"

    // M9 is a 74LS244 buffer
    {NO_BANK_SWITCH, 0x0800,  CSpaceDuelBaseGame::s_MSK_D7,  "M9 ", "3KHZ  "}, // M9 - pin 11
    {NO_BANK_SWITCH, 0x0800,  CSpaceDuelBaseGame::s_MSK_D6,  "M9 ", "HALT  "}, // M9 - pin 13
    {NO_BANK_SWITCH, 0x0800,  CSpaceDuelBaseGame::s_MSK_D5,  "M9 ", "DIAGST"}, // M9 - pin 15
    {NO_BANK_SWITCH, 0x0800,  CSpaceDuelBaseGame::s_MSK_D4,  "M9 ", "SLFTST"}, // M9 - pin 17
    {NO_BANK_SWITCH, 0x0800,  CSpaceDuelBaseGame::s_MSK_D3,  "M9 ", "SLAM  "}, // M9 - pin 6
    {NO_BANK_SWITCH, 0x0800,  CSpaceDuelBaseGame::s_MSK_D2,  "M9 ", "COINAX"}, // M9 - pin 4
    {NO_BANK_SWITCH, 0x0800,  CSpaceDuelBaseGame::s_MSK_D1,  "M9 ", "COIN L"}, // M9 - pin 2
    {NO_BANK_SWITCH, 0x0800,  CSpaceDuelBaseGame::s_MSK_D0,  "M9 ", "COIN R"}, // M9 - pin 8

    // L9 and N9 are 74LS251 multiplexers
    {NO_BANK_SWITCH, 0x0900,  CSpaceDuelBaseGame::s_MSK_D7,  "N9 ", "SHLD 1"}, // N9 - pin 4
    {NO_BANK_SWITCH, 0x0901,  CSpaceDuelBaseGame::s_MSK_D7,  "N9 ", "SHLD 2"}, // N9 - pin 3
    {NO_BANK_SWITCH, 0x0902,  CSpaceDuelBaseGame::s_MSK_D7,  "N9 ", "ROTL 1"}, // N9 - pin 2
    {NO_BANK_SWITCH, 0x0903,  CSpaceDuelBaseGame::s_MSK_D7,  "N9 ", "ROTL 2"}, // N9 - pin 1
    {NO_BANK_SWITCH, 0x0904,  CSpaceDuelBaseGame::s_MSK_D7,  "N9 ", "THRST1"}, // N9 - pin 15
    {NO_BANK_SWITCH, 0x0905,  CSpaceDuelBaseGame::s_MSK_D7,  "N9 ", "THRST2"}, // N9 - pin 14
    {NO_BANK_SWITCH, 0x0906,  CSpaceDuelBaseGame::s_MSK_D7,  "N9 ", "GMSLCT"}, // N9 - pin 13
    {NO_BANK_SWITCH, 0x0907,  CSpaceDuelBaseGame::s_MSK_D7,  "N9 ", "CAB TP"}, // N9 - pin 12
    {NO_BANK_SWITCH, 0x0900,  CSpaceDuelBaseGame::s_MSK_D6,  "L9 ", "FIRE 1"}, // L9 - pin 4
    {NO_BANK_SWITCH, 0x0901,  CSpaceDuelBaseGame::s_MSK_D6,  "L9 ", "FIRE 2"}, // L9 - pin 3
    {NO_BANK_SWITCH, 0x0902,  CSpaceDuelBaseGame::s_MSK_D6,  "L9 ", "ROTR 1"}, // L9 - pin 2
    {NO_BANK_SWITCH, 0x0903,  CSpaceDuelBaseGame::s_MSK_D6,  "L9 ", "ROTR 2"}, // L9 - pin 1
    {NO_BANK_SWITCH, 0x0904,  CSpaceDuelBaseGame::s_MSK_D6,  "L9 ", "START "}, // L9 - pin 15
    {NO_BANK_SWITCH, 0x0905,  CSpaceDuelBaseGame::s_MSK_D6,  "L9 ", "OPTSW1"}, // L9 - pin 14
    {NO_BANK_SWITCH, 0x0906,  CSpaceDuelBaseGame::s_MSK_D6,  "L9 ", "OPTSW2"}, // L9 - pin 13
    {NO_BANK_SWITCH, 0x0907,  CSpaceDuelBaseGame::s_MSK_D6,  "L9 ", "OPTSW3"}, // L9 - pin 12
    {0}  // end of list
};

//
// Output region
//
static const OUTPUT_REGION s_outputRegion[] PROGMEM = { //                                "012", "012345"
    
    // R9 is a 74LS273 latch
    {NO_BANK_SWITCH, 0x0c00, CSpaceDuelBaseGame::s_MSK_D7, CSpaceDuelBaseGame::s_ACT_Hi,  "R9 ", "INVRTY"}, // R9, pin 15
    {NO_BANK_SWITCH, 0x0c00, CSpaceDuelBaseGame::s_MSK_D6, CSpaceDuelBaseGame::s_ACT_Hi,  "R9 ", "INVRTX"}, // R9, pin 12
    {NO_BANK_SWITCH, 0x0c00, CSpaceDuelBaseGame::s_MSK_D5, CSpaceDuelBaseGame::s_ACT_Lo,  "R9 ", "STRT L"}, // R9, pin 16
    {NO_BANK_SWITCH, 0x0c00, CSpaceDuelBaseGame::s_MSK_D4, CSpaceDuelBaseGame::s_ACT_Lo,  "R9 ", "SLCT L"}, // R9, pin 19
    {NO_BANK_SWITCH, 0x0c00, CSpaceDuelBaseGame::s_MSK_D3, CSpaceDuelBaseGame::s_ACT_Hi,  "R9 ", "CNLOCK"}, // R9, pin 9
    //   (nothing on bit 2)
    {NO_BANK_SWITCH, 0x0c00, CSpaceDuelBaseGame::s_MSK_D1, CSpaceDuelBaseGame::s_ACT_Hi,  "R9 ", "CNCNTL"}, // R9, pin 6
    {NO_BANK_SWITCH, 0x0c00, CSpaceDuelBaseGame::s_MSK_D0, CSpaceDuelBaseGame::s_ACT_Hi,  "R9 ", "CNCNTR"}, // R9, pin 5
    
    // L4 is a 74LS42 address decoder
    {NO_BANK_SWITCH, 0x0c80, CSpaceDuelBaseGame::s_MSK_D7, CSpaceDuelBaseGame::s_ACT_Hi,  "L4 ", "VGGO  "}, // L4, pin 2
    {NO_BANK_SWITCH, 0x0d00, CSpaceDuelBaseGame::s_MSK_D7, CSpaceDuelBaseGame::s_ACT_Hi,  "L4 ", "WDCLR "}, // L4, pin 3
    {NO_BANK_SWITCH, 0x0d80, CSpaceDuelBaseGame::s_MSK_D7, CSpaceDuelBaseGame::s_ACT_Hi,  "L4 ", "VGRST "}, // L4, pin 4
    {NO_BANK_SWITCH, 0x0e00, CSpaceDuelBaseGame::s_MSK_D7, CSpaceDuelBaseGame::s_ACT_Hi,  "L4 ", "INTACK"}, // L4, pin 5
    {0} // end of list
};

IGame*
CSpaceDuelGame::createInstanceSet1(
)
{
    return (new CSpaceDuelGame(false, s_romRegionSet1));
}

IGame*
CSpaceDuelGame::createInstanceClockMasterSet1(
)
{
    return (new CSpaceDuelGame(true, s_romRegionSet1));
}

IGame*
CSpaceDuelGame::createInstanceSet2(
)
{
    return (new CSpaceDuelGame(false, s_romRegionSet2));
}

IGame*
CSpaceDuelGame::createInstanceClockMasterSet2(
)
{
    return (new CSpaceDuelGame(true, s_romRegionSet2));
}

IGame*
CSpaceDuelGame::createInstanceSet0(
)
{
    return (new CSpaceDuelGame(false, s_romRegionSet0));
}

IGame*
CSpaceDuelGame::createInstanceClockMasterSet0(
)
{
    return (new CSpaceDuelGame(true, s_romRegionSet0));
}

CSpaceDuelGame::CSpaceDuelGame(
                               const bool       clockMaster,
                               const ROM_REGION *romRegion
                               ) : CSpaceDuelBaseGame( clockMaster,
                                                      romRegion,
                                                      s_ramRegion,
                                                      s_ramRegionByteOnly,
                                                      s_ramRegionWriteOnly,
                                                      s_inputRegion,
                                                      s_outputRegion,
                                                      s_IRQ_RESET_ADDR,
                                                      s_POKEY0_ADDR,
                                                      s_POKEY1_ADDR,
                                                      s_EAROM_WRITE_ADDR,
                                                      s_EAROM_CONTROL_ADDR,
                                                      s_EAROM_READ_ADDR,
                                                      s_CONF_ADDRESS,
                                                      s_CONF_MASK,
                                                      s_CONF_ACTIVE_LOW
                                                      )
{
}

CSpaceDuelGame::~CSpaceDuelGame(
)
{
}
