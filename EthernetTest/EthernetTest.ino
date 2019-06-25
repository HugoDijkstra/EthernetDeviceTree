#include "networkdevice.h"

void TestCommand(String args)
{
  Serial.println("TEST TEST: " + args);
}

void SetLed(String args)
{

}

NetworkDevice device;
String a = "TestCommand";
String b = "2";
void setup() {
  Serial.begin(9600);
  device.RegisterCommand(a, &TestCommand, b);
  device.RegisterCommand("SetLed", &SetLed, "1");
  device.setup("AABBCCDD11223344", "HomeArduino", "A Testing Arduino");
}

void loop() {
  device.loop("AABBCCDD11223344", "HomeArduino", "A Testing Arduino");
}
