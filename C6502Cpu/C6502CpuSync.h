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

#ifndef C6502CpuSync_h
#define C6502CpuSync_h

#include "C6502Cpu.h"

#ifdef ARDUINO_AVR_MEGA2560
#define SYNCED_6502
#endif

typedef enum {
    RS_DEFAULT = 0,
    RS_SYNC_PHI0_1_5_MHZ,
    RS_SYNC_PHI0_SKIP_1_PULSE_1_5_MHZ
} readStrategy_t;

typedef enum {
    WS_DEFAULT = 0,
    WS_SYNC_PHI0_1_5_MHZ,
    WS_SYNC_PHI0_SKIP_1_PULSE_1_5_MHZ
} writeStrategy_t;

typedef struct _MEM_STRATEGY {
    const UINT16 startAddress;
    const UINT16 length;
    const readStrategy_t readStrategy;
    const writeStrategy_t writeStrategy;
} MEM_STRATEGY, *PMEM_STRATEGY;


class C6502CpuSync : public C6502Cpu
{
    public:
    
        //
        // Constructor
        //
        C6502CpuSync(
                 bool dataBusCheck,
                 const MEM_STRATEGY *memStrategy
                 );

        //
        // Override for standard memory read
        //
        virtual
        PERROR
        memoryRead(
                   UINT32 address,
                   UINT16 *pData
                   );

        //
        // Override for standard memory write
        //
        virtual
        PERROR
        memoryWrite(
                    UINT32 address,
                    UINT16 data
                    );

    private:

        PERROR
        memRead_SyncPhi0_Skip1Pulse_1_5_Mhz(
                                    UINT16 readAddress16,
                                    UINT16 *pData
                                    );

        PERROR
        memWrite_SyncPhi0_Skip1Pulse_1_5_Mhz(
                                    UINT16 writeAddress16,
                                    UINT16 data16
                                    );

        const MEM_STRATEGY *m_memStrategy;
};


#endif

