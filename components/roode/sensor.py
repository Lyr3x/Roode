import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import CONF_ID, UNIT_METER, ICON_ARROW_EXPAND_VERTICAL, CONF_INTERVAL
from esphome.core import TimePeriod

DEPENDENCIES = ['i2c']

vl53l1x_ns = cg.esphome_ns.namespace('roode')
Roode = vl53l1x_ns.class_('Roode', sensor.Sensor, cg.PollingComponent,
                                  i2c.I2CDevice)
vl53l1x_distance_mode = vl53l1x_ns.enum('vl53l1x_distance_mode')
vl53l1x_distance_modes = {
    'SHORT': vl53l1x_distance_mode.SHORT,
    'MEDIUM': vl53l1x_distance_mode.MEDIUM,
    'LONG': vl53l1x_distance_mode.LONG,
}

CONF_OPTICAL_CENTER = 'optical_center'
CONF_ROI_HEIGHT= 'roi_height'
CONF_ROI_WIDTH = 'roi_width'
CONF_DIST_THRESHOLD = 'distance_threshold'
CONF_CALIBRATION = "calibration"
CONF_ROI_CALIBRATION = "roi_calibration"
CONF_INVERT_DIRECTION = "invert_direction"
CONF_THRESHOLD_PERCENTAGE = "threshold_percentage"


CONFIG_SCHEMA = sensor.sensor_schema(UNIT_METER, ICON_ARROW_EXPAND_VERTICAL, 2).extend({
    cv.GenerateID(): cv.declare_id(Roode),
    # cv.Optional(CONF_TIMING_BUDGET, default='50ms'):
    #     cv.All(cv.positive_time_period_microseconds,
    #            cv.Range(min=TimePeriod(microseconds=20000),
    #                     max=TimePeriod(microseconds=1100000))),
    # cv.Optional(CONF_OPTICAL_CENTER, default=[239, 175]): cv.All([cv.int_range(min=0, max=255)], cv.Length(min=2, max=2)),
    cv.Optional(CONF_ROI_HEIGHT, default=5): cv.int_range(min=0, max=255),
    cv.Optional(CONF_ROI_WIDTH, default=5): cv.int_range(min=0, max=255),
    cv.Optional(CONF_THRESHOLD_PERCENTAGE, default=85): cv.int_range(min=50, max=100),
    cv.Optional(CONF_CALIBRATION, default='true'): cv.boolean,
    cv.Optional(CONF_DIST_THRESHOLD, default=[1500, 1500]): cv.All([cv.int_range(min=0, max=0xFFFF,
                                                            max_included=False)], cv.Length(min=2, max=2)),

}).extend(cv.polling_component_schema('100ms')).extend(i2c.i2c_device_schema(0x29))

SETTERS = {
    CONF_THRESHOLD_PERCENTAGE: "set_threshold_percentage",
    CONF_OPTICAL_CENTER: 'set_optical_center',
    CONF_ROI_HEIGHT: 'set_roi_height',
    CONF_ROI_WIDTH: 'set_roi_width',
}

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    for key, setter in SETTERS.items():
        if key in config:
            cg.add(getattr(var, setter)(config[key]))

    yield sensor.register_sensor(var, config)
    yield i2c.register_i2c_device(var, config)
 