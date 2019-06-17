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
#include "CAtariAVGBaseGame.h"
#include "C6502Cpu.h"
#include "C6502ClockMasterCpu.h"
#include <DFR_Key.h>
#include "CRomCheck.h"
#include "PinMap.h"
#include "6502PinDescription.h"

//
// RAM region is the same for all versions.
//
static const RAM_REGION s_ramRegion[] PROGMEM = {
    {NO_BANK_SWITCH, 0x0000,      0x00FF,      1, 0x0F, "J2A", "Prog. "}, // Program RAM J2, 2114 - lower 0 page
    {NO_BANK_SWITCH, 0x0100,      0x01FF,      1, 0x0F, "J2Z", "Prog. "}, // Program RAM J2, 2114 - lower 1 page
    {NO_BANK_SWITCH, 0x0200,      0x02FF,      1, 0x0F, "J2O", "Prog. "}, // Program RAM J2, 2114 - lower 2 page
    {NO_BANK_SWITCH, 0x0300,      0x03FF,      1, 0x0F, "J2T", "Prog. "}, // Program RAM J2, 2114 - lower 3 page
    
    {NO_BANK_SWITCH, 0x0000,      0x00FF,      1, 0xF0, "H2A", "Prog. "}, // Program RAM H2, 2114 - upper 0 page
    {NO_BANK_SWITCH, 0x0100,      0x01FF,      1, 0xF0, "H2Z", "Prog. "}, // Program RAM H2, 2114 - upper 1 page
    {NO_BANK_SWITCH, 0x0200,      0x02FF,      1, 0xF0, "H2O", "Prog. "}, // Program RAM H2, 2114 - upper 2 page
    {NO_BANK_SWITCH, 0x0300,      0x03FF,      1, 0xF0, "H2T", "Prog. "}, // Program RAM H2, 2114 - upper 3 page
    
    {NO_BANK_SWITCH, 0x2000,      0x23FF,      1, 0x0F, "A2 ", "Vec.  "}, // Vec RAM, 2114 - lower
    {NO_BANK_SWITCH, 0x2000,      0x23FF,      1, 0xF0, "A1 ", "Vec.  "}, // Vec RAM, 2114 - upper
    {NO_BANK_SWITCH, 0x2400,      0x27FF,      1, 0x0F, "B2 ", "Vec.  "}, // Vec RAM, 2114 - lower
    {NO_BANK_SWITCH, 0x2400,      0x27FF,      1, 0xF0, "B1 ", "Vec.  "}, // Vec RAM, 2114 - upper
    {NO_BANK_SWITCH, 0x2800,      0x2BFF,      1, 0x0F, "C2 ", "Vec.  "}, // Vec RAM, 2114 - lower
    {NO_BANK_SWITCH, 0x2800,      0x2BFF,      1, 0xF0, "C1 ", "Vec.  "}, // Vec RAM, 2114 - upper
    {NO_BANK_SWITCH, 0x2C00,      0x2FFF,      1, 0x0F, "D2 ", "Vec.  "}, // Vec RAM, 2114 - lower
    {NO_BANK_SWITCH, 0x2C00,      0x2FFF,      1, 0xF0, "D1 ", "Vec.  "}, // Vec RAM, 2114 - upper
    {0} // end of list
};


//
// RAM region is the same for all games on this board set.
// This description is used for the byte-wide intensive random access memory test.
//
static const RAM_REGION s_ramRegionByteOnly[] PROGMEM = {
    {NO_BANK_SWITCH, 0x0000,      0x03FF,      1, 0xFF, "2HJ", "Prog. "}, // "Program RAM"
    {NO_BANK_SWITCH, 0x2000,      0x23FF,      1, 0xFF, "A12", "Vec   "}, // "Vec RAM"
    {NO_BANK_SWITCH, 0x2400,      0x27FF,      1, 0xFF, "B12", "Vec   "}, // "Vec RAM"
    {NO_BANK_SWITCH, 0x2800,      0x2BFF,      1, 0xFF, "C12", "Vec   "}, // "Vec RAM"
    {NO_BANK_SWITCH, 0x2C00,      0x2FFF,      1, 0xFF, "D12", "Vec   "}, // "Vec RAM"
    {0} // end of list
};


//
// No write-only RAM
//
static const RAM_REGION s_ramRegionWriteOnly[] PROGMEM = { {0} }; // end of list


//
// Custom functions
//
static const CUSTOM_FUNCTION s_customFunction[] PROGMEM = {
    //                                       "0123456789"
    {CAtariAVGBaseGame::pokeyIdle,           "PKY Idle  "},
    {CAtariAVGBaseGame::pokeySoundTest,      "PKY Sound "},
    {CAtariAVGBaseGame::pokeySwitchTest,     "PKY Switch"},
    {CAtariAVGBaseGame::pokeyRandomTest,     "PKY Random"},
    {NO_CUSTOM_FUNCTION} // end of list
};


CAtariAVGBaseGame::CAtariAVGBaseGame(
    const bool          clockMaster,
    const ROM_REGION    *romRegion,
    const INPUT_REGION  *inputRegion,
    const OUTPUT_REGION *outputRegion,
    const UINT32        pokeyAddress
) : CGame( romRegion,
           s_ramRegion,
           s_ramRegionByteOnly,
           s_ramRegionWriteOnly,
           inputRegion,
           outputRegion,
           s_customFunction )
{
    if (clockMaster)
    {
        m_cpu = new C6502ClockMasterCpu(false);
    }
    else
    {
        m_cpu = new C6502Cpu(false);
    }

    m_cpu->idle();

    // IRQ is not connected on this game
    // NMI is connected to a 250 Hz timer, which is disabled when the game's self-test switch is closed
    m_interrupt = ICpu::NMI;
    m_interruptAutoVector = true;
    
    m_clkPin = new CFastPin(g_pinMap40DIL, &s_CLK2o_o);
    m_pokey = new CPOKEY(m_cpu, pokeyAddress, m_clkPin);
}


CAtariAVGBaseGame::~CAtariAVGBaseGame(
)
{
    delete m_cpu;
    m_cpu = (ICpu *) NULL;

    delete m_pokey;
    m_pokey = (CPOKEY *) NULL;

    delete m_clkPin;
    m_clkPin = (CFastPin *) NULL;
}

//
// Custom interrupt test
//
PERROR
CAtariAVGBaseGame::interruptCheck(
)
{
    PERROR error = errorSuccess;
    UINT16 data16 = 0x0000;
    
    // Only run test if the self-test switch is OFF
    error = m_cpu->memoryRead(0x0800, &data16);
    if (data16 & s_MSK_D4)
    {
        // NMI is connected to a 250 Hz timer,
        // so we just verify that the state is toggling on its own
        for (int i = 0 ; i < 10 ; i++)
        {
            // Wait up to 100ms for interrupt to be active
            error = m_cpu->waitForInterrupt(m_interrupt, true, 100);
            if (SUCCESS(error))
            {
                // Wait for interrupt to be inactive
                error = m_cpu->waitForInterrupt(m_interrupt, false, 100);
            }
            if (FAILED(error))
            {
                break;
            }
        }
    }
    else
    {
        error = errorCustom;
        error->code = ERROR_SUCCESS;
        error->description = "N/A w/test sw ON";
    }

    return error;
}

//
// wrappers for POKEY custom tests
//
PERROR
CAtariAVGBaseGame::pokeyIdle(
                             void *cAtariAVGBaseGame
                             )
{
    CAtariAVGBaseGame *pThis = (CAtariAVGBaseGame *) cAtariAVGBaseGame;
    PERROR error = errorSuccess;
    
    error = pThis->m_pokey->idle();
    return error;
}

PERROR
CAtariAVGBaseGame::pokeySoundTest(
                                  void *cAtariAVGBaseGame
                                  )
{
    CAtariAVGBaseGame *pThis = (CAtariAVGBaseGame *) cAtariAVGBaseGame;
    PERROR error = errorSuccess;
    
    // TO-DO: add sound enable address to game classes instead of hard-coding it here
    error = pThis->m_cpu->memoryWrite(0x1840, 0x20); // enable sound output (turns on 4066 analog switch)
    error = pThis->m_pokey->soundCheck();
    error = pThis->m_cpu->memoryWrite(0x1840, 0x00); // disable sound output
    return error;
}

PERROR
CAtariAVGBaseGame::pokeySwitchTest(
                                   void *cAtariAVGBaseGame
                                   )
{
    CAtariAVGBaseGame *pThis = (CAtariAVGBaseGame *) cAtariAVGBaseGame;
    PERROR error = errorSuccess;
    
    error = pThis->m_pokey->readSwitches();
    return error;
}

PERROR
CAtariAVGBaseGame::pokeyRandomTest(
                                   void *cAtariAVGBaseGame
                                   )
{
    CAtariAVGBaseGame *pThis = (CAtariAVGBaseGame *) cAtariAVGBaseGame;
    PERROR error = errorSuccess;
    
    error = pThis->m_pokey->readRandom();
    return error;
}

