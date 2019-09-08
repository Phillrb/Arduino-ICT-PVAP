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
#include "CSpaceDuelBaseGame.h"
#include "C6502Cpu.h"
#include "C6502ClockMasterCpu.h"

//
// Custom functions
//
static const CUSTOM_FUNCTION s_customFunction[] PROGMEM = {
    //                                    "0123456789"
    //    {CSpaceDuelBaseGame::earomIdle,       "EAROM Idle"},
    {CSpaceDuelBaseGame::earomReadTest,   "EAROM Read"},
    {CSpaceDuelBaseGame::earomSerialDump, "EAROM Dump"},
    {CSpaceDuelBaseGame::earomSerialLoad, "EAROM Load"},
    {CSpaceDuelBaseGame::earomErase,      "EAROM Wipe"},
    {NO_CUSTOM_FUNCTION} // end of list
};

CSpaceDuelBaseGame::CSpaceDuelBaseGame(
                                       const bool          clockMaster,
                                       const ROM_REGION    *romRegion,
                                       const RAM_REGION    *ramRegion,
                                       const RAM_REGION    *ramRegionByteOnly,
                                       const RAM_REGION    *ramRegionWriteOnly,
                                       const INPUT_REGION  *inputRegion,
                                       const OUTPUT_REGION *outputRegion,
                                       const UINT16        irqResetAddress,
                                       const UINT16        pokey0Address,
                                       const UINT16        pokey1Address,
                                       const UINT16        earomWriteBaseAddress,
                                       const UINT16        earomControlAddress,
                                       const UINT16        earomReadAddress,
                                       const UINT16        confirmAddress,
                                       const UINT8         confirmMask,
                                       const bool          confirmActiveLow
) : CGame( romRegion,
           ramRegion,
           ramRegionByteOnly,
           ramRegionWriteOnly,
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
    
    m_interrupt = ICpu::IRQ0;
    m_irqResetAddress = irqResetAddress;
    m_interruptAutoVector = true; // 6502 interrupt is based on an internal vector
    
    m_earom = new CER2055(m_cpu, earomWriteBaseAddress, earomControlAddress, earomReadAddress, EAROM_C1D2_C2D1);
    m_confirmAddress = confirmAddress;
    m_confirmMask = confirmMask;
    m_confirmActiveLow = confirmActiveLow;
}


CSpaceDuelBaseGame::~CSpaceDuelBaseGame(
)
{
    delete m_cpu;
    m_cpu = (ICpu *) NULL;
    
    delete m_earom;
    m_earom = (CER2055 *) NULL;
}

//
// Interrupt Test
//
// NMI - not used
// IRQ - fires every 4(?) ms; cleared by writing any value to the IRQRES address
//
PERROR
CSpaceDuelBaseGame::interruptCheck(
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
CSpaceDuelBaseGame::earomIdle(
                              void *cSpaceDuelBaseGame
                              )
{
    CSpaceDuelBaseGame *pThis = (CSpaceDuelBaseGame *) cSpaceDuelBaseGame;
    PERROR error = errorSuccess;
    error = pThis->m_earom->idle();
    return error;
}

//
// Basic EAROM read test
//
PERROR
CSpaceDuelBaseGame::earomReadTest(
                                  void *cSpaceDuelBaseGame
                                  )
{
    CSpaceDuelBaseGame *pThis = (CSpaceDuelBaseGame *) cSpaceDuelBaseGame;
    PERROR error = errorSuccess;
    error = pThis->m_earom->readTest();
    return error;
}

//
// Erase all contents of EAROM
// WARNING: Clears all high scores, and uses up 64 of the EAROM's 1 million rated write cycles
//
PERROR
CSpaceDuelBaseGame::earomErase(
                               void *cSpaceDuelBaseGame
                               )
{
    CSpaceDuelBaseGame *pThis = (CSpaceDuelBaseGame *) cSpaceDuelBaseGame;
    PERROR error = errorSuccess;
    
    // only proceed if start button is pressed
    error = pThis->m_earom->confirmOperation( pThis->m_confirmAddress,
                                              pThis->m_confirmMask,
                                              pThis->m_confirmActiveLow,
                                              (String *) NULL );
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
CSpaceDuelBaseGame::earomSerialDump(
                                    void *cSpaceDuelBaseGame
                                    )
{
    CSpaceDuelBaseGame *pThis = (CSpaceDuelBaseGame *) cSpaceDuelBaseGame;
    PERROR error = errorSuccess;
    error = pThis->m_earom->serialDump();
    return error;
}

//
// Rewrite EAROM from hex bytes on serial port
//
PERROR
CSpaceDuelBaseGame::earomSerialLoad(
                                    void *cSpaceDuelBaseGame
                                    )
{
    CSpaceDuelBaseGame *pThis = (CSpaceDuelBaseGame *) cSpaceDuelBaseGame;
    PERROR error = errorSuccess;
    
    // only proceed if start button is pressed
    error = pThis->m_earom->confirmOperation( pThis->m_confirmAddress,
                                              pThis->m_confirmMask,
                                              pThis->m_confirmActiveLow,
                                              (String *) NULL );
    if (SUCCESS(error))
    {
        error = pThis->m_earom->serialLoad();
    }
    return error;
}
