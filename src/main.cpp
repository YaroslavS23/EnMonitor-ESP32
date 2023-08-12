#include <Arduino.h>
#include <WiFi.h>

#include <EmonLib.h>             
EnergyMonitor emon1;

#include <GyverPortal.h>
#include <EEPROM.h>

struct LoginPass {
  char ssid[20];
  char pass[20];
};

LoginPass lp;

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  GP.FORM_BEGIN("/login");
  GP.TEXT("lg", "Login", lp.ssid);
  GP.BREAK();
  GP.TEXT("ps", "Password", lp.pass);
  GP.SUBMIT("Submit");
  GP.FORM_END();

  GP.BUILD_END();
}

void action(GyverPortal& p) {
  if (p.form("/login")) {      // кнопка нажата
    p.copyStr("lg", lp.ssid);  // копируем себе
    p.copyStr("ps", lp.pass);
    EEPROM.put(0, lp);              // сохраняем
    EEPROM.commit();                // записываем
    WiFi.softAPdisconnect();        // отключаем AP
  }
}

void loginPortal() {
  Serial.println("Portal start");

  // запускаем точку доступа
  WiFi.mode(WIFI_AP);
  WiFi.softAP("WiFi Config");

  // запускаем портал
  GyverPortal ui;
  ui.attachBuild(build);
  ui.start();
  ui.attach(action);

  // работа портала
  while (ui.tick());
}

void setup() {
  Serial.begin(115200);

  emon1.voltage(34, 196.50, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(36, 1.1);       // Current: input pin, calibration.

  EEPROM.begin(100);
  EEPROM.get(0, lp);

  // если кнопка нажата - открываем портал
  pinMode(2, INPUT_PULLUP);
  if (!digitalRead(2)) loginPortal();

  // пытаемся подключиться
  Serial.print("Connect to: ");
  Serial.println(lp.ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(lp.ssid, lp.pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! Local IP: ");

  Serial.println(WiFi.localIP());
}

void loop() {
  emon1.calcVI(20,1000);         // Calculate all. No.of half wavelengths (crossings), time-out
  emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
  
  float realPower       = emon1.realPower;        //extract Real Power into variable
  float apparentPower   = emon1.apparentPower;    //extract Apparent Power into variable
  float powerFActor     = emon1.powerFactor;      //extract Power Factor into Variable
  float supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable
  float Irms            = emon1.Irms;             //extract Irms into Variable
  
}
