
Setup (Leonardo)
================

Once you have your Leonardo, the first thing to do is to upload the
firmware to it. The first time you launch the desktop application, a
wizard should appear to help you in this process. Later you can do
this by selecting "Update Arduino firmware" from the "Devices" menu.

.. note:: The initial image file is not distributed with the
          application, so the wizard will first download it. It's
          about 74Kb.

At this point you're probably still missing the external tool used to
program the Arduino (except on Linux, where it is embedded in the
AppImage), so the first page of the wizard will point this out.

.. image:: ../images/wizard-01.png
   :align: center

On mac OS, the less painful way to install avrdude is through Homebrew_. On Windows, I use WinAVR_.

.. _Homebrew: https://brew.sh
.. _WinAVR: https://sourceforge.net/projects/winavr/

Once this tool is installed, click the "Try to detect avrdude again" button and the wizard should proceed. Follow the onscreen instructions until the end.
