#include <DHT.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

//WiFi Setup
const char* ssid = "<YOUR SSID>";
const char* pswd = "<YOUR SSID PASSWORD>";

//MQTT Server
const char* mqtt_server = "<MQTT IP Address>";
const int mqtt_port = 1883;

//init value
float humid = 0.0;
float temp = 0.0;
float methane = 0.0;
int threshold = 250;

//timer
long now = millis();
long lastMeasure= 0;

#define DHTPIN D7
#define DHTTYPE DHT11
const int gas_sensor = A0;


DHT dht(DHTPIN, DHTTYPE);
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
  pinMode(gas_sensor, INPUT);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  dht.begin();
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
    humid=dht.readHumidity();
    temp =dht.readTemperature();
    methane = (100.00-((analogRead(gas_sensor)/1023.00)*100.00));

    if(isnan(humid)||isnan(temp)){
      Serial.println("Failed to read sensor");
      return;
    }

    static char temperatureTemp[7];
    dtostrf(temp, 6, 2, temperatureTemp);

    static char humidTemp[7];
    dtostrf(humid, 6, 2, humidTemp);

    static char methaneTemp[7];
    dtostrf(methane, 6, 2, methaneTemp);

    client.publish("/sensor1/temp", temperatureTemp);
    client.publish("/sensor1/humid", humidTemp);
    client.publish("/sensor1/methane", methaneTemp);
    
    Serial.print("Temperature : ");
    Serial.print(temp);
    Serial.println();
    Serial.print("Humidity : ");
    Serial.println(humid);
    Serial.print("Methane: ");
    Serial.println(methane);
    delay(60000);
   }
}
