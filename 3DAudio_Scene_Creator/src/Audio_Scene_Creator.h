#pragma once


/* This is the class to perform the 3D Audio rendering of the sound sources. HRTF's are integrated with the system to perform 3D audio 
 using headphones */

#ifndef AUDIO_SCENE_CREATOR_H
#define AUDIO_SCENE_CREATOR_H


#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>    /* for sqrtf() */
#include <time.h>    /* for time(), to seed srand() */

/* OpenAL headers */
#include <AL/alc.h>
#include<AL/efx.h>
#include "AL/alext.h"
#include<AL/efx-creative.h>
#include "AL/efx-presets.h"
#include "AL/alc.h"
#include <assert.h>
#include <vector>

#include <list>

#define FREQ 44100   // Sample rate
#define CAP_SIZE 1024// How much to capture at a time (affects latency)



// static LPALBUFFERSAMPLESSOFT alBufferSamplesSOFT = wrap_BufferSamples;
static LPALISBUFFERFORMATSUPPORTEDSOFT alIsBufferFormatSupportedSOFT;

/* Effect object functions */

static LPALGENEFFECTS alGenEffects;
static LPALDELETEEFFECTS alDeleteEffects;
static LPALISEFFECT alIsEffect;
static LPALEFFECTI alEffecti;
static LPALEFFECTIV alEffectiv;
static LPALEFFECTF alEffectf;
static LPALEFFECTFV alEffectfv;
static LPALGETEFFECTI alGetEffecti;
static LPALGETEFFECTIV alGetEffectiv;
static LPALGETEFFECTF alGetEffectf;
static LPALGETEFFECTFV alGetEffectfv;

/* Auxiliary Effect Slot object functions */

static LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
static LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
static LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
static LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
static LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
static LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
static LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
static LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
static LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
static LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
static LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;


using namespace std;

class Audio_Scene_Creator
{
public:

	Audio_Scene_Creator();

	void add_source(double X, double Y, double Z, int which_source);
	void load_earth_point_files(string file_names, int which_source);
	void real_time_proc();
	void move_source(int X, int Y, int Z, int which_source);
	void move_song();
	void LoadEffect(const EFXEAXREVERBPROPERTIES *rev, int effect_num);
	void add_effects();
	void add_real_time_effects();
	void reverb_array();
	void real_time_reverb_array();

	void which_effect(int effect_number);
	void*load_file(const char *fname, long *bufsize);
	void listner_location();
	void move_listener(int X, int Y, int Z);
	void get_process_address();
	void add_doppler_effect(int speed_of_sound, int doppler_effect);

	void file_names();
	void files_sound_sources();
	void play_a_source(int which_source);

	void pitch_shifting(int val);
	void change_gain(int which_source, int val);
	void delete_earth_point_sources();


	~Audio_Scene_Creator();


	bool done = false;
	bool set_gain_zero = false;
	double X_real_time; // Real time audio mic input sound source x position  
	double Y_real_time; // Real time audio mic input sound source y position
	double Z_real_time; // Real time audio mic input sound source z position

	double real_time_pitch = 1; // Real time audio input sound source initial pitch 
	bool real_time_effect = false;
	int which_real_time_reverb;

	string names[28]; // Array to store audio file names 

private:

	int sample_rate;
	int source_counter = 0; // Keeps count of of the sound sources being rendered in 3D 
	int listener_x, listener_y, listener_z; // Listener location parameters 

	
	ALuint source[28]; // Source array for synthetic sources 
	ALuint buffer[28]; // Buffer array for synethtic sources
	ALuint real_time_buffer;
	ALuint real_time_source;
	ALuint slot[8]; // Slot array for reverb effects 
	ALCdevice* device[2]; // Device array for 
	ALCcontext* context;
	int subx, suby, subz; 
	int loc_change = 0;
	double max_pitch_shift = 4.0; // Maximum Gain 
	double max_gain = 7; // Maximum Pitch Shift Factor 

	string base_folder = "../bin/data/Sounds/";
	

	ALuint helloBuffer[10], helloSource[1]; // Real time mic input audio source buffer and source array 
	

	EFXEAXREVERBPROPERTIES reverb[4]; // Reverb effect array to be used with synthetic sources 
	EFXEAXREVERBPROPERTIES  real_time_reverb[4]; // Reverb effect array to be used with real time mic input sound source 
	ALenum state;
	ALuint effect[8] = {0, 0, 0, 0, 0, 0, 0 ,0,}; // Effect array which will be linked to one or more reverbs 


	// Variables for Real Time Audio Processing

	short buffer_real[FREQ * 2]; // A buffer to hold captured audio
	ALCint samplesIn = 0;  // How many samples are captured
	ALint availBuffers = 0; // Buffers to be recovered
	ALuint myBuff; // The buffer we're using
	ALuint buffHolder[16]; // An array to hold catch the unqueued buffers

	
};


#endif // HASH_H