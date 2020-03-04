
var dataFromEsp;
var beatpermin;
var rotation;

// Generate a new random MQTT client id on each page load
var MQTT_CLIENT_ID = "iot_web_" + Math.floor((1 + Math.random()) * 0x10000000000).toString(16);

// Create a MQTT client instance
var MQTT_CLIENT = new Paho.MQTT.Client("mqtt.eclipse.org", Number(80), "/mqtt", MQTT_CLIENT_ID);

// Tell the client instance to connect to the MQTT broker
MQTT_CLIENT.connect({ onSuccess: myClientConnected });

// Tell MQTT_CLIENT to call myMessageArrived(message) each time a new message arrives
MQTT_CLIENT.onMessageArrived = myMessageArrived;

// This is the function which handles button clicks
function myButtonWasClicked() {
    // create a new MQTT message with a specific payload
    var mqttMessage = new Paho.MQTT.Message("Hello from website");
    // Set the topic it should be published to
    mqttMessage.destinationName = "controlsignal/iotDevice/from_web";
    // Publish the message
    MQTT_CLIENT.send(mqttMessage);
}

// this is updating device status function
function UdateDeviceStatus() {
    document.getElementById("Destatus").innerHTML = "hello world!";
}

// This is the function which handles subscribing to topics after a connection is made
function myClientConnected() {
    MQTT_CLIENT.subscribe("data/iotDevice/from_esp");
}

// This is the function which handles received messages
function myMessageArrived(message) {
    // Get the payload
    var messageBody = message.payloadString;
    //console.log(messageBody);
    dataFromEsp = messageBody;
    var jsonObj = JSON.parse(dataFromEsp);
    beatpermin = jsonObj.BPM;
    rotation = jsonObj.REV;
    document.getElementById("Destatus").innerHTML = jsonObj.STS;
    //document.getElementById("showJson").innerHTML = dataFromEsp;
};

