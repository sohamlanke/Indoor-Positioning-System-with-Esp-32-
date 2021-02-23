#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>

const int PIN = 2;
const int CUTOFF = -72;
BLEAdvertising *pAdvertising;

void setup() {
  pinMode(PIN, OUTPUT);
  BLEDevice::init("Band 1");
  Serial.begin(115200);
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->start();
}

void loop() {
  BLEScan *scan = BLEDevice::getScan();
  scan->setActiveScan(true);
  BLEScanResults results = scan->start(1);
  int target_rssi = CUTOFF-10;
  Serial.println("-----------------------------");
  for (int i = 0; i < results.getCount(); i++) {
    BLEAdvertisedDevice device = results.getDevice(i);
    int rssi = device.getRSSI();
    BLEAddress addd = device.getAddress();
    Serial.printf("%s : ",addd.toString().c_str());
    Serial.printf("%s : ",device.getName().c_str());
//    Serial.printf("Advertised Device: %s :", device.toString().c_str());
    Serial.println(rssi);
    if (device.getName() == "Band 1" || device.getName() == "Band 2" || addd.toString() == "84:cc:a8:5e:a5:aa") {
      target_rssi = rssi;
    }
  }
  Serial.println(target_rssi);
  Serial.println("-----------------------------");
  digitalWrite(PIN, target_rssi < CUTOFF ? LOW : HIGH);
}
