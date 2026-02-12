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
    _DeltaTimeMilliAvg = 0.0;
    _LongestTimeMilli  = 0.0;
    _FailAlert         = false;
    }

//#######################################################################
inline void ZYNTH_TIME_C::TimeDelta (void)
    {
    static uint64_t strt = 0;       // Starting time for next frame delta calculation

    _RunTime = micros ();
    _DeltaTimeMicro = (int)(_RunTime - strt);
    _DeltaTimeMilli = MICRO_TO_MILLI (_DeltaTimeMicro);
    if ( _DeltaTimeMilliAvg == 0 )
        _DeltaTimeMilliAvg = _DeltaTimeMilli;
    else
        _DeltaTimeMilliAvg = (_DeltaTimeMilliAvg + _DeltaTimeMilli) / 2;
    strt = _RunTime;
    if ( _DeltaTimeMilli > 210 )     // throw out long serial debug outputs
        return;
    if ( _DeltaTimeMilli > _LongestTimeMilli )
        _LongestTimeMilli = _DeltaTimeMilli;
    }

//#######################################################################
inline bool ZYNTH_TIME_C::TickTime (void)
    {
    static uint64_t loop_cnt_100hz = 0;
    static uint64_t icount = 0;

    loop_cnt_100hz += _DeltaTimeMicro;
    icount++;

    if ( loop_cnt_100hz >= MILLI_TO_MICRO (10)  )
        {
        loop_cnt_100hz = 0;
        icount = 0;
        return (true);
        }
    return (false);
    }

//#######################################################################
inline void ZYNTH_TIME_C::TickState (void)
    {
    static uint32_t counter0 = 1;

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

//#######################################################################
//#######################################################################
void ZYNTH_TIME_C::Loop (void)
    {
    TimeDelta ();
    if ( TickTime () )
        TickState ();
    }

//#######################################################################
ZYNTH_TIME_C ZyTime;

