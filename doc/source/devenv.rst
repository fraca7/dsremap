
Everything from here is intended for developers, not users.

Development environment
=======================

You will need

  * Python 3

    * mac OS: The version of Python shipped with mac OS should do but it's untested. I use Homebrew_.

  * Arduino. I use the latest (1.8.12 as of this writing). The build script expects this to be found

    * In /Applications/Arduino.app on mac OS
    * In ~/arduino on Linux
    * Anywhere on Windows (it's detected through the registry)

  * A compiler to build the C extension

    * On mac OS, install XCode and its command-line tools
    * On Linux, the `build-essential` and `python3-dev` packages should be enough
    * On Windows, Visual Studio Community Edition is free. Make sure to install the "C++ Desktop development" module.

.. _Homebrew: https://brew.sh

Python modules
--------------

Install the Python modules listed in requirements.txt at the root of the source tree, plus

  * `colorama` (for the log viewer utility)
  * `Sphinx` (to build the docs)
  * `pylint`

Arduino libraries
-----------------

Install the "USB Host Shield Library 2.0" library the usual way using the Arduino IDE. Unfortunately, this library includes some serial debugging code which is compiled even though it isn't used, and LUFA breaks this. So you must go to the library's installation directory, which should be

  * Windows: Documents/Arduino/libraries/USB_Host_Shield_Library_2.0
  * mac OS: ~/Documents/Arduino/libraries/USB_Host_Shield_Library_2.0
  * Linux: ~/Arduino/libraries/USB_Host_Shield_Library_2.0

and edit the `settings.h` file. Replace the following line

.. code-block:: cpp

   #define USB_HOST_SERIAL Serial

with

.. code-block:: cpp

   #define USB_HOST_SERIAL Serial1

Now install LUFA_. This may prove a pain in the ass on Windows; you must clone the library and run activate.py in an Administrator console. Likewise on mac OS you may need to use `sudo` to install stuff in the application bundle under `/Applications/Arduino.app/Contents/Java/libraries`. No problem on Linux if you extracted the IDE under ~/arduino.

.. _LUFA: https://github.com/Palatis/Arduino-Lufa
