// This class acts as a parallel thread that handles real time mic input convolution with EFX reverbs effects  

#pragma once

#include "ofThread.h"
#include "Audio_Scene_Creator.h"

class Convolution_Thread : public ofThread {


public:


	// Method to change the pitch of the real time mic input sound source 

	void change_pitch(double pitch) {

		double new_pitch;
		if (pitch == 0) {

			new_pitch = 1;
		}

		else {

			new_pitch = (pitch / 255) * 2.5;

		}

		create_sound.real_time_pitch = new_pitch;

	}


	// Method to set the position of the real time mic input sound source in the 3D audio scene 

	void setpos(double X, double Y, double Z) {

		create_sound.X_real_time = X;
		create_sound.Y_real_time = Y; 
		create_sound.Z_real_time = Z;

	}


	// Method to set the real time reverb effect 

	void set_real_time_effect(int val) {

			create_sound.which_real_time_reverb = val;
			create_sound.real_time_effect = true;

	}


	// Method that sets the real time audio processing to done 

	void setdone_true() {


		create_sound.done = true;


	}


	// Method that sets the real time audio processing to not done 

	void setdone_false() {


		create_sound.done = false;

	}

	
	// Function that is called when the thread is started from the main thread 

	void threadedFunction() {


		if (isThreadRunning()) {

			create_sound.real_time_proc();

		}

	}


	// Method that initializes the real time reverb effects and related parameters 

	void real_time_effect_init() {

		create_sound.real_time_reverb_array();
		create_sound.add_real_time_effects();
	}


private:

	Audio_Scene_Creator create_sound;
	double Xpos, Ypos , Zpos ;


};