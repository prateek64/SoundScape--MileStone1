#pragma once

#include "ofMain.h"
#include "Audio_Scene_Creator.h"
#include "ofxMidi.h"
#include "Convolution_Thread.h"




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

	 ~ofApp();

	void newMidiMessage(ofxMidiMessage& eventArgs);
	void play_midi_drumkit();
	void midiSetup();
	void draw_ambient_listener(int radius, int anim_shape, double sine_pct, int color_choice);
	void file_init();

	// bool convolve1D(double conv_buf[256], int dataSize, int kernelSize);
	void draw_scene();
	void death_star();
	void change_drum_pos(int which_drum);
	void intitialize_drumkit_location();
	void move_drums();
	void draw_environments();
	void add_effect_real_time(int);
	void video_initialize();
	void normal_view_drag_events(int x , int y);
	void load_ambient_videos();


	// Earth View Functions 
	void create_earth_view_points(int x, int y);
	void draw_earth_view_points();
	void lat_long_to_x_y();
	string get_country_name(int x, int y);

	void initialize_country_points();
	bool if_India(int x, int y);
	bool if_Syria(int x, int y);
	bool if_Monterey(int x, int y);
	bool if_Florida(int x, int y);
	bool if_Montreal(int x, int y);
	bool if_Nova_Scotia(int x, int y);
	void audio_file_name_parser();
	void get_file_names_from_dir();
	void initialize_earth_point_sources();
	void update_place_info_box(int);

private:

	// Variables for normal view 

	int border_len = 80 ;

	int buffersize = 256;
	int circle_width = 90;
	int circle_height = 90;

	int ambient_env_radius = 150;
	int listener_radius = 30;

	ofImage mic_off;
	ofImage alien;
	ofImage mic_on;
	ofImage earth_texture;
	ofImage song[2];
	
	ofImage acoustic_env[4];
	ofVideoPlayer ambient_environment_videos[4];
	ofVideoPlayer earth_map;
	double death_star_angle = 0.0; 
	double image_rotate = 0.0;
	double song_rotate = 0.0;
	double earth_rotate = 0.0;
	double dist_threshold = 100;
	bool draw_mic_on = false;
	bool draw_mic_off = true;
	bool is_mic_on = false;
	bool is_earth_simulation_playing = false;

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
	ofPoint earth_video;
	ofPoint ambient_videos_loc[4];

	ofSpherePrimitive song_sphere[2];
	ofSpherePrimitive earth;

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

    Convolution_Thread audio_thread;
	
	
	ofSoundPlayer alien_sound[4],point_selection;

	
	// Variables for Earth View 

	bool are_locations_available = false;
	int earth_view_point_radius = 5;
	int number_of_earth_points = 0;

	vector <ofPoint> earth_view_points;
	vector <string> country_name;
	vector<ofPoint>pixel_pos;
	vector <float> latitude;
	vector<float> longitude;
	vector<string> file_names;
	vector<string> activity;
	vector<string> location;
	vector<string> time_recorded;
	int file_counter = 0;
	
	
 	struct country_points {

		ofPoint Syria[2];
		ofPoint India[2];
		ofPoint Monterey[2];
		ofPoint Nova_Scotia[2];
		ofPoint Montreal[2];
		ofPoint Florida[2];

	}; 

	country_points earth_data; 
	

	ofImage cork_board;
	ofImage crumpled_paper;
	ofImage pin;
	ofImage boat;

	ofImage text_box;
	ofTrueTypeFont	verdana14;

	bool*mouse_hover ;
	int which_point;
	
};


