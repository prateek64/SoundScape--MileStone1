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
	// listner_location();
	subx = suby = subz = 10;
	file_names();
	listener_x = 0;
	listener_y = 0;
	listener_z = 0;
}


Audio_Scene_Creator::~Audio_Scene_Creator()
{
	for (int i = 0; i < source_counter; i++) {

		alDeleteSources(1, &source[i]);
		alDeleteBuffers(1, &buffer[i]);
	}
//	alcDestroyContext(context);
//	alcCloseDevice(device);

}


using namespace std;


void Audio_Scene_Creator::listner_location() {


	/* listener at origin, facing down -z (ears at 1.5m height) */

	
	alListener3f(AL_POSITION, listener_x, listener_y, listener_z);
	alListener3f(AL_VELOCITY, 0., 0., 0.);
	float orient[6] = { /*fwd:*/ 0., 0., -1., /*up:*/ 0., 1., 0. };
	alListenerfv(AL_ORIENTATION, orient);


}

void* Audio_Scene_Creator::load_file(const char *fname, long *bufsize) {


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

void Audio_Scene_Creator::change_gain(int which_source, int val) {

	double new_gain;
	new_gain = (max_gain*val)/ 127 ;
	alSourcef(source[which_source], AL_GAIN, new_gain);


}
void Audio_Scene_Creator::pitch_shifting(int val) {

	double new_pitch;

	new_pitch = (val*max_pitch_shift) / 127 ;

	if (new_pitch == 0) {

		new_pitch = 1;
	}

	for (int i = 0; i < source_counter; i++) {

		alSourcef(source[i], AL_PITCH , new_pitch);

	}

}

void Audio_Scene_Creator::move_source(int Xpos , int Ypos , int Zpos, int which_source ) {

	    
	alSource3f(source[which_source], AL_POSITION, Xpos, Ypos, Zpos);
	

}
void Audio_Scene_Creator::add_source(double X, double Y, double Z, int which_source) {

	/* this will be the source of ghostly footsteps... */
	
	alGenSources(1, &source[which_source]);
	

	alSourcef(source[which_source], AL_PITCH, 1);

	
	alSourcef(source[which_source], AL_GAIN, 4);
	// alSource3f(source, AL_POSITION, curr[0], curr[1], curr[2]);

	alSource3f(source[which_source], AL_POSITION, X, Y, Z);
	alSource3f(source[which_source], AL_VELOCITY, 0., 0., 0.);
	alSourcei(source[which_source], AL_LOOPING, AL_FALSE);
	alDistanceModel(AL_EXPONENT_DISTANCE_CLAMPED);
	alSourcef(source[which_source], AL_ROLLOFF_FACTOR,13);
	alSourcef(source[which_source], AL_REFERENCE_DISTANCE, 500);
	alSourcef(source[which_source], AL_MAX_DISTANCE, 900);

	


	//	alSpeedOfSound(200);
	//	alDopplerFactor(3); 

	/* allocate an OpenAL buffer and fill it with monaural sample data */
	
	alGenBuffers(1, &buffer[which_source]);
	{
		long dataSize;
		const char *cstr = names[which_source].c_str();
		const ALvoid* data = load_file(cstr, &dataSize);
		/* for simplicity, assume raw file is signed-16b at 44.1kHz */
		alBufferData(buffer[which_source], AL_FORMAT_MONO16, data, dataSize, 44100);
		free((void*)data);

	}

	alSourcei(source[which_source], AL_BUFFER, buffer[which_source]);

//	alSourcePlay(source[source_counter]);

	source_counter++;

}

void Audio_Scene_Creator::move_listener(int X, int Y, int Z) {


	listener_x = X;
	listener_y = Y;
	listener_z = Z;

	alListener3f(AL_POSITION, X , Y , Z );


}

void Audio_Scene_Creator::file_names() {

	
	files_drumkit();
	
}


void Audio_Scene_Creator::files_drumkit() {

	names[0] = base_folder + "Drums/dope.aiff";
	names[1] = base_folder + "Drums/bassdrum.aiff";
	names[2] = base_folder + "Drums/snardrum.aiff";
	names[3] = base_folder + "Drums/tomlowdr.aiff";
	names[4] = base_folder + "Drums/tommiddr.aiff";
	names[5] = base_folder + "Drums/tomhidrm.aiff";
	names[6] = base_folder + "Drums/hihatcym.aiff";
	names[7] = base_folder + "Drums/ridecymb.aiff";
	names[8] = base_folder + "Drums/crashcym.aiff";
	names[9] = base_folder + "Drums/cowbell1.aiff";
	names[10] = base_folder + "Drums/tambourn.aiff";


}

void Audio_Scene_Creator::play_a_source(int which_source) {


	alSourcePlay(source[which_source]);


}

