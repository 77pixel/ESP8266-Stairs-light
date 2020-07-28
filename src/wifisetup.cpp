#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <Konfiguracja.h>
#include <ArduinoJson.h>
#include <spiffsh.h>

//Konfiguracja wifiknf("/config/wifi.json");
ESP8266WebServer server(80);

int WIFIhtml()
{
    String txt = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'><meta charset='UTF-8'></head><bodystyle='font-family: Tahoma, sans-serif;>";
    
    //wifiknf.loadConfig();
    
    if(server.method() == HTTP_POST)
    {
        if (server.hasArg("naz"))
        { 
           // wifiknf.lista["nazwa"] = server.arg("nazwau");

            txt +="<h1> Zapisano nazwę. Zrestartuj urządzenie...</h1>";
        }
        else if(server.hasArg("res"))
        {
            ESP.restart();
        }
        else
        { 
            String ssid = server.arg("ssid");
           // wifiknf.lista["ssid"] = server.arg("ssid");
           // wifiknf.lista["pass"] = server.arg("pass");
            txt +="<h1> Zapisano sieć WiFi. Zrestartuj urządzenie...</h1>";
        }
    }
    else
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            txt += "<h3>Połączono z <b>" + WiFi.SSID() + " (" + WiFi.localIP().toString() + ")</b></h3>";
        }

        int n = WiFi.scanNetworks();
        if (n == 0) 
        {
            txt += "<h1>BRAK SIECI</h1>";
        } 
        else 
        {
            txt +="<div style=''><h3>Lista znalezionych sieci:</h3>";
            txt +="<table id='tab' style=';'><thead><tr><th></th><th>Nazwa</th><th>Moc</th><th>Kanał</th><th>Zab.</th></tr></thead><tbody>";
            
            for (int i = 0; i < n; ++i) 
            {
                String enc = (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "Brak" : "Tak";
                
                txt +="<tr style='cursor: pointer;'>";

                txt +="<td>"  + String(i+1) + "</td>"
                    + "<td>" + WiFi.SSID(i) + "</td>"
                    + "<td>" + WiFi.RSSI(i) + " dB</td>"
                    + "<td stytle='text-align: right;'>" + WiFi.channel(i) + "</td>"
                    + "<td>" + enc + "</td>"
                    + "</tr>";
            }

            txt +="</tbody></table></div>";
        }

        String nazwa = "Schody";//wifiknf.lista["nazwa"];

        txt += "<div id='con' style='display:none'><form method='post'>Nazwa: <input id='ssid' name='ssid' readonly><br>Hasło: <input id='pass' name='pass' type='password'><input type='submit' value='Zapisz'></form></div>";
        txt += "<div><form method='post'><input value='1' id='naz' name='naz' hidden>Nazwa urządzenia: <input id='nazwau' name='nazwau' value='"+nazwa+"'><input type='submit' value='Zmień'></form></div>";
        txt += "<div><form method='post'><input id='res' name='res' hidden><input type='submit' value='Reset'></form></div>";
        txt += "<script>";
        txt += "document.getElementById('tab').addEventListener('click', m);";
        txt += "function m(e){document.getElementById('ssid').value=e.target.parentNode.cells[1].innerText;document.getElementById('con').style.display='block'}";
        txt += "</script>"; 
  
    }

   // wifiknf.saveConfig();

    txt +="</body></html>";
    server.send(200, "text/html", txt);
    return 1;
}

void WIFIsetup()
{
    WiFi.mode(WIFI_STA);   
	WiFi.disconnect();
	delay(500);

   // wifiknf.loadConfig();

	String nazwa = "Schody";    //wifiknf.lista["nazwa"];  
	String ssid = "HOMENET";    //wifiknf.lista["ssid"];
	String password = "!@#$5678Dom";//wifiknf.lista["pass"];

	if(ssid != "")
	{
		WiFi.hostname(nazwa);
		WiFi.begin(ssid, password);
		int i = 0;
		while (WiFi.status() != WL_CONNECTED) 
		{
			delay(1000);
			if(i > 9) break;
			i++;
		}
	}

	if (WiFi.status() != WL_CONNECTED)
	{
		WiFi.mode(WIFI_AP);
		
		delay(100);
		IPAddress ip(192,168,99,1);
		IPAddress brama(192,168,99,1);
		IPAddress maska(255,255,255,0);
		
		WiFi.softAP(nazwa);
		WiFi.softAPConfig(ip, brama, maska);
		
	}
    server.on("/wifi", WIFIhtml);
}
