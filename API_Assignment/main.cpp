#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

const char* ssid = "Canelo WiFi";
const char* password = "MuffinMan3000";
const int redPin = 5;    // Pin for the red RGB LED
const int greenPin = 4; // Pin for the green RGB LED
const int bluePin = 0;  // Pin for the blue RGB LED
const int ledPin = 15; // Pin D8 for normal LED

void connectWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi ");
}

void setup() {
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
    Serial.begin(9600);
    connectWiFi();                      //Connect to WIFI
    Serial.println("End of setup");
}

void setColor(int red, int green, int blue) {
    analogWrite(redPin, red);
    analogWrite(greenPin, green);
    analogWrite(bluePin, blue);
    pinMode(ledPin, OUTPUT);
}

void fetchLEDStatus() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        WiFiClientSecure client;
        client.setInsecure();
        
        String url = "https://limegreen-ibex-462177.hostingersite.com/updatelights.php";
        Serial.println("Connecting to the following URL:");
        Serial.println(url); 
          
            http.begin(client,url);
            int httpCode = http.GET();
        
        if (httpCode > 0) {
            //Serial.print("HTTP Code: ");
            //Serial.println(httpCode);
            String payload = http.getString();
            Serial.println("Data received: " + payload);

            // Create a JSON document and parse the response
            StaticJsonDocument<200> doc;  // Adjust size based on your expected response
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                // Extract values from JSON
                const char* ledState = doc["LED_Control"];
                int r = doc["RGB"]["R"];
                int g = doc["RGB"]["G"];
                int b = doc["RGB"]["B"];
                    Serial.print("RGB values: ");
                    Serial.print("R: ");
                    Serial.print(r);
                    Serial.print(" G: ");
                    Serial.print(g);
                    Serial.print(" B: ");
                    Serial.println(b);
                        
                // Set LED and RGB values
                digitalWrite(ledPin, (strcmp(ledState, "ON") == 0) ? HIGH : LOW);
                setColor(r, g, b);
            } else {
                Serial.println("Failed to parse JSON: " + String(error.c_str()));
            }
        } else {
            Serial.println("Error in HTTP request");
        }
        http.end();
    } else {
        Serial.println("WiFi not connected");
    }
}

void loop() {
    fetchLEDStatus();  // Fetch LED status and RGB values
    delay(10000);       // Wait for 100 seconds before checking again
}




