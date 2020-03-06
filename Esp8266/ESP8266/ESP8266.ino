
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <Ticker.h>



const char* ssid = "Manh Tien";
const char* password = "12041996";
const char* mqtt_server = "mqtt.eclipse.org";
static int mqtt_port = 1883;
const char* Device_client_ID_01 = "ESPdemo01";

//Pulse sensor
int adc_value = A0;
int outputValue = 0;
int count = 0;
char bpm;
unsigned long previousMillis = 0;     
const long interval = 10000;
char* sts = "Device Connected";


int rev = 0;

WiFiClient WIFI_CLIENT;
PubSubClient MQTT_CLIENT;
Ticker blinker;

#define LIGHT_SENSOR A0


void ICACHE_RAM_ATTR onTimerISR(){
    handleRoot();
    timer1_write(600000*5000);//12us
}

void setup() {
  // Initialize the serial port
  Serial.begin(9600);

  //Initialize I2C pin
  Wire.begin(D1, D2);

  // Attempt to connect to a specific access point
  WiFi.begin(ssid, password);

  // Keep checking the connection status until it is connected
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
  }
  // Print the IP address of your module
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  TransToAr(1);

  //Initialize Ticker every 0.5s
    timer1_attachInterrupt(onTimerISR);
    timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
    timer1_write(600000*5000); //120000 us

  
}

void myMessageArrived(char* topic, byte* payload, unsigned int length) {
  // Convert the message payload from bytes to a string
  String message = "";
  for (unsigned int i=0; i< length; i++) {
    message = message + (char)payload[i];
  }

  if(message == "1"){
    TransToAr(1);
    sts = "Device Running";
  }
  else if(message == "0"){
    TransToAr(0);
    sts = "Device Stopping";
  }
  else{
    TransToAr(1);
    sts = "Device Running";
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
//  Serial.println(adc_value);
  if (adc_value == 185) {
    count++;
    delay(20);
  }
  bpm = count;
}

void TransToAr(int OnOff){
  Wire.beginTransmission(8); /* begin with device address 8 */
  Wire.write(OnOff);  /* sends hello string */
  Wire.endTransmission();    /* stop transmitting */
}

int RequestFromAr(){
  Wire.requestFrom(8,16); /* request & read data of size 13 from slave */
  int x = Wire.read();
  Serial.println(x);
  delay(1);
  return x;
}

void handleRoot() {  

  char Jbuffer[200];
  
  // Allocate JsonBuffer
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<500> jsonBuffer;

  // Create the root object
  DynamicJsonDocument doc(1024);

  doc["STS"] = sts;
  doc["BPM"] = bpm; //Put Sensor value
  doc["REV"] = rev; //Reads Flash Button Status

  serializeMsgPack(doc, Jbuffer);
  size_t n = serializeJson(doc, Jbuffer);
  MQTT_CLIENT.publish("data/iotDevice/from_esp", Jbuffer);
  count = 0;
  
}

void loop() {
  // Check if we're connected to the MQTT broker
  if (!MQTT_CLIENT.connected()) {
    // If we're not, attempt to reconnect
    reconnect();
  }

  //goc quay
  rev = RequestFromAr();
  readSensor();
  //handleRoot();

  MQTT_CLIENT.loop();
}
