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

//TODO: 2n data must be updated with tool!

//2n test of ROMS                        01   02   04   08   10   20   40   80   100  200  400
static const UINT16 s_romData_3_J1[] =  {0x85,0x06,0x7d,0x18,0x02,0xa0,0x1c,0x02,0x3e,0xa7,0x0f}; //036433-03.j1 - 0x0DCC0BE6

static const UINT16 s_romData_2_R2[] =  {0xa0,0x00,0x00,0x00,0x00,0xfe,0x00,0x3b,0x0d,0x7a,0x80}; //036800-02.r2 - 0xBB8CABE1
static const UINT16 s_romData_2_D1[] =  {0xbd,0x88,0x85,0x78,0x02,0x70,0x85,0x9d,0x67,0x9d,0xc0}; //036430-02.d1 - 0xA4D7A525
static const UINT16 s_romData_2_EF1[] = {0x85,0x06,0x7d,0x18,0x02,0xa0,0x1c,0x02,0x3e,0xa7,0x0f}; //036431-02.ef1 - 0xD4004AAE
static const UINT16 s_romData_2_FH1[] = {0xf3,0x7c,0xfa,0x6e,0xfe,0x01,0xc2,0x02,0x8d,0x04,0x60}; //036432-02.fh1 - 0x6D720C41

static const UINT16 s_romData_1_NP2[] = {0xa0,0x00,0x00,0x00,0x00,0xfe,0x00,0x3b,0x81,0x0a,0xc0}; //036799-01.np2 - 0x7D511572

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
    {NO_BANK_SWITCH, s_ROM_ADDR_R2,  s_ROM_2716_SIZE, s_romData_2_R2,  0xBB8CABE1, "R2 "}, //R2 -   v2
    {NO_BANK_SWITCH, s_ROM_ADDR_NP2, s_ROM_2716_SIZE, s_romData_1_NP2, 0x7D511572, "NP2"}, //NP2 -  v1
    {NO_BANK_SWITCH, s_ROM_ADDR_D1,  s_ROM_2716_SIZE, s_romData_2_D1,  0x4D7A525,  "D1 "}, //D1  -  v2
    {NO_BANK_SWITCH, s_ROM_ADDR_EF1, s_ROM_2716_SIZE, s_romData_2_EF1, 0xD4004AAE, "EF1"}, //EF1 -  v2
    {NO_BANK_SWITCH, s_ROM_ADDR_FH1, s_ROM_2716_SIZE, s_romData_2_FH1, 0x6D720C41, "FH1"}, //FH1 -  v2
    {NO_BANK_SWITCH, s_ROM_ADDR_J1,  s_ROM_2716_SIZE, s_romData_3_J1,  0x0DCC0BE6, "J1 "}, //J1  -  v3
    {0} }; // end of list

//
// Set 2 from MAME //TODO UPDATE
//
static const ROM_REGION s_romRegionSet2[] PROGMEM = { //
    {NO_BANK_SWITCH, s_ROM_ADDR_R2,  s_ROM_2716_SIZE, s_romData_2_R2,  0xBB8CABE1, "R2 "}, //R2 -   v2
    {NO_BANK_SWITCH, s_ROM_ADDR_NP2, s_ROM_2716_SIZE, s_romData_1_NP2, 0x7D511572, "NP2"}, //NP2 -  v1
    {NO_BANK_SWITCH, s_ROM_ADDR_D1,  s_ROM_2716_SIZE, s_romData_2_D1,  0x4D7A525,  "D1 "}, //D1  -  v2
    {NO_BANK_SWITCH, s_ROM_ADDR_EF1, s_ROM_2716_SIZE, s_romData_2_EF1, 0xD4004AAE, "EF1"}, //EF1 -  v2
    {NO_BANK_SWITCH, s_ROM_ADDR_FH1, s_ROM_2716_SIZE, s_romData_2_FH1, 0x6D720C41, "FH1"}, //FH1 -  v2
    {NO_BANK_SWITCH, s_ROM_ADDR_J1,  s_ROM_2716_SIZE, s_romData_3_J1,  0x0DCC0BE6, "J1 "}, //J1  -  v3
    {0} }; // end of list

//
// Set 1 from MAME //TODO UPDATE
//
static const ROM_REGION s_romRegionSet1[] PROGMEM = { //
    {NO_BANK_SWITCH, s_ROM_ADDR_R2,  s_ROM_2716_SIZE, s_romData_2_R2,  0xBB8CABE1, "R2 "}, //R2 -   v2
    {NO_BANK_SWITCH, s_ROM_ADDR_NP2, s_ROM_2716_SIZE, s_romData_1_NP2, 0x7D511572, "NP2"}, //NP2 -  v1
    {NO_BANK_SWITCH, s_ROM_ADDR_D1,  s_ROM_2716_SIZE, s_romData_2_D1,  0x4D7A525,  "D1 "}, //D1  -  v2
    {NO_BANK_SWITCH, s_ROM_ADDR_EF1, s_ROM_2716_SIZE, s_romData_2_EF1, 0xD4004AAE, "EF1"}, //EF1 -  v2
    {NO_BANK_SWITCH, s_ROM_ADDR_FH1, s_ROM_2716_SIZE, s_romData_2_FH1, 0x6D720C41, "FH1"}, //FH1 -  v2
    {NO_BANK_SWITCH, s_ROM_ADDR_J1,  s_ROM_2716_SIZE, s_romData_3_J1,  0x0DCC0BE6, "J1 "}, //J1  -  v3
    {0} }; // end of list

ROM_REGION CAsteroidsDeluxeGame::getAstDlxRomset3(){ return *s_romRegionSet3; }
ROM_REGION CAsteroidsDeluxeGame::getAstDlxRomset2(){ return *s_romRegionSet2; }
ROM_REGION CAsteroidsDeluxeGame::getAstDlxRomset1(){ return *s_romRegionSet1; }

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
static const INPUT_REGION s_inputRegion[] PROGMEM = { //                                                         "012", "012345"
    {NO_BANK_SWITCH, 0x2001,  CAsteroidsBaseGame::s_MSK_D7,  "???", "3KHZ  "}, //INPUTS - pin 8 on B4
    {NO_BANK_SWITCH, 0x2002,  CAsteroidsBaseGame::s_MSK_D7,  "???", "HALT  "}, //L11 or L10 - pin 2
    
    {NO_BANK_SWITCH, 0x2003,  CAsteroidsBaseGame::s_MSK_D7,  "???", "SHIELD"}, //PLAYER INPUT - L11 or L10 - pin 1
    {NO_BANK_SWITCH, 0x2004,  CAsteroidsBaseGame::s_MSK_D7,  "???", "FIRE  "}, // L11 or L10 - pin 15
    
    {NO_BANK_SWITCH, 0x2005,  CAsteroidsBaseGame::s_MSK_D7,  "???", "DIASTP"}, //TEST INPUT // L11 or L10 - pin 14
    {NO_BANK_SWITCH, 0x2006,  CAsteroidsBaseGame::s_MSK_D7,  "???", "SLAM  "}, // L11 or L10 - pin 13
    {NO_BANK_SWITCH, 0x2007,  CAsteroidsBaseGame::s_MSK_D7,  "???", "SLFTST"}, // L11 or L10 - pin 12
    
    {NO_BANK_SWITCH, 0x2400,  CAsteroidsBaseGame::s_MSK_D7,  "???", "LFT CN"}, //COIN //J11 or J10 - pin 4
    {NO_BANK_SWITCH, 0x2401,  CAsteroidsBaseGame::s_MSK_D7,  "???", "CNT CN"}, //J11 or J10 - pin 3
    {NO_BANK_SWITCH, 0x2402,  CAsteroidsBaseGame::s_MSK_D7,  "???", "RT CN "}, //J11 or J10 - pin 2
    
    
    //TODO - locations
    {NO_BANK_SWITCH, 0x2403,  CAsteroidsBaseGame::s_MSK_D7,  "???", "1PLYST"}, //MORE PLAYER INPUT
    {NO_BANK_SWITCH, 0x2404,  CAsteroidsBaseGame::s_MSK_D7,  "???", "2PLYST"},
    {NO_BANK_SWITCH, 0x2405,  CAsteroidsBaseGame::s_MSK_D7,  "???", "THRUST"},
    {NO_BANK_SWITCH, 0x2406,  CAsteroidsBaseGame::s_MSK_D7,  "???", "ROT R "},
    {NO_BANK_SWITCH, 0x2407,  CAsteroidsBaseGame::s_MSK_D7,  "???", "ROT L "},
    
    {NO_BANK_SWITCH, 0x2800,  CAsteroidsBaseGame::s_MSK_D0,  "???", "SW1   "}, //OPT SWITCHES
    {NO_BANK_SWITCH, 0x2800,  CAsteroidsBaseGame::s_MSK_D1,  "???", "SW2   "},
    {NO_BANK_SWITCH, 0x2801,  CAsteroidsBaseGame::s_MSK_D0,  "???", "SW3   "},
    {NO_BANK_SWITCH, 0x2801,  CAsteroidsBaseGame::s_MSK_D1,  "???", "SW4   "},
    {NO_BANK_SWITCH, 0x2802,  CAsteroidsBaseGame::s_MSK_D0,  "???", "SW5   "},
    {NO_BANK_SWITCH, 0x2802,  CAsteroidsBaseGame::s_MSK_D1,  "???", "SW6   "},
    {NO_BANK_SWITCH, 0x2803,  CAsteroidsBaseGame::s_MSK_D0,  "???", "SW7   "},
    {NO_BANK_SWITCH, 0x2803,  CAsteroidsBaseGame::s_MSK_D1,  "???", "SW8   "},
    {0}
}; // end of list

//OUTPUT


//
// Output region is the same for all ROM versions.
//
static const OUTPUT_REGION s_outputRegion[] PROGMEM = { //                                         "012", "012345"
    //MIGHT BE ACTIVE LOW?
    {NO_BANK_SWITCH, 0x3000, CAsteroidsBaseGame::s_MSK_ALL, CAsteroidsBaseGame::s_ACT_Hi,  "???", "DMAGO "},
    
//    {NO_BANK_SWITCH, 0x3200, CAsteroidsBaseGame::s_MSK_D0,  CAsteroidsBaseGame::s_ACT_Hi,  "???", "2P LMP"}, //PLAYER 2 LAMP
//    {NO_BANK_SWITCH, 0x3200, CAsteroidsBaseGame::s_MSK_D1,  CAsteroidsBaseGame::s_ACT_Hi,  "???", "1P LMP"}, //PLAYER 1 LAMP
    {NO_BANK_SWITCH, 0x3200, CAsteroidsBaseGame::s_MSK_D1,  CAsteroidsBaseGame::s_ACT_Hi,  "???", "RAMSEL"}, //RAM SELECT
    {NO_BANK_SWITCH, 0x3200, CAsteroidsBaseGame::s_MSK_D3,  CAsteroidsBaseGame::s_ACT_Hi,  "???", "CNCTLT"}, //COIN CONTROL LEFT
    {NO_BANK_SWITCH, 0x3200, CAsteroidsBaseGame::s_MSK_D4,  CAsteroidsBaseGame::s_ACT_Hi,  "???", "CNCTCT"}, //COIN CONTROL MIDDLE
    {NO_BANK_SWITCH, 0x3200, CAsteroidsBaseGame::s_MSK_D5,  CAsteroidsBaseGame::s_ACT_Hi,  "???", "CNCTRT"}, ////COIN CONTROL RIGHT
    
    //MIGHT BE ACTIVE LOW?
    {NO_BANK_SWITCH, 0x3400, CAsteroidsBaseGame::s_MSK_ALL, CAsteroidsBaseGame::s_ACT_Hi,  "???", "WD CLR"}, //WATCHDOG CLEAR
    
    //                                                                                          "012", "012345"
    {NO_BANK_SWITCH, 0x3600, CAsteroidsBaseGame::s_MSK_D6_D7,    CAsteroidsBaseGame::s_ACT_Hi,  "???", "EXPPIT"}, //EXPLOSION PITCH
    {NO_BANK_SWITCH, 0x3600, CAsteroidsBaseGame::s_MSK_D2_TO_D5, CAsteroidsBaseGame::s_ACT_Hi,  "???", "EXPVOL"}, //EXPLOSION VOLUME
    
//    {NO_BANK_SWITCH, 0x3A00, CAsteroidsBaseGame::s_MSK_D4,       CAsteroidsBaseGame::s_ACT_Hi,  "???", "THMPVL"}, //THUMP VOLUME
//    {NO_BANK_SWITCH, 0x3A00, CAsteroidsBaseGame::s_MSK_D0_TO_D3, CAsteroidsBaseGame::s_ACT_Hi,  "???", "THMPFR"}, //THUMP FREQUENCY
    
    //                                                                                     "012", "012345"
    {NO_BANK_SWITCH, 0x3C00, CAsteroidsBaseGame::s_MSK_D7,  CAsteroidsBaseGame::s_ACT_Hi,  "???", "1P LMP"}, //PLAYER 1 LAMP
    {NO_BANK_SWITCH, 0x3C01, CAsteroidsBaseGame::s_MSK_D7,  CAsteroidsBaseGame::s_ACT_Hi,  "???", "2P LMP"}, //PLAYER 2 LAMP
//    {NO_BANK_SWITCH, 0x3C02, CAsteroidsBaseGame::s_MSK_D7,  CAsteroidsBaseGame::s_ACT_Hi,  "???", "SSNDSL"}, //SAUCER SOUND SELECT
    {NO_BANK_SWITCH, 0x3C03, CAsteroidsBaseGame::s_MSK_D7,  CAsteroidsBaseGame::s_ACT_Hi,  "???", "THRSND"}, //THRUST SOUND
//    {NO_BANK_SWITCH, 0x3C04, CAsteroidsBaseGame::s_MSK_D7,  CAsteroidsBaseGame::s_ACT_Hi,  "???", "FIRSND"}, //FIRE SOUND
//    {NO_BANK_SWITCH, 0x3C05, CAsteroidsBaseGame::s_MSK_D7,  CAsteroidsBaseGame::s_ACT_Hi,  "???", "LIFSND"}, //LIFE SOUND
    
    //MIGHT BE ACTIVE LOW?
    {NO_BANK_SWITCH, 0x3E00, CAsteroidsBaseGame::s_MSK_ALL, CAsteroidsBaseGame::s_ACT_Hi,  "???", "NSERST"}, //NOISE RESET
    {0}
}; // end of list

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
                                                      s_inputRegion,
                                                      s_outputRegion )
{
}


