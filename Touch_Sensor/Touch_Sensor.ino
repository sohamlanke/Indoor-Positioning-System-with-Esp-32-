// Complete Instructions: https://RandomNerdTutorials.com/esp32-digital-inputs-outputs-arduino/
#include <FirebaseESP32.h>

const char* ssid = "SO]-[AM";
const char* password =  "Mnis081!";

#define FIREBASE_HOST "https://deepblue-ab539-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "8QR15otxy6bZG0qn1KdXbh5W4RqM0BYaI1tmtM0Y"

FirebaseData firebaseData;
FirebaseJson json;

int PIN = 2;

// set pin numbers
const int buttonPin = 15;  // the number of the pushbutton pin
int TouchState = 0;
String Band1Mac = "84:cc:a8:5f:87:5a";
String Band2Mac = "84:cc:a8:5e:a5:aa";

void setup() {
  Serial.begin(115200);  
  // initialize the pushbutton pin as an input
  pinMode(buttonPin, INPUT);
  pinMode(2, OUTPUT);
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
}

void loop() {
  // read the state of the pushbutton value
  TouchState = digitalRead(buttonPin);
  Serial.println(TouchState);
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH
   if (TouchState == 1) {
    Serial.println("Touch = 1");
    json.set("/isband", "true"); //For Band 1
    Firebase.updateNode(firebaseData,"/Alert/BandDetection/"+Band1Mac,json);
  } else {
    Serial.println("Touch = 0");
    json.set("/isband", "false"); //For Band 1
    Firebase.updateNode(firebaseData,"/Alert/BandDetection/"+Band1Mac,json);
  }
}
