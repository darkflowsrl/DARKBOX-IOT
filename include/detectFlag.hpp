#ifndef DETECTFLAG_H
#define DETECTFLAG_H

#include "Arduino.h"

int debounce(int pin);

class DetectaFlanco
{
private:
  int _pin;
  boolean _anterior_estado;
  boolean _estado;

public:
  DetectaFlanco(int pin)
  {
    _pin = pin;
  };
  void inicio(int input)
  {
    pinMode(_pin, input);
    _anterior_estado = debounce(_pin);
  }; // INPUT or INPUT_PULLUP

  int comprueba()
  {
    _estado = debounce(_pin);

    if (_anterior_estado != _estado)
    {
      if (_estado == HIGH)
      {
        _anterior_estado = _estado;
        return -1; // Flanco Ascendente
      }
      else
      {
        _anterior_estado = _estado;
        return 1; // Flanco Descendente
      }
    }
    else
      return 0;
  };
};


int debounce(int pin)
{
    uint8 counter = 0;

    int state = digitalRead(pin);

    do
    {
        counter++;
        delay(1);
    } while (counter < 50);

    return state;
}
#endif