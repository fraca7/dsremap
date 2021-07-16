
Building the code
=================

At the top-level of the source tree you'll find a `make.py` script which basically acts as a cross-platform makefile. Invoke it with a target name (without any argument, `all` is assumed). Targets are

  * `headers`: Generate .h files (log and opcode constants)
  * `resources`: Generate resources.py (embedded icons)
  * `messages`: Generate the messages.pot file for translations
  * `ext`: Build the C extension module
  * `firmware`: Build the Arduino sketch
  * `manual`: Build the documentation
  * `lint`: Run pylint on the Python code
  * `regtests`: Run the regression tests
  * `unittests`: Run the unit tests
  * `exe`: Build the executable bundle/installer/whatever

  * `prepare`: Everything needed to run `dsremap.py` on the command line, ie `resources` and `ext`
  * `alltests`: Regression and unit tests
  * `all`: ALL THE THINGS. Mostly.

Preparing the Rpi0 image
------------------------

.. note:: This must be done on the Pi itself.

First download and unzip the raspios image file. You'll need the "Raspberry Pi OS", Lite version.

Second, build the "installer" for the service and utilities; you have to generate the header files first, so:

.. code-block:: none

   ./make.py headers
   cd src/server
   make bundle

This will generate an "installer" file named dsremap-service-<version>.sh.

The "prepare_image.sh" script in src/rpi0 takes care of everything from there on hopefully; from installing the required system dependencies to configuring what needs to. It must be run as root (through sudo). Invoke it like:

.. code-block:: none

   sudo ./prepare_image.sh <path_to_image_file> <path_to_installer>

Example:

.. code-block:: none

   sudo ./prepare_image.sh ../../../2020-12-02-raspios-buster-armhf-lite.img ../server/dsremap-service-1.0.0.sh
