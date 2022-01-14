from typing import Dict, Union
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import (
    CONF_HEIGHT,
    CONF_ID,
    CONF_INVERT,
    CONF_SENSOR,
    CONF_WIDTH,
)
from ..vl53l1x import NullableSchema, VL53L1X

DEPENDENCIES = ["vl53l1x"]
AUTO_LOAD = ["vl53l1x", "sensor", "binary_sensor", "text_sensor", "number"]
MULTI_CONF = True

CONF_ROODE_ID = "roode_id"

roode_ns = cg.esphome_ns.namespace("roode")
Roode = roode_ns.class_("Roode", cg.PollingComponent)

CONF_AUTO = "auto"
CONF_ORIENTATION = "orientation"
CONF_DETECTION_THRESHOLDS = "detection_thresholds"
CONF_ENTRY_ZONE = "entry"
CONF_EXIT_ZONE = "exit"
CONF_CENTER = "center"
CONF_MAX = "max"
CONF_MIN = "min"
CONF_ROI = "roi"
CONF_SAMPLING = "sampling"
CONF_ZONES = "zones"

Orientation = roode_ns.enum("Orientation")
ORIENTATION_VALUES = {
    "parallel": Orientation.Parallel,
    "perpendicular": Orientation.Perpendicular,
}

roi_range = cv.int_range(min=4, max=16)


ROI_SCHEMA = cv.Any(
    NullableSchema(
        {
            cv.Optional(CONF_HEIGHT): roi_range,
            cv.Optional(CONF_WIDTH): roi_range,
            cv.Optional(CONF_CENTER): cv.uint8_t,
        },
    ),
    cv.one_of(CONF_AUTO),
)

THRESHOLDS_SCHEMA = NullableSchema(
    {
        cv.Optional(CONF_MIN): cv.Any(cv.uint16_t, cv.percentage),
        cv.Optional(CONF_MAX): cv.Any(cv.uint16_t, cv.percentage),
    }
)

ZONE_SCHEMA = NullableSchema(
    {
        cv.Optional(CONF_ROI, default={}): ROI_SCHEMA,
        cv.Optional(CONF_DETECTION_THRESHOLDS, default={}): THRESHOLDS_SCHEMA,
    }
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(Roode),
        cv.GenerateID(CONF_SENSOR): cv.use_id(VL53L1X),
        cv.Optional(CONF_ORIENTATION, default="parallel"): cv.enum(ORIENTATION_VALUES),
        cv.Optional(CONF_SAMPLING, default=2): cv.All(cv.uint8_t, cv.Range(min=1)),
        cv.Optional(CONF_ROI, default={}): ROI_SCHEMA,
        cv.Optional(CONF_DETECTION_THRESHOLDS, default={}): THRESHOLDS_SCHEMA,
        cv.Optional(CONF_ZONES, default={}): NullableSchema(
            {
                cv.Optional(CONF_INVERT, default=False): cv.boolean,
                cv.Optional(CONF_ENTRY_ZONE, default={}): ZONE_SCHEMA,
                cv.Optional(CONF_EXIT_ZONE, default={}): ZONE_SCHEMA,
            }
        ),
    }
)


async def to_code(config: Dict):
    roode = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(roode, config)

    sens = await cg.get_variable(config[CONF_SENSOR])
    cg.add(roode.set_tof_sensor(sens))

    cg.add(roode.set_orientation(config[CONF_ORIENTATION]))
    cg.add(roode.set_sampling_size(config[CONF_SAMPLING]))
    cg.add(roode.set_invert_direction(config[CONF_ZONES][CONF_INVERT]))
    setup_zone(CONF_ENTRY_ZONE, config, roode)
    setup_zone(CONF_EXIT_ZONE, config, roode)


def setup_zone(name: str, config: Dict, roode: cg.Pvariable):
    zone_config = config[CONF_ZONES][name]
    zone_var = cg.MockObj(f"{roode}->{name}", "->")

    roi_var = cg.MockObj(f"{zone_var}->roi_override", "->")
    setup_roi(roi_var, zone_config.get(CONF_ROI, {}), config.get(CONF_ROI, {}))

    threshold_var = cg.MockObj(f"{zone_var}->threshold", "->")
    setup_thresholds(
        threshold_var,
        zone_config.get(CONF_DETECTION_THRESHOLDS, {}),
        config.get(CONF_DETECTION_THRESHOLDS, {}),
    )


def setup_roi(var: cg.MockObj, config: Union[Dict, str], fallback: Union[Dict, str]):
    config: Dict = (
        config
        if config != CONF_AUTO
        else {CONF_HEIGHT: CONF_AUTO, CONF_WIDTH: CONF_AUTO}
    )
    fallback: Dict = (
        fallback
        if fallback != CONF_AUTO
        else {CONF_HEIGHT: CONF_AUTO, CONF_WIDTH: CONF_AUTO}
    )
    height = config.get(CONF_HEIGHT, fallback.get(CONF_HEIGHT, 16))
    width = config.get(CONF_WIDTH, fallback.get(CONF_WIDTH, 6))
    if height != CONF_AUTO:
        cg.add(var.set_height(height))
    if width != CONF_AUTO:
        cg.add(var.set_width(width))
    if CONF_CENTER in config:
        cg.add(var.set_center(config[CONF_CENTER]))


def setup_thresholds(var: cg.MockObj, config: Dict, fallback: Dict):
    min = config.get(CONF_MIN, fallback.get(CONF_MIN, 0.0))
    max = config.get(CONF_MAX, fallback.get(CONF_MAX, 0.85))
    if isinstance(min, float):
        cg.add(var.set_min_percentage(int(min * 100)))
    else:
        cg.add(var.set_min(min))
    if isinstance(max, float):
        cg.add(var.set_max_percentage(int(max * 100)))
    else:
        cg.add(var.set_max(max))
