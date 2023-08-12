#include <Arduino.h>
#include <WiFi.h>
const char* ssid     = "MERCUSYS_B85E";
const char* password = "47950450";

#include <EmonLib.h>             
EnergyMonitor emon1;

void setup() {
    Serial.begin(115200);
    delay(10);
    
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);

  emon1.voltage(34, 196.50, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(36, 1.1);       // Current: input pin, calibration.
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
