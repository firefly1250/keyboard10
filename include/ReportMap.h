/*
based on
https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLETests/SampleHIDKeyboard.cpp
*/

#include "HIDTypes.h"

constexpr uint8_t reportMapKeyboard[] = {
    USAGE_PAGE(1),
    0x01, // Generic Desktop Ctrls
    USAGE(1),
    0x06, // Keyboard
    COLLECTION(1),
    0x01, // Application
    REPORT_ID(1),
    0x01, // REPORTID
    USAGE_PAGE(1),
    0x07, //   Kbrd/Keypad
    USAGE_MINIMUM(1), 0xE0, USAGE_MAXIMUM(1), 0xE7, LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x01, REPORT_SIZE(1),
    0x01, //   1 byte (Modifier)
    REPORT_COUNT(1), 0x08, HIDINPUT(1),
    0x02, //   Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position
    REPORT_COUNT(1),
    0x01, //   1 byte (Reserved)
    REPORT_SIZE(1), 0x08, HIDINPUT(1),
    0x01, //   Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position
    REPORT_COUNT(1),
    0x05, //   5 bits (Num lock, Caps lock, Scroll lock, Compose, Kana)
    REPORT_SIZE(1), 0x01, USAGE_PAGE(1),
    0x08, //   LEDs
    USAGE_MINIMUM(1),
    0x01, //   Num Lock
    USAGE_MAXIMUM(1),
    0x05, //   Kana
    HIDOUTPUT(1),
    0x02, //   Data,Var,Abs,No Wrap,Linear,Preferred State,No Null
          //   Position,Non-volatile
    REPORT_COUNT(1),
    0x01, //   3 bits (Padding)
    REPORT_SIZE(1), 0x03, HIDOUTPUT(1),
    0x01, //   Const,Array,Abs,No Wrap,Linear,Preferred State,No Null
          //   Position,Non-volatile
    REPORT_COUNT(1),
    0x06, //   6 bytes (Keys)
    REPORT_SIZE(1), 0x08, LOGICAL_MINIMUM(1), 0x00, LOGICAL_MAXIMUM(1),
    0xfe, //   101 keys
    USAGE_PAGE(1),
    0x07, //   Kbrd/Keypad
    USAGE_MINIMUM(1), 0x00, USAGE_MAXIMUM(1), 0xfe, HIDINPUT(1),
    0x00, //   Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position
    END_COLLECTION(0)};

//
// Wheel Mouse - simplified version
//
// Input report - 5 bytes
//
//     Byte | D7      D6      D5      D4      D3      D2      D1      D0
//    ------+---------------------------------------------------------------------
//      0   |  0       0       0    Forward  Back    Middle  Right   Left
//      1   |                             X
//      2   |                             Y
//      3   |                          Vertical Wheel
//      4   |                    Horizontal (Tilt) Wheel
//
// Feature report - 1 byte
//
//     Byte | D7      D6      D5      D4   |  D3      D2  |   D1      D0
//    ------+------------------------------+--------------+----------------
//      0   |  0       0       0       0   |  Horizontal  |    Vertical
//                                             (Resolution multiplier)
//
// Reference
//    http://www.microchip.com/forums/m391435.aspx
//

// https://github.com/T-vK/ESP32-BLE-Mouse/blob/master/BleMouse.cpp
constexpr uint8_t reportMapMouse[] = {
    USAGE_PAGE(1), 0x01, // USAGE_PAGE (Generic Desktop)
    USAGE(1), 0x02,      // USAGE (Mouse)
    COLLECTION(1), 0x01, // COLLECTION (Application)
    USAGE(1), 0x01,      //   USAGE (Pointer)
    COLLECTION(1), 0x00, //   COLLECTION (Physical)
    REPORT_ID(1), 0x01,  //     REPORT_ID (1)
    // ------------------------------------------------- Buttons (Left, Right,
    // Middle, Back, Forward)
    USAGE_PAGE(1), 0x09,      //     USAGE_PAGE (Button)
    USAGE_MINIMUM(1), 0x01,   //     USAGE_MINIMUM (Button 1)
    USAGE_MAXIMUM(1), 0x05,   //     USAGE_MAXIMUM (Button 5)
    LOGICAL_MINIMUM(1), 0x00, //     LOGICAL_MINIMUM (0)
    LOGICAL_MAXIMUM(1), 0x01, //     LOGICAL_MAXIMUM (1)
    REPORT_SIZE(1), 0x01,     //     REPORT_SIZE (1)
    REPORT_COUNT(1), 0x05,    //     REPORT_COUNT (5)
    HIDINPUT(1), 0x02, //     INPUT (Data, Variable, Absolute) ;5 button bits
    // ------------------------------------------------- Padding
    REPORT_SIZE(1), 0x03,  //     REPORT_SIZE (3)
    REPORT_COUNT(1), 0x01, //     REPORT_COUNT (1)
    HIDINPUT(1),
    0x03, //     INPUT (Constant, Variable, Absolute) ;3 bit padding
    // ------------------------------------------------- X/Y position, Wheel
    USAGE_PAGE(1), 0x01,      //     USAGE_PAGE (Generic Desktop)
    USAGE(1), 0x30,           //     USAGE (X)
    USAGE(1), 0x31,           //     USAGE (Y)
    USAGE(1), 0x38,           //     USAGE (Wheel)
    LOGICAL_MINIMUM(1), 0x81, //     LOGICAL_MINIMUM (-127)
    LOGICAL_MAXIMUM(1), 0x7f, //     LOGICAL_MAXIMUM (127)
    REPORT_SIZE(1), 0x08,     //     REPORT_SIZE (8)
    REPORT_COUNT(1), 0x03,    //     REPORT_COUNT (3)
    HIDINPUT(1),
    0x06, //     INPUT (Data, Variable, Relative) ;3 bytes (X,Y,Wheel)
    // ------------------------------------------------- Horizontal wheel
    USAGE_PAGE(1), 0x0c,      //     USAGE PAGE (Consumer Devices)
    USAGE(2), 0x38, 0x02,     //     USAGE (AC Pan)
    LOGICAL_MINIMUM(1), 0x81, //     LOGICAL_MINIMUM (-127)
    LOGICAL_MAXIMUM(1), 0x7f, //     LOGICAL_MAXIMUM (127)
    REPORT_SIZE(1), 0x08,     //     REPORT_SIZE (8)
    REPORT_COUNT(1), 0x01,    //     REPORT_COUNT (1)
    HIDINPUT(1), 0x06,        //     INPUT (Data, Var, Rel)
    END_COLLECTION(0),        //   END_COLLECTION
    END_COLLECTION(0)         // END_COLLECTION
};
