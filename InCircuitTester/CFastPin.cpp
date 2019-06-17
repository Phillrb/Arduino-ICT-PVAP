//
// Copyright (c) 2015, Paul R. Swan
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
#include "CFastPin.h"


CFastPin::CFastPin(
    const UINT8        pinMap[],
    const CONNECTION  *connection
)
{
    UINT8 pin = pinMap[connection->pin];

    m_decodedPinMap = pin;

    m_physicalPinMask         = digitalPinToBitMask(pin);
    m_physicalPinMaskInverted = ~m_physicalPinMask;
    m_physicalPortRegisterIn  = portInputRegister(digitalPinToPort(pin));
    m_physicalPortRegisterOut = portOutputRegister(digitalPinToPort(pin));
};


void
CFastPin::pinMode(
    int     mode
)
{
    ::pinMode(m_decodedPinMap, mode);
};


//
// Create a crude clock signal by toggling the pin as fast as we can.
// On the Mega2560, this produces a "clock" of about 700 KHz.
//
void
CFastPin::doClocks(
    UINT32 numClocks
)
{
    UINT8 originalState = *(m_physicalPortRegisterOut);
    UINT8 alternateState = *(m_physicalPortRegisterOut) ^ m_physicalPinMask;
    for (UINT32 i = 0; i < numClocks; i++)
    {
        *(m_physicalPortRegisterOut) = alternateState;
        *(m_physicalPortRegisterOut) = originalState;
    }
}
