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
    cv.Optional(CONF_ROI_HEIGHT, default=16):
    cv.int_range(min=4, max=16),
    cv.Optional(CONF_ROI_WIDTH, default=6):
    cv.int_range(min=4, max=16),
    cv.Optional(CONF_MAX_THRESHOLD_PERCENTAGE, default=85):
    cv.int_range(min=50, max=100),
    cv.Optional(CONF_MIN_THRESHOLD_PERCENTAGE, default=0):
    cv.int_range(min=0, max=100),
    cv.Optional(CONF_CALIBRATION, default='true'):
    cv.boolean,
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
    cv.Optional(CONF_SENSOR_MODE, default=-1):
    cv.int_range(min=-1, max=2),
}).extend(cv.polling_component_schema("100ms")))


async def setup_conf(config, key, hub):
    if key in config:
        cg.add(getattr(hub, f"set_{key}")(config[key]))


async def to_code(config):
    hub = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(hub, config)
    cg.add_library("EEPROM", None)
    cg.add_library("Wire", None)
    cg.add_library("pololu", "1.3.0", "VL53L1X")
    for key in TYPES:
        await setup_conf(config, key, hub)
