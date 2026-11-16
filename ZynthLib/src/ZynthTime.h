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
    uint8_t     _HeartBeatGPIO;             // User assigned GPIO for heartbeat LED
    bool        _HeartBeatOn;               // on state can be high or low depending on application of LED

public:
        ZYNTH_TIME_C ();

    //#######################################################################
    /*!
    @brief  Setup heartbeat LED configuration
    @param  gpio_pin    Processor GPIO pin assigned to heartbeat LED
    @param  state       Set true for ground referenced LED and false if LED reference to 3.3 ot 5 volts
    */
    void Begin  (uint8_t gpio_pin, bool state);

    //#######################################################################
    /*!
    @brief  Called at start of standeard loop function to monitor code performance
    */
    void Start ();                   // Time process for loop start

    //#######################################################################
    /*!
    @brief  Called at end of standeard loop function to monitor code performance
    */
    void End   ();                   // Time process for loop ending

    //#######################################################################
    /*!
    @brief      Return the clock counting from beginning of execution
    @return     Unsigned 64 bit value in miceoseconds.
    */
    uint64_t TotalRunningTime ()
        {
        return (_RunTime);
        }

    //#######################################################################
    /*!
    @brief      Obtain the time interval since last call
    @return     Floating point value in milliseconds.
    */
    float DeltaTimeMS ()
        {
        return (_DeltaTimeMilli);
        }


    //#######################################################################
    /*!
    @brief      Obtain the average time interval between calls
    @return     Floating point value in milliseconds.
    */
    float DeltaTimeAvg ()
        {
        return (_DeltaTimeMilliAverage);
        }

    //#######################################################################
    /*!
    @brief      Obtain the average time interval between calls
    @return     Floating point value in milliseconds
    */
    float DeltaTimeExecAvg ()
        {
        return (_DeltaExecutionAverage);
        }

    //#######################################################################
    /*!
    @brief      Obtain the longest time interval between calls
    @return     Floating point value in milliseconds.
    */
    float LongestTime ()
        {
        float zf               = _LongestTimeMilli;
        _LongestTimeMilli      = 0.0f;
        _DeltaTimeMilliAverage = 0.0f;
        _DeltaExecutionAverage = 0.0f;
        return (zf);
        }

    //#######################################################################
    /*!
    @brief  Set timer LED to fail flash rate as 2 or more flashes per second
    */
    void SetFailMode (bool state)
        {
        _FailAlert = state;
        }

    //#######################################################################
    /*!
    @brief  Kill the longest time interval and do not calculate for supplied loop iterations
    @param  count      Set the number of loop iterations to skip before starting longest detection
    */
    void SkipLongest (int count)
        {
        _LongestTimeMilli = 0.0f;
        _SkipLongest      = count;
        }
    };

//#####################################
extern ZYNTH_TIME_C ZyTime;

