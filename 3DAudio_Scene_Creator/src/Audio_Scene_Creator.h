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
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include<AL/alext.h>
#include<AL/efx.h>
#include<AL/efx-creative.h>
#include<AL/efx-presets.h>
#include <vector>


using namespace std;

class Audio_Scene_Creator
{
public:

	Audio_Scene_Creator();
	void add_source(double X, double Y, double Z, int which_source);
	void move_source(int X, int Y, int Z, int which_source);
	void delete_source();
	void add_reverb();
	void*load_file(const char *fname, long *bufsize);
	void listner_location();
	void move_listener(int X, int Y, int Z);
	void distance_model_params();

	void add_doppler_effect(int speed_of_sound, int doppler_effect);

	void file_names();
	void files_drumkit();
	void play_a_source(int which_source);

	void pitch_shifting(int val);
	void delete_sources();
	void change_gain(int which_source, int val);
	~Audio_Scene_Creator();


private:

	int sample_rate;
	int X_source, Y_source, Z_source;
	int source_counter = 0;
	int listener_x, listener_y, listener_z;
	vector <ALuint> sound_sources;
	vector <ALuint> buffer_sources;

	ALuint source[11];
	ALuint buffer[11];

	int subx, suby, subz;
	int loc_change = 0;
	double max_pitch_shift = 4.0;
	double max_gain = 7;
	string base_folder = "C:/Users/pmurgai/Desktop/Quarter-1/256A/Final_Project/3DAudio_Scene_Creator/bin/data/Sounds/";
	string names[20];


};

#endif // HASH_H