
#include <Arduino.h>
#include "web_server.h"
#include "tuner.h"
// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>

#define HOST "radio"

extern TUNER tuner;
extern float freq;
extern char *cbuff;

// Replace with your network credentials
const char* ssid = SERVER_DEFAULT_SSID;
const char* password = SERVER_DEFAULT_WIFI_PASSW;

bool ledState = 0;
const int ledPin = D0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html> 
    <head>
      <title>Radio Tuner</title>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <link rel="stylesheet" href="https://hamzaozkan.com.tr/uploads/device/tuner/style.css">
      <script type="text/javascript" src="https://hamzaozkan.com.tr/uploads/device/tuner/app.js" > </script>
    <title>Radio Tuner</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" href="data:,">
    </head>
    <body>
      <div class="topnav">
        <h1>Radio Tuner</h1>
      </div>
      <div class="content">
        <div class="slider">
          <div class="top_nav_slider">
          <span id="prev_freq"><</span>
            <div class="center_f_tex">
                <span id="rangeValue">105.300</span> 
                <span class="funit">MHz</span> 
            </div>
            <span id="next_freq">></span>
          </div>  
          <input type="range" id="radio_slider" min="0" max="900.000" value="" step="0.05"  oninput="rangeValue.innerHTML = this.value">
        </div>
      </div>
    </body>
</html>)rawliteral";

void notifyClients() {
  //ws.textAll(String(ledState));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;

    if (strcmp((char*)data, "get_freq") == 0) {
        ws.textAll(String(cbuff));
    }

    if(strncmp((char*)data, "freq", 4)==0){
        while (!isdigit(*data)) ++data;
        freq = atof((char*)data);
        printf("Set freq: %f\n", freq);
        tuner.set_frequency(freq);
        
    }

  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  return String(var);
}

void init_server(){

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  MDNS.begin(HOST);

  digitalWrite(ledPin, HIGH);

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Start server
  server.begin();
}

void clear_cient(){
    ws.cleanupClients();
    MDNS.update();
}

/*
void loop() {
  ws.cleanupClients();
  digitalWrite(ledPin, ledState);
}*/
