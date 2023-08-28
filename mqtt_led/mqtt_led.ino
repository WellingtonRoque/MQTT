#include <PubSubClient.h>
#include <WiFi.h>

const char* ssid = "WAM 2.4";
const char* password ="manu2020";
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";
int led = 14;
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
    pinMode (led, OUTPUT);

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
}