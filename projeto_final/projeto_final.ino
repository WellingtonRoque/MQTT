#include <PubSubClient.h>
#include <WiFi.h>
#include "DHT.h"

const char* ssid = "WAM 2.4";
const char* password ="manu2020";
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";
int led = 14;

const char* temperatureTopic = "casa/sensor/temperatura";
const char* humidityTopic = "casa/sensor/umidade";  

WiFiClient espClient;
PubSubClient client(espClient);

#define DHT_PIN 12 // Substitua pelo pino do sensor DHT
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

    

void setup() {
    pinMode (led, OUTPUT);
    dht.begin();

    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
       Serial.print("Connecting to WiFi:");
        Serial.println(ssid);
    }

    Serial.println("Connected to the WiFi network");
    Serial.println("");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);

    while (!client.connected()) {
        Serial.println("Connecting to MQTT…");
        String clientId = "ESP32Client-";
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str(), mqttUser, mqttPassword )) {
            Serial.println("connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }

    Serial.print("Tentando enviar a mensagem");
    //client.publish("casa/sensor/temperatura");
    //client.publish("casa/sensor/umidade");
    client.subscribe("casa/led/controle1");

}

void callback(char* topic, byte* payload, unsigned int length) {

    Serial.print("Message arrived in topic: ");
    Serial.println(topic);

    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }

    if (payload[0] == 'd'){
        Serial.println("Desligando luz");
        digitalWrite(led, LOW);
    }

    if (payload[0] == 'l'){
        Serial.println("Ligando luz");
        digitalWrite(led, HIGH);
    }

    Serial.println();
    //Serial.println(“ — — — — — — — — — — — -”);

}

void loop() {
    client.loop();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  Serial.println(temperature);
  Serial.println(humidity);

  if (!isnan(temperature) && !isnan(humidity)) {
    String tempPayload = String(temperature);
    String humPayload = String(humidity);
    client.publish(temperatureTopic, tempPayload.c_str());
    client.publish(humidityTopic, humPayload.c_str());
  }
  delay(2000);
}