from re import I
from typing import Dict, Union, Any
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import (
    CONF_HEIGHT,
    CONF_ID,
    CONF_INTERRUPT,
    CONF_INVERT,
    CONF_OFFSET,
    CONF_PINS,
    CONF_WIDTH,
)
import esphome.pins as pins

# DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["sensor", "binary_sensor", "text_sensor", "number"]
MULTI_CONF = True

CONF_ROODE_ID = "roode_id"

roode_ns = cg.esphome_ns.namespace("roode")
Roode = roode_ns.class_("Roode", cg.PollingComponent)

CONF_AUTO = "auto"
CONF_ORIENTATION = "orientation"
CONF_CALIBRATION = "calibration"
CONF_DETECTION_THRESHOLDS = "detection_thresholds"
CONF_I2C_ADDRESS = "i2c_address"
CONF_ENTRY_ZONE = "entry"
CONF_EXIT_ZONE = "exit"
CONF_CENTER = "center"
CONF_MAX = "max"
CONF_MIN = "min"
CONF_RANGING_MODE = "ranging"
CONF_ROI = "roi"
CONF_SAMPLING = "sampling"
CONF_XSHUT = "xshut"
CONF_XTALK = "xtalk"
CONF_ZONES = "zones"

Orientation = roode_ns.enum("Orientation")
ORIENTATION_VALUES = {
    "parallel": Orientation.Parallel,
    "perpendicular": Orientation.Perpendicular,
}

Ranging = roode_ns.namespace("Ranging")
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
roi_range = cv.int_range(min=4, max=16)


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


ROI_SCHEMA = cv.Any(
    NullableSchema(
        {
            cv.Optional(CONF_HEIGHT): roi_range,
            cv.Optional(CONF_WIDTH): roi_range,
            cv.Optional(CONF_CENTER): cv.uint8_t,
        },
    ),
    cv.one_of(CONF_AUTO),
)

THRESHOLDS_SCHEMA = NullableSchema(
    {
        cv.Optional(CONF_MIN): cv.Any(cv.uint16_t, cv.percentage),
        cv.Optional(CONF_MAX): cv.Any(cv.uint16_t, cv.percentage),
    }
)

ZONE_SCHEMA = NullableSchema(
    {
        cv.Optional(CONF_ROI, default={}): ROI_SCHEMA,
        cv.Optional(CONF_DETECTION_THRESHOLDS, default={}): THRESHOLDS_SCHEMA,
    }
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(Roode),
        cv.Optional(CONF_I2C_ADDRESS, default=0x29): cv.i2c_address,
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
        cv.Optional(CONF_ORIENTATION, default="parallel"): cv.enum(ORIENTATION_VALUES),
        cv.Optional(CONF_SAMPLING, default=2): cv.All(cv.uint8_t, cv.Range(min=1)),
        cv.Optional(CONF_ROI, default={}): ROI_SCHEMA,
        cv.Optional(CONF_DETECTION_THRESHOLDS, default={}): THRESHOLDS_SCHEMA,
        cv.Optional(CONF_ZONES, default={}): NullableSchema(
            {
                cv.Optional(CONF_INVERT, default=False): cv.boolean,
                cv.Optional(CONF_ENTRY_ZONE, default={}): ZONE_SCHEMA,
                cv.Optional(CONF_EXIT_ZONE, default={}): ZONE_SCHEMA,
            }
        ),
    }
)


async def to_code(config: Dict):
    cg.add_library("Wire", None)
    cg.add_library("rneurink", "1.2.3", "VL53L1X_ULD")

    roode = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(roode, config)

    await setup_hardware(config, roode)
    await setup_calibration(config[CONF_CALIBRATION], roode)
    await setup_algorithm(config, roode)


async def setup_hardware(config: Dict, roode: cg.Pvariable):
    cg.add(roode.set_i2c_address(config[CONF_I2C_ADDRESS]))
    pins = config[CONF_PINS]
    if CONF_INTERRUPT in pins:
        interrupt = await cg.gpio_pin_expression(pins[CONF_INTERRUPT])
        cg.add(roode.set_interrupt_pin(interrupt))
    if CONF_XSHUT in pins:
        xshut = await cg.gpio_pin_expression(pins[CONF_XSHUT])
        cg.add(roode.set_xshut_pin(xshut))


async def setup_calibration(config: Dict, roode: cg.Pvariable):
    if config.get(CONF_RANGING_MODE, CONF_AUTO) != CONF_AUTO:
        cg.add(roode.set_ranging_mode(config[CONF_RANGING_MODE]))
    if CONF_XTALK in config:
        cg.add(roode.set_sensor_xtalk_calibration(config[CONF_XTALK]))
    if CONF_OFFSET in config:
        cg.add(roode.set_sensor_offset_calibration(config[CONF_OFFSET]))


async def setup_algorithm(config: Dict, roode: cg.Pvariable):
    cg.add(roode.set_orientation(config[CONF_ORIENTATION]))
    cg.add(roode.set_sampling_size(config[CONF_SAMPLING]))
    cg.add(roode.set_invert_direction(config[CONF_ZONES][CONF_INVERT]))
    setup_zone(CONF_ENTRY_ZONE, config, roode)
    setup_zone(CONF_EXIT_ZONE, config, roode)


def setup_zone(name: str, config: Dict, roode: cg.Pvariable):
    zone_config = config[CONF_ZONES][name]
    zone_var = cg.MockObj(f"{roode}->{name}", "->")

    roi_var = cg.MockObj(f"{zone_var}->roi", "->")
    setup_roi(roi_var, zone_config.get(CONF_ROI, {}), config.get(CONF_ROI, {}))

    threshold_var = cg.MockObj(f"{zone_var}->threshold", "->")
    setup_thresholds(
        threshold_var,
        zone_config.get(CONF_DETECTION_THRESHOLDS, {}),
        config.get(CONF_DETECTION_THRESHOLDS, {}),
    )


def setup_roi(var: cg.MockObj, config: Union[Dict, str], fallback: Union[Dict, str]):
    config: Dict = (
        config if config != "auto" else {CONF_HEIGHT: "auto", CONF_WIDTH: "auto"}
    )
    fallback: Dict = (
        fallback if fallback != "auto" else {CONF_HEIGHT: "auto", CONF_WIDTH: "auto"}
    )
    height = config.get(CONF_HEIGHT, fallback.get(CONF_HEIGHT, 16))
    width = config.get(CONF_WIDTH, fallback.get(CONF_WIDTH, 6))
    if height != "auto":
        cg.add(var.set_height(height))
    if width != "auto":
        cg.add(var.set_width(width))
    if CONF_CENTER in config:
        cg.add(var.set_center(config[CONF_CENTER]))


def setup_thresholds(var: cg.MockObj, config: Dict, fallback: Dict):
    min = config.get(CONF_MIN, fallback.get(CONF_MIN, 0.0))
    max = config.get(CONF_MAX, fallback.get(CONF_MAX, 0.85))
    if isinstance(min, float):
        cg.add(var.set_min_percentage(int(min * 100)))
    else:
        cg.add(var.set_min(min))
    if isinstance(max, float):
        cg.add(var.set_max_percentage(int(max * 100)))
    else:
        cg.add(var.set_max(max))
