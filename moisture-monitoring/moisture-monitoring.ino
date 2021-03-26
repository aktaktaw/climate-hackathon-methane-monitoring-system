#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

//WiFi Setup
const char* ssid = "<YOUR SSID>";
const char* pswd = "<YOUR SSID PASSWORD>";

const char* mqtt_server = "<MQTT IP Address>";
const int mqtt_port = 1883;

float moisture = 0.0;
const int soil_sensor = A0;

long now = millis();
long lastMeasure = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi(){
  delay(10);
  WiFi.begin(ssid, pswd);
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(ssid);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnect(){
  Serial.print("Connecting to MQTT Server...");
  if(client.connect("ESP8266Client")){
    Serial.println("MQTT Connected");
  }
  else{
    Serial.print("MQTT Connection failed, rc=");
    Serial.println(client.state());
    Serial.println("Reconnect in 5s");
    delay(5000);
  }
}


void setup() {
  Serial.begin(115200);
  Serial.setTimeout(500);
  pinMode(soil_sensor, INPUT);
  setup_wifi();
  
}

void loop() {
  if(!client.connected()){
    reconnect();
  }

  if(!client.loop()){
    client.connect("ESPClient8266");
  }

  now = millis();
  if(now - lastMeasure > 10000){
    moisture = (100.00-((analogRead(soil_sensor)/1023.00)*100.00));

    if(isnan(moisture)){
      Serial.println("Failed to read sensor");
      return;
    }

    static char moistureTemp[7];
    dtostrf(moisture, 6, 2, moistureTemp);

    client.publish("/sensor1/moisture",moistureTemp);
    Serial.print("Moisture:");
    Serial.println(moisture);
    delay(60000);
  }
}
