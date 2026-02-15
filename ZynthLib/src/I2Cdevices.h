//#######################################################################
// Module:     I2Cdevices.h
// Descrption: Real time I2C bus devices controller
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#pragma once

#define MAX_ANALOG_PER_BOARD  8

//#######################################################################
typedef struct
    {
    int         Cluster;        // Mux chip address
    int         Slice;          // Mux output select
    int         Port;           // I2C address of device
    int         NumberDtoA;     // Device count on channel: 1 = MCP4725, 4 = MCP4728
    int         NumberAtoD;
    int         NumberDigital;  // number of digital I/O channeld: 8 / 16
    const char* Name;
    } I2C_LOCATION_T;

using CallbackUShort = void (*)(ushort val);

//#######################################################################
class I2C_INTERFACE_C
    {
private:
    enum BOARD_TYPE
        {
        MCP4728 = 0,        // quad 12 bit digital to analog converter
        MCP47FXBX8,         // hex  12 bit digital to analog converter
        ADS1115,            // quad 16 bit analog to digital
        PCF8575,            // 8 bit (PC8574A) & 16 bit (PCF8575) digital out without pullups
        MCP23008,           // 8 bit digital out with pullups
        };

    typedef struct
        {
        I2C_LOCATION_T  Board;              // This board access info
        BOARD_TYPE      BoardType;          // one of the board types from enum
        bool            Valid;              // This board is valid
        uint16_t        NewDataMask;        // bits that represent data updates
        union
            {
            union
                {
                uint8_t     ByteData[MAX_ANALOG_PER_BOARD * 2];
                uint16_t    BitWord;
                };
            uint16_t    DtoA[MAX_ANALOG_PER_BOARD];
            uint64_t    DataDtoA[2];
            uint16_t    AtoD[MAX_ANALOG_PER_BOARD / 2];
            uint64_t    DataAtoD;
            uint16_t    DataDigital;
            };
        } I2C_BOARD_T;
    typedef struct I2C_DEVICE_S
        {
        I2C_BOARD_T*    pBoard;
        uint16_t*       pDtoA;
        uint16_t*       pDigital;
        int             DevIndex;
        uint16_t*       pAtoD;
        uint8_t         DtoAain;
            I2C_DEVICE_S (void) : pBoard(nullptr),
                                  pDtoA(nullptr),
                                  pAtoD(nullptr),
                                  pDigital(nullptr),
                                  DevIndex(0)
                {}
        } I2C_DEVICE_T;

    I2C_BOARD_T*    _pBoard;
    I2C_DEVICE_T*   _pDevice;
    int             _DeviceCount;
    int             _BoardCount;
    ushort          _AtoD_loopDevice;
    CallbackUShort  _CallbackAtoD;
    uint8_t         _LastEndT;
    bool            _DebugI2C;


    void     BuildTables        (I2C_LOCATION_T* plocation);
    char*    ErrorString        (int err);
    void     BusMux             (I2C_LOCATION_T& loc);
    void     EndBusMux          (I2C_LOCATION_T& loc);

    void     Write              (I2C_LOCATION_T& loc, uint8_t* buff, uint8_t length);
    void     WriteByte          (uint8_t port, uint8_t data);
    void     WriteRegisterByte  (uint8_t port, uint8_t addr, uint8_t data);
    void     WriteRegisterWord  (uint8_t port, uint8_t addr, uint16_t data);
    uint16_t ReadRegister16     (uint8_t port, uint8_t addr);
    void     Init47FXBX8        (I2C_LOCATION_T &loc);
    void     Init4728           (I2C_LOCATION_T &loc);
    void     Init857x           (I2C_LOCATION_T &loc);
    void     Init23008          (I2C_LOCATION_T &loc);
    void     Write47FXBX8       (I2C_BOARD_T& board);
    void     Write4728          (I2C_BOARD_T& board);
    void     Write857x          (I2C_BOARD_T& board);
    void     Write23008         (I2C_BOARD_T& board);
    uint8_t  DecodeIndex1115    (uint8_t index);
    void     Init1115           (I2C_LOCATION_T &loc);
    void     Start1115          (I2C_DEVICE_T& device);
    bool     ValidateDevice     (ushort board);

public:
         I2C_INTERFACE_C (void);
         // return:  0 = all good
         //         -1 = Total failure
         //         +X = Some interface errors
    int  Begin              (I2C_LOCATION_T* plocation);
    bool IsPortValid        (short device);
    void Loop               (void);
    bool IsAnalogIn         (short device);
    bool IsAnalogOut        (short device);
    bool IsDigitalOut       (short device);
    void D2Analog           (short device, ushort value);
    void DigitalOut         (short device, bool value);
    void StartAtoD          (short device);
    void AnalogClear        (void);
    void Update             (void);
    void SetDebug           (bool state)
        { _DebugI2C = state; }

    //#######################################################################
    void ResetAnalog (short device)
        { this->Init1115(_pDevice[device].pBoard->Board); }

    //#######################################################################
    int  NumBoards (void)
        { return (this->_BoardCount); }

    //#######################################################################
    int GetDeviceCount (void)
        { return (this->_DeviceCount); }

    //#######################################################################
    void SetCallbackAtoD (CallbackUShort fptr)
        { _CallbackAtoD = fptr; }

    };

//#######################################################################
extern  I2C_INTERFACE_C         I2cDevices;

