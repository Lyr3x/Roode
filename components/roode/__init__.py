import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID, STATE_CLASS_MEASUREMENT, UNIT_EMPTY, UNIT_METER

# DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["sensor", "binary_sensor", "text_sensor"]
MULTI_CONF = True

CONF_ROODE_ID = "roode_id"

roode_ns = cg.esphome_ns.namespace("roode")
Roode = roode_ns.class_("Roode", cg.PollingComponent)

CONF_ROI_HEIGHT = 'roi_height'
CONF_ROI_WIDTH = 'roi_width'
CONF_ADVISED_SENSOR_ORIENTATION = 'advised_sensor_orientation'
CONF_CALIBRATION = "calibration"
CONF_ROI_CALIBRATION = "roi_calibration"
CONF_INVERT_DIRECTION = "invert_direction"
CONF_MAX_THRESHOLD_PERCENTAGE = "max_threshold_percentage"
CONF_MIN_THRESHOLD_PERCENTAGE = "min_threshold_percentage"
CONF_THRESHOLD_PERCENTAGE = "threshold_percentage"
CONF_RESTORE_VALUES = "restore_values"
CONF_I2C_ADDRESS = "i2c_address"
CONF_SENSOR_MODE = "sensor_mode"

TYPES = [
    CONF_MAX_THRESHOLD_PERCENTAGE, CONF_MIN_THRESHOLD_PERCENTAGE,
    CONF_ROI_HEIGHT, CONF_ROI_WIDTH, CONF_RESTORE_VALUES,
    CONF_INVERT_DIRECTION, CONF_ADVISED_SENSOR_ORIENTATION, CONF_CALIBRATION,
    CONF_ROI_CALIBRATION, CONF_I2C_ADDRESS, CONF_SENSOR_MODE
]
CONFIG_SCHEMA = (cv.Schema({
    cv.GenerateID():
    cv.declare_id(Roode),
    cv.Exclusive(
        CONF_CALIBRATION,
        "calibration",
        f"{CONF_CALIBRATION} and {CONF_SENSOR_MODE} can't be used together",
    ):
    cv.boolean,
    cv.Exclusive(
        CONF_SENSOR_MODE,
        "calibration",
        f"{CONF_CALIBRATION} and {CONF_SENSOR_MODE} can't be used together",
    ):
    cv.int_range(min=-1, max=2),
    cv.Inclusive(
        CONF_ROI_HEIGHT,
        "roi_size",
        f"{CONF_ROI_HEIGHT} and {CONF_ROI_WIDTH} must both be set",
    ):
    cv.int_range(min=4, max=16),
    cv.Inclusive(
        CONF_ROI_WIDTH,
        "roi_size",
        f"{CONF_ROI_HEIGHT} and {CONF_ROI_WIDTH} must both be set",
    ):
    cv.int_range(min=4, max=16),
    cv.Optional(CONF_MAX_THRESHOLD_PERCENTAGE, default=85):
    cv.int_range(min=50, max=100),
    cv.Optional(CONF_MIN_THRESHOLD_PERCENTAGE, default=0):
    cv.int_range(min=0, max=100),
    cv.Optional(CONF_ROI_CALIBRATION, default='false'):
    cv.boolean,
    cv.Optional(CONF_INVERT_DIRECTION, default='false'):
    cv.boolean,
    cv.Optional(CONF_RESTORE_VALUES, default='false'):
    cv.boolean,
    cv.Optional(CONF_ADVISED_SENSOR_ORIENTATION, default='true'):
    cv.boolean,
    cv.Optional(CONF_I2C_ADDRESS, default=0x29):
    cv.uint8_t,
}).extend(cv.polling_component_schema("100ms")))


def validate_roode(config):
    if CONF_CALIBRATION not in config and CONF_SENSOR_MODE not in config:
        raise cv.Invalid(
            f" '{CONF_ROI_HEIGHT}', '{CONF_ROI_WIDTH}' and '{CONF_SENSOR_MODE}'are required if '{CONF_CALIBRATION}:' isn't used"
        )
    if CONF_CALIBRATION not in config and CONF_ROI_CALIBRATION in config:
        raise cv.Invalid(
            f" {CONF_CALIBRATION} is a required property if '{CONF_ROI_CALIBRATION}' is used"
        )
    if CONF_CALIBRATION not in config and CONF_MAX_THRESHOLD_PERCENTAGE in config:
        raise cv.Invalid(
            f" {CONF_CALIBRATION} is a required property if '{CONF_MAX_THRESHOLD_PERCENTAGE}:' is configured"
        )
    if CONF_CALIBRATION not in config and CONF_MIN_THRESHOLD_PERCENTAGE in config:
        raise cv.Invalid(
            f" {CONF_CALIBRATION} is a required property if '{CONF_MIN_THRESHOLD_PERCENTAGE}:' is configured"
        )
    if config[CONF_CALIBRATION] == False and (CONF_ROI_HEIGHT not in config
                                              or CONF_ROI_WIDTH not in config):
        raise cv.Invalid(
            f" If {CONF_CALIBRATION} is set to false you need to specify '{CONF_ROI_HEIGHT}', '{CONF_ROI_WIDTH}' and '{CONF_SENSOR_MODE}'"
        )
    if config[CONF_CALIBRATION] == True and (CONF_ROI_HEIGHT in config
                                             or CONF_ROI_WIDTH in config
                                             or CONF_SENSOR_MODE in config):
        raise cv.Invalid(
            f" If {CONF_CALIBRATION} is set to true you cant specify '{CONF_ROI_HEIGHT}', '{CONF_ROI_WIDTH}' and '{CONF_SENSOR_MODE}'"
        )
    return config


async def setup_conf(config, key, hub):
    if key in config:
        cg.add(getattr(hub, f"set_{key}")(config[key]))


async def to_code(config):
    validate_roode(config)
    hub = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(hub, config)
    cg.add_library("EEPROM", None)
    cg.add_library("Wire", None)
    cg.add_library("pololu", "1.3.0", "VL53L1X")
    for key in TYPES:
        await setup_conf(config, key, hub)
