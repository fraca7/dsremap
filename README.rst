
DSRemap allows you to rewrite the Dualshock 4 inputs on the fly by
acting as man-in-the-middle between the controller and the PS4.

This allows you, for instance, to disable buttons, remap them, or even
add gyro aiming to a game that doesn't support it natively.

You'll need either an Arduino Leonardo (wired setup) or a Raspberry Pi
Zero W (Bluetooth setup) in addition to the software. See full
documentation at Readthedocs_.

.. _Readthedocs: https://dsremap.readthedocs.io/en/latest/

Changelog:

  * v1.2.0

    * The RPi0 now uses USB to communicate with the PS4

  * v1.1.0

    * Added support for Bluetooth through the RPi0W
    * Bug fixes in the VM (stack overflow)
