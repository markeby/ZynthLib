//#######################################################################
// Module:     Debug.h
// Descrption: Varialble length debug output
// Creator:    markeby
// Date:       3/17/2024
//#######################################################################
#pragma once
#include <Arduino.h>
#include <esp_debug_helpers.h>

#define DEBUG_SYNTH         1

#define DEBUG_NO_INDEX      255

extern bool __Debug_Off__;

const String vFormat  (const char *const zcFormat, ...);
const String vsFormat (const char *const zcFormat, va_list args);

void DebugMsg  (const char* label, uint8_t index, const char *const fmt, ...);
void DebugMsgN (const char* label, uint8_t index, String name,  const char *const fmt, ...);
void DebugMsgF (const char* label, uint8_t index, String name, char* flag, const char *const fmt, ...);
void ErrorMsg  (const char* label, const char* func, const char* const fmt, ...);

char* ErrorStringI2C (int err);
void  BootDebug      (void);

void  DebugDisable   (bool state);

#define PAUSE    {if (!__Debug_Off__) printf("--- %s:%d\n",__FILE_NAME__,__LINE__);while(!Serial.available ()) continue;char s=Serial.read();}

#define Dbg(x)  {if (!__Debug_Off__) Serial.printf("==> %s:%d %s = ",__FILE_NAME__,__LINE__, #x); Serial.println(x);}

#define DbgN    {printf("\n");}

