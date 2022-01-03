from typing import Optional, OrderedDict

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import (
    CONF_ID,
    CONF_RESTORE_VALUE,
)

PersistedNumber = number.number_ns.class_("PersistedNumber", number.Number, cg.Component)

PERSISTED_NUMBER_SCHEMA = number.NUMBER_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(PersistedNumber),
    cv.Optional(CONF_RESTORE_VALUE, default=True): cv.boolean,
})


async def new_persisted_number(config: OrderedDict, min_value: float, max_value: float, step: Optional[float] = None):
    var = await number.new_number(config, min_value=min_value, max_value=max_value, step=step)
    await cg.register_component(var, config)
    if CONF_RESTORE_VALUE in config:
        cg.add(var.set_restore_value(config[CONF_RESTORE_VALUE]))
    return var
