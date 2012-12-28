#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define rnd(n) (rand()%(n+1))

#define PI 3.14159265f

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

/* TODO: change this to an interrupt handler */
void synth_sample()
{
    int i, si;

    if(!playing_sample)
        return;

    // frequency envelopes/arpeggios
    fperiod*=fslide;
    if(fperiod>fmaxperiod)
    {
        fperiod=fmaxperiod;
        playing_sample = 0;
    }
    float rfperiod=fperiod;
    if(vib_amp>0.0f)
    {
        vib_phase+=vib_speed;
        rfperiod=fperiod*(1.0+sin(vib_phase)*vib_amp);
    }
    period=(int)rfperiod;
    if(period<8) period=8;
    // volume envelope
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
    if(env_stage==1)
        env_vol=1.0f;
    if(env_stage==2)
        env_vol=1.0f-(float)env_time/env_length[2];

    if(flthp_d!=0.0f)
    {
        flthp*=flthp_d;
        if(flthp<0.00001f) flthp=0.00001f;
        if(flthp>0.1f) flthp=0.1f;
    }

    float ssample=0.0f;
    for(si=0;si<8;si++) // 8x supersampling
    {
        float sample=0.0f;
        phase++;
        if(phase>=period)
        {
            phase%=period;
        }
        // base waveform
        float fp=(float)phase/period;
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
        float pp=fltp;
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
    ssample=ssample/8;

    /* TODO: scale this properly and fix the boundaries */
    if(ssample>1.0f) ssample=1.0f;
    if(ssample<-1.0f) ssample=-1.0f;
    /* TODO: check if this is the right register */
    /* OCR2B=(int)ssample;*/
}

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

