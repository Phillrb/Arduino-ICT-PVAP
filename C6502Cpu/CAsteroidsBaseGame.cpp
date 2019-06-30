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



CAsteroidsBaseGame::CAsteroidsBaseGame(
    const bool          clockMaster,
    const ROM_REGION    *romRegion,
    const RAM_REGION    *ramRegion,
    const RAM_REGION    *ramRegionByteOnly,
    const RAM_REGION    *ramRegionWriteOnly,
    const INPUT_REGION  *inputRegion,
    const OUTPUT_REGION *outputRegion,
    const CUSTOM_FUNCTION *customFunction
) : CGame( romRegion,
           ramRegion,
           ramRegionByteOnly,
           ramRegionWriteOnly,
           inputRegion,
           outputRegion,
           customFunction )
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
