//#######################################################################
// Module:     ZynthTime.cpp
// Descrption: Timer management
// Creator:    markeby
// Date:       2/11/2026
//#######################################################################
#include <Arduino.h>
#include <ZynthTime.h>

//#######################################################################
    ZYNTH_TIME_C::ZYNTH_TIME_C ()
    {
    pinMode      (HEARTBEAT_PIN, OUTPUT);
    digitalWrite (HEARTBEAT_PIN, LOW);      // LED off
    _RunTime           = 0;
    _DeltaTimeMicro    = 0.0;
    _DeltaTimeMilli    = 0.0;
    _DeltaTimeMilliAverage = 0.0;
    _LongestTimeMilli  = 0.0;
    _SkipLongest       = 1000;
    _FailAlert         = false;
    }

//#######################################################################
//#######################################################################
void ZYNTH_TIME_C::Start (void)
    {
    static uint64_t strt           = 0;       // Starting time for next frame delta calculation
    static uint64_t loop_cnt_100hz = 0;
    static uint64_t icount         = 0;
    static uint32_t counter0       = 1;

    _RunTime = micros ();
    _DeltaTimeMicro = (int)(_RunTime - strt);
    _DeltaTimeMilli = MICRO_TO_MILLI (_DeltaTimeMicro);
    if ( _DeltaTimeMilliAverage == 0 )
        _DeltaTimeMilliAverage = _DeltaTimeMilli;
    else
        _DeltaTimeMilliAverage = (_DeltaTimeMilliAverage + _DeltaTimeMilli) / 2;
    strt = _RunTime;
    if ( _DeltaTimeMilli < 100 )     // throw out long serial debug outputs
        {
        if ( _SkipLongest )        // interval that dumps a lot of content to
            {
            --_SkipLongest;
            _LongestTimeMilli = 0.0;
            }
        else if ( _DeltaTimeMilli > _LongestTimeMilli )
            _LongestTimeMilli = _DeltaTimeMilli;
        }


    loop_cnt_100hz += _DeltaTimeMicro;
    icount++;

    if ( loop_cnt_100hz >= MILLI_TO_MICRO (10)  )
        {
        loop_cnt_100hz = 0;
        icount = 0;

        if ( --counter0 == 0 )
            {
            digitalWrite (HEARTBEAT_PIN, HIGH);     // LED on
            counter0 = 100;
            }
        if ( _FailAlert )
            {
            if ( counter0 % 25 )
                digitalWrite (HEARTBEAT_PIN, LOW);  // LED off
            else
                digitalWrite (HEARTBEAT_PIN, HIGH); // LED on
            }
        if ( counter0 == 98 )
            digitalWrite (HEARTBEAT_PIN, LOW);      // LED off
        }
    }

//#######################################################################
void ZYNTH_TIME_C::End (void)
    {
    _EndTime = micros ();
    float   zf = _EndTime - _RunTime;

    _DeltaExecutionAverage = (_DeltaExecutionAverage + zf) * 0.0005f;
    }

//#######################################################################
ZYNTH_TIME_C ZyTime;

