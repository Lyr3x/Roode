// Sensor type assertion
#if defined(USE_VL53L0X) && defined(USE_VL53L1X) || defined(USE_VL53L0X) && defined(USE_SHARP_IR) || defined(USE_VL53L1X) && defined(USE_SHARP_IR)
#error You can only use one Sensor type at a time
#endif
#if defined(HIGH_SPEED) && defined(HIGH_ACCURACY)
#error You cant use HIGH_SPEED and HIGH_ACCURACY at the same time.
#endif
#ifndef CALIBRATION
#pragma message("Roode is mabye unreliable without calibration. Activate it with uncommenting: #define CALIBRATION")
#endif
#ifndef USE_MOTION
#pragma message("Roode cant receive messages from the controller without the Motion sensor due to continuous measuring")
#endif