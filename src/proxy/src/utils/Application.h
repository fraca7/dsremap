
/**
 * @file Application.h
 */

/**********************************************************************

  Created: 07 août 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <list>

#include <glib.h>

namespace dsremap
{
  /**
   * Class that hosts the main loop
   */
  class Application
  {
  public:
    enum class State {
      Running,
      Stopping,
      Stopped
    };

    /**
     * Some parts of the program must be stopped asynchronously; they
     * should derive from this and get deleted when actually stopped.
     */
    class Component
    {
    public:
      Component(Application& app)
        : _app(app) {
        _app.register_component(this);
      }

      virtual ~Component() {
        _app.unregister_component(this);
      }

      /**
       * You should initiate the component's shutdown from this
       * method, and delete the object when it's safe to do so.
       */
      virtual void stop() = 0;

      /**
       * Called when configuration changes; reload it
       */
      virtual void reconfigure() = 0;

    protected:
      Application& application() {
        return _app;
      }

    private:
      Application& _app;
    };

    class ErrorHandler {
    public:
      virtual void on_error(std::exception_ptr) = 0;
    };

    Application();
    ~Application();

    /**
     * Runs the main loop
     */
    void run();

    /**
     * Stops registered components. The main loop will only exit once
     * all components are stopped and deleted.
     */
    void stop();

    /**
     * Get current state
     */
    State state() const {
      return _state;
    }

    /**
     * Reconfigure all registered components
     */
    void reconfigure();

  private:
    GMainLoop* _loop;
    State _state;
    std::list<Component*> _components;

    void register_component(Component*);
    void unregister_component(Component*);

    static gboolean _signal_handler(gpointer);

    friend class Component;
  };
}

#endif /* _APPLICATION_H */
