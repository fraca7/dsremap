
/**
 * @file AuthChallenge.h
 */

/**********************************************************************

  Created: 12 août 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _AUTHCHALLENGE_H
#define _AUTHCHALLENGE_H

#include <array>
#include <vector>
#include <cstdint>

#include <src/utils/Logger.h>

namespace dsremap
{
  class Proxy;
  class ControlEndpoint;

  class AuthChallenge : public Logger
  {
  public:
    AuthChallenge(Proxy&);

    void set_report(int id, const std::vector<uint8_t>& data);
    void get_report(ControlEndpoint&, int id);
    void on_bt_data(const std::vector<uint8_t>& data);

  private:
    enum class State {
      Idle,
      Waiting,
      Collecting,
      Ready
    };
    State _state;
    Proxy& _proxy;

    uint8_t _current_seq;
    int _current_rep;
    std::array<std::array<uint8_t, 64>, 19> _responses;
  };
}

#endif /* _AUTHCHALLENGE_H */
