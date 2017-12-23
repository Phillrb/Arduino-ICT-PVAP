//
// Copyright (c) 2016, Paul R. Swan
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
#include <C6809EGenericGame.h>
#include <CStarWarsGame.h>
#include <CPaclandGame.h>

//
// The initial selector to select the game to test.
//
static const SELECTOR s_gameSelector[] PROGMEM = {//0123456789abcde
                                                  {"Generic 2716   ",  onSelectGeneric, (void*) (C6809EGenericGame::createInstance2716),    false},
                                                  {"Generic 2732   ",  onSelectGeneric, (void*) (C6809EGenericGame::createInstance2732),    false},
                                                  {"Star Wars      ",  onSelectGame,    (void*) (CStarWarsGame::createInstance),            false},
                                                  {"Star Wars (O)  ",  onSelectGame,    (void*) (CStarWarsGame::createInstanceSetO),        false},
                                                  {"Pacland        ",  onSelectGame,    (void*) (CPaclandGame::createInstance),             false},
                                                  {"Pacland +H     ",  onSelectGame,    (void*) (CPaclandGame::createInstance_Hi),          false},
                                                  {"Pacland +HR    ",  onSelectGame,    (void*) (CPaclandGame::createInstance_Hi_Rst),      false},
                                                  {"Pacland (J)    ",  onSelectGame,    (void*) (CPaclandGame::createInstance_J),           false},
                                                  {"Pacland (J) +H ",  onSelectGame,    (void*) (CPaclandGame::createInstance_J_Hi),        false},
                                                  {"Pacland (J) +HR",  onSelectGame,    (void*) (CPaclandGame::createInstance_J_Hi_Rst),    false},
                                                  {"Pacland (Jo)   ",  onSelectGame,    (void*) (CPaclandGame::createInstance_Jo),          false},
                                                  {"Pacland (Jo)+H ",  onSelectGame,    (void*) (CPaclandGame::createInstance_Jo_Hi),       false},
                                                  {"Pacland (Jo)+HR",  onSelectGame,    (void*) (CPaclandGame::createInstance_Jo_Hi_Rst),   false},
                                                  {"Pacland (Jo2)  ",  onSelectGame,    (void*) (CPaclandGame::createInstance_Jo2),         false},
                                                  {"Pacland (Jo2)H ",  onSelectGame,    (void*) (CPaclandGame::createInstance_Jo2_Hi),      false},
                                                  {"Pacland (Jo2)HR",  onSelectGame,    (void*) (CPaclandGame::createInstance_Jo2_Hi_Rst),  false},
                                                  {"Pacland (M)    ",  onSelectGame,    (void*) (CPaclandGame::createInstance_M),           false},
                                                  {"Pacland (M) +H ",  onSelectGame,    (void*) (CPaclandGame::createInstance_M_Hi),        false},
                                                  {"Pacland (M) +HR",  onSelectGame,    (void*) (CPaclandGame::createInstance_M_Hi_Rst),    false},
                                                  { 0, 0 }
                                                 };


