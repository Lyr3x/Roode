#include "Calibration.h"

int calculateStandardDeviation(int irValues[])
{
    auto sumOfValues = 0;
    auto arrLength = 0;
    for (int i = 0; i < 30; i++)
    {
        if (irValues[i] != 0)
        {
            sumOfValues += irValues[i];
            arrLength++;
        }
    }

    auto meanValue = sumOfValues / arrLength;

    auto standardDeviation = 0;
    for (int i = 0; i < arrLength; ++i)
    {
        standardDeviation += pow(irValues[i] - meanValue, 2);
    }
    standardDeviation /= arrLength;

    standardDeviation = sqrt(standardDeviation);

    return standardDeviation;
}