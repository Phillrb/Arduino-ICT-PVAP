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
                   const UINT32        irqResetAddress,
                   const UINT32        earomWriteBaseAddress,
                   const UINT32        earomControlAddress,
                   const UINT32        earomReadAddress,
                   const UINT32        p1StartAddress,
                   const UINT32        p1StartMask
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
    
    // IRQ
    m_interrupt = ICpu::IRQ0;
    m_interruptAutoVector = true;
    m_irqResetAddress = irqResetAddress;
    
    m_earom = new CER2055(m_cpu, earomWriteBaseAddress, earomControlAddress, earomReadAddress, EAROM_C1D1_C2D2);
    m_p1StartAddress = p1StartAddress;
    m_p1StartMask = p1StartMask;
}

CCentipedeBaseGame::~CCentipedeBaseGame(
)
{
    delete m_cpu;
    m_cpu = (ICpu *) NULL;

    delete m_earom;
    m_earom = (CER2055 *) NULL;
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

//
// Returns EAROM to dormant state (shouldn't really be needed)
//
PERROR
CCentipedeBaseGame::earomIdle(
                              void *cCentipedeBaseGame
                              )
{
    CCentipedeBaseGame *pThis = (CCentipedeBaseGame *) cCentipedeBaseGame;
    PERROR error = errorSuccess;
    error = pThis->m_earom->idle();
    return error;
}

//
// Basic EAROM read test
//
PERROR
CCentipedeBaseGame::earomReadTest(
                                  void *cCentipedeBaseGame
                                  )
{
    CCentipedeBaseGame *pThis = (CCentipedeBaseGame *) cCentipedeBaseGame;
    PERROR error = errorSuccess;
    error = pThis->m_earom->readTest();
    return error;
}

//
// Erase all contents of EAROM
// WARNING: Clears all high scores, and uses up 64 of the EAROM's 1 million rated write cycles
//
PERROR
CCentipedeBaseGame::earomErase(
                               void *cCentipedeBaseGame
                               )
{
    CCentipedeBaseGame *pThis = (CCentipedeBaseGame *) cCentipedeBaseGame;
    PERROR error = errorSuccess;
    
    error = pThis->confirmWithP1Start(); // only proceed if player 1 start button is pressed
    if (SUCCESS(error))
    {
        error = pThis->m_earom->erase();
    }
    return error;
}

//
// Dump contents of EAROM to serial port as hex bytes
//
PERROR
CCentipedeBaseGame::earomSerialDump(
                                    void *cCentipedeBaseGame
                                    )
{
    CCentipedeBaseGame *pThis = (CCentipedeBaseGame *) cCentipedeBaseGame;
    PERROR error = errorSuccess;
    error = pThis->m_earom->serialDump();
    return error;
}

//
// Rewrite EAROM from hex bytes on serial port
//
PERROR
CCentipedeBaseGame::earomSerialLoad(
                                void *cCentipedeBaseGame
                                )
{
    CCentipedeBaseGame *pThis = (CCentipedeBaseGame *) cCentipedeBaseGame;
    PERROR error = errorSuccess;
    
    error = pThis->confirmWithP1Start(); // only proceed if player 1 start button is pressed
    if (SUCCESS(error))
    {
        error = pThis->m_earom->serialLoad();
    }
    return error;
}

//
// Require the user to hold down P1 Start when initiating a destructive EAROM operation
//
PERROR
CCentipedeBaseGame::confirmWithP1Start()
{
    PERROR error = errorSuccess;
    UINT16 data16 = 0;
    
    error = this->m_cpu->memoryRead(m_p1StartAddress, &data16);
    if ( FAILED(error) || (data16 & (UINT16)m_p1StartMask)) // active low
    {
        error = errorCustom;
        error->code = ERROR_FAILED;
        error->description = "E:Hold P1START";
    }
    return error;
}
