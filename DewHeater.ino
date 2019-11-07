#include "DewHeater.h"
// OneWire
#include <OneWire.h>
#include <DallasTemperature.h>

// LCD
#include <Wire.h> 

// Setup a oneWire instance to communicate with DS18B20
OneWire ds(ONE_WIRE_BUS);

void InitDewHeater(void)
{
  // Heater
  pinMode(HEAT,OUTPUT);
  analogWrite(HEAT,0);
}

void ManageDewHeater(bool slewing) {
  static long secondsSinceLastRun=0;
  static bool pickup=false;
  secondsSinceLastRun++;  
  if (slewing) return;

  if (pickup) {
    //pickupDS18B20reading(T);  
    pickup=false;
    // pickup BME-280
    // calc dew point
    // turn heater on/off
    CommandDewHeater();
  }
   if (secondsSinceLastRun>=30) {
    secondsSinceLastRun=0;
    startDS18B20reading(); pickup=true;
  }
}

void startDS18B20reading() {
  // Poweroff heater (parasits)
  //analogWrite(HEAT,0);
  digitalWrite(HEAT,LOW);
  if (ds.reset()) {
    ds.write(0xcc);
    ds.write(0x44,1);
  }
}

float getTmirror() {
		return Tmirror;
}

void CommandDewHeater() {
  byte data[12];
  Tmirror=-100;
  if (ds.reset()) {
    ds.write(0xcc);
    ds.write(0xbe);
    int i;
    for (i=0;i<2;i++) {
      data[i]=ds.read();
    }
    int16_t raw = (data[1] << 8) | data[0];
    Tmirror = (float)raw / 16.0;
  }
  float Hu=ambient.getHumidity();
  //float Hu=bme.readHumidity();
  float Te =ambient.getTemperature();
  //float Te=bme.readTemperature();
  float Pr=ambient.getDewPoint();
  //float Pr=dewPoint(Te,Hu);
  if (Tmirror>-100) {
    if (Tmirror <80 and Tmirror<(Pr+DIFF)) {
      //analogWrite(HEAT,HEATLEVEL);
      digitalWrite(HEAT,HIGH);
    }
  } 
}

/*
// Dew point calc
double dewPoint(double celsius, double humidity) {
  // Constantes d'approximation
  // Voir http://en.wikipedia.org/wiki/Dew_point pour plus de constantes
  const double a = 17.27;
  const double b = 237.7;
  // Calcul (approximation)
  double temp = (a * celsius) / (b + celsius) + log(humidity * 0.01);
  return (b * temp) / (a - temp);
}
*/


