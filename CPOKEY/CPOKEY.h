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
#ifndef CPOKEY_h
#define CPOKEY_h

#include "ICpu.h"
#include "CFastPin.h"


//
// Implementation for testing Atari C012294 "POKEY" sound and I/O ICs.
// The device is mapped to a base address by the game hardware, and has four
// address bits used to read/write 16 registers.
//
class CPOKEY
{
    public:

        CPOKEY(
            ICpu   *cpu,
            UINT32 regAddress,
            CFastPin *clockPin
        );

        ~CPOKEY(
        );

        PERROR idle(
        );

        PERROR soundCheck(
        );
    
        PERROR readSwitches(
        );
    
        PERROR readRandom(
        );

    private:

        PERROR read(
            UINT8 reg,
            UINT8 *data
        );

        PERROR write(
            UINT8 reg,
            UINT8 data
        );

        PERROR playTone(
            UINT8 freqReg,
            UINT8 ctrlReg,
            UINT8 freqency,
            UINT32 duration
        );
    
    private:

        ICpu    *m_cpu;
        UINT32  m_baseAddress;
        CFastPin *m_clockPin;
};

#endif

