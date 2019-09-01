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

#include "CMillipedeGame.h"

static const UINT32 s_ROM_2732_SIZE = 0x1000; // 4KB (4096 bytes)

// 2n test of ROMS                        01   02   04   08   10   20   40   80   100  200  400 800

// version 1 - MAME 'milliped'
//   program ROMs
static const UINT16 s_romData_1_H1[]  = {0x32,0x20,0x49,0x53,0xcf,0x49,0x52,0x45,0x49,0x4f,0x74,0x4a}; // 136013-101.h1  - 0x46752c7d
static const UINT16 s_romData_1_JK1[] = {0x10,0xdc,0x30,0x00,0x02,0x20,0x94,0x4c,0x21,0x90,0x0c,0x12}; // 136013-102.jk1 - 0x62e137e0
static const UINT16 s_romData_1_L1[]  = {0x38,0x60,0x3b,0x0a,0x39,0x9d,0xb5,0xcf,0x00,0xb5,0x12,0x01}; // 136013-103.l1  - 0xfb01baf2
static const UINT16 s_romData_1_MN1[] = {0x29,0x79,0x20,0x4f,0x20,0x00,0x20,0x4b,0xf5,0xa7,0xbb,0x20}; // 136013-104.mn1 - 0x40711675
//   graphics ROMs (not accessible by CPU, so these are not actually used)
// static const UINT16 s_romData_1_P5[] = {0x00,0x00,0x24,0x3e,0x00,0x00,0x00,0x40,0x00,0x00,0x00}; // 136013-106.p5 - 0xf4468045
// static const UINT16 s_romData_1_R5[] = {0x00,0x00,0x24,0x7f,0x00,0x00,0x00,0x30,0x00,0x00,0x00}; // 136013-107.r5 - 0x68c3437a

//
// program ROM addresses
//
static const UINT32 s_ROM_ADDR_H1  = 0x4000;
static const UINT32 s_ROM_ADDR_JK1 = 0x5000;
static const UINT32 s_ROM_ADDR_L1  = 0x6000;
static const UINT32 s_ROM_ADDR_MN1 = 0x7000;

//
// graphics ROM addresses
//
//   not addressable by the CPU

//
// hardware addresses
//
static const UINT32 s_IRQ_RESET_ADDR = 0x2600;
static const UINT32 s_EAROM_WRITE_ADDR = 0x2780;
static const UINT32 s_EAROM_CONTROL_ADDR = 0x2700;
static const UINT32 s_EAROM_READ_ADDR = 0x2030;
static const UINT32 s_P1START_ADDRESS = 0x2000;  // address for P1START to confirm destructive EAROM operations
static const UINT32 s_P1START_MASK = CCentipedeBaseGame::s_MSK_D5; // bitmask for P1START
static const UINT32 s_POKEY_0_ADDR = 0x0400;
static const UINT32 s_POKEY_1_ADDR = 0x0800;

//
// ROM regions
//
// Version 1 ROM config
static const ROM_REGION s_romRegionSet1[] PROGMEM = {
    {NO_BANK_SWITCH, s_ROM_ADDR_H1,  s_ROM_2732_SIZE, s_romData_1_H1,  0x46752c7d, "H1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_JK1, s_ROM_2732_SIZE, s_romData_1_JK1, 0x62e137e0, "JK1"},
    {NO_BANK_SWITCH, s_ROM_ADDR_L1,  s_ROM_2732_SIZE, s_romData_1_L1,  0xfb01baf2, "L1 "},
    {NO_BANK_SWITCH, s_ROM_ADDR_MN1, s_ROM_2732_SIZE, s_romData_1_MN1, 0x40711675, "MN1"},
    {0} }; // end of list

//
// RAM regions
//
static const RAM_REGION s_ramRegion[] PROGMEM = {
                                                  //    "012", "012345"
    {NO_BANK_SWITCH, 0x0000,      0x03FF,      1, 0x0F, "2F ", "PrgLwr"}, // Program RAM, 2114 (1024 x 4), lower
    {NO_BANK_SWITCH, 0x0000,      0x03FF,      1, 0xF0, "2E ", "PrgUpr"}, // Program RAM, 2114 (1024 x 4), upper
    /*
    // reads/writes of video RAM at 0x1000-0x13ff must be synchronous with phi0 clock; not currently supported
    {NO_BANK_SWITCH, 0x1000,      0x10FF,      1, 0x0F, "2M ", "Video "}, // Video RAM, 2101 256x4 - lower
    {NO_BANK_SWITCH, 0x1100,      0x11FF,      1, 0x0F, "2L ", "Video "}, // Video RAM, 2101 256x4 - lower
    {NO_BANK_SWITCH, 0x1200,      0x12FF,      1, 0x0F, "2K ", "Video "}, // Video RAM, 2101 256x4 - lower
    {NO_BANK_SWITCH, 0x1300,      0x13FF,      1, 0x0F, "2J ", "Video "}, // Video RAM, 2101 256x4 - lower
    {NO_BANK_SWITCH, 0x1000,      0x10FF,      1, 0xF0, "4N ", "Video "}, // Video RAM, 2101 256x4 - upper
    {NO_BANK_SWITCH, 0x1100,      0x11FF,      1, 0xF0, "4M ", "Video "}, // Video RAM, 2101 256x4 - upper
    {NO_BANK_SWITCH, 0x1200,      0x12FF,      1, 0xF0, "4L ", "Video "}, // Video RAM, 2101 256x4 - upper
    {NO_BANK_SWITCH, 0x1300,      0x13FF,      1, 0xF0, "4K ", "Video "}, // Video RAM, 2101 256x4 - upper
    */
    {0} // end of list
};

//
// RAM regions for the byte-wide intensive random access memory test
//
static const RAM_REGION s_ramRegionByteOnly[] PROGMEM = {
                                                  //    "012", "012345"
    {NO_BANK_SWITCH, 0x0000,      0x03FF,      1, 0xFF, "2FE", "Progrm"}, // Program RAM 2F/2E
    /*
    // reads/writes of video RAM at 0x0400-0x07ff must be synchronous with phi0 clock; not currently supported
    {NO_BANK_SWITCH, 0x1000,      0x10FF,      1, 0xFF, "M N", "Video "}, // Video RAM 2M/4N
    {NO_BANK_SWITCH, 0x1100,      0x11FF,      1, 0xFF, "L M", "Video "}, // Video RAM 2L/4M
    {NO_BANK_SWITCH, 0x1200,      0x12FF,      1, 0xFF, "K L", "Video "}, // Video RAM 2K/4L
    {NO_BANK_SWITCH, 0x1300,      0x13FF,      1, 0xFF, "J K", "Video "}, // Video RAM 2J/4K
    */
    {0} // end of list
};

//
// Write-only RAM
//
static const RAM_REGION s_ramRegionWriteOnly[] PROGMEM = {
    //                                                 "012", "012345"
    // color pallette RAM; may need to be synchronous with phi0 clock to actually work
    // TO-DO: verify that the RAM physical locations are correct
    {NO_BANK_SWITCH, 0x2480,      0x248F,     1, 0x0F, "11C", "StpClr"}, // Stamp Color RAM lower, 82S25 (16x4 bipolar)
    {NO_BANK_SWITCH, 0x2480,      0x248F,     1, 0xF0, "11B", "StpClr"}, // Stamp Color RAM upper, 82S25 (16x4 bipolar)
    {NO_BANK_SWITCH, 0x2490,      0x249F,     1, 0x0F, "10A", "MobClr"}, // Motion Object Color RAM lower, 82S25 (16x4 bipolar)
    {NO_BANK_SWITCH, 0x2490,      0x249F,     1, 0xF0, "10B", "MobClr"}, // Motion Object Color RAM upper, 82S25 (16x4 bipolar)
    {0} // end of list
};

//
// Hardware Inputs
//
static const INPUT_REGION s_inputRegion[] PROGMEM = {
    //                                                        "012", "012345"
    {NO_BANK_SWITCH, 0x0408,  CCentipedeBaseGame::s_MSK_ALL,  "4D ", "OPTSW0"}, // option switch block 1, on POKEY 0 (not directly readable?)
    {NO_BANK_SWITCH, 0x0808,  CCentipedeBaseGame::s_MSK_ALL,  "4B ", "OPTSW1"}, // option switch block 2, on POKEY 1 (not directly readable?)

    {NO_BANK_SWITCH, 0x2010,  CCentipedeBaseGame::s_MSK_D7,   "10K", "COIN R"}, // 10K pin 2, bit 7
    {NO_BANK_SWITCH, 0x2010,  CCentipedeBaseGame::s_MSK_D6,   "10K", "COIN L"}, // 10K pin 5, bit 6
    {NO_BANK_SWITCH, 0x2010,  CCentipedeBaseGame::s_MSK_D5,   "10K", "COINAX"}, // 10K pin 11, bit 5
    {NO_BANK_SWITCH, 0x2010,  CCentipedeBaseGame::s_MSK_D4,   "10K", "SLAM  "},  // 10K pin 14 bit 4
    {NO_BANK_SWITCH, 0x2010,  CCentipedeBaseGame::s_MSK_D0_TO_D3, "10K", "JYSTKS"}, // read 10K bits 0-3 as joystick inputs (two, selected by CTLSEL)
    {NO_BANK_SWITCH, 0x2011,  CCentipedeBaseGame::s_MSK_D7,   "10K", "SlfTst"}, // Self-test switch, 10K pin 3, bit 7
    {NO_BANK_SWITCH, 0x2011,  CCentipedeBaseGame::s_MSK_D5,   "10K", "CkTail"}, // Cocktail cabinet harness, 10K pin 10, bit 5
    {NO_BANK_SWITCH, 0x2011,  CCentipedeBaseGame::s_MSK_D4,   "10K", "SPI?  "}, // Unknown (Pin 15 on main edge connector), 10K pin 13, bit 4

    {NO_BANK_SWITCH, 0x2000,  CCentipedeBaseGame::s_MSK_D4,   "10L", "FIRE1 "}, // 10L pin 14, bit 4
    {NO_BANK_SWITCH, 0x2001,  CCentipedeBaseGame::s_MSK_D4,   "10L", "FIRE2 "}, // 10L pin 13, bit 4
    {NO_BANK_SWITCH, 0x2000,  CCentipedeBaseGame::s_MSK_D5,   "10L", "START1"}, // 10L pin 2, bit 5
    {NO_BANK_SWITCH, 0x2001,  CCentipedeBaseGame::s_MSK_D5,   "10L", "START2"}, // 10L pin 3, bit 5
    {NO_BANK_SWITCH, 0x2000,  CCentipedeBaseGame::s_MSK_D6,   "10L", "VBlank"}, // Vertical blank, 10L pin 11, bit 6

    // TO-DO custom test to option switches at same address (switched by TBEN 0x2505 D7)
    {NO_BANK_SWITCH, 0x2000,  CCentipedeBaseGame::s_MSK_TRAK, "9P ", "TRAK-H"}, // Trak ball horizontal count and direction (9P bits 0-3 and 10L bit 7)
    {NO_BANK_SWITCH, 0x2001,  CCentipedeBaseGame::s_MSK_TRAK, "8N ", "TRAK-V"}, // Trak ball vertical count and direction (8N bits 0-3 and 10L bit 7)
    {0} // end of list
};

//
// Hardware Outputs
//
static const OUTPUT_REGION s_outputRegion[] PROGMEM = { //                                 "012", "012345"
    {NO_BANK_SWITCH, 0x2501, CCentipedeBaseGame::s_MSK_D7,  CCentipedeBaseGame::s_ACT_Hi,  "12E", "CNCTLT"}, // 12E pin 5,  LEFT Coin counter
    {NO_BANK_SWITCH, 0x2502, CCentipedeBaseGame::s_MSK_D7,  CCentipedeBaseGame::s_ACT_Hi,  "12E", "CNCTRT"}, // 12E pin 6,  RIGHT Coin counter
    {NO_BANK_SWITCH, 0x2503, CCentipedeBaseGame::s_MSK_D7,  CCentipedeBaseGame::s_ACT_Lo,  "12E", "P1 LED"}, // 12E pin 7,  Player 1 start LED
    {NO_BANK_SWITCH, 0x2504, CCentipedeBaseGame::s_MSK_D7,  CCentipedeBaseGame::s_ACT_Lo,  "12E", "P2 LED"}, // 12E pin 9,  Player 2 start LED
    {NO_BANK_SWITCH, 0x2505, CCentipedeBaseGame::s_MSK_D7,  CCentipedeBaseGame::s_ACT_Lo,  "12E", "TBEN  "}, // 12E pin 10, Trak ball enable
    {NO_BANK_SWITCH, 0x2506, CCentipedeBaseGame::s_MSK_D7,  CCentipedeBaseGame::s_ACT_Lo,  "12E", "VIDROT"}, // 12E pin 11, Video rotate
    {NO_BANK_SWITCH, 0x2507, CCentipedeBaseGame::s_MSK_D7,  CCentipedeBaseGame::s_ACT_Lo,  "12E", "CTLSEL"}, // 12E pin 12, Control select

    {NO_BANK_SWITCH, 0x2600, CCentipedeBaseGame::s_MSK_ALL, CCentipedeBaseGame::s_ACT_Lo,  "3F ", "IRQRST"}, // 3F pin 5, IRQ reset
    {NO_BANK_SWITCH, 0x2680, CCentipedeBaseGame::s_MSK_ALL, CCentipedeBaseGame::s_ACT_Lo,  "3F ", "WD RST"}, // 3F pin 6, Clear watchdog timer
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
CMillipedeGame::createInstanceSet1(
)
{
    return (new CMillipedeGame(false, s_romRegionSet1));
}

IGame*
CMillipedeGame::createInstanceClockMasterSet1(
)
{
    return (new CMillipedeGame(true, s_romRegionSet1));
}

CMillipedeGame::CMillipedeGame(
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
