
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Manh Tien";
const char* password = "12041996";
const char* mqtt_server = "mqtt.eclipse.org";
static int mqtt_port = 1883;
const char* Device_client_ID_01 = "ESPdemo01";

//Pulse sensor
int adc_value = A0;
int outputValue = 0;
int count = 0;
//const char bpm;
char bpm_pt;


unsigned long previousMillis = 0;     
const long interval = 10000;  


WiFiClient WIFI_CLIENT;
PubSubClient MQTT_CLIENT;

#define LIGHT_SENSOR A0

void setup() {
  // Initialize the serial port
  Serial.begin(115200);

  // Attempt to connect to a specific access point
  WiFi.begin(ssid, password);

  // Keep checking the connection status until it is connected
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
  }

  // Print the IP address of your module
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void myMessageArrived(char* topic, byte* payload, unsigned int length) {
  // Convert the message payload from bytes to a string
  String message = "";
  for (unsigned int i=0; i< length; i++) {
    message = message + (char)payload[i];
  }
   
  // Print the message to the serial port
  Serial.println(message);
}

void reconnect() {
  // Set our MQTT broker address and port
  MQTT_CLIENT.setServer(mqtt_server, mqtt_port);
  MQTT_CLIENT.setClient(WIFI_CLIENT);

  // Loop until we're reconnected
  while (!MQTT_CLIENT.connected()) {
    // Attempt to connect
    Serial.println("Attempt to connect to MQTT broker");
    MQTT_CLIENT.connect("ESPdemo01");

    // Wait some time to space out connection requests
    delay(3000);
  }

  Serial.println("MQTT connected");

  // Subscribe to the topic where our web page is publishing messages
  MQTT_CLIENT.subscribe("controlsignal/iotDevice/from_web");

  // Set the message received callback
  MQTT_CLIENT.setCallback(myMessageArrived);
}

void readSensor(){
  
  adc_value = analogRead(A0);
  Serial.println(adc_value);
  if (adc_value == 185) {
    count++;
    delay(20);
  }
  bpm_pt = char(count);
  Serial.print("count: ");
  Serial.println(count);
  delay(20);
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
   
    MQTT_CLIENT.publish("deviceStatus/iotDevice/from_esp", &bpm_pt);
    delay(50);
    count = 0;
  }
}

void loop() {
  
  // Check if we're connected to the MQTT broker
  if (!MQTT_CLIENT.connected()) {
    // If we're not, attempt to reconnect
    reconnect();
  }
   
   readSensor();

  MQTT_CLIENT.loop();
}
