#include <iostream>
#include <Arduino.h>
#include <vector>

/*
pinMode(switchUpPin, INPUT_PULLUP)

extraPin0 -> Input
extraPin1 -> Input
extraPin2 -> Input
extraPin3 -> Input
extraPin4 -> Output Salida 4
extraPin5 -> Output Salida 3
extraPin6 -> Output Salida 2
extraPin7 -> Output Salida 1
*/

#define Input1 13
#define Input2 12
#define Input3 14
#define Input4 16
#define Output1 15

class inputController
{
private:
public:
    void inputSetup()
    {
        pinMode(Input1, INPUT_PULLUP);
        pinMode(Input2, INPUT_PULLUP);
        pinMode(Input3, INPUT_PULLUP);
        pinMode(Input4, INPUT_PULLUP);
        pinMode(Output1, OUTPUT);
    }
    std::vector<std::string> inputData()
    {
        std::vector<std::string> myVector;

        if (!digitalRead(Input1))
        {
            digitalWrite(Output1, HIGH);
            myVector.push_back("HIGH");
        }
        else
        {
            myVector.push_back("LOW");
        }
        if (!digitalRead(Input2))
        {
            digitalWrite(Output1, HIGH);
            myVector.push_back("HIGH");
        }
        else
        {
            myVector.push_back("LOW");
        }
        if (!digitalRead(Input3))
        {
            digitalWrite(Output1, HIGH);
            myVector.push_back("HIGH");
        }
        else
        {
            myVector.push_back("LOW");
        }
        if (!digitalRead(Input4))
        {
            digitalWrite(Output1, HIGH);
            myVector.push_back("HIGH");
        }
        else
        {
            myVector.push_back("LOW");
        }
        if (digitalRead(Input1) && digitalRead(Input2) && digitalRead(Input3) && digitalRead(Input4))
        {
            digitalWrite(Output1, LOW);
        }

        return myVector;

        myVector.clear();
    }
    void readInputs()
    {
        if (!digitalRead(Input1))
        {
            digitalWrite(Output1, HIGH);
        }
        if (!digitalRead(Input2))
        {
            digitalWrite(Output1, HIGH);
        }
        if (!digitalRead(Input3))
        {
            digitalWrite(Output1, HIGH);
        }
        if (!digitalRead(Input4))
        {
            digitalWrite(Output1, HIGH);
        }
        if (digitalRead(Input1) && digitalRead(Input2) && digitalRead(Input3) && digitalRead(Input4))
        {
            digitalWrite(Output1, LOW);
        }
    }
    std::string returnSingleInput(uint8_t customInput)
    {
        if (!digitalRead(customInput))
        {
            return "HIGH";
        }
        return "LOW";
    }
};