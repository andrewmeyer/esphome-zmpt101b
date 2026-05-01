import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_PIN, CONF_FREQUENCY, CONF_SENSITIVITY

from . import zmpt101b_ns, validate_adc_pin

ZMPT101BSensor = zmpt101b_ns.class_("ZMPT101BSensor", sensor.Sensor, cg.PollingComponent)

CODEOWNERS = ["@toniotruel"]

CONFIG_SCHEMA = sensor.sensor_schema(
    ZMPT101BSensor,
    unit_of_measurement="V",
    accuracy_decimals=1,
    state_class="measurement",
).extend({
    cv.Required(CONF_PIN): validate_adc_pin,
    cv.Required(CONF_SENSITIVITY): cv.float_,
    cv.Optional(CONF_FREQUENCY, default=50): cv.positive_int,
}).extend(cv.polling_component_schema("60s"))

async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)

    pin = await cg.gpio_pin_expression(config[CONF_PIN])
    cg.add(var.set_pin(pin))
    cg.add(var.set_sensitivity(config[CONF_SENSITIVITY]))
    cg.add(var.set_frequency(config[CONF_FREQUENCY]))
