#include <Arduino.h>
#include "tuner.h"
#include "web_server.h"
#include <ESP8266WiFi.h>

TUNER tuner;
float last_freq = 0.0;
float freq = 105.3; 
char *cbuff = (char*) malloc(20 * sizeof(char));

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

  for(address = 0; address < 255; address++ ){
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
      
      //Wire.requestFrom(address, 1);    // request 1 bytes 
      //read = Wire.read();
      Serial.println(address, HEX);
    
  
        nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    

  }

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

  //delay(1000);

  while(tuner.init()!=0){
    Serial.println("error init tuner..");
    //i2c_scan();
    delay(100);
  }

  //tuner.set_frequency(110.0);

  delay(10);

  set_freq(105.3);
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


  init_server();
  

}


void loop() {
  clear_cient();

  while (Serial.available()==0) {             //Wait for user input
    clear_cient();
    digitalWrite(D0, WiFi.isConnected());
    if(last_freq!=freq){
        snprintf(cbuff, 14, "freq:%0.3f,\n", freq);
        last_freq = freq;
    }
  }

  
  float freq_s = Serial.parseFloat ();

  if(freq_s==1.0){
    freq-=0.1;
  }else if(freq_s==3.0){
    freq+=0.1;
  }else{
    freq = freq_s;
  }

  set_freq(freq);
  delay(10);

  /*delay(100);
  while(freq<400){
    freq += 0.05;
    set_freq(freq);
  }*/


}
