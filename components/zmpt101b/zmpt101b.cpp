#include "zmpt101b.h"
#include <cmath>
#include "esphome/core/log.h"

namespace esphome {
namespace zmpt101b {

static const char *const TAG = "zmpt101b";

void ZMPT101BSensor::setup() {
  // Initial zero-point calibration over one full AC period
  float vsum = 0.0f;
  uint32_t count = 0;
  uint32_t t_start = micros();
  while (micros() - t_start < period_) {
    vsum += adc_sensor_->sample();
    count++;
  }
  zero_volts_ = count ? (vsum / count) : 1.65f;
  ESP_LOGD(TAG, "Initial zero=%.4fV (n=%u)", zero_volts_, count);
}

void ZMPT101BSensor::update() {
  float zsum = 0.0f;
  double sq_sum = 0.0;
  uint32_t count = 0;

  uint32_t t_start = micros();
  while (micros() - t_start < period_) {
    float raw = adc_sensor_->sample();
    zsum += raw;
    float v = raw - zero_volts_;
    sq_sum += (double)(v * v);
    count++;
  }

  if (count == 0)
    return;

  // Track zero_point drift (VCC fluctuation)
  zero_volts_ = zsum / (float)count;

  // sample() returns volts — v_rms is already in volts, no ADC_SCALE needed
  double v_rms = sqrt(sq_sum / count);
  float voltage = (float)(v_rms * (1000.0 / sensitivity_));

  ESP_LOGD(TAG, "zero=%.4fV n=%u v_rms=%.4fV out=%.2fV", zero_volts_, count, v_rms, voltage);
  publish_state(voltage);
}

void ZMPT101BSensor::dump_config() {
  LOG_SENSOR("", "ZMPT101B", this);
  ESP_LOGCONFIG(TAG, "  Period: %u µs (%.0f Hz)", period_, 1000000.0f / period_);
  ESP_LOGCONFIG(TAG, "  Sensitivity: %.2f", sensitivity_);
}

}  // namespace zmpt101b
}  // namespace esphome
