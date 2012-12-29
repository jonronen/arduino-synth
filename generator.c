#include <avr/io.h>
#include <avr/interrupt.h>

#define PI 3.14159265f

#define PWM_PIN       3
#define PWM_VALUE     OCR2B
#define PWM_INTERRUPT TIMER2_OVF_vect

#define VIBRATO_SPEED_CTRL 0
#define VIBRATO_DEPTH_CTRL 1
#define LOWPASS_FREQ_CTRL 2
#define LOWPASS_RAMP_CTRL 3
#define LOWPASS_RESONANCE_CTRL 4

int wave_type;

float p_base_freq;
float p_freq_ramp;
float p_duty;
float p_duty_ramp;

float p_vib_strength;
float p_vib_speed;
float p_vib_delay;

float p_env_attack;
float p_env_decay;

int filter_on;
float p_lpf_resonance;
float p_lpf_freq;
float p_lpf_ramp;
float p_hpf_freq;
float p_hpf_ramp;

int playing_sample;
int phase;
double fperiod;
double fmaxperiod;
double fslide;
int period;
int env_stage;
int env_time;
int env_length[3];
float env_vol;
float fltp;
float fltdp;
float fltw;
float fltw_d;
float fltdmp;
float fltphp;
float flthp;
float flthp_d;
float vib_phase;
float vib_speed;
float vib_amp;


void setup()
{
    // set up the PWM pin
    pinMode(PWM_PIN,OUTPUT);
    // Set up PWM to 31.25kHz, phase accurate
    TCCR2A = _BV(COM2B1) | _BV(WGM20);
    TCCR2B = _BV(CS20);
    TIMSK2 = _BV(TOIE2);

    // for debugging
    Serial.begin(115200);

    // set up the initial values for all the controls
    wave_type = 1; // sawtooth

    p_env_attack = 0.05f;
    p_env_decay = 0.7f;

    p_base_freq = 0.8f;
    p_freq_ramp = 0.0f;

    p_vib_strength = 0.0f;
    p_vib_speed = 0.0f;

    p_lpf_freq = 1.0f;
    p_lpf_ramp = 0.0f;
    p_lpf_resonance = 0.0f;
    p_hpf_freq = 0.0f;
    p_hpf_ramp = 0.0f;

    // start playing
    reset_sample(0);
}

void loop() {
    // The loop is pretty simple - it just updates the parameters
    //p_lpf_freq = (float)analogRead(LOWPASS_FREQ_CTRL) / 1024.0f;
    //p_lpf_ramp = (float)analogRead(LOWPASS_RAMP_CTRL) / 1024.0f;
    //p_lpf_resonance = (float)analogRead(LOWPASS_RESONANCE_CTRL) / 1024.0f;
    //p_vib_speed = (float)analogRead(VIBRATO_SPEED_CTRL) / 1024.0f;
    //p_vib_strength = (float)analogRead(VIBRATO_DEPTH_CTRL) / 1024.0f;

/*
    Slider(xpos, (ypos++)*18, p_env_attack, 0, "ATTACK TIME");
    Slider(xpos, (ypos++)*18, p_env_decay, 0, "DECAY TIME");

    Slider(xpos, (ypos++)*18, p_base_freq, 0, "START FREQUENCY");
    Slider(xpos, (ypos++)*18, p_freq_ramp, 1, "SLIDE");

    Slider(xpos, (ypos++)*18, p_vib_strength, 0, "VIBRATO DEPTH");
    Slider(xpos, (ypos++)*18, p_vib_speed, 0, "VIBRATO SPEED");

    Slider(xpos, (ypos++)*18, p_lpf_freq, 0, "LP FILTER CUTOFF");
    Slider(xpos, (ypos++)*18, p_lpf_ramp, 1, "LP FILTER CUTOFF SWEEP");
    Slider(xpos, (ypos++)*18, p_lpf_resonance, 0, "LP FILTER RESONANCE");
    Slider(xpos, (ypos++)*18, p_hpf_freq, 0, "HP FILTER CUTOFF");
    Slider(xpos, (ypos++)*18, p_hpf_ramp, 1, "HP FILTER CUTOFF SWEEP");
*/
}

void reset_sample(int restart)
{
    int i;

    playing_sample = 1;

    if(!restart)
        phase=0;
    fperiod=100.0/(p_base_freq*p_base_freq+0.001);
    period=(int)fperiod;
    fmaxperiod=2205.0;
    fslide=1.0-pow((double)p_freq_ramp, 3.0)*0.01;
    if(!restart)
    {
        // reset filter
        fltp=0.0f;
        fltdp=0.0f;
        fltw=pow(p_lpf_freq, 3.0f)*0.1f;
        fltw_d=1.0f+p_lpf_ramp*0.0001f;
        fltdmp=5.0f/(1.0f+pow(p_lpf_resonance, 2.0f)*20.0f)*(0.01f+fltw);
        if(fltdmp>0.8f) fltdmp=0.8f;
        fltphp=0.0f;
        flthp=pow(p_hpf_freq, 2.0f)*0.1f;
        flthp_d=1.0+p_hpf_ramp*0.0003f;
        // reset vibrato
        vib_phase=0.0f;
        vib_speed=pow(p_vib_speed, 2.0f)*0.01f;
        vib_amp=p_vib_strength*0.5f;
        // reset envelope
        env_vol=0.0f;
        env_stage=0;
        env_time=0;
        env_length[0]=(int)(p_env_attack*p_env_attack*100000.0f);
        env_length[1]=0;
        env_length[2]=(int)(p_env_decay*p_env_decay*100000.0f);
    }
}

/* interrupt handler - that's where the synthesis happens */
SIGNAL(PWM_INTERRUPT)
{
    int i, si;
    float rfperiod, ssample, fp, pp;

    if(!playing_sample)
        return;

    // frequency envelopes/arpeggios
    fperiod*=fslide;
    if(fperiod>fmaxperiod)
    {
        fperiod=fmaxperiod;
        playing_sample = 0;
    }
    rfperiod=fperiod;
    if(vib_amp>0.0f)
    {
        vib_phase+=vib_speed;
        rfperiod=fperiod*(1.0+sin(vib_phase)*vib_amp);
    }
    period=(int)rfperiod;
    if(period<8) period=8;
    
    //
    // volume envelope
    //

    // compute by stage, keeping env_vol between zero and one
    env_time++;
    if(env_time>env_length[env_stage])
    {
        env_time=0;
        env_stage++;
        if(env_stage==3)
            reset_sample(1);
    }
    if(env_stage==0)
        env_vol=(float)env_time/env_length[0];
    else if(env_stage==1)
        env_vol=1.0f;
    else
        env_vol=1.0f-(float)env_time/env_length[2];

    // high-pass filter adjustments
    if(flthp_d!=0.0f)
    {
        flthp*=flthp_d;
        if(flthp<0.00001f) flthp=0.00001f;
        if(flthp>0.1f) flthp=0.1f;
    }
    
    //
    // current sample computation
    //

    ssample=0.0f;
    for(si=0;si<8;si++) // 8x supersampling
    {
        float sample=0.0f;
        phase++;
        if(phase>=period)
        {
            phase%=period;
        }
        // base waveform
        fp=(float)phase/period;
        switch(wave_type)
        {
        case 1: // sawtooth
            sample=1.0f-fp*2;
            break;
        case 2: // sine
            sample=(float)sin(fp*2*PI);
            break;
        }
        // lp filter
        pp=fltp;
        fltw*=fltw_d;
        if(fltw<0.0f) fltw=0.0f;
        if(fltw>0.1f) fltw=0.1f;
        if(p_lpf_freq!=1.0f)
        {
            fltdp+=(sample-fltp)*fltw;
            fltdp-=fltdp*fltdmp;
        }
        else
        {
            fltp=sample;
            fltdp=0.0f;
        }
        fltp+=fltdp;
        // hp filter
        fltphp+=fltp-pp;
        fltphp-=fltphp*flthp;
        sample=fltphp;
        // final accumulation and envelope application
        ssample+=sample*env_vol;
    }
    // now sample is between -8 and 8
    // scale it between -128 and 127
    ssample=ssample*16;

    if(ssample>127.0f) ssample=127.0f;
    if(ssample<-128.0f) ssample=-128.0f;
    /* check if this is the right register */
    PWM_VALUE=(int)ssample + 128;
}

