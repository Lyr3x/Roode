#include <Communication.h>

unsigned long SLEEP_TIME = 0; //sleep forever

void reportToController(int threshold = 0, int errorcode = -1)
{
    if (threshold > 8000)
    {
        String errorMsg = "Error code: " + errorcode;
        errorMsg += " , Out of range";
        send(thrMsg.set(errorMsg));
    }
}
