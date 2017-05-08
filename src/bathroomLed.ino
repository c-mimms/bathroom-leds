#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266WiFiMulti.h"
#include <ESP8266HTTPClient.h>
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER   // map output pins to what they are called on the NodeMCU
#include "FastLED.h"

#include <ArduinoJson.h>

#define NUM_LEDS 50

// Data pin that led data will be written out over
#define DATA_PIN 3

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

int stall1 = 0;
int stall2 = 0;

void setup() {
    // sanity check delay - allows reprogramming if accidently blowing power w/leds
   FastLED.addLeds<WS2811, DATA_PIN>(leds, NUM_LEDS);

   Serial.begin(9600);

   WiFi.mode(WIFI_STA);
   WiFi.begin("Mirror", "S@lesforcE123");
   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
   }
}


void loop()
{
  callUrlForStall();
  paintLeds();
  delay(1000);
}

void callUrlForStall() {
  HTTPClient http;
  String address = "http://pardot-pingpong.herokuapp.com/bathrooms.json";
  Serial.print("\nAddress : " + address);
  // Get request
  http.begin(address);
  int httpCode = http.GET();

  // httpCode will be negative on error
  if(httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if(httpCode == HTTP_CODE_OK) {
          StaticJsonBuffer<2000> jsonBuffer;
          String payload = http.getString();
          Serial.println(payload.substring(1, payload.length()-1));
          JsonObject& root = jsonBuffer.parseObject(payload.substring(1, payload.length()-1));
          if (!root.success()) {
            Serial.println("parseObject() failed");
          }
          String state = root["stalls"][0]["state"];
          if(root["stalls"][0]["state"] == "true"){
            stall1 = 1;
          }
          else{
            stall1 = 0;
          }
          if(root["stalls"][1]["state"] == "true"){
            stall2 = 1;
          }
          else{
            stall2 = 0;
          }
      }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

void paintLeds(){
    if(stall1 == 1){
      leds[48] = CRGB::Red;
    }
    else{
      leds[48] = CRGB::Green;
    }

    if(stall2 == 1){
      leds[49] = CRGB::Red;
    }
    else{
      leds[49] = CRGB::Green;
    }
    FastLED.show(); // display this frame
}


void flashLeds(CRGB color){
  for( int j = 0; j < NUM_LEDS; j++) {
    FastLED.clear();
      leds[j] = color;
      FastLED.show(); // display this frame
  }
}
