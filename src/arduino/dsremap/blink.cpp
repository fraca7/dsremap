
#include "blink.h"

Blink::Blink()
  : m_Count(0),
    m_State(State::Off),
    m_Counter(0)
{
}

void Blink::SetBlinkCount(uint8_t count)
{
  m_Count = count;
  m_Counter = count;

  if (count) {
    digitalWrite(LED_BUILTIN, HIGH);
    m_State = State::On;
    m_Last = millis();
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void Blink::loop()
{
  if (m_Count) {
    uint32_t now = millis();

    switch (m_State) {
      case State::On:
        if (now - m_Last >= 100) {
          m_State = (--m_Counter == 0) ? State::Pause : State::Off;
          digitalWrite(LED_BUILTIN, LOW);
          m_Last = now;
        }
        break;
      case State::Off:
        if (now - m_Last >= 100) {
          m_State = State::On;
          digitalWrite(LED_BUILTIN, HIGH);
          m_Last = now;
        }
        break;
      case State::Pause:
        if (now - m_Last >= 1000) {
          m_State = State::On;
          m_Counter = m_Count;
          digitalWrite(LED_BUILTIN, HIGH);
          m_Last = now;
        }
        break;
    }
  }
}
