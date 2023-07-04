#include <WiFi.h>
#include <PubSubClient.h>
#include "driver/gpio.h"

int relay = 26;
#define SSID "Mi Phone"
#define WIFIPASSWORD "12345678"
unsigned long interval = 30000;
unsigned long previousMillis = 0;
int mqttinit = 0;

const char* mqtt_server = "sand14.duckdns.org";
WiFiClient espClient;
PubSubClient client(espClient);
struct credentials
{
  const char* username = "sand14";
  const char* password = "baccara1";
} credentials;

void setState(String message){ //publishing the current state of the relay
  if(message == "1"){
    digitalWrite(relay, LOW);
    client.publish("/house/esprelaystate/", "1");
    }
  else if(message =="0"){
    digitalWrite(relay, HIGH);
    client.publish("/house/esprelaystate/", "0");
    }
}
void callback(char* topic, byte* payload, unsigned int length) { //received data manipulation

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message: ");

  String message;
  for (int i = 0; i < length; i++) {
    message = message + (char)payload[i];
  }
  Serial.print(message);
  Serial.println();
  setState(message);
}

void Wifi_connected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("ESP32 WIFI Connected to Access Point");
}

void Get_IPAddress(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WIFI Connected!");
  Serial.println("IP address of Connected WIFI: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server,1883); //setting mqtt server dns and port
  client.setCallback(callback); //setting 

  if(client.connect("ESPRelay",credentials.username,credentials.password)){ //connecting to mqtt 
    Serial.println("Relay - MQTT - OK");
    client.subscribe("/house/esprelay/"); // subscribing to the mqtt topic
    digitalWrite(LED_BUILTIN, HIGH);
  }else{
    Serial.println("Relay - MQTT - ERROR");
  }
}

void Wifi_disconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Disconnected from WIFI");
  Serial.print("Connection Lost ");
  Serial.println("Reconnecting...");
  WiFi.begin(SSID, WIFIPASSWORD);
}

void setup() {
  Serial.begin(115200);
  WiFi.disconnect(true);
  delay(1000);
  digitalWrite(relay, HIGH);
  pinMode(relay,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  WiFi.onEvent(Wifi_connected,ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(Get_IPAddress, ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(Wifi_disconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.begin(SSID,WIFIPASSWORD); // connecting to wifi
}

void loop() {
  client.loop();
}