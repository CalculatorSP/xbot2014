#include <iostream>
#include <Windows.h>

#include "Collections/IirFilter.h"
#include "PSX/XboxController.h"
#include "Task/Scheduler.h"

#include "MidiParser.h"

// Files to read
static const char* _songFile = "C:/Users/John/Git/xbot2014/VS/GuitarHeroBot/songs/Through The Fire And Flames.mid";

int main(int argc, const char **argv)
{
    XboxController xboxController("COM4");

    Song* song = MidiParser::parseMidi(_songFile, GUITAR, EXPERT);
    if (song == NULL)
    {
        std::cerr << "Could not open file " << _songFile << std::endl;
        return -1;
    }

    Scheduler scheduler;

    std::cout << "Press any key to start...";
    std::cin.ignore();

    // Add all note events to scheduler
    uint64_t microsOffset = scheduler.getTime();
    for (int i = 0; i < song->size; ++i)
    {
        if ((*song)[i].type == PREPARE)
        {
            switch ((*song)[i].key)
            {
            case GREEN:
                if ((*song)[i].press)
                    scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::press, &xboxController, XboxButton::LEFT_TRIGGER);
                else
                    scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::release, &xboxController, XboxButton::LEFT_TRIGGER);
                break;
            case RED:
                if ((*song)[i].press)
                    scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::press, &xboxController, XboxButton::LEFT_BUMPER);
                else
                    scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::release, &xboxController, XboxButton::LEFT_BUMPER);
                break;
            case YELLOW:
                if ((*song)[i].press)
                    scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::press, &xboxController, XboxButton::RIGHT_BUMPER);
                else
                    scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::release, &xboxController, XboxButton::RIGHT_BUMPER);
                break;
            case BLUE:
                if ((*song)[i].press)
                    scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::press, &xboxController, XboxButton::RIGHT_TRIGGER);
                else
                    scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::release, &xboxController, XboxButton::RIGHT_TRIGGER);
                break;
            case ORANGE:
                if ((*song)[i].press)
                    scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::press, &xboxController, XboxButton::A);
                else
                    scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::release, &xboxController, XboxButton::A);
                break;
            default:
                break;
            }
        }
        else if ((*song)[i].type == ACTUATE)
            scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::sendState, &xboxController, 2);
    }

    // Run scheduler to completion
    while (scheduler.run());

    return 0;
}
