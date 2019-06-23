#include <Ethernet.h>
#include <SPI.h>

#include <HashMap.h>

EthernetClient client;

typedef void (*FunctionPointer)();
class Command;
const byte HASH_SIZE = 100;
HashType<String*, Command*>FunctionCommands[HASH_SIZE];
HashMap<String*, Command*> hashMap = HashMap<String*, Command*>(FunctionCommands, HASH_SIZE);

class Command
{
  public:
    Command(FunctionPointer* com, String typeString)
    {
      pointer = com;
      types = typeString;
    }
    FunctionPointer* pointer;
    String types;
};

class NetworkDevice
{
  public:
    byte mac[6] = {0xDD, 0xFF, 0xBB, 0xEF, 0xFE, 0xED};
    byte server[4] = { 192, 168, 1, 3 };
    byte ip[4] = { 192, 168, 1, 4 };
    bool Init(String authKey, String name, String description)
    {
      // put your setup code here, to run once:
      Serial.println("Initialize Ethernet with DHCP:");
      Ethernet.begin(mac, ip);
      //if (Ethernet.begin(mac,ip) == 0) {
      //  Serial.println("Failed to configure Ethernet using DHCP");
      //   if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      //     Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      //   } else if (Ethernet.linkStatus() == LinkOFF) {
      //    Serial.println("Ethernet cable is not connected.");
      //  }
      //  return false;
      //}
      // print your local IP address:
      Serial.print("My IP address: ");
      Serial.println(Ethernet.localIP());
      Serial.println("Starting Connection");
      if (client.connect(server, 11000))
      {
        Serial.println("Connected");
        SendMessageToServer("AUTHDEV;" + authKey + ";" + name + ";" + description);
        int cs = client.available();
        while (cs == 0)
        {
          delay(1);
          cs = client.available();
        }
        delay(1000);
        for (int i = 0; i < commandAmount; i++)
        {
          SendRegisterCommand((*hashMap[i].getHash()), hashMap[i].getValue());
        }
      }
      else
      {
        return false;
      }
      return true;
    }

    //INT=0,FLOAT=1,BOOL=2,STRING=3
    void RegisterCommand(String name, FunctionPointer* command, String types)
    {
      Command* com = new Command(command, types);
      String *h = new String(name);
      hashMap[commandAmount](h, com);
      commandAmount++;
      // SendRegisterCommand(name, com);
    }

    void SendMessageToServer(String str) {
      if (client.connected())
      {
        Serial.println(str);
        client.println(str);
      }
    }

    void HandleCommand()
    {
      if (client.connected())
      {
        String command = "";
        int cs = client.available();
        if (cs == 0)
          return;
        Serial.println(cs);
        while (cs)
        {
          char c = (char)client.read();
          command += c;
          cs--;
          Serial.print(c);
        }
        String subCommand = command.substring(0, 3);
        if (subCommand == "REG")
        {

        }
        else if (subCommand == "COM")
        {

          String CommandString = command.substring(4);
          for (int i = 0; i < commandAmount; i++)
          {
            if ((*hashMap[i].getHash()) == "TestCommand")
            {
              Serial.println("GOT IT");
              FunctionPointer* f = hashMap[i].getValue()->pointer;
              (*f)();
              Serial.println();
              return;
            }
            Serial.println("\nNot");
          }
        }
        else {
          Serial.println("NO FITTING COMMAND");
        }
      }
    }
  private:
    int commandAmount = 0;
    void SendRegisterCommand(String Name, Command* command)
    {
      SendMessageToServer("REGCOM;" + Name + ";" + command->types);
    }

};

void TestCommand()
{
  Serial.println("TEST TEST: ");
}

NetworkDevice device;
String a = "TestCommand";
String b = "int,string";
void setup() {
  Serial.begin(9600);
  void (*test)();
  test = &TestCommand;
  test();
  device.RegisterCommand(a, (FunctionPointer*)&TestCommand, b);
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
