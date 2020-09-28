/*
 * IoT ESP8266 Based Mood Lamp (RGB LED) Controller Program
 * https://circuits4you.com
 * https://circuits4you.com/2019/03/28/esp8266-iot-rgb-led-strip-mood-lamp-controller/
 */
 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
 
//SSID and Password of your WiFi router
//const char* ssid = "2.4G_OpenSoftware";
//const char* password = "";
 
String setcolor="#ff00ff"; //Set color for HTML

void execWork(long r, long g, long b);
 
const char MAIN_page[]  = R"=====(
<!DOCTYPE html>
<html>
<head>
<title>Mood Lamp (RGB LED Color) Controller</title>
</head>
<body style="background:@@color@@;">
 
<center>
    Mood Lamp (RGB LED Color) Controller<br><br><br>
  <form method="post" action="/form">
      Color:
      <input type="color" name="color" value="@@color@@">
      <button type="submit" name="state" value="stop">SET</button>
  </form>
<br><br><br>
<a href="Papai te ama filhinha">Papai te ama filhinha!!!</a><br>
</center>
</body>
</html>
)=====";
 
//=======================================================================
//                    handles main page
//=======================================================================
void handleRoot() {
  String p = MAIN_page;  
  p.replace("@@color@@",setcolor);    //Set page background color and selected color
  server.send(200, "text/html", p);    
  Serial.println("Loading page...");
}
 
//=======================================================================
//                    Handle Set Color
//=======================================================================
void handleForm() {
  String color = server.arg("color");
  //form?color=%23ff0000
  setcolor = color; //Store actual color set for updating in HTML
  
  //See what we have recived
  //We get #RRGGBB in hex string
 
  // Get rid of '#' and convert it to integer, Long as we have three 8-bit i.e. 24-bit values
  long number = (int) strtol( &color[0], NULL, 16);
 
  //Split them up into r, g, b values
  long r = number >> 16;
  long g = (number >> 8) & 0xFF;
  long b = number & 0xFF;

  //Serial.print("Cor..: ");Serial.print(number,16);
  //Serial.print(" Red..: ");Serial.print(r);
  //Serial.print(" Green: ");Serial.print(g);
  //Serial.print(" Blue.: ");Serial.println(b);
  
  //PWM Correction
  r = r * 4; 
  g = g * 4;
  b = b * 4;
  
  //Execute the work we are for.
  execWork(r,g,b);
  
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Updated-- Press Back Button");
 
  delay(500);  
}
//=======================================================================
//                    SETUP
//=======================================================================
void setupServer(){
 
  server.on("/", handleRoot);  //Associate handler function to path
  server.on("/form",handleForm);
    
  server.begin();                           //Start server
  Serial.println("HTTP server started");
}
