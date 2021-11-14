
#ifndef _DS4HID_H_
#define _DS4HID_H_

//--------------------------------------------------------------------------------
// Decoded Application Collection
//--------------------------------------------------------------------------------

static const uint8_t usbHidReportDescriptor[] PROGMEM =
{
  0x05, 0x01,                  // (GLOBAL) USAGE_PAGE         0x0001 Generic Desktop Page 
  0x09, 0x05,                  // (LOCAL)  USAGE              0x00010005 Game Pad (Application Collection)  
  0xA1, 0x01,                  // (MAIN)   COLLECTION         0x01 Application (Usage=0x00010005: Page=Generic Desktop Page, Usage=Game Pad, Type=Application Collection)
  0x85, 0x01,                  //   (GLOBAL) REPORT_ID          0x01 (1)  
  0x09, 0x30,                  //   (LOCAL)  USAGE              0x00010030 X (Dynamic Value)  
  0x09, 0x31,                  //   (LOCAL)  USAGE              0x00010031 Y (Dynamic Value)  
  0x09, 0x32,                  //   (LOCAL)  USAGE              0x00010032 Z (Dynamic Value)  
  0x09, 0x35,                  //   (LOCAL)  USAGE              0x00010035 Rz (Dynamic Value)  
  0x15, 0x00,                  //   (GLOBAL) LOGICAL_MINIMUM    0x00 (0)  <-- Info: Consider replacing 15 00 with 14
  0x26, 0xFF, 0x00,            //   (GLOBAL) LOGICAL_MAXIMUM    0x00FF (255)  
  0x75, 0x08,                  //   (GLOBAL) REPORT_SIZE        0x08 (8) Number of bits per field  
  0x95, 0x04,                  //   (GLOBAL) REPORT_COUNT       0x04 (4) Number of fields  
  0x81, 0x02,                  //   (MAIN)   INPUT              0x00000002 (4 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x09, 0x39,                  //   (LOCAL)  USAGE              0x00010039 Hat switch (Dynamic Value)  
  0x15, 0x00,                  //   (GLOBAL) LOGICAL_MINIMUM    0x00 (0) <-- Redundant: LOGICAL_MINIMUM is already 0 <-- Info: Consider replacing 15 00 with 14
  0x25, 0x07,                  //   (GLOBAL) LOGICAL_MAXIMUM    0x07 (7)  
  0x35, 0x00,                  //   (GLOBAL) PHYSICAL_MINIMUM   0x00 (0)  <-- Info: Consider replacing 35 00 with 34
  0x46, 0x3B, 0x01,            //   (GLOBAL) PHYSICAL_MAXIMUM   0x013B (315)  
  0x65, 0x14,                  //   (GLOBAL) UNIT               0x14 Rotation in degrees [1° units] (4=System=English Rotation, 1=Rotation=Degrees)  
  0x75, 0x04,                  //   (GLOBAL) REPORT_SIZE        0x04 (4) Number of bits per field  
  0x95, 0x01,                  //   (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields  
  0x81, 0x42,                  //   (MAIN)   INPUT              0x00000042 (1 field x 4 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 1=Null 0=NonVolatile 0=Bitmap 
  0x65, 0x00,                  //   (GLOBAL) UNIT               0x00 No unit (0=None)  <-- Info: Consider replacing 65 00 with 64
  0x05, 0x09,                  //   (GLOBAL) USAGE_PAGE         0x0009 Button Page 
  0x19, 0x01,                  //   (LOCAL)  USAGE_MINIMUM      0x00090001 Button 1 Primary/trigger (Selector, On/Off Control, Momentary Control, or One Shot Control)  
  0x29, 0x0E,                  //   (LOCAL)  USAGE_MAXIMUM      0x0009000E Button 14 (Selector, On/Off Control, Momentary Control, or One Shot Control)  
  0x15, 0x00,                  //   (GLOBAL) LOGICAL_MINIMUM    0x00 (0) <-- Redundant: LOGICAL_MINIMUM is already 0 <-- Info: Consider replacing 15 00 with 14
  0x25, 0x01,                  //   (GLOBAL) LOGICAL_MAXIMUM    0x01 (1)  
  0x75, 0x01,                  //   (GLOBAL) REPORT_SIZE        0x01 (1) Number of bits per field  
  0x95, 0x0E,                  //   (GLOBAL) REPORT_COUNT       0x0E (14) Number of fields  
  0x81, 0x02,                  //   (MAIN)   INPUT              0x00000002 (14 fields x 1 bit) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x06, 0x00, 0xFF,            //   (GLOBAL) USAGE_PAGE         0xFF00 Vendor-defined 
  0x09, 0x20,                  //   (LOCAL)  USAGE              0xFF000020 <-- Warning: Undocumented usage (document it by inserting 0020 into file FF00.conf)
  0x75, 0x06,                  //   (GLOBAL) REPORT_SIZE        0x06 (6) Number of bits per field  
  0x95, 0x01,                  //   (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields  
  0x15, 0x00,                  //   (GLOBAL) LOGICAL_MINIMUM    0x00 (0) <-- Redundant: LOGICAL_MINIMUM is already 0 <-- Info: Consider replacing 15 00 with 14
  0x25, 0x7F,                  //   (GLOBAL) LOGICAL_MAXIMUM    0x7F (127)  
  0x81, 0x02,                  //   (MAIN)   INPUT              0x00000002 (1 field x 6 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap  <-- Error: REPORT_SIZE (6) is too small for LOGICAL_MAXIMUM (127) which needs 7 bits.
  0x05, 0x01,                  //   (GLOBAL) USAGE_PAGE         0x0001 Generic Desktop Page 
  0x09, 0x33,                  //   (LOCAL)  USAGE              0x00010033 Rx (Dynamic Value)  
  0x09, 0x34,                  //   (LOCAL)  USAGE              0x00010034 Ry (Dynamic Value)  
  0x15, 0x00,                  //   (GLOBAL) LOGICAL_MINIMUM    0x00 (0) <-- Redundant: LOGICAL_MINIMUM is already 0 <-- Info: Consider replacing 15 00 with 14
  0x26, 0xFF, 0x00,            //   (GLOBAL) LOGICAL_MAXIMUM    0x00FF (255)  
  0x75, 0x08,                  //   (GLOBAL) REPORT_SIZE        0x08 (8) Number of bits per field  
  0x95, 0x02,                  //   (GLOBAL) REPORT_COUNT       0x02 (2) Number of fields  
  0x81, 0x02,                  //   (MAIN)   INPUT              0x00000002 (2 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x06, 0x00, 0xFF,            //   (GLOBAL) USAGE_PAGE         0xFF00 Vendor-defined 
  0x09, 0x21,                  //   (LOCAL)  USAGE              0xFF000021 <-- Warning: Undocumented usage (document it by inserting 0021 into file FF00.conf)
  0x95, 0x36,                  //   (GLOBAL) REPORT_COUNT       0x36 (54) Number of fields  
  0x81, 0x02,                  //   (MAIN)   INPUT              0x00000002 (54 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x05,                  //   (GLOBAL) REPORT_ID          0x05 (5)  
  0x09, 0x22,                  //   (LOCAL)  USAGE              0xFF000022 <-- Warning: Undocumented usage (document it by inserting 0022 into file FF00.conf)
  0x95, 0x1F,                  //   (GLOBAL) REPORT_COUNT       0x1F (31) Number of fields  
  0x91, 0x02,                  //   (MAIN)   OUTPUT             0x00000002 (31 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x04,                  //   (GLOBAL) REPORT_ID          0x04 (4)  
  0x09, 0x23,                  //   (LOCAL)  USAGE              0xFF000023 <-- Warning: Undocumented usage (document it by inserting 0023 into file FF00.conf)
  0x95, 0x24,                  //   (GLOBAL) REPORT_COUNT       0x24 (36) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (36 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x02,                  //   (GLOBAL) REPORT_ID          0x02 (2)  
  0x09, 0x24,                  //   (LOCAL)  USAGE              0xFF000024 <-- Warning: Undocumented usage (document it by inserting 0024 into file FF00.conf)
  0x95, 0x24,                  //   (GLOBAL) REPORT_COUNT       0x24 (36) Number of fields <-- Redundant: REPORT_COUNT is already 36 
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (36 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x08,                  //   (GLOBAL) REPORT_ID          0x08 (8)  
  0x09, 0x25,                  //   (LOCAL)  USAGE              0xFF000025 <-- Warning: Undocumented usage (document it by inserting 0025 into file FF00.conf)
  0x95, 0x03,                  //   (GLOBAL) REPORT_COUNT       0x03 (3) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (3 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x10,                  //   (GLOBAL) REPORT_ID          0x10 (16)  
  0x09, 0x26,                  //   (LOCAL)  USAGE              0xFF000026 <-- Warning: Undocumented usage (document it by inserting 0026 into file FF00.conf)
  0x95, 0x04,                  //   (GLOBAL) REPORT_COUNT       0x04 (4) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (4 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x11,                  //   (GLOBAL) REPORT_ID          0x11 (17)  
  0x09, 0x27,                  //   (LOCAL)  USAGE              0xFF000027 <-- Warning: Undocumented usage (document it by inserting 0027 into file FF00.conf)
  0x95, 0x02,                  //   (GLOBAL) REPORT_COUNT       0x02 (2) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (2 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x12,                  //   (GLOBAL) REPORT_ID          0x12 (18)  
  0x06, 0x02, 0xFF,            //   (GLOBAL) USAGE_PAGE         0xFF02 Vendor-defined 
  0x09, 0x21,                  //   (LOCAL)  USAGE              0xFF020021 <-- Warning: Undocumented usage (document it by inserting 0021 into file FF02.conf)
  0x95, 0x0F,                  //   (GLOBAL) REPORT_COUNT       0x0F (15) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (15 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x13,                  //   (GLOBAL) REPORT_ID          0x13 (19)  
  0x09, 0x22,                  //   (LOCAL)  USAGE              0xFF020022 <-- Warning: Undocumented usage (document it by inserting 0022 into file FF02.conf)
  0x95, 0x16,                  //   (GLOBAL) REPORT_COUNT       0x16 (22) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (22 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x14,                  //   (GLOBAL) REPORT_ID          0x14 (20)  
  0x06, 0x05, 0xFF,            //   (GLOBAL) USAGE_PAGE         0xFF05 Vendor-defined 
  0x09, 0x20,                  //   (LOCAL)  USAGE              0xFF050020 <-- Warning: Undocumented usage (document it by inserting 0020 into file FF05.conf)
  0x95, 0x10,                  //   (GLOBAL) REPORT_COUNT       0x10 (16) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (16 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x15,                  //   (GLOBAL) REPORT_ID          0x15 (21)  
  0x09, 0x21,                  //   (LOCAL)  USAGE              0xFF050021 <-- Warning: Undocumented usage (document it by inserting 0021 into file FF05.conf)
  0x95, 0x2C,                  //   (GLOBAL) REPORT_COUNT       0x2C (44) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (44 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x06, 0x80, 0xFF,            //   (GLOBAL) USAGE_PAGE         0xFF80 Vendor-defined 
  0x85, 0x80,                  //   (GLOBAL) REPORT_ID          0x80 (128)  
  0x09, 0x20,                  //   (LOCAL)  USAGE              0xFF800020 <-- Warning: Undocumented usage (document it by inserting 0020 into file FF80.conf)
  0x95, 0x06,                  //   (GLOBAL) REPORT_COUNT       0x06 (6) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (6 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x81,                  //   (GLOBAL) REPORT_ID          0x81 (129)  
  0x09, 0x21,                  //   (LOCAL)  USAGE              0xFF800021 <-- Warning: Undocumented usage (document it by inserting 0021 into file FF80.conf)
  0x95, 0x06,                  //   (GLOBAL) REPORT_COUNT       0x06 (6) Number of fields <-- Redundant: REPORT_COUNT is already 6 
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (6 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x82,                  //   (GLOBAL) REPORT_ID          0x82 (130)  
  0x09, 0x22,                  //   (LOCAL)  USAGE              0xFF800022 <-- Warning: Undocumented usage (document it by inserting 0022 into file FF80.conf)
  0x95, 0x05,                  //   (GLOBAL) REPORT_COUNT       0x05 (5) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (5 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x83,                  //   (GLOBAL) REPORT_ID          0x83 (131)  
  0x09, 0x23,                  //   (LOCAL)  USAGE              0xFF800023 <-- Warning: Undocumented usage (document it by inserting 0023 into file FF80.conf)
  0x95, 0x01,                  //   (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (1 field x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x84,                  //   (GLOBAL) REPORT_ID          0x84 (132)  
  0x09, 0x24,                  //   (LOCAL)  USAGE              0xFF800024 <-- Warning: Undocumented usage (document it by inserting 0024 into file FF80.conf)
  0x95, 0x04,                  //   (GLOBAL) REPORT_COUNT       0x04 (4) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (4 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x85,                  //   (GLOBAL) REPORT_ID          0x85 (133)  
  0x09, 0x25,                  //   (LOCAL)  USAGE              0xFF800025 <-- Warning: Undocumented usage (document it by inserting 0025 into file FF80.conf)
  0x95, 0x06,                  //   (GLOBAL) REPORT_COUNT       0x06 (6) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (6 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x86,                  //   (GLOBAL) REPORT_ID          0x86 (134)  
  0x09, 0x26,                  //   (LOCAL)  USAGE              0xFF800026 <-- Warning: Undocumented usage (document it by inserting 0026 into file FF80.conf)
  0x95, 0x06,                  //   (GLOBAL) REPORT_COUNT       0x06 (6) Number of fields <-- Redundant: REPORT_COUNT is already 6 
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (6 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x87,                  //   (GLOBAL) REPORT_ID          0x87 (135)  
  0x09, 0x27,                  //   (LOCAL)  USAGE              0xFF800027 <-- Warning: Undocumented usage (document it by inserting 0027 into file FF80.conf)
  0x95, 0x23,                  //   (GLOBAL) REPORT_COUNT       0x23 (35) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (35 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x88,                  //   (GLOBAL) REPORT_ID          0x88 (136)  
  0x09, 0x28,                  //   (LOCAL)  USAGE              0xFF800028 <-- Warning: Undocumented usage (document it by inserting 0028 into file FF80.conf)
  0x95, 0x3F,                  //   (GLOBAL) REPORT_COUNT       0x3F (63) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (63 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x89,                  //   (GLOBAL) REPORT_ID          0x89 (137)  
  0x09, 0x29,                  //   (LOCAL)  USAGE              0xFF800029 <-- Warning: Undocumented usage (document it by inserting 0029 into file FF80.conf)
  0x95, 0x02,                  //   (GLOBAL) REPORT_COUNT       0x02 (2) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (2 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x90,                  //   (GLOBAL) REPORT_ID          0x90 (144)  
  0x09, 0x30,                  //   (LOCAL)  USAGE              0xFF800030 <-- Warning: Undocumented usage (document it by inserting 0030 into file FF80.conf)
  0x95, 0x05,                  //   (GLOBAL) REPORT_COUNT       0x05 (5) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (5 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x91,                  //   (GLOBAL) REPORT_ID          0x91 (145)  
  0x09, 0x31,                  //   (LOCAL)  USAGE              0xFF800031 <-- Warning: Undocumented usage (document it by inserting 0031 into file FF80.conf)
  0x95, 0x03,                  //   (GLOBAL) REPORT_COUNT       0x03 (3) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (3 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x92,                  //   (GLOBAL) REPORT_ID          0x92 (146)  
  0x09, 0x32,                  //   (LOCAL)  USAGE              0xFF800032 <-- Warning: Undocumented usage (document it by inserting 0032 into file FF80.conf)
  0x95, 0x03,                  //   (GLOBAL) REPORT_COUNT       0x03 (3) Number of fields <-- Redundant: REPORT_COUNT is already 3 
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (3 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x93,                  //   (GLOBAL) REPORT_ID          0x93 (147)  
  0x09, 0x33,                  //   (LOCAL)  USAGE              0xFF800033 <-- Warning: Undocumented usage (document it by inserting 0033 into file FF80.conf)
  0x95, 0x0C,                  //   (GLOBAL) REPORT_COUNT       0x0C (12) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (12 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0x94,                  //   (GLOBAL) REPORT_ID          0x94 (148)  
  0x09, 0x34,                  //   (LOCAL)  USAGE              0xFF800034 <-- Warning: Undocumented usage (document it by inserting 0034 into file FF80.conf)
  0x95, 0x3F,                  //   (GLOBAL) REPORT_COUNT       0x3F (63) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (63 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xA0,                  //   (GLOBAL) REPORT_ID          0xA0 (160)  
  0x09, 0x40,                  //   (LOCAL)  USAGE              0xFF800040 <-- Warning: Undocumented usage (document it by inserting 0040 into file FF80.conf)
  0x95, 0x06,                  //   (GLOBAL) REPORT_COUNT       0x06 (6) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (6 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xA1,                  //   (GLOBAL) REPORT_ID          0xA1 (161)  
  0x09, 0x41,                  //   (LOCAL)  USAGE              0xFF800041 <-- Warning: Undocumented usage (document it by inserting 0041 into file FF80.conf)
  0x95, 0x01,                  //   (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (1 field x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xA2,                  //   (GLOBAL) REPORT_ID          0xA2 (162)  
  0x09, 0x42,                  //   (LOCAL)  USAGE              0xFF800042 <-- Warning: Undocumented usage (document it by inserting 0042 into file FF80.conf)
  0x95, 0x01,                  //   (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields <-- Redundant: REPORT_COUNT is already 1 
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (1 field x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xA3,                  //   (GLOBAL) REPORT_ID          0xA3 (163)  
  0x09, 0x43,                  //   (LOCAL)  USAGE              0xFF800043 <-- Warning: Undocumented usage (document it by inserting 0043 into file FF80.conf)
  0x95, 0x30,                  //   (GLOBAL) REPORT_COUNT       0x30 (48) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (48 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xA4,                  //   (GLOBAL) REPORT_ID          0xA4 (164)  
  0x09, 0x44,                  //   (LOCAL)  USAGE              0xFF800044 <-- Warning: Undocumented usage (document it by inserting 0044 into file FF80.conf)
  0x95, 0x0D,                  //   (GLOBAL) REPORT_COUNT       0x0D (13) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (13 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xF0,                  //   (GLOBAL) REPORT_ID          0xF0 (240)  
  0x09, 0x47,                  //   (LOCAL)  USAGE              0xFF800047 <-- Warning: Undocumented usage (document it by inserting 0047 into file FF80.conf)
  0x95, 0x3F,                  //   (GLOBAL) REPORT_COUNT       0x3F (63) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (63 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xF1,                  //   (GLOBAL) REPORT_ID          0xF1 (241)  
  0x09, 0x48,                  //   (LOCAL)  USAGE              0xFF800048 <-- Warning: Undocumented usage (document it by inserting 0048 into file FF80.conf)
  0x95, 0x3F,                  //   (GLOBAL) REPORT_COUNT       0x3F (63) Number of fields <-- Redundant: REPORT_COUNT is already 63 
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (63 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xF2,                  //   (GLOBAL) REPORT_ID          0xF2 (242)  
  0x09, 0x49,                  //   (LOCAL)  USAGE              0xFF800049 <-- Warning: Undocumented usage (document it by inserting 0049 into file FF80.conf)
  0x95, 0x0F,                  //   (GLOBAL) REPORT_COUNT       0x0F (15) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (15 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xA7,                  //   (GLOBAL) REPORT_ID          0xA7 (167)  
  0x09, 0x4A,                  //   (LOCAL)  USAGE              0xFF80004A <-- Warning: Undocumented usage (document it by inserting 004A into file FF80.conf)
  0x95, 0x01,                  //   (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (1 field x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xA8,                  //   (GLOBAL) REPORT_ID          0xA8 (168)  
  0x09, 0x4B,                  //   (LOCAL)  USAGE              0xFF80004B <-- Warning: Undocumented usage (document it by inserting 004B into file FF80.conf)
  0x95, 0x01,                  //   (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields <-- Redundant: REPORT_COUNT is already 1 
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (1 field x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xA9,                  //   (GLOBAL) REPORT_ID          0xA9 (169)  
  0x09, 0x4C,                  //   (LOCAL)  USAGE              0xFF80004C <-- Warning: Undocumented usage (document it by inserting 004C into file FF80.conf)
  0x95, 0x08,                  //   (GLOBAL) REPORT_COUNT       0x08 (8) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (8 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xAA,                  //   (GLOBAL) REPORT_ID          0xAA (170)  
  0x09, 0x4E,                  //   (LOCAL)  USAGE              0xFF80004E <-- Warning: Undocumented usage (document it by inserting 004E into file FF80.conf)
  0x95, 0x01,                  //   (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (1 field x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xAB,                  //   (GLOBAL) REPORT_ID          0xAB (171)  
  0x09, 0x4F,                  //   (LOCAL)  USAGE              0xFF80004F <-- Warning: Undocumented usage (document it by inserting 004F into file FF80.conf)
  0x95, 0x39,                  //   (GLOBAL) REPORT_COUNT       0x39 (57) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (57 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xAC,                  //   (GLOBAL) REPORT_ID          0xAC (172)  
  0x09, 0x50,                  //   (LOCAL)  USAGE              0xFF800050 <-- Warning: Undocumented usage (document it by inserting 0050 into file FF80.conf)
  0x95, 0x39,                  //   (GLOBAL) REPORT_COUNT       0x39 (57) Number of fields <-- Redundant: REPORT_COUNT is already 57 
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (57 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xAD,                  //   (GLOBAL) REPORT_ID          0xAD (173)  
  0x09, 0x51,                  //   (LOCAL)  USAGE              0xFF800051 <-- Warning: Undocumented usage (document it by inserting 0051 into file FF80.conf)
  0x95, 0x0B,                  //   (GLOBAL) REPORT_COUNT       0x0B (11) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (11 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xAE,                  //   (GLOBAL) REPORT_ID          0xAE (174)  
  0x09, 0x52,                  //   (LOCAL)  USAGE              0xFF800052 <-- Warning: Undocumented usage (document it by inserting 0052 into file FF80.conf)
  0x95, 0x01,                  //   (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (1 field x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xAF,                  //   (GLOBAL) REPORT_ID          0xAF (175)  
  0x09, 0x53,                  //   (LOCAL)  USAGE              0xFF800053 <-- Warning: Undocumented usage (document it by inserting 0053 into file FF80.conf)
  0x95, 0x02,                  //   (GLOBAL) REPORT_COUNT       0x02 (2) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (2 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xB0,                  //   (GLOBAL) REPORT_ID          0xB0 (176)  
  0x09, 0x54,                  //   (LOCAL)  USAGE              0xFF800054 <-- Warning: Undocumented usage (document it by inserting 0054 into file FF80.conf)
  0x95, 0x3F,                  //   (GLOBAL) REPORT_COUNT       0x3F (63) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (63 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xE0,                  //   (GLOBAL) REPORT_ID          0xE0 (224)  
  0x09, 0x57,                  //   (LOCAL)  USAGE              0xFF800057 <-- Warning: Undocumented usage (document it by inserting 0057 into file FF80.conf)
  0x95, 0x02,                  //   (GLOBAL) REPORT_COUNT       0x02 (2) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (2 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xB3,                  //   (GLOBAL) REPORT_ID          0xB3 (179)  
  0x09, 0x55,                  //   (LOCAL)  USAGE              0xFF800055 <-- Warning: Undocumented usage (document it by inserting 0055 into file FF80.conf)
  0x95, 0x3F,                  //   (GLOBAL) REPORT_COUNT       0x3F (63) Number of fields  
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (63 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xB4,                  //   (GLOBAL) REPORT_ID          0xB4 (180)  
  0x09, 0x55,                  //   (LOCAL)  USAGE              0xFF800055 <-- Warning: Undocumented usage (document it by inserting 0055 into file FF80.conf)
  0x95, 0x3F,                  //   (GLOBAL) REPORT_COUNT       0x3F (63) Number of fields <-- Redundant: REPORT_COUNT is already 63 
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (63 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xB5,                  //   (GLOBAL) REPORT_ID          0xB5 (181)  
  0x09, 0x56,                  //   (LOCAL)  USAGE              0xFF800056 <-- Warning: Undocumented usage (document it by inserting 0056 into file FF80.conf)
  0x95, 0x3F,                  //   (GLOBAL) REPORT_COUNT       0x3F (63) Number of fields <-- Redundant: REPORT_COUNT is already 63 
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (63 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xD0,                  //   (GLOBAL) REPORT_ID          0xD0 (208)  
  0x09, 0x58,                  //   (LOCAL)  USAGE              0xFF800058 <-- Warning: Undocumented usage (document it by inserting 0058 into file FF80.conf)
  0x95, 0x3F,                  //   (GLOBAL) REPORT_COUNT       0x3F (63) Number of fields <-- Redundant: REPORT_COUNT is already 63 
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (63 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0x85, 0xD4,                  //   (GLOBAL) REPORT_ID          0xD4 (212)  
  0x09, 0x59,                  //   (LOCAL)  USAGE              0xFF800059 <-- Warning: Undocumented usage (document it by inserting 0059 into file FF80.conf)
  0x95, 0x3F,                  //   (GLOBAL) REPORT_COUNT       0x3F (63) Number of fields <-- Redundant: REPORT_COUNT is already 63 
  0xB1, 0x02,                  //   (MAIN)   FEATURE            0x00000002 (63 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
  0xC0,                        // (MAIN)   END_COLLECTION     Application  <-- Warning: Physical units are still in effect PHYSICAL(MIN=0,MAX=315) UNIT(0x00000000,EXP=0)
};

#endif // _DS4HID_H_
