
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

//--------------------------------------------------------------------------------
// Vendor-defined featureReport 04 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x04 (4)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0023[36];                      // Usage 0xFF000023: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport04_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 02 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x02 (2)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0024[36];                      // Usage 0xFF000024: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport02_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 08 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x08 (8)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0025[3];                       // Usage 0xFF000025: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport08_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 10 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x10 (16)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0026[4];                       // Usage 0xFF000026: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport10_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 11 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x11 (17)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0027[2];                       // Usage 0xFF000027: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport11_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 12 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x12 (18)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0021[15];                      // Usage 0xFF020021: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport12_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 13 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x13 (19)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0022[22];                      // Usage 0xFF020022: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport13_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 14 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x14 (20)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0020[16];                      // Usage 0xFF050020: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport14_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 15 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x15 (21)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0021[44];                      // Usage 0xFF050021: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport15_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 80 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x80 (128)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0020[6];                       // Usage 0xFF800020: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport80_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 81 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x81 (129)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0021[6];                       // Usage 0xFF800021: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport81_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 82 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x82 (130)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0022[5];                       // Usage 0xFF800022: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport82_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 83 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x83 (131)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0023;                          // Usage 0xFF800023: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport83_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 84 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x84 (132)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0024[4];                       // Usage 0xFF800024: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport84_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 85 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x85 (133)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0025[6];                       // Usage 0xFF800025: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport85_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 86 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x86 (134)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0026[6];                       // Usage 0xFF800026: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport86_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 87 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x87 (135)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0027[35];                      // Usage 0xFF800027: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport87_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 88 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x88 (136)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0028[63];                      // Usage 0xFF800028: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport88_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 89 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x89 (137)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0029[2];                       // Usage 0xFF800029: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport89_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 90 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x90 (144)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0030[5];                       // Usage 0xFF800030: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport90_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 91 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x91 (145)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0031[3];                       // Usage 0xFF800031: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport91_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 92 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x92 (146)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0032[3];                       // Usage 0xFF800032: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport92_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 93 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x93 (147)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0033[12];                      // Usage 0xFF800033: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport93_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport 94 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x94 (148)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0034[63];                      // Usage 0xFF800034: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport94_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport A0 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xA0 (160)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0040[6];                       // Usage 0xFF800040: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportA0_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport A1 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xA1 (161)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0041;                          // Usage 0xFF800041: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportA1_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport A2 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xA2 (162)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0042;                          // Usage 0xFF800042: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportA2_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport A3 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xA3 (163)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0043[48];                      // Usage 0xFF800043: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportA3_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport A4 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xA4 (164)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0044[13];                      // Usage 0xFF800044: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportA4_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport F0 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xF0 (240)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0047[63];                      // Usage 0xFF800047: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportF0_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport F1 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xF1 (241)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0048[63];                      // Usage 0xFF800048: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportF1_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport F2 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xF2 (242)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0049[15];                      // Usage 0xFF800049: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportF2_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport A7 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xA7 (167)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad004A;                          // Usage 0xFF80004A: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportA7_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport A8 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xA8 (168)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad004B;                          // Usage 0xFF80004B: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportA8_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport A9 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xA9 (169)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad004C[8];                       // Usage 0xFF80004C: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportA9_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport AA (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xAA (170)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad004E;                          // Usage 0xFF80004E: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportAA_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport AB (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xAB (171)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad004F[57];                      // Usage 0xFF80004F: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportAB_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport AC (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xAC (172)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0050[57];                      // Usage 0xFF800050: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportAC_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport AD (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xAD (173)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0051[11];                      // Usage 0xFF800051: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportAD_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport AE (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xAE (174)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0052;                          // Usage 0xFF800052: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportAE_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport AF (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xAF (175)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0053[2];                       // Usage 0xFF800053: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportAF_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport B0 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xB0 (176)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0054[63];                      // Usage 0xFF800054: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportB0_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport E0 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xE0 (224)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0057[2];                       // Usage 0xFF800057: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportE0_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport B3 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xB3 (179)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0055[63];                      // Usage 0xFF800055: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportB3_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport B4 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xB4 (180)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0055[63];                      // Usage 0xFF800055: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportB4_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport B5 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xB5 (181)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0056[63];                      // Usage 0xFF800056: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportB5_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport D0 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xD0 (208)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0058[63];                      // Usage 0xFF800058: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportD0_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport D4 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xD4 (212)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0059[63];                      // Usage 0xFF800059: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportD4_t;


//--------------------------------------------------------------------------------
// Generic Desktop Page inputReport 01 (Device --> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x01 (1)
                                                     // Collection: CA:GamePad
  uint8_t  GD_GamePadX;                              // Usage 0x00010030: X, Value = 0 to 255
  uint8_t  GD_GamePadY;                              // Usage 0x00010031: Y, Value = 0 to 255
  uint8_t  GD_GamePadZ;                              // Usage 0x00010032: Z, Value = 0 to 255
  uint8_t  GD_GamePadRz;                             // Usage 0x00010035: Rz, Value = 0 to 255
  uint8_t  GD_GamePadHatSwitch : 4;                  // Usage 0x00010039: Hat switch, Value = 0 to 7, Physical = Value x 45 in degrees
  uint8_t  BTN_GamePadButton1 : 1;                   // Usage 0x00090001: Button 1 Primary/trigger, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton2 : 1;                   // Usage 0x00090002: Button 2 Secondary, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton3 : 1;                   // Usage 0x00090003: Button 3 Tertiary, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton4 : 1;                   // Usage 0x00090004: Button 4, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton5 : 1;                   // Usage 0x00090005: Button 5, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton6 : 1;                   // Usage 0x00090006: Button 6, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton7 : 1;                   // Usage 0x00090007: Button 7, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton8 : 1;                   // Usage 0x00090008: Button 8, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton9 : 1;                   // Usage 0x00090009: Button 9, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton10 : 1;                  // Usage 0x0009000A: Button 10, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton11 : 1;                  // Usage 0x0009000B: Button 11, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton12 : 1;                  // Usage 0x0009000C: Button 12, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton13 : 1;                  // Usage 0x0009000D: Button 13, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton14 : 1;                  // Usage 0x0009000E: Button 14, Value = 0 to 1, Physical = Value x 315
  uint8_t  VEN_GamePad0020 : 6;                      // Usage 0xFF000020: , Value = 0 to 127, Physical = Value x 315 / 127
  uint8_t  GD_GamePadRx;                             // Usage 0x00010033: Rx, Value = 0 to 255, Physical = Value x 21 / 17
  uint8_t  GD_GamePadRy;                             // Usage 0x00010034: Ry, Value = 0 to 255, Physical = Value x 21 / 17
  uint8_t  VEN_GamePad0021[54];                      // Usage 0xFF000021: , Value = 0 to 255, Physical = Value x 21 / 17
} inputReport01_t;


//--------------------------------------------------------------------------------
// Vendor-defined outputReport 05 (Device <-- Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x05 (5)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0022[31];                      // Usage 0xFF000022: , Value = 0 to 255, Physical = Value x 21 / 17
} outputReport05_t;

#endif // _DS4HID_H_
