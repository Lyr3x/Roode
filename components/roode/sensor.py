import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    ICON_ARROW_EXPAND_VERTICAL,
    ICON_COUNTER,
    ICON_NEW_BOX,
    ICON_RULER,
    STATE_CLASS_MEASUREMENT,
    UNIT_EMPTY,
    ENTITY_CATEGORY_DIAGNOSTIC,
)
from . import Roode, CONF_ROODE_ID

DEPENDENCIES = ["roode"]

CONF_DISTANCE = "distance_sensor"
CONF_PEOPLE_COUNTER = "people_counter_sensor"
CONF_MAX_THRESHOLD_ZONE0 = "max_threshold_zone0"
CONF_MAX_THRESHOLD_ZONE1 = "max_threshold_zone1"
CONF_MIN_THRESHOLD_ZONE0 = "min_threshold_zone0"
CONF_MIN_THRESHOLD_ZONE1 = "min_threshold_zone1"
CONF_ROI_HEIGHT = "roi_height"
CONF_ROI_WIDTH = "roi_width"
CONFIG_SCHEMA = sensor.sensor_schema().extend(
    {
        cv.Optional(CONF_DISTANCE): sensor.sensor_schema(
            icon=ICON_RULER,
            unit_of_measurement=UNIT_EMPTY,
            accuracy_decimals=2,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_PEOPLE_COUNTER): sensor.sensor_schema(
            icon=ICON_COUNTER,
            unit_of_measurement=UNIT_EMPTY,
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_MAX_THRESHOLD_ZONE0): sensor.sensor_schema(
            icon="mdi:map-marker-distance",
            unit_of_measurement="mm",
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_MAX_THRESHOLD_ZONE1): sensor.sensor_schema(
            icon="mdi:map-marker-distance",
            unit_of_measurement="mm",
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_MIN_THRESHOLD_ZONE0): sensor.sensor_schema(
            icon="mdi:map-marker-distance",
            unit_of_measurement="mm",
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_MIN_THRESHOLD_ZONE1): sensor.sensor_schema(
            icon="mdi:map-marker-distance",
            unit_of_measurement="mm",
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_ROI_HEIGHT): sensor.sensor_schema(
            icon="mdi:table-row-height",
            unit_of_measurement="px",
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_ROI_WIDTH): sensor.sensor_schema(
            icon="mdi:table-column-width",
            unit_of_measurement="px",
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
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
    if CONF_MAX_THRESHOLD_ZONE0 in config:
        count = await sensor.new_sensor(config[CONF_MAX_THRESHOLD_ZONE0])
        cg.add(var.set_max_threshold_zone0_sensor(count))
    if CONF_MAX_THRESHOLD_ZONE1 in config:
        count = await sensor.new_sensor(config[CONF_MAX_THRESHOLD_ZONE1])
        cg.add(var.set_max_threshold_zone1_sensor(count))
    if CONF_MIN_THRESHOLD_ZONE0 in config:
        count = await sensor.new_sensor(config[CONF_MIN_THRESHOLD_ZONE0])
        cg.add(var.set_min_threshold_zone0_sensor(count))
    if CONF_MIN_THRESHOLD_ZONE1 in config:
        count = await sensor.new_sensor(config[CONF_MIN_THRESHOLD_ZONE1])
        cg.add(var.set_min_threshold_zone1_sensor(count))
    if CONF_ROI_HEIGHT in config:
        count = await sensor.new_sensor(config[CONF_ROI_HEIGHT])
        cg.add(var.set_roi_height_sensor(count))
    if CONF_ROI_WIDTH in config:
        count = await sensor.new_sensor(config[CONF_ROI_WIDTH])
        cg.add(var.set_roi_width_sensor(count))
