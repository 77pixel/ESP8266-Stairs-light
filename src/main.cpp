#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include <wifisetup.h>
#include <Wire.h>
#include <BH1750.h>

int tryb = 0;
int nrled = 0;
int krok = 0;
int kier = 1;
unsigned long czasPop;
int cykle = 5;
int cykl = 0;

unsigned long czas = 2;
unsigned long czasLEDzap = 25; 
unsigned long czasLEDgas = 65;
int maxjas = 100;
int r = 255;
int g = 255;
int b = 240;
int efekt = 0;

#define PIN            D8 //nodemcu
#define NUMPIXELS      14

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Konfiguracja konfig("/config/app.json");
BH1750 lightMeter(0x23);

String HTTPlink(String link)
{
  	HTTPClient http;    	 
  	http.begin(link);     
  	int httpCode = http.GET();  
	if(httpCode == 404) return "Brak Strony";         
 	String ret = http.getString();
  	http.end();  
	return ret;
}

void ustawZmienne()
{
	konfig.loadConfig();
	
	r = konfig.lista["r"];
	g = konfig.lista["g"];
	b = konfig.lista["b"];
	czas = konfig.lista["czas"]; 
	czasLEDzap = konfig.lista["czasLEDzap"]; 
	czasLEDgas = konfig.lista["czasLEDgas"];
	maxjas = konfig.lista["maxjas"];
	efekt = konfig.lista["efekt"];
	krok = maxjas/cykle; 
}

void setKolor(int r, int g, int b)
{
	for (int i = 0; i < NUMPIXELS; i++)
	{
		pixels.setPixelColor(i, pixels.Color(r, g, b));	
	}
	pixels.show();
}

void zapal2(int faza)
{
	setKolor(0,0,0);	
	for (int i = 0; i < NUMPIXELS; i++)
	{
		if ( i % 2 == faza ) pixels.setPixelColor(i, pixels.Color(r, g, b));	
	}
	pixels.show();
}

void zapal3(int i)
{
	setKolor(0,0,0);	
	pixels.setPixelColor(i, pixels.Color(r, g, b));	
	pixels.show();
}

void handleFile()
{
	String n = server.uri();
	if (n == "/") n = "/index.html";
	File f = handleFileRead(n);
	if (f)
	{	
		String contentType = getContentType(n);
		server.streamFile(f, contentType);
	}
	else
	{
		server.send(404, "text/plain", "404: BRAK" + n);	
	}
}

void handleTryb()
{
	int k = server.arg("kier").toInt();
	if(k == 1)
	{
		nrled = 0;
		kier = 1;	
		tryb = 1;
	}
	else if(k == 0)
	{
		nrled = 13;
		kier = 0;	
		tryb = 1;
	}
	else if(k == 2)
	{
		tryb = 3;
	}
	else if(k == 3)
	{
		tryb = 4;

	}
	czasPop = millis();
	server.send(200, "text/plain", "ok");
}

String str()
{
	
	float lux = lightMeter.readLightLevel();
	IPAddress ip = WiFi.localIP();

	String s = "{\"czas\" : \""			+ String(czas) +
           		"\",\"czasLEDzap\":\""	+ String(czasLEDzap) +
             	"\",\"czasLEDgas\":\"" 	+ String(czasLEDgas) + 
             	"\",\"maxjas\":\"" 		+ String(maxjas) + 
             	"\",\"r\":\"" 			+ String(r) + 
			 	"\",\"g\":\"" 			+ String(g) + 
			 	"\",\"b\":\"" 			+ String(b) + 
				"\",\"efekt\":\"" 		+ String(efekt) +
			 	"\",\"pomiar\":\"" 		+ String(lux) +
				"\",\"ip\":\"" 			+ ip.toString() +
				"\",\"mac\":\"" 		+ WiFi.macAddress() +
				"\"}";
	return s;
}

void handleSet()
{
	if(server.hasArg("czas")) konfig.lista["czas"] = server.arg("czas").toInt();
	if(server.hasArg("czasLEDzap")) konfig.lista["czasLEDzap"] = server.arg("czasLEDzap").toInt(); 
	if(server.hasArg("czasLEDgas")) konfig.lista["czasLEDgas"] = server.arg("czasLEDgas").toInt();;
	if(server.hasArg("maxjas")) konfig.lista["maxjas"] = 	 server.arg("maxjas").toInt();
	if(server.hasArg("efekt")) konfig.lista["efekt"] = 	 server.arg("efekt").toInt();
	if(server.hasArg("r")) konfig.lista["r"] = 		 server.arg("r").toInt();
	if(server.hasArg("g")) konfig.lista["g"] = 		 server.arg("g").toInt();
	if(server.hasArg("b")) konfig.lista["b"] = 		 server.arg("b").toInt();

	konfig.saveConfig();
	ustawZmienne();

	server.send(200, "text/json", str());
}

void handleGet()
{
	server.send(200, "text/json", str() );
}

void zapal(int nled)
{
	
	for (int i = 0; i<maxjas; i+= krok)
	{
		pixels.setPixelColor(nled, pixels.Color(r*i/100, g*i/100, b*i/100));
		pixels.show();
		delay(czasLEDzap);	
	}

	if(efekt == 1)
	{
		for (int i = maxjas/2; i>-1; i-=krok)
		{
			pixels.setPixelColor(nled, pixels.Color(r*i/100, g*i/100, b*i/100));
			pixels.show();
			delay(20);	
		}

		for (int i = maxjas/2; i<maxjas; i+= krok)
		{
			pixels.setPixelColor(nled, pixels.Color(r*i/100, g*i/100, b*i/100));
			pixels.show();
			delay(8);	
		}

		for (int i = maxjas/3; i>-1; i-=krok)
		{
			pixels.setPixelColor(nled, pixels.Color(r*i/100, g*i/100, b*i/100));
			pixels.show();
			delay(20);	
		}

		for (int i = maxjas/3; i<maxjas; i+= krok)
		{
			pixels.setPixelColor(nled, pixels.Color(r*i/100, g*i/100, b*i/100));
			pixels.show();
			delay(8);	
		}
	}
}

void zgas(int nled)
{
	for (int i = maxjas; i>-1; i-=krok)
	{
		pixels.setPixelColor(nled, pixels.Color(r*i/100, g*i/100, b*i/100));
		pixels.show();
		delay(czasLEDgas);	
	}
	pixels.setPixelColor(nled, pixels.Color(0, 0, 0));
}

byte * Wheel(byte WheelPos) 
{
	static byte c[3];
	if (WheelPos < 85) 
	{
		c[0] = WheelPos * 3;
		c[1] = 255 - WheelPos * 3;
		c[2] = 0;
	} 
	else if (WheelPos < 170) 
	{
		WheelPos -= 85;
		c[0] = 255 - WheelPos * 3;
		c[1] = 0;
		c[2] = WheelPos * 3;
	} 
	else 
	{
		WheelPos -= 170;
		c[0] = 0;
		c[1] = WheelPos * 3;
		c[2] = 255 - WheelPos * 3;
	}
  	return c;
}

void setup (void) 
{
	Serial.begin (115200);
	
	SPIFFS.begin();
	
	WIFIsetup();
	
	server.on("/", handleFile);
	server.on("/set", HTTP_POST, handleSet);
	server.on("/tryb", HTTP_POST, handleTryb);
	server.on("/get", HTTP_POST, handleGet);

	server.onNotFound(handleFile);
	server.begin();
	
	Serial.println();
	Serial.println();
	Serial.print("Nazwa: Schody");
	Serial.println();
	Serial.print("IP:  ");
	Serial.println(WiFi.localIP());
   	Serial.print("MAC: ");
   	Serial.println(WiFi.macAddress());

	ArduinoOTA.setHostname("Schody"); 
	ArduinoOTA.begin();
	pixels.begin();
	setKolor(0,0,0);
	ustawZmienne();

	Wire.begin(D4, D3);
	if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE_2)) Serial.println(F("BH1750 OK"));

	pinMode(D1, INPUT); 
	pinMode(D2, INPUT);
}

void loop (void) 
{
	server.handleClient();
  	ArduinoOTA.handle();
	
	switch (tryb)
	{
		case 0:
			if (digitalRead(D1) != LOW) //jest
			{
				nrled = 0;
				kier = 1;	
				tryb = 1;
				czasPop = millis();
			}

			if (digitalRead(D2) != LOW) //jest
			{	
				nrled = 13;
				kier = 0;
				tryb = 1;
				czasPop = millis();
			}	
		break;
		case 1:
			if(kier)
			{
				if(nrled < NUMPIXELS)
				{
					zapal(nrled);		
					nrled++;
				}
				else
				{
					tryb = 2;
					nrled = 0;
				}
			}
			else
			{
				if(nrled > -1)
				{
					zapal(nrled);		
					nrled--;
				}
				else
				{
					tryb = 2;
					nrled = 13;
				}
			}
		break;
		case 2:		
			if(czasPop + (czas*1000) < millis()) 
			{
				if(kier)
				{
					if (digitalRead(D1) == LOW) //brak
					{
						if(nrled < NUMPIXELS)
						{
							zgas(nrled);
							nrled++;
						}
						else
						{
							tryb = 0; //koniec
						}
					}
				}
				else
				{
					if (digitalRead(D2) == LOW) //brak
					{
						if(nrled > -1)
						{
							zgas(nrled);		
							nrled--;
						}
						else
						{
							tryb = 0;
						}
					}
				}
			}
		break;
		case 3:
			if (digitalRead(D1) != LOW || digitalRead(D2) != LOW)tryb = 0;
			
			if(czasPop + (1000) < millis()) 
			{
				zapal2(cykl);
				cykl = !cykl;
				czasPop = millis();
			}
			
		break;
		case 4:
			if (digitalRead(D1) != LOW || digitalRead(D2) != LOW)tryb = 0;
			
			if(czasPop + (300) < millis()) 
			{
				zapal3(cykl);
				cykl +=1;
				if(cykl > NUMPIXELS) cykl = 0;
				czasPop = millis();
			}
			
		break;
	}
}