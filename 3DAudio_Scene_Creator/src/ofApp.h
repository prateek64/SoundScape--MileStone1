#pragma once

#include "ofMain.h"
#include "Audio_Scene_Creator.h"
#include "ofxMidi.h"
#include "AudioThread.h"

class ofApp : public ofBaseApp, public ofxMidiListener {

public:

	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(ofMouseEventArgs &mouse);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void audioIn(float * input, int bufferSize, int nChannels);


	void newMidiMessage(ofxMidiMessage& eventArgs);
	void play_midi_drumkit();
	void midiSetup();


	void draw_ambient_listener();
	void file_init();

	// bool convolve1D(double conv_buf[256], int dataSize, int kernelSize);

	void image_init();
	void draw_scene();
	void death_star();
	void change_drum_pos(int which_drum);
	void intitialize_drumkit_location();

	void move_drums();
	void displace_listener();
	void draw_environments();
//	void ofApp::audioOut(float * out, int bufferSize, int nChannels);


private:


	int border_len = 80 ;

	int buffersize = 256;
	int circle_width = 90;
	int circle_height = 90;

	int ambient_env_radius = 100;
	ofImage mic_off;
	ofImage alien;
	ofImage mic_on;
	ofImage road;
	ofImage song[2];
	ofImage real_time_audio;

	ofImage acoustic_env[4];

	double death_star_angle = 0.0; 
	double image_rotate = 0.0;
	bool draw_mic_on = false;
	bool draw_mic_off = true;
	bool is_mic_on = false;

	vector<double> input_sig;
	vector<double> impulse_resp;
	vector <float> left;
	vector< vector<float> > leftHistory;

	int number_of_sources = 17;
	double*output;
	double phase = 0;
	double drum_pos_x[17];
	double drum_pos_y[17];
	double drum_pos_z[17];

	double change_pos_x[17] = { 13,12,13,14,13,13,16,12,13,15,14, 12, 8, 10, 2.5,6,3 };
	double change_pos_y[17] = { 3,12,10.9,4,10,10,16,2,3,5,14,10,9,8.5,12.5,9.7,10 };
	double change_pos_z[11] = { 12,12,12,12,12,12,12,12,12,12,12 };
	double radius_of_rev = 500;

	double old_point_x = 100;
	double old_point_y = 0;

	double new_point_x;
	double new_point_y;

	double listener_dx = 8;
	double listener_dy = 8;
	double theta = 0;

	float smoothedVol = 0;
	float scaledVol = 0.08;
	

	bool dragged[17];
	Audio_Scene_Creator create_sounds;

	ofImage drumkit_parts[17];

	ofPoint drum_kit[17];
	ofPoint point_listener;
	ofPoint ambient_environment;
	ofSpherePrimitive song_sphere[2];

	vector<ofPoint> vertices;
	vector<ofColor> colors;
	int nTri;		//The number of triangles
	int nVert;	//The number of the vertices equals nTri * 3

	float Rad = 600;	//The sphere's radius
	float rad = 25;

	int circle_speed = 0;

	ofxMidiIn midiIn;
	ofxMidiMessage midiMessage;

	ofSoundStream sound_stream;

    AudioThread audio_thread;
	
	ofMutex lock;

	// Variables for Real Time Audio Processing 

};



