//#######################################################################
// Module:     SoftLFO.h
// Descrption: Sine wave processor
// Creator:    markeby
// Date:       5/22/2026
//#######################################################################
#pragma once
#include <deque>

//#######################################################################
//#######################################################################
class SOFT_LFO_C
    {
private:
    byte    _Midi;
    float   _Frequency_hz;
    float   _Current_ms;
    float   _WaveLength_ms;
    float*  _pOutput;

public:
          SOFT_LFO_C (byte midi, float* fp);
    void  Process       ();
    void  SetFrequency  (float val);
    byte  Midi          ()         { return (_Midi); }
    };

//#######################################################################
class LFO_STACK_C
    {
private:
    std::deque<SOFT_LFO_C>  _Lfo;
    float*                  _pOutputs;          // pointer to hold array of the number of voices to effect (midi channels)

public:
                LFO_STACK_C     (int voice_count);
    void        Process         ();
    void        Create          (byte midi);
    void        Remove          (byte midi);
    SOFT_LFO_C* GetLFO          (byte midi);
    void        SetFrequency    (byte midi,  float val);
    float*      Output          (byte midi)                 { return (&(_pOutputs[midi - 1])); }
    };

