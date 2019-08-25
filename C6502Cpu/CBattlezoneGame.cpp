//
// Copyright (c) 2019, Warren Ondras
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
#include "CBattlezoneGame.h"

static const UINT32 s_ROM_2716_SIZE = 0x0800; //2KB - 2048 bytes - 0x800 in hex

// 2n test of ROMS                        01   02   04   08   10   20   40   80   100  200  40
// version 1 - MAME 'bzonea'
//  program ROMs
static const UINT16 s_romData_1_E1[]  = {0x39,0x90,0x20,0x28,0x03,0x7a,0xd0,0xa9,0x5a,0x61,0x2d}; // 036414-01.e1  - 0xefbc3fa0
static const UINT16 s_romData_1_H1[]  = {0x7a,0xa9,0xa2,0x7a,0xa7,0x39,0xa9,0x37,0xa0,0x40,0x90}; // 036413-01.h1  - 0x5d9d9111
static const UINT16 s_romData_1_J1[]  = {0xa9,0x02,0xcb,0xb5,0x18,0x20,0x0f,0xc1,0x90,0xa9,0xc4}; // 036412-01.j1  - 0xab55cbd2
static const UINT16 s_romData_1_K1[]  = {0xca,0xad,0x18,0x2a,0xa0,0x04,0x61,0x75,0xb9,0x0d,0x18}; // 036411-01.k1  - 0xad281297
static const UINT16 s_romData_1_LM1[] = {0x1e,0x00,0x1e,0x9e,0x1e,0x02,0x18,0x1e,0x6e,0xd8,0x1e}; // 036410-01.lm1 - 0x0b7bfaa4
static const UINT16 s_romData_1_N1[]  = {0x06,0x38,0xc0,0x38,0x60,0x70,0x80,0xad,0x01,0x95,0x29}; // 036409-01.n1  - 0x1e14e919
//  vector ROMs
static const UINT16 s_romData_1_A3[]  = {0x12,0x13,0x13,0x13,0x14,0x16,0x1a,0x20,0x00,0xfc,0xd4}; // 036421-01.a3  - 0x8ea8f939
static const UINT16 s_romData_1_BC3[] = {0x00,0x00,0x00,0x65,0xf9,0x28,0x20,0x0c,0x00,0xe0,0xed}; // 036422-01.bc3 - 0x7414177b

//  version 2 - MAME 'bzone' (only one ROM is different from v1)
static const UINT16 s_romData_2_E1[]  = {0x39,0x90,0x20,0x28,0x03,0x7a,0xd0,0xa9,0x5a,0x61,0x2d}; // 036414-02.e1  - 0x13de36d5

// program ROM addresses
static const UINT32 s_ROM_ADDR_E1  = 0x5000;
static const UINT32 s_ROM_ADDR_H1  = 0x5800;
static const UINT32 s_ROM_ADDR_J1  = 0x6000;
static const UINT32 s_ROM_ADDR_K1  = 0x6800;
static const UINT32 s_ROM_ADDR_LM1 = 0x7000;
static const UINT32 s_ROM_ADDR_N1  = 0x7800;

// vector ROM addresses
static const UINT32 s_ROM_ADDR_BC3 = 0x3000;
static const UINT32 s_ROM_ADDR_A3  = 0x3800;

// Version 2 ROM config
static const ROM_REGION s_romRegionSet2[] PROGMEM = {
    {NO_BANK_SWITCH, s_ROM_ADDR_E1,  s_ROM_2716_SIZE, s_romData_2_E1,  0x13de36d5, "E1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_H1,  s_ROM_2716_SIZE, s_romData_1_H1,  0x5d9d9111, "H1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_J1,  s_ROM_2716_SIZE, s_romData_1_J1,  0xab55cbd2, "J1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_K1,  s_ROM_2716_SIZE, s_romData_1_K1,  0xad281297, "K1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_LM1, s_ROM_2716_SIZE, s_romData_1_LM1, 0x0b7bfaa4, "LM1"},
    {NO_BANK_SWITCH, s_ROM_ADDR_N1,  s_ROM_2716_SIZE, s_romData_1_N1,  0x1e14e919, "N1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_A3,  s_ROM_2716_SIZE, s_romData_1_A3,  0x8ea8f939, "A3 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_BC3, s_ROM_2716_SIZE, s_romData_1_BC3, 0x7414177b, "BC3"},
    {0} }; // end of list

// Version 1 ROM config
static const ROM_REGION s_romRegionSet1[] PROGMEM = {
    {NO_BANK_SWITCH, s_ROM_ADDR_E1,  s_ROM_2716_SIZE, s_romData_1_E1,  0xefbc3fa0, "E1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_H1,  s_ROM_2716_SIZE, s_romData_1_H1,  0x5d9d9111, "H1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_J1,  s_ROM_2716_SIZE, s_romData_1_J1,  0xab55cbd2, "J1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_K1,  s_ROM_2716_SIZE, s_romData_1_K1,  0xad281297, "K1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_LM1, s_ROM_2716_SIZE, s_romData_1_LM1, 0x0b7bfaa4, "LM1"},
    {NO_BANK_SWITCH, s_ROM_ADDR_N1,  s_ROM_2716_SIZE, s_romData_1_N1,  0x1e14e919, "N1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_A3,  s_ROM_2716_SIZE, s_romData_1_A3,  0x8ea8f939, "A3 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_BC3, s_ROM_2716_SIZE, s_romData_1_BC3, 0x7414177b, "BC3"},
    {0} }; // end of list

//
// Input region is the same for all ROM versions.
//
static const INPUT_REGION s_inputRegion[] PROGMEM = {
    // 74LS244 at location R11, address 0x0800
    {NO_BANK_SWITCH, 0x0800,  CAtariAVGBaseGame::s_MSK_D0,  "R11", "RT  CN"}, // right coin, bit 0
    {NO_BANK_SWITCH, 0x0800,  CAtariAVGBaseGame::s_MSK_D1,  "R11", "CNT CN"}, // center coin, bit 1 (test point is "COIN L" on early boards)
    {NO_BANK_SWITCH, 0x0800,  CAtariAVGBaseGame::s_MSK_D2,  "R11", "LFT CN"}, // left coin, bit 2 (test point is "COIN C" on early boards)
    {NO_BANK_SWITCH, 0x0800,  CAtariAVGBaseGame::s_MSK_D3,  "R11", "SLAM  "}, // slam, bit 3
    {NO_BANK_SWITCH, 0x0800,  CAtariAVGBaseGame::s_MSK_D4,  "R11", "SLFTST"}, // self test, bit 4
    {NO_BANK_SWITCH, 0x0800,  CAtariAVGBaseGame::s_MSK_D5,  "R11", "DIASTP"}, // diag step, bit 5
    {NO_BANK_SWITCH, 0x0800,  CAtariAVGBaseGame::s_MSK_D6,  "R11", "HALT  "}, // vector HALT, bit 6
    {NO_BANK_SWITCH, 0x0800,  CAtariAVGBaseGame::s_MSK_D7,  "R11", "3KHZ  "}, // diag step, bit 7

    // 74LS244 at location N11, address 0x0A00
    {NO_BANK_SWITCH, 0x0a00,  CAtariAVGBaseGame::s_MSK_ALL, "N11", "DIPSW0"}, // option switch block 0 (8 bits)
    
    // 74LS244 at location P11, address 0x0C00
    {NO_BANK_SWITCH, 0x0c00,  CAtariAVGBaseGame::s_MSK_ALL, "P11", "DIPSW1"}, // option switch block 1 (8 bits)
    
    {0}
}; // end of list

//
// Output region is the same for all ROM versions.
//
static const OUTPUT_REGION s_outputRegion[] PROGMEM = { //                               "012", "012345"
    // 74LS174 latch at location M11, address 0x1000
    {NO_BANK_SWITCH, 0x1000, CAtariAVGBaseGame::s_MSK_D0,  CAtariAVGBaseGame::s_ACT_Hi,  "M11", "CNCTRT"}, // right coin counter, bit 0
    {NO_BANK_SWITCH, 0x1000, CAtariAVGBaseGame::s_MSK_D1,  CAtariAVGBaseGame::s_ACT_Hi,  "M11", "CNCTCT"}, // center coin counter, bit 1
    {NO_BANK_SWITCH, 0x1000, CAtariAVGBaseGame::s_MSK_D2,  CAtariAVGBaseGame::s_ACT_Hi,  "M11", "CNCTLT"}, // left coin counter, bit 2
    // bit 3 - unused
    // bit 4 - listed as VIDEO INVERT X
    // bit 5 - listed as VIDEO INVERT Y
    // bit 3 - unused

    // 74LS135 address decoder at location K5 (data bus agnostic)
    {NO_BANK_SWITCH, 0x1200, CAtariAVGBaseGame::s_MSK_ALL, CAtariAVGBaseGame::s_ACT_Lo,  "K5 ", "VG GO "}, // vec gen GO
    {NO_BANK_SWITCH, 0x1400, CAtariAVGBaseGame::s_MSK_ALL, CAtariAVGBaseGame::s_ACT_Lo,  "K5 ", "WD CLR"}, // watchdog clear
    {NO_BANK_SWITCH, 0x1600, CAtariAVGBaseGame::s_MSK_ALL, CAtariAVGBaseGame::s_ACT_Lo,  "K5 ", "VG RST"}, // vec gen RESET

    // 	74LS273 at aux M5 controls sound hardware and start LED
    //                                                             "012", "012345"
    {NO_BANK_SWITCH, 0x1840, 0x40, CBattlezoneGame::s_M5_INV_MASK, "xM5", "ST LED"},  // Start LED, bit 0 (active low)
    {NO_BANK_SWITCH, 0x1840, 0xA0, CBattlezoneGame::s_M5_INV_MASK, "xM5", "MTR   "},  // Motor + sound enable, bits 7 and 5
    {NO_BANK_SWITCH, 0x1840, 0xB0, CBattlezoneGame::s_M5_INV_MASK, "xM5", "MTR RV"},  // Motor + Motor Rev + sound enable, bits 7, 4 and 5
    {NO_BANK_SWITCH, 0x1840, 0x2C, CBattlezoneGame::s_M5_INV_MASK, "xM5", "SHELL "},  // Shell + Sh Loud + sound enable, bits 2, 3 and 5
    {NO_BANK_SWITCH, 0x1840, 0x24, CBattlezoneGame::s_M5_INV_MASK, "xM5", "SHELL-"},  // Shell + sound enable, bits 2 and 5
    {NO_BANK_SWITCH, 0x1840, 0x23, CBattlezoneGame::s_M5_INV_MASK, "xM5", "EXPL  "},  // Explosion + Exp Loud + sound enable, bits 0, 1 and 5
    {NO_BANK_SWITCH, 0x1840, 0x21, CBattlezoneGame::s_M5_INV_MASK, "xM5", "EXPL- "},  // Explosion + sound enable, bits 0 and 5
    {0}
}; // end of list

IGame*
CBattlezoneGame::createInstanceSet2(
)
{
    return (new CBattlezoneGame(false, s_romRegionSet2));
}

IGame*
CBattlezoneGame::createInstanceClockMasterSet2(
)
{
    return (new CBattlezoneGame(true, s_romRegionSet2));
}

IGame*
CBattlezoneGame::createInstanceSet1(
)
{
    return (new CBattlezoneGame(false, s_romRegionSet1));
}

IGame*
CBattlezoneGame::createInstanceClockMasterSet1(
)
{
    return (new CBattlezoneGame(true, s_romRegionSet1));
}

CBattlezoneGame::CBattlezoneGame(
   const bool       clockMaster,
   const ROM_REGION *romRegion
   ) : CAtariAVGBaseGame( clockMaster,
                          romRegion,
                          s_inputRegion,
                          s_outputRegion
                         )
{
}


