//ItKindaWorks - Creative Commons 2016
//github.com/ItKindaWorks
//
//Requires PubSubClient found here: https://github.com/knolleary/pubsubclient
//
//ESP8266 Simple MQTT light controller


#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>

#include <DallasTemperature.h>


 // Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 5
//for relay
int in1=13;
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

//EDIT THESE LINES TO MATCH YOUR SETUP
#define MQTT_SERVER "192.168.1.30"
const char* ssid = "eMumba-2";
const char* password = "3Mumba!p@$$";

//LED on ESP8266 GPIO2


char* firstrelay = "/home/r1";


WiFiClient wifiClient;
void callback(char* topic, byte* payload, unsigned int length);


PubSubClient client(MQTT_SERVER, 1883, callback, wifiClient);

void setup() {
  //initialize the light as an output and set to LOW (off)
  //for relay
  pinMode(in1,OUTPUT);
  digitalWrite(in1,HIGH);

  //start the serial line for debugging
  Serial.begin(9600);
  delay(100);

  Serial.println("Dallas Temperature IC Control Library Demo");
  //start wifi subsystem
  WiFi.begin(ssid, password);
  //attempt to connect to the WIFI network and then connect to the MQTT server
  reconnect();

  //wait a bit before starting the main loop
      delay(2000);
}



void loop(){

  //reconnect if connection is lost
  if (!client.connected() && WiFi.status() == 3) {reconnect();}

  //maintain MQTT connection
  client.loop();
  

   sensordata();
   
  //MUST delay to allow ESP8266 WIFI functions to run
  delay(10); 
}

void sensordata(){

    char charBuf[5];
    String msg;

   Serial.println(" Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");

  Serial.println("Temperature is: ");
  Serial.print(sensors.getTempCByIndex(0)); // Why "byIndex"? 
    // You can have more than one IC on the same bus. 
    // 0 refers to the first IC on the wire
   msg =  String(sensors.getTempCByIndex(0), DEC);
  
  msg.toCharArray(charBuf, 5);
  client.publish("/test/ts", charBuf);

  unsigned int AnalogValue;
  Serial.println("light is: ");
  AnalogValue = analogRead(0);
  Serial.print(AnalogValue);
  Serial.println("");
   msg =  String(AnalogValue, DEC);
  
  msg.toCharArray(charBuf, 5);
   
  
  client.publish("/test/light",charBuf);
  delay(3000);
}


void callback(char* topic, byte* payload, unsigned int length) {

  //convert topic to string to make it easier to work with
  String topicStr = topic; 

  //Print out some debugging info
  Serial.println("Callback update.");
  Serial.print("Topic: ");
  Serial.println(topicStr);
  Serial.println("payload: "+payload[0]);
  if(payload[0] == '1'){
    digitalWrite(in1, HIGH);
     client.publish("/test/confirm", "on");
  }
   if(payload[0] == '0'){
    digitalWrite(in1, LOW);
     client.publish("/test/confirm", "off");
  }
  //turn the light on if the payload is '1' and publish to the MQTT server a confirmation message


 

  

}




void reconnect() {

  //attempt to connect to the wifi if connection is lost
  if(WiFi.status() != WL_CONNECTED){
    //debug printing
    Serial.print("Connecting to ");
    Serial.println(ssid);

    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    //print out some more debug once connected
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  //make sure we are connected to WIFI before attemping to reconnect to MQTT
  if(WiFi.status() == WL_CONNECTED){
  // Loop until we're reconnected to the MQTT server
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");

      // Generate client name based on MAC address and last 8 bits of microsecond counter
      String clientName;
      clientName += "esp8266-";
      uint8_t mac[6];
      WiFi.macAddress(mac);
      clientName += macToStr(mac);

      //if connected, subscribe to the topic(s) we want to be notified about
      if (client.connect((char*) clientName.c_str())) {
        Serial.print("\tMTQQ Connected");
        client.subscribe(firstrelay);
      }

      //otherwise print failed for debugging
      else{Serial.println("\tFailed."); abort();}
    }
  }
}

//generate unique name from MAC addr
String macToStr(const uint8_t* mac){

  String result;

  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);

    if (i < 5){
      result += ':';
    }
  }

  return result;
}
