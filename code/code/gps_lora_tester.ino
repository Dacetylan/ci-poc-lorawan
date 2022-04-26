// disconnect rx and tx on the arduino nano if you want to program it.
// button is wired to digital pin 6
// gps is wired to rx and tx
// lorawan is wired to digital pin 4 and 5

#include <rn2xx3.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
TinyGPSPlus gps; // need to rename TinyGPS++

SoftwareSerial mySerial(4, 5); // software serial
rn2xx3 myLora(mySerial);       // software serial

union dataUnion {                  //  This union is used to 
    float dataFloat;               //  convert the gps data
    unsigned char dataBytes[4];    //  (which is a float) into
};                                 //  seperate bytes

void setup()
{
  pinMode(6, INPUT);     // button
 
  Serial.begin(9600);
  mySerial.begin(9600);
  
  initialize_radio();   // start lorawan

  Serial.println("Startup");
  delay(2000);
}

void initialize_radio()
{
  //reset rn2483
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  delay(500);
  digitalWrite(12, HIGH);

  delay(100); //wait for the RN2xx3's startup message
  mySerial.flush();

  //Autobaud the rn2483 module to 9600. The default would otherwise be 57600.
  myLora.autobaud();

  //check communication with radio
  String hweui = myLora.hweui();
  while(hweui.length() != 16)
  {
    Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  //print out the HWEUI so that we can register it via ttnctl
  Serial.println("When using OTAA, register this DevEUI: ");
  Serial.println(myLora.hweui());
  Serial.println("RN2xx3 firmware version:");
  Serial.println(myLora.sysver());

  //configure your keys and join the network
  Serial.println("Trying to join TTN");
  bool join_result = false;

  const char *appEui = "0000100000000000";
  const char *appKey = "58376D90237C4402F807E1B395619616";

  join_result = myLora.initOTAA(appEui, appKey);
  while(!join_result)
  {
    Serial.println("Unable to join. Are your keys correct, and do you have TTN coverage?");
    delay(5000);
    join_result = myLora.init();
  }
  Serial.println("Successfully joined TTN");

}

void loop()
{
    while (Serial.available() > 0) {
    if (gps.encode(Serial.read()) && digitalRead(6) == HIGH) {
          byte payload[8];
          
          dataUnion latitude;
          latitude.dataFloat = (float)gps.location.lat();
          dataUnion longitude;
          longitude.dataFloat = (float)gps.location.lng();
          
          memcpy(payload, latitude.dataBytes, sizeof(latitude.dataBytes));
          memcpy(payload+sizeof(latitude.dataBytes), longitude.dataBytes, sizeof(longitude.dataBytes));
          
          Serial.print(latitude.dataFloat, 6);
          Serial.print("  :  ");
          Serial.println(longitude.dataFloat, 6);
          myLora.txBytes(payload, sizeof(payload));
          delay(200);
    }
   }
    if (millis() > 5000 && gps.charsProcessed() < 10) {
      Serial.println(F("No GPS Module Found! Check Hardware!!"));
      while(true);
    }
}
