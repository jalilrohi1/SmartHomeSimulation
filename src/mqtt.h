#ifndef MQTT_H
#define MQTT_H

void handleMQTT();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);
void publishMQTT();

#endif // MQTT_H