# Home Assistant Integration

Roode integrates seamlessly with Home Assistant via ESPHome. All entities are created automatically, but here are some useful automations and configurations.

## Available Entities

After adding Roode to Home Assistant, you'll have access to:

| Entity Type | Name | Description |
|-------------|------|-------------|
| Number | People Counter | Current count (adjustable) |
| Binary Sensor | Presence | Someone in detection area |
| Text Sensor | Version | Roode firmware version |
| Text Sensor | Entry/Exit Event | Last event: "Entry", "Exit", or "Timeout" |
| Sensor | Distance Entry/Exit | Current distance readings |
| Sensor | Threshold Entry/Exit | Current min/max thresholds |
| Sensor | ROI Height/Width | Current ROI configuration |
| Sensor | Sensor Status | VL53L1X status code |

## Example Automations

### Reset Counter at Night

Reset the people counter when everyone should be out (or in bed):

```yaml
automation:
  - alias: "Reset people counter at midnight"
    trigger:
      - platform: time
        at: "00:00:00"
    condition:
      - condition: state
        entity_id: binary_sensor.roode32_presence
        state: "off"
    action:
      - service: number.set_value
        target:
          entity_id: number.roode32_people_counter
        data:
          value: 0
```

### Notify on Timeout Events

Get notified if there are frequent timeout events (might indicate calibration issues):

```yaml
automation:
  - alias: "Notify on frequent Roode timeouts"
    trigger:
      - platform: state
        entity_id: text_sensor.roode32_last_direction
        to: "Timeout"
    action:
      - service: counter.increment
        target:
          entity_id: counter.roode_timeout_count
      - condition: numeric_state
        entity_id: counter.roode_timeout_count
        above: 10
      - service: notify.mobile_app
        data:
          title: "Roode Sensor Alert"
          message: "High timeout count - consider recalibrating"
```

### Sync Counter with Input Number (Legacy)

If you prefer using an input_number slider to adjust the count:

```yaml
input_number:
  roode_people_count:
    name: "Room Occupancy"
    min: 0
    max: 20
    step: 1
    mode: slider

automation:
  - alias: "Sync Roode counter to slider"
    trigger:
      - platform: state
        entity_id: number.roode32_people_counter
    action:
      - service: input_number.set_value
        target:
          entity_id: input_number.roode_people_count
        data:
          value: "{{ states('number.roode32_people_counter') | int }}"

  - alias: "Sync slider to Roode counter"
    trigger:
      - platform: state
        entity_id: input_number.roode_people_count
    condition:
      - condition: template
        value_template: >
          {{ states('input_number.roode_people_count') | int !=
             states('number.roode32_people_counter') | int }}
    action:
      - service: number.set_value
        target:
          entity_id: number.roode32_people_counter
        data:
          value: "{{ states('input_number.roode_people_count') | int }}"
```

### Turn Off Lights When Room Empty

```yaml
automation:
  - alias: "Turn off lights when room empty"
    trigger:
      - platform: numeric_state
        entity_id: number.roode32_people_counter
        below: 1
        for:
          minutes: 5
    action:
      - service: light.turn_off
        target:
          entity_id: light.living_room
```

### Monitor Threshold Drift

Track if adaptive thresholds are drifting significantly:

```yaml
automation:
  - alias: "Alert on significant threshold change"
    trigger:
      - platform: state
        entity_id: sensor.roode32_max_zone_0
    condition:
      - condition: template
        value_template: >
          {{ (trigger.to_state.state | float - trigger.from_state.state | float) | abs > 100 }}
    action:
      - service: notify.mobile_app
        data:
          title: "Roode Threshold Alert"
          message: >
            Entry zone threshold changed from {{ trigger.from_state.state }}mm
            to {{ trigger.to_state.state }}mm
```

## ESPHome Services

Roode exposes a recalibration service that can be called from Home Assistant:

```yaml
# In Developer Tools > Services
service: esphome.roode32_recalibrate
```

Or in an automation:

```yaml
automation:
  - alias: "Recalibrate Roode daily"
    trigger:
      - platform: time
        at: "03:00:00"
    condition:
      - condition: state
        entity_id: binary_sensor.roode32_presence
        state: "off"
        for:
          minutes: 10
    action:
      - service: esphome.roode32_recalibrate
```

## Dashboard Cards

### Simple Counter Card

```yaml
type: entities
entities:
  - entity: number.roode32_people_counter
    name: People in Room
  - entity: binary_sensor.roode32_presence
    name: Motion Detected
  - entity: text_sensor.roode32_last_direction
    name: Last Event
```

### Detailed Sensor Card

```yaml
type: entities
title: Roode Sensor Details
entities:
  - entity: number.roode32_people_counter
  - entity: text_sensor.roode32_last_direction
  - type: section
    label: Zone 0 (Entry)
  - entity: sensor.roode32_distance_zone_0
  - entity: sensor.roode32_max_zone_0
  - entity: sensor.roode32_min_zone_0
  - type: section
    label: Zone 1 (Exit)
  - entity: sensor.roode32_distance_zone_1
  - entity: sensor.roode32_max_zone_1
  - entity: sensor.roode32_min_zone_1
  - type: section
    label: System
  - entity: text_sensor.roode32_version
  - entity: sensor.roode32_sensor_status
```
