#include "MidiParser.h"

#include <intrin.h>

Song* MidiParser::parseMidi(const char* filename, Instrument instrument, SongDifficulty difficulty)
{
	Song* output = new Song();
	FILE* fp = fopen(filename, "rb");

	// Get tempo from header track
	uint64_t microsPerTick = _parseHeader(fp);

	// For bass part, skip guitar track
	uint32_t trackLen;
	fseek(fp, 4, SEEK_CUR);
	fread_s(&trackLen, 4, 4, 1, fp);
	if (instrument == BASS)
	{
		fseek(fp, trackLen + 4, SEEK_CUR);
		fread_s(&trackLen, 4, 4, 1, fp);
	}

	// Parse instrument track
	long endPos = ftell(fp) + trackLen;
	while (ftell(fp) < endPos)
	{

	}

	return output;
}

uint32_t MidiParser::_parseHeader(FILE* fp)
{
	// Get ticksPerQuarterNote from Midi Header
	uint32_t ticksPerQuarterNote;
	fseek(fp, 12, SEEK_CUR);
	fread_s(&ticksPerQuarterNote, 4, 4, 1, fp);
	ticksPerQuarterNote = _byteswap_ulong(ticksPerQuarterNote);

	// Get length of header track
	uint32_t trackLen;
	fseek(fp, 4, SEEK_CUR);
	fread_s(&trackLen, 4, 4, 1, fp);
	trackLen = _byteswap_ulong(trackLen);

	// Parse header track to get tempo
	uint32_t microsPerQuarterNote;
	long endPos = ftell(fp) + trackLen;
	while (ftell(fp) < endPos)
	{
		// Skip "00 FF"
		fseek(fp, 2, SEEK_CUR);

		// Get header command byte
		uint8_t cmd;
		fread_s(&cmd, 1, 1, 1, fp);

		switch (cmd)
		{
		case 0x51:
			// Tempo
			uint8_t tmp[4] = { 0, 0, 0, 0 };
			fread_s(&tmp[1], 3, 3, 1, fp);
			microsPerQuarterNote = _byteswap_ulong(*tmp);
			break;

		default:
			// Skip any other commands
			fseek(fp, len, SEEK_CUR);
			break;
		}
	}

	return microsPerQuarterNote / ticksPerQuarterNote;
}

uint32_t MidiParser::_readVariableLen(FILE* fp)
{
	uint32_t output = 0;
	uint8_t bytes[4];
	int8_t count = 0;

	for (int i = 0; i < 4; ++i, count += 7)
	{
		fread_s(&bytes[i], 1, 1, 1, fp);
		if (bytes[i] > 0x7F)
			bytes[i] -= 0x80;
		else
			break;
	}

	for (int i = 0; i < 4; ++i, count -= 7)
		output += bytes[i] << (count - 7);

	return output;
}
