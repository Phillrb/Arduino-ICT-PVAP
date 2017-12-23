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
#ifndef CPaclandGame_h
#define CPaclandGame_h

#include "CPaclandBaseGame.h"

class CPaclandGame : public CPaclandBaseGame
{
public:
    
    //
    // Constructors for this game.
    //
    
    static IGame* createInstance();     //Default set
    static IGame* createInstance_J();   //Japan set
    static IGame* createInstance_Jo();  //Japan old set
    static IGame* createInstance_Jo2(); //Another Japan old set
    static IGame* createInstance_M();   //Midway / Bally set

    //Highscore mod - no soft reset
    static IGame* createInstance_Hi();
    static IGame* createInstance_J_Hi();
    static IGame* createInstance_Jo_Hi();
    static IGame* createInstance_Jo2_Hi();
    static IGame* createInstance_M_Hi();
    
    //Highscore mode - includes reset
    static IGame* createInstance_Hi_Rst();
    static IGame* createInstance_J_Hi_Rst();
    static IGame* createInstance_Jo_Hi_Rst();
    static IGame* createInstance_Jo2_Hi_Rst();
    static IGame* createInstance_M_Hi_Rst();
    
private:
    
    //
    // Different ROM sets supplied.
    //
    CPaclandGame(
    const ROM_REGION *romRegion
    );
    
};

#endif

