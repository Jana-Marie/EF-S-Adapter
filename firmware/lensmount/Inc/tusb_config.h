#pragma once

//--------------------------------------------------------------------
// DEVICE CONFIGURATION
//--------------------------------------------------------------------
#define BOARD_TUD_RHPORT      0
#define BOARD_DEVICE_RHPORT_SPEED   OPT_MODE_HIGH_SPEED
#define CFG_TUSB_RHPORT0_MODE      (OPT_MODE_DEVICE | BOARD_DEVICE_RHPORT_SPEED)
#define CFG_TUD_ENDPOINT0_SIZE    64

//------------- CLASS -------------//
#define CFG_TUD_CDC               1
#define CFG_TUD_MSC               0
#define CFG_TUD_HID               0
#define CFG_TUD_MIDI              0
#define CFG_TUD_VENDOR            0

// CDC FIFO size of TX and RX
#define CFG_TUD_CDC_RX_BUFSIZE   (TUD_OPT_HIGH_SPEED ? 512 : 64)
#define CFG_TUD_CDC_TX_BUFSIZE   (TUD_OPT_HIGH_SPEED ? 512 : 64)

// CDC Endpoint transfer buffer size, more is faster
#define CFG_TUD_CDC_EP_BUFSIZE   (TUD_OPT_HIGH_SPEED ? 512 : 64)