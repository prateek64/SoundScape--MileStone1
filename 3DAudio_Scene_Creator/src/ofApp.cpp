#include "ofApp.h"
#include<iostream>
#include<string>
#include<fstream>
//--------------------------------------------------------------



// Initializes images, sounds, framerate of the application and other input files.

using namespace boost::filesystem;
using namespace std;


void ofApp::setup() {

	initialize_instrument_size();

	sound_stream.setup(this, 0, 1, 44100, buffersize, 2); // 2 output and 1 input channel with a sampling rate of 44100
	sound_stream.printDeviceList();
	sound_stream.setDeviceID(0);

	file_init(); // Initializes all the files 


	// Initializes the GUI screen parameters
	ofSetBackgroundColor(0); // Sets the background color to be black 
	ofSetFullscreen(true);
	ofSetFrameRate(40); 
	ofEnableSmoothing();
	ofEnableDepthTest();

	// Initializing vector size to be equal to buffer size( this will be used to store real-time audio input of buffersize into this vector  

	left.resize(buffersize);
	left.assign(buffersize, 0.0);


	
	get_file_names_from_dir();
	audio_file_name_parser();
	lat_long_to_x_y();


	midiSetup();  // Setup for midi input 

	intitialize_drumkit_location();
	

	create_sounds.reverb_array(); // Initializes the reverb array with all the different OpenAl soft reverbs 
	create_sounds.add_effects(); // Connects all the OpenAL reverbs with OpenAL auxillary effect slots



	// Initializes drag value for each midi instrument icon

	for (int i = 0; i < number_of_sources; i++) {

		dragged[i] = false;

	}


	// Initial position of the alien listener graphic in the GUI

	point_listener.x = 300;
	point_listener.y = ofGetScreenHeight() / 4;

	double init_listener_x, init_listener_y, init_listener_z;


	init_listener_x = point_listener.x - ofGetScreenWidth() / 2;
	init_listener_z = point_listener.y - ofGetScreenHeight() / 2;
	init_listener_y = 5;

	// Moves the listener to the initial position in the 3D audio scene that we are creating here

	create_sounds.move_listener(init_listener_x, init_listener_y, init_listener_z);


    // Initializes the earth sphere parameters

	earth.setRadius(150);
	earth.setResolution(45);
	earth.mapTexCoords(0, 0, earth_texture.getWidth(), earth_texture.getHeight()); 

	// Initial position of the ambient environment graphic in the GUI

	ambient_environment.x = ofGetScreenWidth() - 300;
	ambient_environment.y = 300;


	double x_init, y_init, z_init;

	x_init = ambient_environment.x - ofGetScreenWidth() / 2;
	z_init = ambient_environment.y - ofGetScreenHeight() / 2;
	y_init = rand() % 50 - 25;

	// Initial position of the ambient environment(read through mic of the device) in the 3D audio scene that we are creating 

	if (!audio_thread.isThreadRunning()) {
		
		audio_thread.setdone_false();
		audio_thread.setpos(x_init, y_init, z_init);
		audio_thread.startThread();

	}

	
	// Initializes the earth view video parameters 

	earth_video_initialize();


	// Initializes the verdana14 font object parameters

	ofTrueTypeFont::setGlobalDpi(84);
	verdana14.load("verdana.ttf", 14, true, true);
	verdana14.setLineHeight(18.0f);
	verdana14.setLetterSpacing(1.037);


	welcome.setSpeed(0.8);
	welcome.setVolume(0.4);
	welcome.play();
	
	
}


/*

*********************************************Initializes midi instrument icon width and height*****************************************************************************
Each midi note icon gets its width and height. This will be employed later on as we modify the width and height of each instrument icon individually later on in the program

*/


void ofApp::initialize_instrument_size() {

	circle_width = new int[number_of_sources]; // Array of icon width sizes 
	circle_height = new int[number_of_sources]; // Array of icon height sizes


// Initial instrument icon sizes in pixels 

	for (int i = 0; i < number_of_sources; i++) {

		circle_width[i] = 120;
		circle_height[i] = 120;

	}

}


// This method initializes the earth view video location, volume , video speed and loop state 

void ofApp::earth_video_initialize() {

	earth_video.x = 0;
	earth_video.y = 0;
	earth_video.z = 30;

	earth_map.setVolume(0);
	earth_map.setSpeed(0.5);
	earth_map.setLoopState(OF_LOOP_NORMAL);

}


// Initializes and loads all the image files, videos and sound files 

void ofApp::file_init() {

	load_reverb_videos(); // loads the videos into the GUI that depict the different reverbs that can be used

	mic_off.load("Images/mic_state_off.png");
	mic_on.load("Images/mic_state_on.png");

	earth_texture.loadImage("Images/earth_texture.jpg");

	// Loads the images for the midi note icons into the GUI

	for (int i = 0; i < number_of_sources; i++) {

		drumkit_parts[i].loadImage("Images/Bokeh.png");
		
	}

	earth_map.loadMovie("video/earth_map_2.mp4");

	
	// Loads the sound files for the alien movement ( up , down , left and right )

	for (int i = 0; i < 4; i++) {

		alien_sound[i].load("Sounds/Alien/alien_sound_" + to_string(i + 1) + ".wav");
		alien_sound[i].setVolume(0.1);
		
	}
		
	
	welcome.load("Sounds/Alien/welcome.wav"); // Welcome Message audio file 
	cork_board.load("Images/cork_board.jpg");
	crumpled_paper.load("Images/crumpled_paper_1.jpg");

	
}	

// Method to load the videos into the GUI that depict the different reverbs

void ofApp::load_reverb_videos() {


	// Loads all the reverb video 
	ambient_environment_videos[0].loadMovie("video/church.mp4");
	ambient_environment_videos[1].loadMovie("video/underwater.mp4");
	ambient_environment_videos[2].loadMovie("video/auditorium.mp4");
	ambient_environment_videos[3].loadMovie("video/psychotic.mp4");
	

	// Initializes the reverb video parameters like volume, speed and loop state 
	for (int i = 0; i < 4; i++) {

		ambient_environment_videos[i].setVolume(0);
		ambient_environment_videos[i].setSpeed(1);
		ambient_environment_videos[i].setLoopState(OF_LOOP_NORMAL);

	}

	// Initializes the reverb video locations 
	for (int i = 0; i < 4; i++) {

		ambient_videos_loc[i].z = 10;

		if (i < 2) {

			ambient_videos_loc[i].x = ofGetScreenWidth() - 600 + 300 * i;
			ambient_videos_loc[i].y = ofGetScreenHeight() - 400;
			
		}

		else {

			ambient_videos_loc[i].x = ofGetScreenWidth() - 600 + 300 * (i - 2);
			ambient_videos_loc[i].y = ofGetScreenHeight() - 400 + 200;

		}
	}


	// Pauses all the revebrb videos at GUI initialization
	for (int i = 0; i < 4; i++) {

		ambient_environment_videos[i].play();
		ambient_environment_videos[i].setPaused(true);
		
	}

	
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


// Initializes the drum kit sound locations in the 3D audio space that is being created (which are then rendered using HRTF through OpenAL soft)

void ofApp::intitialize_drumkit_location() {

	double init_radius = 40;
	int drum_sources = number_of_sources;
	int sign = 1;
	double theta = 0;

	for (int i = 0; i < drum_sources; i++) {


		drum_pos_x[i] = (radius_of_rev - 50)*cos(2*PI*i/drum_sources) - 50 ;
		drum_pos_z[i] = (radius_of_rev - 50)*sin(2*PI*i / drum_sources) ;
		drum_pos_y[i] = rand()%((int)(drum_pos_x[i]/2 + drum_pos_z[i]/2)) - radius_of_rev/2 ;
		create_sounds.add_source(drum_pos_x[i], drum_pos_y[i], drum_pos_z[i], i); // Method to add sound sources in the 3D space 

	}


}

// Method to draw the the reverb videos on the GUI. This function gets called in the ofApp draw() function 

void ofApp::draw_environments() {

	
	ofSetLineWidth(abs(12*sin(earth_rotate)));
	ofSetColor(255*cos(earth_rotate),0,255*sin(earth_rotate));
	ofDrawLine(ofGetScreenWidth() - 600, ofGetScreenHeight(), ofGetScreenWidth() - 600, ofGetScreenHeight() - 400);
	ofDrawLine(ofGetScreenWidth() - 604, ofGetScreenHeight() - 400, ofGetScreenWidth(), ofGetScreenHeight() - 400);
	ofSetColor(211, 211, 211);
	
	// Draws the reverb videos on the GUI at certain locations given by ambient_video_loc 

	for (int i = 0; i < 4; i++) {

		ambient_environment_videos[i].draw(ambient_videos_loc[i], 300, 200);
	}

	
}


// Method to draw the ambient environment graphics in the GUI . It is an abstract shape that depicts the ambient environment around us. 

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

//  Method to draw the midi instrument icons on the GUI

void ofApp::draw_scene() {

	double theta = 0;

	for (int i = 0; i < number_of_sources; i++) {

		ofSetColor(sin(color_change /200) * 255, sin(color_change/120)*100 , cos(color_change / 240)*200); // midi note icon color 
		
		color_change = color_change + 1;

		// Initial location of the midi note icons 

		if(!dragged[i]){
		


			drum_kit[i].x = (radius_of_rev -30)*cos(2 * PI*i / number_of_sources);
			drum_kit[i].y = (radius_of_rev-30)*sin(2 * PI*i / number_of_sources);
			drumkit_parts[i].draw(drum_kit[i].x -50 , drum_kit[i].y -50 , i*0.5, circle_width[i], circle_height[i]);



		}


		// Location of the midi note icons when the icons have been dragged 

		else  {

			
			drumkit_parts[i].draw(drum_kit[i].x - 50, drum_kit[i].y - 50, i*0.5, circle_width[i], circle_height[i]);

		
	  }


  }

	
}


//--------------------------------------------------------------

// ofApp function that updates the draw function and the variables within the update callback 

void ofApp::update() {

	ofSoundUpdate();

	// If midi instruments are not being dragged on the GUI 

	if (!is_midi_instrument_drag_event) {

		for (int i = 0; i < number_of_sources; i++) {


			if (circle_width[i] > 120) {

				circle_width[i] = circle_width[i] - 4;
				circle_height[i] = circle_height[i] - 4;
			}


		}


	}

	// Updates the reverb video frames on the GUI 

	for (int i = 0; i < 4; i++) {

		ambient_environment_videos[i].update();
		
	}

	earth_map.update(); // Updates the earth view video frames 


	if (is_mic_on) {

		scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true); // Converts the audio input buffer values to within 0-1 range 
		move_drums();

	}

	death_star_angle = death_star_angle + 3.5; // Rotation angle of the death star 
    
	image_rotate = image_rotate + 4;
	theta = theta + 0.05;

	earth_rotate += abs(10*scaledVol*4); // Rotation  of the earth 


	// If earth view is not on 

	if (!is_earth_simulation_playing) {

		if (sqrt(pow(point_listener.x - 300, 2) + pow(point_listener.y - ofGetScreenHeight() + 250, 2)) < dist_threshold) {

			listener_radius = 10;
			is_earth_simulation_playing = true;
			earth_map.play();
			initialize_earth_point_sources();

		}


	}

	
	earth_view_point_radius = 5 * sin(5 * theta) + 10;
}


// Method that creates the revolving circles in the center of the GUI 

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


// Method that moves the midi instruments icons in the GUI according to the RMS value of the real time audio input 

void ofApp::move_drums() {

	double distance[4]; 
	for (int i = 0; i < number_of_sources; i++) {

		dragged[i] = true;
		drum_kit[i].x = drum_kit[i].x - 5* scaledVol*change_pos_x[i];
		drum_kit[i].y = drum_kit[i].y - 5 * scaledVol*change_pos_y[i];
		change_drum_pos(i);

		distance[0] = sqrt(pow(drum_kit[i].x, 2) + pow(drum_kit[i].y, 2));

		distance[1] = sqrt(pow(drum_kit[i].x + circle_width[i], 2) + pow(drum_kit[i].y, 2));
		distance[2] = sqrt(pow(drum_kit[i].x + circle_width[i], 2) + pow(drum_kit[i].y + circle_height[i], 2));
		distance[3] = sqrt(pow(drum_kit[i].x, 2) + pow(drum_kit[i].y + circle_width[i], 2));

		if (distance[0] > Rad - 20 || distance[1] > Rad - 20 || distance[2] > Rad - 20 || distance[3] > Rad - 20) { // Keeps the moving icons within a certain area

			change_pos_x[i] = -change_pos_x[i];
			change_pos_y[i] = -change_pos_y[i];

		}
		
	}

}


// Method to move the midi notes in the 3D audio scene around your head 

void ofApp::change_drum_pos(int which_drum) {


	drum_pos_x[which_drum] = drum_kit[which_drum].x ;
	drum_pos_z[which_drum] = 2*drum_kit[which_drum].y  ;
	drum_pos_y[which_drum] = rand()%50 - 25;


}


// Method to move the midi instrument notes along a certain path in the 3D Audio Scene around your head

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


		create_sounds.move_source(drum_pos_x[midiMessage.pitch], drum_pos_y[midiMessage.pitch], drum_pos_z[midiMessage.pitch], midiMessage.pitch); // Method to move the soundd sources in 3D space
		create_sounds.play_a_source(midiMessage.pitch); // Method to play a certain midi note ( given by midi pitch ) in 3D 

	}


}



// Method that gets called whenever there is a MIDI message input 

void ofApp::newMidiMessage(ofxMidiMessage& msg) {

	// If earth view is not on. ( This is done to prevent the midi notes from playing in the earth view mode )

	if (!is_earth_simulation_playing) {
		
		midiMessage = msg;
		

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
}


// Method to add reverb to the real time audio input through the convolution thread (using Fast Convolution method) 

void ofApp::add_effect_real_time(int val) {

	audio_thread.set_real_time_effect(val);

}

// Method that is called when the mouse is dragged in the normal view mode ( not in the earth view mode )

void ofApp::normal_view_drag_events(int x, int y) {


	double x_init, y_init, z_init;

	// If the listener is being dragged 

	if (sqrt(pow(point_listener.x - x, 2) + pow(point_listener.y - y, 2)) <= listener_radius + 80) {

		point_listener.x = x;
		point_listener.y = y;

	}


	// If the ambient environment is being dragged 

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


	// If the midi note icons are being dragged 

	else {



		for (int i = 0; i < number_of_sources; i++) {

			if (abs(x - drum_kit[i].x - ofGetScreenWidth() / 2) <= circle_width[i]  && abs(y - drum_kit[i].y - ofGetScreenHeight() / 2) <= circle_height[i] ) {

				is_midi_instrument_drag_event = true;
				drum_kit[i].x = x - ofGetScreenWidth() / 2;
				drum_kit[i].y = y - ofGetScreenHeight() / 2;
				change_drum_pos(i);
				dragged[i] = true;

				if (circle_width[i] <= 170) {

					circle_width[i] = circle_width[i] + 4;
					circle_height[i] = circle_width[i] + 4;
					
				}

				break;

			}

		}

	}

}

// Callback function to receive the real time audio input 

void ofApp::audioIn(float * input, int bufferSize, int nChannels) {

	// Checks if the mic is on 
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


	}


	

}

/*
*****************************************************************************************************************************
*****************************EARTH VIEW FUNCTIONS*****************************************************************************
******************************************************************************************************************************
*/



// Initializes the earth view sound sources (recorded audio from different locations)   

void ofApp::initialize_earth_point_sources() {


	for (int i = 0; i < pixel_pos.size(); i++) {

		create_sounds.load_earth_point_files(file_names[i], i + 17);
		create_sounds.add_source(pixel_pos[i].x - ofGetScreenWidth() / 2, 0, pixel_pos[i].y - ofGetScreenHeight() / 2, i + number_of_sources);
		create_sounds.play_a_source(i + number_of_sources);
	}

}



// Function that maps latitude and longitude values to pixel positions on the earth map 

void ofApp::lat_long_to_x_y() {

	pixel_pos.resize(latitude.size());

	mouse_hover = new bool[pixel_pos.size()];

	for (int i = 0; i < pixel_pos.size(); i++) {

		mouse_hover[i] = false;

	}
	for (int i = 0; i < pixel_pos.size(); i++) {

		pixel_pos[i].x = (((int)ofGetScreenWidth() + 180) / 360.0)*(180 + longitude[i]) - 100 ;
		pixel_pos[i].y = (((int)ofGetScreenHeight() - 90)/ 180.0)*(90 - latitude[i]) ;

	}

	are_locations_available = true;
	
}



// Updates the information box with the data of the audio ( activity in the audio, location and time recorded) over which the mouse is hovered in the earth view 

void ofApp::update_place_info_box(int x ) {

	ofSetColor(255, 255, 255);
	ofDrawRectRounded(0,0, 400, 80, 10);
	ofSetColor(245, 58, 135);
	ofTranslate(0, 0, 10);
	verdana14.drawString("Activity : " + activity[x], 30, 20);
	verdana14.drawString("Location : " + location[x], 30, 40);
	verdana14.drawString("Recorded in : " + time_recorded[x], 30, 60);



}

// Method that draws a graphic to show on the earth map where the different audio have been recorded . It acts as a pointer to all the recorded audio on the earth map 

void ofApp::draw_earth_view_points(double radius) {

	double old_point_x = radius, old_point_y = 3;
	double new_point_x, new_point_y;
	double angle; 
	ofSetLineWidth(10);

	for (int i = 0; i < 40; i++) {

		angle =  PI*i / 40;

		
		ofSetColor(255 * sin(3 * theta), 255 * sin(theta), 255 * cos(5 * theta));
		new_point_x = radius*cos(angle);
		new_point_y = 3 + radius*sin(angle);

		if (i < 39)
			ofDrawLine(old_point_x, old_point_y, new_point_x, new_point_y);

		old_point_x = new_point_x;
		old_point_y = new_point_y;

		}

	old_point_x = -radius;
	old_point_y = -3; 

	for (int i = 0; i < 40; i++) {

		angle =  PI*i / 40;


		ofSetColor(255 * sin(3 * theta), 255 * sin(theta), 255 * cos(5 * theta));
		new_point_x =radius*cos(PI+angle);
		new_point_y = -3 + radius*sin(PI+angle);

		if (i < 39)
			ofDrawLine(old_point_x, old_point_y, new_point_x, new_point_y);

		old_point_x = new_point_x;
		old_point_y = new_point_y;

	}

}


// This method parses the audio file names to separate the activity, location and time of recording and saves this data in different vectors.

void ofApp::audio_file_name_parser() {

	
	vector<string> file_names_clone(file_names);

	mouse_hover = new bool[file_names_clone.size()];

	for (int k = 0; k < file_names_clone.size(); k++) {

		for (int i = 0; i < file_names_clone[k].size() - 5; i++) {

			
			if (file_names_clone[k][i] == '_') {

				file_names_clone[k][i] = ' ';

			}

		}

	}

	for (int k = 0; k < file_names_clone.size(); k++) {

		for (int i = 0; i < 5; i++) {

			file_names_clone[k].pop_back();

		}

	}


	file_counter = 0;

	for (int i = 0; i < file_names_clone.size(); i++) {

		std::stringstream s(file_names_clone[i]);

	
		file_counter++;

		latitude.resize(file_counter);
		longitude.resize(file_counter);
		activity.resize(file_counter);
		location.resize(file_counter);
		time_recorded.resize(file_counter);

		s >> activity[file_counter - 1] >> location[file_counter - 1] >> latitude[file_counter - 1] >> longitude[file_counter - 1] >> time_recorded[file_counter-1];

	}
	

}

// Method that fetches all the ".aiff" audio files from a given directory

void ofApp::get_file_names_from_dir() {


	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	//Dont' forget you change this directory with one you want to traverse!
	TCHAR  *directory = TEXT("C:/Users/pmurgai/Desktop/Quarter-1/256A/Final_Project/3DAudio_Scene_Creator/bin/data/Sounds/Geo_Tagged_Data");
	TCHAR patter[MAX_PATH];
	TCHAR fileName[MAX_PATH];
	memset(patter, 0x00, MAX_PATH);
	_stprintf(patter, TEXT("%s/*.aiff"), directory);
	hFind = FindFirstFile(patter, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("FindFirstFile failed (%d)\n", GetLastError());

	}
	else
	{
		do
		{


			//ignore current and parent directories
			if (_tcscmp(FindFileData.cFileName, TEXT(".")) == 0 || _tcscmp(FindFileData.cFileName, TEXT("..")) == 0)
				continue;

			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//ignore directories
			}
			else
			{
				//list the Files

				file_counter++;
				file_names.resize(file_counter);

				//	_tprintf(TEXT("%s\n"),FindFileData.cFileName);


				memset(fileName, 0x00, sizeof(fileName));
				file_names[file_counter - 1] = FindFileData.cFileName;
				_stprintf(fileName, TEXT("%s\\%s"), directory, FindFileData.cFileName);
				/*	FILE *fptr = _tfopen((const TCHAR *)fileName, TEXT("r"));
				//do here whatever you want to do..
				fclose(fptr); */

				

			}
		} while (FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}


}

/*
**************************************************************************************
************************ Common Functions For All the Views***************************
**************************************************************************************
*/


// ofApp method that draws the GUI elements and updates them as the application runs ( it is called everytime update() function is called ) 

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

		
		
	
		if (mouse_hover[which_point] == true) {

			ofPushMatrix();
			ofTranslate(pixel_pos[which_point].x + 50 ,pixel_pos[which_point].y - 50, 30);
			update_place_info_box(which_point);
		    ofPopMatrix();
			
		}
		

		ofSetColor(211, 211, 211);
		ofPushMatrix();
		ofTranslate(0, -51);
		earth_map.draw(earth_video, ofGetScreenWidth() , 2*ofGetScreenHeight() + 51 - 1396 );

		ofPopMatrix();

		if (are_locations_available) {

			for (int i = 0; i < pixel_pos.size() ; i++) {

				 

					ofPushMatrix();
					ofTranslate(pixel_pos[i].x, pixel_pos[i].y, 10);
					ofRotateZ(image_rotate / 2);
					draw_earth_view_points(earth_view_point_radius);

					ofPopMatrix();
				
			}
		}

	}

	else {


		ofPushMatrix();

			ofTranslate(300, ofGetScreenHeight() / 4,60);
			ofRotateZ(image_rotate);
			draw_earth_view_points(200);

		ofPopMatrix();


		ofPushMatrix();
		ofSetColor(211, 211, 211);
		ofTranslate(-700, -400, -600);
		cork_board.draw(0, 0, 2 * ofGetScreenWidth(), 2 * ofGetScreenHeight());

		ofPopMatrix();

		ofPushMatrix();
		draw_environments();
		ofPopMatrix();

		ofPushMatrix();

		
		ofTranslate(ambient_environment.x, ambient_environment.y);

		ofPushMatrix();
		// ofSetColor(211, 211, 211);
		ofTranslate(-225, -280, -100);
		crumpled_paper.draw(0, 0, 600, 700);
		ofPopMatrix();

		ofTranslate(0, 100);
		ofRotateZ(image_rotate / 1.5);

		draw_ambient_listener(ambient_env_radius, 4, 0.8, 1);

		ofPopMatrix();

		ofPushMatrix();

			ofSetColor(0, 255, 255);
			ofTranslate(300, ofGetScreenHeight()/2 + 400);

			ofPushMatrix();
				ofSetColor(211, 211, 211);
				ofTranslate(-340, -270, -100);
				crumpled_paper.draw(0, 0, 500, 600);
			ofPopMatrix();

			ofTranslate(0, 30);
			ofRotateY(earth_rotate);

			earth_texture.getTextureReference().bind();
			earth.drawWireframe();
			earth_texture.unbind();

		
		ofPopMatrix();


		ofPushMatrix();

		ofTranslate(300, 300);
		ofSetColor(211, 211, 211);


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
		// ofSetColor(211, 211, 211);
		ofTranslate(-1050, -1000, -590);
		crumpled_paper.draw(0, 0, 2000, 2000);
		ofPopMatrix();

		ofTranslate(-40, 0);

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

		ofEnableAlphaBlending();
		
		
		draw_scene();

		ofDisableAlphaBlending();
		ofPopMatrix();

		ofPopMatrix();

	}

}


// ofApp destructor 
ofApp::~ofApp()
{

	for (int i = 0; i < 4; i++) {

		alien_sound[i].unload(); // Unloads all the alien sounds 
		
	}

	for (int i = 0; i < number_of_sources; i++) {

		drumkit_parts[i].clear(); // Clears all the midi note icons 
	}

	earth_map.close(); 
	mic_off.clear();  	
	mic_on.clear();

	audio_thread.setdone_true();
	audio_thread.stopThread(); // Stops the convolution thread 
	
	

}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	
	if (key == OF_KEY_UP) {

		// Moves the alien up in the GUI 

		point_listener.y = point_listener.y - listener_dy;

		// Plays the alien sound associated with the up movement 

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

	// When the R key is pressed ( this key is meant to reset things)

	if (key == 114) {

		// Brings back all the midi note icons to their initial locations 

		if (!is_earth_simulation_playing) {

			for (int i = 0; i < number_of_sources; i++) {


				drum_kit[i].x = (radius_of_rev - 30)*cos(2 * PI*i / number_of_sources);
				drum_kit[i].y = (radius_of_rev - 30)*sin(2 * PI*i / number_of_sources);
				drumkit_parts[i].draw(drum_kit[i].x - 50, drum_kit[i].y - 50, i*0.5, circle_width[i], circle_height[i]);
				dragged[i] = false;

			}
		}

		// If earth view is on , stop the earth video , delete all the sound sources associated with the earth view and leave the earth view to get back into normal mode 

		else {

			listener_radius = 30;
			is_earth_simulation_playing = false;
			earth_map.stop();
			create_sounds.delete_earth_point_sources();

		}

	}

}
	

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {


}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

	// Keeps track of mouse movements in the earth view to see if it lands on an audio source on the earth map 

	for (int i = 0; i < pixel_pos.size(); i++) {

		
		// If it lands on a recorded audio on the earth map 
		if (sqrt(pow(x - pixel_pos[i].x, 2) + pow(y - pixel_pos[i].y, 2)) < 10) {
			
			mouse_hover[i] = true;
			which_point = i;
			
			
		}

		else {

			mouse_hover[i] = false;
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	// If earth view is on , just activate drag events related to the listener .
	// If earth view if off, react to the drag events related to the midi note icons, listener and the ambient environment  

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

		// Switches between the mic on and mic off icons 

		if (mouse.x >= 0 && mouse.x <= border_len && mouse.y >= 0 && mouse.y <= border_len) {



			if (draw_mic_off) {

				draw_mic_off = false;
				draw_mic_on = true;
				is_mic_on = true; // Turns on the mic 




			}

			else {

				draw_mic_off = true;
				draw_mic_on = false;
				is_mic_on = false; // Turns off the mic 

			}


		}


		/* 
			If mouse is pressed over this reverb video region in the GUI actitivate that reverb for the 3D audio scene and deactivate all the other active reverbs , also play
			that reverb video to show visually that its active.
		*/

		// Checks for the First Reverb ( St Peters Basilica)
		if (mouse.x >= ofGetScreenWidth() - 600 && mouse.x <= ofGetScreenWidth() - 600 + 300 && mouse.y >= ofGetScreenHeight() - 400 && mouse.y <= ofGetScreenHeight() - 400 + 200) {

			create_sounds.which_effect(0);

			add_effect_real_time(4);
			ambient_environment_videos[0].setPaused(false);
			ambient_environment_videos[0].play();
			 
			for (int i = 0; i < 4; i++) {

				if(i!=0)
				 ambient_environment_videos[i].setPaused(true);
			}
			
			

		}

		// Checks for the Second Reverb (Underwater )

		else if (mouse.x > ofGetScreenWidth() - 600 + 300 && mouse.x <= ofGetScreenWidth() - 600 + 600 && mouse.y >= ofGetScreenHeight() - 400 && mouse.y <= ofGetScreenHeight() - 400 + 200) {

			create_sounds.which_effect(1);

			add_effect_real_time(4);

			ambient_environment_videos[1].setPaused(false);
			ambient_environment_videos[1].play();

			for (int i = 0; i < 4; i++) {

				if (i != 1)
					ambient_environment_videos[i].setPaused(true);
			}


		}

		// Checks for the Thrid Reverb (Auditorium)

		else if (mouse.x > ofGetScreenWidth() - 600 && mouse.x <= ofGetScreenWidth() - 600 + 300 && mouse.y >= ofGetScreenHeight() - 400 + 200 && mouse.y <= ofGetScreenHeight() - 400 + 400) {

			create_sounds.which_effect(2);

			add_effect_real_time(4);

			ambient_environment_videos[2].setPaused(false);
			ambient_environment_videos[2].play();

			for (int i = 0; i < 4; i++) {

				if (i != 2)
					ambient_environment_videos[i].setPaused(true);
			}


		}


		// Checks for the Fourth Reverb (Psychotic)

		else   if (mouse.x >= ofGetScreenWidth() - 600 + 300 && mouse.x <= ofGetScreenWidth() - 600 + 600 && mouse.y >= ofGetScreenHeight() - 400 + 200 && mouse.y <= ofGetScreenHeight() - 400 + 400) {

			create_sounds.which_effect(3);

			add_effect_real_time(4);

			ambient_environment_videos[3].setPaused(false);
			ambient_environment_videos[3].play();

			for (int i = 0; i < 4; i++) {

				if (i != 3)
					ambient_environment_videos[i].setPaused(true);
			}

		}

	}

	
}



//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	
	is_midi_instrument_drag_event = false; 

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



