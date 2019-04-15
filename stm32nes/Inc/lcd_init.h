#ifndef __TFT_INIT_H
#define __TFT_INIT_H

#define TFT_ORIENTATION_CMD    0x36
#define TFT_EOF_MARKER         0x42
#define TFT_EOL_MARKER         0x43
#define TFT_DELAY_MARKER       0xFF

#ifdef MODEL_ILI9486
const uint8_t tft_init_code[] = { 0x00,
    // Interface Mode Control
    0xb0, 0x0,                                                        TFT_EOL_MARKER,
    // Sleep OUT
    0x11, TFT_DELAY_MARKER, 100,                                      TFT_EOL_MARKER,
    // Interface Pixel Format, 18 bits / pixel
    0x3A, 0x66,                                                       TFT_EOL_MARKER,
    // Power control VRH[5:0] = 4.60 V
    0xC0, 0x23,                                                       TFT_EOL_MARKER,
    // Power control SAP[2:0];BT[3:0] = adjust power to lowest possible
    0xC1, 0x10,                                                       TFT_EOL_MARKER,
    // Power Control 3 (For Normal Mode)
    0xC2, 0x44,                                                       TFT_EOL_MARKER,
    // VCOM Control
    0xC5, 0x00, 0x00, 0x00, 0x00,                                     TFT_EOL_MARKER,
    // PGAMCTRL(Positive Gamma Control)
    0xE0, 0x0F, 0x1F, 0x1C, 0x0C, 0x0F, 0x08, 0x48, 0x98, 0x37, 0x0A,
        0x13, 0x04, 0x11, 0x0D, 0x00,                                 TFT_EOL_MARKER,
    // NGAMCTRL (Negative Gamma Correction)
    0xE1, 0x0F, 0x32, 0x2E, 0x0B, 0x0D, 0x05, 0x47, 0x75, 0x37, 0x06,
        0x10, 0x03, 0x24, 0x20, 0x00,                                 TFT_EOL_MARKER,
    // Digital Gamma Control 1
    0xE2, 0x0F, 0x32, 0x2E, 0x0B, 0x0D, 0x05, 0x47, 0x75, 0x37, 0x06,
        0x10, 0x03, 0x24, 0x20, 0x00,                                 TFT_EOL_MARKER,
    // Memory Access Control
    TFT_ORIENTATION_CMD,                                              TFT_EOL_MARKER,
    // Exit sleep
    0x11, TFT_DELAY_MARKER, 100,                                      TFT_EOL_MARKER,
    0x29, TFT_DELAY_MARKER, 100,                                      TFT_EOL_MARKER,
    0x2c,                                                             TFT_EOL_MARKER,

    TFT_EOF_MARKER
};
const uint8_t tft_orientation_init_code[] = {
    0x48,
    0x28,
    0x88,
    0xE8,
};
#endif

#ifdef MODEL_ILI9341
const uint8_t tft_init_code[] = { 0x00,

    0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,                     TFT_EOL_MARKER,
    0xCF, 0x00, 0XC1, 0X30,                                 TFT_EOL_MARKER,
    0xE8, 0x85, 0x00, 0x78,                                 TFT_EOL_MARKER,
    0xEA, 0x00, 0x00,                                       TFT_EOL_MARKER,
    0xEA, 0x00, 0x00,                                       TFT_EOL_MARKER,
    0xED, 0x64, 0x03, 0x12, 0x81,                           TFT_EOL_MARKER,
    0xF7, 0x20,                                             TFT_EOL_MARKER,
    // Power control VRH[5:0] = 4.60 V
    0xC0, 0x23,                                             TFT_EOL_MARKER,
    // Power control SAP[2:0];BT[3:0] = adjust power to lowest possible
    0xC1, 0x10,                                             TFT_EOL_MARKER,
    // VCM control 1 Contrast VCOMH = 4.250 VCOML = -1.500
    0xC5, 0x3e, 0x28,                                       TFT_EOL_MARKER,
    // VCM control 2
    0xC7, 0x86,                                             TFT_EOL_MARKER,
    // Memory Access Control - 0x08 = BGR color filter panel, 0x40 = Column Address Order 1
    TFT_ORIENTATION_CMD,                                    TFT_EOL_MARKER,
    // COLMOD: Pixel Format Set 16bits for pixel
    0x3A, 0x55,                                             TFT_EOL_MARKER,
    // Frame Rate Control (In Normal Mode/Full Colors) - division ratio: 1 - 24 clocks
    0xB1, 0x00, 0x18,                                       TFT_EOL_MARKER,
    // Display Function Control
    0xB6, 0x08, 0x82, 0x27,                                 TFT_EOL_MARKER,
    // Exit sleep
    0x11, TFT_DELAY_MARKER, 120,                            TFT_EOL_MARKER,
    0x29,                                                   TFT_EOL_MARKER,
    0x2c,                                                   TFT_EOL_MARKER,

    // EOF - not sent to controller
    TFT_EOF_MARKER
};
const uint8_t tft_orientation_init_code[] = {
    0x48,
    0x28,
    0x88,
    0xE8,
};
#endif

#ifdef MODEL_ST7735
const uint8_t tft_init_code[] = { 0x00,

    // 0x01, TFT_DELAY_MARKER, 10,                             TFT_EOL_MARKER,
    0x11, TFT_DELAY_MARKER, 10,                             TFT_EOL_MARKER,

    // ST7735R Frame Rate
    0xB1, 0x01, 0x2C, 0x2D,                                 TFT_EOL_MARKER,
    0xB2, 0x01, 0x2C, 0x2D,                                 TFT_EOL_MARKER,
    0xB3, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D,               TFT_EOL_MARKER,

    // Column inversion 
    0xB4, 0x07,                                             TFT_EOL_MARKER,
 
    // ST7735R Power Sequence
    0xC0, 0xA2, 0x02, 0x84,                                 TFT_EOL_MARKER,
    0xC1, 0xC5,                                             TFT_EOL_MARKER,
    0xC2, 0x0A, 0x00,                                       TFT_EOL_MARKER,
    0xC3, 0x8A, 0x2A,                                       TFT_EOL_MARKER,
    0xC4, 0x8A, 0xEE,                                       TFT_EOL_MARKER,

    // VCOM
    0xC5, 0x0E,                                             TFT_EOL_MARKER,
 
    // MX, MY, RGB mode 
    TFT_ORIENTATION_CMD,                                    TFT_EOL_MARKER,   

    // ST7735R Gamma Sequence
    0xe0, 0x0f, 0x1a, 0x0f, 0x18, 0x2f, 0x28, 0x20, 0x22, 0x1f, 0x1b, 0x23, 0x37, 0x00, 0x07, 0x02, 0x10,            TFT_EOL_MARKER,
    0xe1, 0x0f, 0x1b, 0x0f, 0x17, 0x33, 0x2c, 0x29, 0x2e, 0x30, 0x30, 0x39, 0x3f, 0x00, 0x07, 0x03, 0x10,            TFT_EOL_MARKER,
    0x2a, 0x00, 0x00, 0x00, 0x7f,                           TFT_EOL_MARKER,
    0x2b, 0x00, 0x00, 0x00, 0x9f,                           TFT_EOL_MARKER,

    // Enable test command
    0xF0, 0x01,                                             TFT_EOL_MARKER,

    // Disable ram power save mode
    0xF6, 0x00,                                             TFT_EOL_MARKER,

    // 65k mode
    0x3A, 0x05,                                             TFT_EOL_MARKER,

    // Display on
    0x29,                                                   TFT_EOL_MARKER,
    0x2c,                                                   TFT_EOL_MARKER,

    // EOF - not sent to controller
    TFT_EOF_MARKER
};
const uint8_t tft_orientation_init_code[] = {
    0xC0,
    0xA0,
    0x00,
    0x60,
};
#endif

#endif
