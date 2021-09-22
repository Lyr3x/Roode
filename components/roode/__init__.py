import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import CONF_ID, STATE_CLASS_MEASUREMENT, UNIT_EMPTY, UNIT_METER

DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["sensor", "binary_sensor", "text_sensor"]
MULTI_CONF = True

CONF_ROODE_ID = "roode_id"

roode_ns = cg.esphome_ns.namespace("roode")
Roode = roode_ns.class_("Roode", cg.PollingComponent, i2c.I2CDevice)

CONF_ROI_HEIGHT= 'roi_height'
CONF_ROI_WIDTH = 'roi_width'
CONF_CALIBRATION = "calibration"
CONF_ROI_CALIBRATION = "roi_calibration"
CONF_INVERT_DIRECTION = "invert_direction"
CONF_THRESHOLD_PERCENTAGE = "threshold_percentage"
CONF_RESTORE_VALUES = "restore_values"
SETTERS = {
    CONF_THRESHOLD_PERCENTAGE: "set_threshold_percentage",
    CONF_ROI_HEIGHT: 'set_roi_height',
    CONF_ROI_WIDTH: 'set_roi_width',
    CONF_RESTORE_VALUES: 'set_restore_values',
    CONF_INVERT_DIRECTION: 'set_invert_direction', 
}
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Roode),
            cv.Optional(CONF_ROI_HEIGHT, default=16): cv.int_range(min=4, max=16),
            cv.Optional(CONF_ROI_WIDTH, default=6): cv.int_range(min=4, max=16),
            cv.Optional(CONF_THRESHOLD_PERCENTAGE, default=85): cv.int_range(min=50, max=100),
            cv.Optional(CONF_CALIBRATION, default='true'): cv.boolean,
            cv.Optional(CONF_ROI_CALIBRATION, default='false'): cv.boolean,
            cv.Optional(CONF_INVERT_DIRECTION, default='false'): cv.boolean,
            cv.Optional(CONF_RESTORE_VALUES, default='false'): cv.boolean,
        }
    )
    .extend(cv.polling_component_schema("100ms"))
    .extend(i2c.i2c_device_schema(0x29))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    for key, setter in SETTERS.items():
        if key in config:
            cg.add(getattr(var, setter)(config[key]))
    