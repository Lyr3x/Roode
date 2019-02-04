#include <Communication.h>

unsigned long SLEEP_TIME = 0; //sleep forever

void reportToController(int sensor_value)
{
    if (sensor_value > 8000)
    {
        String errorMsg = "Error code: " + sensor_value;
        errorMsg += " , Out of range";
        send(thrMsg.set(errorMsg));
    }else if(sensor_value == -1){
        String errorMsg = "Error code: " + sensor_value;
        errorMsg += " , MASTER SELECT ERROR (I2C)";
        send(thrMsg.set(errorMsg));
    }else if(sensor_value == -13){
        String errorMsg = "Error code: " + sensor_value;
        errorMsg += " , NOT ENABLED (I2C)";
        send(thrMsg.set(errorMsg));
    }else if(sensor_value == 65535){
        String errorMsg = "Error code: " + sensor_value;
        errorMsg += " , TIMEOUT (VL53L0X)";
        send(thrMsg.set(errorMsg));
    }
}
