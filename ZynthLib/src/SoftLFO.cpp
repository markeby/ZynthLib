//#######################################################################
//host libraries
#include <Arduino.h>

//ZynthLib
#include <SoftLFO.h>
#include <Debug.h>

// multiplier to get 127 to 4095 as a 12 bit D to A equivalent
#define MIDI_MULTIPLIER     32.245
#define ANALOG_MAX          4095

//#######################################################################
//#######################################################################
    SOFT_LFO_C::SOFT_LFO_C ()
    {
    _FreqCoarse = 0;
    _FreqFine   = 1;
    ProcessFreq ();
    _Current = 0.0;
    _Midi = 0;
    }

//#######################################################################
void SOFT_LFO_C::SetFreqFine (short value)
    {
    if ( value == 0 )
        value = 1;
    _FreqFine = value;
    ProcessFreq ();
    }
//#######################################################################
void SOFT_LFO_C::ProcessFreq ()
    {
    _Freq = (_FreqCoarse * MIDI_MULTIPLIER) + _FreqFine;
    if ( _Freq > ANALOG_MAX )
        _Freq = ANALOG_MAX;
    OutputFrequency ();
    }

//#######################################################################
void SOFT_LFO_C::OutputFrequency ()
    {
    _Frequency = _Freq * 0.014648;
    _WaveLength = 1000 / _Frequency;
    }

//#######################################################################
// Generator for sin  and triangle waves
//  - Output is -1 to +1
//#######################################################################
void SOFT_LFO_C::Loop (float delta_milli_sec)
    {
    // Calculate current position of wavelength and remove overflow
    _Current += delta_milli_sec;
    if ( _Current > _WaveLength )
        _Current -= _WaveLength;

    float zr = _Current / _WaveLength;  //Determine percentage of wavelength achieved and convert to radians
    float zt = zr * 2;                  //Double that value to use 1.0 as direction change downward for triangle

    //Calculate position in triangle wave
    if ( zt > 1.0 )                 //going down
        _Triangle = (1 - (zt - 1) - 0.5) * 2;
    else                            //going up
        _Triangle = (zt - 0.5) * 2;

    _Sine = sin (zr  * 6.28);       //Calculate position in sine wave

#if 0
    uint16_t zs = (uint16_t)((_Sine + 1.0) * 2047.0);
    uint16_t zz = (uint16_t)((_Triangle + 1.0) * 2047.0);
    I2cDevices.D2Analog (181, zz);     // for calibration testing
    I2cDevices.D2Analog (180, zs);     // for calibration testing
    I2cDevices.UpdateAnalog  ();
#endif
    }

//#######################################################################
SOFT_LFO_C   SoftLFO;

