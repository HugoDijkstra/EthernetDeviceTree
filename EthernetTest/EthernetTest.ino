#include "networkdevice.h"

void TestCommand()
{
  Serial.println("TEST TEST: ");
}

NetworkDevice device;
String a = "TestCommand";
String b = "int,string";
void setup() {
  Serial.begin(9600);
  device.RegisterCommand(a, &TestCommand, b);
  bool done = device.Init("AABBCCDD11223344", "Testing", "A Testing Arduino");
  while (!done)
  {
    done = device.Init("AABBCCDD11223344", "Testing", "A Testing Arduino");
  }
}

void loop() {
  if (!client.connected()) {
    bool done = device.Init("AABBCCDD11223344", "Testing", "A Testing Arduino");
    while (!done)
    {
      done = device.Init("AABBCCDD11223344", "Testing", "A Testing Arduino");
    }
  }
  device.HandleCommand();

}
