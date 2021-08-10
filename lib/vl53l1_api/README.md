# ST VL53L1X API implementation for Arduino

This is an implementation of ST's [STSW-IMG007][api] VL53L1X API for the Arduino
platform, for use with the [Pololu VL53L1X time-of-flight distance sensor
carrier][product]. It includes an example sketch adapted from ST's
[X-CUBE-53L1A1][exp].

To load the sketch, open the file **vl53l1x-st-api.ino** in the vl53l1x-st-api
directory.

We have also released a more basic [VL53L1X library for Arduino][lib] that is
intended to provide a quicker and easier way to get started using the VL53L1X
with an Arduino-compatible controller, in contrast to using this API from ST.
The library has a more streamlined interface, as well as smaller storage and
memory footprints. However, it does not currently implement some of the more
advanced functionality available in the API (for example, calibrating the
sensor to work well under a cover glass or selecting a smaller region of
interest (ROI)), and it has less robust error checking.

[product]: https://www.pololu.com/product/3415
[api]: http://www.st.com/content/st_com/en/products/embedded-software/proximity-sensors-software/stsw-img007.html
[exp]: http://www.st.com/content/st_com/en/products/ecosystems/stm32-open-development-environment/stm32cube-expansion-software/stm32-ode-sense-sw/x-cube-53l1a1.html
[lib]: https://github.com/pololu/vl53l1x-arduino
