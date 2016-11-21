# SoundScape--MileStone1
This is my first milestone for my final project. (REQUIRES HEADPHONES)

I am exploring the creation of soundscapes ( rendering 3D audio ) using HRTF's on headphones. Currently I am using the following addons/libraries:

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

Coming Up 

1. Granular Synthesis and some real time FM synthesis
2. A new view which explores ambient sound sources around us ( rain, wind, thunder etc.) in different locations in 3D ( maybe different locations on the earth). 

3. Better Graphics for each view
