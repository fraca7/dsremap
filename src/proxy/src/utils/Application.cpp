/**
 * @file Application.cpp
 */

/**********************************************************************

  Created: 07 août 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <stdexcept>

#include <glib-unix.h>

#include "Application.h"

namespace dsremap
{
  Application::Application()
    : _loop(g_main_loop_new(NULL, FALSE)),
      _state(State::Stopped),
      _components()
  {
    g_unix_signal_add(SIGINT, &Application::_signal_handler, static_cast<gpointer>(this));
  }

  Application::~Application()
  {
    g_main_loop_unref(_loop);
  }

  void Application::run()
  {
    switch (_state) {
      case State::Running:
        throw std::runtime_error("Already running");
      case State::Stopping:
        throw std::runtime_error("Try to run while stopping");
      case State::Stopped:
        _state = State::Running;
        g_main_loop_run(_loop);
        break;
    }

    _state = State::Stopped;
  }

  void Application::stop()
  {
    switch (_state) {
      case State::Running:
        if (_components.size() == 0) {
          g_main_loop_quit(_loop);
        } else {
          _state = State::Stopping;

          std::list<Component*> components(_components.begin(), _components.end());
          for (auto component : components)
            component->stop();
        }
        break;
      case State::Stopping:
        break;
      case State::Stopped:
        throw std::runtime_error("Already stopped");
    }
  }

  void Application::register_component(Component* component)
  {
    _components.push_back(component);
    if (_state == State::Stopping)
      component->stop();
  }

  void Application::unregister_component(Component* component)
  {
    _components.remove(component);
    if ((_state == State::Stopping) && (_components.size() == 0))
      g_main_loop_quit(_loop);
  }

  void Application::reconfigure()
  {
    for (auto& component : _components)
      component->reconfigure();
  }

  gboolean Application::_signal_handler(gpointer ptr)
  {
    static_cast<Application*>(ptr)->stop();

    return TRUE;
  }
}
