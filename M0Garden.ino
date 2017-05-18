/*
g-m0

eb.5.18.17
	*/

#include <SPI.h>
#include <WiFi101.h>
#include <PubSubClient.h>
//#include "Adafruit_MQTT.h"
//#include "Adafruit_MQTT_Client.h"
#include "DHT.h"

// WiFi
char ssid[] = "Lil Wayne's World";     //  your network SSID (name)
char pass[] = "trukfit69";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

#define MQTT_DEBUG

// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "e_bitikofer"
#define AIO_KEY         "219b4afd0a80433198b614dfdcb8c521"

// DHT 11 sensor
#define DHTPIN 12
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE, 15);

// Photocell sensor
#define LPIN A0

// Soil Moisture sensor
#define MPIN A1

// Functions
void connect();
void callback(char* topic, byte* payload, unsigned int length);

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
PubSubClient mqttclient(AIO_SERVER, AIO_SERVERPORT, callback, client);

// Store the MQTT server, client ID, username, and password in flash memory.
const char MQTT_SERVER[] = AIO_SERVER;

const char MQTT_CLIENTID[] = AIO_KEY __DATE__ __TIME__;
//const char MQTT_PORT[] = AIO_SERVERPORT;
const char MQTT_USERNAME[] = AIO_USERNAME;
const char MQTT_PASSWORD[] = AIO_KEY;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
//Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_CLIENTID, MQTT_USERNAME, MQTT_PASSWORD);

/****************************** Feeds ***************************************/
#define TEMPERATURE_PUBLISH_PATH "e_bitikofer/feeds/temperature"
#define HUMIDITY_PUBLISH_PATH "e_bitikofer/feeds/humidity"
#define LIGHT_PUBLISH_PATH "e_bitikofer/feeds/light"
#define MOISTURE_PUBLISH_PATH "e_bitikofer/feeds/light"

//const char *T = MQTT_USERNAME+TEMPERATURE_PUBLISH_PATH;
//const char *H = MQTT_USERNAME+HUMIDITY_PUBLISH_PATH;
//const char *L = MQTT_USERNAME+LIGHT_PUBLISH_PATH;
//const char *M = MQTT_USERNAME+MOISTURE_PUBLISH_PATH;

// Setup feeds for temperature & humidity
//const char TEMPERATURE_FEED[] = AIO_USERNAME "/feeds/temperature";
//Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, TEMPERATURE_FEED);

//const char HUMIDITY_FEED[] = AIO_USERNAME "/feeds/humidity";
//Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, HUMIDITY_FEED);

//const char LIGHT_FEED[] = AIO_USERNAME "/feeds/light";
//Adafruit_MQTT_Publish light = Adafruit_MQTT_Publish(&mqtt, LIGHT_FEED);

//const char MOISTURE_FEED[] = AIO_USERNAME "/feeds/moisture";
//Adafruit_MQTT_Publish moisture = Adafruit_MQTT_Publish(&mqtt, MOISTURE_FEED);

void setup() {
  
  WiFi.setPins(8,7,4,2);

  // Init sensor
  dht.begin();
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
//  printCurrentNet();
//  printWifiData();

}

void loop() {

  if(! client.connected())
      connect();
  
  // Grab the current state of the sensor
  uint32_t humidity_data = (int)dht.readHumidity();
  uint32_t temperature_data = (int)dht.readTemperature();
  uint32_t light_data = analogRead(A0);
  uint32_t moisture_data = analogRead(A1);

  Serial.println(humidity_data);
  Serial.println(temperature_data);
  Serial.println(light_data);
  Serial.println(moisture_data);

  // Publish data
  if (! mqttclient.publish(TEMPERATURE_PUBLISH_PATH, (char*) temperature_data))
    Serial.println(F("Failed to publish temperature"));
  else
    Serial.println(F("Temperature published!"));

  if (! mqttclient.publish(HUMIDITY_PUBLISH_PATH, (char*) humidity_data))
    Serial.println(F("Failed to publish humidity"));
  else
    Serial.println(F("Humidity published!"));

  if (! mqttclient.publish(LIGHT_PUBLISH_PATH, (char*) light_data))
    Serial.println(F("Failed to publish light"));
  else
    Serial.println(F("Light published!"));

  if (! mqttclient.publish(MOISTURE_PUBLISH_PATH, (char*) moisture_data))
    Serial.println(F("Failed to publish moisture"));
  else
    Serial.println(F("Moisture published!"));

  // Repeat every 10 seconds
  delay(10000);

  mqttclient.loop();
}

void printWifiData() {
  // ping adafruit io a few times to make sure we remain connected
//  if(! mqtt.ping(3)) {
    // reconnect to adafruit io
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[5], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.println(bssid[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void connect() {

  Serial.println(F("Connecting to Adafruit IO... "));

  int8_t ret;

  while ((ret = client.connect(MQTT_USERNAME, 1883)) != 0) {

    switch (ret) {
      case 1: Serial.println(F("Wrong protocol")); break;
      case 2: Serial.println(F("ID rejected")); break;
      case 3: Serial.println(F("Server unavail")); break;
      case 4: Serial.println(F("Bad user/pass")); break;
      case 5: Serial.println(F("Not authed")); break;
      case 6: Serial.println(F("Failed to subscribe")); break;
      default: Serial.println(F("Connection failed")); break;
    }

    if(ret >= 0)
      mqttclient.disconnect();

    Serial.println(F("Retrying connection..."));
    delay(5000);

  }

  char client_name[30];

  // generate new client name
  sprintf(client_name, "adafruit-cc3000-%ul", micros());

  // attempt connection
  if (mqttclient.connect(client_name, AIO_KEY, NULL)) {
    Serial.println(F("Connected to AIO"));
  } else {
    Serial.println(F("AIO connect failed"));
  }

}

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

