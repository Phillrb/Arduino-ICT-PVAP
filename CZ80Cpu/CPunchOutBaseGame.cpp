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
#include "CPunchOutBaseGame.h"
#include "CZ80Cpu.h"
#include <DFR_Key.h>

//
// Probe Head GND:
//   Z80 GND Pin 29
//
// Watchdog Disable:
//   LS161 at 2A drives the watchdog?


// c000-c3ff NVRAM
// 1100 0000 0000 0000 - 0xC000 to 0xC3FF - TC5514AP - 1K (4 bit) NVRAM - 8M
// Essentially a low-power 2114 that can hold its values by means of an external battery

// d000-dfff RAM 4K
// 1101 0000 0000 0000 - 0xD000 to 0xDFFF - TC5533P-A - 4K Byte RAM - 8D
// Can be replaced with D4364C, uPD4364 or 6264 

// e000-ffff RAM 8K 
// 1110 0000 0000 0000 - 0xE000 to 0xFFFF - HM6264P-15 - 8K Byte RAM - 8C


// NOT REACHABLE BY CPU:
// Support VLM5030 at 6M:
// - 27128 ROM - 16k ROM - 6P - 0x0000-0x3FFF
// Support 2A03 at 4H:
// - 2764 ROM - 8k ROM - 4K - Audio CPU - 0xE000-0xFFFF
// - 6116 - 2K RAM - 4L - 0x0000-0x07FF

//
// RAM region is the same for all games on this board set.
//
static const RAM_REGION s_ramRegion[] PROGMEM = {   //"012", "012345"
            {NO_BANK_SWITCH, 0xC000, 0xC3FF, 1, 0x0F, " 8M", "NVRAM "}, // TC5514AP - 1K (4 bit) NVRAM
    {0}
}; // end of list

//
// RAM region is the same for all games on this board set.
//
static const RAM_REGION s_ramRegionByteOnly[] PROGMEM = {   //"012", "012345"                                 
                    {NO_BANK_SWITCH, 0xD000, 0xDFFF, 1, 0xFF, " 8D", "Video."}, // TC5533P-A - 4K Byte RAM - 8D
                    {NO_BANK_SWITCH, 0xE000, 0xFFFF, 1, 0xFF, " 8C", "Video."}, // HM6264P-15 - 8K Byte RAM - 8C
                    {0}
                }; // end of list

//
// No write-only RAM on this platform. Yay!
//
static const RAM_REGION s_ramRegionWriteOnly[] PROGMEM = { {0} }; // end of list

//
// Input region is the same for all games on this board set.
//
static const INPUT_REGION s_inputRegion[] PROGMEM = { 
                                                      {0}
                                                    }; // end of list

//
// Output region is the same for all versions on this board set.
//
static const OUTPUT_REGION s_outputRegion[] PROGMEM = {
                                                        {0}
                                                      }; // end of list

//
// Custom functions implemented for this game.
//
static const CUSTOM_FUNCTION s_customFunction[] PROGMEM = { //                                    "0123456789"
                                                    
                                                            {NO_CUSTOM_FUNCTION}}; // end of list


CPunchOutBaseGame::CPunchOutBaseGame(
    const ROM_REGION    *romRegion
) : CGame( romRegion,
           s_ramRegion,
           s_ramRegionByteOnly,
           s_ramRegionWriteOnly,
           s_inputRegion,
           s_outputRegion,
           s_customFunction)
{
    m_cpu = new CZ80Cpu();
    m_cpu->idle();

    // The VBLANK interrupt is on the NMI pin.
    m_interrupt = ICpu::NMI;

    // There is no direct hardware response of a vector on this platform.
    m_interruptAutoVector = true;

}


CPunchOutBaseGame::~CPunchOutBaseGame(
)
{
    delete m_cpu;
    m_cpu = (ICpu *) NULL;
}


// This is a specific implementation for Galaxian that uses an externally
// maskable/resetable latch for the VBLANK interrupt on the NMI line.
PERROR
CPunchOutBaseGame::interruptCheck(
)
{
    PERROR error = errorSuccess;

    errorCustom->code = ERROR_SUCCESS;
    errorCustom->description = "OK:";

    // for (int i = 0 ; i < 4 ; i++)
    // {
    //     // Unmask the interrupt.
    //     m_cpu->memoryWrite(0x7001L, 0x01);

    //     error = m_cpu->waitForInterrupt(m_interrupt,
    //                                     true,
    //                                     1000);
    //     if (FAILED(error))
    //     {
    //         break;
    //     }

    //     // Mask the interrupt (also resets the latch)
    //     m_cpu->memoryWrite(0x7001L, 0x00);

    //     error = m_cpu->waitForInterrupt(m_interrupt,
    //                                     true,
    //                                     0);
    //     if (SUCCESS(error))
    //     {
    //         error = errorUnexpected;
    //         break;
    //     }
    //     else
    //     {
    //         error = errorSuccess;
    //     }

    //     // Unmask the interrupt. Irq should have been cleared.
    //     m_cpu->memoryWrite(0x7001L, 0x01);

    //     error = m_cpu->waitForInterrupt(m_interrupt,
    //                                     true,
    //                                     0);
    //     if (SUCCESS(error))
    //     {
    //         error = errorUnexpected;
    //         break;
    //     }
    //     else
    //     {
    //         error = errorSuccess;
    //     }

    //     // Mask the interrupt (also resets the latch)
    //     m_cpu->memoryWrite(0x7001L, 0x00);

    //     error = m_cpu->waitForInterrupt(m_interrupt,
    //                                     true,
    //                                     1000);
    //     if (SUCCESS(error))
    //     {
    //         error = errorUnexpected;
    //         break;
    //     }
    //     else
    //     {
    //         error = errorSuccess;
    //     }
    // }

    return error;
}
