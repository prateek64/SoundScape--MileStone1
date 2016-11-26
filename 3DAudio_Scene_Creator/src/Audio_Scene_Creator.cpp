#include "Audio_Scene_Creator.h"
#include<iostream>
#include<string>

using namespace std;
Audio_Scene_Creator::Audio_Scene_Creator()
{

	/* initialize OpenAL context, asking for 44.1kHz to match HRIR data */

	sample_rate = 44100;
	ALCint contextAttr[] = { ALC_FREQUENCY,sample_rate,0 };
	device[0] = alcOpenDevice(NULL);
	context = alcCreateContext(device[0], contextAttr);
	alcMakeContextCurrent(context);

    device[0] = alcOpenDevice(NULL);

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
	alcDestroyContext(context);
	
	for (int i = 0; i < 1; i++) {

		alcCloseDevice(device[i]);
	}

	
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

	
	alSourcef(source[which_source], AL_GAIN, 1);
	// alSource3f(source, AL_POSITION, curr[0], curr[1], curr[2]);

	alSource3f(source[which_source], AL_POSITION, X, Y, Z);
	alSource3f(source[which_source], AL_VELOCITY, 0., 0., 0.);
	alSourcei(source[which_source], AL_LOOPING, AL_FALSE);
	alDistanceModel(AL_EXPONENT_DISTANCE_CLAMPED);
	alSourcef(source[which_source], AL_ROLLOFF_FACTOR,13);
	alSourcef(source[which_source], AL_REFERENCE_DISTANCE, 300);
	alSourcef(source[which_source], AL_MAX_DISTANCE, 700);

	


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


   void Audio_Scene_Creator::real_time_proc() {


	   cout << " Thread has started "<<endl;
	   list<ALuint> bufferQueue; // A quick and dirty queue of buffer objects

	   ALenum errorCode = 0;
	
	   // Request the default capture device with a half-second buffer
	   device[1] = alcCaptureOpenDevice(NULL, FREQ, AL_FORMAT_MONO16, FREQ / 2);
	   errorCode = alcGetError(device[1]);
	   alcCaptureStart(device[1]); // Begin capturing
	   errorCode = alcGetError(device[1]);
	   alGenBuffers(16, &helloBuffer[0]); // Create some buffer-objects
	   errorCode = alGetError();

	   // Queue our buffers onto an STL list
	   for (int ii = 0; ii<16; ++ii) {
		   bufferQueue.push_back(helloBuffer[ii]);
	   }

	   alGenSources(1, &helloSource[0]); // Create a sound source
	   alSource3f(helloSource[0], AL_POSITION, X_real_time, Y_real_time, Z_real_time);
	   alDistanceModel(AL_EXPONENT_DISTANCE_CLAMPED);
	   alSourcef(helloSource[0], AL_PITCH, real_time_pitch);
	   alSourcef(helloSource[0], AL_GAIN, 1.5);
	   alSourcef(helloSource[0], AL_ROLLOFF_FACTOR, 13);
	   alSourcef(helloSource[0], AL_REFERENCE_DISTANCE, 300);
	   alSourcef(helloSource[0], AL_MAX_DISTANCE, 500);

	/*   reverb = EFX_REVERB_PRESET_PSYCHOTIC;

	   if (!alcIsExtensionPresent(alcGetContextsDevice(alcGetCurrentContext()), "ALC_EXT_EFX"))
	   {
		   fprintf(stderr, "Error: EFX not supported\n");
		   
	   }

	   get_process_address();

	   LoadEffect(&reverb);

	   slot = 0;

	   alGenAuxiliaryEffectSlots(1, &slot);
	   alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, effect);
	   assert(alGetError() == AL_NO_ERROR && "Failed to set effect slot");

	   alSource3i(helloSource[0], AL_AUXILIARY_SEND_FILTER, slot, 0, AL_FILTER_NULL);
	   assert(alGetError() == AL_NO_ERROR && "Failed to setup sound source");
	   */

	   errorCode = alGetError();
	   
	   while (!done) { // Main loop
					   // Poll for recoverable buffer

		   alSourcef(helloSource[0], AL_PITCH, real_time_pitch);
		   alSource3f(helloSource[0], AL_POSITION, X_real_time, Y_real_time, Z_real_time);
		  
		   alGetSourcei(helloSource[0], AL_BUFFERS_PROCESSED, &availBuffers);
		   if (availBuffers>0) {
			   alSourceUnqueueBuffers(helloSource[0], availBuffers, buffHolder);
			   for (int ii = 0; ii<availBuffers; ++ii) {
				   // Push the recovered buffers back on the queue
				   bufferQueue.push_back(buffHolder[ii]);
			   }
		   }
		   // Poll for captured audio
		   alcGetIntegerv(device[1], ALC_CAPTURE_SAMPLES, 1, &samplesIn);
		   if (samplesIn>CAP_SIZE) {
			   // Grab the sound
			   alcCaptureSamples(device[1], buffer_real, CAP_SIZE);

			   //***** Process/filter captured data here *****//
			   for (int ii=0;ii<CAP_SIZE;++ii) {

			        buffer_real[ii]*=0.3; // Make it quieter

			   }

			   // Stuff the captured data in a buffer-object
			   if (!bufferQueue.empty()) { // We just drop the data if no buffers are available
				   myBuff = bufferQueue.front(); bufferQueue.pop_front();
				   alBufferData(myBuff, AL_FORMAT_MONO16, buffer_real, CAP_SIZE * sizeof(short), FREQ);

				   // Queue the buffer
				   alSourceQueueBuffers(helloSource[0], 1, &myBuff);

				   // Restart the source if needed
				   // (if we take too long and the queue dries up,
				   //  the source stops playing).
				   ALint sState = 0;
				   alGetSourcei(helloSource[0], AL_SOURCE_STATE, &sState);
				   if (sState != AL_PLAYING) {
					   alSourcePlay(helloSource[0]);
				   }
			   }
		   }
	   }
	 

	   alcCaptureStop(device[1]);
	   alcCaptureCloseDevice(device[1]);

	   alcCloseDevice(device[1]);
	   
	   alSourceStopv(1, &helloSource[0]);
	   alDeleteSources(1, &helloSource[0]);
	   alDeleteBuffers(16, &helloBuffer[0]);
	   cout << "Thread has ended" << endl;
	   // Stop the sources
	  
	  
	 


} 

void Audio_Scene_Creator::which_effect(int effect_number) {

	if (effect_number == 1) {

		cout << "Effect is  : " << effect_number << endl;
		reverb = EFX_REVERB_PRESET_DOME_SAINTPAULS;
		add_effects(effect_number);
	}


	else if (effect_number == 2) {


		cout << "Effect is  : " << effect_number<<endl ;
		reverb = EFX_REVERB_PRESET_UNDERWATER;
		add_effects(effect_number);

	}


	else if (effect_number == 3) {


		cout << "Effect is  : " << effect_number<<endl;
		reverb = EFX_REVERB_PRESET_AUDITORIUM;
		add_effects(effect_number);
		
	}


	else if (effect_number == 4) {


		cout << "Effect is  : " << effect_number << endl;
		reverb = EFX_REVERB_PRESET_PSYCHOTIC;
		add_effects(effect_number);
		
	}


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
	
	
	for (int i = 11; i < 17; i++) {

		names[i] = base_folder + "Bells/chimey_" + to_string(i-10) + ".aiff";

	}

	names[17] = base_folder + "Songs/ed.aiff";

}

void Audio_Scene_Creator::play_a_source(int which_source) {


	alSourcePlay(source[which_source]);


}

void Audio_Scene_Creator::LoadEffect(const EFXEAXREVERBPROPERTIES *reverb , int effect_num) {


	
	ALenum err;

	/* Create the effect object and check if we can do EAX reverb. */
	alGenEffects(1, &effect[effect_num]);
	if (alGetEnumValue("AL_EFFECT_EAXREVERB") != 0)
	{
		printf("Using EAX Reverb\n");

		/* EAX Reverb is available. Set the EAX effect type then load the
		* reverb properties. */
		alEffecti(effect[effect_num], AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);

		alEffectf(effect[effect_num], AL_EAXREVERB_DENSITY, reverb->flDensity);
		alEffectf(effect[effect_num], AL_EAXREVERB_DIFFUSION, reverb->flDiffusion);
		alEffectf(effect[effect_num], AL_EAXREVERB_GAIN, reverb->flGain);
		alEffectf(effect[effect_num], AL_EAXREVERB_GAINHF, reverb->flGainHF);
		alEffectf(effect[effect_num], AL_EAXREVERB_GAINLF, reverb->flGainLF);
		alEffectf(effect[effect_num], AL_EAXREVERB_DECAY_TIME, reverb->flDecayTime);
		alEffectf(effect[effect_num], AL_EAXREVERB_DECAY_HFRATIO, reverb->flDecayHFRatio);
		alEffectf(effect[effect_num], AL_EAXREVERB_DECAY_LFRATIO, reverb->flDecayLFRatio);
		alEffectf(effect[effect_num], AL_EAXREVERB_REFLECTIONS_GAIN, reverb->flReflectionsGain);
		alEffectf(effect[effect_num], AL_EAXREVERB_REFLECTIONS_DELAY, reverb->flReflectionsDelay);
		alEffectfv(effect[effect_num], AL_EAXREVERB_REFLECTIONS_PAN, reverb->flReflectionsPan);
		alEffectf(effect[effect_num], AL_EAXREVERB_LATE_REVERB_GAIN, reverb->flLateReverbGain);
		alEffectf(effect[effect_num], AL_EAXREVERB_LATE_REVERB_DELAY, reverb->flLateReverbDelay);
		alEffectfv(effect[effect_num], AL_EAXREVERB_LATE_REVERB_PAN, reverb->flLateReverbPan);
		alEffectf(effect[effect_num], AL_EAXREVERB_ECHO_TIME, reverb->flEchoTime);
		alEffectf(effect[effect_num], AL_EAXREVERB_ECHO_DEPTH, reverb->flEchoDepth);
		alEffectf(effect[effect_num], AL_EAXREVERB_MODULATION_TIME, reverb->flModulationTime);
		alEffectf(effect[effect_num], AL_EAXREVERB_MODULATION_DEPTH, reverb->flModulationDepth);
		alEffectf(effect[effect_num], AL_EAXREVERB_AIR_ABSORPTION_GAINHF, reverb->flAirAbsorptionGainHF);
		alEffectf(effect[effect_num], AL_EAXREVERB_HFREFERENCE, reverb->flHFReference);
		alEffectf(effect[effect_num], AL_EAXREVERB_LFREFERENCE, reverb->flLFReference);
		alEffectf(effect[effect_num], AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, reverb->flRoomRolloffFactor);
		alEffecti(effect[effect_num], AL_EAXREVERB_DECAY_HFLIMIT, reverb->iDecayHFLimit);
	}
	else
	{

		printf("Using Standard Reverb\n");

		/* No EAX Reverb. Set the standard reverb effect type then load the
		* available reverb properties. */
		alEffecti(effect[effect_num], AL_EFFECT_TYPE, AL_EFFECT_REVERB);

		alEffectf(effect[effect_num], AL_REVERB_DENSITY, reverb->flDensity);
		alEffectf(effect[effect_num], AL_REVERB_DIFFUSION, reverb->flDiffusion);
		alEffectf(effect[effect_num], AL_REVERB_GAIN, reverb->flGain);
		alEffectf(effect[effect_num], AL_REVERB_GAINHF, reverb->flGainHF);
		alEffectf(effect[effect_num], AL_REVERB_DECAY_TIME, reverb->flDecayTime);
		alEffectf(effect[effect_num], AL_REVERB_DECAY_HFRATIO, reverb->flDecayHFRatio);
		alEffectf(effect[effect_num], AL_REVERB_REFLECTIONS_GAIN, reverb->flReflectionsGain);
		alEffectf(effect[effect_num], AL_REVERB_REFLECTIONS_DELAY, reverb->flReflectionsDelay);
		alEffectf(effect[effect_num], AL_REVERB_LATE_REVERB_GAIN, reverb->flLateReverbGain);
		alEffectf(effect[effect_num], AL_REVERB_LATE_REVERB_DELAY, reverb->flLateReverbDelay);
		alEffectf(effect[effect_num], AL_REVERB_AIR_ABSORPTION_GAINHF, reverb->flAirAbsorptionGainHF);
		alEffectf(effect[effect_num], AL_REVERB_ROOM_ROLLOFF_FACTOR, reverb->flRoomRolloffFactor);
		alEffecti(effect[effect_num], AL_REVERB_DECAY_HFLIMIT, reverb->iDecayHFLimit);
	}

	/* Check if an error occured, and clean up if so. */
	err = alGetError();
	if (err != AL_NO_ERROR)
	{
		fprintf(stderr, "OpenAL error: %s\n", alGetString(err));
		if (alIsEffect(effect[effect_num]))
			alDeleteEffects(1, &effect[effect_num]);
		
	}


}


void Audio_Scene_Creator::add_effects(int effect_number) {

	if (!alcIsExtensionPresent(alcGetContextsDevice(alcGetCurrentContext()), "ALC_EXT_EFX"))  {
		
		fprintf(stderr, "Error: EFX not supported\n");

	}

	get_process_address();


	LoadEffect(&reverb,effect_number);
	
	alGenAuxiliaryEffectSlots(1, &slot[effect_number]);
	alAuxiliaryEffectSloti(slot[effect_number], AL_EFFECTSLOT_EFFECT, effect[effect_number]);
	assert(alGetError() == AL_NO_ERROR && "Failed to set effect slot");

	for (int i = 0; i < source_counter; i++) {

		alSource3i(source[i], AL_AUXILIARY_SEND_FILTER, slot[effect_number], 0, AL_FILTER_NULL);
		assert(alGetError() == AL_NO_ERROR && "Failed to setup sound source");

	}

	
}


void Audio_Scene_Creator::get_process_address() {


	alGenEffects = (LPALGENEFFECTS)
		alGetProcAddress("alGenEffects");

	alDeleteEffects = (LPALDELETEEFFECTS)
		alGetProcAddress("alDeleteEffects");

	alIsEffect = (LPALISEFFECT)
		alGetProcAddress("alIsEffect");

	alEffecti = (LPALEFFECTI)
		alGetProcAddress("alEffecti");

	alEffectf = (LPALEFFECTF)
		alGetProcAddress("alEffectf");

	alEffectfv = (LPALEFFECTFV)
		alGetProcAddress("alEffectfv");

	alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)
		alGetProcAddress("alGenAuxiliaryEffectSlots");

	alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)
		alGetProcAddress("alAuxiliaryEffectSloti");


}