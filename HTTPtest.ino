#include "WiFiS3.h"
#include "WiFiSSLClient.h"
#include <ArduinoHttpClient.h>
#include "arduino_secrets.h"
#include <string>

int    HTTP_PORT   = 3000; // 443 port for HTTPS/secure hosts
String HTTP_METHOD = "GET"; // or "POST"
char   HOST_NAME[] = "172.20.10.2"; // hostname of web server:
String PATH_NAME   = "/latest";
String queryString = "?value1=26&value2=70";

char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;

/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-http-request
 */


// WiFiSSLClient client;
WiFiClient client; // Temp for testing (as testing will be with http -- not https)
HttpClient http(client, "172.20.12.2", 3000);

int status = WL_IDLE_STATUS;

#define SENSOR_PIN A0  // Analog pin for sensor
int sensorValue = 0;


void setup() {
  Serial.begin(9600);
  Serial.print("Start");
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass); // use for personal
    //status = WiFi.begin(ssid); // guest wifi

    // wait 10 seconds for connection:
    delay(10000);
  }

  // print your board's IP address:
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // connect to web server on port 443:
  if (client.connect(HOST_NAME, HTTP_PORT)) {
    Serial.println("Connected to server");


    client.print("GET /moisture?value=");
    client.print("1");
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.print(String(HOST_NAME));
    client.print("\r\n");


    client.print("Connection: close\r\n");
    client.print("\r\n");

    while (client.connected() || client.available()) {
      if (client.available()) {
        // read an incoming byte from the server and print it to serial monitor:
        char c = client.read();
        Serial.print(c);
      }
    }

    // the server's disconnected, stop the client:
    client.stop();
    Serial.println();
    Serial.println("disconnected");
  } else {  // if not connected:
    Serial.println("connection failed");
  }
  
}

void loop() {
  
  
  // Variable to store ADC value ( 0 to 1023 )
  sensorValue = analogRead(SENSOR_PIN);

  // Converts value to percentage (approximation)
  int moisturePercent = map(sensorValue, 1023, 200, 0, 100);
  String moistureLevel = "Moisture Level: " + String(moisturePercent) + "%";
  Serial.println(moistureLevel);

  if (client.connect(HOST_NAME, HTTP_PORT)) {
    Serial.println("Connected to server");


    client.print("GET /moisture?value=");
    client.print(moisturePercent);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.print(String(HOST_NAME));
    client.print("\r\n");


    client.print("Connection: close\r\n");
    client.print("\r\n");

    unsigned long timeout = millis();
    while (client.connected() && millis() - timeout < 500) {  // wait max 500ms
      while (client.available()) {
        client.read();  // just read and discard
      }
    }

    // the server's disconnected, stop the client:
    client.stop();
    Serial.println();
    Serial.println("disconnected");
  } else {  // if not connected:
    Serial.println("connection failed");
  }


  delay(5000);
  

  
}