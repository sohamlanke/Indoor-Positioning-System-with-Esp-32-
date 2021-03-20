#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <FirebaseESP32.h>
#include "time.h"
#include "DHT.h"
///////////////////////////////SETUP//////////////////////////////
#define DHTPIN 13 //Temmperature sensor
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

const int PIN = 2;
const int CUTOFF = -60;
const int TouchPin = 15;
int TouchState = 0;
BLEAdvertising *pAdvertising;

const char* ssid = "SO]-[AM";
const char* password =  "Mnis081!";

#define FIREBASE_HOST "https://deepblue-ab539-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "8QR15otxy6bZG0qn1KdXbh5W4RqM0BYaI1tmtM0Y"

FirebaseData firebaseData;
FirebaseJson json;
FirebaseJson bjson, tjson;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;
int detect_flag = 0;

String Band1Mac = "84:cc:a8:5f:87:5a";
String Band2Mac = "84:cc:a8:5e:a5:aa";

///////////////////////////////SETUP END//////////////////////////////

void update_touch(){
//   buttonState = digitalRead(buttonPin);
Serial.println("Entered Touch Fn");
  TouchState = digitalRead(TouchPin);
  Serial.println(TouchState);
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH
   if (TouchState == 1) {
    Serial.println("Touch = 1");
    json.set("/isband", "true"); //For Band 1
    Firebase.pushJSON(firebaseData, "/Alert/BandDetection/"+Band1Mac, json);
//    Firebase.updateNode(firebaseData,"/Alert/BandDetection/"+Band1Mac,json);
  } else {
    Serial.println("Touch = 0");
    json.set("/isband", "false"); //For Band 1
    Firebase.pushJSON(firebaseData, "/Alert/BandDetection/"+Band1Mac, json);
//    Firebase.updateNode(firebaseData,"/Alert/BandDetection/"+Band1Mac,json);
  }
}

void update_temp(){
  Serial.println("Entered Temp Fn");
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
//  float t = dht.readTemperature();
//  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  
  float hif = dht.computeHeatIndex(f, h);
  tjson.set("/Temp", 93.5); //For Band 1
  Firebase.updateNode(firebaseData,"/Temperature/"+Band1Mac,tjson);
  Serial.println("Temp =");
  Serial.print(hif);
  
  }
void update_battery()
{
    //read battery voltage per %
    Serial.println("Entered Battery Fn");
    const int BatteryPin = 36;
    long sum = 0;                  // sum of samples taken
    float voltage = 0.0;           // calculated voltage
    float output = 0.0;            //output value
    const float battery_max = 4.5; //maximum voltage of battery
    const float battery_min = 3.3; //minimum voltage of battery before shutdown

//    float R1 = 100000.0; // resistance of R1 (100K)
//    float R2 = 10000.0;  // resistance of R2 (10K)

    float R1 = 0.0; // resistance of R1 (100K)
    float R2 = 0.0;  // resistance of R2 (10K)

    for (int i = 0; i < 500; i++)
    {
        sum += analogRead(BatteryPin);
        delayMicroseconds(1000);
    }
    // calculate the voltage
    voltage = sum / (float)500;
    voltage = voltage / 930.68;
    voltage = 3.7;
//    voltage = (voltage * 1.1) / 4096.0; //for internal 1.1v reference
    // use if added divider circuit
    // voltage = voltage / (R2/(R1+R2));
    //round value by two precision
//    voltage = roundf(voltage * 100) / 100;
    Serial.print("voltage: ");
    Serial.println(voltage, 2);
    
    output = ((voltage - battery_min) / (battery_max - battery_min)) * 100;
    if (output < 100){
      if(output<0){
        output = 0;
        }
      bjson.set("/Percentage", output); //For Band 1
      Firebase.updateNode(firebaseData,"/Battery/"+Band1Mac,bjson);
      }
    else{
      bjson.set("/Percentage", 100); //For Band 1
      Firebase.updateNode(firebaseData,"/Battery/"+Band1Mac,bjson);
      }
}

void printLocalTime()
{
  Serial.println("Current time: ");
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
////////////////////////////////////Function END/////////////////////
void setup() {
  pinMode(PIN, OUTPUT);
  pinMode(TouchPin, INPUT);
  BLEDevice::init("Band 1"); //Band Name
  Serial.begin(115200);
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->start();

  WiFi.begin("SO]-[AM", "Mnis081!");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
  digitalWrite(PIN, LOW);

  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());
  
//--------------Firebase-------------------------
 Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
//    delay(1000);
//     update_battery();
//     delay(1000);
//     update_temp();
//     delay(1000);
  
}
//int count = 0;
void loop() {
//  printLocalTime();
//  count += 1;
//  if(count == 2){
    update_touch();
    delay(100);
//    }

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
    if (device.getName() == "Band 1" || device.getName() == "Band 2" || addd.toString() == "84:cc:a8:5e:a5:aa" || addd.toString() == "84:cc:a8:5f:87:5a") {
      target_rssi = rssi;
    }
  }
  Serial.println(target_rssi);
  Serial.println("-----------------------------");
//  digitalWrite(PIN, target_rssi < CUTOFF ? LOW : HIGH);
  if(target_rssi > CUTOFF && detect_flag == 0){
    detect_flag = 1;
    digitalWrite(PIN, HIGH);
    Serial.println("Band 2 detected!");
    struct tm timeinfo;
    if(getLocalTime(&timeinfo)){
      Serial.println("Writing Alert to firebase!");
      char timeStringBuff[50]; 
      strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
      String asString(timeStringBuff);
//      json.set("/bandid", "84:cc:a8:5e:a5:aa");  //For Band 2
      json.set("bandid", "84:cc:a8:5f:87:5a"); //For Band 1
      Firebase.updateNode(firebaseData,"/Alert/socialDistancing/"+asString,json);
      }   
    }
    else{
      if(target_rssi < CUTOFF){
        detect_flag = 0;
        digitalWrite(PIN, LOW);
        }
      }
}
