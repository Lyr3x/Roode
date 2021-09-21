# import esphome.codegen as cg
# import esphome.config_validation as cv
# from esphome.components import sensor
# from esphome.const import (
#     UNIT_EMPTY,
#     STATE_CLASS_MEASUREMENT,
# )
# from . import Roode, CONF_ROODE_ID
# DEPENDENCIES = ['roode']
# CONF_DISTANCE = 'distance_sensor'
# roode_ns = cg.esphome_ns.namespace("roode")

# CONFIG_SCHEMA = (
#     cv.Schema(
#         {
#             cv.Optional(CONF_DISTANCE): sensor.sensor_schema(
#                 unit_of_measurement=UNIT_EMPTY,
#                 accuracy_decimals=0,
#                 state_class=STATE_CLASS_MEASUREMENT,
#             ),
#         }
#     ).extend({
#     cv.GenerateID(CONF_ROODE_ID): cv.use_id(Roode),
# })
# )


# async def to_code(config):
#     hub = await cg.get_variable(config[CONF_ROODE_ID])
#     var = await sensor.new_sensor(config)
#     if CONF_DISTANCE in config:
#         sens = await sensor.new_sensor(config[CONF_DISTANCE])

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_TYPE,
    STATE_CLASS_MEASUREMENT,
    UNIT_EMPTY,
    UNIT_METER,
    UNIT_PERCENT,
    ICON_LIGHTBULB,
)
from . import Roode, CONF_ROODE_ID

DEPENDENCIES = ["roode"]

CONF_DISTANCE = 'distance_sensor'
CONF_PEOPLE_COUNTER = 'people_counter_sensor'
CONFIG_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_METER,
    icon=ICON_LIGHTBULB,
    accuracy_decimals=1,
    state_class=STATE_CLASS_MEASUREMENT,
).extend(
    {
        cv.Optional(CONF_DISTANCE): sensor.sensor_schema(
                unit_of_measurement=UNIT_EMPTY,
                accuracy_decimals=2,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        cv.Optional(CONF_PEOPLE_COUNTER): sensor.sensor_schema(
                unit_of_measurement=UNIT_EMPTY,
                accuracy_decimals=0,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        cv.GenerateID(CONF_ROODE_ID): cv.use_id(Roode),
    }
)


async def to_code(config):
    var = await cg.get_variable(config[CONF_ROODE_ID])
    if CONF_DISTANCE in config:
        distance = await sensor.new_sensor(config[CONF_DISTANCE])
        cg.add(var.set_distance_sensor(distance))
    if CONF_PEOPLE_COUNTER in config:
        count = await sensor.new_sensor(config[CONF_PEOPLE_COUNTER])
        cg.add(var.set_people_counter_sensor(count))