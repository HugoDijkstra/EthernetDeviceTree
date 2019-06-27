// TODO: make easy inhert class for use
// TODO: document easy inhert class (extra: write pdf documentation?)


#ifndef NETWORKDEVICE
#define NETWORKDEVICE

#include <Ethernet2.h>
#include <SPI.h>

#include <HashMap.h>

EthernetClient client;

class Command;
class ValueRegister;

const byte HASH_SIZE = 30;

class Command
{
  public:
    Command(void* com, String typeString)
    {
      pointer = com;
      types = typeString;
    }
    void* pointer;
    String types;
};

class ValueRegister
{
  public:
    ValueRegister(void* val, int type)
    {
      object = val;
      valueType = type;
    }
    void* object;
    int valueType;
};

HashType<String*, Command*>FunctionCommands[HASH_SIZE];
HashType<String*, ValueRegister*>valueRegisters[HASH_SIZE];
HashMap<String*, ValueRegister*> valueHash = HashMap<String*, ValueRegister*>(valueRegisters, HASH_SIZE);
HashMap<String*, Command*> hashMap = HashMap<String*, Command*>(FunctionCommands, HASH_SIZE);
class NetworkDevice
{
  public:
    byte mac[6] = { 0xA8, 0x61, 0x0A, 0xAE, 0x01, 0x45 };
    byte server[4] = { 192, 168, 1, 3 };
    byte ip[4] = { 192, 168, 1, 13 };
    String* authKey;
    String* name;
    String* description;

    bool Init(String authKey, String name, String description)
    {
      pinMode(6, OUTPUT);
      digitalWrite(6, LOW);
      // put your setup code here, to run once:
      Serial.println("Initialize Ethernet:");
      Ethernet.begin(ip, mac);
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
        for (int i = 0; i < commandAmount; i++)
        {
          SendRegisterCommand((*hashMap[i].getHash()), hashMap[i].getValue());
        }
        for (int i = 0; i < valueAmount; i++)
        {
          SendRegisterRequest((*valueHash[i].getHash()), valueHash[i].getValue());
        }
        SendMessageToServer("COMPLETE");
      }
      else
      {
        return false;
      }
      return true;
    }

    void Setup(String authKey, String name, String description)
    {
      bool done = Init(authKey, name, description);
      while (!done)
      {
        done = Init(authKey, name, description);
      }
      this->authKey = new String(authKey);
      this->name = new String(name);
      this->description = new String(description);
    }

    void Loop()
    {
      if (!client.connected()) {
        bool done = Init((*authKey), (*name), (*description));
        while (!done)
        {
          done = Init((*authKey), (*name), (*description));
        }
      }
      HandleCommand();
    }

    //INT=0,FLOAT=1,BOOL=2,STRING=3
    void RegisterCommand(String name, void* command, String types)
    {
      Command* com = new Command(command, types);
      String *h = new String(name);
      hashMap[commandAmount](h, com);
      commandAmount++;
    }

    void SendMessageToServer(String str) {
      if (client.connected())
      {
        Serial.println(str);
        client.println(str);
      }
    }

    //INT=0,FLOAT=1,BOOL=2,STRING=3
    void RegisterRequest(String name, void* valuePointer, int typeN)
    {
      ValueRegister* reg = new ValueRegister(valuePointer, typeN);
      String *h = new String(name);
      valueHash[valueAmount](h, reg);
      valueAmount++;
    }

    void HandleCommand()
    {
      if (client.connected())
      {
        String command = "";
        int cs = client.available();
        if (cs == 0)
          return;
        while (cs)
        {
          char c = (char)client.read();
          command += c;
          cs--;
        }
        String subCommand = command.substring(0, 3);
        if (subCommand == "COM")
        {
          String CommandString = command.substring(4);
          String functionName = CommandString.substring(0, CommandString.indexOf(';'));
          String arguments = CommandString.substring(CommandString.indexOf(';') + 1);
          Serial.println(functionName);
          for (int i = 0; i < commandAmount; i++)
          {
            Serial.println((*hashMap[i].getHash()));
            if (functionName != (*hashMap[i].getHash()))
              continue;
            void(*f)(String f) = hashMap[i].getValue()->pointer;
            f(arguments);
            return;
          }
        }
        else if (subCommand == "REQ")
        {
          for (int i = 0; i < valueAmount; i++)
          {
            if (command.substring(4) == (*valueHash[i].getHash())) {
              ValueRegister send = (*valueHash[i].getValue());

              String toSend = "RET:";
              //INT=0,FLOAT=1,BOOL=2,STRING=3
              switch (send.valueType)
              {
                case 0:
                  toSend += (*(int*)(send.object));
                  break;
                case 1:
                  toSend += (*(float*)(send.object));
                  break;
                case 2:
                  toSend += (*(bool*)(send.object));
                  break;
                case 3:
                  toSend += (*(String*)(send.object));
                  break;
                default :
                  return;
              }
              SendMessageToServer(toSend);
            }
            return;
          }
        }
        else if (subCommand == "INI")
        {

        }
      }
    }
  private:
    int commandAmount = 0;
    int valueAmount = 0;
    void SendRegisterCommand(String Name, Command* command)
    {
      SendMessageToServer("REGCOM;" + Name + ";" + command->types);
    }
    void SendRegisterRequest(String Name, ValueRegister* r)
    {
      SendMessageToServer("REGRET;" + Name + ";" + r->valueType);
    }
};

#endif
