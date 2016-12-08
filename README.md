# The SoundScape 

This is my Music 256A final project (I call it the SoundScape) . The explanation of the project has been divided into the following five parts :- 

1. Introduction and Motivation 

2. Application Components 

3. Software Components 

4. Setting up the project on your system 

5. Resources 


# 1. Introduction and Motivation

When I heard the Virtual Barbershop(https://www.youtube.com/watch?v=IUDTlvagjJA)  for the first time six years ago (which employs HRTF's and spatialized audio rendering) it triggered my curiousity and imagination. The first time I heard it I was blown away by the fact that our ears could be employed in this way to fake 3D positions of audio sources. 
Rendering spatialized audio for headphones using HRTF's has always excited me and I felt this course gave me a perfect platform to build something on the lines of rendering HRTF convolved audio and spatialized audio.

My application aims at creating different soundscapes within a single screen so that the listener can experience different acoustic spaces (both synthetic and ambient) within the same restricted area. The components and the different soundscapes are explained in the following part. 

The abstractness of the user interface reflects my thoguhts, my love for what I do, my past experiences and everything I appreciate in this world. 

# 2. Application components 

 1. The Spatialized MIDI playing area :- This soundscape lets the user play the MIDI notes and instruments in the 3D audio scene that I have created in my application. Each note is depicted using a graphic (a bokeh in this case) and can be moved around in the GUI (works best with a touch screen). The device screen lets the user move the notes in the Z and X axis . Y axis displacement is randomly selected within a small range ( negative to positive values on the y-axis ). 

 2. Ambient World Soundscape : - This soundscape lets the user move everything the mic hears from the outside world in the 3D audio scene that we have created in our application. If the headphones provide decent noise cancellation, moving the ambient environment around you in the 3D sound field around your head sounds really interesting 

 3. Earth View SoundScape :- This view provides a bigger soundscape by presenting a earth simulation with the geo-tagged audio recorded by a user around the globe being mapped onto the earth map. All these audio recordings are also getting spatialized using the same spatialization method. An audio file parser parses the audio files to get the latitude and longitude information and then maps these latitudes and longitudes onto the earth simulation 

 4. Listener SoundScape :- Remember the listener has its own soundscape i.e the the listener has its own sound field which has to mingle with the sound field of the sources to actually experience the in 3D. Thus the listener in the application (depicted as an abstract alien) has to physically move to the different soundscapes to experience them. 

 5. Distance Models - I have employed an exponential model for the decay of the sound sources with the increasing distance from the listener. The sound levels remain constant within a specified radius and then the sound level dies out exponentially after the specified distance from the source. 

 6. Real Time Reverb : The application gives you an option to add reverb from four different locations ( St. Peter's Basilica, an auditorium, underwater and a synthetic psychotic location). This lets you experience the soundscapes within different acoustic spaces from real and synthetic environments.

 7. Movement of soundscape - Each sound source, listener, ambient environment can be moved in the 3D audio space that is being created through my application. I guess the importance lies a lot on the position of the listener as that is how we hear whatever there is out in the environment. 
 
 8. MIDI :- As the application lets you play on a MIDI keyboard, the MIDI forms another important part of the application. 

# 3 Software Components  

My application has various software components, they are as follows : 

 1. Audio Scene Creator class : This clas handles the 3D rendering and the HRTF convolution of the audio. It provides the following   functions:- 

   a. Moving the sound sources around in the 3D using an input X,Y,Z position with respect to a origin.
  
   b. Adding new sources to the 3D audio scene.
  
   c. Add doppler effect to the moving sources.
  
   d. Play the sources and stop the sources.
  
   e. Add reverb effects to the synthetic and real time mic ambient environment.
  
   f. Change the gain and pitch of the sound sources 
  
   g. Move the listener around in the 3D audio scene around the head 
  

 2. Convolution Thread Class : This class performs the fast convolution that renders the real time reverb for the ambient environment ( the real time mic input). This is a parallel thread ( ofThread() openframeworks ) that runs parallel to the main thread to perform the low latency convolution for the real time mic input. A separate thread was made for this purpose as the convolution can be a slower process( even though a fast low latency implementation) and can block the faster graphics rate of the main thread. 
 
 
 3. OfApp Class: This class handles the main chunk of the application features. I see this as a central class to which everything is connected. It provides the following functions :-
 
  a. GUI display and updating the graphical elements of the application 
  
  b. Mouse and Key events 
  
  c. MIDI note events 
  
  d. Image and Video loading 
  
  e. Initialization of the Convolution and Audio Scene Creator class 
  
  f. Calling the Audio Scene Creator and Convolution thread functions 
  
  g. Audio input and Output callback functions 
  
  h. 


a. OpenAL SOft (https://github.com/kcat/openal-soft)

b. OpenAL (https://www.openal.org/)

c. ofxMIDI (https://github.com/danomatika/ofxMidi)

d. HRTF's from IRCAM  (http://recherche.ircam.fr/equipes/salles/listen/)

Currently my system includes an alien visitor who is exploring the world of spatial audio. The GUI has different sound fields where the alien can enter and exit. The main feature is an aura which when you enter lets you play Midi notes in the 3D space around your head  ( while wearing your headphones). The alien can go to different locations and experience the Midi drum kit, move the notes in the 3D space and play them in real time. The alien can also use the mic input to randomly move the notes around his/her head (using input rms value).

Some Features of the current system :

1. User can add doppler effect by giving in the speed of sound and the doppler effect factor (Audio_Scene_Creator::add_doppler_effect(int speed_of_sound, int doppler_effect))

2. The movement on the screen is replicated in the sound source positions in the headphones by real time HRTF convolution

3. Listener position can change so you can experience audio from different locations.

4. Mic input rms value can move the sources around in 3D. 

5. You can pitch shift each midi note using a midi control knob.(void Audio_Scene_Creator::pitch_shifting(int val))

