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
#ifndef CASpaceDuelBaseGame_h
#define CASpaceDuelBaseGame_h

#include "CGame.h"
#include "CER2055.h"

class CSpaceDuelBaseGame : public CGame
{
    public:
    
        PERROR interruptCheck();

        //
        // custom tests for the EAROM
        //
        static PERROR earomIdle( void *cSpaceDuelGame );
        static PERROR earomReadTest( void *cSpaceDuelGame );
        static PERROR earomErase( void *cSpaceDuelGame );
        static PERROR earomSerialDump( void *cSpaceDuelGame );
        static PERROR earomSerialLoad( void *cSpaceDuelGame );
    
        //Bit masks - used to test inputs etc by masking / selecting specific bits of data
        static const UINT32 s_MSK_D0 = 0x01;
        static const UINT32 s_MSK_D1 = 0x02;
        static const UINT32 s_MSK_D2 = 0x04;
        static const UINT32 s_MSK_D3 = 0x08;
        static const UINT32 s_MSK_D4 = 0x10;
        static const UINT32 s_MSK_D5 = 0x20;
        static const UINT32 s_MSK_D6 = 0x40;
        static const UINT32 s_MSK_D7 = 0x80;
    
        static const UINT32 s_MSK_D0_TO_D3 = 0x0F;
        static const UINT32 s_MSK_D2_TO_D5 = 0x3C;
        static const UINT32 s_MSK_D6_D7 = 0xC0;
    
        static const UINT32 s_MSK_ALL = 0xFF;
    
        static const UINT32 s_ACT_Hi = 0x00; //Active High
        static const UINT32 s_ACT_Lo = 0x01; //Active Low
   
    protected:

        CSpaceDuelBaseGame(
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
        );

        ~CSpaceDuelBaseGame(
        );

        UINT16 m_irqResetAddress;
        UINT16 m_confirmAddress;
        UINT8  m_confirmMask;
        bool   m_confirmActiveLow;
        CER2055 *m_earom;

};

#endif

