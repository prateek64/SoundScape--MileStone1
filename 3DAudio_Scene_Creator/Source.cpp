

// 3daudio.cpp : Defines the entry point for the console application.
//

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

using namespace std;
/* load a file into memory, returning the buffer and
* setting bufsize to the size-in-bytes */
void* load(char *fname, long *bufsize) {
	FILE* fp = fopen(fname, "rb");
	fseek(fp, 0L, SEEK_END);
	long len = ftell(fp);
	rewind(fp);
	void *buf = malloc(len);
	fread(buf, 1, len, fp);
	fclose(fp);
	*bufsize = len;
	return buf;
}

void initi() {

	LPALGENEFFECTS alGenEffects = NULL;
	LPALDELETEEFFECTS alDeleteEffects = NULL;
	LPALISEFFECT alIsEffecti = NULL;

	LPALEFFECTI alEffecti = NULL;

	ALuint effect;

	alGenEffects = (LPALGENEFFECTS)
		alGetProcAddress("alGenEffects");
	alDeleteEffects = (LPALDELETEEFFECTS)
		alGetProcAddress("alDeleteEffects");

	alIsEffecti = (LPALISEFFECT)
		alGetProcAddress("alIsEffect");

	alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti");




	alGenEffects(1, &effect);
	alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);



}




int main() {


	/* initialize OpenAL context, asking for 44.1kHz to match HRIR data */
	ALCint contextAttr[] = { ALC_FREQUENCY,44100,0 };
	ALCdevice* device = alcOpenDevice(NULL);
	ALCcontext* context = alcCreateContext(device, contextAttr);
	alcMakeContextCurrent(context);



	/* listener at origin, facing down -z (ears at 1.5m height) */
	alListener3f(AL_POSITION, 0., 0, 0.);
	alListener3f(AL_VELOCITY, 0., 2., 0.);
	float orient[6] = { /*fwd:*/ 0., 0., -1., /*up:*/ 0., 1., 0. };
	alListenerfv(AL_ORIENTATION, orient);



	/* this will be the source of ghostly footsteps... */
	ALuint source;
	alGenSources(1, &source);
	alSourcef(source, AL_PITCH, 1.);
	alSourcef(source, AL_GAIN, 4);
	// alSource3f(source, AL_POSITION, curr[0], curr[1], curr[2]);


	alSource3f(source, AL_VELOCITY, 0., 0., 0.);
	alSourcei(source, AL_LOOPING, AL_TRUE);
	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
	alSourcef(source, AL_ROLLOFF_FACTOR, 1);
	alSourcef(source, AL_REFERENCE_DISTANCE, 6);
	alSourcef(source, AL_MAX_DISTANCE, 300);


	//	alSpeedOfSound(200);
	//	alDopplerFactor(3); 

	/* allocate an OpenAL buffer and fill it with monaural sample data */
	ALuint buffer;
	alGenBuffers(1, &buffer);
	{
		long dataSize;
		const ALvoid* data = load("C:/Users/pmurgai/Desktop/Quarter-1/256A/OPEN_AL_HRTF/ed.aiff", &dataSize);
		/* for simplicity, assume raw file is signed-16b at 44.1kHz */
		alBufferData(buffer, AL_FORMAT_MONO16, data, dataSize, 44100);
		free((void*)data);
	}

	alSourcei(source, AL_BUFFER, buffer);


	alSourcePlay(source);





	/* EAX Reverb is available. Set the EAX effect type then load the
	90          * reverb properties. */



	int Xpos = 100, Ypos = 100, Zpos = 100;
	int subx, suby, subz;
	int phase = 0;
	subx = suby = subz = 4;

	while (1) {

		alSource3f(source, AL_POSITION, Xpos, Ypos, Zpos);

		Xpos = Xpos - 2 * subx;
		Ypos = Ypos - suby;
		Zpos = Zpos + 1.5*subz;


		if (Xpos < -100 || Xpos > 100) {

			subx = -subx;

		}


		if (Ypos < -100 || Ypos >  140) {

			suby = -suby;

		}


		if (Zpos > 200 || Zpos < -100) {

			subz = -subz;

		}


		Sleep(200);

	}
	/* cleanup that should be done when you have a proper exit... ;) */
	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);
	alcDestroyContext(context);
	alcCloseDevice(device);

	return 0;
}



