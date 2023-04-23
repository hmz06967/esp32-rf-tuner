#include "tuner.h"
#include <Wire.h>

int TUNER::init(){
    ctrl.cp = 1;//charge pump
    set_mode(0);//normal mode
    set_divider(0);//step count, 0: 50khz, 1: 31.25khz, 2: 62.50khz
    set_tuning_amplifer(1);
    set_frequency(50.0);//45 mhz
    set_band(6);//vhf
    
    uint8_t f1 = data.freq & 0x00FF,
            f2 = ((data.freq>>8) & 0x00FF);
            
    Wire.begin();   
    Wire.beginTransmission(I2C_ADDR_TUNER);
    Wire.write(f1);
    Wire.write(f2);
    Wire.write(data.ctrl);
    Wire.write(data.band);
    err = Wire.endTransmission();

    if(err==0)
        data.init = 1;

    return err;

}

int TUNER::set_mode(uint8_t mode){
    ctrl.test = mode;
    return set_control();
}

int TUNER::set_divider(uint8_t dvdr){
    if(dvdr==0){
        ctrl.dvd_ratio = DVDRATIO_50KHZ;
    }else if(dvdr==1){
        ctrl.dvd_ratio = DVDRATIO_31_25KHZ;
    }else {
        ctrl.dvd_ratio = DVDRATIO_62_50KHZ;
    }
    return set_control();
}   

int TUNER::set_frequency(float freq){
    
    uint8_t ratio = 50;//50 khz step counter
    if(ctrl.dvd_ratio==DVDRATIO_31_25KHZ){
        ratio = 31.25;
    }else if(ctrl.dvd_ratio==DVDRATIO_62_50KHZ){
        ratio = 62.5;
    }

    if(freq>361.0){
        set_band(0);
    }else if(freq<127.26){
        set_band(6);
    }else if(freq>133.24 && freq<361.26){
        set_band(7);
    }else{
        set_band(4);//all frequency
    }

    data.freq_offset = 37.7;
    data.freq = ((freq + data.freq_offset)*1000)/ratio;

    set_control();
    
    uint8_t f1 = data.freq & 0x00FF,
            f2 = ((data.freq>>8) & 0x00FF);
            
    if(data.init){//is init? direct send register  
      Wire.beginTransmission(I2C_ADDR_TUNER);
      Wire.write(f1);
      Wire.write(f2);
      //Wire.write(data.ctrl);
      //Wire.write(data.band);
      err = Wire.endTransmission();
    }

    return err;
}

int TUNER::set_band(uint8_t band){

    switch (band){
        case 0:
            data.band = BAND_UHF;
            break;
        case 1:
            data.band = BAND_LOW_UHF;
            break;
        case 2:
            data.band = BAND_MEDIUM_UHF;
            break;
        case 3:
            data.band = BAND_HIGH_UHF;
            break;
        case 4:
            data.band = BAND_ULTRAHIGH_UHF;
            break;
        case 5:
            data.band = BAND_VHF;
            break;
        case 6:
            data.band = BAND_LOW_VHF;
            break;
        case 7:
            data.band = BAND_HIGH_VHF;
            break;      
        default:
            break;
    }

    return set_control();
}

int TUNER::set_tuning_amplifer(uint8_t ton){
    ctrl.turn_amp = ton;
    return set_control();
}

int TUNER::set_control(){
    err = 0;  
    data.ctrl = 0x80 | (ctrl.cp<<6);
    data.ctrl |= (ctrl.test<<3); 
    data.ctrl |= (ctrl.dvd_ratio<<2);
    data.ctrl |= (ctrl.turn_amp & 0x01); 
    
    /*if(data.init){
       Wire.beginTransmission(I2C_ADDR_TUNER);
       Wire.write(data.ctrl);
       Wire.write(data.band);
       err = Wire.endTransmission(); 
    }*/
    return err;
}

int TUNER::read_pll_status(){
    /*Wire.beginTransmission(I2C_ADDR_TUNER | 1<<7);
    Wire.write(0xB8);
    err = Wire.endTransmission();*/
    err = Wire.requestFrom(I2C_ADDR_TUNER | 1<<7, 1);    //read register from tuner 1 bytes 

    if(Wire.available()){
        uint8_t r = Wire.read();
        ctrl.adc_level = 7 & r;
        ctrl.lock = (64 & r)>>6;
        ctrl.power = (128 & r)>>7;
        //Serial.println(r,HEX);
        return 0;
    }
    
    return err;
}
