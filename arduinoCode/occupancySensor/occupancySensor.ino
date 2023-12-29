// Bluetooth occupancy client V1
// bkp515 12/23/23

// Library includes
#include <Arduino.h> 
#include <ArduinoJson.h> 
#include <WiFi.h> 
#include <WiFiMulti.h>
#include <HTTPClient.h> 
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <SPIFFS.h>

// Setup bluetooth scanning
int scanTime = 60; //In seconds
BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    }
};

WiFiMulti wifiMulti;

const char* targetServer = "http://192.168.68.115";

int sendToServer(String httpRequestData) {
  if(WiFi.status() == WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;

    http.begin(targetServer); // define path to server 
    http.addHeader("Content-Type", "application/json"); // Define the type of file being sent
    return http.POST(httpRequestData);
  }
}

void setup() {
  // Setup status led and turn it off (Active low LED)
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);

  // Initilize filesystem
  while(!SPIFFS.begin(true));

  // Use smart config to get wifi credentials
  Serial.begin(115200);

  //Init WiFi as Station, start SmartConfig
  WiFi.mode(WIFI_AP_STA);
  WiFi.beginSmartConfig();

  //Wait for SmartConfig packet from mobile
  Serial.println("Waiting for SmartConfig.");
  while (!WiFi.smartConfigDone()) {
    delay(500);
    Serial.print(".");
    digitalWrite(8, LOW); // Turn on the waiting light
  }

  digitalWrite(8, HIGH);
  Serial.println("");
  Serial.println("SmartConfig received.");

  //Wait for WiFi to connect to AP
  Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(8, LOW);
    delay(500);
    digitalWrite(8, HIGH);
    Serial.print(".");
  }

  Serial.println("WiFi Connected.");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  // Smart config complete

  // Setup bluetooth scanning parameters
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(false); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value

  

}

void loop() {
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  File file = SPIFFS.open("/spiffs/data.jsonl", "w");
  file.close();
  for (uint16_t ii = 0; ii < foundDevices.getCount(); ii++){ // Loop through all the devices that were found
    BLEAdvertisedDevice device = foundDevices.getDevice(ii); // Load the devices one by one
    StaticJsonDocument<256> doc; // Create a small JSON file to add all the new device data 
    doc["name"] = device.getName().c_str(); // Collect all the data about the device and attach it to the definition in the json
    doc["address"] = device.getAddress().toString();
    doc["manufacturer"] = device.getManufacturerData().data();
    doc["serviceUUID"] = device.getServiceDataUUID().toString();
    doc["txPower"] = device.getTXPower();
    doc["rssi"] = device.getRSSI();

    File file = SPIFFS.open("/spiffs/data.jsonl", "a"); // Open the actual data file in append mode to save memory
    serializeJson(doc, file); // put all the data collected into the file
    file.println();
    file.close();
  }
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory

  // Open the json file and print to terminal to show that data has been properly logged.
  File file1 = SPIFFS.open("/spiffs/data.jsonl");
  Serial.println("");
  Serial.println(sendToServer(file1.readString()));
  file1.close();

  
  
  SPIFFS.remove("/spiffs/data.jsonl"); // Delete the data from the last scan
  delay(2000);

}
