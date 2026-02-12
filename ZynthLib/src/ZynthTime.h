//#######################################################################
// Module:     ZynthTime.h
// Descrption: Timer management
// Creator:    markeby
// Date:       2/11/2026
//#######################################################################

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
    float       _DeltaTimeMicro;            // µSec interval
    float       _DeltaTimeMilli;            // mSec interval
    float       _DeltaTimeMilliAvg;         // Average run time in mSec
    float       _LongestTimeMilli;          // longest running loop in mSec
    bool        _FailAlert;                 // true to alert failure mode

    void TimeDelta (void);
    bool TickTime  (void);
    void TickState (void);

public:
        ZYNTH_TIME_C (void);

    void Loop (void);                   // looping function for time information

    uint64_t TotalRunningTime (void)    // return the continues counting clock that starts at zero
        {
        return (_RunTime);
        }

    float DeltaTimeMS (void)            // return the time interval since last call in floating point mSec
        {
        return (_DeltaTimeMilli);
        }

    float DeltaTimeAvg (void)           // return the average time interval between callx in floating point mSec
        {
        return (_DeltaTimeMilliAvg);
        }

    float LongestTime (void)            // return the longest time interval between calls in floating point mSec
        {
        float zf = _LongestTimeMilli;
        _LongestTimeMilli = 0.0;
        return (zf);
        }

    void SetFailMode (bool state)
        {
        _FailAlert = state;
        }
    };

//#####################################
extern ZYNTH_TIME_C ZyTime;

