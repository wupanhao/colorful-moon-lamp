#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
//#include "index.h"

#include "FastLED.h"
#define NUM_LEDS 3
CRGB leds[NUM_LEDS];
void ledInit(){
   FastLED.addLeds<NEOPIXEL, 8>(leds, NUM_LEDS);
  };
void setCorlor(int red,int green,int blue){
  char i;
  for(i=0;i<NUM_LEDS;i++){
    leds[i] = CRGB(red,green,blue);
    }
    FastLED.show();
  };


// AP password
const char WiFiAPPSK[] = "12345678";

ESP8266WebServer server(80);

const int led = 13;

void handleRoot() {
   String res = "<!DOCTYPE html><html><head>  <title>Moon Lamp</title>";
      res+="<meta name=viewport content=\"width=device-width, initial-scale=1\">";
      res+="<meta name=viewport content=\"width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no\">";
      res+="<style type=\"text/css\">";
      res+="input{width: 100%;height: 72px;}";
      res+="label{font-size: 24px;}";
      res +="</style>";
      res +="</head><body>";
      res +="<H1>Welcome To Moon Lamp</H1>";
      res +="<form id=myForm action=/set-color>";
      res += "<div><label for=red> Red </label><div></div> <input type=range name=red id=red value=0 onchange=Submit()></div>";
      res += "<div><label for=red> Green </label><div></div> <input type=range name=green id=green value=0 onchange=Submit()></div>";
      res += "<div><label for=red> Blue </label><div></div> <input type=range name=blue id=blue value=0 onchange=Submit()></div>";
      res +="</form>";
      res +="<script type='text/javascript'>";
      res +=" function setRGB(red,green,blue) {";
      res +="                var xmlHttp = '';";
      res +="                var requstData = '';";
      res +="                var html = '';";
      res +="                if (window.XMLHttpRequest) {";
      res +="                    xmlHttp = new XMLHttpRequest();";
      res +="                } else {";
      res +="                    xmlHttp = new ActiveXObject('Microsoft.XMLHTTP');";
      res +="                }";
      res +="                xmlHttp.open('GET', '/set-color?red='+red+'&green='+green+'&blue='+blue, true); ";
      res +="                xmlHttp.send(); ";

      res +="                xmlHttp.onreadystatechange = function() {";
      res +="                    if (xmlHttp.readyState == 4 && xmlHttp.status == 200) {";
      res +="                        requstData = xmlHttp.responseText; ";
      res +="                        console.log(requstData);";
      res +="                    }";
      res +="                };";
      res +="            }";
      res +=" function setColor(){";
      res +="   var red = document.getElementById('red').value;";
      res +="   var green = document.getElementById('green').value;";
      res +="   var blue = document.getElementById('blue').value;";
      res +=" }";
      res +=" function Submit(){";
      res +="   var red = document.getElementById('red').value;";
      res +="   var green = document.getElementById('green').value;";
      res +="   var blue = document.getElementById('blue').value;";
      res +="   console.log(red,green,blue);";
      res +="   setRGB(red,green,blue);";
      res +=" }";
      res +="</script>";
      res +="</body>";
      res +="</html>";

      server.send(200, "text/html", res);

}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){
  ledInit();
  Serial.begin(115200);

    // AP SSID Name
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) + String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();

  String AP_NameString = "Moon Lamp - " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i = 0; i < AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
  IPAddress myIP = WiFi.softAPIP();
  
 /*
//  Serial.println(res);

const char* ssid = "SSID";
const char* password = "password";
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }


  */

  server.on("/", handleRoot);

  server.on("/set-color", [](){
    String uri = server.uri();
    Serial.println(uri);
    String red = server.arg("red");
    String green = server.arg("green");
    String blue = server.arg("blue");
    Serial.println("red="+red+" green="+green+" blue="+blue);
    setCorlor(red.toInt(),green.toInt(),blue.toInt());
    server.send(200, "text/plain", String("set to ")+"red="+red+" green="+green+" blue="+blue);
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
