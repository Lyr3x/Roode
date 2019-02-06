// Sensor type assertions
#if defined(USE_VL53L0X) && defined(USE_VL53L1X) || defined(USE_VL53L0X) && defined(USE_SHARP_IR) || defined(USE_VL53L1X) && defined(USE_SHARP_IR)
#error You can only use one Sensor type at a time
#endif
#if defined(HIGH_SPEED) && defined(HIGH_ACCURACY)
#error You cant use HIGH_SPEED and HIGH_ACCURACY at the same time.
#endif
#if CORRIDOR_SENSOR_newAddress == ROOM_SENSOR_newAddress
#error CORRIDOR_SENSOR and ROOM_SENSOR needs distinct I2C addresses
#endif
#if CORRIDOR_XSHUT == ROOM_XSHUT
#error CORRIDOR_SENSOR and ROOM_SENSOR needs distinct XSHUT pins
#endif
#ifndef CALIBRATION
#pragma message("Roode is mabye unreliable without calibration. Activate it with uncommenting: #define CALIBRATION")
#endif
#ifndef USE_MOTION
#pragma message("Roode cant receive messages from the controller without the Motion sensor due to continuous measuring")
#endif

//MySensors assertions

#if defined(MY_DEBUG)
#pragma message("Turn off the Debug mode in production! Misbehavior like freezes and reboots can occur!")
#endif
#if defined(MY_PA_LEVEL_HIGH) || defined(MY_PA_LEVEL_MAX)
#pragma message("Ensure that your Radio modul is getting enough power. Otherwise you may experience some issues.")
#endif
