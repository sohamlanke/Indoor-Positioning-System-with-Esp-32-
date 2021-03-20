#include <driver/adc.h>
#include <FirebaseESP32.h>
const int BatteryPin = 36;
const char* ssid = "SO]-[AM";
const char* password =  "Mnis081!";

#define FIREBASE_HOST "https://deepblue-ab539-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "8QR15otxy6bZG0qn1KdXbh5W4RqM0BYaI1tmtM0Y"

FirebaseData firebaseData;
FirebaseJson json;

float battery_read()
{
    //read battery voltage per %
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
//    voltage = (voltage * 1.1) / 4096.0; //for internal 1.1v reference
    // use if added divider circuit
    // voltage = voltage / (R2/(R1+R2));
    //round value by two precision
//    voltage = roundf(voltage * 100) / 100;
    Serial.print("voltage: ");
    Serial.println(voltage, 2);

    digitalWrite(2, HIGH);
    json.set("/Voltage", voltage); //For Band 1
    Firebase.updateNode(firebaseData,"/Battery",json);
    digitalWrite(2, LOW);
    
    output = ((voltage - battery_min) / (battery_max - battery_min)) * 100;
    if (output < 100)
        return output;
    else
        return 100.0f;
}

void setup()
{
    pinMode(2, OUTPUT);
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_0db); //set reference voltage to internal
    Serial.begin(9600);
      WiFi.begin("SO]-[AM", "Mnis081!");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");

  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());
    //--------------Firebase-------------------------
 Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
}

void loop()
{
    Serial.print("Battery Level: ");
    Serial.println(battery_read(), 2);
    delay(1000);
    digitalWrite(2, HIGH);
    json.set("/Percentage", battery_read()); //For Band 1
    Firebase.updateNode(firebaseData,"/Battery",json);
    digitalWrite(2, LOW);
}
