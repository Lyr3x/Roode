import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
import esphome.final_validate as fv
from esphome.const import (
    CONF_ID,
    CONF_DEVICE_CLASS,
    DEVICE_CLASS_OCCUPANCY,
)
from . import Roode, CONF_ROODE_ID

DEPENDENCIES = ["roode"]

CONF_PRESENCE = "presence_sensor"
TYPES = [CONF_PRESENCE]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ROODE_ID): cv.use_id(Roode),
        cv.Optional(CONF_PRESENCE): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
            }
        ),
    }
)


# def validate_can_use_presence(value):
#     main = fv.full_config.get()["roode"][0]
#     presence_sensor = main.get(CONF_USE_PRESENCE)
#     print(presence_sensor)
#     if presence_sensor == False:
#         raise cv.Invalid("Presence sensor is not enabled")
#     else:
#         return presence_sensor


# FINAL_VALIDATE_SCHEMA = cv.Schema(
#     {cv.Optional(CONF_PRESENCE): validate_can_use_presence}, extra=cv.ALLOW_EXTRA
# )


async def setup_conf(config, key, hub):
    if key in config:
        conf = config[key]
        sens = cg.new_Pvariable(conf[CONF_ID])
        await binary_sensor.register_binary_sensor(sens, conf)
        cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))


async def to_code(config):
    hub = await cg.get_variable(config[CONF_ROODE_ID])
    for key in TYPES:
        await setup_conf(config, key, hub)
