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
#include "CER2055.h"

/*
 
 Handler for the ER2055 Electrically Alterable Read Only Memory (EAROM) used in early-80s Atari games

 WARNING: The ER2055 is a limited-life device, rated for 1 million total write cycles.
          Use of write functions WILL shorten its useful life, so use them sparingly.
 
 References:
    http://www.jrok.com/datasheet/ER2055_EAROM.pdf

 Hardware description:
 
    64 byte capacity
    Requires -28V power supply for writes (and reads?)
 
    All(?) Atari games with an EAROM used three addresses to access it:
        EAWRITE (aka EAADDR) - loads latches connected to the EAROM's address and data lines
            - base address plus 6-bit offset
            - sets address for reads
            - sets address and data for writes
        EACONTROL - D0-D3 load a latch that sets the state of EAROM's control signals
            - clock (D0)
            - mode control C1 (D1, inverted so it will be high when latch is reset)
            - mode control C2 (D2)
            - chip select (D3)
        EAREAD - puts data from the last read operation on CPU data bus

    The chip has three modes, selected by pins C1/C2:
        READ  (C1 high, C2 high)
        ERASE (C1 low,  C2 low )
        WRITE (C1 low,  C2 high)
 
    Timing:
        Read access time: 2 us - 30 sec
        Erase cycle time: 50-200 ms
        Write cycle time: 50-200 ms
 
    An erase (preconditioning) cycle is required before writing
    Data state is invalid/undetermined after an erase cycle
    Clock pulse is optional for erase/write cycles if chip select goes low between address changes

*/

// EACONTROL bit definitions
#define CTL_IDLE  0x00 // clock low, C1/C2 set for read mode, chip select low
#define CTL_READ  0x00 // C1 high (inverted), C2 low
#define CTL_ERASE 0x06 // C1 low (inverted), C2 high
#define CTL_WRITE_C1D1_C2D2 0x02 // C1 low (inverted), C2 low
#define CTL_WRITE_C1D2_C2D1 0x04 // C1 low (inverted), C2 low, with C1/C2 pins swapped
#define CTL_CLOCK 0x01 // clock high
#define CTL_CS    0x08 // chip select high

#define ER2055_SIZE 0x40 // 64 bytes

CER2055::CER2055(
    ICpu   *cpu,
    UINT16 writeBaseAddress,
    UINT16 controlAddress,
    UINT16 readAddress,
    UINT8  C1C2mapping
) : m_cpu(cpu),
    m_writeBaseAddress(writeBaseAddress), // EAWRITE
    m_controlAddress(controlAddress),     // EACONTROL
    m_readAddress(readAddress)            // EAREAD
{
    // configure write mode to use the control signals that the game hardware expects
    m_CTL_WRITE = (C1C2mapping == EAROM_C1D2_C2D1) ? CTL_WRITE_C1D2_C2D1 : CTL_WRITE_C1D1_C2D2;
}

CER2055::~CER2055(
)
{
}

//
// Return to a dormant/idle state.
//
PERROR
CER2055::idle(
)
{
    PERROR error = errorSuccess;
    error = m_cpu->memoryWrite(m_controlAddress, CTL_IDLE);  // clock low, read mode, chip select disabled
    return error;
}

//
// Read a byte from EAROM
//
PERROR
CER2055::read(
                UINT8 offset,
                UINT8 *data
              )
{
    PERROR error = errorSuccess;
    UINT16 data16 = 0x0000;

    // start in idle state
    idle();
    
    // set address using EAWRITE latch
    m_cpu->memoryWrite(m_writeBaseAddress + offset, data16); // data doesn't matter
    
    // set EACONTROL to enable the chip in read mode
    m_cpu->memoryWrite(m_controlAddress, CTL_READ | CTL_CS);
    
    // toggle the clock line
    m_cpu->memoryWrite(m_controlAddress, CTL_READ | CTL_CS | CTL_CLOCK); // set clock high
    delayMicroseconds(2); // make sure we wait for the minimum access time (2us)
    m_cpu->memoryWrite(m_controlAddress, CTL_READ | CTL_CS); // back to clock low
    
    // read the data from EAREAD
    error = m_cpu->memoryRead(m_readAddress, &data16);
    if SUCCESS(error)
    {
        // return the data
        *data = (UINT8) data16;
    }
 
    // go back to idle state
    idle();
    
    return error;
}

//
// Write a byte to EAROM
//
PERROR
CER2055::write(
               UINT8 offset,
               UINT8 data
              )
{
    PERROR error = errorSuccess;

    // start in idle state
    error = idle();

    if SUCCESS(error)
    {
        // set address and data using EAWRITE latch
        error = m_cpu->memoryWrite(m_writeBaseAddress + offset, (UINT16) data);
    }
    
    if SUCCESS(error)
    {
        // activate EAROM in erase mode
        error = m_cpu->memoryWrite(m_controlAddress, CTL_ERASE | CTL_CS);
        delay(50); // wait the minimum erase cycle time (50ms)

        // switch to write mode
        error = m_cpu->memoryWrite(m_controlAddress, m_CTL_WRITE | CTL_CS);
        delay(50); // wait the minimum write cycle time (50ms)
    }
    
    // go back to idle state
    idle();

    return error;
}

//
// Simple read test - loads and displays the first four bytes
//
PERROR
CER2055::readTest(
)
{
    PERROR error = errorSuccess;
    String resultStr = "OK:";
    UINT8 data = 0x00;
    for (UINT8 i = 0; i < 4; i++)
    {
        error = read(i, &data);
        if FAILED(error)
        {
            break;
        }
        STRING_UINT8_HEX(resultStr, data);
    }
    
    if SUCCESS(error)
    {
        error = errorCustom;
        error->code = ERROR_SUCCESS;
        error->description = resultStr;
    }
    
    return error;
}

//
// Erase entire contents of EAROM
//
PERROR
CER2055::erase(
)
{
    // TO-DO:
    //      find out if unused EAROM contains 0x00 or 0xff bytes (or something else)
    
    PERROR error = errorSuccess;
    static const UINT8 clearVal = 0xff;
    UINT8 data = 0x00;
    UINT8 i = 0;
    
    for (i = 0; i < ER2055_SIZE; i++)
    {
        error = write(i, clearVal);
        if FAILED(error)
        {
            break;
        }
    }
    
    if SUCCESS(error)
    {
        // verify all bytes are cleared
        for (i = 0; i < ER2055_SIZE; i++)
        {
            error = read(i, &data);
            if FAILED(error)
            {
                break;
            }
            CHECK_VALUE_UINT8_BREAK(error, "RD", i, clearVal, data);
        }
    }
 
    return error;
}

//
// Dump entire EAROM contents to the serial port
//
PERROR
CER2055::serialDump(
)
{
    PERROR error = errorSuccess;
    const UINT8 line_size = 16;
    UINT8 data = 0x00;
    char buf[3];
    bool serial_was_active = false;
    
    if (Serial)
    {
        serial_was_active = true; // store previous port state
    }
    else
    {
        Serial.begin(9600);
        while (!Serial) {}; // wait for port to become active
    }
    
    for (UINT8 i = 0; i < ER2055_SIZE; i++)
    {
        error = read(i, &data);
        if FAILED(error)
        {
            break;
        }
        sprintf(buf, "%.2x", data);
        Serial.print(buf);
        // add a line break if this is the last byte on the line
        if ( (i+1) % line_size == 0)
        {
            Serial.println();
        }
        else
        {
            Serial.print(" ");
        }
    }
    
    // leave the serial port the way we found it
    if (!serial_was_active) {
        Serial.end();
    }
    return error;
}

//
// Load entire EAROM contents using hex bytes from the serial port
//
PERROR
CER2055::serialLoad(
)
{
    PERROR error = errorSuccess;
    char buf[ER2055_SIZE];
    bool serial_was_active = false;
    bool reading_high_nibble = true;
    bool cancelled = false;
    UINT8 inChar;
    UINT8 curNibble = 0;
    UINT8 pos = 0;

    // open the serial port, if not already open
    if (Serial)
    {
        serial_was_active = true;
    }
    else
    {
        Serial.begin(9600);
        while (!Serial) {}; // wait for port to become active
    }

    Serial.println(F("Ready to receive EAROM data"));
    Serial.println(F("   64 hex bytes; spaces and line breaks optional"));
    Serial.println(F("   'P' to stop receiving and program the bytes sent so far"));
    Serial.println(F("   Any other character to cancel"));
    
    // Load hex bytes into buffer
    while ((pos < ER2055_SIZE) and (!cancelled))
    {
        if (Serial.available() > 0)
        {
            inChar = Serial.read();
            // stop receiving and program now, if we aren't in the middle of getting a byte
            if ( ((inChar == 'P') || (inChar == 'p')) && (reading_high_nibble))
            {
                break; // exit the while loop without setting cancelled = true
            }
            curNibble = 0xff; // invalid (indicates no value has been set)
            switch (inChar)
            {
                case 'A' ... 'F':
                    curNibble = inChar - 'A' + 10;
                    break;
                case 'a' ... 'f':
                    curNibble = inChar - 'a' + 10;
                    break;
                case '0' ... '9':
                    curNibble = inChar - '0';
                    break;
                case ' ': // space
                case 10:  // LF
                case 13:  // CR
                    // shouldn't get these if we're looking for the second hex digit of a byte
                    if (!reading_high_nibble)
                    {
                        cancelled = true;
                    }
                    break;
                default:
                    // anything else is incorrect, so halt the receive
                    cancelled = true;
                    break;
            }
            if (curNibble != 0xff)
            {
                if (reading_high_nibble)
                {
                    buf[pos] = curNibble << 4;
                    reading_high_nibble = false;
                }
                else
                {
                    buf[pos] |= curNibble;
                    reading_high_nibble = true;
                    pos++;
                }
            }
        }
    }
    
    // If buffer was loaded, write it to the EAROM
    if ((!cancelled) && (pos > 0))
    {
        Serial.print(pos);
        Serial.println(F(" bytes received, now programming the EAROM"));

        for (UINT8 i = 0; i < pos; i++)
        {
            error = write(i, buf[i]);
            if FAILED(error)
            {
                break;
            }
        }
        if SUCCESS(error)
        {
            Serial.println(F("Programming complete"));
        }
        else
        {
            Serial.println(F("Programming failed"));
        }
    }
    else
    {
        Serial.println(F("Error receiving data; programming aborted"));
        error = errorUnexpected;
    }
    
    if (!serial_was_active) {
        Serial.end();
    }

    return error;
}
