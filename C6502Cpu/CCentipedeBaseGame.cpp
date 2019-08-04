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

#include "CCentipedeBaseGame.h"
#include "C6502Cpu.h"
#include "C6502ClockMasterCpu.h"
#include <DFR_Key.h>
#include "CRomCheck.h"

CCentipedeBaseGame::CCentipedeBaseGame(
                   const bool          clockMaster,
                   const ROM_REGION    *romRegion,
                   const RAM_REGION    *ramRegion,
                   const RAM_REGION    *ramRegionByteOnly,
                   const RAM_REGION    *ramRegionWriteOnly,
                   const INPUT_REGION  *inputRegion,
                   const OUTPUT_REGION *outputRegion,
                   const CUSTOM_FUNCTION *customFunction,
                   const UINT32        irqResetAddress
)  : CGame( romRegion,
            ramRegion,
            ramRegionByteOnly,
            ramRegionWriteOnly,
            inputRegion,
            outputRegion,
            customFunction
)
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
    
    // NMI not used
    // IRQ
    m_interrupt = ICpu::IRQ0;
    m_interruptAutoVector = true;
    m_irqResetAddress = irqResetAddress;
}

CCentipedeBaseGame::~CCentipedeBaseGame(
)
{
    delete m_cpu;
    m_cpu = (ICpu *) NULL;
}

//
// Interrupt Tests
//
// NMI
//      not used
// IRQ
//      fires every 32 scanlines (about 2ms)
//      cleared by writing any value to the IRQRES address (0x1800 for Centipede, 0x2600 for Millipede)
PERROR
CCentipedeBaseGame::interruptCheck(
)
{
    UINT16 maxLoops = 10;
    PERROR error = errorSuccess;
    
    // repeat test a few times
    for (UINT16 i = 0 ; i < maxLoops ; i++)
    {
        // Wait up to 10ms for IRQ to be active
        error = m_cpu->waitForInterrupt(m_interrupt, true, 10);
        if (SUCCESS(error))
        {
            // clear the interrupt
            error = m_cpu->memoryWrite(m_irqResetAddress, 0x00);
            if (SUCCESS(error))
            {
                // Wait just 1ms for interrupt to be inactive
                error = m_cpu->waitForInterrupt(m_interrupt, false, 1);
            }
        }
        if (FAILED(error))
        {
            break;
        }
    }
    return error;
}
