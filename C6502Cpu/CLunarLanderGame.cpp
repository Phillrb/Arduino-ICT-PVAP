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
#include "CLunarLanderGame.h"

static const UINT32 s_ROM_2716_SIZE = 0x0800; //2KB - 2048 bytes - 0x800 in hex

//2n test of ROMS                        01   02   04   08   10   20   40   80   100  200  400
static const UINT16 s_romData_1_R3[] =  {0x27,0x00,0x80,0x80,0x00,0x00,0x80,0x80,0xc6,0x00,0x00}; //034599-01.r3 -  0x355A9371
static const UINT16 s_romData_1_NP3[] = {0xc8,0x4c,0x52,0x00,0x68,0x8d,0x6f,0xd1,0xaa,0x00,0xd7}; //034598-01.np3 - 0x9C4FFA68
static const UINT16 s_romData_2_B1[] =  {0x07,0xfe,0xfb,0x45,0x10,0x18,0xe6,0x02,0xf0,0xaa,0x9a}; //034569-02.b1 -  0x72837A4E
static const UINT16 s_romData_1_C1[] =  {0x17,0x49,0xa8,0xff,0x65,0x40,0x37,0x86,0xa2,0x85,0x91}; //034570-01.c1 -  0x2724E591
static const UINT16 s_romData_2_DE1[] = {0x86,0x41,0x33,0x0a,0x03,0x86,0x20,0x7f,0x62,0x32,0x38}; //034571-02.de1 - 0x77DA4B2F
static const UINT16 s_romData_2_F1[] =  {0x20,0x13,0xa9,0x24,0x85,0x13,0x20,0x67,0xd0,0x20,0xa0}; //034572-02.f1 -  0xB8763EEA

static const UINT16 s_romData_1_B1[] =  {0x07,0xfe,0xfb,0x45,0x10,0x18,0xe6,0x02,0xf0,0xaa,0x9a}; //034569-01.b1 -  0xB11A7D01
static const UINT16 s_romData_1_DE1[] = {0x7b,0x49,0x18,0xa2,0x0a,0xa9,0x47,0x10,0xa2,0x32,0x38}; //034571-01.de1 - 0x493E24B7
static const UINT16 s_romData_1_F1[] =  {0x20,0x13,0xa9,0x24,0x85,0x13,0x20,0x67,0xd0,0x20,0xd0}; //034572-01.f1 -  0x2AFF3140


//034597-01.m3 0x503F992E - optional ROM for other languages
//034602-01.c8 0x97953DB8 - vec PROM

//4800-4FFF - VECTOR ROM at R3 - Read-only
//5000-5FFF - VECTOR ROM at N/P3 - Read-only

//6000-7FFF - PROG ROM - Read-only - 8316E's
//  ADDRESS     POS     CODE    SIGNAL
//  6000-6FFF - F1 -   034569 - PROM0
//  7000-77FF - D/E1 - 034570 - PROM1
//  7800-7FFF - C1 -   034571 - PROM2
//  7800-8FFF - B1 -   034572 - PROM3

static const UINT32 s_ROM_ADDR_R3 =  0x4800;
static const UINT32 s_ROM_ADDR_NP3 = 0x5000;

static const UINT32 s_ROM_ADDR_F1 =  0x6000;
static const UINT32 s_ROM_ADDR_DE1 = 0x6800;
static const UINT32 s_ROM_ADDR_C1 =  0x7000;
static const UINT32 s_ROM_ADDR_B1 =  0x7800;

//
// Set 2 from MAME - 'llander'
//
static const ROM_REGION s_romRegionSet2[] PROGMEM = { //
    {NO_BANK_SWITCH, s_ROM_ADDR_R3,  s_ROM_2716_SIZE, s_romData_1_R3,  0x355A9371, "R3"},  //R3 -  v1
    {NO_BANK_SWITCH, s_ROM_ADDR_NP3, s_ROM_2716_SIZE, s_romData_1_NP3, 0x9C4FFA68, "NP3"}, //NP3 - v1
    {NO_BANK_SWITCH, s_ROM_ADDR_F1,  s_ROM_2716_SIZE, s_romData_2_F1,  0xB8763EEA, "F1 "}, //F1 -  v2
    {NO_BANK_SWITCH, s_ROM_ADDR_DE1, s_ROM_2716_SIZE, s_romData_2_DE1, 0x77DA4B2F, "DE1"}, //DE1 - v2
    {NO_BANK_SWITCH, s_ROM_ADDR_C1,  s_ROM_2716_SIZE, s_romData_1_C1,  0x2724E591, "C1 "}, //C1 -  v1
    {NO_BANK_SWITCH, s_ROM_ADDR_B1,  s_ROM_2716_SIZE, s_romData_2_B1,  0x72837A4E, "B1 "}, //B1 -  v2
    {0} }; // end of list

//
// Set 1 from MAME - 'llander1'
//
static const ROM_REGION s_romRegionSet1[] PROGMEM = { //
    {NO_BANK_SWITCH, s_ROM_ADDR_R3,  s_ROM_2716_SIZE, s_romData_1_R3,  0x355A9371, "R3"},  //R3 -  v1
    {NO_BANK_SWITCH, s_ROM_ADDR_NP3, s_ROM_2716_SIZE, s_romData_1_NP3, 0x9C4FFA68, "NP3"}, //NP3 - v1
    {NO_BANK_SWITCH, s_ROM_ADDR_F1,  s_ROM_2716_SIZE, s_romData_1_F1,  0x2AFF3140, "F1 "}, //F1 -  v1
    {NO_BANK_SWITCH, s_ROM_ADDR_DE1, s_ROM_2716_SIZE, s_romData_1_DE1, 0x493E24B7, "DE1"}, //DE1 - v1
    {NO_BANK_SWITCH, s_ROM_ADDR_C1,  s_ROM_2716_SIZE, s_romData_1_C1,  0x2724E591, "C1 "}, //C1 -  v1
    {NO_BANK_SWITCH, s_ROM_ADDR_B1,  s_ROM_2716_SIZE, s_romData_1_B1,  0xB11A7D01, "B1 "}, //B1 -  v1
    {0} }; // end of list

ROM_REGION CLunarLanderGame::getLLanderRomset2(){
    return *s_romRegionSet2;
}
ROM_REGION CLunarLanderGame::getLLanderRomset1(){
    return *s_romRegionSet1;
}

//INPUT
//2000 D0 HALT
//2000 D1 TEST SW
//2000 D2 SLAM SW
//2000 D6 3KHz
//2000 D7 DIA STEP

//2400 D7       START
//2401 D7       COIN 3
//2402 D7       COIN 2
//2403 D7       COIN 1
//2404 D7       GAME SELECT
//2405 D7       ABORT
//2406 D7       ROT RIGHT
//2407 D7       ROT LEFT

//2800 D0 D1    OPTION
//2801 D0 D1    OPTION
//2802 D0 D1    OPTION
//2803 D0 D1    OPTION
//2C00 D0-D7    POT IN

//
// Input region is the same for all ROM versions.
//
static const INPUT_REGION s_inputRegion[] PROGMEM = { //                                                           "012", "012345"
                                                        {NO_BANK_SWITCH, 0x2000,  CLunarLanderBaseGame::s_MSK_D0,  "M10", "HALT  "}, //INPUTS - M10 pin 7 (buffer)
                                                        {NO_BANK_SWITCH, 0x2000,  CLunarLanderBaseGame::s_MSK_D1,  "M10", "TEST  "}, // M10 pin 11
                                                        {NO_BANK_SWITCH, 0x2000,  CLunarLanderBaseGame::s_MSK_D2,  "M10", "SLAM  "}, // M10 pin 5
                                                        {NO_BANK_SWITCH, 0x2000,  CLunarLanderBaseGame::s_MSK_D6,  "M10", "3KHZ  "}, // M10 pin 13
                                                        {NO_BANK_SWITCH, 0x2000,  CLunarLanderBaseGame::s_MSK_D7,  "M10", "DIASTP"}, // M10 pin 3

                                                        {NO_BANK_SWITCH, 0x2400,  CLunarLanderBaseGame::s_MSK_D7,  "K10", "START "}, // K10 pin 6 - pins 9,10,11 all low (AB2, AB1, AB0) (mux) - driven by pin 4
                                                        {NO_BANK_SWITCH, 0x2401,  CLunarLanderBaseGame::s_MSK_D7,  "K10", "COIN 3"}, // K10 pin 6 ????? - DONT SEE ON SCHEMATICS
                                                        {NO_BANK_SWITCH, 0x2402,  CLunarLanderBaseGame::s_MSK_D7,  "K10", "COIN 2"}, // K10 pin 6
                                                        {NO_BANK_SWITCH, 0x2403,  CLunarLanderBaseGame::s_MSK_D7,  "K10", "COIN 1"}, // K10 pin 6
                                                        {NO_BANK_SWITCH, 0x2404,  CLunarLanderBaseGame::s_MSK_D7,  "K10", "SELECT"}, // K10 pin 6 (pin 15 driven)
                                                        {NO_BANK_SWITCH, 0x2405,  CLunarLanderBaseGame::s_MSK_D7,  "K10", "ABORT "}, // K10 pin 6 (pin 14 driven)
                                                        {NO_BANK_SWITCH, 0x2406,  CLunarLanderBaseGame::s_MSK_D7,  "K10", "ROT R "}, // K10 pin 6 (pin 13 driven)
                                                        {NO_BANK_SWITCH, 0x2407,  CLunarLanderBaseGame::s_MSK_D7,  "K10", "ROT L "}, // K10 pin 6 (pin 12 driven)

                                                        {NO_BANK_SWITCH, 0x2800,  CLunarLanderBaseGame::s_MSK_D0,  "N8 ", "SW1   "}, //OPT SWITCHES
                                                        {NO_BANK_SWITCH, 0x2800,  CLunarLanderBaseGame::s_MSK_D1,  "N8 ", "SW2   "},
                                                        {NO_BANK_SWITCH, 0x2801,  CLunarLanderBaseGame::s_MSK_D0,  "N8 ", "SW3   "},
                                                        {NO_BANK_SWITCH, 0x2801,  CLunarLanderBaseGame::s_MSK_D1,  "N8 ", "SW4   "},
                                                        {NO_BANK_SWITCH, 0x2802,  CLunarLanderBaseGame::s_MSK_D0,  "N8 ", "SW5   "},
                                                        {NO_BANK_SWITCH, 0x2802,  CLunarLanderBaseGame::s_MSK_D1,  "N8 ", "SW6   "},
                                                        {NO_BANK_SWITCH, 0x2803,  CLunarLanderBaseGame::s_MSK_D0,  "N8 ", "SW7   "},
                                                        {NO_BANK_SWITCH, 0x2803,  CLunarLanderBaseGame::s_MSK_D1,  "N8 ", "SW8   "},
    
                                                        {NO_BANK_SWITCH, 0x2C00,  CLunarLanderBaseGame::s_MSK_ALL, "N9 ", "POT IN"}, //THRUST IN VAL
                                                      {0}
                                                    }; // end of list

//OUTPUT

//3200 D0 LAMP 0
//3200 D1 LAMP 1
//3200 D2 LAMP 2
//3200 D3 LAMP 3
//3200 D4 START SELECT LEDS
//3200 D5 COIN COUNTER

//3C00 D0 D1 D2 THRUST INTENSITY
//3C00 D3       THRUST PITCH
//3C00 D4 D5    TONE INTENSITY
//3E00 ???      NOISE RESET


//
// Output region is the same for all ROM versions.
//
static const OUTPUT_REGION s_outputRegion[] PROGMEM = { //                                             "012", "012345"
            {NO_BANK_SWITCH, 0x3200, CLunarLanderBaseGame::s_MSK_D0,  CLunarLanderBaseGame::s_ACT_Hi,  "???", "LAMP 0"},
            {NO_BANK_SWITCH, 0x3200, CLunarLanderBaseGame::s_MSK_D1,  CLunarLanderBaseGame::s_ACT_Hi,  "???", "LAMP 1"},
            {NO_BANK_SWITCH, 0x3200, CLunarLanderBaseGame::s_MSK_D2,  CLunarLanderBaseGame::s_ACT_Hi,  "???", "LAMP 2"},
            {NO_BANK_SWITCH, 0x3200, CLunarLanderBaseGame::s_MSK_D3,  CLunarLanderBaseGame::s_ACT_Hi,  "???", "LAMP 3"},
            {NO_BANK_SWITCH, 0x3200, CLunarLanderBaseGame::s_MSK_D4,  CLunarLanderBaseGame::s_ACT_Hi,  "???", "S LEDS"},
            {NO_BANK_SWITCH, 0x3200, CLunarLanderBaseGame::s_MSK_D5,  CLunarLanderBaseGame::s_ACT_Hi,  "???", "CNCTRT"},

            {0}
                                                      }; // end of list

IGame*
CLunarLanderGame::createInstanceSet2(
)
{
    return (new CLunarLanderGame(false, s_romRegionSet2));
}

IGame*
CLunarLanderGame::createInstanceClockMasterSet2(
)
{
    return (new CLunarLanderGame(true, s_romRegionSet2));
}

IGame*
CLunarLanderGame::createInstanceSet1(
)
{
    return (new CLunarLanderGame(false, s_romRegionSet1));
}

IGame*
CLunarLanderGame::createInstanceClockMasterSet1(
)
{
    return (new CLunarLanderGame(true, s_romRegionSet1));
}

CLunarLanderGame::CLunarLanderGame(
    const bool       clockMaster,
    const ROM_REGION *romRegion
) : CLunarLanderBaseGame( clockMaster,
                           romRegion,
                           s_inputRegion,
                           s_outputRegion )
{
}


