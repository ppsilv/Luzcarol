#include <ESP8266WebServer.h>

//#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <ESP8266mDNS.h>        // Include the mDNS library

//If this pin is low at boot, force ssid config.
#define CONFIG_PIN 5

//LED Connections
const int RedLED=14;     //14  GPIO15
const int GreenLED=12;  //D6  GPIO12
const int BlueLED=13;    //D7  GPIO13

#define LED_BUILTIN 16

ESP8266WebServer server(80);

bool connectWifi();
void setupAccessPoint();
void setupServer();

void setup()
{
    Serial.begin(115200); //Initialising if(DEBUG)Serial Monitor
    Serial.println();
    Serial.println("Disconnecting current wifi connection");
    WiFi.disconnect();
    delay(10);
    
    pinMode(CONFIG_PIN, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);

    if( digitalRead(CONFIG_PIN) == HIGH ) {
      Serial.println("The config key is on HIGH state calling normal process..."); 
      if ( ! connectWifi() ){
        setupAccessPoint();      
      }
    }else{  
      Serial.println("The config key is on LOW state calling configuration..."); 
      setupAccessPoint();
    }
    pinMode(RedLED,OUTPUT);
    pinMode(GreenLED,OUTPUT);
    pinMode(BlueLED,OUTPUT);      

    setupServer();

    setupOta();
}

void execWork(long r, long g, long b){
  //ESP supports analogWrite All IOs are PWM

  analogWrite(RedLED,r);
  analogWrite(GreenLED,g);
  analogWrite(BlueLED,b);  
}
void handleLed()
{
  static uint8_t ledState = LOW;
  static long timeHigh = millis()+1000;
  static long timeLow  = millis()+100;
  
  if( ledState == HIGH ){
    if ( millis() > timeHigh  ){
      digitalWrite(LED_BUILTIN, LOW);
      timeLow  = millis()+100;
      ledState = LOW;  
    }
  }
  if( ledState == LOW ){
    if( millis() > timeLow){
      digitalWrite(LED_BUILTIN, HIGH);
      ledState = HIGH;  
      timeHigh = millis()+1000;
    }
  }
}
void handleLedConfig()
{
  static uint8_t ledState = LOW;
  static long timeHigh = millis()+100;
  static long timeLow  = millis()+1;
  
  if( ledState == HIGH ){
    if ( millis() > timeHigh  ){
      digitalWrite(LED_BUILTIN, LOW);
      timeLow  = millis()+1;
      ledState = LOW;  
    }
  }
  if( ledState == LOW ){
    if( millis() > timeLow){
      digitalWrite(LED_BUILTIN, HIGH);
      ledState = HIGH;  
      timeHigh = millis()+100;
    }
  }
}

void loop()
{
    loopOta();
    if ((WiFi.status() == WL_CONNECTED))
    {
        server.handleClient();
        handleLed();
    }

}

/*

Configuração:

0 - Ligar a chave para configuração.
1 - Ligar o equipamento.
2 - Conferir a luz no equipamento está piscando rápido.
3 - Configurar seu celular para o wifi OpenSoftwareRGB(Não precisa de senha)
    obs.: Alguns celulares perguntam se voce quer manter a escolha, porque está sem internet, escolha sim para manter a conexão.  
4 - No browser(chrome) do seu celular apontar para o endereço: http://192.168.4.1
5 - Apareceu uma lista de wifi
    Caso o nome do seu wifi não aparecer na lista, atualize a página até aparecer, se voce já sabe o nome dele
    é só digitar no box Nome wifi.
6 - Se essa página estiver pequena aumente, para que voce possa ver e digitar melhor.
7 - No rodapé dessa página tem 2 boxes para configuração.
8 - O primeiro é para voce escrever o nome do seu wifi, que deve ter aparecido na lista de wifi acima
9 - E o segundo é para voce escrever a senha do seu wifi.
10 - desligue a chave configuração
11- confira se o nome do wifi e a senha estão corretos, se estiverem clique no botão Enviar




 */
