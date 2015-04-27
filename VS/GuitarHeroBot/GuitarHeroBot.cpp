#include "Task/Scheduler.h"
#include "MidiParser.h"
#include <iostream>
#include <Windows.h>
#include "PSX/XboxController.h"
#include "Collections/FirFilter.h"
#include "portaudio.h"

// Files to read
const char* songFile = "C:/Users/John/Source/Repos/xbot2014/VS/GuitarHeroBot/songs/Through the Fire and Flames.mid";
const char* templateFile = "C:/Users/John/Source/Repos/xbot2014/VS/GuitarHeroBot/resources/ghThumpTemplate.raw";

// Audio parameters
#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (512)
#define NUM_SECONDS     (2)
#define NUM_CHANNELS    (2)
#define FILT_SIZE	(SAMPLE_RATE * NUM_SECONDS * NUM_CHANNELS)

// Data type for audio
typedef float SAMPLE;
#define PA_SAMPLE_TYPE  paFloat32
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"


/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int recordCallback(const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	FirFilter < SAMPLE, FILT_SIZE, SAMPLE >* templateMatcher = (FirFilter < SAMPLE, FILT_SIZE, SAMPLE >*)userData;
	const SAMPLE *rptr = (const SAMPLE*)inputBuffer;

	// Prevent unused variable warnings
	(void)outputBuffer;
	(void)timeInfo;
	(void)statusFlags;

	if (inputBuffer != NULL)
	{
		for (uint32_t i = 0; i < framesPerBuffer; ++i)
		{
			// Deposit left and right samples to template matching filter
			templateMatcher->deposit(*rptr++);
			if (NUM_CHANNELS == 2) templateMatcher->deposit(*rptr++);
		}
	}

	return paContinue;
}

int main(int argc, const char **argv)
{
	// Load thump template from file
	FILE* fid;
	if (fopen_s(&fid, templateFile, "r"))
	{
		std::cerr << "Could not open template file " << templateFile << std::endl;
		return -1;
	}

	// Read samples from thump template file
	SAMPLE tmpl[FILT_SIZE];
	fread(tmpl, sizeof(SAMPLE), FILT_SIZE, fid);

	// Create filter for template matching
	FirFilter < SAMPLE, FILT_SIZE, SAMPLE > templateMatcher;
	ModBuffer<SAMPLE, FILT_SIZE> templateKernel(tmpl);

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

	// Set up audio capture stream
	PaStream* stream;
	err = Pa_OpenStream(
		&stream,
		&inputParameters,
		NULL,                  /* &outputParameters, */
		SAMPLE_RATE,
		FRAMES_PER_BUFFER,
		paClipOff,
		recordCallback,
		&templateMatcher);
	if (err != paNoError)
	{
		Pa_Terminate();
		std::cerr << "Error: " << Pa_GetErrorText(err) << std::endl;
		return -1;
	}

	// Start capturing audio
	err = Pa_StartStream(stream);
	if (err != paNoError)
	{
		Pa_Terminate();
		std::cerr << "Error: " << Pa_GetErrorText(err) << std::endl;
		return -1;
	}

	// Print progress
	std::cout << "Listening for thump";
	while ((err = Pa_IsStreamActive(stream)) == 1)
	{
		Pa_Sleep(1000);
		std::cout << ".";
	}
	std::cout << std::endl;
	if (err < 0)
	{
		Pa_Terminate();
		std::cerr << "Error: " << Pa_GetErrorText(err) << std::endl;
		return -1;
	}

	// Close stream
	err = Pa_CloseStream(stream);
	if (err != paNoError)
	{
		Pa_Terminate();
		std::cerr << "Error: " << Pa_GetErrorText(err) << std::endl;
		return -1;
	}

	return 0;







	//XboxController xboxController("COM4");

	//Song* song = MidiParser::parseMidi(songFile, GUITAR, EXPERT);
	//if (song == NULL)
	//{
	//	std::cerr << "Could not open file " << songFile << std::endl;
	//	return -1;
	//}

	//Scheduler scheduler;
	//int foo;
	//scanf_s("%d", &foo);

	//uint64_t microsOffset = scheduler.getTime();
	//for (int i = 0; i < song->size; ++i)
	//{
	//	if ((*song)[i].type == PREPARE)
	//	{
	//		switch ((*song)[i].key)
	//		{
	//		case GREEN:
	//			if ((*song)[i].press)
	//				scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::press, &xboxController, XboxController::LT_D);
	//			else
	//				scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::release, &xboxController, XboxController::LT_D);
	//			break;
	//		case RED:
	//			if ((*song)[i].press)
	//				scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::press, &xboxController, XboxController::LB);
	//			else
	//				scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::release, &xboxController, XboxController::LB);
	//			break;
	//		case YELLOW:
	//			if ((*song)[i].press)
	//				scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::press, &xboxController, XboxController::RB);
	//			else
	//				scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::release, &xboxController, XboxController::RB);
	//			break;
	//		case BLUE:
	//			if ((*song)[i].press)
	//				scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::press, &xboxController, XboxController::RT_D);
	//			else
	//				scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::release, &xboxController, XboxController::RT_D);
	//			break;
	//		case ORANGE:
	//			if ((*song)[i].press)
	//				scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::press, &xboxController, XboxController::A);
	//			else
	//				scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::release, &xboxController, XboxController::A);
	//			break;
	//		default:
	//			break;
	//		}
	//	}
	//	else if ((*song)[i].type == ACTUATE)
	//		scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::sendState, &xboxController, 2);

	//}

	//while (scheduler.run());

	//return 0;
}