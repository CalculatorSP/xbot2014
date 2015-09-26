#include "Task/Scheduler.h"
#include "MidiParser.h"
#include <iostream>
#include <Windows.h>
#include "PSX/XboxController.h"
#include "Collections/IirFilter.h"
#include "portaudio.h"

// Files to read
static const char* _songFile = "C:/Users/John/Source/Repos/xbot2014/VS/GuitarHeroBot/songs/Through The Fire And Flames.mid";

// Filter parameters
static const float _A[2] = { -1.91139741777364f, 0.9151594335679912f };
static const float _B[3] = { 0.0009405039485878813f, 0.0018810078971757626f, 0.0009405039485878813f };

// Audio parameters
#define SAMPLE_RATE			(44100)
#define FRAMES_PER_BUFFER	(2048)
#define NUM_CHANNELS		(2)
#define AUDIO_BUFSIZE		(2048)

// Data type for audio
typedef float SAMPLE;
#define PA_SAMPLE_TYPE  paFloat32
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"

// Algorithm parameters
#define THUMP_THRESH	(0.15f)		// Threshold for thump detection
#define THUMP_DELAY		(0L)	    // Delay for detecting thump (us)

static int _recordCallback(
	const void *inputBuffer,
	void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData);

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

	// Set up portaudio
	PaError err = Pa_Initialize();
	if (err != paNoError)
	{
		Pa_Terminate();
		std::cerr << "Error: " << Pa_GetErrorText(err) << std::endl;
		return -1;
	}

	// Set up audio capture from default device with 2 channels
	PaStreamParameters inputParameters;
	inputParameters.device = Pa_GetDefaultInputDevice();
	if (inputParameters.device == paNoDevice)
	{
		Pa_Terminate();
		std::cerr << "No default input device" << std::endl;
		return -1;
	}
	inputParameters.channelCount = 2;
	inputParameters.sampleFormat = PA_SAMPLE_TYPE;
	inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;

	// Set up low-pass biquad filter for thump detection
	Biquadf lowPassFilter;
	lowPassFilter.setA(ModBuf2f(_A));
	lowPassFilter.setB(ModBuf3f(_B));

	// Set up audio capture stream
	PaStream* iStream;
	err = Pa_OpenStream(
		&iStream,
		&inputParameters,
		NULL,
		SAMPLE_RATE,
		FRAMES_PER_BUFFER,
		paClipOff,
		_recordCallback,
		(void*)&lowPassFilter);				
	if (err != paNoError)
	{
		Pa_Terminate();
		std::cerr << "Error: " << Pa_GetErrorText(err) << std::endl;
		return -1;
	}

	// Start capturing audio
	err = Pa_StartStream(iStream);
	if (err != paNoError)
	{
		Pa_Terminate();
		std::cerr << "Error: " << Pa_GetErrorText(err) << std::endl;
		return -1;
	}

	// Wait for thump
	std::cout << "Listening for thump..." << std::endl;
	while ((err = Pa_IsStreamActive(iStream)) == 1);
	if (err < 0)
	{
		Pa_Terminate();
		std::cerr << "Error: " << Pa_GetErrorText(err) << std::endl;
		return -1;
	}

	std::cout << "GOT IT!!" << std::endl;

	// Close stream
	err = Pa_CloseStream(iStream);
	if (err != paNoError)
	{
		Pa_Terminate();
		std::cerr << "Error: " << Pa_GetErrorText(err) << std::endl;
		return -1;
	}
	Pa_Terminate();

	// Add all note events to scheduler
	uint64_t microsOffset = scheduler.getTime() - THUMP_DELAY;
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

/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int _recordCallback(
	const void* inputBuffer,
	void* outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void* userData)
{
	Biquadf* filter = (Biquadf*)userData;
	const SAMPLE* rptr = (const SAMPLE*)inputBuffer;

	// Prevent unused variable warnings
	(void)outputBuffer;
	(void)timeInfo;
	(void)statusFlags;

	// Deposit all the samples into the filter
	if (inputBuffer != NULL)
	{
		for (uint32_t i = 0; i < framesPerBuffer; ++i)
		{
			// Deposit average of left and right samples to filter
			if (filter->process(0.5f * (rptr[0] + rptr[1])) > THUMP_THRESH)
				return paComplete;

			rptr += 2;
		}
	}

	return paContinue;
}
