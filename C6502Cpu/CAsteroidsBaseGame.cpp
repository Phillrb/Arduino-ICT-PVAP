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
#include "CAsteroidsBaseGame.h"
#include "C6502Cpu.h"
#include "C6502ClockMasterCpu.h"
#include <DFR_Key.h>
#include "CRomCheck.h"
#include "CAsteroidsGame.h"

//D2 & E2 together cover 0000-03FF - D2 lower E2 upper
//0000-01FF D0 -> D7 Zero and one page RAM - RW
//0200-02FF D0 -> D7 Player 1 RAM - RW
//0300-03FF D0 -> D7 Player 2 RAM - RW
//4000-47FF - VECTOR RAM - RW
//M4 & R4 4000-43FF
//N4 & P4 4400-47FF



//RAM CHECK
//E:E2  0000 90 80

//BUS CHECK
//E:D0 ff 6c

//
// RAM region is the same for all versions.
//
static const RAM_REGION s_ramRegion[] PROGMEM = { //            "012", "012345"
            {NO_BANK_SWITCH, 0x0000,      0x01FF,      1, 0x0F, "D2Z", "Prog. "}, // "Program RAM, 2114 - lower"
            {NO_BANK_SWITCH, 0x0200,      0x02FF,      1, 0x0F, "D2O", "Prog. "}, // "Program RAM, 2114 - lower"
            {NO_BANK_SWITCH, 0x0300,      0x03FF,      1, 0x0F, "D2T", "Prog. "}, // "Program RAM, 2114 - lower"

            {NO_BANK_SWITCH, 0x0000,      0x00FF,      1, 0xF0, "E2A", "Prog. "}, // "Program RAM, 2114 - upper" 0 page
            {NO_BANK_SWITCH, 0x0100,      0x01FF,      1, 0xF0, "E2Z", "Prog. "}, // "Program RAM, 2114 - upper" 1 page
            {NO_BANK_SWITCH, 0x0200,      0x02FF,      1, 0xF0, "E2O", "Prog. "}, // "Program RAM, 2114 - upper" 2 page
            {NO_BANK_SWITCH, 0x0300,      0x03FF,      1, 0xF0, "E2T", "Prog. "}, // "Program RAM, 2114 - upper" 3 page

            {NO_BANK_SWITCH, 0x4000,      0x43FF,      1, 0x0F, "M4 ", "Vec.  "}, // "Vec RAM, 2114 - lower"
            {NO_BANK_SWITCH, 0x4000,      0x43FF,      1, 0xF0, "R4 ", "Vec.  "}, // "Vec RAM, 2114 - upper"
            {NO_BANK_SWITCH, 0x4400,      0x47FF,      1, 0x0F, "N4 ", "Vec.  "}, // "Vec RAM, 2114 - lower"
            {NO_BANK_SWITCH, 0x4400,      0x47FF,      1, 0xF0, "P4 ", "Vec.  "}, // "Vec RAM, 2114 - upper"
             {0}
}; // end of list

//
// RAM region is the same for all games on this board set.
// This description is used for the byte-wide intensive random access memory test.
//
static const RAM_REGION s_ramRegionByteOnly[] PROGMEM = { //                                                  "012", "012345"
                                                          {NO_BANK_SWITCH, 0x0000,      0x03FF,      1, 0xFF, "2DE", "Prog. "}, // "Program RAM, 2114, ROM PCB 2M/2F"
                                                          {NO_BANK_SWITCH, 0x4000,      0x43FF,      1, 0xFF, "4MR", "Vec   "}, // "Vec RAM"
                                                          {NO_BANK_SWITCH, 0x4400,      0x47FF,      1, 0xFF, "4NP", "Vec   "}, // "Vec RAM"
                                                          {0}
                                                        }; // end of list

//
// No write-only RAM on this platform. Yay!
//
static const RAM_REGION s_ramRegionWriteOnly[] PROGMEM = { {0} }; // end of list

//
// Custom functions implemented for this game.
//
static const CUSTOM_FUNCTION s_customFunction[] PROGMEM = {
    //                                         "0123456789"
    {NO_CUSTOM_FUNCTION}
}; // end of list

CAsteroidsBaseGame::CAsteroidsBaseGame(
    const bool          clockMaster,
    const ROM_REGION    *romRegion,
    const INPUT_REGION  *inputRegion,
    const OUTPUT_REGION *outputRegion
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
        m_cpu = new C6502ClockMasterCpu(0);
    }
    else
    {
        m_cpu = new C6502Cpu();
    }

    m_cpu->idle();

    // VBLANK is on the INT pin (==IRQ0).
    m_interrupt = ICpu::IRQ0;

    // The interrupt is based on an internal vector
    m_interruptAutoVector = true;
}


CAsteroidsBaseGame::~CAsteroidsBaseGame(
)
{
    delete m_cpu;
    m_cpu = (ICpu *) NULL;
}
