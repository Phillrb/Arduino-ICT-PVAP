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
#include "CPaclandBaseGame.h"
#include "C6809ECpu.h"

//
// Notes
// -----
//
// * The Clock output from the ICT (J14 pin 8) should be connected to pin 35 of CUS27 at 6D pin that's
//   been disconnected from the '49MHz / 32 = 1.536MHz' signal clock source (e.g. bend out pin 35 and connect the ICT clock to it).
// * GND input from the board (select a suitable IC near the clock to hook up a GND pin - e.g. 7B pin 8) should be fed into J13 (either pin)
// * RAMs at 4E, 2T, 3S and 3T are not accessible by CPU
// * ROMs at 7E, 6E, 7F, 6F, 3E, 6N, 6T, 6L, 5T, 4N, 1T, 1R are not accessible by CPU
// * ROMs at 8E, 8F, 8H and 8J are bank switched as 2 slices each
//   - 0x3c00 must have 0-7 written to it and then a designated slice becomes available at 0x4000 to 0x5FFF

// ROM bank switching
static const UINT32 c_MPAGE  = 0x3c00; //Address to write value -> selects active ROM slice
static const UINT8  c_MPAGE_8E_Lo  = 0x00;
static const UINT8  c_MPAGE_8E_Hi  = 0x01;
static const UINT8  c_MPAGE_8F_Lo  = 0x02;
static const UINT8  c_MPAGE_8F_Hi  = 0x03;
static const UINT8  c_MPAGE_8H_Lo  = 0x04;
static const UINT8  c_MPAGE_8H_Hi  = 0x05;
static const UINT8  c_MPAGE_8J_Lo  = 0x06;
static const UINT8  c_MPAGE_8J_Hi  = 0x07;

//
// Write-only RAM region
//
static const RAM_REGION s_ramRegionWriteOnly[] PROGMEM = {
    {0}
}; // end of list

//
// RAM region
//

//This information was taken from MAME - 'pacland_driver.cpp'
//It's a list of just the READ/WRITE RAM
//0x0000 - 0x0FFF \ 0000xxxxxxxxxxx0 R/W xxxxxxxx RAM2      tilemap RAM 9S
//0x0000 - 0x0FFF / 0000xxxxxxxxxxx1 R/W xxxxxxxx RAM3      tilemap RAM 9R
//0x1000 - 0x1FFF \ 0001xxxxxxxxxxx0 R/W xxxxxxxx RAM0      tilemap RAM 10S
//0x1000 - 0x1FFF / 0001xxxxxxxxxxx1 R/W xxxxxxxx RAM1      tilemap RAM 10R

//0x2000 - 0x27FF   00100xxxxxxxxxxx R/W xxxxxxxx ORAM0     work RAM -> 0x2000 - 0x277F -> 0x2780 - 0x27FF 001001111xxxxxxx portion holding sprite registers (sprite number & color)
//0x2800 - 0x2FFF   00101xxxxxxxxxxx R/W xxxxxxxx ORAM1     work RAM -> 0x2800 - 0x2F7F -> 0x2F80 - 0x2FFF 001011111xxxxxxx portion holding sprite registers (x, y)
//0x3000 - 0x37FF   00110xxxxxxxxxxx R/W xxxxxxxx ORAM2     work RAM -> 0x3000 - 0x377F -> 0x3780 - 0x37FF 001101111xxxxxxx portion holding sprite registers (x msb, flip, size)

//0x6800 - 0x6BFF   01101xxxxxxxxxxx R/W xxxxxxxx RAM 3J/3K sound RAM (through CUS30, shared with MCU) -> two 1024x4bit 8148 RAMs can be replaced by one 2048x8bit 2018 RAM @ 3L
//                  01101000xxxxxxxx portion holding the sound wave data
//                  0110100100xxxxxx portion holding the sound registers


//This is a list of all individual READ/WRITE RAMs that are accessible by the CPU
//Info on how the RAMs are laid out is needed to individually exercise and test them

static const RAM_REGION s_ramRegion[] PROGMEM = { //    "012", "012345"
    
    //These 4 RAMs are in pairs as they couple-up to form two 16bit data bus
    //Each RAM is assigned to the odd or even bytes
    {NO_BANK_SWITCH, 0x0000,      0x0FFF,      2, 0xFF, "9S ", "VID 2 "}, // "Vid RAM 2&3 6116 at 9S & 9R" - uses 'Step = 2' as these are two RAMS, odd and even
    {NO_BANK_SWITCH, 0x0001,      0x0FFF,      2, 0xFF, "9R ", "VID 3 "},
    {NO_BANK_SWITCH, 0x1000,      0x1FFF,      2, 0xFF, "10S", "VID 0 "}, // "Vid RAM 0&1 6116 at 10S & 10R" - uses 'Step = 2' as these are two RAMS, odd and even
    {NO_BANK_SWITCH, 0x1001,      0x1FFF,      2, 0xFF, "10R", "VID 1 "},
    
    //These 3 RAMs have no special setup
    {NO_BANK_SWITCH, 0x2000,      0x27FF,      1, 0xFF, "9N ", "SPRI 0"}, // "Sprite RAM" // 0x2000 - 0x27ff: 9N - work RAM
    {NO_BANK_SWITCH, 0x2800,      0x2FFF,      1, 0xFF, "9P ", "SPRI 1"}, // 0x2800 - 0x2fff: 9P - work RAM
    {NO_BANK_SWITCH, 0x3000,      0x37FF,      1, 0xFF, "9M ", "SPRI 2"}, // 0x3000 - 0x37ff: 9M - work RAM
    
    //These 2 RAMs are 4-bit each and so work as a pair - shared with MCU
    {NO_BANK_SWITCH, 0x6800,      0x6BFF,      1, 0x0F, "3K ", "SND LO"}, // 0x6800 - 0x6BFF: sound RAM - 4bits
    {NO_BANK_SWITCH, 0x6800,      0x6BFF,      1, 0xF0, "3J ", "SND HI"}, // 0x6800 - 0x6BFF: sound RAM - 4bits

    {0}
}; // end of list

//This is all RAMs treated as blocks of 8-bit bytes - used to exercise all of the available memory space
static const RAM_REGION s_ramRegionByteOnly[] PROGMEM = { //"012", "012345"
    {NO_BANK_SWITCH, 0x0000,      0x0FFF,      2, 0xFF, "9S ", "VID 2 "}, // "Vid RAM 2&3 6116 at 9S & 9R" - uses 'Step = 2' as these are two RAMS, odd and even
    {NO_BANK_SWITCH, 0x0001,      0x0FFF,      2, 0xFF, "9R ", "VID 3 "},
    {NO_BANK_SWITCH, 0x1000,      0x1FFF,      2, 0xFF, "10S", "VID 0 "}, // "Vid RAM 0&1 6116 at 10S & 10R" - uses 'Step = 2' as these are two RAMS, odd and even
    {NO_BANK_SWITCH, 0x1001,      0x1FFF,      2, 0xFF, "10R", "VID 1 "},
    {NO_BANK_SWITCH, 0x2000,      0x27FF,      1, 0xFF, "9N ", "SPRI 0"}, // "Sprite RAM" // 0x2000 - 0x27ff: 9N - work RAM
    {NO_BANK_SWITCH, 0x2800,      0x2FFF,      1, 0xFF, "9P ", "SPRI 1"}, // 0x2800 - 0x2fff: 9P - work RAM
    {NO_BANK_SWITCH, 0x3000,      0x37FF,      1, 0xFF, "9M ", "SPRI 2"}, // 0x3000 - 0x37ff: 9M - work RAM
    {NO_BANK_SWITCH, 0x6800,      0x6BFF,      1, 0xFF, "3JK", "SOUND "}, // 0x6800 - 0x6BFF: 3J/3K - sound RAM (might be 3L instead) - via CUS30 at 3F & shared with MCU
    {0}
}; // end of list


//RAM that is not accessible by CPU - 6116 at 4E, 2148s at 2T/3S, 2148(?) at 3T
//The MCU (CUS60 at 2B) has the following RAM address range:
    //AM_RANGE(0xc000, 0xc7ff): Work RAM at 4E
    //AM_RANGE(0x1000, 0x17ff): 3J/3K or 3L via CUS30 at 3F - but these are accessible by CPU
//These are ignored by MAME (see 'pacland_vid.cpp') as CUS29 at 2R is emulated:
//CUS29 at 2R (sprite line buffer and sprite/tilemap mixer):
    // 2T & 3S form an 8-bit pair (4bit RAMs each - like 3J/3K)
    // 3T is labelled as a 8147 (rather than a 8148 like 2T/3S/3J/3K)




//
// Input region is the same for all versions.
//
static const INPUT_REGION s_inputRegion[] PROGMEM = { //                                      "012", "012345"
//THIS IS FROM STAR WARS - JUST FOR REFERENCE
    /*{NO_BANK_SWITCH, 0x4300,    0xFF,       "9J ", "IN0   "}, // Input port 0
     {NO_BANK_SWITCH, 0x4320,    0xFF,       "9H ", "IN1   "}, // Input port 1
     {NO_BANK_SWITCH, 0x4340,    0xFF,       "9E ", "OPT0  "}, // DIP Switch 10D
     {NO_BANK_SWITCH, 0x4360,    0xFF,       "9F ", "OPT1  "}, // DIP Switch 10E/F
     {NO_BANK_SWITCH, c_ADC_A,   0xFF,       "9K ", "ADC   "}, // Yoke analog inputs
     {NO_BANK_SWITCH, c_MBRUN_A, c_MBRUN_D,  "9H ", "MB RUN"}, // MATHRUN
     {NO_BANK_SWITCH, c_REH_A,   0xFF,       "4J ", "REH   "}, // REH Hi - Quotient Hi
     {NO_BANK_SWITCH, c_REL_A,   0xFF,       "4K ", "REL   "}, // REH Lo - Quotient Lo*/
    {0}
}; // end of list



//
// Output region is the same for all versions.
//
static const OUTPUT_REGION s_outputRegion[] PROGMEM = { //                                                "012", "012345"
    //THIS IS FROM STAR WARS - JUST FOR REFERENCE
    /*{NO_BANK_SWITCH, c_ADCSTART0_A, 0x00,      0x00,  "9K ", "ADCS0 "}, // ADC start channel 0 (pitch,  J)
     {NO_BANK_SWITCH, c_ADCSTART1_A, 0x00,      0x00,  "9K ", "ADCS1 "}, // ADC start channel 0 (yaw,    K)
     {NO_BANK_SWITCH, c_ADCSTART2_A, 0x00,      0x00,  "9K ", "ADCS2 "}, // ADC start channel 0 (thrust, 9)
     {NO_BANK_SWITCH, c_MPAGE_A,     c_MPAGE_D, 0x00,  "9LM", "MPAGE "}, // MPAGE ROM bank switch
     {NO_BANK_SWITCH, c_MW0_A,       0xFF,      0x00,  "   ", "MW0-PA"}, // MW0 - MP Address MPA2-MPA9, run
     {NO_BANK_SWITCH, c_MW1_A,       0x01,      0x00,  "3D ", "MW1-BI"}, // MW1 - MP Block Index BIC8
     {NO_BANK_SWITCH, c_MW2_A,       0xFF,      0x00,  "   ", "MW2-BI"}, // MW2 - MP Block Index BIC0-BIC7
     {NO_BANK_SWITCH, c_DVSRH_A,     0xFF,      0x00,  "45P", "DVSRH "}, // DVSRH - Divisor Hi, Q clear, load div.
     {NO_BANK_SWITCH, c_DVSRL_A,     0xFF,      0x00,  "6PL", "DVSRL "}, // DVSRL - Divisor Lo, start
     {NO_BANK_SWITCH, c_DVDDH_A,     0xFF,      0x00,  "4L ", "DVDDH "}, // DVDDH - Dividend Hi
     {NO_BANK_SWITCH, c_DVDDL_A,     0xFF,      0x00,  "5L ", "DVDDL "}, // DVDDL - Dividend Lo*/
    {0}
}; // end of list

//
// Custom functions implemented for this game.
//
static const CUSTOM_FUNCTION s_customFunction[] PROGMEM = { //                                               "0123456789"
                                                             {CPaclandBaseGame::testClockPulse,              "Clk Pulse "},
    {NO_CUSTOM_FUNCTION}
}; // end of list

CPaclandBaseGame::CPaclandBaseGame(
                                   const ROM_REGION    *romRegion
                                   ) : CGame( romRegion,
                                             s_ramRegion,
                                             s_ramRegionByteOnly,
                                             s_ramRegionWriteOnly,
                                             s_inputRegion,
                                             s_outputRegion,
                                             s_customFunction ),
                                        m_clockPulseCount(0)
{
    m_cpu = new C6809ECpu(8); //8 = number of clocks to wait before clocking in data
    m_cpu->idle();
}


CPaclandBaseGame::~CPaclandBaseGame(
)
{
    delete m_cpu;
    m_cpu = (ICpu *) NULL;
}


// Bank switching functions
// Need to write magic number to specific location to swap in a particular RAM slice
// at a predefined CPU accessible address range.
PERROR
CPaclandBaseGame::onBankSwitchMPAGE_8E_Lo(
                                     void *cPaclandBaseGame
                                     )
{
    CPaclandBaseGame *thisGame  = (CPaclandBaseGame *) cPaclandBaseGame;
    ICpu              *cpu       = thisGame->m_cpu;
    
    return cpu->memoryWrite(c_MPAGE, c_MPAGE_8E_Lo);
}

PERROR
CPaclandBaseGame::onBankSwitchMPAGE_8E_Hi(
                                          void *cPaclandBaseGame
                                          )
{
    CPaclandBaseGame *thisGame  = (CPaclandBaseGame *) cPaclandBaseGame;
    ICpu              *cpu       = thisGame->m_cpu;
    
    return cpu->memoryWrite(c_MPAGE, c_MPAGE_8E_Hi);
}

PERROR
CPaclandBaseGame::onBankSwitchMPAGE_8F_Lo(
                                          void *cPaclandBaseGame
                                          )
{
    CPaclandBaseGame *thisGame  = (CPaclandBaseGame *) cPaclandBaseGame;
    ICpu              *cpu       = thisGame->m_cpu;
    
    return cpu->memoryWrite(c_MPAGE, c_MPAGE_8F_Lo);
}

PERROR
CPaclandBaseGame::onBankSwitchMPAGE_8F_Hi(
                                          void *cPaclandBaseGame
                                          )
{
    CPaclandBaseGame *thisGame  = (CPaclandBaseGame *) cPaclandBaseGame;
    ICpu              *cpu       = thisGame->m_cpu;
    
    return cpu->memoryWrite(c_MPAGE, c_MPAGE_8F_Hi);
}

PERROR
CPaclandBaseGame::onBankSwitchMPAGE_8H_Lo(
                                          void *cPaclandBaseGame
                                          )
{
    CPaclandBaseGame *thisGame  = (CPaclandBaseGame *) cPaclandBaseGame;
    ICpu              *cpu       = thisGame->m_cpu;
    
    return cpu->memoryWrite(c_MPAGE, c_MPAGE_8H_Lo);
}

PERROR
CPaclandBaseGame::onBankSwitchMPAGE_8H_Hi(
                                          void *cPaclandBaseGame
                                          )
{
    CPaclandBaseGame *thisGame  = (CPaclandBaseGame *) cPaclandBaseGame;
    ICpu              *cpu       = thisGame->m_cpu;
    
    return cpu->memoryWrite(c_MPAGE, c_MPAGE_8H_Hi);
}

PERROR
CPaclandBaseGame::onBankSwitchMPAGE_8J_Lo(
                                          void *cPaclandBaseGame
                                          )
{
    CPaclandBaseGame *thisGame  = (CPaclandBaseGame *) cPaclandBaseGame;
    ICpu              *cpu       = thisGame->m_cpu;
    
    return cpu->memoryWrite(c_MPAGE, c_MPAGE_8J_Lo);
}

PERROR
CPaclandBaseGame::onBankSwitchMPAGE_8J_Hi(
                                          void *cPaclandBaseGame
                                          )
{
    CPaclandBaseGame *thisGame  = (CPaclandBaseGame *) cPaclandBaseGame;
    ICpu              *cpu       = thisGame->m_cpu;
    
    return cpu->memoryWrite(c_MPAGE, c_MPAGE_8J_Hi);
}

PERROR
CPaclandBaseGame::testClockPulse(
                                 void   *context
                                 )
{
    CPaclandBaseGame *thisGame = (CPaclandBaseGame *) context;
    C6809ECpu *cpu = (C6809ECpu *) thisGame->m_cpu;
    PERROR error = errorCustom;
    
    cpu->clockPulse();
    thisGame->m_clockPulseCount++;
    
    errorCustom->code = ERROR_SUCCESS;
    errorCustom->description = "OK: Count ";
    errorCustom->description += String(thisGame->m_clockPulseCount, DEC);
    
    return error;
}



// ******** PACLAND MAME
//static INPUT_PORTS_START( pacland )
//PORT_START("DSWA")
//PORT_SERVICE_DIPLOC( 0x80, IP_ACTIVE_LOW, "SWA:1" )
//PORT_DIPNAME( 0x60, 0x60, DEF_STR( Lives ) )        PORT_DIPLOCATION("SWA:3,2")
//PORT_DIPSETTING(    0x40, "2" )
//PORT_DIPSETTING(    0x60, "3" )
//PORT_DIPSETTING(    0x20, "4" )
//PORT_DIPSETTING(    0x00, "5" )
//PORT_DIPNAME( 0x18, 0x18, DEF_STR( Coin_A ) )       PORT_DIPLOCATION("SWA:5,4")
//PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) )
//PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
//PORT_DIPSETTING(    0x18, DEF_STR( 1C_1C ) )
//PORT_DIPSETTING(    0x10, DEF_STR( 1C_2C ) )
//PORT_DIPNAME( 0x04, 0x04, DEF_STR( Demo_Sounds ) )  PORT_DIPLOCATION("SWA:6")
//PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
//PORT_DIPSETTING(    0x04, DEF_STR( On ) )
//PORT_DIPNAME( 0x03, 0x03, DEF_STR( Coin_B ) )       PORT_DIPLOCATION("SWA:8,7")
//PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) )
//PORT_DIPSETTING(    0x01, DEF_STR( 2C_1C ) )
//PORT_DIPSETTING(    0x03, DEF_STR( 1C_1C ) )
//PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
//
//PORT_START("DSWB")
//PORT_DIPNAME( 0xe0, 0xe0, DEF_STR( Bonus_Life ) )   PORT_DIPLOCATION("SWB:3,2,1")
//PORT_DIPSETTING(    0xe0, "30K 80K 130K 300K 500K 1M" )     // "A"
//PORT_DIPSETTING(    0x80, "30K 80K every 100K" )            // "D"
//PORT_DIPSETTING(    0x40, "30K 80K 150K" )                  // "F"
//PORT_DIPSETTING(    0xc0, "30K 100K 200K 400K 600K 1M" )    // "B"
//PORT_DIPSETTING(    0xa0, "40K 100K 180K 300K 500K 1M" )    // "C"
//PORT_DIPSETTING(    0x20, "40K 100K 200K" )                 // "G"
//PORT_DIPSETTING(    0x00, "40K" )                           // "H"
//PORT_DIPSETTING(    0x60, "50K 150K every 200K" )           // "E"
//PORT_DIPNAME( 0x18, 0x18, DEF_STR( Difficulty ) )   PORT_DIPLOCATION("SWB:5,4")
//PORT_DIPSETTING(    0x10, "B (Easy)" )
//PORT_DIPSETTING(    0x18, "A (Average)" )
//PORT_DIPSETTING(    0x08, "C (Hard)" )
//PORT_DIPSETTING(    0x00, "D (Very Hard)" )
//PORT_DIPNAME( 0x04, 0x04, "Round Select" )          PORT_DIPLOCATION("SWB:6")
//PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
//PORT_DIPSETTING(    0x00, DEF_STR( On ) )
//PORT_DIPNAME( 0x02, 0x02, "Freeze" )                PORT_DIPLOCATION("SWB:7")
//PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
//PORT_DIPSETTING(    0x00, DEF_STR( On ) )
//PORT_DIPNAME( 0x01, 0x01, "Trip Select" )           PORT_DIPLOCATION("SWB:8")
//PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
//PORT_DIPSETTING(    0x01, DEF_STR( On ) )
//
//PORT_START("IN0")   /* Memory Mapped Port */
//PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL  PORT_NAME("P2 Right")
//PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SERVICE1 )
//PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN1 )
//PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN2 )
//PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START1 )
//PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_START2 )
//PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SERVICE )    // service mode again
//PORT_DIPNAME( 0x80, 0x80, DEF_STR( Cabinet ) )
//PORT_DIPSETTING(    0x80, DEF_STR( Upright ) )
//PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
//PORT_START("IN1")   /* Memory Mapped Port */
//PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
//PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
//PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )
//PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
//PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED ) // IPT_JOYSTICK_DOWN according to schematics
//PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED ) // IPT_JOYSTICK_UP according to schematics
//PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED ) // IPT_JOYSTICK_DOWN according to schematics
//PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_COCKTAIL   // IPT_JOYSTICK_UP according to schematics
//
//PORT_START("IN2")   /* MCU Input Port */
//PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SPECIAL ) PORT_COCKTAIL  /* OUT:coin lockout */
//PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SPECIAL )    /* OUT:coin counter 1 */
//PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SPECIAL )    /* OUT:coin counter 2 */
//PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P1 Jump")
//PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P1 Left")
//PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P1 Right")
//PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_COCKTAIL PORT_NAME("P2 Jump")
//PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL PORT_NAME("P2 Left")
//INPUT_PORTS_END
//******** END PACLAND MAME
