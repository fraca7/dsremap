
#ifndef _BLINK_H
#define _BLINK_H

#include <Arduino.h>

class Blink
{
public:
  Blink();

  void SetBlinkCount(uint8_t);

  void loop();

private:
  uint8_t m_Count;
  enum class State {
    On,
    Off,
    Pause
  };
  State m_State;
  uint32_t m_Last;
  uint8_t m_Counter;
};

#endif /* _BLINK_H */
