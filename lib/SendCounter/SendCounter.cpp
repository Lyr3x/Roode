#include <Configuration.h>
#include <SendCounter.h>
uint8_t peopleCount = 0; //default state: nobody is inside the room
void sendCounter(int inout)
{
    if (inout == 1)
    {
        peopleCount++;
        send(msg.set(inout));
        wait(100);
        send(pcMsg.set(peopleCount));
    }
    else if (inout == 0)
    {
        if (peopleCount > 0)
        {
            peopleCount--;
        }
        if (peopleCount == 0)
        {
            send(msg.set(inout));
        }
        wait(100);
        send(pcMsg.set(peopleCount));
    }

#ifdef USE_BATTERY
    float voltage = battery.checkBatteryLevel();
    send(voltage_msg.set(voltage, 3)); // redVcc returns millivolts. Set wants volts and how many decimals (3 in our case)
    sendBatteryLevel(round((voltage - BATTERY_ZERO) * 100.0 / (BATTERY_FULL - BATTERY_ZERO)));
#endif
#ifdef USE_OLED
    oled.clear();
    oled.setCursor(5, 0);
    oled.setTextSize(2, 1);
    oled.print("Counter: ");
    oled.println(peopleCount);
#endif
    Serial.print("PeopleCounter: ");
    Serial.println(peopleCount);
}