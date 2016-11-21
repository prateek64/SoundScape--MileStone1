#include "Audio_Scene_Creator.h"



Audio_Scene_Creator::Audio_Scene_Creator()
{

	/* initialize OpenAL context, asking for 44.1kHz to match HRIR data */

	sample_rate = 44100;
	ALCint contextAttr[] = { ALC_FREQUENCY,sample_rate,0 };

	ALCdevice* device = alcOpenDevice(NULL);
	ALCcontext* context = alcCreateContext(device, contextAttr);
	alcMakeContextCurrent(context);

	device = alcOpenDevice(NULL);

	if (!device) {

		cout << "Device Not Available" << endl;

	}


	if (!context) {

		cout << " Error Creating Context" << endl;

	}

	source_counter = 0;
	listner_location();
}


Audio_Scene_Creator::~Audio_Scene_Creator()
{
}


using namespace std;


void Audio_Scene_Creator::listner_location() {


	/* listener at origin, facing down -z (ears at 1.5m height) */

	alListener3f(AL_POSITION, 0., 0, 0.);
	alListener3f(AL_VELOCITY, 0., 2., 0.);
	float orient[6] = { /*fwd:*/ 0., 0., -1., /*up:*/ 0., 1., 0. };
	alListenerfv(AL_ORIENTATION, orient);


}

void* Audio_Scene_Creator::load_file(char *fname, long *bufsize) {

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

void Audio_Scene_Creator::distance_model_params() {



}


void Audio_Scene_Creator::add_doppler_effect(int speed_of_sound, int doppler_effect) {


	alSpeedOfSound(speed_of_sound);
	alDopplerFactor(doppler_effect);


}