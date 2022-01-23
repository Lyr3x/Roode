from typing import Dict

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components.sensor import new_sensor, sensor_schema
from esphome.const import (
    ENTITY_CATEGORY_DIAGNOSTIC,
)
from . import VL53L1X, CONF_VL53L1X_ID

DEPENDENCIES = ["vl53l1x"]

CONF_ERROR = "error"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_VL53L1X_ID): cv.use_id(VL53L1X),
        cv.Optional(CONF_ERROR): sensor_schema(
            icon="mdi:alert-decagram",
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
)


async def to_code(config: Dict):
    var = await cg.get_variable(config[CONF_VL53L1X_ID])

    if CONF_ERROR in config:
        cg.add(var.set_error_sensor(await new_sensor(config[CONF_ERROR])))
