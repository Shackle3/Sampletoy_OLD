//
// Created by Fabian Uni on 31/03/2024.
//

#include "AudioSynthesis.h"
#include <math.h>

//DEFINING GLOBAL CONSTANTS

//macros
//
//Max possible synthesizers in playlist
#define MAX_SYNTHESIZERS 16 //Current supported max size (from code) is 255
//Setting time for once sample (double)
#define SAMPLE_DURATION 0.00002267573 //SAMPLE RATE IS FIXED TO 44.100 khz, WILL CAUSE BUGS IF CHANGED

//const variables
//
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

//Public Variables:
//
//List of how many samples a note_on has been signaled to synth, index corresponds to playlist order.
//

/// List of currently playing notes (nDFAWInfo[i] != 0),
/// how long (in samples) each note will be held for, and at what duration one phase has in time
//@todo Samples per smallest subdivision of note time should be calculated in the project initialisation, with BPM
unsigned int noteDurFreqAmpWaveInfo[MAX_SYNTHESIZERS][4]; //size max synths x 2
//noteDurationSamples[synth][0] := samples the note is held for
//noteDurationSamples[synth][1] := 1/frequency of that note, time it takes for one oscillation
//noteDurationSamples[synth][2] := Amplitude, equals (Note Velocity / 127) 127 being math velocity, max amp being 1
//noteDurationSamples[synth][3] := Wave type

/// List of current sample, so the code knows which phase to play.
/// When currentPlayingSampleNumber = noteDurationSamples, complete last wave computation and then
/// set noteDurationSamples[i] back to 0
unsigned int currentPlayingSampleNumber[MAX_SYNTHESIZERS];


//Functions:
//
//Assuming midiIn is >= 21, inline translates a midi code into a frequency.
double midi_frequency_translator(unsigned midiNoteCode){return NOTE_FREQUENCIES[midiNoteCode - 21];}

void parse_midi_input(unsigned noteCode, unsigned char velocity, unsigned duration,
                      unsigned synthNumber, unsigned char waveType){
    noteDurFreqAmpWaveInfo[synthNumber][0] = duration;
    noteDurFreqAmpWaveInfo[synthNumber][1] = 1 / midi_frequency_translator(noteCode);
    noteDurFreqAmpWaveInfo[synthNumber][2] = velocity / 127;
    noteDurFreqAmpWaveInfo[synthNumber][3] = waveType;

}

double math_wave_gen(unsigned char synthesiserNumber){
    /*
     * Generates value that a synthesiser returns at the current sample
     *
     * Inputs:
     *  waveType: the function that should be used to generate the wave. In oversight
     *      0: 0 funtion
     *      1: sin
     *      @todo MAKE MORE MATH FUNCTINOS, whatever you want. Planned implementation below
     *      2: cos
     *      3: square
     *      4: triange
     *      5: saw
     *
     *  synthesiserNumber: used to access information of that synthesiser and the various wave properties.
     *
     * Outputs:
     *  double: returns function value at that phase
     */

    //Get note properties
    unsigned char phaseDuration = noteDurFreqAmpWaveInfo[synthesiserNumber][1];
    unsigned char amplitude = noteDurFreqAmpWaveInfo[synthesiserNumber][2];
    //calculate time
    //we know frequency, follows time for one phase = 1/frequency
    // time/T1Phase * 2pi = phase
    double time = currentPlayingSampleNumber[synthesiserNumber] * SAMPLE_DURATION;
    double phase = (time/phaseDuration) * 2 *  M_PI;
    //Increment phase values
    currentPlayingSampleNumber[synthesiserNumber]++;
    //Generate function output
    switch (noteDurFreqAmpWaveInfo[synthesiserNumber][3]) {
        case 0:
            return 0;
        case 1:
            return sin(phase) * amplitude;
        default:
            return 0;
    }
}

void cleanup_finished_midi(){
    for (char synth = 0; synth < MAX_SYNTHESIZERS; synth++)
    {
        if (noteDurFreqAmpWaveInfo[synth][0] != 0){
            if (noteDurFreqAmpWaveInfo[synth][0] < currentPlayingSampleNumber[synth]){
                //case, the synth sample exceeds the duration specified on the creation
                //destroy note in memory
                currentPlayingSampleNumber[synth] = 0;
                noteDurFreqAmpWaveInfo[synth][0] = 0;
                noteDurFreqAmpWaveInfo[synth][1] = 0;
                noteDurFreqAmpWaveInfo[synth][2] = 0;
                noteDurFreqAmpWaveInfo[synth][3] = 0;
            }
        }
    }
}


