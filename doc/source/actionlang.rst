
Action language
===============

Actions (the building bricks of a configuration) are actually written in a simple programming language reminiscent of C++. After enabling "Developer mode" in the preferences, you'll be able to create custom actions, for which you'll have to write your own logic, and convert builtin actions to custom ones. This is a very short overview of this language.

Builtin types
-------------

The only builtin types are `int` (signed, 32 bits) and `float`.

Builtin global variables
------------------------

There are builtin global variables for the current input report's state. Boolean variables (like buttons) are actually integer ones, with values 0 or not 0 for false/true. Those variables are:

  * `LpadX`, `RPadX`, `LPadY`, `RPadY`: Analog pad values, 0..255
  * `Hat`: DPad state. 8 means released; 0 means up, 1 up+right, 2 right, etc, in clockwise order
  * `Square`, `Cross`, `Circle`, `Triangle`, `L1`, `L2`, `L3`, `R1`, `R2`, `R3`, `Share`, `Options`, `TPad`, `PS` are the buttons states
  * `L2Value`, `R2Value` are the trigger values, 0..255
  * `IMUX`, `IMUY`, `IMUZ` (read-only) are the Euler angles for the current pad orientation, in degrees.
  * `DELTA` is the number of milliseconds elapsed since the last input report

Structures
----------

You can define structured types, with methods:

.. code-block:: c++

   struct Point {
     int x;
     int y;

     void init() {
       x = 0;
       y = 0;
     }

     void advance() {
       x += 10;
       y += 10;
     }
   };

   Point startPoint;

They behave just like C++ classes. There is no constructor/destructor; initial values for members are undetermined.

Functions
---------

You can declare functions just like in C

.. code-block:: c++

   int addup(int a, int b) {
     return a + b;
   }

.. note:: Functions cannot be forward declared. This is by design. It prevents recursivity, which is desirable because the stack size must be computed at compile time.

States
------

States are special objects, not exactly types. The program must be in a single state at any point in time. The default state (entry point) is named `idle`. Any state may have

  * An `enter` method, which will be executed when entering the state
  * A method named after the state itself, which will be executed on each new input report

States may have members (with the same lifetime as the state itself). You can change state using the `go` keyword.

.. code-block:: c++

   state idle {
     idle() {
       if (L2 && R2)
         go pressing;
     }
   };

   state pressing {
     int count;

     enter() {
       count = 0;
     }

     pressing() {
       count++;
       if (!(L2 && R2))
         go idle;
     }
   };

Note that state "methods" do not have arguments or return values.

Flow control
------------

A subset of the usual flow control instructions is available: `if` / `else` and `while`. Two flow control instructions are particular:

  * `go` is used to change the current state
  * `yield` is used to wait for the next input report

A state's main method contains an implicit `yield` at the end, so

.. code-block:: c++

   state idle {
     idle() {
       // do something
     }
   };

is actually equivalent to

.. code-block:: c++

   state idle {
     idle() {
       while (1) {
         // do something
	 yield;
       }
     }
   };

Example
-------

Here is a program that swaps the right and left pads:

.. code-block:: c++

   state idle {
     idle() {
       int tmp = LPadX;
       LPadX = RPadX;
       RPadX = tmp;
       tmp = LPadY;
       LPadY = RPadY;
       RPadY = tmp;
     }
   };

Here is the full code for gyro aiming on the right pad when pressing L2 and R2 (as used in the builtin Horizon: Zero Dawn configuration):

.. code-block:: c++

  #define FPS 30
  #define PPD_X (1920 / 10)
  #define PPD_Y (1080 / 10)
  #define DEADZONE 50
  
  #define MAX_SPEED 4.4
  #define MIN_SPEED MAX_SPEED / 8
  
  struct AxisState {
    float origin;
    float current;
    float target;
    float speed;
    int padval;
  
    void init(float angle) {
      origin = angle;
      current = 0;
      target = 0;
      speed = 0;
      padval = 0x80;
    }
  
    void integrate() {
      current = current + speed * DELTA;
    }
  
    void compute(float angle, int ppd) {
      target = 1000.0 * (angle - origin) * ppd;
  
      float delta = target - current;
      int sign = 1;
      if (delta < 0) {
        delta = -delta;
        sign = -1;
      }
  
      if (delta < 1) {
        speed = 0;
      } else {
        speed = FPS * delta / 1000000.0;
        if (speed > MAX_SPEED)
          speed = MAX_SPEED;
        if (speed < MIN_SPEED)
          speed = MIN_SPEED;
      }
  
      speed = sign * speed;
      padval = 128 * speed / MAX_SPEED + 0x80;
    }
  };
  
  int rpad_delta() {
    return (RPadX - 128) * (RPadX - 128) + (RPadY - 128) * (RPadY - 128);
  }
  
  int should_aim() {
    return L2 && R2;
  }
  
  state idle {
    idle() {
      if (should_aim()) {
        if (rpad_delta() < DEADZONE) {
          go gyro_aiming;
        } else {
          go manual_aiming;
        }
      }
    }
  };
  
  state manual_aiming {
    manual_aiming() {
      if (!should_aim()) {
        go idle;
      }
  
      if (rpad_delta() < DEADZONE) {
        go gyro_aiming;
      }
    }
  };
  
  state gyro_aiming {
    AxisState stateX;
    AxisState stateY;
  
    enter() {
      stateX.init(IMUY);
      stateY.init(IMUX);
    }
  
    gyro_aiming() {
      if (!should_aim()) {
        go idle;
      }
  
      if (rpad_delta() >= DEADZONE) {
        go manual_aiming;
      }
  
      stateX.compute(IMUY, PPD_X);
      stateY.compute(IMUX, PPD_Y);
  
      int count = 0;
      while (count++ * 5 <= 1000 / FPS) {
        stateX.integrate();
        RPadX = stateX.padval;
  
        stateY.integrate();
        RPadY = stateY.padval;
  
        yield;
      }
    }
  };

Debugging
---------

Right now there is no debugging options, so you're probably in for a world of pain. Compilation error reporting is very rough and the messages are unreadable for anyone who didn't write the parser or spent time studying it. A future version may include a symbolic debugger and simulation options.
