import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_TYPE, UNIT_EMPTY, ICON_COUNTER
from esphome.core import TimePeriod
from . import Roode, CONF_ROODE_ID
DEPENDENCIES = ['roode']

CONFIG_SCHEMA = sensor.sensor_schema(UNIT_EMPTY, ICON_COUNTER, 0).extend({
    cv.GenerateID(CONF_ROODE_ID): cv.use_id(Roode),
    
})

async def to_code(config):
    hub = await cg.get_variable(config[CONF_ROODE_ID])
    var = await sensor.new_sensor(config)
