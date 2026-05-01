#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/gpio.h"

namespace esphome {
namespace zmpt101b {

class ZMPT101BSensor : public sensor::Sensor, public PollingComponent {
 public:
  void set_pin(InternalGPIOPin *pin) { pin_ = pin; }
  void set_frequency(uint16_t freq) { period_ = 1000000UL / freq; }
  void set_sensitivity(float sens) { sensitivity_ = sens; }

  float get_setup_priority() const override { return setup_priority::DATA; }
  void setup() override;
  void update() override;
  void dump_config() override;

 protected:
  InternalGPIOPin *pin_{nullptr};
  uint32_t period_{20000};
  float sensitivity_{500.0f};
  int zero_point_{0};
};

}  // namespace zmpt101b
}  // namespace esphome
