
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

