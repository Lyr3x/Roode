#include <Communication.h>

unsigned long SLEEP_TIME = 0; //sleep forever

void reportToController(int threshold = 0, int errorcode = -1)
{
    if (threshold > 8000)
    {
        String errorMsg = "Error code: " + errorcode;
        errorMsg += " , Out of range";
        send(thrMsg.set(errorMsg));
    }else if(errorcode == -1){
        String errorMsg = "Error code: " + errorcode;
        errorMsg += " , MASTER SELECT ERROR (I2C)";
        send(thrMsg.set(errorMsg));
    }else if(errorcode == -13){
        String errorMsg = "Error code: " + errorcode;
        errorMsg += " , NOT ENABLED (I2C)";
        send(thrMsg.set(errorMsg));
    }else if(errorcode == 65535){
        String errorMsg = "Error code: " + errorcode;
        errorMsg += " , TIMEOUT (VL53L0X)";
        send(thrMsg.set(errorMsg));
    }
}
