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
#ifndef CAsteroidsDeluxeGame_h
#define CAsteroidsDeluxeGame_h
#include "CAsteroidsBaseGame.h"
#include "CER2055.h"

class CAsteroidsDeluxeGame : public CAsteroidsBaseGame
{
    public:

        //
        // Constructors for this game.
        //

        static IGame* createInstanceSet3();
        static IGame* createInstanceClockMasterSet3();
        static IGame* createInstanceSet2();
        static IGame* createInstanceClockMasterSet2();
        static IGame* createInstanceSet1();
        static IGame* createInstanceClockMasterSet1();

        //
        // custom tests for the EAROM
        //
        static PERROR earomReadTest(void *cCentipedeGame);
        static PERROR earomErase(void *cCentipedeGame);
        static PERROR earomSerialDump(void *cCentipedeGame);
        static PERROR earomSerialLoad(void *cCentipedeGame);
        PERROR confirmDestructiveOperation();
    
        //
        // IGame Interface - wholly implemented in the Base game.
        //

    private:

        //
        // Different ROM sets supplied.
        //
        CAsteroidsDeluxeGame(
            const bool       clockMaster,
            const ROM_REGION *romRegion
        );
 
        ~CAsteroidsDeluxeGame(
        );

        PERROR confirmWithP1Start(
        );

        CER2055 *m_earom;
};

#endif


