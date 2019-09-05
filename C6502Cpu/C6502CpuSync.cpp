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

#include "C6502CpuSync.h"
#include "PinMap.h"
#include "6502PinDescription.h"

// TO-DO: try making this a class member instead
volatile byte g_dataBusVal = 0x00;  // used by ASM routines (global so compiler won't use the stack or a register)

// CPU pins are 26-33 (D7-D0)
// function    CPU pin     Arduino     ATmega2560
// D7          26          43          PL6
// D6          27          41          PG0
// D5          28          39          PG2
// D4          29          37          PC0
// D3          30          35          PC2
// D2          31          33          PC4
// D1          32          31          PC6
// D0          33          29          PA7

//
// Constructor
//
C6502CpuSync::C6502CpuSync(
                 bool dataBusCheck,
                 const MEM_STRATEGY *memStrategy
                ) : C6502Cpu(dataBusCheck)
{
    m_memStrategy = memStrategy;
}

//
// Override of standard C6502Cpu memory read
//
PERROR C6502CpuSync::memoryRead(
           UINT32 address,
           UINT16 *pData
           )
{
    PERROR error = errorSuccess;
    readStrategy_t readStrategy = RS_DEFAULT;
    UINT8 stratIdx = 0;
    while (m_memStrategy[stratIdx].length > 0)
    {
        if ( ((UINT16)address >= m_memStrategy[stratIdx].startAddress)
            && ((UINT16)address < m_memStrategy[stratIdx].startAddress + m_memStrategy[stratIdx].length) )
        {
            readStrategy = m_memStrategy[stratIdx].readStrategy;
            break;
        }
        stratIdx++;
    }
    
    switch (readStrategy)
    {
        case RS_DEFAULT:
            error = C6502Cpu::memoryRead(address, pData); // use asynchronous method in base class
            break;
        case RS_SYNC_PHI0_1_5_MHZ:
        case RS_SYNC_PHI0_SKIP_1_PULSE_1_5_MHZ:
            error = memRead_SyncPhi0_Skip1Pulse_1_5_Mhz((UINT16)address, pData);
            break;
        default:
            error = errorNotImplemented;
            break;
    }
    return error; // errorNotImplemented;
}

//
// Override of standard C6502Cpu memory write
//
PERROR C6502CpuSync::memoryWrite(
            UINT32 address,
            UINT16 data
            )
{
    PERROR error = errorSuccess;
    writeStrategy_t writeStrategy = WS_DEFAULT;
    UINT8 stratIdx = 0;
    while (m_memStrategy[stratIdx].length > 0)
    {
        if ( ((UINT16)address >= m_memStrategy[stratIdx].startAddress)
            && ((UINT16)address < m_memStrategy[stratIdx].startAddress + m_memStrategy[stratIdx].length) )
        {
            writeStrategy = m_memStrategy[stratIdx].writeStrategy;
            break;
        }
        stratIdx++;
    }
    
    switch (writeStrategy)
    {
        case WS_DEFAULT:
            error = C6502Cpu::memoryWrite(address, data); // use asynchronous method in base class
            break;
        case WS_SYNC_PHI0_1_5_MHZ:
        case WS_SYNC_PHI0_SKIP_1_PULSE_1_5_MHZ:
            error = memWrite_SyncPhi0_Skip1Pulse_1_5_Mhz((UINT16)address, data);
            break;
        default:
            error = errorNotImplemented;
            break;
    }
    return error; // errorNotImplemented;
}



//
// These functions only work on the Mega2560, due to I/O pin mapping and cycle-timed ASM code
//
#ifdef ARDUINO_AVR_MEGA2560

//
// Read strategy that assumes every other phi0 pulse is missing
// (Centipede video RAM)
//
PERROR C6502CpuSync::memRead_SyncPhi0_Skip1Pulse_1_5_Mhz(
                                                     UINT16 readAddress16,
                                                     UINT16 *pData
                                                     )
{
    PERROR error = errorSuccess;
    
    // Set a read cycle.
    digitalWrite(g_pinMap40DIL[s_R_W_o.pin], HIGH);
    
    // Enable the address bus and set the value (the lower 16 bits only)
    m_busA.pinMode(OUTPUT);
    m_busA.digitalWrite(readAddress16);
    
    // Set the databus to input.
    m_busD.pinMode(INPUT);

    // Syncs to phi0, waits one phi0 clock cycle, then sends phi2 pulse
    // (the wait is needed for Centipede; phi0 skips every other pulse when the CPU is accessing video RAM)

    // TO-DO: add a loop with timeout to verify that phi0 clock is active
    // without this, the ICT hangs if there is no phi0 clock coming from the game PCB

    asm(
        "INIT%=:          CLI                               \n"  // global interrupts disable
        "                                                   \n"
        "                 LDS   r24, %[phi2Port]            \n"  // get current state of port with phi2 pin
        "                 MOV   r25, r24                    \n"  // copy
        "                 ANDI  r24, ~(1 << %[phi2Pin])     \n"  // r24 stores port state for phi2 low
        "                 ORI   r25, 1 << %[phi2Pin]        \n"  // r25 stores port state for phi2 high
        "                                                   \n"
        
        // establish rough synchronization with the 6502 phi0 clock
        "WAIT_PHI0_LOW%=: SBIC  %[phi0Port], %[phi0Pin]     \n"  // skip next instruction if phi0 is LOW
        "                 RJMP  WAIT_PHI0_LOW%=             \n"  // repeat
        "                                                   \n"
        "WAIT_PHI0_HIGH%=: SBIS  %[phi0Port], %[phi0Pin]    \n"  // skip next instruction if phi0 is HIGH
        "                 RJMP  WAIT_PHI0_HIGH%=            \n"  // repeat
        "                                                   \n"
        "                 RJMP  SHORTSYNC%=                 \n"  // shorter wait to make sure we end up in the low phase
        "                                                   \n"

        // The SYNC/CHECK loop is 11 AVR cycles long, which is 20ns more than one clock cycle of the 1.5 MHz 6502
        // We start the first pass at SHORTSYNC, to make sure phi0 is low on the first CHECK
        // Each loop gets 20ns closer to phi0 high. When CHECK is true, we are synchronized to within 20ns of the
        // rising edge of phi0. Then we idle until we reach the expected start of the next phi0 pulse, and fire phi2 to match it
        "SYNC%=:          NOP \n" // attempt to sync with the next phi0 high phase
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "SHORTSYNC%=:     NOP \n" // shorter wait for first pass, to make sure we end up in the low phase
        "                 NOP \n"
        "                 NOP \n"
        " \n"
        "CHECK%=:         SBIS  %[phi0Port], %[phi0Pin]     \n"
        "                 RJMP  SYNC%=                      \n" // didn't quite reach phi0 high; try again
        " \n"
        "WAIT_ONE_PHI0%=: NOP                               \n" // cycles 3-10
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        // phi0 normally goes high here, but we are assuming every other phi0 pulse is skipped, so we wait another 6502 cycle
        "                 NOP \n"                              // cycles 1-9
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        // phi0 is about to go high again
        "PHI2_GO%=:       STS   %[phi2Port], r25            \n" // set phi2 high (cycles 10-11)
        "                 NOP                               \n" // 1 cycle
        "                 STS   %[phi2Port], r24            \n" // set phi2 low (2 cycles), 2 cycles early so we have more time read the data bus
        "                 IN    r20, %[pinA]                \n" // store port A (1 cycle)
        "                 IN    r21, %[pinC]                \n" // store port C (1 cycle)
        "                 IN    r22, %[pinG]                \n" // store port G (1 cycle)
        "                 LDS   r23, %[pinL]                \n" // store port L (2 cycles)

        // bit twiddling to reassemble the 8 bits of the CPU data bus
        "COMBINEDATABUS%=: CLR    r25                       \n"  // clear register r25 to use as output value
        "                  SBRC   r23, 6                    \n"  // skip D7 if PL6 was cleared
        "                  ORI    r25, 0x80                 \n"  // set D7
        "                  SBRC   r22, 0                    \n"  // skip D6 if PG0 was cleared
        "                  ORI    r25, 0x40                 \n"  // set D6
        "                  SBRC   r22, 2                    \n"  // skip D5 if PG2 was cleared
        "                  ORI    r25, 0x20                 \n"  // set D5
        "                  SBRC   r21, 0                    \n"  // skip D4 if PC0 was cleared
        "                  ORI    r25, 0x10                 \n"  // set D4
        "                  SBRC   r21, 2                    \n"  // skip D3 if PC2 was cleared
        "                  ORI    r25, 0x08                 \n"  // set D3
        "                  SBRC   r21, 4                    \n"  // skip D2 if PC4 was cleared
        "                  ORI    r25, 0x04                 \n"  // set D2
        "                  SBRC   r21, 6                    \n"  // skip D1 if PC6 was cleared
        "                  ORI    r25, 0x02                 \n"  // set D1
        "                  SBRC   r20, 7                    \n"  // skip D0 if PA7 was cleared
        "                  ORI    r25, 0x01                 \n"  // set DO
        "                  STS   (g_dataBusVal), r25        \n"  // pass output value to main program variable
        
        "FINALIZE%=:       SEI                              \n"  // global interrupts enable
        
        :
        : [phi0Port] "I" (_SFR_IO_ADDR(PIND)),
        [phi0Pin] "I" (PIND0),
        [phi2Port] "i" (_SFR_MEM_ADDR(PORTH)),
        [phi2Pin] "I" (PINH0),
        [pinA] "I" (_SFR_IO_ADDR(PINA)),
        [pinC] "I" (_SFR_IO_ADDR(PINC)),
        [pinG] "I" (_SFR_IO_ADDR(PING)),
        [pinL] "i" (_SFR_MEM_ADDR(PINL))
        : "r24", "r25", "r20", "r21", "r22", "r23"
        );
    
    *pData = (UINT16) g_dataBusVal;
    
    return error;
}

//
// Write strategy that assumes every other phi0 pulse is missing
// (Centipede video RAM)
//
PERROR C6502CpuSync::memWrite_SyncPhi0_Skip1Pulse_1_5_Mhz(
                                                     UINT16 writeAddress16,
                                                     UINT16 data16
                                                     )
{
    PERROR error = errorSuccess;
    
    // Set a write cycle.
    digitalWrite(g_pinMap40DIL[s_R_W_o.pin], LOW);
    
    // Enable the address bus and set the value.
    m_busA.pinMode(OUTPUT);
    m_busA.digitalWrite(writeAddress16);
    
    // Set the databus to output and set a value.
    m_busD.pinMode(OUTPUT);
    m_busD.digitalWrite(data16);
    
    // TO-DO: add a loop with timeout to verify that phi0 clock is active
    
    asm(
        "INIT%=:          CLI                               \n"  // global interrupts disable
        "                                                   \n"
        "                 LDS   r24, %[phi2Port]            \n"  // get current state of port with phi2 pin
        "                 MOV   r25, r24                    \n"  // copy
        "                 ANDI  r24, ~(1 << %[phi2Pin])     \n"  // r24 stores port state for phi2 low
        "                 ORI   r25, 1 << %[phi2Pin]        \n"  // r25 stores port state for phi2 high
        "                                                   \n"

        // establish rough synchronization with the 6502 phi0 clock
        "WAIT_PHI0_LOW%=: SBIC  %[phi0Port], %[phi0Pin]     \n"  // skip next instruction if phi0 is LOW
        "                 RJMP  WAIT_PHI0_LOW%=             \n"  // repeat
        "                                                   \n"
        "WAIT_PHI0_HIGH%=: SBIS  %[phi0Port], %[phi0Pin]    \n"  // skip next instruction if phi0 is HIGH
        "                 RJMP  WAIT_PHI0_HIGH%=            \n"  // repeat
        "                                                   \n"
        "                 RJMP  SHORTSYNC%=                 \n"  // shorter wait to make sure we end up in the low phase
        "                                                   \n"

        // The SYNC/CHECK loop to match phi0 within 20ns
        "SYNC%=:          NOP \n" // attempt to sync with the next phi0 high phase
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "SHORTSYNC%=:     NOP \n" // shorter wait for first pass, to make sure we end up in the low phase
        "                 NOP \n"
        "                 NOP \n"
        " \n"
        "CHECK%=:         SBIS  %[phi0Port], %[phi0Pin]     \n"
        "                 RJMP  SYNC%=                      \n" // didn't quite reach phi0 high; try again
        " \n"
        "WAIT_ONE_PHI0%=: NOP                               \n" // cycles 3-10
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        // phi0 normally goes high here, but we are assuming every other phi0 pulse is skipped, so we wait another 6502 cycle
        "                 NOP \n"                              // cycles 1-9
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        "                 NOP \n"
        // phi0 is about to go high again
        "PHI2_GO%=:       STS   %[phi2Port], r25            \n" // set phi2 high (cycles 10-11)
        "                 NOP                               \n" // wait until it's time for phi2 to go low again
        "                 NOP                               \n"
        "                 NOP                               \n"
        "                 STS   %[phi2Port], r24            \n" // set phi2 low
        
        "FINALIZE%=:      SEI                               \n"  // global interrupts enable
        "                                                   \n"
        : : [phi0Port] "I" (_SFR_IO_ADDR(PIND)), [phi0Pin] "I" (PIND0), [phi2Port] "i" (_SFR_MEM_ADDR(PORTH)), [phi2Pin] "I" (PINH0) : "r24", "r25"
        );
    
    // Set a read cycle.
    digitalWrite(g_pinMap40DIL[s_R_W_o.pin], HIGH);

    return error;
}

#endif



//
// Alternate stub versions for platforms where clock sync isn't supported
//
#ifndef SYNCED_6502

PERROR C6502CpuSync::memRead_SyncPhi0_Skip1Pulse_1_5_Mhz(
                                                         UINT16 readAddress16,
                                                         UINT16 *pData
                                                         )
{
    return errorNotImplemented;
}

PERROR C6502CpuSync::memWrite_SyncPhi0_Skip1Pulse_1_5_Mhz(
                                                          UINT16 readAddress16,
                                                          UINT16 data16
                                                          )
{
    return errorNotImplemented;
}

#endif
