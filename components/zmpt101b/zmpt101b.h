#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/adc/adc_sensor.h"

namespace esphome {
namespace zmpt101b {

class ZMPT101BSensor : public sensor::Sensor, public PollingComponent {
 public:
  void set_adc_sensor(adc::ADCSensor *sensor) { adc_sensor_ = sensor; }
  void set_frequency(uint16_t freq) { period_ = 1000000UL / freq; }
  void set_sensitivity(float sens) { sensitivity_ = sens; }

  float get_setup_priority() const override { return setup_priority::DATA; }
  void setup() override;
  void update() override;
  void dump_config() override;

 protected:
  adc::ADCSensor *adc_sensor_{nullptr};
  uint32_t period_{20000};
  float sensitivity_{500.0f};
  float zero_volts_{-1.0f};
};

}  // namespace zmpt101b
}  // namespace esphome
