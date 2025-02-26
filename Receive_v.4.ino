#include <ArduinoBLE.h>
#include <ArduinoMotorCarrier.h>
#include <string>
#include <vector>
#include <Arduino_PMIC.h>

// ---------------------- ↓↓↓↓↓↓↓↓↓ set up for bluetooth ↓↓↓↓↓↓↓↓↓↓ ----------------------------------

// define uuid for service
const char * UuidService = "19b10000-e8f2-537e-4f6c-d104768a1214"; 

// define uuid for 'read' and 'write' characteristic
const char * UuidReadCharacteristic = "19b10001-e8f2-537e-4f6c-d104768a1215";  
const char * UuidWriteCharacteristic = "19b10001-e8f2-537e-4f6c-d104768a1216"; 

// Creates a Service Object and Prepare for Service
BLEService motorControlService(UuidService); 

// define reading and writing characteristics
BLEStringCharacteristic readCharacteristic(UuidReadCharacteristic, BLERead, 20);  
BLEStringCharacteristic writeCharacteristic(UuidWriteCharacteristic, BLEWrite, 20);

// ---------------------- ↑↑↑↑↑↑↑↑↑↑ set up for bluetooth ↑↑↑↑↑↑↑↑↑↑----------------------------------


void setup() {
  Serial.begin(9600);   // Start serial communication at 9600 baud

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!"); 
    while (1);  // stop the whole thing if bluetooth can't connect
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Rainbow carhhh"); 
  BLE.setAdvertisedService(motorControlService);

  // add the characteristic to the service
  motorControlService.addCharacteristic(readCharacteristic);
  motorControlService.addCharacteristic(writeCharacteristic);

  // add service
  BLE.addService(motorControlService);

  // set the initial value for the characeristic:
//  motorControlService.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("Rainbow carhhh");

//-------- for motor ----------------
  //Establishing the communication with the Motor Carrier
  if (controller.begin())
  {
    Serial.print("Motor Carrier connected, firmware version ");
    Serial.println(controller.getFWVersion());
  }
  else
  {
    Serial.println("Couldn't connect! Is the red LED blinking? You may need to update the firmware with FWUpdater sketch");
    while (1);
  }

  // Reboot the motor controller; brings every value back to default
  Serial.println("reboot");
  controller.reboot();
  delay(500);

  int dutyInit = 0; // at 50 it works as expected, at 60 shift sides and is too small duty to move, at 70 is very big duty.
  M1.setDuty(dutyInit);
  M2.setDuty(dutyInit);
  M3.setDuty(dutyInit);
  M4.setDuty(dutyInit);
  Serial.print("Duty init: ");
  Serial.println(dutyInit);
  //  int duty2 = dutyInit * 16777215 / 100;
  //  Serial.print("Conversion formula: ");
  //  Serial.println(duty2);
  //while (1); //WHILE 1!!!!!!!!!!!!!!!!!!!!!!!!!!!! REMOVE!!!!

  //-------- for motor ----------------

}

void loop() {
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central && central.connected()) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral: code for the car
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:

      if (writeCharacteristic.written()) {
        String receivedValue = writeCharacteristic.value();

        // Use sscanf to parse the speed and angle directly from the string
        int duty, angle;
        if (sscanf(receivedValue.c_str(), "%d,%d", &duty, &angle) == 2) {
          // Successfully parsed two integers
          Serial.print("*  Received Speed: ");
          Serial.print(duty);
          M3.setDuty(duty);

          Serial.print(" , Received Angle: ");
          Serial.print(angle);
          Serial.println("  *");
          servo1.setAngle(angle);
          
        }   else {
          Serial.println("Failed to parse values from received data!");
        }
      }
    } 
    // when the central disconnects, print it out:
  }
      Serial.print("Disconnected from central: ");
      Serial.println(central.address());
      delay(500);
  
}