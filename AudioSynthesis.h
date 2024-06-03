//
// Created by Fabian Uni on 31/03/2024.
//

#include <stdint.h>
#include <stdbool.h>
#ifndef SAMPLETOYC_AUDIOSYNTHESIS_H
#define SAMPLETOYC_AUDIOSYNTHESIS_H
#define MAX_SYNTHESIZERS 16 //Current supported max size (from code) is 255

extern const double NOTE_FREQUENCIES[];
/*
 *https://newt.phys.unsw.edu.au/jw/notes.html
 * range from index 0 - 87, corresponding
 */

extern float samplePerSmallestSubdivision;
/*
 * Calculates the amount of samples between a smallest subdivision on the grid.
 * For current smallest subdivisions of time check SUBDIVISIONS_PER_BEAT ->
 *  1/SUBDIVISIONS_PER_BEAT beats is the smallest subdivision
 */

struct MathSynthHost{
    unsigned int samplesHeldFor; // num of samples the note is held for
    double notePeriod; // 1/frequency of that note, time it takes for one oscillation
    float noteAmplitude; // 1 corresponds to 0 db, @todo logarithmic
    uint8_t waveType; // Setting as to which math wave should be modelled
};

typedef struct MathSynthHost mathsynth;

extern mathsynth mathSynthInstances[];
/*
 * Array of math synthesisers, see above for struct implementation
 *
 * When samplesheldfor = noteDurationSamples, complete last wave computation and then
 * set noteDurationSamples[i] back to 0 using (and handled by) cleanup_finished_midi
 */

extern unsigned int currentPlayingSampleNumber[];
/*
 * List, equal length to NDFAWI containing the sample number a synthesiser is currently playing.
 *
 * Order is important
 */

//Functions

double midi_frequency_translator(unsigned midiNoteCode);
/*
 * translates a midi code into a corresponding index for NOTE_FREQUENCIES, then returns frequency
 *
 * Input:
 *  midiNoteCode: A midi code for a note
 *
 * Output:
 *  Note frequency
 */

__attribute__((unused)) void update_samples_per_smallest_subdiv(uint8_t bpm);
/*
 * Updates samplePerSmallestSubdivision to represent the smallest possible subdivision on the grid.
 *
 * This is important and should be used after every BPM change in the project. It is required to calculate the amount
 * of samples that need to generated for a note.
 *
 * Input:
 *  bpm: new BPM of project
 */

void parse_midi_input(unsigned noteCode, uint8_t velocity, unsigned  duration, unsigned synthNumber, uint8_t waveType);
/*
 * Takes a midi input, and adds it to the NDFAWI registery.
 *
 * Input:
 *  noteCode: Midi code for the note, passed to midi_frequency_translator
 *  velocity: Velocity of the note, max is 127. Hard coded to calculate amplitude
 *  duration: duration in smallest subdivisions for the note
 *  synthNumber: corresponding index to which this information should be added
 *  waveType: type of wave that should be generated:
     *      0: 0 function
     *      1: sin
     *      2: cos
     *      @ todo MAKE MORE MATH FUNCTIONS, whatever you want. Planned implementation below
     *      3: square
     *      4: triange
     *      5: saw
 */

double math_wave_gen(uint8_t synthesiserNumber);
/*
 * Generates a function value for a synthesiser index, returning synth output voltage at a certain sample.
 *
 * Inputs:
 *  synthesiserNumber: index at which synthesiser the sample should be generated
 *
 * Outputs:
 *  voltage over output terminals of digital synth.
 */

void cleanup_finished_midi();
/*
 * Cleans up note information from expired notes. Run at the end of a sample loop in main.
 */

void initialise_the_mathsynths();
#endif //SAMPLETOYC_AUDIOSYNTHESIS_H
