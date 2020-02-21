#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>              //Async MQTT Library
//needed for library
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <Ticker.h>                       //Ticker for running multithread

#define MQTT_HOST "iot-connect.in"        //MQTT Server address
#define MQTT_PORT 1883                    //MQTT Server port
#define MQTT_UNAME "iotconnect"
#define MQTT_PASS "iot-12345"
#define MQTT_MAX_PACKET_SIZE 1024

Ticker TickerForconnectToMqtt;
Ticker TickerForTestMessage;
Ticker TickerForCheckInput;

String chipid = String(ESP.getChipId());
String intopic = chipid+"-in";
String s = "";

AsyncMqttClient mqtt;                     //Variable to initiate MQTT.

void onMqttConnect(bool sessionPresent);
void onMqttSubscribe(uint16_t packetId, uint8_t qos);
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
void send_test_msg();
void checkInput();
void serialDisplay(String icon, String head, String body);
String conv(const char* str);

WiFiClient client;
String mqtt_debug = "MQTT-Test-DEBUG";
String mqtt_output = "garage-x-controller-output";
String mqtt_input = intopic.c_str();

WiFiManager wifiManager;
String action, serialInput;

void configModeCallback (WiFiManager *myWiFiManager) 
{
  serialDisplay("wifi", "Access Point", "Garage X Quiz");
}


void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
  serialDisplay("wifi", "Device ID", intopic.c_str());
  delay(1000);
  //serialDisplay("wifi", "Status", "Connecting");
  delay(1000);
  wifiManager.setDebugOutput(false); 
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.autoConnect("Garage X Quiz");
  mqtt.onConnect(onMqttConnect);
  mqtt.onDisconnect(onMqttDisconnect);
  mqtt.onSubscribe(onMqttSubscribe);
  mqtt.onMessage(onMqttMessage);
  mqtt.setCredentials(MQTT_UNAME, MQTT_PASS);
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  connectToMqtt();
  TickerForconnectToMqtt.attach_ms(10000, connectToMqtt);
}

void onMqttConnect(bool sessionPresent) 
{
  mqtt.subscribe(mqtt_input.c_str(), 2);
  TickerForTestMessage.attach_ms(5000, send_test_msg);
  TickerForCheckInput.attach_ms(10, checkInput);
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) 
{
  serialDisplay("device", "Status", "Ready To Pair");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) 
{
  serialDisplay("","MQTT","Disconnected.");
  TickerForTestMessage.detach();
  TickerForCheckInput.detach();
  connectToMqtt();
}

void connectToMqtt() {
  mqtt.connect();
}

void send_test_msg()
{
  String msg = "testData";
  mqtt.publish(mqtt_debug.c_str(), 2, false, msg.c_str(), msg.length());
}

/*-------Meathod called on reciving message from MQTT-------*/
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
  String s = "";
  for(int i=index; (unsigned)i<len;i++)
  {
    s += payload[i];
  }
  StaticJsonDocument<1000> root;
  DeserializationError error = deserializeJson(root, s);
  if (error) 
  {
    return;
  }
  action = conv(root["action"]);
  if(action == "logo")
  {
    serialDisplay("logo", conv(root["head"]), conv(root["body"]));
  }
  else if(action == "wifi")
  {
    serialDisplay("wifi", conv(root["head"]), conv(root["body"]));
  }
  else if(action == "sync")
  {
    serialDisplay("sync", conv(root["head"]), conv(root["body"]));
  }
  else if(action == "device")
  {
    serialDisplay("device", conv(root["head"]), conv(root["body"]));
  }
  else if(action == "done")
  {
    serialDisplay("done", conv(root["head"]), conv(root["body"]));
  }
  else if(action == "failed")
  {
    serialDisplay("failed", conv(root["head"]), conv(root["body"]));
  }
  else if(action == "print")
  {
    serialDisplay("", conv(root["head"]), conv(root["body"]));
  }
  else if(action == "reset")
  {
    ESP.reset();
  }
}


String conv(const char* str)
{
  return str;
}
void serialDisplay(String icon, String head, String body)
{
  StaticJsonDocument<1000> root;
  root["icon"] = icon;
  root["head"] = head;
  root["body"] = body;
  String data = "";
  serializeJson(root, data);
  for(int i=0; i<data.length(); i++)
  {
    Serial.print(data[i]);
  }
  Serial.println();
  delay(10);
}

void checkInput()
{
  if(Serial.available())
  {
    serialInput = "";
    serialInput = Serial.readStringUntil('\n');
    Serial.flush();
    serialInput.trim();

    StaticJsonDocument<1000> root;
    root["c"] = chipid;
    String output = "";
    if(serialInput == "Left Top")
    { 
      root["a"] = "bn_press";
      root["v"] = 1;
      serializeJsonPretty(root, output);
    }
    else if(serialInput == "Right Top")
    {
      root["a"] = "bn_press";
      root["v"] = 2;
      serializeJsonPretty(root, output);
    }
    else if(serialInput == "Left Bottom")
    {
      root["a"] = "bn_press";
      root["v"] = 3;
      serializeJsonPretty(root, output);
    }
    else if(serialInput == "Right Bottom")
    {
      root["a"] = "bn_press";
      root["v"] = 4;
      serializeJsonPretty(root, output);
    }
    else if(serialInput == "Start")
    {
      root["a"] = "bn_press";
      root["v"] = 0;
      serializeJsonPretty(root, output);
    }
    else if(serialInput == "Select")
    {
      root["a"] = "bn_press";
      root["v"] = -1;
      serializeJsonPretty(root, output);
    }
    mqtt.publish(mqtt_output.c_str(), 2, false, output.c_str(), output.length());
  }
}

void loop() {
}
