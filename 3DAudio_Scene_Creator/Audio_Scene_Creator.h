#pragma once

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
	void add_source();
	void move_source();
	void delete_source();
	void*load_file(char *fname, long *bufsize);
	void listner_location();
	void move_listener();
	void distance_model_params();

	void add_doppler_effect(int speed_of_sound, int doppler_effect);


	~Audio_Scene_Creator();


private:

	int sample_rate;

	int source_counter;

	vector <ALuint> sound_sources;
	vector <ALuint> buffer;


};

#endif // HASH_H