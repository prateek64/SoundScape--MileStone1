#pragma once

#include "ofThread.h"
#include "Audio_Scene_Creator.h"

class Convolution_Thread : public ofThread {


public:

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

	void setpos(double X, double Y, double Z) {

		create_sound.X_real_time = X;
		create_sound.Y_real_time = Y; 
		create_sound.Z_real_time = Z;

	}

	void set_real_time_effect(int val) {

			create_sound.which_real_time_reverb = val;
			create_sound.real_time_effect = true;

	}


	void setdone_true() {


		create_sound.done = true;


	}

	void setdone_false() {


		create_sound.done = false;

	}

	

	void threadedFunction() {


		if (isThreadRunning()) {

			create_sound.real_time_proc();

		}

	}

	void real_time_effect_init() {

		create_sound.real_time_reverb_array();
		create_sound.add_real_time_effects();
	}

private:

	Audio_Scene_Creator create_sound;
	double Xpos, Ypos , Zpos ;


};