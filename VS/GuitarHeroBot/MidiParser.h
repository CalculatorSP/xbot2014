#pragma once

#include <stdio.h>

#include "Song.h"

class MidiParser
{
    static const uint8_t _greenButton[4];
    static const uint8_t _redButton[4];
    static const uint8_t _yellowButton[4];
    static const uint8_t _blueButton[4];
    static const uint8_t _orangeButton[4];

    static uint32_t _readVariableLen(FILE* fp);
    static uint32_t _parseHeader(FILE* fp);

public:
    static Song* parseMidi(const char* filename, Instrument instrument, SongDifficulty difficulty);

};