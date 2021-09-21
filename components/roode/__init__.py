import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import CONF_ID, STATE_CLASS_MEASUREMENT, UNIT_EMPTY, UNIT_METER

DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["sensor", "binary_sensor"]
MULTI_CONF = True

CONF_ROODE_ID = "roode_id"

roode_ns = cg.esphome_ns.namespace("roode")
Roode = roode_ns.class_("Roode", cg.PollingComponent, i2c.I2CDevice)

CONF_OPTICAL_CENTER = 'optical_center'
CONF_ROI_HEIGHT= 'roi_height'
CONF_ROI_WIDTH = 'roi_width'
CONF_DIST_THRESHOLD = 'distance_threshold'
CONF_CALIBRATION = "calibration"
CONF_ROI_CALIBRATION = "roi_calibration"
CONF_INVERT_DIRECTION = "invert_direction"
CONF_THRESHOLD_PERCENTAGE = "threshold_percentage"
SETTERS = {
    CONF_THRESHOLD_PERCENTAGE: "set_threshold_percentage",
    CONF_OPTICAL_CENTER: 'set_optical_center',
    CONF_ROI_HEIGHT: 'set_roi_height',
    CONF_ROI_WIDTH: 'set_roi_width',
}
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Roode),
            cv.Optional(CONF_ROI_HEIGHT, default=5): cv.int_range(min=0, max=255),
            cv.Optional(CONF_ROI_WIDTH, default=5): cv.int_range(min=0, max=255),
            cv.Optional(CONF_THRESHOLD_PERCENTAGE, default=85): cv.int_range(min=50, max=100),
            cv.Optional(CONF_CALIBRATION, default='true'): cv.boolean,
            cv.Optional(CONF_ROI_CALIBRATION, default='false'): cv.boolean,
            cv.Optional(CONF_DIST_THRESHOLD, default=[1500, 1500]): cv.All([cv.int_range(min=0, max=0xFFFF, max_included=False)], cv.Length(min=2, max=2)),
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
    