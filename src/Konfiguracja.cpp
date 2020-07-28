#include <ArduinoJson.h>
#include <Konfiguracja.h>
#include "FS.h"

StaticJsonDocument<150> lista;

Konfiguracja::Konfiguracja(const char * plik) 
{
	_plik = plik;
}
	
bool Konfiguracja::loadConfig() 
{
	//Serial.println(_plik);
	File configFile = SPIFFS.open(_plik, "r");
	if (!configFile) { return false;}
	size_t size = configFile.size();
	if (size > 1024) {return false;}
		//Serial.println(configFile.readString());
	std::unique_ptr<char[]> buf(new char[size]);
	configFile.readBytes(buf.get(), size);

	auto error = deserializeJson(lista, buf.get());
	if (error) {return false;};
	return true;
}

bool Konfiguracja::saveConfig() 
{
	File configFile = SPIFFS.open(_plik, "w");
	if (!configFile) return false;
	serializeJson(lista, configFile);
	return true;
}
