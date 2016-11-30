#include "ofApp.h"
#include<iostream>
#include<string>


//--------------------------------------------------------------


// Initializes images, sounds, framerate of the application and other input files.


using namespace std;

void ofApp::setup() {


	sound_stream.setup(this, 0, 1, 44100, buffersize, 2); // 2 output and 1 input channel with a sampling rate of 44100
	file_init();
	ofBackground(211, 211, 211); // Grey Background 
	ofSetFullscreen(true);
	ofSetFrameRate(30);
	ofEnableSmoothing();

	sound_stream.printDeviceList();
	sound_stream.setDeviceID(0);
	left.resize(buffersize);
	left.assign(buffersize, 0.0);

	midiSetup();  // Setup for midi input 
	intitialize_drumkit_location();
	ofEnableDepthTest();
	death_star();

	create_sounds.reverb_array();
	create_sounds.add_effects();




	for (int i = 0; i < number_of_sources; i++) {

		dragged[i] = false;
	}


	point_listener.x = 150;
	point_listener.y = ofGetScreenHeight() / 2;

	double init_listener_x, init_listener_y, init_listener_z;


	init_listener_x = point_listener.x - ofGetScreenWidth() / 2;
	init_listener_z = point_listener.y - ofGetScreenHeight() / 2;
	init_listener_y = 5;
	create_sounds.move_listener(init_listener_x, init_listener_y, init_listener_z);


	song_sphere[0].setRadius(40);
	song_sphere[0].mapTexCoords(0, 0, song[0].getWidth(), song[0].getHeight());

	song_sphere[1].setRadius(40);
	song_sphere[1].mapTexCoords(0, 0, song[1].getWidth(), song[1].getHeight());

	earth.setRadius(150);
	earth.setResolution(45);
	earth.mapTexCoords(0, 0, earth_texture.getWidth(), earth_texture.getHeight());

	ambient_environment.x = ofGetScreenWidth() - 300;
	ambient_environment.y = 300;


	double x_init, y_init, z_init;

	x_init = ambient_environment.x - ofGetScreenWidth() / 2;
	z_init = ambient_environment.y - ofGetScreenHeight() / 2;
	y_init = rand() % 50 - 25;


	if (!audio_thread.isThreadRunning()) {

		audio_thread.setdone_false();
		audio_thread.setpos(x_init, y_init, z_init);
		audio_thread.startThread();

	}

	video_initialize();
	initialize_country_points();

	
	cout << ofGetScreenWidth() << endl;
	cout << ofGetScreenHeight() << endl;

	
}
	

// Initializes the image files for the drum kit 

void ofApp::video_initialize() {

	earth_video.x = 0;
	earth_video.y = 0;
	earth_video.z = 30;

	earth_map.setVolume(0);
	earth_map.setSpeed(0.5);
	earth_map.setLoopState(OF_LOOP_NORMAL);




}


void ofApp::file_init() {


	mic_off.load("Images/mic_state_off.png");
	mic_on.load("Images/mic_state_on.png");
	song[0].loadImage("Images/gay_flag.png");
	song[1].loadImage("Images/gay_flag.png");

	earth_texture.loadImage("Images/earth_texture.jpg");

	acoustic_env[0].loadImage("Images/saint_paul.jpg");
	acoustic_env[1].loadImage("Images/underwater.jpg");
	acoustic_env[2].loadImage("Images/auditorium.jpg");
	acoustic_env[3].loadImage("Images/psychotic.jpg");



	for (int i = 0; i < number_of_sources; i++) {

		drumkit_parts[i].loadImage("Images/circles/circle_" + to_string(i + 1) + ".jpg");

	}


	earth_map.loadMovie("video/earth_map_2.mp4");

	for (int i = 0; i < 4; i++) {

		alien_sound[i].load("Sounds/Alien/alien_sound_" + to_string(i + 1) + ".wav");
		alien_sound[i].setVolume(0.1);
		
	}
		
	point_selection.load("Sounds/Bells/selection.wav");
	point_selection.setVolume(0.1);
}	



// Initializes the midi parameters

void ofApp::midiSetup() {

	ofSetLogLevel(OF_LOG_VERBOSE);

	// print input ports to console
	midiIn.listPorts(); // via instance
						//ofxMidiIn::listPorts(); // via static as well

						// open port by number (you may need to change this)
	midiIn.openPort(0);
	//midiIn.openPort("IAC Pure Data In");	// by name
	//midiIn.openVirtualPort("ofxMidiIn Input"); // open a virtual port

	// don't ignore sysex, timing, & active sense messages,
	// these are ignored by default
	midiIn.ignoreTypes(false, false, false);

	// add ofApp as a listener
	midiIn.addListener(this);

	// print received messages to the console
	midiIn.setVerbose(true);


}


// Initializes the drum kit sound locations in the 3D space (which are then rendered using HRTF)

void ofApp::intitialize_drumkit_location() {

	double init_radius = 40;
	int drum_sources = number_of_sources;
	int sign = 1;
	double theta = 0;

	for (int i = 0; i < drum_sources; i++) {


		drum_pos_x[i] = (radius_of_rev - 50)*cos(2*PI*i/drum_sources) - 50 ;
		drum_pos_z[i] = (radius_of_rev - 50)*sin(2*PI*i / drum_sources) ;
		drum_pos_y[i] = rand()%((int)(drum_pos_x[i]/2 + drum_pos_z[i]/2)) - radius_of_rev/2 ;
		create_sounds.add_source(drum_pos_x[i], drum_pos_y[i], drum_pos_z[i], i);

	}




}


// Method to draw the drum kit parts in the GUI 


void ofApp::draw_environments() {

	ofSetLineWidth(abs(12*sin(earth_rotate)));
	ofSetColor(255*cos(earth_rotate),0,255*sin(earth_rotate));
	ofDrawLine(ofGetScreenWidth() - 600, ofGetScreenHeight(), ofGetScreenWidth() - 600, ofGetScreenHeight() - 400);
	ofDrawLine(ofGetScreenWidth() - 604, ofGetScreenHeight() - 400, ofGetScreenWidth(), ofGetScreenHeight() - 400);
	ofSetColor(211, 211, 211);
	for (int i = 0; i < 4; i++) {

		if (i < 2) {

			acoustic_env[i].draw(ofGetScreenWidth() - 600 + 300*i , ofGetScreenHeight() - 400 , 300, 200);
		}

		else {

			acoustic_env[i].draw(ofGetScreenWidth() - 600 + 300 * (i-2), ofGetScreenHeight() - 400 +200,300,200);

		}
	}
}
void ofApp::draw_ambient_listener(int radius, int anim_shape,double sine_pct, int color_choice) {


	int circle_resolution = 1000; // amount of points circle is made of, more points - better quality, but could decrease perfomance
	
	int smoothing_amount = 10;

	ofPolyline circle_sin;
	ofPolyline circle_cos;
	ofPolyline circle_un_cos;



	int wave_height = radius*0.1*(scaledVol)*20;
	if (scaledVol < 0.1) {

		wave_height = radius*0.3;

	}
	

	float speed_increment = ofGetElapsedTimef();

	int line_w = 7;
	int radius_cos = radius + line_w - 1; // let's make it little less then line width, so circles will overlap each other
	int radius_un_cos = radius + (line_w - 1) *2;

	for (int i = 0; i<circle_resolution; i++) {

		float angle = TWO_PI / circle_resolution*i;
		float raidus_addon = wave_height*sin((angle + speed_increment)*anim_shape);

		float x = cos(angle + speed_increment)*radius;
		float y = sin(angle + speed_increment)*radius;

		// drawing sine wave only on a part of the circle
		if (i<sine_pct*circle_resolution)
		{
			x = cos(angle + speed_increment)*(radius + raidus_addon);
			y = sin(angle + speed_increment)*(radius + raidus_addon);
		}
		circle_sin.addVertex(ofPoint(x, y));


		raidus_addon = wave_height*cos((angle + speed_increment)*anim_shape);
		x = cos(angle + speed_increment)*(radius_cos );
		y = sin(angle + speed_increment)*(radius_cos);

		if (i<sine_pct*circle_resolution)
		{
			x = cos(angle + speed_increment)*(radius_cos + raidus_addon);
			y = sin(angle + speed_increment)*(radius_cos + raidus_addon);
		}

		circle_cos.addVertex(ofPoint(x, y));


		//increment a wave angle to flip the wave
		raidus_addon = 1.5*wave_height*cos((angle + TWO_PI / 3 + 2*speed_increment)*anim_shape);
		x = cos(angle + speed_increment)*(radius_un_cos);
		y = sin(angle + speed_increment)*(radius_un_cos);

		if (i<sine_pct*circle_resolution)
		{
			x = cos(angle + speed_increment)*(radius_un_cos + raidus_addon);
			y = sin(angle + speed_increment)*(radius_un_cos + raidus_addon);
		}

		circle_un_cos.addVertex(ofPoint(x, y));


	}

	ofSetLineWidth(line_w);

	if (color_choice == 1) {
		ofColor clr = ofColor::fromHex(0x000000);
		ofSetColor(clr);
		circle_sin.close(); // to connect first and last point of our shape we need to use ‘close’ function
		circle_sin = circle_sin.getSmoothed(smoothing_amount);
		circle_sin.draw();

		clr = ofColor::fromHex(0x00FF34);
		ofSetColor(clr);
		circle_cos.close();
		circle_cos = circle_cos.getSmoothed(smoothing_amount);
		circle_cos.draw();


		clr = ofColor::fromHex(0x00F3FF);
		ofSetColor(clr);
		circle_un_cos.close();
		circle_un_cos = circle_un_cos.getSmoothed(smoothing_amount);
		circle_un_cos.draw();
	}

	if (color_choice == 2) {
		ofColor clr = ofColor::fromHex(0xFF0000);
		ofSetColor(clr);
		circle_sin.close(); // to connect first and last point of our shape we need to use ‘close’ function
		circle_sin = circle_sin.getSmoothed(smoothing_amount);
		circle_sin.draw();

		clr = ofColor::fromHex(0x00FF00);
		ofSetColor(clr);
		circle_cos.close();
		circle_cos = circle_cos.getSmoothed(smoothing_amount);
		circle_cos.draw();


		clr = ofColor::fromHex(0x0000FF);
		ofSetColor(clr);
		circle_un_cos.close();
		circle_un_cos = circle_un_cos.getSmoothed(smoothing_amount);
		circle_un_cos.draw();
	}


}
void ofApp::draw_scene() {

	double theta = 0,x,y;


	for (int i = 0; i < number_of_sources; i++) {

		if(!dragged[i]){

			drum_kit[i].x = (radius_of_rev -30)*cos(2 * PI*i / number_of_sources);
			drum_kit[i].y = (radius_of_rev-30)*sin(2 * PI*i / number_of_sources);
			drumkit_parts[i].draw(drum_kit[i].x -50 , drum_kit[i].y -50 , i*0.5, circle_width, circle_height);

		}

		else  {

			
			drumkit_parts[i].draw(drum_kit[i].x - 50, drum_kit[i].y - 50, i*0.5, circle_width, circle_height);

		
	  }


  }

	
}


// Initializes the image files other than that contribute towards sound 



//--------------------------------------------------------------
void ofApp::update() {

	ofSoundUpdate();

	earth_map.update();

	if (is_mic_on) {

		scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
		move_drums();

	}

	death_star_angle = death_star_angle + 3.5;
    
	image_rotate = image_rotate + 4;
	theta = theta + 0.05;

	song_rotate = song_rotate + 2;
	earth_rotate += abs(10*scaledVol*4);

	if (!is_earth_simulation_playing) {

		if (sqrt(pow(point_listener.x - 300, 2) + pow(point_listener.y - ofGetScreenHeight() + 250, 2)) < dist_threshold) {

			listener_radius = 10;
			is_earth_simulation_playing = true;
			earth_map.play();
		}

	}
	
	earth_view_point_radius = 5 * sin(5 * theta) + 10;
}


void ofApp::death_star() {

	if (scaledVol < 0.1)
		nTri = 400;

	else
		nTri = 400*scaledVol*20;
						//The number of the triangles
	nVert = nTri * 3;		//The number of the vertices

							//The sphere's radius
							//Maximal triangle's “radius”
							//(formally, it's the maximal coordinates'
							//deviation from the triangle's center)

							//Fill the vertices array
	vertices.resize(nVert);		//Set the array size
	for (int i = 0; i<nTri; i++) {	//Scan all the triangles
									//Generate the center of the triangle
									//as a random point on the sphere

									//Take the random point from
									//cube [-1,1]x[-1,1]x[-1,1]	
		ofPoint center(ofRandom(-1, 1),
			ofRandom(-1, 1),
			0);
		center.normalize(); //Normalize vector's length to 1
		center *= Rad;	//Now the center vector has
						//length Rad

						//Generate the triangle's vertices
						//as the center plus random point from
						//[-rad, rad]x[-rad, rad]x[-rad, rad]
		for (int j = 0; j<3; j++) {
			vertices[i * 3 + j] =
				center + ofPoint(ofRandom(-rad, rad),
					ofRandom(-rad, rad),
					ofRandom(-rad/3, rad/3));
		}
	}

	//Fill the array of triangles' colors
	colors.resize(nTri);
	for (int i = 0; i<nTri; i++) {

		//Take a random color from black to red
		colors[i] = ofColor(abs(255 * sin(3 * theta)), abs(150 * sin(theta)) + rand()%100, abs(150 * cos(5 * theta)) + rand() % 100);
	

	}


}


// Method to move the drum according to the mic input 
void ofApp::move_drums() {

	double distance[4]; 
	for (int i = 0; i < number_of_sources; i++) {

		dragged[i] = true;
		drum_kit[i].x = drum_kit[i].x - 5* scaledVol*change_pos_x[i];
		drum_kit[i].y = drum_kit[i].y - 5 * scaledVol*change_pos_y[i];
		change_drum_pos(i);

		distance[0] = sqrt(pow(drum_kit[i].x, 2) + pow(drum_kit[i].y, 2));

		distance[1] = sqrt(pow(drum_kit[i].x + circle_width, 2) + pow(drum_kit[i].y, 2));
		distance[2] = sqrt(pow(drum_kit[i].x + circle_width, 2) + pow(drum_kit[i].y + circle_height, 2));
		distance[3] = sqrt(pow(drum_kit[i].x, 2) + pow(drum_kit[i].y + circle_width, 2));

		if (distance[0] > Rad - 20 || distance[1] > Rad - 20 || distance[2] > Rad - 20 || distance[3] > Rad - 20) {

			change_pos_x[i] = -change_pos_x[i];
			change_pos_y[i] = -change_pos_y[i];

		}


		
	}


}


// Method to move drums sounds  in the 3D space 
void ofApp::change_drum_pos(int which_drum) {


	drum_pos_x[which_drum] = drum_kit[which_drum].x ;
	drum_pos_z[which_drum] = 2*drum_kit[which_drum].y  ;
	drum_pos_y[which_drum] = rand()%50 - 25;


}


// Methods to play the midi kit in 3D space
void ofApp::play_midi_drumkit() {

	if (midiMessage.status == 144) {

		drum_pos_x[midiMessage.pitch] = drum_pos_x[midiMessage.pitch] - change_pos_x[midiMessage.pitch];
		drum_pos_y[midiMessage.pitch] = drum_pos_y[midiMessage.pitch] - change_pos_y[midiMessage.pitch];
		drum_pos_z[midiMessage.pitch] = drum_pos_z[midiMessage.pitch] + change_pos_z[midiMessage.pitch];

		if (drum_pos_x[midiMessage.pitch] < -100 || drum_pos_x[midiMessage.pitch] > 100) {

			change_pos_x[midiMessage.pitch] = -change_pos_x[midiMessage.pitch]; 

		}

		if (drum_pos_y[midiMessage.pitch] < -100 || drum_pos_y[midiMessage.pitch] > 120) {

			change_pos_y[midiMessage.pitch] = -change_pos_y[midiMessage.pitch];

		}

		if (drum_pos_z[midiMessage.pitch] < -100 || drum_pos_z[midiMessage.pitch] > 200) {

			change_pos_z[midiMessage.pitch] = -change_pos_z[midiMessage.pitch];

		}


		create_sounds.move_source(drum_pos_x[midiMessage.pitch], drum_pos_y[midiMessage.pitch], drum_pos_z[midiMessage.pitch], midiMessage.pitch);
		create_sounds.play_a_source(midiMessage.pitch);

	}


}
// Takes in the incoming midi messages 


void ofApp::newMidiMessage(ofxMidiMessage& msg) {

	// make a copy of the latest message
	midiMessage = msg;
	// play_midi_drumkit();

	if (midiMessage.status == 144) {

		create_sounds.move_source(drum_pos_x[midiMessage.pitch], drum_pos_y[midiMessage.pitch], drum_pos_z[midiMessage.pitch], midiMessage.pitch);
		create_sounds.change_gain(midiMessage.pitch, midiMessage.velocity);
		create_sounds.play_a_source(midiMessage.pitch);
		
	}

	if (midiMessage.control == 1) {

		create_sounds.pitch_shifting(midiMessage.value);

	}

	audio_thread.change_pitch(midiMessage.value);
}


void ofApp::add_effect_real_time(int val) {

	audio_thread.set_real_time_effect(val);

}

void ofApp::normal_view_drag_events(int x, int y) {

	double x_init, y_init, z_init;

	if (sqrt(pow(point_listener.x - x, 2) + pow(point_listener.y - y, 2)) <= listener_radius + 80) {

		point_listener.x = x;
		point_listener.y = y;

	}


	else if (sqrt(pow(x - ambient_environment.x, 2) + pow(y - ambient_environment.y, 2)) <= ambient_env_radius) {


		ambient_environment.x = x;
		ambient_environment.y = y;
		x_init = ambient_environment.x - ofGetScreenWidth() / 2;
		z_init = ambient_environment.y - ofGetScreenHeight() / 2;
		y_init = (x_init + z_init) / (rand() % 4 + 1);


		if (audio_thread.isThreadRunning()) {

			audio_thread.setpos(x_init, y_init, z_init);

		}

	}


	else {

		for (int i = 0; i < number_of_sources; i++) {

			if (abs(x - drum_kit[i].x - ofGetScreenWidth() / 2) <= circle_width && abs(y - drum_kit[i].y - ofGetScreenHeight() / 2) <= circle_height) {

				drum_kit[i].x = x - ofGetScreenWidth() / 2;
				drum_kit[i].y = y - ofGetScreenHeight() / 2;
				change_drum_pos(i);
				dragged[i] = true;
				break;

			}

		}

	}

}
void ofApp::audioIn(float * input, int bufferSize, int nChannels) {

	// void *buf = malloc(bufferSize);
	
	
	
	if (is_mic_on) {

			
		float curVol = 0.0;

		// samples are "interleaved"
		int numCounted = 0;
		
		//lets go through each sample and calculate the root mean square which is a rough way to calculate volume	
		for (int i = 0; i < bufferSize; i++) {

			left[i] = input[i];
			curVol += left[i] * left[i];
			numCounted += 1;
		}

		//this is how we get the mean of rms :) 
		curVol /= (float)numCounted;
		// this is how we get the root of rms :) 
		curVol = sqrt(curVol);

		smoothedVol *= 0.93;
		smoothedVol += 0.07 * curVol;

		//buf = input;
	//	in = &left[0];
	 //   create_sounds.real_time_proc(-5, 5, 5, input);
		//free((void*)buf);

		

	}

	// create_sounds.real_time_proc(-10, 10, 10);
	

}

/*
*****************************************************************************************************************************
*****************************EARTH VIEW FUNCTIONS*****************************************************************************
******************************************************************************************************************************
*/


void ofApp::lat_long_to_x_y(double lati, double longi) {

	pixel_pos.x = ((int)ofGetScreenWidth() / 360.0)*(180 + longi);
	pixel_pos.y = ((int)ofGetScreenHeight() / 180.0)*(90 - lati);

}
void ofApp:: initialize_country_points() {

	earth_data.India[0].x = 1807;
	earth_data.India[0].y = 458;
	earth_data.India[1].x = 1961;
	earth_data.India[1].y = 651;
	
	earth_data.Monterey[0].x = 334;
	earth_data.Monterey[0].y = 423;

	earth_data.Florida[0].x = 658;
	earth_data.Florida[0].y = 500;
	
	earth_data.Syria[0].x = 1562;
	earth_data.Syria[0].y = 436;

}

bool ofApp::if_India(int x, int y) {

	if (x >= earth_data.India[0].x && x <= earth_data.India[1].x  && y >= earth_data.India[0].y && y <= earth_data.India[1].y) {

		point_selection.play();
		return true;
	}

	return false;

}

bool ofApp::if_Syria(int x, int y) {


	if (sqrt(pow(x - earth_data.Syria[0].x, 2) + pow(y - earth_data.Syria[0].y, 2)) < 20) {
		point_selection.play();
		return true;
	}

	return false;
}

bool ofApp::if_Monterey(int x, int y) {

	if (sqrt(pow(x - earth_data.Monterey[0].x, 2) + pow(y - earth_data.Monterey[0].y, 2)) < 20) {
		point_selection.play();
		return true;
	}

	return false;
}

bool ofApp::if_Nova_Scotia(int x, int y) {


	return false;
}

bool ofApp::if_Florida(int x, int y) {

	if (sqrt(pow(x - earth_data.Florida[0].x, 2) + pow(y - earth_data.Florida[0].y, 2)) < 20) {
		point_selection.play();
		return true;
	}

	return false;
}

string ofApp::get_country_name(int x, int y) {

	if (if_India(x, y))
		return "India";

	else if (if_Syria(x, y)) {

		create_sounds.add_source(x, (x + y) / 2, y, 17);
		create_sounds.play_a_source(17);
		return "Syria";
	}
		

	else if (if_Monterey(x, y))
		return "Monterey";

	else if (if_Florida(x, y))
		return "Florida";


	return "No Data";
}
void ofApp::create_earth_view_points(int x, int y) {

	
	number_of_earth_points++; 

	earth_view_points.resize(number_of_earth_points);
	country_name.resize(number_of_earth_points);
	

	earth_view_points[number_of_earth_points - 1].x = x;
	earth_view_points[number_of_earth_points - 1].y = y;
	country_name[number_of_earth_points - 1] = get_country_name(x,y);

	cout << "country : " << country_name[number_of_earth_points - 1];
	cout << " X : " << earth_view_points[number_of_earth_points - 1].x << endl;
	cout << " Y : " << earth_view_points[number_of_earth_points - 1].y << endl;
	
}

void ofApp::draw_earth_view_points() {

	double old_point_x = earth_view_point_radius, old_point_y = 3;
	double new_point_x, new_point_y;
	double angle; 
	ofSetLineWidth(7);

	for (int i = 0; i < 40; i++) {

		angle =  PI*i / 40;

		
		ofSetColor(255 * sin(3 * theta), 255 * sin(theta), 255 * cos(5 * theta));
		new_point_x = earth_view_point_radius*cos(angle);
		new_point_y = 3 + earth_view_point_radius*sin(angle);

		if (i < 39)
			ofDrawLine(old_point_x, old_point_y, new_point_x, new_point_y);

		old_point_x = new_point_x;
		old_point_y = new_point_y;

		}

	old_point_x = -earth_view_point_radius;
	old_point_y = -3; 
	for (int i = 0; i < 40; i++) {

		angle =  PI*i / 40;


		ofSetColor(255 * sin(3 * theta), 255 * sin(theta), 255 * cos(5 * theta));
		new_point_x = earth_view_point_radius*cos(PI+angle);
		new_point_y = -3 + earth_view_point_radius*sin(PI+angle);

		if (i < 39)
			ofDrawLine(old_point_x, old_point_y, new_point_x, new_point_y);

		old_point_x = new_point_x;
		old_point_y = new_point_y;

	}



	


}

void ofApp::audio_file_name_parser(char*audio_file_name) {

	audio_file_name = "320A-Class_CCRMA_20.567_99.123.aiff";

	int buf_size = 0;

	while (audio_file_name[buf_size] != '\0') {

		buf_size++;

	}
	
	for (int i = 0; i < buf_size; i++) {



	}
}

/*
**************************************************************************************
************************ Common Functions For All the Views***************************
**************************************************************************************
*/

void ofApp::draw() {

	int listener_x, listener_y, listener_z;


	ofPushMatrix();

	ofTranslate(point_listener.x, point_listener.y, abs(5 * sin(theta)) + 60);

	draw_ambient_listener(abs(listener_radius * sin(theta / 3.5)) + 20, 25, 0.3, 2);

	listener_x = point_listener.x - ofGetScreenWidth() / 2;
	listener_z = point_listener.y - ofGetScreenHeight() / 2;
	listener_y = 5 * sin(theta) + 10;

	create_sounds.move_listener(listener_x, listener_y, listener_z);

	ofPopMatrix();

	if (is_earth_simulation_playing) {

		ofSetColor(211, 211, 211);

		ofPushMatrix();

		earth_map.draw(earth_video, ofGetScreenWidth(), ofGetScreenHeight());

		ofPopMatrix();

		for (int i = 0; i < number_of_earth_points; i++) {

			ofPushMatrix();

				ofTranslate(earth_view_points[i].x, earth_view_points[i].y,10);
				ofRotateZ(image_rotate/2);
				draw_earth_view_points();

			ofPopMatrix();

		}

	}

	else {

		ofPushMatrix();
		draw_environments();
		ofPopMatrix();

		ofPushMatrix();

		ofTranslate(ambient_environment.x, ambient_environment.y);
		ofRotateZ(image_rotate / 1.5);

		draw_ambient_listener(ambient_env_radius, 4, 0.8, 1);

		ofPopMatrix();

		ofPushMatrix();

		ofSetColor(0, 255, 255);
		ofTranslate(300, ofGetScreenHeight() - 250);
		ofRotateY(earth_rotate);

		earth_texture.getTextureReference().bind();
		earth.drawWireframe();
		earth_texture.unbind();

		ofPopMatrix();

		ofPushMatrix();

		ofTranslate(300, 300);
		ofSetColor(211, 211, 211);



		ofPushMatrix();
		ofRotateY(image_rotate);
		ofTranslate(90 + 90 * sin(theta), 0);

		song[0].getTextureReference().bind();  // Binds the image textures onto the spheres (so they look like planets)
		song_sphere[0].draw();
		song[0].unbind();
		ofPopMatrix();

		ofPushMatrix();
		ofRotateY(-2 * image_rotate);
		ofTranslate(-90 + 120 * cos(theta), -0);
		ofRotateY(image_rotate);
		song[1].getTextureReference().bind();  // Binds the image textures onto the spheres (so they look like planets)
		song_sphere[1].draw();
		song[1].unbind();
		ofPopMatrix();



		ofPopMatrix();

		ofPushStyle();

		ofEnableAlphaBlending();

		if (draw_mic_off == true) {

			ofSetColor(11, 220, 198, 255);
			mic_off.draw(0, 0, border_len, border_len);

			ofSetColor(11, 220, 198, 0);
			mic_on.draw(0, 0, border_len, border_len, 5);

		}

		if (draw_mic_on == true) {

			ofSetColor(11, 220, 198, 0);
			mic_off.draw(0, 0, 4, border_len, border_len);

			ofSetColor(11, 220, 198, 255);
			mic_on.draw(0, 0, 5, border_len, border_len);


		}

		ofDisableAlphaBlending();

		ofPopStyle();

		ofPushMatrix();
		glEnable(GL_DEPTH_TEST);
		ofTranslate(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2);

		ofPushMatrix();

		ofRotateY(death_star_angle);
		ofPushStyle();

		death_star();
		circle_speed++;


		if (circle_speed % 1 == 0) {
			for (int i = 0; i < nTri; i++) {

				ofSetColor(colors[i]);	//Set color
				ofTriangle(vertices[i * 3],
					vertices[i * 3 + 1],
					vertices[i * 3 + 2]); //Draw triangle

			}


		}

		ofPopStyle();

		ofPopMatrix();


		ofPushMatrix();

		ofRotateX(death_star_angle);
		ofPushStyle();

	    death_star();
		circle_speed++;


		if (circle_speed % 1 == 0) {
			for (int i = 0; i < nTri; i++) {

				ofSetColor(colors[i]);	//Set color
				ofTriangle(vertices[i * 3],
					vertices[i * 3 + 1],
					vertices[i * 3 + 2]); //Draw triangle

			}


		}
		ofPopStyle();

		ofPopMatrix();

		ofPushMatrix();

		ofRotateX(-death_star_angle);
		ofRotateY(death_star_angle);
		ofPushStyle();

		death_star();
		circle_speed++;


		if (circle_speed % 1 == 0) {
			for (int i = 0; i < nTri; i++) {

				ofSetColor(colors[i]);	//Set color
				ofTriangle(vertices[i * 3],
					vertices[i * 3 + 1],
					vertices[i * 3 + 2]); //Draw triangle

			}


		}
		ofPopStyle();

		ofPopMatrix();


		ofPushMatrix();

		ofRotateX(-2 * death_star_angle);
		ofRotateY(-death_star_angle);
		ofPushStyle();

		death_star();
		circle_speed++;


		if (circle_speed % 1 == 0) {
			for (int i = 0; i < nTri; i++) {

				ofSetColor(colors[i]);	//Set color
				ofTriangle(vertices[i * 3],
					vertices[i * 3 + 1],
					vertices[i * 3 + 2]); //Draw triangle

			}


		}
		ofPopStyle();

		ofPopMatrix();


		ofPushMatrix();
		ofSetColor(211, 211, 211);
		draw_scene();
		ofPopMatrix();

		ofPopMatrix();

	}

}

ofApp::~ofApp()
{

	for (int i = 0; i < 4; i++) {

		alien_sound[i].unload();
		acoustic_env[i].clear();
	}

	for (int i = 0; i < number_of_sources; i++) {

		drumkit_parts[i].clear();
	}

	earth_map.close();
	mic_off.clear();
	mic_on.clear();
	song[0].clear();
	song[1].clear();

	audio_thread.setdone_true();
	audio_thread.stopThread();
	
	

}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	
	if (key == OF_KEY_LEFT_SHIFT) {

		
		// create_sounds.add_effects();
	
	}

	if (key == OF_KEY_UP) {


		point_listener.y = point_listener.y - listener_dy;

		if (!alien_sound[0].isPlaying()) {

			alien_sound[0].play();

		}

	}

	if (key == OF_KEY_DOWN) {


		point_listener.y = point_listener.y + listener_dy;

		if (!alien_sound[1].isPlaying()) {

			alien_sound[1].play();
		}

	}

	if (key == OF_KEY_LEFT) {


		point_listener.x = point_listener.x - listener_dx;

		if (!alien_sound[2].isPlaying()) {

			alien_sound[2].play();

		}
	}

	if (key == OF_KEY_RIGHT) {


		point_listener.x = point_listener.x + listener_dx;

		if (!alien_sound[3].isPlaying()) {

			alien_sound[3].play();

		}

	}

	if (key == 114) {

		if (!is_earth_simulation_playing) {

			for (int i = 0; i < number_of_sources; i++) {


				drum_kit[i].x = (radius_of_rev - 30)*cos(2 * PI*i / number_of_sources);
				drum_kit[i].y = (radius_of_rev - 30)*sin(2 * PI*i / number_of_sources);
				drumkit_parts[i].draw(drum_kit[i].x - 50, drum_kit[i].y - 50, i*0.5, circle_width, circle_height);
				dragged[i] = false;

			}
		}

		else {

			listener_radius = 30;
			is_earth_simulation_playing = false;
			earth_map.stop();

		}

	}
}
	

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {


}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {


}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	if (!is_earth_simulation_playing) {
		
		normal_view_drag_events(x,y);

	}

	else {

		if (sqrt(pow(point_listener.x - x, 2) + pow(point_listener.y - y, 2)) <= listener_radius + 80) {

			point_listener.x = x;
			point_listener.y = y;

		}


	}
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(ofMouseEventArgs &mouse) {
	
	if (!is_earth_simulation_playing) {

		if (mouse.x >= 0 && mouse.x <= border_len && mouse.y >= 0 && mouse.y <= border_len) {



			if (draw_mic_off) {

				draw_mic_off = false;
				draw_mic_on = true;
				is_mic_on = true;




			}

			else {

				draw_mic_off = true;
				draw_mic_on = false;
				is_mic_on = false;

			}


		}

		if (mouse.x >= ofGetScreenWidth() - 600 && mouse.x <= ofGetScreenWidth() - 600 + 300 && mouse.y >= ofGetScreenHeight() - 400 && mouse.y <= ofGetScreenHeight() - 400 + 200) {

			create_sounds.which_effect(0);

			add_effect_real_time(4);

		}

		else if (mouse.x > ofGetScreenWidth() - 600 + 300 && mouse.x <= ofGetScreenWidth() - 600 + 600 && mouse.y >= ofGetScreenHeight() - 400 && mouse.y <= ofGetScreenHeight() - 400 + 200) {

			create_sounds.which_effect(1);

			add_effect_real_time(4);

		}

		else if (mouse.x > ofGetScreenWidth() - 600 && mouse.x <= ofGetScreenWidth() - 600 + 300 && mouse.y >= ofGetScreenHeight() - 400 + 200 && mouse.y <= ofGetScreenHeight() - 400 + 400) {

			create_sounds.which_effect(2);

			add_effect_real_time(4);

		}

		else   if (mouse.x >= ofGetScreenWidth() - 600 + 300 && mouse.x <= ofGetScreenWidth() - 600 + 600 && mouse.y >= ofGetScreenHeight() - 400 + 200 && mouse.y <= ofGetScreenHeight() - 400 + 400) {

			create_sounds.which_effect(3);

			add_effect_real_time(4);

		}

	}


	else {

		create_earth_view_points(mouse.x, mouse.y);
		cout << " Entered view point Creation" << endl;

	}
}



//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}



