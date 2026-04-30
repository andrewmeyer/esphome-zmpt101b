#pragma once

#include "esphome/core/component.h"
#include "esphome/components/adc/adc_sensor.h"

namespace esphome {
namespace zmpt101b {

#define ADC_SCALE 4095
#define VREF 3.3

class ZMPT101BSensor : public sensor::Sensor, public Component {
 public:
  void set_adc_sensor(adc::ADCSensor *sensor) { adc_sensor_ = sensor; }
  void set_frequency(uint16_t freq) { period_ = 1000000UL / freq; }
  void set_sensitivity(float sens) { sensitivity_ = sens; }
  void set_loop_count(float sens) { loop_count_ = sens; }

  void setup() override {}
  void loop() override;

 protected:
  enum class State { CALIBRATING, MEASURING };

  adc::ADCSensor *adc_sensor_;
  uint32_t period_;
  float sensitivity_;
  int loop_count_;

  State state_{State::CALIBRATING};
  uint32_t t_start_{0};
  uint32_t vsum_{0};
  uint32_t count_{0};
  int32_t zero_point_{0};
  double accumulated_v_{0.0};
  uint8_t iteration_{0};
};

}  // namespace zmpt101b
}  // namespace esphome
