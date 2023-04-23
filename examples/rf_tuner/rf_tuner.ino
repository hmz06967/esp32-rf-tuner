#include <Arduino.h>
#include "tuner.h"

TUNER tuner;

void set_freq(float freq){
    tuner.set_frequency(freq);
    delay(5);
    if(tuner.read_pll_status()==0){
      if(tuner.ctrl.lock==1){
        Serial.print("freq: ");
        Serial.println(freq);
        Serial.print("pll lock: ");
        Serial.println(tuner.ctrl.lock);
      }
    }
}


void i2c_scan()
{
  char error, address, read;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  address = 0xE1;

  /*for(address = 0; address < 255; address++ )
  {*/
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      
      Wire.requestFrom(address, 1);    // request 1 bytes 
      read = Wire.read();
      Serial.println(read, HEX);
    
  
        nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    

   /*}*/

  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000);           // wait 5 seconds for next scan
}

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200); 
  Serial.println("nodemcu init..");
  
  if(tuner.init()!=0){
    Serial.println("error init tuner..");
    i2c_scan();
    return;
  }

  tuner.set_frequency(427.0);

  delay(10);
  
  Serial.print("tuner frequenncy raw: ");
  Serial.println(tuner.data.freq);

  Serial.println("read status...");
  if(tuner.read_pll_status()==0){
    Serial.print("power: ");
    Serial.println(tuner.ctrl.power);
    Serial.print("pll lock: ");
    Serial.println(tuner.ctrl.lock);
    Serial.print("adc level: ");
    Serial.println(tuner.ctrl.adc_level);
  }
  Serial.println("scan channel freq...");
}

float freq = 50; 

void loop() {
  
  if (Serial.available() > 0) {
    freq= Serial.parseFloat ();
    set_freq(freq);
    delay(1);
  }

  delay(100);
  while(freq<400){
    freq += 0.05;
    set_freq(freq);
  }

  delay(5000);
}
