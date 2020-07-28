#ifndef Konfiguracja_h
#define Konfiguracja_h

#include "Arduino.h"
#include <ArduinoJson.h>
#include "FS.h"

class Konfiguracja 
{        
    public: 
    Konfiguracja(const char * plik);
    StaticJsonDocument<1500> lista;
    bool loadConfig();
    bool saveConfig(); 
    private:
    const char * _plik;
};

#endif
