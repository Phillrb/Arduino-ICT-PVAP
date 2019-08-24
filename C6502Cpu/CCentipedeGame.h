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

#ifndef CCentipedeGame_h
#define CCentipedeGame_h

#include "CCentipedeBaseGame.h"

class CCentipedeGame : public CCentipedeBaseGame
{
public:
    
    //
    // Constructors for this game.
    //
    static IGame* createInstanceSet4();
    static IGame* createInstanceClockMasterSet4();
    static IGame* createInstanceSet3();
    static IGame* createInstanceClockMasterSet3();
    static IGame* createInstanceSet2();
    static IGame* createInstanceClockMasterSet2();
    static IGame* createInstanceSet1();
    static IGame* createInstanceClockMasterSet1();

    static PERROR videoTest(
                            void *cCentipedeGame
                            );

private:
    
    //
    // Different ROM sets supplied.
    //
    CCentipedeGame(
                    const bool       clockMaster,
                    const ROM_REGION *romRegion
                    );

};

#endif
