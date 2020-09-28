#include <ESP8266WebServer.h>
#include <EEPROM.h>


//Variables
int statusCode;
String st;
String content;

//Function Decalration
bool testWifi(void);
void launchWeb(void);
void setupAP(void);
void handleLedConfig();

bool connectWifi()
{
    EEPROM.begin(512); //Initialasing EEPROM

    //---------------------------------------- Read eeprom for ssid and pass
    Serial.println("Reading EEPROM ssid");
    
    String esid;
    for (int i = 0; i < 32; ++i){
      esid += char(EEPROM.read(i));
    }
    Serial.println();
    Serial.print("SSID: ");
    Serial.println(esid);
    Serial.println("Reading EEPROM pass");
    
    String epass = "";
    for (int i = 32; i < 96; ++i){
      epass += char(EEPROM.read(i));
    }
    Serial.print("PASS: ");
    Serial.println(epass); 
    
    WiFi.mode(WIFI_STA);
    WiFi.hostname("luzCarol");
    WiFi.begin(esid.c_str(), epass.c_str());
    
    WiFi.hostname("luzcarol");
    if (testWifi() )  {
      Serial.println("Succesfully Connected!!!");
      Serial.print("Connected to:  ");
      Serial.println(WiFi.SSID());              // Tell us what network we're connected to
      Serial.print("IP address:    ");
      Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer        
      if (!MDNS.begin("luzcarol")) {             // Start the mDNS responder for esp8266.local
        Serial.println("Error setting up MDNS responder!");
        return true;
      }     
      return true;
    }
    return false;  
}



void setupAccessPoint()
{

    Serial.println("Turning the HotSpot On");
    launchWeb();
    setupAP(); // Setup HotSpot
   
    Serial.println();
    Serial.println("Waiting.");

    while ((WiFi.status() != WL_CONNECTED))
    {
//        Serial.print(".");
        server.handleClient();
        handleLedConfig();
    }
}


//----------------------------------------------- Fuctions used for WiFi credentials saving and connecting to it which you do not need to change
bool testWifi(void)
{
    int c = 0;
    Serial.println("Waiting for Wifi to connect");
    while (c < 120)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            return true;
        }
        delay(500);
        Serial.print("*");
        c++;
    }
    Serial.println("");
    Serial.println("Connect timed out, opening AP");
    return false;
}

void launchWeb()
{
    Serial.println("");
    if (WiFi.status() == WL_CONNECTED)
        Serial.println("WiFi connected");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("SoftAP IP: ");
    Serial.println(WiFi.softAPIP());
    createWebServer();
    // Start the server
    server.begin();
    Serial.println("Server started");
}

void scanWifi()
{
    delay(100);
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0)
        Serial.println("no networks found");
    else
    {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i)
        {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
            delay(10);
        }
    }
    Serial.println("");
    st = "<ol>";
    for (int i = 0; i < n; ++i)
    {
        // Print SSID and RSSI for each network found
        st += "<li>";
        st += WiFi.SSID(i);
        st += " (";
        st += WiFi.RSSI(i);

        st += ")";
        st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
        st += "</li>";
    }
    st += "</ol>";
    delay(100);  
}

void setupAP(void)
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    scanWifi();
    
    
    WiFi.softAP("OpenSoftwareRGB");//, "");
    Serial.println("Initializing_softap_for_wifi credentials_modification");
    launchWeb();
    Serial.println("over");
}

void createWebServer()
{
    {
        server.on("/", []() {
            scanWifi();
            IPAddress ip = WiFi.softAPIP();
            String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
            content = "<!DOCTYPE HTML>\r\n<html>Bem vindo &agrave; configura&ccedil;&atilde;o do seu aparelho.";
            content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"Lista de wifis\"></form>";
            content += ipStr;
            content += "<p>";
            content += st;
            content += "</p><form method='get' action='setting'>";
            content += "<label>Nome  wifi: </label><input name='ssid' length=32><br><br>";
            content += "<label>Senha wifi: </label><input name='pass' length=64><br><br>";
            content += "<input type='submit' value='Enviar'></form>";
            content += "</html>";
            server.send(200, "text/html", content);
        });
        server.on("/scan", []() {
            //setupAP();
            IPAddress ip = WiFi.softAPIP();
            String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);

            content = "<!DOCTYPE HTML>\r\n<html>go back";
            server.send(200, "text/html", content);
        });

        server.on("/setting", []() {
            String qsid = server.arg("ssid");
            String qpass = server.arg("pass");
            if (qsid.length() > 0 && qpass.length() > 0)
            {
                Serial.println("clearing eeprom");
                for (int i = 0; i < 96; ++i)
                {
                    EEPROM.write(i, 0);
                }
                Serial.println(qsid);
                Serial.println("");
                Serial.println(qpass);
                Serial.println("");

                Serial.println("writing eeprom ssid:");
                for (int i = 0; i < qsid.length(); ++i)
                {
                    EEPROM.write(i, qsid[i]);
                    Serial.print("Wrote: ");
                    Serial.println(qsid[i]);
                }
                Serial.println("writing eeprom pass:");
                for (int i = 0; i < qpass.length(); ++i)
                {
                    EEPROM.write(32 + i, qpass[i]);
                    Serial.print("Wrote: ");
                    Serial.println(qpass[i]);
                }
                EEPROM.commit();

                content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
                statusCode = 200;
                ESP.reset();
            }
            else
            {
                content = "{\"Error\":\"404 not found\"}";
                statusCode = 404;
                Serial.println("Sending 404");
            }
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(statusCode, "application/json", content);
        });
    }
}
