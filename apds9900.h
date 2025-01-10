#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "Wire.h"

namespace esphome {
namespace apds9900 {

class APDS9900Component : public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  void update() override;

  void set_light_sensor(sensor::Sensor *light_sensor) { light_sensor_ = light_sensor; }
  void set_proximity_sensor(sensor::Sensor *proximity_sensor) { proximity_sensor_ = proximity_sensor; }
  
  uint8_t atimeHex[5] = {0xff, 0xf6, 0xdb, 0xc0, 0x00}; // [1x - 2.72 ms, 10x - 27.2 ms, 37x - 100.64 ms, 64x - 174.08 ms, 256x - 696.32 ms]
  uint8_t wtimeHex[3] = {0xff, 0xb6, 0x00}; // [1x - 2.72 ms, 74x - 201.29 ms, 256x - 696.32 ms]
  int againMultiple[4] = {1, 8, 16, 120};

  void set_pdrive(uint8_t level) { this->pdrive_ = level; }
  void set_atime(uint8_t a_time) { this->atime_ = a_time; }
  void set_wtime(uint8_t w_time) { this->wtime_ =  w_time; }
  void set_again(uint8_t gain) { this->again_ = gain; }
  
  private:
  int indexA = -1;
  int indexW = -1;
  void write_reg_data(uint8_t reg, uint8_t data) {
    Wire.beginTransmission(0x39);
    Wire.write(0x80 | reg);
    Wire.write(data);
    Wire.endTransmission();
  }

  uint16_t read_word(uint8_t reg) {
    uint8_t buffer[2] = {0};
    Wire.beginTransmission(0x39);
    Wire.write(0xA0 | reg);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)0x39, (uint8_t)2);

    if (Wire.available() == 2) {
        buffer[0] = Wire.read();
        buffer[1] = Wire.read();
    }

    return (uint16_t)(buffer[0] + (buffer[1] << 8));
  }
  
  uint8_t pdrive_;
  uint8_t atime_;
  uint8_t wtime_;
  uint8_t again_;
  
  enum ErrorCode {
    NONE = 0,
    COMMUNICATION_FAILED,
    WRONG_ID,
  } error_code_{NONE};

 protected:
  bool read_sensor_(float *proximity, float *light);
  sensor::Sensor *light_sensor_{nullptr};
  sensor::Sensor *proximity_sensor_{nullptr};
};

}  // namespace apds9900
}  // namespace esphome