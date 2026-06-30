//#######################################################################
// Module:     Envelope.cpp
// Descrption: Envelope processor
// Creator:    markeby
// Date:       6/25/2024
//#######################################################################
//host libraries
#include <Arduino.h>

//ZynthLib
#include <ZynthTime.h>
#include <Debug.h>
#include <I2Cdevices.h>
#include <Envelope.h>

using namespace std;


#ifdef DEBUG_SYNTH
static const char* Label = "ENV";
#define DBG(args...)  {if (debugENV){DebugMsgF(Label,_Index,_Name,stateLabel[(int)_State],args); } }
#else
#define DBG(args...)
#endif

static char* stateLabel[] = { "IDLE", "START", "ATTACK", "DECAY", "SUSTAIN", "RELEASE" };
#define TIME_THRESHOLD  0.0
static bool debugENV = false;

//#######################################################################
// Envelope creation class
//#######################################################################
ENV_GENERATOR_C::ENV_GENERATOR_C ()
    {
    }

//#######################################################################
// call to enable debug dumps if DEBUG_SYNTH was on at compile time
//#######################################################################
void ENV_GENERATOR_C::Debug (bool state)
    {
    debugENV = state;
    }

//#######################################################################
ENVELOPE_C* ENV_GENERATOR_C::NewADSR (uint8_t index, String name, uint16_t device, uint16_t device_range, uint8_t& usecount)
    {
    ENVELOPE_C adsl (index, name, device, device_range, usecount);
    _Envelopes.push_back (adsl);
    return (&(_Envelopes.back ()));
    }

//#######################################################################
void ENV_GENERATOR_C::Loop ()
    {
    for ( deque<ENVELOPE_C>::iterator it = _Envelopes.begin();  it != _Envelopes.end();  ++it )
        {
        if ( it->IsActive () )      // if we ain't active then we don't need to run this.
            {
            it->Process (ZyTime.DeltaTimeMS ());
            it->Update ();
            }
        }
    I2cDevices.Update ();           // process all changes on I2C devices
    }

//#######################################################################
//#######################################################################
ENVELOPE_C::ENVELOPE_C (uint8_t index, String name, uint16_t device, uint16_t device_range, uint8_t& usecount) : _UseCount(usecount), _TremoloOuput (nullptr)
    {
    _Name               = name;
    _DevicePortIO       = device;
    _Index              = index + 1;
    _Muted              = false;
    _DualUse            = false;
    _Current            = 0;
    _Top                = 0;
    _Bottom             = 0;
    _SetSustain         = 0;
    _AttackTime         = 0;
    _DecayTime          = 0;
    _ReleaseTime        = 0;
    _Active             = 0;
    _UseTremolo         = false;
    _DamperMode         = DAMPER::OFF;
    _Expression         = 1.0;
    _DeviceRange        = device_range;
    _TremoloWheel       = false;
    _TremoloWheelLevel  = 0.0f;
    _LevelDelta         = 0;
    Clear ();
    }

//#######################################################################
void ENVELOPE_C::Clear ()
    {
    if ( _Active && _UseCount )
        _UseCount--;
    _Active        = false;
    _TriggerEnd    = false;
    _State         = ESTATE::IDLE;
    _Current       = _Bottom;
    _Updated       = true;
    DBG("clearing");
    Update ();
    }

//#######################################################################
void ENVELOPE_C::Mute (bool state)
    {
    _Muted = state;
    DBG ("Mute set to %d", state);
    if ( state )
        Clear ();
    }


//#######################################################################
void ENVELOPE_C::SetTime (ESTATE state, float time)
    {
    switch (state )
        {
        case ESTATE::ATTACK:
            _AttackTime = time;
            break;
        case ESTATE::DECAY:
            _DecayTime = time;
            break;
        case ESTATE::RELEASE:
            _ReleaseTime = time;
            break;
        }
    DBG ("%s - Time setting > %f mSec", stateLabel[(int)state], time );
    }

//#######################################################################
void ENVELOPE_C::SetLevel (ESTATE state, float percent)
    {
    String str;

    switch ( state )
        {
        case ESTATE::START:
            str = "BASE";
            _Bottom = percent;
            if ( _DualUse )
                {
                _LevelDelta = _Top - _Bottom;
                if ( _State == ESTATE::IDLE )
                    {
                    _Current = _Bottom;
                    _Updated = true;
                    Update ();
                    }
                }
            break;
        case ESTATE::ATTACK:
            str = "MAXIMUM";
            _Top = percent;
            if ( _DualUse )
                _LevelDelta = _Top - _Bottom;
            break;
        case ESTATE::DECAY:
        case ESTATE::SUSTAIN:
            str = "SUSTAIN LEVEL";
            _SetSustain = percent;
            break;
        case ESTATE::RELEASE:
            break;
        }
    DBG ("Setting %s > %f", str.c_str (), percent );
    }

//#######################################################################
void ENVELOPE_C::SetDualUse (bool sel)
    {
    _DualUse = sel;

    if ( sel )
        {
        _UseTremolo = false;
        _Current = _Bottom;
        _LevelDelta = _Top - _Bottom;
        Update ();
        DBG ("Enable Dual Use");
        }
    else
        {
        _Current = 0.0f;
        _Bottom  = 0.0f;
        Update ();
        DBG ("Disable Dual Use");
        }
    }

//#######################################################################
void ENVELOPE_C::SetModulationLevel (float lvl)
    {
    _Current = _Bottom + (_LevelDelta * lvl);
    _Updated = true;
    Update ();
    }

//#######################################################################
void ENVELOPE_C::Start ()
    {
    if ( _Active || (_Top == 0.0 || _Muted ) )
        return;
    _Active = true;
    _State = ESTATE::START;
    _UseCount++;
    DBG ("Starting");
    }

//#######################################################################
void ENVELOPE_C::End ()
    {
    if ( !_Active )
        return;
    _TriggerEnd = true;
    _State = ESTATE::IDLE;
    }

//#######################################################################
void ENVELOPE_C::SetOverride (uint16_t data)
    {
    I2cDevices.D2Analog (_DevicePortIO, data);
    }

//#######################################################################
void ENVELOPE_C::Update ()
    {
    float output;

    if ( _Updated )
        {
        output = _Current;
        if ( _UseTremolo )
            {
            float zf = *_TremoloOuput;
            float zl = _TremoloMaxLevel;
            if ( _TremoloInvert )
                zf = -zf;
            if ( _TremoloWheel )
                {
                zl *= _TremoloWheelLevel;
                }
            output *= (1.0 - 0.5 * (zf + 1.0) * zl);
            }
        int16_t z = (int16_t)(_DeviceRange * output * _Expression);    //Calculate final D to A with output level with expression and tremolo
        DBG ("Updating port %d with %d", _DevicePortIO, z)
        I2cDevices.D2Analog (_DevicePortIO, z);;
        _Updated = false;
        }
    }

//#######################################################################
//#######################################################################
void ENVELOPE_C::Process (float deltaTime)
    {
    if ( _UseTremolo )
        _Updated = true;

    //***************************************
    //  Beginning of the end
    //***************************************
    if ( _TriggerEnd && (_State != ESTATE::RELEASE) )
        {
        _State   = ESTATE::RELEASE;
        _Timer   = _ReleaseTime;
        _Delta   = _Current - _Bottom;
        DBG ("%f mSec from level %f to %f", _ReleaseTime, _Current, _Bottom);
        return;
        }

    switch ( _State )
        {
        //***************************************
        //  Start envelope
        //***************************************
        case ESTATE::START:
            {
            _Current = _Bottom;
            _Sustain = _SetSustain;             // update runtime sustain with sustain as user set
            _NoDecay = false;
            if ( _DecayTime < 8.0 )
                _NoDecay = true;

            _Timer       = 0.0;
            _Delta       = _Top - _Bottom;
            _PeakLevel   = false;
            _TargetTime  = _AttackTime - TIME_THRESHOLD;
            _State       = ESTATE::ATTACK;
            DBG ("Start > %f mSec from level %f to %f", _AttackTime, _Current, _Top);
            return;
            }
        //***************************************
        //  ATTACK
        //***************************************
        case ESTATE::ATTACK:
            {
            _Timer += deltaTime;
            if ( _Timer < _TargetTime )
                {
                _Current  = _Bottom + ((_Timer / _TargetTime) * _Delta);
                _Updated = true;
                DBG ("Timer > %f mSec at level %f", _Timer, _Current);
                return;
                }
            _Current     = _Top;
            _Updated     = true;
            if ( _NoDecay )
                {
                _Timer   = 0.0;
                _State = ESTATE::SUSTAIN;
                DBG ("Hold at level %f", _Current);
                }
            else
                {
                _Timer      = _DecayTime - TIME_THRESHOLD;;
                _State      = ESTATE::DECAY;
                _Delta      = _Top - _Sustain;
                _TargetTime = 0.0;
                DBG ("%f mSec from level %f to %f", _DecayTime, _Current, _Sustain);
                }
            return;
            }

        //***************************************
        //  DECAY
        //***************************************
        case ESTATE::DECAY:
            {
            _Timer -= deltaTime;
            if ( _Timer > 10 )
                {
                _Current = _Sustain + ((_Timer / _DecayTime) * _Delta);
                _Updated = true;
                DBG ("Timer > %f mSec at level %f", _Timer, _Current);
                return;
                }
            _Current = _Sustain;
            _Updated = true;
            _Timer   = 0.0;
            _State   = ESTATE::SUSTAIN;

            if ( _Sustain >= _Top )
                _PeakLevel = true;

            DBG ("sustained at level %f", _Current);
            return;
            }
        //***************************************
        //  SUSTAIN
        //***************************************
        case ESTATE::SUSTAIN:
            {
            if ( _PeakLevel && (_Current != _Top) && !_DualUse )
                {
                _Current = _Top;
                _Updated = true;
                }
            return;
            }
        //***************************************
        //  RELEASE
        //***************************************
        case ESTATE::RELEASE:
            {
            _TriggerEnd = false;
            _Timer  -= deltaTime;
            if ( _Timer > 20)
                {
                _Current = _Bottom + ((_Timer / _ReleaseTime) * _Delta);
                _Updated = true;
                DBG ("Timer > %f mSec at level %f", _Timer, _Current);

                // Process string damper
                bool damper = false;
                switch ( _DamperMode )
                    {
                    default:
                        break;
                    case DAMPER::NORMAL:
                        if ( _Damper )      damper = false;
                        else                damper = true;
                        break;
                    case DAMPER::INVERT:
                        if ( _Damper )      damper = true;
                        else                damper = false;
                        break;
                    }
                if ( !damper )
                    return;
                }
            Clear ();          // We got to here so this envelope process in finished.
            return;
            }
        }
    //***************************************
    //  This should never happen
    //***************************************
    DBG ("DANGER! DANGER! We should have never gotten here during envelope processing!");
    Clear ();
    }

//#######################################################################
ENV_GENERATOR_C EnvelopeGenerator;  //Envelope generator spawn tool

