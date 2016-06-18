#include "MidiParser.h"

#include <intrin.h>

const uint8_t MidiParser::_greenButton[4]	= { 60, 72, 84, 96 };
const uint8_t MidiParser::_redButton[4]		= { 61, 73, 85, 97 };
const uint8_t MidiParser::_yellowButton[4]	= { 62, 74, 86, 98 };
const uint8_t MidiParser::_blueButton[4]	= { 63, 75, 87, 99 };
const uint8_t MidiParser::_orangeButton[4]	= { 64, 76, 88, 100 };


Song* MidiParser::parseMidi(const char* filename, Instrument instrument, SongDifficulty difficulty)
{
    Song* song = new Song();
    FILE* fp;
    uint64_t currentTimestamp = 0;
    NoteEvent* lastStage = NULL;
    bool press = false;
    bool lastEventPress = false;
    bool stagedForRelease[5] = { false, false, false, false, false };

    if (fopen_s(&fp, filename, "rb"))
        return NULL;

    // Add release at beginning of song
    NoteEvent initialEvent;
    initialEvent.timestamp = 0;
    initialEvent.key = GREEN;
    initialEvent.press = false;
    initialEvent.type = PREPARE;
    song->add(initialEvent);
    initialEvent.key = RED;
    song->add(initialEvent);
    initialEvent.key = YELLOW;
    song->add(initialEvent);
    initialEvent.key = BLUE;
    song->add(initialEvent);
    initialEvent.key = ORANGE;
    song->add(initialEvent);
    initialEvent.timestamp = 250;
    initialEvent.type = ACTUATE;
    song->add(initialEvent);

    // Get tempo from header track
    uint64_t microsPerTick = _parseHeader(fp);

    // For bass part, skip guitar track
    uint32_t trackLen;
    fseek(fp, 4, SEEK_CUR);
    fread_s(&trackLen, 4, 4, 1, fp);
    trackLen = _byteswap_ulong(trackLen);
    if (instrument == BASS)
    {
        fseek(fp, trackLen + 4, SEEK_CUR);
        fread_s(&trackLen, 4, 4, 1, fp);
        trackLen = _byteswap_ulong(trackLen);
    }

    // Parse instrument track
    long endPos = ftell(fp) + trackLen;
    uint64_t lastUsedTimestamp = 0;
    while (ftell(fp) < endPos)
    {
        // Update running timestamp
        uint64_t deltaTime = (uint64_t)(_readVariableLen(fp)) * microsPerTick;
        currentTimestamp += deltaTime;
        uint8_t cmd;
        fread_s(&cmd, 1, 1, 1, fp);

        // Skip any meta events
        if (cmd == 0xFF)
        {
            fseek(fp, 1, SEEK_CUR);
            fseek(fp, _readVariableLen(fp), SEEK_CUR);
        }
        else
        {
            NoteEvent noteEvent;
            noteEvent.type = PREPARE;
            bool goodEvent = true;

            // Check if button is pressed or released
            if (cmd == 0x90)
            {
                press = true;
                fread_s(&cmd, 1, 1, 1, fp);
            }
            else if (cmd == 0x80)
            {
                press = false;
                fread_s(&cmd, 1, 1, 1, fp);
            }
            noteEvent.press = press;

            if (press)
                noteEvent.timestamp = currentTimestamp - 250;	// PREPARE needs to happen before ACTUATE
            else
                noteEvent.timestamp = currentTimestamp - 750;	// Release needs to happen before press
            
            // Get key based on difficulty
            if (cmd == _greenButton[difficulty])
                noteEvent.key = GREEN;
            else if (cmd == _redButton[difficulty])
                noteEvent.key = RED;
            else if (cmd == _yellowButton[difficulty])
                noteEvent.key = YELLOW;
            else if (cmd == _blueButton[difficulty])
                noteEvent.key = BLUE;
            else if (cmd == _orangeButton[difficulty])
                noteEvent.key = ORANGE;
            else
                goodEvent = false;

            if (goodEvent)
            {
                bool isChord = false;

                // If we have a new timestamp, we need to have an actuation of the previous note
                if (noteEvent.timestamp > lastUsedTimestamp)
                {
                    NoteEvent actuateEvent;
                    actuateEvent.timestamp = lastUsedTimestamp + 250;
                    actuateEvent.key = GREEN;
                    actuateEvent.press = false;
                    if (lastEventPress)
                    {
                        actuateEvent.type = ACTUATE;
                        for (int i = 0; i < 5; ++i)
                            stagedForRelease[i] = false;
                        lastStage = NULL;
                    }
                    else
                        actuateEvent.type = STAGE;

                    song->add(actuateEvent);

                    if (actuateEvent.type == STAGE)
                        lastStage = &(*song)[song->size - 1];

                    lastUsedTimestamp = noteEvent.timestamp;
                }
                else
                    isChord = true;

                // If this buttonpress is staged for release, we need to commit the previous actuate event first
                if ((isChord || (noteEvent.press && stagedForRelease[noteEvent.key])) && lastStage != NULL)
                {
                    lastStage->type = ACTUATE;
                    for (int i = 0; i < 5; ++i)
                        stagedForRelease[i] = false;
                    lastStage = NULL;
                }

                song->add(noteEvent);

                if (!noteEvent.press)
                    stagedForRelease[noteEvent.key] = true;

                lastEventPress = noteEvent.press;
            }

            // Skip velocity
            fseek(fp, 1, SEEK_CUR);
        }
    }

    // Add release at end of song
    NoteEvent finalEvent;
    finalEvent.timestamp = lastUsedTimestamp + 250;
    finalEvent.key = GREEN;
    finalEvent.press = false;
    finalEvent.type = ACTUATE;
    song->add(finalEvent);

    return song;
}

uint32_t MidiParser::_parseHeader(FILE* fp)
{
    // Get ticksPerQuarterNote from Midi Header
    uint16_t ticksPerQuarterNote;
    fseek(fp, 12, SEEK_CUR);
    fread_s(&ticksPerQuarterNote, 2, 2, 1, fp);
    ticksPerQuarterNote = _byteswap_ushort(ticksPerQuarterNote);

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
        uint32_t len = _readVariableLen(fp);

        if (cmd == 0x51)
        {
            // Tempo
            uint8_t tmp[4] = { 0, 0, 0, 0 };
            fread_s(&tmp[1], 3, 3, 1, fp);
            microsPerQuarterNote = _byteswap_ulong(*(uint32_t *)tmp);
        }
        else // Skip any other commands
            fseek(fp, len, SEEK_CUR);
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

    for (int i = 0; i < 4 && count >= 0; ++i, count -= 7)
        output += bytes[i] << (count);

    return output;
}
