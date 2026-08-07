//#######################################################################
// Module:     ZynthTime.h
// Descrption: Timer management
// Creator:    markeby
// Date:       2/11/2026
//#######################################################################
#pragma once

//#####################################
// Usefull multipliers
//#####################################
#define MICRO_TO_MILLI(x) ((x) * 0.001)
#define MILLI_TO_MICRO(x) ((x) * 1000)

//#####################################
//   Alarms and alerts ESP32-WROOM
//#####################################
#define HEARTBEAT_PIN       2
#define BEEP_PIN            15

//#####################################
// TIme class
//#####################################
class ZYNTH_TIME_C
    {
private:
    uint64_t    _RunTime;                   // total run time
    uint64_t    _EndTime;                   // Time stanp and end of execution loop
    float       _DeltaTimeMicro;            // µSec interval
    float       _DeltaTimeMilli;            // mSec interval
    float       _DeltaTimeMilliAverage;     // Average run time in mSec
    float       _DeltaExecutionAverage;     // Exec time of Zynth based code average over time
    float       _LongestTimeMilli;          // longest running loop in mSec
    bool        _FailAlert;                 // true to alert failure mode
    int         _SkipLongest;

public:
        ZYNTH_TIME_C ();

    void Start ();                   // Time process for loop start
    void End   ();                   // Time process for loop ending

    // return the continues counting clock that starts at zero
    uint64_t TotalRunningTime ()
        {
        return (_RunTime);
        }

    // return the time interval since last call in floating point mSec
    float DeltaTimeMS ()
        {
        return (_DeltaTimeMilli);
        }

    // return the average time interval between callx in floating point mSec
    float DeltaTimeAvg ()
        {
        return (_DeltaTimeMilliAverage);
        }

    // return the average time interval between callx in floating point mSec
    float DeltaTimeExecAvg ()
        {
        return (_DeltaExecutionAverage);
        }

     // return the longest time interval between calls in floating point mSec and reset longest and all averages
    float LongestTime ()
        {
        float zf = _LongestTimeMilli;
        _LongestTimeMilli = 0.0f;
        _DeltaTimeMilliAverage = 0.0f;
        _DeltaExecutionAverage = 0.0f;

        return (zf);
        }

    void SetFailMode (bool state)
        {
        _FailAlert = state;
        }

    void SkipLongest (int count)
        {
        _SkipLongest = count;
        }

    };

//#####################################
extern ZYNTH_TIME_C ZyTime;

