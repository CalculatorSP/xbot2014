#pragma once

#include "Song.h"
#include <stdio.h>

class MidiParser
{
	static uint32_t _readVariableLen(FILE* fp);
	static uint32_t _parseHeader(FILE* fp);

public:
	static Song* parseMidi(const char* filename, Instrument instrument, SongDifficulty difficulty);

};