//
// Copyright (c) 2018, Phillip Riscombe-Burton
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
#include "CAsteroidsDeluxeGame.h"

static const UINT32 s_ROM_2716_SIZE = 0x0800; //2KB - 2048 bytes - 0x800 in hex

//2n test of ROMS                        01   02   04   08   10   20   40   80   100  200  400

// set 1
// program ROMs
static const UINT16 s_romData_1_D1[]  = {0xf5,0x7b,0xd4,0xd3,0xd4,0xa5,0x1f,0x03,0x60,0x69,0xce}; // 036430-01.d1  - 0x8f5dabc6
static const UINT16 s_romData_1_EF1[] = {0xc9,0x18,0x08,0x17,0x85,0x1e,0x8d,0xb0,0xa5,0x07,0xd0}; // 036431-01.ef1 - 0x157a8516
static const UINT16 s_romData_1_FH1[] = {0xa0,0x00,0x23,0xb9,0xf5,0xa5,0x55,0x11,0x71,0x30,0x9a}; // 036432-01.fh1 - 0xfdea913c
static const UINT16 s_romData_1_J1[]  = {0x85,0x04,0x02,0xc9,0x8a,0x20,0x00,0xd0,0x9a,0x69,0x03}; // 036433-01.j1  - 0xef09bac7
// vector ROMs
static const UINT16 s_romData_1_R2[]  = {0x00,0x02,0x3c,0xbd,0x05,0x62,0x10,0x86,0x13,0x05,0xc9}; // 036800-01.r2  - 0x3b597407
static const UINT16 s_romData_1_NP2[] = {0x76,0x84,0x9e,0x06,0xce,0x0b,0xa8,0xa3,0x80,0x00,0x40}; // 036799-01.np2 - 0x7D511572

// set 2 changes
static const UINT16 s_romData_2_J1[]  = {0x85,0x04,0x02,0xc9,0x8a,0x20,0x00,0xd0,0x9a,0x69,0x03}; // 036433-02.j1  - 0xd8db74e3

// set 3 changes
static const UINT16 s_romData_2_D1[]  = {0xe3,0x7b,0xcd,0xcc,0xcd,0xa5,0x1f,0xff,0x10,0xa5,0x5b}; //036430-02.d1 - 0xA4D7A525
static const UINT16 s_romData_2_EF1[] = {0x02,0x8d,0x02,0x8d,0xa5,0x20,0xfd,0x5b,0x29,0x29,0x9d}; //036431-02.ef1 - 0xD4004AAE
static const UINT16 s_romData_2_FH1[] = {0xca,0x10,0xa5,0xc5,0x90,0x00,0x11,0xa9,0x40,0x7e,0xb6}; //036432-02.fh1 - 0x6D720C41
static const UINT16 s_romData_3_J1[]  = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x25,0x00,0x56,0xfa}; //036433-03.j1 - 0x0DCC0BE6
static const UINT16 s_romData_2_R2[]  = {0x00,0x02,0x3c,0xbd,0x05,0x61,0x10,0x86,0x13,0xfb,0x56}; //036800-02.r2 - 0xBB8CABE1


//4800-4FFF - VECTOR ROM at R2   - Read-only
//5000-57FF - VECTOR ROM at N/P2 - Read-only

//6000-7FFF - PROG ROM - Read-only
//  ADDRESS     POS    REF   CODE     SIGNAL
//  6000-67FF - D1 -   d1 -  036430 - PROM0
//  6800-6FFF - E/F1 - ef1 - 036431 - PROM1
//  7000-77FF - F/H1 - fh1 - 036432 - PROM2
//  7800-7FFF - J1 -   j1 -  036433 - PROM3

static const UINT32 s_ROM_ADDR_R2 =  0x4800;
static const UINT32 s_ROM_ADDR_NP2 = 0x5000;
static const UINT32 s_ROM_ADDR_D1 =  0x6000;
static const UINT32 s_ROM_ADDR_EF1 = 0x6800;
static const UINT32 s_ROM_ADDR_FH1 = 0x7000;
static const UINT32 s_ROM_ADDR_J1 =  0x7800;

//
// Set 3 from MAME
//
static const ROM_REGION s_romRegionSet3[] PROGMEM = { //
    {NO_BANK_SWITCH, s_ROM_ADDR_R2,  s_ROM_2716_SIZE, s_romData_2_R2,  0xbb8cabe1, "R2 "}, //R2  -  v2
    {NO_BANK_SWITCH, s_ROM_ADDR_NP2, s_ROM_2716_SIZE, s_romData_1_NP2, 0x7d511572, "NP2"}, //NP2 -  v1
    {NO_BANK_SWITCH, s_ROM_ADDR_D1,  s_ROM_2716_SIZE, s_romData_2_D1,  0xa4d7a525, "D1 "}, //D1  -  v2
    {NO_BANK_SWITCH, s_ROM_ADDR_EF1, s_ROM_2716_SIZE, s_romData_2_EF1, 0xd4004aae, "EF1"}, //EF1 -  v2
    {NO_BANK_SWITCH, s_ROM_ADDR_FH1, s_ROM_2716_SIZE, s_romData_2_FH1, 0x6d720c41, "FH1"}, //FH1 -  v2
    {NO_BANK_SWITCH, s_ROM_ADDR_J1,  s_ROM_2716_SIZE, s_romData_3_J1,  0x0dcc0be6, "J1 "}, //J1  -  v3
    {0} }; // end of list

//
// Set 2 from MAME
//
static const ROM_REGION s_romRegionSet2[] PROGMEM = { //
    {NO_BANK_SWITCH, s_ROM_ADDR_R2,  s_ROM_2716_SIZE, s_romData_1_R2,  0x3b597407, "R2 "}, //R2  -  v1
    {NO_BANK_SWITCH, s_ROM_ADDR_NP2, s_ROM_2716_SIZE, s_romData_1_NP2, 0x7d511572, "NP2"}, //NP2 -  v1
    {NO_BANK_SWITCH, s_ROM_ADDR_D1,  s_ROM_2716_SIZE, s_romData_1_D1,  0x8f5dabc6, "D1 "}, //D1  -  v1
    {NO_BANK_SWITCH, s_ROM_ADDR_EF1, s_ROM_2716_SIZE, s_romData_1_EF1, 0x157a8516, "EF1"}, //EF1 -  v1
    {NO_BANK_SWITCH, s_ROM_ADDR_FH1, s_ROM_2716_SIZE, s_romData_1_FH1, 0xfdea913c, "FH1"}, //FH1 -  v1
    {NO_BANK_SWITCH, s_ROM_ADDR_J1,  s_ROM_2716_SIZE, s_romData_2_J1,  0xd8db74e3, "J1 "}, //J1  -  v2
    {0} }; // end of list

//
// Set 1 from MAME
//
static const ROM_REGION s_romRegionSet1[] PROGMEM = { //
    {NO_BANK_SWITCH, s_ROM_ADDR_R2,  s_ROM_2716_SIZE, s_romData_1_R2,  0x3b597407, "R2 "}, //R2  -  v1
    {NO_BANK_SWITCH, s_ROM_ADDR_NP2, s_ROM_2716_SIZE, s_romData_1_NP2, 0x7d511572, "NP2"}, //NP2 -  v1
    {NO_BANK_SWITCH, s_ROM_ADDR_D1,  s_ROM_2716_SIZE, s_romData_1_D1,  0x8f5dabc6, "D1 "}, //D1  -  v1
    {NO_BANK_SWITCH, s_ROM_ADDR_EF1, s_ROM_2716_SIZE, s_romData_1_EF1, 0x157a8516, "EF1"}, //EF1 -  v1
    {NO_BANK_SWITCH, s_ROM_ADDR_FH1, s_ROM_2716_SIZE, s_romData_1_FH1, 0xfdea913c, "FH1"}, //FH1 -  v1
    {NO_BANK_SWITCH, s_ROM_ADDR_J1,  s_ROM_2716_SIZE, s_romData_1_J1,  0xef09bac7, "J1 "}, //J1  -  v1
    {0} }; // end of list

ROM_REGION CAsteroidsDeluxeGame::getAstDlxRomset3(){ return *s_romRegionSet3; }
ROM_REGION CAsteroidsDeluxeGame::getAstDlxRomset2(){ return *s_romRegionSet2; }
ROM_REGION CAsteroidsDeluxeGame::getAstDlxRomset1(){ return *s_romRegionSet1; }

//
// RAM regions for Asteroids Deluxe are the same as Asteroids, but chip locations are different
//
static const RAM_REGION s_ramRegion[] PROGMEM = { //            "012", "012345"
            {NO_BANK_SWITCH, 0x0000,      0x00FF,      1, 0x0F, "L1A", "Prog. "}, // "Program RAM, 2114 - lower" 0 page
            {NO_BANK_SWITCH, 0x0100,      0x01FF,      1, 0x0F, "L1Z", "Prog. "}, // "Program RAM, 2114 - lower"
            {NO_BANK_SWITCH, 0x0200,      0x02FF,      1, 0x0F, "L1O", "Prog. "}, // "Program RAM, 2114 - lower"
            {NO_BANK_SWITCH, 0x0300,      0x03FF,      1, 0x0F, "L1T", "Prog. "}, // "Program RAM, 2114 - lower"

            {NO_BANK_SWITCH, 0x0000,      0x00FF,      1, 0xF0, "M1A", "Prog. "}, // "Program RAM, 2114 - upper" 0 page
            {NO_BANK_SWITCH, 0x0100,      0x01FF,      1, 0xF0, "M1Z", "Prog. "}, // "Program RAM, 2114 - upper" 
            {NO_BANK_SWITCH, 0x0200,      0x02FF,      1, 0xF0, "M1O", "Prog. "}, // "Program RAM, 2114 - upper" 
            {NO_BANK_SWITCH, 0x0300,      0x03FF,      1, 0xF0, "M1T", "Prog. "}, // "Program RAM, 2114 - upper" 

            {NO_BANK_SWITCH, 0x4000,      0x43FF,      1, 0x0F, "M3 ", "Vec.  "}, // "Vec RAM, 2114 - lower"
            {NO_BANK_SWITCH, 0x4000,      0x43FF,      1, 0xF0, "R3 ", "Vec.  "}, // "Vec RAM, 2114 - upper"
            {NO_BANK_SWITCH, 0x4400,      0x47FF,      1, 0x0F, "N3 ", "Vec.  "}, // "Vec RAM, 2114 - lower"
            {NO_BANK_SWITCH, 0x4400,      0x47FF,      1, 0xF0, "P3 ", "Vec.  "}, // "Vec RAM, 2114 - upper"
             {0}
}; // end of list

//
// RAM region 
// This is Used for the byte-wide intensive random access memory test.
//
static const RAM_REGION s_ramRegionByteOnly[] PROGMEM = { //                                                  "012", "012345"
                                                          {NO_BANK_SWITCH, 0x0000,      0x03FF,      1, 0xFF, "1LM", "Prog. "}, // "Program RAM, 2114, ROM PCB 2M/2F"
                                                          {NO_BANK_SWITCH, 0x4000,      0x43FF,      1, 0xFF, "3MR", "Vec   "}, // "Vec RAM"
                                                          {NO_BANK_SWITCH, 0x4400,      0x47FF,      1, 0xFF, "3NP", "Vec   "}, // "Vec RAM"
                                                          {0}
                                                        }; // end of list

//
// No write-only RAM on this platform. Yay!
//
static const RAM_REGION s_ramRegionWriteOnly[] PROGMEM = { {0} }; // end of list


//INPUT
//2001 D7 3KHz - R
//2002 D7 HALT - R
//2003 D7 Hyperspace SW - R
//2004 D7 Fire - R
//2005 D7 Diag step - R
//2006 D7 Slam sw - R
//2007 D7 Self test sw - R

//2400 D7 Left coin SW - R
//2401 D7 Center coin SW - R
//2402 D7 Right coin SW - R
//2403 D7 1 Player start - R
//2404 D7 2 Player start - R
//2405 D7 Thrust - R
//2406 D7 ROT right - R
//2407 D7 ROT left - R

//2800 - 2803 D0,D1 - R OPT switches SW1 -> SW8 - R

//
// Input region is the same for all ROM versions.
//
static const INPUT_REGION s_inputRegion[] PROGMEM = { //     "012", "012345"
    // 0x2000 A0-A3 POKEY
    // 0x2c40 A0-A5 EAROM READ


    // CLOCK/CONTROL INPUTS
    {NO_BANK_SWITCH, 0x2001,  CAsteroidsBaseGame::s_MSK_D7,  "L10", "3KHZ  "}, // L10 - pin 3
    {NO_BANK_SWITCH, 0x2002,  CAsteroidsBaseGame::s_MSK_D7,  "L10", "HALT  "}, // L10 - pin 2
    
    // PLAYER INPUTS
    {NO_BANK_SWITCH, 0x2003,  CAsteroidsBaseGame::s_MSK_D7,  "L10", "SHIELD"}, // L10 - pin 1
    {NO_BANK_SWITCH, 0x2004,  CAsteroidsBaseGame::s_MSK_D7,  "L10", "FIRE  "}, // L10 - pin 15
    
    // TEST INPUTS
    {NO_BANK_SWITCH, 0x2005,  CAsteroidsBaseGame::s_MSK_D7,  "L10", "DIASTP"}, // L10 - pin 14
    {NO_BANK_SWITCH, 0x2006,  CAsteroidsBaseGame::s_MSK_D7,  "L10", "SLAM  "}, // L10 - pin 13
    {NO_BANK_SWITCH, 0x2007,  CAsteroidsBaseGame::s_MSK_D7,  "L10", "SLFTST"}, // L10 - pin 12
    
    // COIN INPUTS
    {NO_BANK_SWITCH, 0x2400,  CAsteroidsBaseGame::s_MSK_D7,  "J10", "COIN L"}, // J10 - pin 4
    {NO_BANK_SWITCH, 0x2401,  CAsteroidsBaseGame::s_MSK_D7,  "J10", "COIN C"}, // J10 - pin 3
    {NO_BANK_SWITCH, 0x2402,  CAsteroidsBaseGame::s_MSK_D7,  "J10", "COIN R"}, // J10 - pin 2
    
    // MORE PLAYER INPUTS
    {NO_BANK_SWITCH, 0x2403,  CAsteroidsBaseGame::s_MSK_D7,  "J10", "1PLYST"}, // J10 - pin 1
    {NO_BANK_SWITCH, 0x2404,  CAsteroidsBaseGame::s_MSK_D7,  "J10", "2PLYST"}, // J10 - pin 15
    {NO_BANK_SWITCH, 0x2405,  CAsteroidsBaseGame::s_MSK_D7,  "J10", "THRUST"}, // J10 - pin 14
    {NO_BANK_SWITCH, 0x2406,  CAsteroidsBaseGame::s_MSK_D7,  "J10", "ROT R "}, // J10 - pin 13
    {NO_BANK_SWITCH, 0x2407,  CAsteroidsBaseGame::s_MSK_D7,  "J10", "ROT L "}, // J10 - pin 12
    
    // DIP SWITCH INPUTS (Combined)
    {NO_BANK_SWITCH, 0x2800,  CAsteroidsBaseGame::s_MSK_ALL, "P5 ", "SW ALL"}, // All option switches as a byte

    // DIP SWITCH INPUTS (Individual)
    // {NO_BANK_SWITCH, 0x2800,  CAsteroidsBaseGame::s_MSK_D0,  "P5 ", "SW1   "}, // P5 - pin 6
    // {NO_BANK_SWITCH, 0x2800,  CAsteroidsBaseGame::s_MSK_D1,  "P5 ", "SW2   "}, // P5 - pin 5
    // {NO_BANK_SWITCH, 0x2801,  CAsteroidsBaseGame::s_MSK_D0,  "P5 ", "SW3   "}, // P5 - pin 4
    // {NO_BANK_SWITCH, 0x2801,  CAsteroidsBaseGame::s_MSK_D1,  "P5 ", "SW4   "}, // P5 - pin 3
    // {NO_BANK_SWITCH, 0x2802,  CAsteroidsBaseGame::s_MSK_D0,  "P5 ", "SW5   "}, // P5 - pin 10
    // {NO_BANK_SWITCH, 0x2802,  CAsteroidsBaseGame::s_MSK_D1,  "P5 ", "SW6   "}, // P5 - pin 11
    // {NO_BANK_SWITCH, 0x2803,  CAsteroidsBaseGame::s_MSK_D0,  "P5 ", "SW7   "}, // P5 - pin 12
    // {NO_BANK_SWITCH, 0x2803,  CAsteroidsBaseGame::s_MSK_D1,  "P5 ", "SW8   "}, // P5 - pin 13
    {0}
}; // end of list


//OUTPUT

//
// Output region is the same for all ROM versions.
//
static const OUTPUT_REGION s_outputRegion[] PROGMEM = { //                                 "012", "012345"

    // L5 is a 75LS42 BCD to decimal decoder
    //
    // Pin   Address   Address Bits   Data Bits   Signal       Description
    // 1     0x3000                               _DMAGO       Starts Vector Generator
    // 2     0x3200    A0-A5          D0-D7       _EAADDRL     EAROM address/data latches
    // 3     0x3400                               _WDCLR       Resets the watchdog timer
    // 4     0x3600                   D2-D7       _EXPLODE     D6-D7 set explosion pitch, D2-D5 set explosion volume
    // 5     0x3800                               _DMARESET    Resets Vector Generator
    // 6     0x3A00                   D1-D2       _EACONTROL   EAROM control latch (use caution!)
    // 7     0x3C00    A0-A2          D7          _AUDIO       RAMSEL, coin counters, start LEDs, Ship Thrust Enable
    // 9     0x3E00                               _NOISESRESET Noise generator reset

    //                                                                                     "012", "012345"
    {NO_BANK_SWITCH, 0x3000, CAsteroidsBaseGame::s_MSK_ALL, CAsteroidsBaseGame::s_ACT_Lo,  "L5 ", "DMAGO "}, // DMAGO, L5 - pin 1
    // 0x3200-0x33ff - EAROM Latches - not implemented
    {NO_BANK_SWITCH, 0x3400, CAsteroidsBaseGame::s_MSK_ALL, CAsteroidsBaseGame::s_ACT_Lo,  "L5 ", "WD CLR"}, // WATCHDOG CLEAR, L5 - pin 3 

    {NO_BANK_SWITCH, 0x3600, CAsteroidsBaseGame::s_MSK_D6_D7,    CAsteroidsBaseGame::s_ACT_Hi,  "P6 ", "EXPPIT"}, //EXPLOSION PITCH, P6 D6-D7
    {NO_BANK_SWITCH, 0x3600, CAsteroidsBaseGame::s_MSK_D2_TO_D5, CAsteroidsBaseGame::s_ACT_Hi,  "P6 ", "EXPVOL"}, //EXPLOSION VOLUME, P6 D2-D5

    {NO_BANK_SWITCH, 0x3800, CAsteroidsBaseGame::s_MSK_ALL, CAsteroidsBaseGame::s_ACT_Lo,  "L5 ", "DMARST"}, // DMARESET, L5 - pin 5
 
    // 0x3c00-0x3c07 enables M10, a 74LS259 8-bit addressable latch (data on D7)
    {NO_BANK_SWITCH, 0x3C00, CAsteroidsBaseGame::s_MSK_D7,  CAsteroidsBaseGame::s_ACT_Lo,  "M10", "1P LMP"}, // PLAYER 1 LAMP, M10 - pin 4
    {NO_BANK_SWITCH, 0x3C01, CAsteroidsBaseGame::s_MSK_D7,  CAsteroidsBaseGame::s_ACT_Lo,  "M10", "2P LMP"}, // PLAYER 2 LAMP, M10 - pin 5
    // 0x3C02 - no connection
    {NO_BANK_SWITCH, 0x3C03, CAsteroidsBaseGame::s_MSK_D7,  CAsteroidsBaseGame::s_ACT_Hi,  "M10", "THR EN"}, // SHIPTHRUSTEN, M10 - pin 7
    {NO_BANK_SWITCH, 0x3C04, CAsteroidsBaseGame::s_MSK_D7,  CAsteroidsBaseGame::s_ACT_Hi,  "M10", "RAMSEL"}, // RAM SELECT, M10 - pin 9
    {NO_BANK_SWITCH, 0x3C05, CAsteroidsBaseGame::s_MSK_D7,  CAsteroidsBaseGame::s_ACT_Hi,  "M10", "CNCTLT"}, // COIN COUNTER LEFT, M10 - pin 10
    {NO_BANK_SWITCH, 0x3C06, CAsteroidsBaseGame::s_MSK_D7,  CAsteroidsBaseGame::s_ACT_Hi,  "M10", "CNCTCT"}, // COIN COUNTER MIDDLE, M10 - pin 11
    {NO_BANK_SWITCH, 0x3C07, CAsteroidsBaseGame::s_MSK_D7,  CAsteroidsBaseGame::s_ACT_Hi,  "M10", "CNCTRT"}, // COIN COUNTER RIGHT, M10 - pin 12

    {NO_BANK_SWITCH, 0x3E00, CAsteroidsBaseGame::s_MSK_ALL, CAsteroidsBaseGame::s_ACT_Lo,  "R8 ", "NSERST"}, // NOISE RESET, R8   
    {0}
}; // end of list

//
// Custom functions implemented for this game
// POKEY tests will go here
//
static const CUSTOM_FUNCTION s_customFunction[] PROGMEM = {
    //                                         "0123456789"
    {NO_CUSTOM_FUNCTION} // end of list
};

IGame*
CAsteroidsDeluxeGame::createInstanceSet3(
)
{
    return (new CAsteroidsDeluxeGame(false, s_romRegionSet3));
}

IGame*
CAsteroidsDeluxeGame::createInstanceClockMasterSet3(
)
{
    return (new CAsteroidsDeluxeGame(true, s_romRegionSet3));
}

IGame*
CAsteroidsDeluxeGame::createInstanceSet2(
)
{
    return (new CAsteroidsDeluxeGame(false, s_romRegionSet2));
}

IGame*
CAsteroidsDeluxeGame::createInstanceClockMasterSet2(
)
{
    return (new CAsteroidsDeluxeGame(true, s_romRegionSet2));
}

IGame*
CAsteroidsDeluxeGame::createInstanceSet1(
)
{
    return (new CAsteroidsDeluxeGame(false, s_romRegionSet1));
}

IGame*
CAsteroidsDeluxeGame::createInstanceClockMasterSet1(
)
{
    return (new CAsteroidsDeluxeGame(true, s_romRegionSet1));
}

CAsteroidsDeluxeGame::CAsteroidsDeluxeGame(
                               const bool       clockMaster,
                               const ROM_REGION *romRegion
                               ) : CAsteroidsBaseGame( clockMaster,
                                                      romRegion,
                                                      s_ramRegion,
                                                      s_ramRegionByteOnly,
                                                      s_ramRegionWriteOnly,
                                                      s_inputRegion,
                                                      s_outputRegion,
                                                      s_customFunction )
{
}
