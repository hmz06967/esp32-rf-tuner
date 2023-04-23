/**
 * @author Ozk
 * @email hamza@hamzaozkan.com.tr
 * @create date 2022-08-19 05:14:51
 * @modify date 2022-08-19 05:14:51
 * @desc Fm tuner software free licance
 */
#include "tuner.h"

int TUNER::init(){
    set_mode(1);//normal mode
    set_divider(0);//step count, 0: 50khz, 1: 31.25khz, 2: 62.50khz
    set_band(4);//vhf
    set_tuning_amplifer(0);//amp on set 0, off: 1 data.os

    Wire.begin(SDA, SCL);   

    data.init = 1;

    return set_frequency(427.0);//setup;

}

int TUNER::set_mode(uint8_t mode){
    ctrl.cp = 1;
    ctrl.test = mode;
    return 0;
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
    
    float ratio = 50.0;//50 khz step counter
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

    data.freq_offset = 37.7;//selected modulator ic if osc frequency
    //data.freq = (uint16_t)(((freq + data.freq_offset)*1000)/ratio);//other tuner look for youre datasheet
    data.freq = (freq+data.freq_offset)*16;
    err = 1;
    
    if(data.init){//is init? direct send register
       Wire.beginTransmission(I2C_ADDR_TUNER);
       Wire.write(((data.freq>>8) & 0x00FF));
       Wire.write((data.freq & 0x00FF));
       Wire.write(data.ctrl);
       Wire.write(data.band);
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

    data.band &= 0x0F; 

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
    data.ctrl |= (ctrl.dvd_ratio<<1);
    data.ctrl |= (ctrl.turn_amp); 
    //printf("ctrl: 0x%X", data.ctrl);

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
        r = r<<1;
        ctrl.adc_level = 7 & r;
        ctrl.lock = (0x40 & r)>>6;
        ctrl.power = (0x80 & r)>>7;
        //Serial.println(r,HEX);
    }
    err = Wire.endTransmission(); 
    
    return err;
}

