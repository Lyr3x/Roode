import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import CONF_ID, CONF_ICON, CONF_ENTITY_CATEGORY, ENTITY_CATEGORY_DIAGNOSTIC
from . import Roode, CONF_ROODE_ID

DEPENDENCIES = ["roode"]

VERSION = "version"
ENTRY_EXIT_EVENT = "entry_exit_event"

TYPES = [VERSION, ENTRY_EXIT_EVENT]

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_ROODE_ID):
    cv.use_id(Roode),
    cv.Optional(VERSION):
    text_sensor.TEXT_SENSOR_SCHEMA.extend({
        cv.Optional(CONF_ICON, default="mdi:git"):
        text_sensor.icon,
        cv.GenerateID():
        cv.declare_id(text_sensor.TextSensor),
        cv.Optional(CONF_ENTITY_CATEGORY, default=ENTITY_CATEGORY_DIAGNOSTIC):
        cv.entity_category,
    }),
    cv.Optional(ENTRY_EXIT_EVENT):
    text_sensor.TEXT_SENSOR_SCHEMA.extend({
        cv.Optional(CONF_ICON, default="mdi:sign-direction"):
        text_sensor.icon,
        cv.GenerateID():
        cv.declare_id(text_sensor.TextSensor),
        cv.Optional(CONF_ENTITY_CATEGORY, default=ENTITY_CATEGORY_DIAGNOSTIC):
        cv.entity_category,
    }),
})


async def setup_conf(config, key, hub):
    if key in config:
        conf = config[key]
        sens = cg.new_Pvariable(conf[CONF_ID])
        await text_sensor.register_text_sensor(sens, conf)
        cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))


async def to_code(config):
    hub = await cg.get_variable(config[CONF_ROODE_ID])
    for key in TYPES:
        await setup_conf(config, key, hub)