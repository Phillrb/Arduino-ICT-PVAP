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

#include "CCentipedeGame.h"

static const UINT32 s_ROM_2716_SIZE = 0x0800; //2KB - 2048 bytes - 0x800 in hex

// 2n test of ROMS                        01   02   04   08   10   20   40   80   100  200  400

// version 1 - MAME 'centiped1'
//   program ROMs
static const UINT16 s_romData_1_D1[]  = {0x06,0x3b,0x31,0x20,0x73,0x20,0x20,0xa0,0xe9,0xb5,0x4c}; // 136001-107.d1  - 0x674ab844
static const UINT16 s_romData_1_E1[]  = {0xa6,0x8e,0xf4,0x01,0x2e,0x8d,0x00,0x85,0x00,0x6c,0x64}; // 136001-108.e1  - 0x47ceddc9
static const UINT16 s_romData_1_FH1[] = {0x30,0xa9,0x85,0xb5,0xe6,0x95,0x13,0x19,0xd0,0x01,0xa5}; // 136001-109.fh1 - 0x7ee80952
static const UINT16 s_romData_1_J1[]  = {0x04,0x84,0xa4,0x29,0x10,0x8b,0xa9,0x10,0x95,0xc0,0xca}; // 136001-110.j1  - 0x93b33dce
//   graphics ROMs (not accessible by CPU, so these are not actually used)
// static const UINT16 s_romData_1_F7[]  = {0x00,0x00,0x3c,0x7e,0x00,0x00,0x00,0x00,0x22,0x00,0x00}; // 136001-111.f7 -  0x880acfb9
// static const UINT16 s_romData_1_HJ7[] = {0x00,0x00,0x3c,0x7e,0x00,0x00,0x00,0x00,0x22,0x00,0x00}; // 136001-112.hj7 - 0xb1397029

// version 2 - MAME 'centiped2'
//   program ROMs
static const UINT16 s_romData_2_D1[]  = {0x0d,0x3b,0x31,0x20,0x73,0x20,0x20,0xa0,0xe9,0xb5,0x4c}; // 136001-207.d1  - 0xb2909e2f
static const UINT16 s_romData_2_E1[]  = {0xa6,0x8e,0xf4,0x01,0x2e,0x8d,0x00,0x85,0x00,0x6c,0x64}; // 136001-208.e1  - 0x110e04ff
static const UINT16 s_romData_2_FH1[] = {0x30,0xa9,0x85,0xb5,0xe6,0x95,0x13,0x19,0xd0,0x01,0xd3}; // 136001-209.fh1 - 0xcc2edb26
static const UINT16 s_romData_2_J1[]  = {0x91,0xc8,0x93,0x04,0x3f,0x2f,0x02,0xd2,0x03,0xb0,0x50}; // 136001-210.j1  - 0x93999153
//   graphics ROMs
//     same as version 1

// version 3 - MAME 'centiped3'
//   program ROMs
static const UINT16 s_romData_3_D1[]  = {0x04,0x3b,0x31,0x20,0x28,0x20,0x20,0xa0,0xe9,0xb5,0x4c}; // 136001-307.d1  - 0x5ab0d9de
static const UINT16 s_romData_3_E1[]  = {0x8e,0xa9,0x85,0x95,0xa9,0xa9,0x29,0x88,0xa5,0x2c,0x00}; // 136001-308.e1  - 0x4c07fd3e
static const UINT16 s_romData_3_FH1[] = {0xa9,0x00,0x8b,0x34,0x8b,0x35,0x85,0xa4,0x24,0x84,0xca}; // 136001-309.fh1 - 0xff69b424
static const UINT16 s_romData_3_J1[]  = {0x84,0x8b,0x8b,0x3f,0x02,0xb1,0x20,0xa9,0xc2,0xf0,0xfb}; // 136001-310.j1  - 0x44e40fa4
//   graphics ROMs
//     same as version 1

// version 4 - MAME 'centiped'
//   program ROMs
static const UINT16 s_romData_4_D1[]  = {0x4b,0x3b,0x31,0x20,0x28,0x20,0x20,0xa0,0xe9,0xb5,0xa9}; // 136001-407.d1  - 0xc4d995eb
static const UINT16 s_romData_4_E1[]  = {0x60,0xe6,0xa6,0x85,0xf0,0x8d,0xf0,0x9c,0x86,0x90,0x45}; // 136001-408.e1  - 0xbcdebe1b
static const UINT16 s_romData_4_FH1[] = {0x61,0x90,0xa9,0x90,0xb0,0xe6,0x4f,0x01,0xa5,0x84,0x90}; // 136001-409.fh1 - 0x66d7b04a
static const UINT16 s_romData_4_J1[]  = {0xcf,0x06,0x20,0x58,0xcd,0x4e,0xef,0x01,0x13,0x38,0xa9}; // 136001-410.j1  - 0x33ce4640
//   graphics ROMs
//     same as version 1

//
// program ROM addresses
//
static const UINT32 s_ROM_ADDR_D1  = 0x2000;
static const UINT32 s_ROM_ADDR_E1  = 0x2800;
static const UINT32 s_ROM_ADDR_FH1 = 0x3000;
static const UINT32 s_ROM_ADDR_J1  = 0x3800;
//
// graphics ROM addresses
//
//   not addressable by the CPU

//
// hardware addresses
//
static const UINT32 s_IRQ_RESET_ADDR = 0x1800;
static const UINT32 s_EAROM_WRITE_ADDR = 0x1600;
static const UINT32 s_EAROM_CONTROL_ADDR = 0x1680;
static const UINT32 s_EAROM_READ_ADDR = 0x1700;
static const UINT32 s_P1START_ADDRESS = 0x0c01;  // address for P1START to confirm destructive EAROM operations
static const UINT32 s_P1START_MASK = CCentipedeBaseGame::s_MSK_D0; // bitmask for P1START

//
// ROM regions
//
// Version 1 ROM config
static const ROM_REGION s_romRegionSet1[] PROGMEM = {
    {NO_BANK_SWITCH, s_ROM_ADDR_D1,  s_ROM_2716_SIZE, s_romData_1_D1,  0x674ab844, "D1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_E1,  s_ROM_2716_SIZE, s_romData_1_E1,  0x47ceddc9, "E1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_FH1, s_ROM_2716_SIZE, s_romData_1_FH1, 0x7ee80952, "FH1"},
    {NO_BANK_SWITCH, s_ROM_ADDR_J1,  s_ROM_2716_SIZE, s_romData_1_J1,  0x93b33dce, "J1 "},
    {0} }; // end of list

// Version 2 ROM config
static const ROM_REGION s_romRegionSet2[] PROGMEM = {
    {NO_BANK_SWITCH, s_ROM_ADDR_D1,  s_ROM_2716_SIZE, s_romData_2_D1,  0xb2909e2f, "D1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_E1,  s_ROM_2716_SIZE, s_romData_2_E1,  0x110e04ff, "E1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_FH1, s_ROM_2716_SIZE, s_romData_2_FH1, 0xcc2edb26, "FH1"},
    {NO_BANK_SWITCH, s_ROM_ADDR_J1,  s_ROM_2716_SIZE, s_romData_2_J1,  0x93999153, "J1 "},
    {0} }; // end of list

// Version 3 ROM config
static const ROM_REGION s_romRegionSet3[] PROGMEM = {
    {NO_BANK_SWITCH, s_ROM_ADDR_D1,  s_ROM_2716_SIZE, s_romData_3_D1,  0x5ab0d9de, "D1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_E1,  s_ROM_2716_SIZE, s_romData_3_E1,  0x4c07fd3e, "E1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_FH1, s_ROM_2716_SIZE, s_romData_3_FH1, 0xff69b424, "FH1"},
    {NO_BANK_SWITCH, s_ROM_ADDR_J1,  s_ROM_2716_SIZE, s_romData_3_J1,  0x44e40fa4, "J1 "},
    {0} }; // end of list

// Version 4 ROM config
static const ROM_REGION s_romRegionSet4[] PROGMEM = {
    {NO_BANK_SWITCH, s_ROM_ADDR_D1,  s_ROM_2716_SIZE, s_romData_4_D1,  0xc4d995eb, "D1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_E1,  s_ROM_2716_SIZE, s_romData_4_E1,  0xbcdebe1b, "E1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_FH1, s_ROM_2716_SIZE, s_romData_4_FH1, 0x66d7b04a, "FH1"},
    {NO_BANK_SWITCH, s_ROM_ADDR_J1,  s_ROM_2716_SIZE, s_romData_4_J1,  0x33ce4640, "J1 "},
    {0} }; // end of list

//
// RAM regions
//
static const RAM_REGION s_ramRegion[] PROGMEM = {
                                                  //    "012", "012345"
    {NO_BANK_SWITCH, 0x0000,      0x03FF,      1, 0x0F, "H2 ", "PrgLwr"}, // Program RAM H2, 2114 (1024 x 4), lower
    {NO_BANK_SWITCH, 0x0000,      0x03FF,      1, 0xF0, "F2 ", "PrgUpr"}, // Program RAM F2, 2114 (1024 x 4), upper
    /*
    // reads/writes of video RAM at 0x0400-0x07ff must be synchronous with phi0 clock; not currently supported
    {NO_BANK_SWITCH, 0x0400,      0x04FF,      1, 0x0F, "K7 ", "Video "}, // Video RAM, 2101 256x4 - lower
    {NO_BANK_SWITCH, 0x0500,      0x05FF,      1, 0x0F, "L7 ", "Video "}, // Video RAM, 2101 256x4 - lower
    {NO_BANK_SWITCH, 0x0600,      0x06FF,      1, 0x0F, "M7 ", "Video "}, // Video RAM, 2101 256x4 - lower
    {NO_BANK_SWITCH, 0x0700,      0x07FF,      1, 0x0F, "N7 ", "Video "}, // Video RAM, 2101 256x4 - lower
    {NO_BANK_SWITCH, 0x0400,      0x04FF,      1, 0xF0, "K5 ", "Video "}, // Video RAM, 2101 256x4 - upper
    {NO_BANK_SWITCH, 0x0500,      0x05FF,      1, 0xF0, "L5 ", "Video "}, // Video RAM, 2101 256x4 - upper
    {NO_BANK_SWITCH, 0x0600,      0x06FF,      1, 0xF0, "M5 ", "Video "}, // Video RAM, 2101 256x4 - upper
    {NO_BANK_SWITCH, 0x0700,      0x07FF,      1, 0xF0, "N5 ", "Video "}, // Video RAM, 2101 256x4 - upper
    */
    {0} // end of list
};

//
// RAM regions for the byte-wide intensive random access memory test
//
static const RAM_REGION s_ramRegionByteOnly[] PROGMEM = {
                                                  //    "012", "012345"
    {NO_BANK_SWITCH, 0x0000,      0x03FF,      1, 0xFF, "2HF", "Progrm"}, // Program RAM H2/F2
    /*
    // reads/writes of video RAM at 0x0400-0x07ff must be synchronous with phi0 clock; not currently supported
    {NO_BANK_SWITCH, 0x0400,      0x04FF,      1, 0xFF, "K75", "Video "}, // Video RAM K7/K5
    {NO_BANK_SWITCH, 0x0500,      0x05FF,      1, 0xFF, "L75", "Video "}, // Video RAM L7/L5
    {NO_BANK_SWITCH, 0x0600,      0x06FF,      1, 0xFF, "M75", "Video "}, // Video RAM M7/M5
    {NO_BANK_SWITCH, 0x0700,      0x07FF,      1, 0xFF, "N75", "Video "}, // Video RAM N7/N5
    */
    {0} // end of list
};

//
// Write-only RAM
//
static const RAM_REGION s_ramRegionWriteOnly[] PROGMEM = {
    //                                                 "012", "012345"
    // color pallette RAM; may need to be synchronous with phi0 clock to actually work
    {NO_BANK_SWITCH, 0x1400,      0x140F,     1, 0x0F, "C8 ", "Color "}, // Color RAM, 82S25 16x4 bipolar
    {0} // end of list
};

//
// Hardware Inputs
//
static const INPUT_REGION s_inputRegion[] PROGMEM = {
    //                                                        "012", "012345"
    {NO_BANK_SWITCH, 0x0800,  CCentipedeBaseGame::s_MSK_ALL,  "M89", "OPTSW1"}, // option switch block 1, M8/M9 (8 bits)
    {NO_BANK_SWITCH, 0x0801,  CCentipedeBaseGame::s_MSK_ALL,  "M89", "OPTSW2"}, // option switch block 2, M8/M9 (8 bits)

    {NO_BANK_SWITCH, 0x0c01,  CCentipedeBaseGame::s_MSK_D7,   "K9 ", "COIN R"}, // K9 pin 10, bit 7
    {NO_BANK_SWITCH, 0x0c01,  CCentipedeBaseGame::s_MSK_D6,   "K9 ", "COIN C"}, // K9 pin 6,  bit 6
    {NO_BANK_SWITCH, 0x0c01,  CCentipedeBaseGame::s_MSK_D5,   "K9 ", "COIN L"}, // K9 pin 13, bit 5
    {NO_BANK_SWITCH, 0x0c01,  CCentipedeBaseGame::s_MSK_D4,   "K9 ", "SLAM "},  // K9 pin 3,  bit 4
    {NO_BANK_SWITCH, 0x0c01,  CCentipedeBaseGame::s_MSK_D3,   "L9 ", "FIRE2 "}, // L9 pin 10, bit 3
    {NO_BANK_SWITCH, 0x0c01,  CCentipedeBaseGame::s_MSK_D2,   "L9 ", "FIRE1 "}, // L9 pin 6,  bit 2
    {NO_BANK_SWITCH, 0x0c01,  CCentipedeBaseGame::s_MSK_D1,   "L9 ", "START2"}, // L9 pin 13, bit 1
    {NO_BANK_SWITCH, 0x0c01,  CCentipedeBaseGame::s_MSK_D0,   "L9 ", "START1"}, // L9 pin 3,  bit 0

    {NO_BANK_SWITCH, 0x0c00,  CCentipedeBaseGame::s_MSK_TRAK, "L9 ", "TRAK-H"}, // Trak ball horizontal count and direction (L9 bits 0-3 and K9 bit 7)
    {NO_BANK_SWITCH, 0x0c02,  CCentipedeBaseGame::s_MSK_TRAK, "D9 ", "TRAK-V"}, // Trak ball vertical count and direction (D9 bits 0-3 and DE10 bit 7)

    {NO_BANK_SWITCH, 0x0c03,  CCentipedeBaseGame::s_MSK_ALL,  "D9 ", "JYSTKS"}, // read D9 bits 0-3 and DE10 bits 4-7 as joystick inputs

    {NO_BANK_SWITCH, 0x0c00,  CCentipedeBaseGame::s_MSK_D6,   "K9 ", "VBlank"}, // Vertical blank, K9 pin 5, bit 6
    {NO_BANK_SWITCH, 0x0c00,  CCentipedeBaseGame::s_MSK_D5,   "K9 ", "SlfTst"}, // Self-test switch, K9 pin 14, bit 5
    {NO_BANK_SWITCH, 0x0c00,  CCentipedeBaseGame::s_MSK_D4,   "K9 ", "CkTail"}, // Cocktail cabinet harness, K9 pin 2, bit 4
    {0} // end of list
};

//
// Hardware Outputs
//
static const OUTPUT_REGION s_outputRegion[] PROGMEM = { //                                 "012", "012345"
    {NO_BANK_SWITCH, 0x1c00, CCentipedeBaseGame::s_MSK_D7,  CCentipedeBaseGame::s_ACT_Hi,  "M10", "CNCTLT"}, // M10 pin 4,  LEFT Coin counter
    {NO_BANK_SWITCH, 0x1c01, CCentipedeBaseGame::s_MSK_D7,  CCentipedeBaseGame::s_ACT_Hi,  "M10", "CNCTCT"}, // M10 pin 5,  CENTER Coin counter
    {NO_BANK_SWITCH, 0x1c02, CCentipedeBaseGame::s_MSK_D7,  CCentipedeBaseGame::s_ACT_Hi,  "M10", "CNCTRT"}, // M10 pin 6,  RIGHT Coin counter
    {NO_BANK_SWITCH, 0x1c03, CCentipedeBaseGame::s_MSK_D7,  CCentipedeBaseGame::s_ACT_Lo,  "M10", "P1 LED"}, // M10 pin 7,  Player 1 start LED
    {NO_BANK_SWITCH, 0x1c04, CCentipedeBaseGame::s_MSK_D7,  CCentipedeBaseGame::s_ACT_Lo,  "M10", "P2 LED"}, // M10 pin 9,  Player 2 start LED
    // {NO_BANK_SWITCH, 0x1c05, CCentipedeBaseGame::s_MSK_D7,  CCentipedeBaseGame::s_ACT_Lo,  "M10", "LED 3 "}, // M10 pin 10, LED 3 (unused)
    // {NO_BANK_SWITCH, 0x1c06, CCentipedeBaseGame::s_MSK_D7,  CCentipedeBaseGame::s_ACT_Lo,  "M10", "LED 4 "}, // M10 pin 11, LED 4 (unused)
    {NO_BANK_SWITCH, 0x1c07, CCentipedeBaseGame::s_MSK_D7,  CCentipedeBaseGame::s_ACT_Lo,  "M10", "TrkFlp"}, // M10 pin 12, Trak ball flip control

    {NO_BANK_SWITCH, 0x1800, CCentipedeBaseGame::s_MSK_ALL, CCentipedeBaseGame::s_ACT_Lo,  "F3 ", "IRQAck"}, // F3 pin 11, IRQ Acknowledge
    {NO_BANK_SWITCH, 0x2000, CCentipedeBaseGame::s_MSK_ALL, CCentipedeBaseGame::s_ACT_Lo,  "F3 ", "WD Rst"}, // F3 pin 3,  Clear watchdog timer
    {NO_BANK_SWITCH, 0x2400, CCentipedeBaseGame::s_MSK_ALL, CCentipedeBaseGame::s_ACT_Lo,  "F3 ", "TrkClr"}, // F3 pin 6,  Clear trakball counters
    {0} // end of list
};

//
// Custom functions
//
static const CUSTOM_FUNCTION s_customFunction[] PROGMEM = {
    //                                    "0123456789"
//    {CCentipedeBaseGame::earomIdle,       "EAROM Idle"},
    {CCentipedeBaseGame::earomReadTest,   "EAROM Read"},
    {CCentipedeBaseGame::earomSerialDump, "EAROM Dump"},
    {CCentipedeBaseGame::earomSerialLoad, "EAROM Load"},
    {CCentipedeBaseGame::earomErase,      "EAROM Wipe"},
    {NO_CUSTOM_FUNCTION} // end of list
};


IGame*
CCentipedeGame::createInstanceSet1(
)
{
    return (new CCentipedeGame(false, s_romRegionSet1));
}

IGame*
CCentipedeGame::createInstanceClockMasterSet1(
)
{
    return (new CCentipedeGame(true, s_romRegionSet1));
}

IGame*
CCentipedeGame::createInstanceSet2(
)
{
    return (new CCentipedeGame(false, s_romRegionSet2));
}

IGame*
CCentipedeGame::createInstanceClockMasterSet2(
)
{
    return (new CCentipedeGame(true, s_romRegionSet2));
}

IGame*
CCentipedeGame::createInstanceSet3(
)
{
    return (new CCentipedeGame(false, s_romRegionSet3));
}

IGame*
CCentipedeGame::createInstanceClockMasterSet3(
)
{
    return (new CCentipedeGame(true, s_romRegionSet3));
}

IGame*
CCentipedeGame::createInstanceSet4(
)
{
    return (new CCentipedeGame(false, s_romRegionSet4));
}

IGame*
CCentipedeGame::createInstanceClockMasterSet4(
)
{
    return (new CCentipedeGame(true, s_romRegionSet4));
}

CCentipedeGame::CCentipedeGame(
               const bool       clockMaster,
               const ROM_REGION *romRegion
               ) : CCentipedeBaseGame( clockMaster,
                                       romRegion,
                                       s_ramRegion,
                                       s_ramRegionByteOnly,
                                       s_ramRegionWriteOnly,
                                       s_inputRegion,
                                       s_outputRegion,
                                       s_customFunction,
                                       s_IRQ_RESET_ADDR,
                                       s_EAROM_WRITE_ADDR,
                                       s_EAROM_CONTROL_ADDR,
                                       s_EAROM_READ_ADDR,
                                       s_P1START_ADDRESS,
                                       s_P1START_MASK
                                      )
{
}
