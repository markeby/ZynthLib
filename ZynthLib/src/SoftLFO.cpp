//#######################################################################
//host libraries
#include <Arduino.h>
#include <deque>
#include <algorithm>

//ZynthLib
#include <ZynthTime.h>
#include <Debug.h>

#include "SoftLFO.h"

// multiplier to get 127 to 4095 as a 12 bit D to A equivalent
#define MIDI_MULTIPLIER     32.245
#define ANALOG_MAX          4095

//#######################################################################
//#######################################################################
    LFO_STACK_C::LFO_STACK_C (int voice_count)
    {
    _pOutputs = new float[voice_count];
    std::fill (_pOutputs, _pOutputs + voice_count, 0.0f);
    }

//#######################################################################
void LFO_STACK_C::Create (byte midi)
    {
    if ( midi > 0 )
        {
        SOFT_LFO_C* p = GetLFO (midi);
        if ( p == nullptr )
            {
            SOFT_LFO_C lfo (midi, &(_pOutputs[midi - 1]));
            _Lfo.push_back (lfo);
            }
        }
    }

//#######################################################################
void LFO_STACK_C::Remove (byte midi)
    {
    auto it = std::find_if (_Lfo.begin (), _Lfo.end (), [midi] (SOFT_LFO_C sl) {return (sl.Midi () == midi);});

    if ( it != _Lfo.end () )
        _Lfo.erase (it);
    }

//#######################################################################
void LFO_STACK_C::SetFrequency (byte midi,  float val)
    {
    SOFT_LFO_C* p = GetLFO (midi);
    if ( p == nullptr )
        return;

    p->SetFrequency (val);
    }

//#######################################################################
SOFT_LFO_C* LFO_STACK_C::GetLFO (byte midi)
    {
    SOFT_LFO_C* p = nullptr;

    auto it = std::find_if (_Lfo.begin (), _Lfo.end (), [midi] (SOFT_LFO_C sl) {return (sl.Midi () == midi);});

    if ( it != _Lfo.end () )
        p = &(*it);

    return (p);
    }

//#######################################################################
void LFO_STACK_C::Process ()
    {
    for ( std::deque<SOFT_LFO_C>::iterator it = _Lfo.begin();  it != _Lfo.end();  ++it )
        it->Process ();
    }

//#######################################################################
//#######################################################################
    SOFT_LFO_C::SOFT_LFO_C (byte midi, float* pf) : _Midi (midi), _pOutput (pf)
    {
    SetFrequency (0.1f);
    }

//#######################################################################
void SOFT_LFO_C::SetFrequency (float val)
    {
    if ( val == 0.0f )
        return;
    _Frequency_hz  = val;
    _WaveLength_ms = 1000.0 / val;      // save wave lenght as milli-seconds
    _Current_ms    = 0.0f;
    }

//#######################################################################
// Generator for sin wave
//  - Output is -1 to +1
//#######################################################################
void SOFT_LFO_C::Process ()
    {
    // Calculate current position of wavelength and remove overflow
    _Current_ms += ZyTime.DeltaTimeMS ();
    if ( _Current_ms > _WaveLength_ms )
        _Current_ms -= _WaveLength_ms;

    float zr = _Current_ms / _WaveLength_ms;  // Determine percentage of wavelength achieved
    *_pOutput = sin (zr  * 6.28);             // convert to radians and calculate position in sine wave
    }

