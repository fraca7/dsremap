
Setup (RPi)
===========

With the Raspberry Pi, you'll need a micro-SD card (4Gb minimum). The
first step is to burn the operating system image on this card. The
first time you launch the desktop application, a wizard will help you
set this up. Later you can create a new image by selecting "Setup a
new SD card for the RPi Zero W" from the "Devices" menu.

.. note:: The initial image file is not distributed with the
          application, so the wizard will first download it. It's
          about 564Mb.

The wizard will first ask you for your Wifi credentials. Those
credentials will be added to the disk image, so if you ever change
your Wifi SSID or password you'll have to create a new SD card.

.. image:: ../images/wizard-02.png
   :align: center

The image file will then be copied to your Downloads directory. You
can the use the `Raspberry Pi OS imager
<https://www.raspberrypi.org/software/>`_ to burn it to your SD card
(select the "Custom image" option).
