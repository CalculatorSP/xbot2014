#pragma once

#include <stdint.h>

enum Instrument
{
	GUITAR,
	BASS
};

enum SongDifficulty
{
	EASY,
	MEDIUM,
	HARD,
	EXPERT
};

class NoteEvent
{
public:
	uint64_t timestamp;		// ticks since start of song
	uint8_t key;			// button value, 0 (green) to 4 (orange)
	uint8_t press;			// 1 if pressed, 0 if released
};

typedef List<NoteEvent> Song;
