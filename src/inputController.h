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

#define Input1 16
#define Input2 14
#define Input3 12
#define Input4 13
#define Output1 15

class inputController{
    private:

    public:
        void inputSetup(){
            pinMode(Input1, INPUT_PULLUP);
            pinMode(Input2, INPUT_PULLUP);
            pinMode(Input3, INPUT_PULLUP);
            pinMode(Input4, INPUT_PULLUP);
            pinMode(Output1, OUTPUT);
        }
        std::vector<std::string> inputData(){
            std::vector<std::string> myVector;

            if(!digitalRead(Input1)){
                digitalWrite(Output1, HIGH);
                myVector.push_back("Input1");
                myVector.push_back("HIGH");
            }
            if(!digitalRead(Input2)){
                digitalWrite(Output1, HIGH);
                myVector.push_back("Input2");
                myVector.push_back("HIGH");
            }
            if(!digitalRead(Input3)){
                digitalWrite(Output1, HIGH);
                myVector.push_back("Input3");
                myVector.push_back("HIGH");
            }
            if(!digitalRead(Input4)){
                digitalWrite(Output1, HIGH);
                myVector.push_back("Input4");
                myVector.push_back("HIGH");
            }
            if(digitalRead(Input1) && digitalRead(Input2) && digitalRead(Input3) && digitalRead(Input4)){
                digitalWrite(Output1, LOW);
            }

            return myVector;

            myVector.clear();
        }
};