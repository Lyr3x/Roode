from typing import OrderedDict

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_MAX_VALUE
from esphome.cpp_generator import MockObj

from components.persisted_number import PERSISTED_NUMBER_SCHEMA, new_persisted_number
from . import Roode, CONF_ROODE_ID

DEPENDENCIES = ["roode"]
AUTO_LOAD = ["number", "persisted_number"]

CONF_PEOPLE_COUNTER = "people_counter"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ROODE_ID): cv.use_id(Roode),
        cv.Optional(CONF_PEOPLE_COUNTER): PERSISTED_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_ICON, default="mdi:counter"): cv.icon,  # new default
                cv.Optional(CONF_MAX_VALUE, 10): cv.int_range(1, 255),
            }
        ),
    }
)


async def setup_people_counter(config: OrderedDict, hub: MockObj):
    counter = await new_persisted_number(
        config, min_value=0, step=1, max_value=config[CONF_MAX_VALUE]
    )
    cg.add(hub.set_people_counter(counter))


async def to_code(config: OrderedDict):
    hub = await cg.get_variable(config[CONF_ROODE_ID])
    if CONF_PEOPLE_COUNTER in config:
        await setup_people_counter(config[CONF_PEOPLE_COUNTER], hub)
