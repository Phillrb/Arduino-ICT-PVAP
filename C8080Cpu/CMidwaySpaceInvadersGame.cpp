//
// Copyright (c) 2019, Phillip Riscombe-Burton
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
#include "CMidwaySpaceInvadersGame.h"

//
// See the base game cpp comments for details about this platform.
//

// https://github.com/mamedev/mame/blob/244fd4da8f42557318580bc1e623bede66df8b6c/src/mame/drivers/8080bw.cpp

//
//  (sisv2) - Space Invaders v2 - MIDWAY ROMSET
//                                         01   02   04   08   10   20   40   80  100  200
// TODO UPDATE THIS 2N DATA FROM ROMS
static const UINT16 s_romData2nSV2Rev2_r36[] = {0x00,0x00,0xd4,0xf5,0xf5,0xdb,0xea,0x09,0x21,0xcd}; //CRC: 0Xd0c32d72
static const UINT16 s_romData2nSV2Rev2_r35[] = {0x32,0x2a,0xcd,0x00,0xcd,0x0c,0x25,0x7d,0xc2,0x3a}; //CRC: 0x0e159534
static const UINT16 s_romData2nSV2Rev2_r34[] = {0x32,0xc1,0xcd,0x67,0x02,0x18,0x0a,0xc3,0x00,0xf1}; //CRC: 0x483e651e
static const UINT16 s_romData2nSV2Rev2_r31[] = {0xcd,0x74,0x00,0xd3,0xaf,0x0c,0x09,0x13,0xb8,0x05}; //CRC: 0x1293b826
static const UINT16 s_romData2nSV2Rev2_r42[] = {0xc3,0x00,0x21,0xa7,0x06,0x3e,0x10,0x20,0x20,0xfe}; //CRC: 0xcd80b13f
static const UINT16 s_romData2nSV2Rev2_r41[] = {0x00,0x39,0x7a,0xfa,0x00,0x00,0x00,0x40,0x01,0x00}; //CRC: 0x2c68e0b4

//
//  (superinv) - Super Invaders
//                                         01   02   04   08   10   20   40   80  100  200
// TODO UPDATE THIS 2N DATA FROM ROMS
static const UINT16 s_romData2nSU_1[] = {0x00,0x00,0xd4,0xf5,0xf5,0xdb,0xea,0x09,0x21,0xcd}; //CRC: 0x7a9b4485
static const UINT16 s_romData2nSU_2[] = {0x32,0x2a,0xcd,0x00,0xcd,0x0c,0x25,0x7d,0xc2,0x3a}; //CRC: 0x7c86620d
static const UINT16 s_romData2nSU_3[] = {0x32,0xc1,0xcd,0x67,0x02,0x18,0x0a,0xc3,0x00,0xf1}; //CRC: 0xccaf38f6
static const UINT16 s_romData2nSU_4[] = {0xcd,0x74,0x00,0xd3,0xaf,0x0c,0x09,0x13,0xb8,0x05}; //CRC: 0x8ec9eae2
static const UINT16 s_romData2nSU_5[] = {0xc3,0x00,0x21,0xa7,0x06,0x3e,0x10,0x20,0x20,0xfe}; //CRC: 0x68719b30
static const UINT16 s_romData2nSU_6[] = {0x00,0x39,0x7a,0xfa,0x00,0x00,0x00,0x40,0x01,0x00}; //CRC: 0x8abe2466

//
//  (superinvrd) - Super Invaders modified by Ruffler & Deith
//                                           01   02   04   08   10   20   40   80  100  200
static const UINT16 s_romData2nSURD_1[] = {0x00,0x00,0xd4,0xf5,0xf5,0xdb,0xea,0x09,0x21,0xcd}; //CRC: 0xF625F153
static const UINT16 s_romData2nSURD_2[] = {0x32,0x2a,0xcd,0x00,0xcd,0x0c,0x25,0x7d,0xc2,0x3a}; //CRC: 0x9D628753
static const UINT16 s_romData2nSURD_3[] = {0x32,0xc1,0xcd,0x67,0x02,0x18,0x0a,0xc3,0x00,0xf1}; //CRC: 0xDB342868
static const UINT16 s_romData2nSURD_4[] = {0xcd,0x74,0x00,0xd3,0xaf,0x0c,0x09,0x13,0xb8,0x05}; //CRC: 0xB635E8AC
static const UINT16 s_romData2nSURD_5[] = {0xc3,0x00,0x21,0xa7,0x06,0x3e,0x10,0x20,0x20,0xfe}; //CRC: 0x014E761F
static const UINT16 s_romData2nSURD_6[] = {0x00,0x39,0x7a,0xfa,0x00,0x00,0x00,0x40,0x01,0x00}; //CRC: 0x599C6B78
static const UINT16 s_romData2nSURD_7[] = {0x11,0x04,0x04,0x26,0x26,0x0d,0x07,0xc3,0xbc,0x32}; //CRC: 0x0E8A75DF
static const UINT16 s_romData2nSURD_8[] = {0x64,0x20,0x00,0x4f,0xf1,0x43,0xcd,0x0e,0xce,0x3d}; //CRC: 0x11392151

//
// SV Rev 2 from MAME (sisv2)
//
static const ROM_REGION s_romRegionSV2[] PROGMEM = { //
                                                        {NO_BANK_SWITCH, 0x0000, 0x0400, s_romData2nSV2Rev2_r36, 0xd0c32d72, "r36"}, //
                                                        {NO_BANK_SWITCH, 0x0400, 0x0400, s_romData2nSV2Rev2_r35, 0x0e159534, "r35"}, //
                                                        {NO_BANK_SWITCH, 0x0800, 0x0400, s_romData2nSV2Rev2_r34, 0x483e651e, "r34"}, //
                                                        {NO_BANK_SWITCH, 0x1400, 0x0400, s_romData2nSV2Rev2_r31, 0x1293b826, "r31"}, //
                                                        {NO_BANK_SWITCH, 0x1800, 0x0400, s_romData2nSV2Rev2_r42, 0xcd80b13f, "r42"}, //
                                                        {NO_BANK_SWITCH, 0x1C00, 0x0400, s_romData2nSV2Rev2_r41, 0x2c68e0b4, "r41"}, //
                                                        {0} }; // end of list

//
// SU from MAME (superinv)
//
static const ROM_REGION s_romRegionSU[] PROGMEM = { //
                                                        {NO_BANK_SWITCH, 0x0000, 0x0400, s_romData2nSU_1, 0x7a9b4485, "r36"}, //
                                                        {NO_BANK_SWITCH, 0x0400, 0x0400, s_romData2nSU_2, 0x7c86620d, "r35"}, //
                                                        {NO_BANK_SWITCH, 0x0800, 0x0400, s_romData2nSU_3, 0xccaf38f6, "r34"}, //
                                                        {NO_BANK_SWITCH, 0x1400, 0x0400, s_romData2nSU_4, 0x8ec9eae2, "r31"}, //
                                                        {NO_BANK_SWITCH, 0x1800, 0x0400, s_romData2nSU_5, 0x68719b30, "r42"}, //
                                                        {NO_BANK_SWITCH, 0x1C00, 0x0400, s_romData2nSU_6, 0x8abe2466, "r41"}, //
                                                        {0} }; // end of list

//
// SURD from MAME (superinvrd)
//
static const ROM_REGION s_romRegionSURD[] PROGMEM = { //
                                                        {NO_BANK_SWITCH, 0x0000, 0x0400, s_romData2nSURD_1, 0xF625F153, "1 H"}, //
                                                        {NO_BANK_SWITCH, 0x0400, 0x0400, s_romData2nSURD_2, 0x9D628753, "2 G"}, //
                                                        {NO_BANK_SWITCH, 0x0800, 0x0400, s_romData2nSURD_3, 0xDB342868, "3 F"}, //
                                                        {NO_BANK_SWITCH, 0x1400, 0x0400, s_romData2nSURD_4, 0xB635E8AC, "4 E"}, //
                                                        {NO_BANK_SWITCH, 0x1800, 0x0400, s_romData2nSURD_5, 0x014E761F, "5 D"}, //
                                                        {NO_BANK_SWITCH, 0x1C00, 0x0400, s_romData2nSURD_6, 0x599C6B78, "6 C"}, //
                                                        {NO_BANK_SWITCH, 0x2000, 0x0400, s_romData2nSURD_7, 0x0E8A75DF, "7 B"}, //
                                                        {NO_BANK_SWITCH, 0x2400, 0x0400, s_romData2nSURD_8, 0x11392151, "8 A"}, //
                                                        {0} }; // end of list


IGame*
CMidwaySpaceInvadersGame::createInstanceMidway(
)
{
    return (new CMidwaySpaceInvadersGame(s_romRegionSV2));
}

IGame*
CMidwaySpaceInvadersGame::createInstanceSuperInvaders(
)
{
    return (new CMidwaySpaceInvadersGame(s_romRegionSU));
}

IGame*
CMidwaySpaceInvadersGame::createInstanceSuperInvadersRD(
)
{
    return (new CMidwaySpaceInvadersGame(s_romRegionSURD));
}


CMidwaySpaceInvadersGame::CMidwaySpaceInvadersGame(
    const ROM_REGION *romRegion
) : CSpaceInvadersBaseGame( romRegion )
{
}


