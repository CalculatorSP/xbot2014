#pragma once

#include <stdint.h>
#include "Collections/List.h"

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

enum ButtonColor
{
	GREEN,
	RED,
	YELLOW,
	BLUE,
	ORANGE
};

enum EventType
{
	PREPARE,
	ACTUATE
};

class NoteEvent
{
public:
	uint64_t timestamp;		// ticks since start of song
	EventType type;			// PREPARE will set the button, ACTUATE will send state
	ButtonColor key;		// button value, 0 (green) to 4 (orange)
	bool press;				// true if pressed, false if released
};

typedef List<NoteEvent> Song;
