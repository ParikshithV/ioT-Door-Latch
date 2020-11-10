#include <SPI.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define touchPin D0

/********* WiFi Access Point ***********/


#define WLAN_SSID       "TP-LINK_BASE" //Wifi Name
#define WLAN_PASS       "264271556" // Wifi Password


/********* Adafruit.io Setup ***********/


#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "RedRabbit1" // AdaFruit Usernmae
#define AIO_KEY         "aio_EvHB06efAgaILhvBj7MY1HoK0GPu"// AdaFruit Key

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe latch = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/latch");


/********* Servo & touch switch Setup ***********/

int activate =0;
Servo myservo;

void setup() {
 Serial.begin(115200);
 delay(10);
 Servo myservo;
 // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.println("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  }
  Serial.println();
  
  
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  
  
  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&latch);

  
 SPI.begin(); // Init SPI bus
 myservo.attach(5); //attach the servo to pin 5
 myservo.write(0); // 0 degree angle for servo
 myservo.detach();
 pinMode(A0, INPUT);//Touch switch

 }
  
  
  uint32_t x=0;

void loop() {

  MQTT_connect();

    Adafruit_MQTT_Subscribe *subscription;
    while ((subscription = mqtt.readSubscription(2000))) {
    if (subscription == &latch) {
      Serial.print(F("\nGot: "));
      char * temp = (char *)latch.lastread;
      Serial.println(temp);
      Serial.println("latch trigger ...");
      if (activate==0){
        myservo.attach(5);
        myservo.write(90); // 90 degree angle for servo (Door will open)
        Serial.println("Door is open");
        delay(1000);
        myservo.detach();
        activate = activate+1;
      }
      else
      {
        myservo.attach(5);
        myservo.write(0); // 0 degree angle for servo (Door will close)
        activate = 0;
        Serial.println("Door is close");
        delay(1000);
        myservo.detach();
      }
    }
  }
}


void MQTT_connect() {
  int8_t ret;
  
  
  // Stop if already connected.
  if (mqtt.connected()) {
    //Serial.println("wifi on");
    return;
  }
  
  
  Serial.println("Connecting to MQTT... ");
  
  
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}

//void printDec(byte *buffer, byte bufferSize) {
// for (byte i = 0; i < bufferSize; i++) {
// Serial.print(buffer[i] < 0x10 ? " 0" : " ");
// Serial.print(buffer[i], DEC);
// }
//}
