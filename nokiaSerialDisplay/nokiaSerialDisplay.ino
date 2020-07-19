#include <ArduinoJson.h>
#include <bb_hx1230.h>

String serialData;
byte lastPin = 0;
// Software SPI (slower updates, more flexible pin options):
// pin 4 - Serial clock out (SCLK)
// pin 5 - Serial data out (DIN)
// pin 6 - Data/Command select (D/C)
// pin 7 - LCD chip select (CS)
// pin 8 - LCD reset (RST)
StaticJsonDocument<1000> doc;


#define bkled 5
#define BTN1 6
#define BTN2 7
#define BTN3 8
#define BTN4 9
#define BTN5 10

String icon, head, body;
byte w = 84;
byte h = 48;

void setup() {
  hx1230Init(2, 5, 4, 12, 13, 0, 0); // on the ATtiny85, I wired CE to ground, and BL to Vcc, so 3 pins are needed.
  hx1230Backlight(1); // turn on backlight (if connected)
  hx1230Fill(0);      // erase display memory
  hx1230SetContrast(15);
  Serial.begin(115200);
  lcdDisplay("WiFi", "Remote");
  pinMode(bkled, OUTPUT);
  digitalWrite(bkled, HIGH);
  delay(1000);
  pinMode(BTN1, INPUT);
  pinMode(BTN2, INPUT);
  pinMode(BTN3, INPUT);
  pinMode(BTN4, INPUT);
  pinMode(BTN5, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available())
  {
    serialData = "";
    char r;
    delay(10);
    while(Serial.available())
    {
      r = Serial.read();
      serialData += r;
    }
    serialData.trim();
    Serial.flush();
    StaticJsonDocument<200> jsonBuffer;
    DeserializationError error = deserializeJson(jsonBuffer, serialData);
    if (error) {
      return;
    }
    icon = conv(jsonBuffer["icon"]);
    head = conv(jsonBuffer["head"]);
    body = conv(jsonBuffer["body"]);
    lcdDisplay(String(head), String(body));
  }
  StaticJsonDocument<200> jsonBuffer;
  if(digitalRead(BTN1) == HIGH)
  {
    if(lastPin != 1)
    {
      lastPin = 1;
      jsonBuffer["a"] = "btn_pressed";
      jsonBuffer["b"] = "Left Top";
      serializeJson(jsonBuffer, Serial);
      Serial.println();
    }
  }
  else if(digitalRead(BTN2) == HIGH)
  {
    if(lastPin != 2)
    {
      lastPin = 2;
      jsonBuffer["b"] = "Right Top";
      jsonBuffer["a"] = "btn_pressed";
      serializeJson(jsonBuffer, Serial);
      Serial.println();
    }
  }
  else if(digitalRead(BTN3) == HIGH)
  {
    if(lastPin != 3)
    {
      lastPin = 3;
      jsonBuffer["b"] = "Left Bottom";
      jsonBuffer["a"] = "btn_pressed";
      serializeJson(jsonBuffer, Serial);
      Serial.println();
    }
  }  
  else if(digitalRead(BTN4) == HIGH)
  {
    if(lastPin != 4)
    {
      lastPin = 4;
      jsonBuffer["b"] = "Right Bottom";
      jsonBuffer["a"] = "btn_pressed";
      serializeJson(jsonBuffer, Serial);
      Serial.println();
    }
  }
  else if(digitalRead(BTN5) == HIGH)
  {
    if(lastPin != 5)
    {
      lastPin = 5;
      jsonBuffer["b"] = "Select";
      jsonBuffer["a"] = "btn_pressed";
      serializeJson(jsonBuffer, Serial);
      Serial.println();
    }
  }
  else
  {
    lastPin = 0;
  }
  delay(10);
}


void lcdDisplay(String head, String body)
{ 
  hx1230Fill(0);
  hx1230WriteString(1, 0, head.c_str(), FONT_SMALL, 0);
  if(body.length() > 14)
  {
    int row = 1;
    for(int i=0; i<body.length(); i=i+15)
    {
      String sub =  "";
      char r;
      for(int j=i; j < min((i+15),body.length()); j++)
      {
        r = body.c_str()[j];
        sub += r;
      }
      hx1230WriteString(1, row, sub.c_str(), FONT_SMALL, 0);
      row++;
    }
  }
  else
  {
    hx1230WriteString(1, 1, body.c_str(), FONT_SMALL, 0);
  }
}


String conv(const char* str)
{
  return str;
}
