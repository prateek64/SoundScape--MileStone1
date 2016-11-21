#include "ofApp.h"



//--------------------------------------------------------------


// Initializes images, sounds, framerate of the application and other input files.

void ofApp::setup() {

	sound_stream.setup(this, 0, 1, 44100, buffersize, 2); // 2 output and 1 input channel with a sampling rate of 44100
	file_init();
	ofBackground(211, 211, 211); // Grey Background 
    ofSetFullscreen(true);
	ofSetFrameRate(30);

	left.resize(buffersize);
	left.assign(buffersize, 0.0);

	midiSetup();  // Setup for midi input 

	image_init(); // Loads all the images
	intitialize_drumkit_location();
	ofEnableDepthTest();
	death_star();

	for (int i = 0; i < 11; i++) {

		dragged[i] = false;
	}


	point_listener.x = 10;
	point_listener.y = ofGetScreenHeight()/2;

	double init_listener_x, init_listener_y, init_listener_z;

	init_listener_x = point_listener.x - ofGetScreenWidth() / 2;
	init_listener_z = point_listener.y - ofGetScreenHeight() / 2;
	init_listener_y = init_listener_x / 2 + init_listener_z / 2;
	create_sounds.move_listener(init_listener_x, init_listener_y, init_listener_z);

}

// Initializes the image files for the drum kit 
void ofApp::image_init() {


	for (int i = 0; i < 11; i++) {

		drumkit_parts[i].loadImage("Images/bubble.jpg");

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


// Initializes the drum kit sound locations in the 3D space (which are then rendered using HRTF)

void ofApp::intitialize_drumkit_location() {

	double init_radius = 40;
	int drum_sources = 11;
	int sign = 1;
	double theta = 0;

	for (int i = 0; i < drum_sources; i++) {


		drum_pos_x[i] = radius_of_rev*cos(2*PI*i/drum_sources) - 50 ;
		drum_pos_z[i] = radius_of_rev*sin(2*PI*i / drum_sources) - 50;
		drum_pos_y[i] = drum_pos_x[i]/2 + drum_pos_z[i]/2;
		create_sounds.add_source(drum_pos_x[i], drum_pos_y[i], drum_pos_z[i], i);

	}


}


// Method to draw the drum kit parts in the GUI 

void ofApp::draw_scene() {

	double theta = 0,x,y;


	for (int i = 0; i < 11; i++) {

		if(!dragged[i]){

			drum_kit[i].x = radius_of_rev*cos(2 * PI*i / 11);
			drum_kit[i].y = radius_of_rev*sin(2 * PI*i / 11);
			drumkit_parts[i].draw(drum_kit[i].x -50 , drum_kit[i].y -50 , drumkit_parts[i].getWidth() / 2, drumkit_parts[i].getHeight() / 2);

		}

		else  {

			
			drumkit_parts[i].draw(drum_kit[i].x, drum_kit[i].y, drumkit_parts[i].getWidth() / 2, drumkit_parts[i].getHeight() / 2);

		
	  }


  }


}

// Initializes the image files other than that contribute towards sound 
void ofApp::file_init() {


	mic_off.load("Images/mic_state_off.png");
	mic_on.load("Images/mic_state_on.png");
	alien.loadImage("Images/alien.png");
	road.loadImage("Images/road.jpg");
	song[0].loadImage("Images/song.jpg");
	song[1].loadImage("Images/song_1.jpg");

}

/*void ofApp::audioOut(float * out, int bufferSize, int nChannels) {


	double conv_buf[256];
	int conv_index = 0;

	if (impulse_resp.size() - buf_pos_end >= convolve_buf) {

		for (int i = buf_pos_begin; i < buf_pos_end; i++) {


			conv_buf[conv_index] = impulse_resp[i];
			conv_index++;

		}

		convolve = convolve1D(conv_buf, 44100, conv_index);


		buf_pos_begin = buf_pos_end;
		buf_pos_end = buf_pos_end + convolve_buf;
		conv_index = 0;

	}

	else {

		for (int i = buf_pos_end; i < impulse_resp.size(); i++) {


			conv_buf[conv_index] = impulse_resp[i];
			conv_index++;

		}

		convolve = convolve1D(conv_buf, 44100, conv_index);

		buf_pos_begin = 0;
		buf_pos_end = convolve_buf;
		conv_index = 0;


	}


	for (int i = 0; i < 44100 + 256; i = i + 2) {


		out[i] = 0.5*output[i];
		out[i + 1] = 0.25*output[i];

	}




} */


//--------------------------------------------------------------
void ofApp::update() {

	ofSoundUpdate();
	
	if (is_mic_on) {

		scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
		move_drums();

	}

	death_star_angle = death_star_angle + 3;
	theta = theta + 0.1;

}


void ofApp::death_star() {


	nTri =500;			//The number of the triangles
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
		colors[i] = ofColor((int)255*cos(phase), 0 , (int)255*sin(phase));
		phase = phase + 0.005;

	}


}


//--------------------------------------------------------------
void ofApp::draw() {

	int listener_x, listener_y, listener_z;
	

	ofPushMatrix();
	ofTranslate(300, 300);
	
	ofSetColor(211, 211, 211);
	song[0].draw(100 * cos(theta) - 20, 100 * sin(theta) - 20, song[0].getWidth()/2, song[0].getHeight()/2);
	song[1].draw(-100 * cos(theta) + 20 , -100 * sin(theta) + 20, song[0].getWidth()/2, song[0].getHeight()/2);
	ofPopMatrix();

	ofPushMatrix();
		
		ofSetColor(211, 211, 211);
		displace_listener();

		listener_x = point_listener.x - ofGetScreenWidth() / 2;
		listener_z = point_listener.y - ofGetScreenHeight() / 2;
		listener_y = (listener_x + listener_z) / 2;

		create_sounds.move_listener(listener_x, listener_y, listener_z);

	ofPopMatrix();
	
	ofPushStyle();

	

	ofEnableAlphaBlending();

	if (draw_mic_off == true) {

		ofSetColor(11, 220, 198, 255);
		mic_off.draw(0, 0, border_len, border_len);

		ofSetColor(11, 220, 198, 0);
		mic_on.draw(0, 0, border_len, border_len,5);

	}

	 if (draw_mic_on == true) {

		ofSetColor(11, 220, 198, 0);
		mic_off.draw(0, 0,4, border_len, border_len);

		ofSetColor(11, 220, 198, 255);
		mic_on.draw(0, 0,5, border_len, border_len);


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

	// death_star();
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

	// death_star();
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

	ofRotateX(-2*death_star_angle);
	ofRotateY(-death_star_angle);
	ofPushStyle();

//	death_star();
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

// Method to move the alien visitor around in the sound field

void ofApp::displace_listener() {

	alien.draw(point_listener.x, point_listener.y, -3, alien.getWidth()/1.5, alien.getHeight()/1.5);

}

// Method to move the drum according to the mic input 
void ofApp::move_drums() {

	double distance[4]; 
	for (int i = 0; i < 11; i++) {

		dragged[i] = true;
		drum_kit[i].x = drum_kit[i].x - 5* scaledVol*change_pos_x[i];
		drum_kit[i].y = drum_kit[i].y - 5 * scaledVol*change_pos_y[i];
		change_drum_pos(i);

		distance[0] = sqrt(pow(drum_kit[i].x, 2) + pow(drum_kit[i].y, 2));

		distance[1] = sqrt(pow(drum_kit[i].x + drumkit_parts[i].getWidth()/2, 2) + pow(drum_kit[i].y, 2));
		distance[2] = sqrt(pow(drum_kit[i].x + drumkit_parts[i].getWidth() / 2, 2) + pow(drum_kit[i].y + drumkit_parts[i].getHeight() / 2, 2));
		distance[3] = sqrt(pow(drum_kit[i].x, 2) + pow(drum_kit[i].y + drumkit_parts[i].getHeight() / 2, 2));

		if (distance[0] > Rad - 20) {

			change_pos_x[i] = -change_pos_x[i];
			change_pos_y[i] = -change_pos_y[i];

		}

		if (distance[1] > Rad - 20) {

			change_pos_x[i] = -change_pos_x[i];
			change_pos_y[i] = - change_pos_y[i];


		}

		if (distance[2] > Rad - 20) {

			change_pos_x[i] = -change_pos_x[i];
			change_pos_y[i] = -change_pos_y[i];


		}

		if (distance[3] > Rad - 20) {

			change_pos_x[i] = -change_pos_x[i];
			change_pos_y[i] = - change_pos_y[i];


		}

		
	}


}

// Method to move drums sounds  in the 3D space 
void ofApp::change_drum_pos(int which_drum) {


	drum_pos_x[which_drum] = drum_kit[which_drum].x ;
	drum_pos_z[which_drum] = drum_kit[which_drum].y  ;
	drum_pos_y[which_drum] = (drum_pos_x[which_drum] + drum_pos_z[which_drum]) / 2;


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
}

void ofApp::audioIn(float * input, int bufferSize, int nChannels) {

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

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	

	if (key == OF_KEY_UP) {

		point_listener.y = point_listener.y - listener_dy;

	}

	if (key == OF_KEY_DOWN) {

		point_listener.y = point_listener.y + listener_dy;

	}

	if (key == OF_KEY_LEFT) {

		point_listener.x = point_listener.x - listener_dx;

	}

	if (key == OF_KEY_RIGHT) {

		point_listener.x = point_listener.x + listener_dx;

	}

	if (key == 114) {

		for (int i = 0; i < 11; i++) {


			drum_kit[i].x = radius_of_rev*cos(2 * PI*i / 11);
			drum_kit[i].y = radius_of_rev*sin(2 * PI*i / 11);
			drumkit_parts[i].draw(drum_kit[i].x - 50, drum_kit[i].y - 50, drumkit_parts[i].getWidth() / 2, drumkit_parts[i].getHeight() / 2);
			dragged[i] = false;

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

	for (int i = 0; i < 11; i++) {

		if (abs(x - drum_kit[i].x - ofGetScreenWidth() / 2) <= drumkit_parts[i].getWidth() / 2 && abs(y - drum_kit[i].y - ofGetScreenHeight() / 2) <= drumkit_parts[i].getHeight() / 2) {

			drum_kit[i].x = x - ofGetScreenWidth()/2 - drumkit_parts[i].getWidth()/4;
			drum_kit[i].y = y - ofGetScreenHeight() / 2 - drumkit_parts[i].getHeight() / 4;
			change_drum_pos(i);
			dragged[i] = true;
			break;
		}


	}
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(ofMouseEventArgs &mouse) {

	
	if (mouse.x >= 0 && mouse.x <=border_len && mouse.y >= 0 && mouse.y <= border_len) {

		
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

