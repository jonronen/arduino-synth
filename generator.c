/*
 *
 * Arduino Synth
 * Copyright 2012-2013 Jon Ronen-Drori
 *
 *
 * A simple Arduino-based one-note synthesizer
 * with vibrato, low-pass filter, resonance,
 * distortion, and tremolo
 *
 * Inspiration and pieces of code taken from:
 * - Auduino (http://code.google.com/p/tinkerit/wiki/Auduino)
 * - Arduino Octosynth (http://www.instructables.com/id/The-Arduino-OctoSynth/)
 * - sfxr (http://www.drpetter.se/project_sfxr.html)
 * - Speaker PCM (http://www.arduino.cc/playground/Code/PCMAudio)
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "Arduino.h"

// digital inputs
#define TONE1_PIN       9
#define TONE2_PIN       10
#define TONE3_PIN       11
#define TONE4_PIN       12
#define TONE5_PIN       13
//
// Note:
// TONE5_PIN should be changed if you really want to use an Arduino Uno...
// pin #13 is usually the LED and may not work properly as an input
//

#define LOWPASS_PIN     5
#define FREQ_PIN        6
#define DIST_PIN        7
#define TREM_PIN        8

// "analog" output
#define PWM_PIN       3
#define PWM_VALUE     OCR2B

// interrupt on timer comparison
#define PWM_INTERRUPT TIMER1_COMPA_vect

#define ATTACK_CTRL 0
#define RELEASE_CTRL 1
#define VIBRATO_CTRL 2
#define RESONANCE_CTRL 3

void setup();
void loop();
static void reset_sample();

// 0 for rough sawtooth, 1 for sawtooth, 2 for sine, 3 for square
static const unsigned char g_wave_type = 1;

// tone frequency
static unsigned short g_base_freq;
static unsigned short g_curr_freq;
static unsigned short g_freq_ramp_cnt;

// base frequency generation
static unsigned short g_phase;

//
// envelope
//

// type: 0=amplitude, 1=low-pass, 2=frequency
static unsigned char g_env_type;
// stage: 0=attack, 1=sustain, 2=release, 3=silence
static unsigned char g_env_stage;
static unsigned short g_env_time;
static unsigned short g_env_lengths[2];

// low-pass filter
static unsigned char g_lpf_resonance;
static unsigned char g_lpf_freq;
static short g_lpf_prev;
static short g_lpf_prev_delta;

// vibrato
static unsigned short g_vib_phase;
static unsigned char g_vib_strength;
static short g_vib_fix_accum;

// tremolo and distortion
static unsigned short g_interrupt_cnt;

// buttons
static uint8_t g_button_status;
static uint8_t g_current_pitch_bit;
static uint8_t g_dist_status;
static uint8_t g_trem_status;

static const short phase_to_delta[4] = {1,-1,-1,1};

static const unsigned char g_rand_shit[256] = {
  127, 245, 223, 255, 254, 255, 255, 255, 255, 191, 254, 255, 255, 255, 255, 223,
  255, 191, 254, 247, 255, 255, 255, 255, 223, 255, 61,  255, 183, 255, 255, 255,
  255, 191, 249, 255, 255, 245, 255, 250, 215, 255, 255, 255, 127, 255, 255, 255,
  255, 223, 255, 255, 255, 255, 255, 255, 255, 127, 255, 254, 243, 255, 239, 255,
  237, 255, 255, 255, 255, 247, 255, 255, 255, 255, 187, 255, 251, 251, 255, 223,
  255, 253, 255, 255, 255, 255, 127, 255, 191, 255, 255, 255, 255, 255, 223, 255,
  255, 251, 123, 255, 253, 254, 239, 251, 255, 255, 223, 255, 255, 255, 255, 255,
  251, 255, 251, 223, 255, 231, 251, 255, 255, 255, 221, 255, 251, 255, 255, 223,
  255, 219, 255, 245, 255, 255, 255, 255, 255, 255, 251, 255, 59,  255, 255, 223,
  255, 191, 255, 127, 255, 223, 255, 127, 255, 255, 255, 255, 255, 255, 255, 119,
  255, 255, 255, 253, 255, 255, 253, 255, 255, 254, 252, 247, 255, 255, 255, 62,
  255, 251, 247, 255, 189, 255, 255, 255, 255, 126, 251, 253, 255, 255, 191, 255,
  255, 127, 222, 255, 159, 255, 255, 249, 255, 255, 191, 255, 254, 255, 255, 255,
  255, 207, 221, 251, 253, 255, 255, 255, 190, 191, 255, 255, 255, 255, 127, 255,
  255, 243, 255, 247, 255, 255, 253, 255, 255, 223, 255, 255, 255, 251, 255, 255,
  255, 191, 255, 254, 191, 255, 255, 255, 251, 255, 255, 191, 255, 255, 255, 255
};

/* TODO: for some reason, this doesn't work properly
const prog_int16_t mysin_table[2048] = {
   0,  3,  6,  9,  12,  15,  18,  21,  
   25,  28,  31,  34,  37,  40,  43,  47,  
   50,  53,  56,  59,  62,  65,  69,  72,  
   75,  78,  81,  84,  87,  90,  94,  97,  
   100,  103,  106,  109,  112,  115,  119,  122,  
   125,  128,  131,  134,  137,  140,  144,  147,  
   150,  153,  156,  159,  162,  165,  168,  171,  
   175,  178,  181,  184,  187,  190,  193,  196,  
   199,  202,  205,  209,  212,  215,  218,  221,  
   224,  227,  230,  233,  236,  239,  242,  245,  
   248,  251,  254,  257,  260,  264,  267,  270,  
   273,  276,  279,  282,  285,  288,  291,  294,  
   297,  300,  303,  306,  309,  312,  315,  318,  
   321,  324,  327,  330,  333,  336,  339,  342,  
   344,  347,  350,  353,  356,  359,  362,  365,  
   368,  371,  374,  377,  380,  383,  386,  388,  
   391,  394,  397,  400,  403,  406,  409,  412,  
   414,  417,  420,  423,  426,  429,  432,  434,  
   437,  440,  443,  446,  449,  451,  454,  457,  
   460,  463,  466,  468,  471,  474,  477,  479,  
   482,  485,  488,  491,  493,  496,  499,  501,  
   504,  507,  510,  512,  515,  518,  521,  523,  
   526,  529,  531,  534,  537,  539,  542,  545,  
   547,  550,  553,  555,  558,  561,  563,  566,  
   568,  571,  574,  576,  579,  581,  584,  587,  
   589,  592,  594,  597,  599,  602,  604,  607,  
   609,  612,  615,  617,  620,  622,  625,  627,  
   629,  632,  634,  637,  639,  642,  644,  647,  
   649,  652,  654,  656,  659,  661,  664,  666,  
   668,  671,  673,  675,  678,  680,  683,  685,  
   687,  690,  692,  694,  696,  699,  701,  703,  
   706,  708,  710,  712,  715,  717,  719,  721,  
   724,  726,  728,  730,  732,  735,  737,  739,  
   741,  743,  745,  748,  750,  752,  754,  756,  
   758,  760,  762,  765,  767,  769,  771,  773,  
   775,  777,  779,  781,  783,  785,  787,  789,  
   791,  793,  795,  797,  799,  801,  803,  805,  
   807,  809,  811,  813,  814,  816,  818,  820,  
   822,  824,  826,  828,  829,  831,  833,  835,  
   837,  839,  840,  842,  844,  846,  847,  849,  
   851,  853,  854,  856,  858,  860,  861,  863,  
   865,  866,  868,  870,  871,  873,  875,  876,  
   878,  879,  881,  883,  884,  886,  887,  889,  
   890,  892,  894,  895,  897,  898,  900,  901,  
   903,  904,  906,  907,  908,  910,  911,  913,  
   914,  916,  917,  918,  920,  921,  922,  924,  
   925,  927,  928,  929,  930,  932,  933,  934,  
   936,  937,  938,  939,  941,  942,  943,  944,  
   946,  947,  948,  949,  950,  951,  953,  954,  
   955,  956,  957,  958,  959,  960,  962,  963,  
   964,  965,  966,  967,  968,  969,  970,  971,  
   972,  973,  974,  975,  976,  977,  978,  978,  
   979,  980,  981,  982,  983,  984,  985,  986,  
   986,  987,  988,  989,  990,  990,  991,  992,  
   993,  994,  994,  995,  996,  997,  997,  998,  
   999,  999,  1000,  1001,  1001,  1002,  1003,  1003,  
   1004,  1004,  1005,  1006,  1006,  1007,  1007,  1008,  
   1008,  1009,  1009,  1010,  1010,  1011,  1011,  1012,  
   1012,  1013,  1013,  1014,  1014,  1015,  1015,  1015,  
   1016,  1016,  1017,  1017,  1017,  1018,  1018,  1018,  
   1019,  1019,  1019,  1019,  1020,  1020,  1020,  1020,  
   1021,  1021,  1021,  1021,  1022,  1022,  1022,  1022,  
   1022,  1022,  1023,  1023,  1023,  1023,  1023,  1023,  
   1023,  1023,  1023,  1023,  1023,  1023,  1023,  1023,  
   1023,  1023,  1023,  1023,  1023,  1023,  1023,  1023,  
   1023,  1023,  1023,  1023,  1023,  1023,  1023,  1022,  
   1022,  1022,  1022,  1022,  1022,  1021,  1021,  1021,  
   1021,  1020,  1020,  1020,  1020,  1019,  1019,  1019,  
   1019,  1018,  1018,  1018,  1017,  1017,  1017,  1016,  
   1016,  1015,  1015,  1015,  1014,  1014,  1013,  1013,  
   1012,  1012,  1011,  1011,  1010,  1010,  1009,  1009,  
   1008,  1008,  1007,  1007,  1006,  1006,  1005,  1004,  
   1004,  1003,  1003,  1002,  1001,  1001,  1000,  999,  
   999,  998,  997,  997,  996,  995,  994,  994,  
   993,  992,  991,  990,  990,  989,  988,  987,  
   986,  986,  985,  984,  983,  982,  981,  980,  
   979,  978,  978,  977,  976,  975,  974,  973,  
   972,  971,  970,  969,  968,  967,  966,  965,  
   964,  963,  962,  960,  959,  958,  957,  956,  
   955,  954,  953,  951,  950,  949,  948,  947,  
   946,  944,  943,  942,  941,  939,  938,  937,  
   936,  934,  933,  932,  930,  929,  928,  927,  
   925,  924,  922,  921,  920,  918,  917,  916,  
   914,  913,  911,  910,  908,  907,  906,  904,  
   903,  901,  900,  898,  897,  895,  894,  892,  
   890,  889,  887,  886,  884,  883,  881,  879,  
   878,  876,  875,  873,  871,  870,  868,  866,  
   865,  863,  861,  860,  858,  856,  854,  853,  
   851,  849,  847,  846,  844,  842,  840,  839,  
   837,  835,  833,  831,  829,  828,  826,  824,  
   822,  820,  818,  816,  814,  813,  811,  809,  
   807,  805,  803,  801,  799,  797,  795,  793,  
   791,  789,  787,  785,  783,  781,  779,  777,  
   775,  773,  771,  769,  767,  765,  762,  760,  
   758,  756,  754,  752,  750,  748,  745,  743,  
   741,  739,  737,  735,  732,  730,  728,  726,  
   724,  721,  719,  717,  715,  712,  710,  708,  
   706,  703,  701,  699,  696,  694,  692,  690,  
   687,  685,  683,  680,  678,  675,  673,  671,  
   668,  666,  664,  661,  659,  656,  654,  652,  
   649,  647,  644,  642,  639,  637,  634,  632,  
   629,  627,  625,  622,  620,  617,  615,  612,  
   609,  607,  604,  602,  599,  597,  594,  592,  
   589,  587,  584,  581,  579,  576,  574,  571,  
   568,  566,  563,  561,  558,  555,  553,  550,  
   547,  545,  542,  539,  537,  534,  531,  529,  
   526,  523,  521,  518,  515,  512,  510,  507,  
   504,  501,  499,  496,  493,  491,  488,  485,  
   482,  479,  477,  474,  471,  468,  466,  463,  
   460,  457,  454,  451,  449,  446,  443,  440,  
   437,  434,  432,  429,  426,  423,  420,  417,  
   414,  412,  409,  406,  403,  400,  397,  394,  
   391,  388,  386,  383,  380,  377,  374,  371,  
   368,  365,  362,  359,  356,  353,  350,  347,  
   344,  342,  339,  336,  333,  330,  327,  324,  
   321,  318,  315,  312,  309,  306,  303,  300,  
   297,  294,  291,  288,  285,  282,  279,  276,  
   273,  270,  267,  264,  260,  257,  254,  251,  
   248,  245,  242,  239,  236,  233,  230,  227,  
   224,  221,  218,  215,  212,  209,  205,  202,  
   199,  196,  193,  190,  187,  184,  181,  178,  
   175,  171,  168,  165,  162,  159,  156,  153,  
   150,  147,  144,  140,  137,  134,  131,  128,  
   125,  122,  119,  115,  112,  109,  106,  103,  
   100,  97,  94,  90,  87,  84,  81,  78,  
   75,  72,  69,  65,  62,  59,  56,  53,  
   50,  47,  43,  40,  37,  34,  31,  28,  
   25,  21,  18,  15,  12,  9,  6,  3,  
   0,  -3,  -6,  -9,  -12,  -15,  -18,  -21,  
   -25,  -28,  -31,  -34,  -37,  -40,  -43,  -47,  
   -50,  -53,  -56,  -59,  -62,  -65,  -69,  -72,  
   -75,  -78,  -81,  -84,  -87,  -90,  -94,  -97,  
   -100,  -103,  -106,  -109,  -112,  -115,  -119,  -122,  
   -125,  -128,  -131,  -134,  -137,  -140,  -144,  -147,  
   -150,  -153,  -156,  -159,  -162,  -165,  -168,  -171,  
   -175,  -178,  -181,  -184,  -187,  -190,  -193,  -196,  
   -199,  -202,  -205,  -209,  -212,  -215,  -218,  -221,  
   -224,  -227,  -230,  -233,  -236,  -239,  -242,  -245,  
   -248,  -251,  -254,  -257,  -260,  -264,  -267,  -270,  
   -273,  -276,  -279,  -282,  -285,  -288,  -291,  -294,  
   -297,  -300,  -303,  -306,  -309,  -312,  -315,  -318,  
   -321,  -324,  -327,  -330,  -333,  -336,  -339,  -342,  
   -344,  -347,  -350,  -353,  -356,  -359,  -362,  -365,  
   -368,  -371,  -374,  -377,  -380,  -383,  -386,  -388,  
   -391,  -394,  -397,  -400,  -403,  -406,  -409,  -412,  
   -414,  -417,  -420,  -423,  -426,  -429,  -432,  -434,  
   -437,  -440,  -443,  -446,  -449,  -451,  -454,  -457,  
   -460,  -463,  -466,  -468,  -471,  -474,  -477,  -479,  
   -482,  -485,  -488,  -491,  -493,  -496,  -499,  -501,  
   -504,  -507,  -510,  -512,  -515,  -518,  -521,  -523,  
   -526,  -529,  -531,  -534,  -537,  -539,  -542,  -545,  
   -547,  -550,  -553,  -555,  -558,  -561,  -563,  -566,  
   -568,  -571,  -574,  -576,  -579,  -581,  -584,  -587,  
   -589,  -592,  -594,  -597,  -599,  -602,  -604,  -607,  
   -609,  -612,  -615,  -617,  -620,  -622,  -625,  -627,  
   -629,  -632,  -634,  -637,  -639,  -642,  -644,  -647,  
   -649,  -652,  -654,  -656,  -659,  -661,  -664,  -666,  
   -668,  -671,  -673,  -675,  -678,  -680,  -683,  -685,  
   -687,  -690,  -692,  -694,  -696,  -699,  -701,  -703,  
   -706,  -708,  -710,  -712,  -715,  -717,  -719,  -721,  
   -724,  -726,  -728,  -730,  -732,  -735,  -737,  -739,  
   -741,  -743,  -745,  -748,  -750,  -752,  -754,  -756,  
   -758,  -760,  -762,  -765,  -767,  -769,  -771,  -773,  
   -775,  -777,  -779,  -781,  -783,  -785,  -787,  -789,  
   -791,  -793,  -795,  -797,  -799,  -801,  -803,  -805,  
   -807,  -809,  -811,  -813,  -814,  -816,  -818,  -820,  
   -822,  -824,  -826,  -828,  -829,  -831,  -833,  -835,  
   -837,  -839,  -840,  -842,  -844,  -846,  -847,  -849,  
   -851,  -853,  -854,  -856,  -858,  -860,  -861,  -863,  
   -865,  -866,  -868,  -870,  -871,  -873,  -875,  -876,  
   -878,  -879,  -881,  -883,  -884,  -886,  -887,  -889,  
   -890,  -892,  -894,  -895,  -897,  -898,  -900,  -901,  
   -903,  -904,  -906,  -907,  -908,  -910,  -911,  -913,  
   -914,  -916,  -917,  -918,  -920,  -921,  -922,  -924,  
   -925,  -927,  -928,  -929,  -930,  -932,  -933,  -934,  
   -936,  -937,  -938,  -939,  -941,  -942,  -943,  -944,  
   -946,  -947,  -948,  -949,  -950,  -951,  -953,  -954,  
   -955,  -956,  -957,  -958,  -959,  -960,  -962,  -963,  
   -964,  -965,  -966,  -967,  -968,  -969,  -970,  -971,  
   -972,  -973,  -974,  -975,  -976,  -977,  -978,  -978,  
   -979,  -980,  -981,  -982,  -983,  -984,  -985,  -986,  
   -986,  -987,  -988,  -989,  -990,  -990,  -991,  -992,  
   -993,  -994,  -994,  -995,  -996,  -997,  -997,  -998,  
   -999,  -999,  -1000,  -1001,  -1001,  -1002,  -1003,  -1003,  
   -1004,  -1004,  -1005,  -1006,  -1006,  -1007,  -1007,  -1008,  
   -1008,  -1009,  -1009,  -1010,  -1010,  -1011,  -1011,  -1012,  
   -1012,  -1013,  -1013,  -1014,  -1014,  -1015,  -1015,  -1015,  
   -1016,  -1016,  -1017,  -1017,  -1017,  -1018,  -1018,  -1018,  
   -1019,  -1019,  -1019,  -1019,  -1020,  -1020,  -1020,  -1020,  
   -1021,  -1021,  -1021,  -1021,  -1022,  -1022,  -1022,  -1022,  
   -1022,  -1022,  -1023,  -1023,  -1023,  -1023,  -1023,  -1023,  
   -1023,  -1023,  -1023,  -1023,  -1023,  -1023,  -1023,  -1023,  
   -1024,  -1023,  -1023,  -1023,  -1023,  -1023,  -1023,  -1023,  
   -1023,  -1023,  -1023,  -1023,  -1023,  -1023,  -1023,  -1022,  
   -1022,  -1022,  -1022,  -1022,  -1022,  -1021,  -1021,  -1021,  
   -1021,  -1020,  -1020,  -1020,  -1020,  -1019,  -1019,  -1019,  
   -1019,  -1018,  -1018,  -1018,  -1017,  -1017,  -1017,  -1016,  
   -1016,  -1015,  -1015,  -1015,  -1014,  -1014,  -1013,  -1013,  
   -1012,  -1012,  -1011,  -1011,  -1010,  -1010,  -1009,  -1009,  
   -1008,  -1008,  -1007,  -1007,  -1006,  -1006,  -1005,  -1004,  
   -1004,  -1003,  -1003,  -1002,  -1001,  -1001,  -1000,  -999,  
   -999,  -998,  -997,  -997,  -996,  -995,  -994,  -994,  
   -993,  -992,  -991,  -990,  -990,  -989,  -988,  -987,  
   -986,  -986,  -985,  -984,  -983,  -982,  -981,  -980,  
   -979,  -978,  -978,  -977,  -976,  -975,  -974,  -973,  
   -972,  -971,  -970,  -969,  -968,  -967,  -966,  -965,  
   -964,  -963,  -962,  -960,  -959,  -958,  -957,  -956,  
   -955,  -954,  -953,  -951,  -950,  -949,  -948,  -947,  
   -946,  -944,  -943,  -942,  -941,  -939,  -938,  -937,  
   -936,  -934,  -933,  -932,  -930,  -929,  -928,  -927,  
   -925,  -924,  -922,  -921,  -920,  -918,  -917,  -916,  
   -914,  -913,  -911,  -910,  -908,  -907,  -906,  -904,  
   -903,  -901,  -900,  -898,  -897,  -895,  -894,  -892,  
   -890,  -889,  -887,  -886,  -884,  -883,  -881,  -879,  
   -878,  -876,  -875,  -873,  -871,  -870,  -868,  -866,  
   -865,  -863,  -861,  -860,  -858,  -856,  -854,  -853,  
   -851,  -849,  -847,  -846,  -844,  -842,  -840,  -839,  
   -837,  -835,  -833,  -831,  -829,  -828,  -826,  -824,  
   -822,  -820,  -818,  -816,  -814,  -813,  -811,  -809,  
   -807,  -805,  -803,  -801,  -799,  -797,  -795,  -793,  
   -791,  -789,  -787,  -785,  -783,  -781,  -779,  -777,  
   -775,  -773,  -771,  -769,  -767,  -765,  -762,  -760,  
   -758,  -756,  -754,  -752,  -750,  -748,  -745,  -743,  
   -741,  -739,  -737,  -735,  -732,  -730,  -728,  -726,  
   -724,  -721,  -719,  -717,  -715,  -712,  -710,  -708,  
   -706,  -703,  -701,  -699,  -696,  -694,  -692,  -690,  
   -687,  -685,  -683,  -680,  -678,  -675,  -673,  -671,  
   -668,  -666,  -664,  -661,  -659,  -656,  -654,  -652,  
   -649,  -647,  -644,  -642,  -639,  -637,  -634,  -632,  
   -629,  -627,  -625,  -622,  -620,  -617,  -615,  -612,  
   -609,  -607,  -604,  -602,  -599,  -597,  -594,  -592,  
   -589,  -587,  -584,  -581,  -579,  -576,  -574,  -571,  
   -568,  -566,  -563,  -561,  -558,  -555,  -553,  -550,  
   -547,  -545,  -542,  -539,  -537,  -534,  -531,  -529,  
   -526,  -523,  -521,  -518,  -515,  -512,  -510,  -507,  
   -504,  -501,  -499,  -496,  -493,  -491,  -488,  -485,  
   -482,  -479,  -477,  -474,  -471,  -468,  -466,  -463,  
   -460,  -457,  -454,  -451,  -449,  -446,  -443,  -440,  
   -437,  -434,  -432,  -429,  -426,  -423,  -420,  -417,  
   -414,  -412,  -409,  -406,  -403,  -400,  -397,  -394,  
   -391,  -388,  -386,  -383,  -380,  -377,  -374,  -371,  
   -368,  -365,  -362,  -359,  -356,  -353,  -350,  -347,  
   -344,  -342,  -339,  -336,  -333,  -330,  -327,  -324,  
   -321,  -318,  -315,  -312,  -309,  -306,  -303,  -300,  
   -297,  -294,  -291,  -288,  -285,  -282,  -279,  -276,  
   -273,  -270,  -267,  -264,  -260,  -257,  -254,  -251,  
   -248,  -245,  -242,  -239,  -236,  -233,  -230,  -227,  
   -224,  -221,  -218,  -215,  -212,  -209,  -205,  -202,  
   -199,  -196,  -193,  -190,  -187,  -184,  -181,  -178,  
   -175,  -171,  -168,  -165,  -162,  -159,  -156,  -153,  
   -150,  -147,  -144,  -140,  -137,  -134,  -131,  -128,  
   -125,  -122,  -119,  -115,  -112,  -109,  -106,  -103,  
   -100,  -97,  -94,  -90,  -87,  -84,  -81,  -78,  
   -75,  -72,  -69,  -65,  -62,  -59,  -56,  -53,  
   -50,  -47,  -43,  -40,  -37,  -34,  -31,  -28,  
   -25,  -21,  -18,  -15,  -12,  -9,  -6,  -3 
};
*/

void setup()
{
  pinMode(PWM_PIN, OUTPUT);
  
  // Set up Timer 2 to do pulse width modulation on the speaker
  // pin.
  
  // Use internal clock (datasheet p.160)
  ASSR &= ~(_BV(EXCLK) | _BV(AS2));
  
  // Set fast PWM mode  (p.157)
  TCCR2A |= _BV(WGM21) | _BV(WGM20);
  TCCR2B &= ~_BV(WGM22);
  
  // Do non-inverting PWM on pin OC2B (p.155)
  // On the Arduino this is pin 3.
  TCCR2A = (TCCR2A | _BV(COM2B1)) & ~_BV(COM2B0);
  TCCR2A &= ~(_BV(COM2A1) | _BV(COM2A0));
  
  // No prescaler (p.158)
  TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);
  
  // Set initial pulse width to the first sample.
  PWM_VALUE = 0x80;
  
  // Set up Timer 1 to send a sample every interrupt.
  
  cli();
  
  // disable Timer0 - this means that delay() is no longer available
  TIMSK0 &= (~TOIE0);
  
  // Set CTC mode (Clear Timer on Compare Match) (p.133)
  // Have to set OCR1A *after*, otherwise it gets reset to 0!
  TCCR1B = (TCCR1B & ~_BV(WGM13)) | _BV(WGM12);
  TCCR1A = TCCR1A & ~(_BV(WGM11) | _BV(WGM10));
  
  // No prescaler (p.134)
  TCCR1B = (TCCR1B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);
  
  // Set the compare register (OCR1A).
  // OCR1A is a 16-bit register, so we have to do this with
  // interrupts disabled to be safe.
  OCR1A = 976;    // 16e6 / 16384
  
  // Enable interrupt when TCNT1 == OCR1A (p.136)
  TIMSK1 |= _BV(OCIE1A);
  
  // set up the initial values for all the controls

  g_env_lengths[0] = 0;
  g_env_lengths[1] = 0;
  g_env_type=0;

  g_base_freq = 440;
  g_freq_ramp_cnt = 0;

  g_lpf_resonance = 0;
  
  g_vib_strength = 0;
  
  // set the parameters but don't start playing
  reset_sample();
  
  // play notes
  pinMode(TONE1_PIN, INPUT);
  digitalWrite(TONE1_PIN, HIGH);
  pinMode(TONE2_PIN, INPUT);
  digitalWrite(TONE2_PIN, HIGH);
  pinMode(TONE3_PIN, INPUT);
  digitalWrite(TONE3_PIN, HIGH);
  pinMode(TONE4_PIN, INPUT);
  digitalWrite(TONE4_PIN, HIGH);
  pinMode(TONE5_PIN, INPUT);
  digitalWrite(TONE5_PIN, HIGH);
  g_button_status = 0; // no buttons are pressed
  g_current_pitch_bit = 0; // no buttons are pressed

  // low-pass
  pinMode(LOWPASS_PIN, INPUT);
  digitalWrite(LOWPASS_PIN, HIGH);

  // distortion
  pinMode(FREQ_PIN, INPUT);
  digitalWrite(FREQ_PIN, HIGH);

  // distortion
  pinMode(DIST_PIN, INPUT);
  digitalWrite(DIST_PIN, HIGH);
  g_dist_status = 0; // not pressed

  // tremolo
  pinMode(TREM_PIN, INPUT);
  digitalWrite(TREM_PIN, HIGH);
  g_trem_status = 0; // not pressed

  // now enable interrupts
  sei();
}

static unsigned char get_button_status()
{
  unsigned char ans = 0;
  if (digitalRead(TONE1_PIN) == LOW) ans |= 0x01;
  if (digitalRead(TONE2_PIN) == LOW) ans |= 0x02;
  if (digitalRead(TONE3_PIN) == LOW) ans |= 0x04;
  if (digitalRead(TONE4_PIN) == LOW) ans |= 0x08;
  if (digitalRead(TONE5_PIN) == LOW) ans |= 0x10;
  return ans;
}

static unsigned short get_base_freq(unsigned char status)
{
  if (status & 0x10) return 320;
  if (status & 0x08) return 360;
  if (status & 0x04) return 400;
  if (status & 0x02) return 480;
  return 540;
}

void loop()
{
  uint8_t new_status;
  uint8_t tmp;
  uint8_t tmp_pitch_bit;

  //
  // the loop updates the sound parameters
  //
  // since we don't want all the parameters updated when playing,
  // some parameters are updated only on silence and on sustain,
  // and others are updated only on silence
  //

  // envelopes are updated only on silence and sustain
  if ((g_env_stage != 0) && (g_env_stage != 2)) {
    g_env_lengths[0] = analogRead(ATTACK_CTRL) * 16;
    g_env_lengths[1] = analogRead(RELEASE_CTRL) * 16;
    g_env_type = 
      (digitalRead(LOWPASS_PIN) == LOW) ? 1 :
      ((digitalRead(FREQ_PIN) == LOW) ? 2 : 0);
  }

  // vibrato and resonance are always welcome
  tmp = (uint8_t)(analogRead(VIBRATO_CTRL) / 4);
  g_vib_strength = (tmp > 20) ? tmp : 0;
  tmp = analogRead(RESONANCE_CTRL) / 4;
  g_lpf_resonance = (tmp > 20) ? tmp : 0;

  // so are tremolo and distortion
  g_trem_status = digitalRead(TREM_PIN)==LOW ? 1 : 0;
  g_dist_status = digitalRead(DIST_PIN)==LOW ? 1 : 0;

  //
  // pitch buttons
  //

  new_status = get_button_status();

  // is the current pitch released?
  if ((g_env_stage < 2) && ((g_current_pitch_bit & new_status) == 0)) {
    g_env_stage = 2;
  }

  // are all buttons released?
  if (new_status == 0) {
    g_current_pitch_bit = 0x00;
  }
  // or is a different pitch selected?
  else if (new_status != g_button_status) {
    tmp_pitch_bit = g_current_pitch_bit;

    // if a new button is pressed - select it
    if (new_status & (~g_button_status)) {
      tmp_pitch_bit = new_status & (~g_button_status);
    }
    // or if the current pitch is released - select another
    else if ((new_status & (~g_current_pitch_bit)) == 0) {
      tmp_pitch_bit = new_status;
    }

    // select only one pitch
    tmp_pitch_bit &= (~(tmp_pitch_bit-1));

    // if it's really a different pitch, play it
    if (tmp_pitch_bit != g_current_pitch_bit) {
      g_current_pitch_bit = tmp_pitch_bit;

      // change the pitch with interrupts disabled
      cli();
      g_base_freq = get_base_freq(g_current_pitch_bit);
      reset_sample();
      g_env_stage = 0;
      sei();
    }
  }

  g_button_status = new_status;
}

static void reset_sample()
{
  g_phase = 0;
  g_freq_ramp_cnt = 0;

  g_curr_freq = g_base_freq;
  if (g_env_type == 2) {
    g_curr_freq = g_base_freq / (g_env_lengths[0]/64 + 1);
    if (g_curr_freq < 40) g_curr_freq = 40;
  }

  // reset filters
  g_lpf_prev = 0;
  g_lpf_prev_delta = 0;
  g_lpf_freq = ((g_env_type == 1) ? 1 : 255); // zero if lpf envelope, else max
  
  // reset vibrato
  g_vib_phase = 0;
  g_vib_fix_accum = 0;
  
  // reset tremolo, distortion, and shit
  g_interrupt_cnt = 0;
  
  // reset envelope
  g_env_stage=3;
  g_env_time=0;
}

/* interrupt handler - that's where the synthesis happens */
SIGNAL(PWM_INTERRUPT)
{
  short sample;
  unsigned short fp;
  unsigned char env_vol = 0xff;
  unsigned char* p_env_item;
  unsigned short vibrated_freq;
  short vib_fix;

  // use another oscillator with a lower frequency for the vibrato
  g_vib_phase += 20;
  
  if(g_vib_strength)
  {
    // vib_fix should be between -0x80 and 0x7f
    g_vib_fix_accum += phase_to_delta[g_vib_phase/0x4000];
    vib_fix = ((g_vib_fix_accum / 0x80) * (short)g_vib_strength) / 0x10;
    
    // fix the frequency according to the vibrato
    if ((vib_fix < 0) && ((unsigned short)(-vib_fix) >= g_curr_freq)) {
      vibrated_freq = 1;
    }
    else {
      vibrated_freq = g_curr_freq + vib_fix;
    }
  }
  else {
    vibrated_freq = g_curr_freq;
  }
  
  //
  // volume/low-pass envelope
  //

  p_env_item = (g_env_type==0) ? &env_vol : &g_lpf_freq;

  // this is performed anyway - for all env types
  if (g_env_stage == 3) {
    // at stage three there's nothing to play
    *p_env_item = 0;
    env_vol = 0;
  }
  else if (g_env_type < 2) {
    // compute by stage, keeping *p_env_item between zero and 0xff
    if (g_env_stage == 0) {
      g_env_time++;
      if (g_env_time >= g_env_lengths[0]) {
        g_env_time = 0;
        g_env_stage = 1;
      }
      *p_env_item=(unsigned char)(g_env_time / (g_env_lengths[0]/0x100 + 1));
    }
    else if (g_env_stage == 2) {
      g_env_time++;
      if (g_env_time >= g_env_lengths[1]) {
        g_env_time = 0;
        g_env_stage = 3;
      }
      *p_env_item=255-(unsigned char)(g_env_time/ (g_env_lengths[1]/0x100 + 1));
    }
    else { // g_env_stage == 1
      *p_env_item = 255;
    }
  }

  g_interrupt_cnt++;
  // tremolo adjustments
  if (g_trem_status) {
    // do the shit at ~16Hz
    if (g_interrupt_cnt & 0x200) {
      env_vol /= 2;
    }
  }
  
  // distortion adjustments
  if (g_dist_status) {
    env_vol &= g_rand_shit[g_interrupt_cnt & 0xFF];
  }
  
  //
  // phase of the current wave
  //
  
  g_phase += vibrated_freq;
  if (g_phase & 0xC000)
  {
    g_phase &= 0x3FFF;
    
    //
    // wave length wraparound
    // this is a good time to make changes in the frequency
    //
    
    // fix the frequency according to the ramp
    if (g_env_type == 2) {
      if (g_env_stage == 0) {
        g_freq_ramp_cnt += (0xff - g_env_lengths[0]/64);
        g_curr_freq += 1+(g_freq_ramp_cnt/32);
        g_freq_ramp_cnt &= 0x001f;
        if (g_curr_freq > g_base_freq) {
          g_curr_freq = g_base_freq;
          g_env_stage = 1;
          g_freq_ramp_cnt = 0;
        }
      }
      else if (g_env_stage == 1) {
        g_curr_freq = g_base_freq;
      }
      else if (g_env_stage == 2) {
        g_freq_ramp_cnt += (0xff - g_env_lengths[1]/64);
        g_curr_freq += 1+(g_freq_ramp_cnt/8);
        g_freq_ramp_cnt &= 0x0007;
        if (g_curr_freq >= 4000) {
          g_env_stage = 3;
        }
      }
      else if (g_env_stage == 3) {
        env_vol = 0;
      }
    }
  }
  
  //
  // current sample computation
  //
  
  // base waveform
  fp = g_phase >> 3; // keep fp between zero and 2047
  sample = 0;
  if (g_wave_type < 2)
  {
    if (g_wave_type == 0) { // rough sawtooth
      sample = (short)1023-(short)fp;
    }
    else { // sawtooth
      sample = (fp < 1024) ? (short)1023-(short)fp*2 : (short)fp*2-3072;
    }
  }
  //else if (g_wave_type == 2) { // sine
  //  sample = (short)pgm_read_word(&mysin_table[fp]);
  //}
  else { // square
    sample = (fp & 1024) ? 1023 : -1023;
  }
  
  // sample is between -1024 and 1023
  
  //
  // low-pass filter
  //

  // start with the resonance - multiply g_lpf_prev_delta by the factor
  g_lpf_prev_delta /= 0x10;
  g_lpf_prev_delta *= g_lpf_resonance;
  g_lpf_prev_delta /= 0x10;

  //
  // now add the low-pass part
  //
  // since sample and g_lpf_prev are both between -1024 and 1023,
  // we can be sure that g_lpf_prev_delta will not be
  // incremented/decremented by more than 2047
  //
  g_lpf_prev_delta +=
    ((((short)sample-(short)g_lpf_prev) / 0x10) * (short)g_lpf_freq) / 0x10;
  if (g_lpf_prev_delta > 2047) g_lpf_prev_delta = 2047;
  else if (g_lpf_prev_delta < -2048) g_lpf_prev_delta = -2048;
 
  // accumulate it to the filter's output
  g_lpf_prev += g_lpf_prev_delta;

  if (g_lpf_prev > 1023) g_lpf_prev = 1023;
  if (g_lpf_prev < -1024) g_lpf_prev = -1024;

  // filter output
  sample = g_lpf_prev;

  // now sample is between -1024 and 1023
  // scale it between -128 and 127
  sample = sample / 8;
  
  // adjust with the volume envelope
  sample *= env_vol;
  sample = sample / 256;
  
  PWM_VALUE=(unsigned char)(sample + 128);
}

