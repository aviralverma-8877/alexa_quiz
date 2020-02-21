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
  lcdDisplay("", "WiFi", "Remote", true);
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
    serialData = Serial.readString();
    Serial.flush();
    serialData.trim();
    DeserializationError error = deserializeJson(doc, serialData);
    if (error) {
      return;
    }
    icon = conv(doc["icon"]);
    head = conv(doc["head"]);
    body = conv(doc["body"]);
    lcdDisplay(icon, head, body, false);
  }
  if(digitalRead(BTN5) == HIGH)
  {
    if(lastPin != 1)
    {
      lastPin = 1;
      Serial.println("Start");
    }
  }
  else if(digitalRead(BTN4) == HIGH)
  {
    if(lastPin != 2)
    {
      lastPin = 2;
      Serial.println("Right Bottom");
    }
  }
  else if(digitalRead(BTN2) == HIGH)
  {
    if(lastPin != 3)
    {
      lastPin = 3;
      Serial.println("Right Top");
    }
  }  
  else if(digitalRead(BTN3) == HIGH)
  {
    if(lastPin != 4)
    {
      lastPin = 4;
      Serial.println("Left Bottom");
    }
  }
  else if(digitalRead(BTN1) == HIGH)
  {
    if(lastPin != 5)
    {
      lastPin = 5;
      Serial.println("Left Top");
    }
  }
  else
  {
    lastPin = 0;
  }
  delay(10);
}


void lcdDisplay(String icon, String head, String body, bool blank)
{ 
  hx1230Fill(0);
  hx1230WriteString(4, 0, head.c_str(), FONT_SMALL, 0);
  char r;
  int i=0;
  int row = 1;
  String disp = "";
  while(i<body.length())
  {
    r = body.c_str()[i];
    if(isAlphaNumeric(r) || r=='.' || r=='-' || r=='+' || r==' ')
      disp = disp + r;
    if(r == '\n')
    {
      hx1230WriteString(4, row, disp.c_str(), FONT_SMALL, 0);
      row++;
      disp = "";    
    }
    i++; 
  }
  hx1230WriteString(4, row, disp.c_str(), FONT_SMALL, 0);
}


String conv(const char* str)
{
  return str;
}
