#include "apds9900.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace apds9900 {

static const char *const TAG = "apds9900";

void APDS9900Component::update() 
{
  float proximity, light; 
  this->read_sensor_(&proximity, &light);
  
  if (this->light_sensor_ != nullptr)
    this->light_sensor_->publish_state(light);
  if (this->proximity_sensor_ != nullptr)
    this->proximity_sensor_->publish_state(proximity);
  this->status_clear_warning();
  
  ESP_LOGD(TAG, "Got light=%.1flx proximity=%.1fmm", light, proximity);
}

void APDS9900Component::setup() 
{
  ESP_LOGCONFIG(TAG, "Setting up APDS9900...");
  
  int atm[] = {1, 10, 37, 64, 256};
  int an = sizeof(atm) / sizeof(atm[0]);
  uint8_t ATIMEv = 0;
  for (int i = 0; i < an; i++) 
  {
      if (atm[i] == atime_) 
	  {
        uint8_t ATIMEv = atimeHex[atm[i]];
      }
  }
  int wtm[] = {1, 74, 256};
  int wn = sizeof(wtm) / sizeof(wtm[0]);
  uint8_t WTIMEv = 0;
  for (int i = 0; i < wn; i++) 
  {
      if (wtm[i] == atime_) 
	  {
        uint8_t WTIMEv = atimeHex[wtm[i]];
      } 
  }
  
  uint8_t ATIME = ATIMEv; // 2.72 ms - 696.32 ms 
  uint8_t PTIME = 0xFF;   // 2.7 ms
  uint8_t WTIME = WTIMEv; // 2.72 ms - 696.32 ms
  uint8_t PPCOUNT = 0x01; // Minimum prox pulse count
    
  // Disable and power down
  write_reg_data(0x00, 0x00);

  // Set integration times and pulse count
  write_reg_data(0x01, ATIME);
  write_reg_data(0x02, PTIME);
  write_reg_data(0x03, WTIME);
  write_reg_data(0x0E, PPCOUNT);

  uint8_t PDRIVE = pdrive_; // 0x00 100mA LED power
  uint8_t PDIODE = 0x20;    // CH1 diode, 0x10 = CH0 diode?
  uint8_t PGAIN = 0x00;     // 1x Prox gain
  uint8_t AGAIN = again_;   // 0x00 1x ALS gain

  // Configure drive, diode, and gain settings
  write_reg_data(0x0F, PDRIVE | PDIODE | PGAIN | AGAIN);

  uint8_t WEN = 0x08; // Enable Wait - writing a 1 activates, 0 disables the wait timer.
  uint8_t PEN = 0x04; // Enable Prox - writing 1 enables, 0 disables proximity. pen_
  uint8_t AEN = 0x02; // Enable ALS - writing 1 activates, 0 disables the two channel ADC.
  uint8_t PON = 0x01; // Enable Power On - activates the internal oscillator to permit the timers and ADC channels to operate. Writing 1 activates, 0 disables the oscillator.
  // Enable sensor features
  write_reg_data(0x00, WEN | PEN | AEN | PON);
  
  // Wait for sensor to stabilize
  delay(12);
}

void APDS9900Component::dump_config() 
{
  ESP_LOGD(TAG, "APDS9900:");
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);

  if (this->is_failed()) 
  {
    switch (this->error_code_) 
	{
      case COMMUNICATION_FAILED:
        ESP_LOGE(TAG, "Communication with APDS9900 failed!");
        break;
      case WRONG_ID:
        ESP_LOGE(TAG, "APDS9900 has invalid id!");
        break;
      default:
        ESP_LOGE(TAG, "Setting up APDS9900 registers failed!");
        break;
    }
  }
}

bool HOT IRAM_ATTR APDS9900Component::read_sensor_(float *proximity, float *light) 
{
  *proximity = NAN;
  *light = NAN;
  
  if (!pdrive_ && !again_ && !atime_ && !wtime_) 
  {
	  return false;
  } 

  // Definition:
  int ATIME = 256 - atime_;
  float ALSIT = 2.72 * (256 - ATIME);
  int GAIN = againMultiple[again_];
  int DF = 52;     // Device Factor, 52 for APDS-9900/9901
  float GA = 0.48; // Glass (or Lens) Attenuation Factor
  float B = 2.23;  // Coefficients in open air
  float C = 0.7;   // Coefficients in open air
  float D = 1.42;  // Coefficients in open air
  float LPC = ((float)GA * (float)DF) / ((float)ALSIT * GAIN); // Lux per Count

  uint8_t CDATAL = read_word(0x14);
  uint8_t CDATAH = read_word(0x15);
  uint8_t IRDATAL = read_word(0x16);
  uint8_t IRDATAH = read_word(0x17);
  uint8_t PDATAL = read_word(0x18);
  uint8_t PDATAH = read_word(0x19);

  uint16_t CH0DATA = 256 * (uint16_t)CDATAH + CDATAL;
  uint16_t CH1DATA = 256 * (uint16_t)IRDATAH + IRDATAL;
  uint16_t Prox_data = 256 * (uint16_t)PDATAH + PDATAL;

  float IAC1 = CH0DATA - ((float)B * CH1DATA);
  float IAC2 = ((float)C * CH0DATA) - ((float)D * CH1DATA);
  int IAC = IAC1;
  if (IAC1 < IAC2) 
  {
    IAC = IAC2;
  }
  int LUX = IAC * (float)LPC;
  
  // This calculation works well in the 0-100mm range and under datasheet conditions.
  // In other situations, such as the presence of glass or other reflective surfaces, the result may be inaccurate.
  // Prox_data = 100 = 0mm???, Prox_data = 520(+-104) = 100mm, max. 1023
  int distance_mm = (uint16_t)Prox_data / 5.2;
  if (distance_mm < 0) 
  {
     distance_mm = -1  
  }
  
  *proximity = distance_mm;
  *light = LUX;
  
  // LOG_SENSOR("  ", "Proximity", this->proximity_sensor_);
  // LOG_SENSOR("  ", "Light", this->light_sensor_);
  
  return true;
}

}  // namespace apds9900
}  // namespace esphome
