
#ifndef MESSAGES_H
#define MESSAGES_H

/* Common */

#define USB_INIT_FAILED 0 // USB initialization failed

/* Descriptors.cpp */

#define ERROR_READ_FROM_HOST 1 // Error reading from host: 0x%02x
#define RECV_FROM_HOST 2 // Received %d bytes from host
#define USB_CONN 3 // USB connected
#define USB_DISCONN 4 // USB disconnected
#define USB_CONF_CHANGED 5 // Configuration changed
#define WARN_MIN_UNK_ENTITY 6 // GetMinimum for unknown entity %d
#define WARN_MAX_UNK_ENTITY 7 // GetMaximum for unknown entity %d
#define WARN_RES_UNK_ENTITY 8 // GetResolution for unknown entity %d
#define WARN_UNHANDLED_REQ 9 // Unhandled request
#define SEND_DEV_DES 10 // Sending device descriptor
#define SEND_CONF_DES 11 // Sending configuration descriptor
#define SEND_HID_DES 12 // Sending HID descriptor
#define SEND_HID_REP_DES 13 // Sending HID report descriptor

/* Host.cpp */

#define REPORT_COUNT 14 // %d reports in %d ms
#define LOST_REPORT 15 // Lost report
#define GOT_CALIB 16 // Got calibration data
#define GOT_REPORT_DATA_R 17 // Got RAM report data
#define GOT_REPORT_DATA_F 18 // Got Flash report data
#define ERROR_UNSUPPORTED_MEMSPACE 19 // Unsupported memory space
#define LOOP_COUNT 20 // %d loops in %d ms
#define ERROR_SEND_REPORT_01 21 // Error sending main report: 0x%02x
#define INFO_GET_REPORT 22 // GetReport type=0x%02x, ID=0x%02x
#define INFO_SET_REPORT 23 // SetReport type=0x%02x; id=0x%02x, len=%d
#define SEND_OUT_EP 24 // Send %d bytes on OUT endpoint
#define SEND_SET_REPORT 25 // SetReport: send %d bytes
#define WARN_OOB_REPORT_DATA 26 // Got unexpected report data from device: 0x%02x (in state %d)
#define ERROR_DEVICE_PLUG 27 // Device plugged while in state %d
#define ERROR_REPORTDESC 28 // Report descriptor sent while in state %d
#define ERROR_GET_REPORT_STATE 29 // GET_REPORT in state %d
#define ERROR_SET_REPORT_STATE 30 // SET_REPORT in state %d
#define ERROR_GOT_DATA_STATE 31 // Got data in state %d
#define GOT_DATA_FROM_HOST 32 // Got %d bytes from host

#define STATE_CHANGE 33 // Going from state %d to state %d

#define ERROR_WRONG_MAGIC 34 // Invalid magic word 0x%04x in EEPROM
#define CONFIGURATION_SIZE 35 // Configuration with size %d bytes
#define ACTION_SIZE 36 // Action with size %d bytes
#define ACTION_STACKSIZE 37 // Stack size is %d bytes
#define VM_DONE 38 // %d VMs instantiated

/* DS4USB.cpp */

#define DEVICE_PLUGGED 39 // Device plugged in
#define ERROR_SET_IDLE 40 // Error on SetIdle: 0x%02x
#define DS4_HID_IF 41 // Dualshock 4 on HID interface %d
#define ERROR_GET_REPORT 42 // Error on GetReport: 0x%02x
#define ERROR_SET_REPORT 43 // Error on SetReport: 0x%02x
#define ERROR_SNDRPT 44 // Error on SndRpt: 0x%02x
#define WARN_OOB_REPORT 45 // Got OOB report 0x%02x

/* Calibrator.cpp */

#define CALIB_MOVE 46 // RPAD: 0x%02x for %d frames (%d reports)
#define CALIB_END 47 // Calibration over

/* BytecodeWriter.cpp */

#define BYTECODE_RECEIVED 48 // Received %d bytes for EEPROM at offset %d
#endif
