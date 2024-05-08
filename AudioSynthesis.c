//
// Created by Fabian Uni on 31/03/2024.
//

#include "AudioSynthesis.h"

//DEFINING GLOBAL CONSTANTS
//macros

//Max possible synthesizers in playlist
#define MAX_SYNTHESIZERS 16 //Current supported max size (from code) is 255

//const variables
//https://newt.phys.unsw.edu.au/jw/notes.html
const double NOTE_FREQUENCIES[] = {27.5, 29.135, 30.868, 32.703, 34.648, 36.708, 38.891,
                                   41.203, 43.654, 46.249, 48.999, 51.913, 55, 58.27,
                                   61.735, 65.406, 69.296, 73.416, 77.782, 82.407,
                                   87.307, 92.499, 97.999, 103.83, 110, 116.54,
                                   123.47, 130.81, 138.59, 146.83, 155.56, 164.81,
                                   174.61, 185, 196, 207.65, 220, 233.08, 246.94,
                                   261.63, 277.18, 293.67, 311.13, 329.63, 349.23,
                                   369.99, 392, 415.3, 440, 466.16, 493.88, 523.25,
                                   554.37, 587.33, 622.25, 659.26, 698.46, 739.99,
                                   783.99, 830.61, 880, 932.33, 987.77, 1046.5,
                                   1108.7, 1174.7, 1244.5, 1318.5, 1396.9, 1480,
                                   1568, 1661.2, 1760, 1864.7, 1975.5, 2093, 2217.5,
                                   2349.3, 2489, 2637, 2793, 2960, 3136, 3322.4,
                                   3520, 3729.3, 3951.1, 4186};


//List of how many samples a noteon has been signaled to synth, index corresponds to playlist order.
//@todo not fully implemented until playlist gets implemented, for now declaring a maximum of 16 v-synths in a project
unsigned int noteSampleNum[MAX_SYNTHESIZERS]; // 16 corresponds to 16 possible usable synth
//automatically initialised to 0 btw POTENTIALLY REDUNDANT

/// List of currently playing notes (cNDS[i] != 0) and how long (in samples) each note will be held for
//@todo Samples per smallest subdivision of note time should be calculated in the project initialisation, with BPM
unsigned int currentNoteDurationSamples[MAX_SYNTHESIZERS];

/// List of current sample, so the code knows which phase to play.
/// When currentPlayingSampleNumber = noteDurationSamples, complete last wave computation and then
/// set noteDurationSamples[i] back to 0
unsigned int currentPlayingSampleNumber[MAX_SYNTHESIZERS];

//Assuming midiIn is >= 21
double midi_frequency_translator(unsigned midiNoteCode){return NOTE_FREQUENCIES[midiNoteCode - 21];}


void remove_finished_midi (){
    /*
     * //Compare current playing to intended duration, remove finished notes
     * @todo definite time save possible, change for loop to Stack to avoid checking disabled synthesizers
     */
    for (unsigned char synthesizer = 0; synthesizer < MAX_SYNTHESIZERS; synthesizer++ ){
        //For index position corresponding to synth, do:
        if (&currentNoteDurationSamples[synthesizer] != 0){
            //exclude disabled synths, try to find way to bring this logical step in before
            if (currentNoteDurationSamples[synthesizer] == currentPlayingSampleNumber[synthesizer]) {
                //&todo investigate pointers here
                currentNoteDurationSamples[synthesizer] = 0;
                currentPlayingSampleNumber[synthesizer] = 0;
            }
        }
    }

}



//double math_wave_gen(unsigned char waveType)