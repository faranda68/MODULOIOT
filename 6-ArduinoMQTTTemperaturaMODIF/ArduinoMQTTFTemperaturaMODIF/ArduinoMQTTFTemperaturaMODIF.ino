/*
 Basic MQTT example

 This sketch demonstrates the basic capabilities of the library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 
*/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//byte mac[]    = {  0xD8, 0xA7, 0x56, 0x31, 0x06, 0xD0 };
IPAddress ip(192,168,8,20);
IPAddress server(192,168,8,3);

//Sensor
const int sensorPin= A0;
char lm35dz;
long now;
long lastMsg =0;
int value = analogRead(sensorPin);
float millivolts = (value / 1023.0) * 5000;
float celsius = millivolts / 10; 
  
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  if(topic=="felix_rele"){
    if(payload[0]==1){
      digitalWrite(4, HIGH);
    }else{
       digitalWrite(4, LOW);
    }
  }
  Serial.println();
}

EthernetClient ethClient;
PubSubClient client(ethClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("felix_temp","Proba arduino esta publicant");
      // ... and resubscribe
      client.subscribe("felix_rele");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}





void setup()
{
  Serial.begin(57600);

  client.setServer(server, 1883);
  client.setCallback(callback);

  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);
}

void loop()
{
  int Value;         // variable que almacena el valor raw (0 a 1023)
  float milivolts, celsius;
  char charTemperature[] = "00.0";
  
 
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  now=millis();
  if ((now-lastMsg)>5000) {
    lastMsg = now;
    
    Value = analogRead(sensorPin);          // realizar la lectura
    milivolts = (Value / 1023.0) * 5000;
    celsius = milivolts / 10;
   
    if (client.connect("arduinoClient")) {
     
     
      dtostrf(celsius, 4, 1, charTemperature);
      client.publish("felix_temp", charTemperature);
    } 
  } 
}
