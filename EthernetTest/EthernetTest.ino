#include "networkdevice.h"

void TestCommand(String args)
{
  Serial.println("TEST TEST: " + args);
  if (args == "True")
  {
    Serial.println("YA BOI");
  }
}

void SetLed(String args)
{

}

NetworkDevice device ;

String a = "TestCommand";
String b = "2";
void setup() {
  Serial.begin(9600);
  device.RegisterCommand(a, &TestCommand, b);
  device.RegisterCommand("SetLed", &SetLed, "1");
  device.Setup("AABBCCDD11223344", "HomeArduino", "A Testing Arduino");
}

void loop() {
  device.Loop();
}
