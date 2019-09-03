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
#ifndef CER2055_h
#define CER2055_h

#include "ICpu.h"

// Mapping of the C1/C2 control pins to D1/D2 of the CPU bus is game-dependent
#define EAROM_C1D1_C2D2 0 // Centipede, Dig Dug, Millipede
#define EAROM_C1D2_C2D1 1 // Asteroids Deluxe, Black Widow, Gravitar, Liberator, Red Baron Space Duel, Tempest

class CER2055
{
    public:

        CER2055(
                      ICpu   *cpu,
                      UINT16 writeBaseAddress,
                      UINT16 controlAddress,
                      UINT16 readAddress,
                      UINT8  C1C2mapping
        );

        ~CER2055(
        );

        PERROR idle(
        );

        PERROR read(
                    UINT8 offset,
                    UINT8 *data
                    );
    
        PERROR write(
                     UINT8 offset,
                     UINT8 data
        );

        PERROR readTest(
        );

        PERROR erase(
        );
 
        PERROR serialDump(
        );
 
        PERROR serialLoad(
        );

     private:

        ICpu    *m_cpu;
        UINT16  m_writeBaseAddress;
        UINT16  m_controlAddress;
        UINT16  m_readAddress;
        UINT8   m_CTL_WRITE;
};

#endif

