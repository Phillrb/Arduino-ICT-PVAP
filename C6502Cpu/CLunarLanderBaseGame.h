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
#ifndef CLunarLanderBaseGame_h
#define CLunarLanderBaseGame_h

#include "CGame.h"


class CLunarLanderBaseGame : public CGame
{
    public:
    
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
    
    //Active High
    static const UINT32 s_ACT_Hi = 0x00;

        //
        // CLunarLanderBaseGame
        //
    
    protected:

        CLunarLanderBaseGame(
            const bool          clockMaster,
            const ROM_REGION    *romRegion,
            const INPUT_REGION  *inputRegion,
            const OUTPUT_REGION *outputRegion
        );

        ~CLunarLanderBaseGame(
        );

};

#endif

