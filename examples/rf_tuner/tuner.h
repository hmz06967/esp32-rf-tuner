#ifndef TUNER_H_
#define TUNER_H_

#include <Arduino.h>
#include <Wire.h>

#define I2C_ADDR_TUNER 0x61//C2 write, C2+1 read

//vhf 50 - 400 Mhz, Uhf 300 - 900 Mhz
//BS3 port 1; 
#define BAND_UHF 0x4//361.25 - 801.25 Mhz 
#define BAND_LOW_UHF 0x0C //port off
#define BAND_MEDIUM_UHF 0x0D//vhf low + uhf
#define BAND_HIGH_UHF 0x0E //vhf high + uhf
#define BAND_ULTRAHIGH_UHF 0x0F //all freq

#define BAND_VHF 0x07//vhf 50 - 400 mhz
#define BAND_HIGH_VHF 0x06//133.25 - 361.25 Mhz
#define BAND_LOW_VHF 0x05 //55.25 - 127.25 Mhz

#define DVDRATIO_50KHZ 0x00
#define DVDRATIO_31_25KHZ 0x01
#define DVDRATIO_62_50KHZ 0x03

typedef struct ctrl_byte_{
    uint8_t cp;//Charge-pumpcurrentsetbit default: 1
    uint8_t test;//test bits default: 1
    uint8_t dvd_ratio;//diveder ratio 
    uint8_t turn_amp;
    uint8_t adc_level;
    uint8_t lock;
    bool power;
} ctrl_byte;

typedef struct Data_
{
    uint8_t ctrl;
    uint16_t freq;
    uint8_t band;
    float freq_offset;
    bool init = 0;
} Data;

class TUNER{
    public:
        ctrl_byte ctrl;
        Data data;
        uint8_t err;

        int init();
        int set_mode(uint8_t mode);
        int set_frequency(float freq);
        int set_divider(uint8_t diveder);
        int set_adc_level(uint8_t level);//output voltage range set
        int set_band(uint8_t band);
        int read_pll_status();
    private:
        int set_tuning_amplifer(uint8_t ton);
        int set_control();


};

#endif
