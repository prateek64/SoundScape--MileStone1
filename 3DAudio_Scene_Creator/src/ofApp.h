#pragma once

#include "ofMain.h"
#include "Audio_Scene_Creator.h"
#include "ofxMidi.h"

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


	void gui_layout();
	void file_init();

	// bool convolve1D(double conv_buf[256], int dataSize, int kernelSize);

	void image_init();
	void draw_scene();
	void death_star();
	void change_drum_pos(int which_drum);
	void intitialize_drumkit_location();

	void move_drums();
	void displace_listener();
	void draw_songs();
//	void ofApp::audioOut(float * out, int bufferSize, int nChannels);



private:


	int border_len = 80 ;

	int buffersize = 256;


	ofImage mic_off;
	ofImage alien;
	ofImage mic_on;
	ofImage road;
	ofImage song[2];

	double death_star_angle = 2.5; 
	bool draw_mic_on = false;
	bool draw_mic_off = true;
	bool is_mic_on = false;

	vector<double> input_sig;
	vector<double> impulse_resp;
	vector <float> left;
	vector< vector<float> > leftHistory;

	double*output;
	double phase = 0;
	double drum_pos_x[11];
	double drum_pos_y[11];
	double drum_pos_z[11];

	double change_pos_x[11] = { 3,2,1,4,1,1,6,2,3,5,4 };
	double change_pos_y[11] = { 3,-2,1,+4,1,+1,-6,2,3,5,-4 };
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
	float scaledVol = 1;

//	stk::FileWvIn impulse;

//	stk::FileWvIn input;
//	stk::FileWvOut outpt_file;
	bool convolve;
	bool dragged[11];
	Audio_Scene_Creator create_sounds;

	ofImage drumkit_parts[11];

	ofPoint drum_kit[11];
	ofPoint point_listener;


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
	
};
