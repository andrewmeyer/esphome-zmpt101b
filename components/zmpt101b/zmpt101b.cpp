#include "zmpt101b.h"
#include <Arduino.h>
#include <cmath>
#include "esphome/core/log.h"

namespace esphome {
namespace zmpt101b {

static const char *const TAG = "zmpt101b";

void ZMPT101BSensor::setup() {
  pin_->setup();
  // Initial zero-point calibration: average one full period
  uint32_t vsum = 0;
  uint32_t count = 0;
  uint8_t pin_num = pin_->get_pin();
  uint32_t t_start = micros();
  while (micros() - t_start < period_) {
    vsum += analogRead(pin_num);
    count++;
  }
  zero_point_ = count ? (int)(vsum / count) : 2048;
  ESP_LOGD(TAG, "Initial zero_point=%d (n=%u)", zero_point_, count);
}

void ZMPT101BSensor::update() {
  uint8_t pin_num = pin_->get_pin();

  // Sample one full AC period.
  // Accumulate raw sum (for zero_point drift update) and squared deviations.
  uint32_t zsum = 0;
  uint64_t sq_sum = 0;
  uint32_t count = 0;
  int zero = zero_point_;

  uint32_t t_start = micros();
  while (micros() - t_start < period_) {
    uint32_t raw = analogRead(pin_num);
    zsum += raw;
    int32_t v = (int32_t)raw - zero;
    sq_sum += (uint64_t)((int64_t)v * v);
    count++;
  }

  if (count == 0)
    return;

  // Update zero_point from this window to track VCC drift
  zero_point_ = (int)(zsum / count);

  double v_rms = sqrt((double)sq_sum / count) / 4095.0 * 3.3;
  float voltage = (float)(v_rms * (1000.0 / sensitivity_));

  ESP_LOGD(TAG, "zero=%d n=%u v_rms=%.4fV out=%.2fV", zero_point_, count, v_rms, voltage);
  publish_state(voltage);
}

void ZMPT101BSensor::dump_config() {
  LOG_SENSOR("", "ZMPT101B", this);
  LOG_PIN("  Pin: ", pin_);
  ESP_LOGCONFIG(TAG, "  Period: %u µs (%.0f Hz)", period_, 1000000.0f / period_);
  ESP_LOGCONFIG(TAG, "  Sensitivity: %.2f", sensitivity_);
}

}  // namespace zmpt101b
}  // namespace esphome
