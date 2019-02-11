#ifndef SENDCOUNTER_H
#define SENDCOUNTER_H
// #include <Config.h>
#include <Transmitter.h>
int peopleCount = 0;
template <typename T>
void sendCounter(int inout, T transmitter)
{
    if (inout == 1)
    {
        peopleCount++;
        transmitter.transmit(transmitter.devices.peoplecounter, peopleCount);
        transmitter.transmit(transmitter.devices.room_switch, 1, "On");
    }
    else if (inout == 0)
    {
        if (peopleCount > 0)
        {
            peopleCount--;
            transmitter.transmit(transmitter.devices.peoplecounter, peopleCount);
        }
        if (peopleCount == 0)
        {
            transmitter.transmit(transmitter.devices.peoplecounter, peopleCount);
            transmitter.transmit(transmitter.devices.room_switch, 0, "Off");
        }
    }

#ifdef USE_BATTERY
    float voltage = battery.checkBatteryLevel();
    // send(voltage_msg.set(voltage, 3)); // redVcc returns millivolts. Set wants volts and how many decimals (3 in our case)
    sendBatteryLevel(round((voltage - BATTERY_ZERO) * 100.0 / (BATTERY_FULL - BATTERY_ZERO)));
#endif
#ifdef USE_OLED
    oled.clear();
    oled.setCursor(5, 0);
    oled.setTextSize(2, 1);
    oled.print("Counter: ");
    oled.println(peopleCount);
#endif
#ifdef USE_OLED_ASCII
  oled.clear();
  oled.setCursor(5, 0);
  oled.set2X();
  oled.print("Inside: ");
  oled.println(peopleCount);
#endif
    Serial.print(F("PeopleCounter: "));
    Serial.println(peopleCount);
}

#endif