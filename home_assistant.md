To make all functions of Roode work with home assistant you need to set up a few entities and automations. 
Roode has endpoints to set the count value, reset the counter to 0 and to recalibrate. Unfourtunately its not possible to expose buttons via ESPHome that do just that.

```
# This automation script runs when a value is received via MQTT on retained topic: setTemperature
# It sets the value slider on the GUI. This slides also had its own automation when the value is changed.
- alias: "Set people slider"
  trigger:
    platform: mqtt
    topic: "roode/sensor/people_set_counter/state"
  action:
    service: input_number.set_value
    target:
      entity_id: input_number.set_people
    data:
      value: "{{ trigger.payload }}"
- alias: "people slider moved"
  trigger:
    platform: state
    entity_id: input_number.set_people
  action:
    service: mqtt.publish
    data:
      topic: "roode/sensor/people/set"
      retain: true
      payload: "{{ states('input_number.set_people') | int }}"
```