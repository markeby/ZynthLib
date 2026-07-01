//#######################################################################
// Module:     Envelope.h
// Descrption: Envelope processor
// Creator:    markeby
// Date:       6/25/2024
//#######################################################################
#pragma once
#include <deque>

//###########################################
// Envelope selection bytes
//###########################################
enum class ENV_CTRL_E : int
    {
    FIXED = 0,
    ENVELOPE = 1,
    MODULATE = 2,
    MODWHEEL = 3
    };

//###########################################
// Envelope states
//###########################################
enum class ESTATE
    {
    IDLE = 0,
    START,
    ATTACK,
    DECAY,
    SUSTAIN,
    RELEASE
    };

//###########################################
// Damper modes
//###########################################
enum class DAMPER_MODE : byte
    {
    OFF = 0,
    NORMAL,
    INVERT,
    MAX
    };

//###########################################
// Default output scaling macro
//###########################################
#define FromUnityDA(vf) (vf * )

//#######################################################################
class ENVELOPE_C
    {
private:
    // State change
    int         _Active;
    bool        _TriggerEnd;

    // Runtime state
    byte&       _UseCount;          // increment started and decriment as idle
    ESTATE      _State;             // Current state of this mixer channel
    float       _LevelDelta;        // delta between base and top level setting for use with modulation generation

    bool        _Muted;             // Do not respond to Start directive
    bool        _Updated;           // Flag indicating update output
    bool        _PeakLevel;         // Flag indicating sustain and peak are the same
    float*      _TremoloOuput;

    // User supplied inputs
    bool        _DualUse;           // Dual usage flag  (false = VCA,  true = VCF,other)
    bool        _UseTremolo;        // Flag to enable sofware LFO
    DAMPER_MODE _DamperMode;        // Mode to utilize string damper
    float       _Top;               // Fraction of one (percent)
    float       _Bottom;            // Fraction of one (percent)
    float       _SetSustain;        // The settin of sustain level up to one
    float       _AttackTime;        // Attack time in uSec
    float       _DecayTime;         // Decay time to sustatin level in uSec
    float       _ReleaseTime;       // How long to end back at base level in uSec
    float       _Expression;        // Final volume multiplier
    bool        _Damper;            // state of damper pedal
    float       _TremoloMaxLevel;   // Maximum level tremolo can effect (0 to 1)
    float       _TremoloWheelLevel; // Mod wheel position for tremelo level
    float       _TremoloWheel;      // Use mod wheel
    bool        _TremoloInvert;     // invert tremolo wave

    // runtime calculations
    float       _Delta;             // Distance for the current state
    float       _Sustain;           // The usable Sustain level up to one
    bool        _NoDecay;           // Decay time set so low that there is no decay.  Sustain serves no purpose then.
    float       _Timer;             // Timer loaded with state time
    float       _TargetTime;        // Timer is incrimented until this time is exceeded
    float       _Current;           // Current level zero to one

    // Fixed parameters at initialization
    String      _Name;
    byte        _Index;
    uint16_t    _DevicePortIO;
    float       _DeviceRange;

public:
                ENVELOPE_C          (uint8_t index, String name, uint16_t device, uint16_t device_rang, uint8_t& usecount);
    void        Clear               ();
    void        Mute                (bool state);
    void        Process             (float deltaTime);
    void        SetOverride         (uint16_t data);
    void        Update              ();
    void        Start               ();
    void        End                 ();
    void        SetTime             (ESTATE state, float time);
    void        SetLevel            (ESTATE state, float percent);
    void        SetDualUse          (bool sel);
    void        SetModulationLevel  (float lvl);
    uint16_t    GetPortIO           ()                  { return (_DevicePortIO); }  // Return D/A channel number
    void        SetDamperMode       (DAMPER_MODE mode)  { _DamperMode = mode; }
    void        Expression          (float level)       { _Expression = level; }
    void        Damper              (bool state)        { _Damper = state; }
    void        TremoloMax          (float lvl)         { _TremoloMaxLevel = lvl; }
    void        TremoloPointer      (float* pf)         { _TremoloOuput = pf; _UseTremolo = ( pf != nullptr ) ? true : false; }
    void        TremoloWheel        (bool state)        { _TremoloWheel = state;  _TremoloWheelLevel = 0.0f; }
    void        TremoloWheelLevel   (float val)         { _TremoloWheelLevel = val; }
    void        TremoloInvert       (bool state)        { _TremoloInvert = state; }

    int IsActive (void)
        { return (_Active); }
    };  // end ENVELOPE_C

//#######################################################################
class ENV_GENERATOR_C
    {
private:
    std::deque<ENVELOPE_C>  _Envelopes;

public:
                ENV_GENERATOR_C (void);
    ENVELOPE_C* NewADSR         (uint8_t index, String name, uint16_t device, uint16_t device_range, uint8_t& usecount);
    void        Debug           (bool state);
    void        Loop            (void);
    };

//#######################################################################
extern ENV_GENERATOR_C  EnvelopeGenerator;  // Envelope generator spawn tool


