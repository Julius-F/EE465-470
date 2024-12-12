#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "Canelo WiFi";
const char* password = "MuffinMan3000";

// MQTT variables
const char* mqtt_server = "broker.hivemq.com";
const char* publishTopic = "testtopic/temp/outTopic/starfoxesp"; // Unique topic
const char* switchTopic = "testtopic/temp/outTopic/lunabearesp"; // Unique topic for switch
const char* subscribeTopic = "testtopic/temp/inTopic/solarbatesp"; // Subscription topic
#define publishTimeInterval 20000 // 20 seconds

// Definitions
unsigned long lastMsg = 0;
unsigned long switchLastPressed = 0;
#define MSG_BUFFER_SIZE (50)
WiFiClient espClient;
PubSubClient client(espClient);
char msg[MSG_BUFFER_SIZE];

// Pin assignments
#define POT_PIN A0          // Potentiometer connected to A0
#define SWITCH_PIN D1       // Switch connected to digital pin D1
#define LED_PIN D2          // LED connected to digital pin D2

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (String(topic) == subscribeTopic) {
    if ((char)payload[0] == '1') {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED Turned ON via MQTT");
    } else if ((char)payload[0] == '0') {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED Turned OFF via MQTT");
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(subscribeTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void readAndPrintPotentiometer() {
  int potValue = analogRead(POT_PIN); // 10-bit ADC value (0-1023)
  float voltage = (potValue / 1023.0) * 3.3; // Assuming 3.3V reference voltage
  Serial.print("Potentiometer Value: ");
  Serial.print(potValue);
  Serial.print(" (Volts: ");
  Serial.print(voltage);
  Serial.println(")");
}

void checkSwitchState() {
  static int lastSwitchState = HIGH;
  int switchState = digitalRead(SWITCH_PIN);

  if (switchState == LOW && lastSwitchState == HIGH) { // Switch pressed
    snprintf(msg, MSG_BUFFER_SIZE, "1");
    client.publish(switchTopic, msg);
    Serial.println("Switch Pressed: Published 1");
    switchLastPressed = millis();
  }

  if (switchState == HIGH && lastSwitchState == LOW) { // Switch released
    unsigned long now = millis();
    if (now - switchLastPressed >= 5000) {
      snprintf(msg, MSG_BUFFER_SIZE, "0");
      client.publish(switchTopic, msg);
      Serial.println("Switch Released: Published 0");
    }
  }
  lastSwitchState = switchState;
}

void handleSerialCommands() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == '1') {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED Turned ON");
    } else if (command == '0') {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED Turned OFF");
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Initialize pins
  pinMode(SWITCH_PIN, INPUT_PULLUP); // Configure switch pin as input with pull-up resistor
  pinMode(LED_PIN, OUTPUT);          // Configure LED pin as output
  digitalWrite(LED_PIN, LOW);        // Ensure LED is initially off

  Serial.println("ESP8266 Ready. Enter '1' to turn on the LED, '0' to turn it off.");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > publishTimeInterval) {
    lastMsg = now;
    int potValue = analogRead(POT_PIN); // Read potentiometer value
    snprintf(msg, MSG_BUFFER_SIZE, "Potentiometer Value: %d", potValue);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(publishTopic, msg);
  }

  // Keep other functionalities available
  readAndPrintPotentiometer();
  checkSwitchState();
  handleSerialCommands();
  delay(5000); // Short delay to avoid flooding the terminal
}
