#include "zmpt101b.h"
#include <cmath>
#include "esphome/core/log.h"

namespace esphome {
namespace zmpt101b {

static const char *const TAG = "zmpt101b";

void ZMPT101BSensor::update() {
	accumulated_v_ = 0.0;
	iteration_ = 0;
	vsum_ = 0;
	count_ = 0;
	state_ = State::CALIBRATING;
	measuring_ = true;
}

void ZMPT101BSensor::loop() {
	if (!measuring_)
		return;

	uint32_t now = micros();

	if (count_ == 0)
		t_start_ = now;

	if (state_ == State::CALIBRATING) {
		if (now - t_start_ < period_) {
			vsum_ += adc_sensor_->sample() * ADC_SCALE;
			count_++;
		} else {
			zero_point_ = count_ ? (vsum_ / count_) : 0;
			vsum_ = 0;
			count_ = 0;
			state_ = State::MEASURING;
		}
	} else {
		if (now - t_start_ < period_) {
			int32_t v = (adc_sensor_->sample() * ADC_SCALE) - zero_point_;
			vsum_ += (int64_t)v * v;
			count_++;
		} else {
			double v_rms = sqrt((double) vsum_ / count_) / ADC_SCALE * VREF;
			accumulated_v_ += v_rms * (1000.0 / sensitivity_);
			vsum_ = 0;
			count_ = 0;
			if (++iteration_ >= loop_count_) {
				publish_state(accumulated_v_ / loop_count_);
				measuring_ = false;
			}
			state_ = State::CALIBRATING;
		}
	}
}

}  // namespace zmpt101b
}  // namespace esphome
