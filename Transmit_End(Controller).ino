#include <ArduinoBLE.h>
#include <string>
#include <vector>

// define uuid for service
const char * UuidService = "19b10000-e8f2-537e-4f6c-d104768a1214"; 

// define uuid for 'read' and 'write' characteristic
const char * UuidReadCharacteristic = "19b10001-e8f2-537e-4f6c-d104768a1215";  
const char * UuidWriteCharacteristic = "19b10001-e8f2-537e-4f6c-d104768a1216"; 

//--------- for joystick-----------
// joystick pins
const int LRpin = A0;
const int UDpin = A1;

//set duty, angle
int duty;
int angle;
int rawDuty;
int rawAngle;

// neutral readings for alibration
int LR_neutral;
int UD_neutral;
//--------- for joystick-----------

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // initialize the Bluetooth® Low Energy hardware
  BLE.begin();
  Serial.println("Bluetooth® Low Energy Central - LED control");

  //-------- for getting neutral reading for calibration  ----------------
  //make sure the joystick is not touched when the program starts
  LR_neutral = analogRead(LRpin);
  UD_neutral = analogRead(UDpin);
 //-------- for getting neutral reading for calibration  ----------------
}

void loop() {
  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();

  // start scanning for peripherals
  BLE.scanForUuid(UuidService);
  Serial.println("Scanning for Service...");
  delay(500);

  // A connection trial
  if (peripheral) {
    // discovered a peripheral, print out address, local name, and advertised service
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();

    if (peripheral.localName() != "Rainbow carhhh") {
      Serial.print("incorrect name format");
        Serial.println(peripheral.localName());
      return;
    }  

        // stop scanning
    BLE.stopScan();

    controlLed(peripheral);
  }

}

void controlLed(BLEDevice peripheral) {
  // connect to the peripheral
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

    // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
  }

  // retrieve the characteristics
  BLECharacteristic readCharacteristic = peripheral.characteristic(UuidReadCharacteristic);  
  BLECharacteristic writeCharacteristic = peripheral.characteristic(UuidWriteCharacteristic);


  if (!readCharacteristic.canRead()) {
    Serial.println("Peripheral does not have a readable characteristic!");
    peripheral.disconnect();
    return;
  }
  if (!writeCharacteristic.canWrite()) {
    Serial.println("Peripheral does not have a writable LED characteristic!");
    peripheral.disconnect();
    return;
  }

  while (peripheral.connected()) {
    // while the peripheral is connected

  int LR = analogRead(LRpin);
  int UD = analogRead(UDpin);

  if(UD > UD_neutral*1.1) {
    rawDuty = map(UD,UD_neutral,1023,0,255);
    duty = 1.2*rawDuty;
  }  else if(UD < UD_neutral/1.1) {
    rawDuty = map(UD,0,UD_neutral,-255,0);
    duty = 1.2*rawDuty;
  }  else {
    duty = 0;
  }

  if(LR < LR_neutral/1.1){
    rawAngle = map(LR,0,LR_neutral,30,90);
    angle = rawAngle/1.2;
  }  else if(LR > LR_neutral*1.1){
    rawAngle = map(LR,LR_neutral,1023,90,150);
    angle = 1.2*rawAngle;
  }  else{
    angle = 90;
  }
  

  String dataToSend = String(duty) + "," + String(angle);
  writeCharacteristic.writeValue(dataToSend.c_str());
  Serial.print("Sent: ");
  Serial.println(dataToSend);

  delay(200);
  }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from peripheral: "));
    Serial.println(peripheral.address());
  }
