import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import (
    CONF_DEVICE_CLASS,
    DEVICE_CLASS_OCCUPANCY,

)
from . import Roode, CONF_ROODE_ID

DEPENDENCIES = ["roode"]

CONF_PRESENCE = 'presence_sensor'

CONFIG_SCHEMA = binary_sensor.BINARY_SENSOR_SCHEMA.extend(
    {
        cv.Optional(
            CONF_DEVICE_CLASS, default=DEVICE_CLASS_OCCUPANCY
        ): binary_sensor.device_class,
        cv.GenerateID(CONF_ROODE_ID): cv.use_id(Roode),
    }
)

async def to_code(config):
    var = await cg.get_variable(config[CONF_ROODE_ID])
    presence = await binary_sensor.new_binary_sensor(config[CONF_PRESENCE])
    cg.add(var.set_presence_sensor(presence))
