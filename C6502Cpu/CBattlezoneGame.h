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
#ifndef CBattlezoneGame_h
#define CBattlezoneGame_h

#include "CAtariAVGBaseGame.h"

class CBattlezoneGame : public CAtariAVGBaseGame
{
    public:

        // active low/high mask for 74LS273 at M5 (controls sound hardware and start LED)
        static const UINT32 s_M5_INV_MASK = 0x40; // all pins are active high except bit 6 (start LED)

        //
        // Constructors for this game.
        //
        static IGame* createInstanceSet2();
        static IGame* createInstanceClockMasterSet2();
        static IGame* createInstanceSet1();
        static IGame* createInstanceClockMasterSet1();

        //
        // IGame Interface - wholly implemented in the Base game.
        //

    private:

        //
        // Different ROM sets supplied.
        //
        CBattlezoneGame(
            const bool       clockMaster,
            const ROM_REGION *romRegion
        );

};

#endif

