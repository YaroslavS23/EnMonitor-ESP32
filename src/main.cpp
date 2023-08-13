#include <Arduino.h>
#include <WiFi.h>

#include <EmonLib.h>             
EnergyMonitor emon1;
float Irms ;
float supplyVoltage;
float apparentPower;
float powerFactor;

#define AP_SSID "MERCUSYS_B85E"
#define AP_PASS "47950450"

#include <GyverPortal.h>
GyverPortal ui;

TaskHandle_t Task1, Task2; 

void taskCore1(void * pvParameters) {
  for(;;){
    emon1.calcVI(20,1000);         // Calculate all. No.of half wavelengths (crossings), time-out
    //emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
  
    // float realPower       = emon1.realPower;        //extract Real Power into variable
    //float apparentPower   = emon1.apparentPower;    //extract Apparent Power into variable
    // float powerFActor     = emon1.powerFactor;      //extract Power Factor into Variable
    supplyVoltage   = emon1.Vrms;
    // if (emon1.Vrms < 50.0) supplyVoltage = 0.0;

    Irms            = emon1.Irms;             //extract Irms into Variable
    // if (emon1.Irms < 0.001) Irms  = 0.0; 

    apparentPower =  Irms * supplyVoltage;

    powerFactor = emon1.powerFactor;

    Serial.print(supplyVoltage);
    Serial.print("V ");
    Serial.print(Irms);
    Serial.print("A ");
    Serial.print(apparentPower);
    Serial.println("W ");

  }
}

void taskCore0(void * pvParameters) {
  for(;;){
    ui.tick();
  }
}

void build() {
  GP.BUILD_BEGIN(600);
  GP.PAGE_TITLE("Счетчик");
  GP.THEME(GP_DARK);
  GP.TITLE("Датчик энергии");
  GP.HR();
  GP.UPDATE("num1,num2,num3",2000);
  GP_MAKE_BLOCK_TAB(
    "Параметры",
    GP_MAKE_BOX(GP.LABEL("Ток");        GP.NUMBER_F("num1","",Irms,3,"120px",true);           GP.LABEL("А"); );
    GP_MAKE_BOX(GP.LABEL("Напряжение"); GP.NUMBER_F("num2","",supplyVoltage,2,"120px",true);  GP.LABEL("В"); );
    GP_MAKE_BOX(GP.LABEL("Мощность");   GP.NUMBER_F("num3","",apparentPower,2,"120px",true);  GP.LABEL("Вт"); ); 



  );
  GP.SYSTEM_INFO(); 
  GP.BREAK(); 
  GP.BUILD_END();
}

void action() {
  if (ui.update()) {
    ui.updateFloat("num1", Irms);
    ui.updateFloat("num2", supplyVoltage);
    ui.updateFloat("num3", apparentPower);
  }
}

void setup() {
  Serial.begin(115200);

  emon1.voltage(34, 196.50, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(36, 1.1);       // Current: input pin, calibration.

  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  ui.attachBuild(build);
  ui.attach(action);
  ui.start();
 
  xTaskCreatePinnedToCore(taskCore0,"Task1",10000,NULL,1,&Task1,0);   //Запускаем void taskCore0 на 0-вом ядре
  delay(500); 
  xTaskCreatePinnedToCore(taskCore1,"Task2",10000,NULL,1,&Task2,1);   //Запускаем void taskCore1 на 1-вом ядре
  delay(500); 
}

void loop() {

}
