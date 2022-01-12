from typing import Dict, Any
from esphome.components import i2c
from esphome.core import CORE
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import (
    CONF_FREQUENCY,
    CONF_ID,
    CONF_I2C,
    CONF_I2C_ID,
    CONF_INTERRUPT,
    CONF_OFFSET,
    CONF_PINS,
)
import esphome.pins as pins

DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["i2c"]

vl53l1x_ns = cg.esphome_ns.namespace("vl53l1x")
VL53L1X = vl53l1x_ns.class_("VL53L1X", cg.Component)

CONF_AUTO = "auto"
CONF_CALIBRATION = "calibration"
CONF_RANGING_MODE = "ranging"
CONF_XSHUT = "xshut"
CONF_XTALK = "xtalk"

Ranging = vl53l1x_ns.namespace("Ranging")
RANGING_MODES = {
    CONF_AUTO: CONF_AUTO,
    "shortest": Ranging.Shortest,
    "short": Ranging.Short,
    "medium": Ranging.Medium,
    "long": Ranging.Long,
    "longer": Ranging.Longer,
    "longest": Ranging.Longest,
}

int16_t = cv.int_range(min=-32768, max=32768)  # signed


def NullableSchema(*args, default: Any = None, **kwargs):
    """
    Same as Schema but will convert nulls to empty objects. Useful when all the schema keys are optional.
    Allows YAML lines to be commented out leaving an "empty dict" which is mistakenly parsed as None.
    """

    def none_to_empty(value):
        if value is None:
            return {} if default is None else default
        raise cv.Invalid("Expected none")

    return cv.Any(cv.Schema(*args, **kwargs), none_to_empty)


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(VL53L1X),
        cv.Optional(CONF_PINS, default={}): NullableSchema(
            {
                cv.Optional(CONF_XSHUT): pins.gpio_input_pin_schema,
                cv.Optional(CONF_INTERRUPT): pins.gpio_output_pin_schema,
            }
        ),
        cv.Optional(CONF_CALIBRATION, default={}): NullableSchema(
            {
                cv.Optional(CONF_RANGING_MODE, default=CONF_AUTO): cv.enum(
                    RANGING_MODES
                ),
                cv.Optional(CONF_XTALK): cv.uint16_t,
                cv.Optional(CONF_OFFSET): int16_t,
            }
        ),
    }
).extend(i2c.i2c_device_schema(0x52))


async def to_code(config: Dict):
    cg.add_library("rneurink", "1.2.3", "VL53L1X_ULD")

    vl53l1x = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(vl53l1x, config)
    await i2c.register_i2c_device(vl53l1x, config)

    # If i2c frequency has not been explicitly set, then increase it to our recommended
    i2c_id = config[CONF_I2C_ID]
    i2c_config = next(
        entry for entry in CORE.config[CONF_I2C] if entry[CONF_ID] == i2c_id
    )
    if i2c_config[CONF_FREQUENCY] == 50000:  # default
        i2c_var = await cg.get_variable(i2c_id)
        cg.add(i2c_var.set_frequency(400000))

    await setup_hardware(vl53l1x, config)
    await setup_calibration(vl53l1x, config[CONF_CALIBRATION])


async def setup_hardware(vl53l1x: cg.Pvariable, config: Dict):
    pins = config[CONF_PINS]
    if CONF_INTERRUPT in pins:
        interrupt = await cg.gpio_pin_expression(pins[CONF_INTERRUPT])
        cg.add(vl53l1x.set_interrupt_pin(interrupt))
    if CONF_XSHUT in pins:
        xshut = await cg.gpio_pin_expression(pins[CONF_XSHUT])
        cg.add(vl53l1x.set_xshut_pin(xshut))


async def setup_calibration(vl53l1x: cg.Pvariable, config: Dict):
    if config.get(CONF_RANGING_MODE, CONF_AUTO) != CONF_AUTO:
        cg.add(vl53l1x.set_ranging_mode_override(config[CONF_RANGING_MODE]))
    if CONF_XTALK in config:
        cg.add(vl53l1x.set_xtalk(config[CONF_XTALK]))
    if CONF_OFFSET in config:
        cg.add(vl53l1x.set_offset(config[CONF_OFFSET]))
