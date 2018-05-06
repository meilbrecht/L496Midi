/*
 * usb_midi.h
 *
 *  Created on: 03.05.2018
 *      Author: meilbrecht
 *      Info: This file implements USB audio and MIDI functionallity based on
 *      		the STM32_USB_Device_Libraries USB Audio Class Middleware
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef USBD_MIDI_H_
#define USBD_MIDI_H_

/**
  ******************************************************************************
  * @file    usb_midi.h
  * @author  meilbrecht
  * @version V1.0.0
  * @date    03-March-2018
  * @brief   header file for the usb_midi.c file.
  ******************************************************************************
  */

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USB_MIDI
  * @brief This file is the Header file for usb_midi.c
  * @{
  */


/** @defgroup USB_MIDI_Exported_Defines
  * @{
  */
 // todo - adjust the following lines
#define MIDI_OUT_EP 								 0x01
#define MIDI_IN_EP                                   0x81
#define USB_MIDI_CONFIG_DESC_SIZ                     109
#define MIDI_INTERFACE_DESC_SIZE                     9
#define USB_MIDI_DESC_SIZ                            0x09
#define MIDI_STANDARD_ENDPOINT_DESC_SIZE             0x09
#define MIDI_STREAMING_ENDPOINT_DESC_SIZE            0x07

//#define AUDIO_DESCRIPTOR_TYPE                         0x21
//#define USB_DEVICE_CLASS_AUDIO                        0x01
//#define AUDIO_SUBCLASS_AUDIOCONTROL                   0x01
//#define AUDIO_SUBCLASS_AUDIOSTREAMING                 0x02
#define AUDIO_SUBCLASS_MIDISTREAMING					0x03
//#define AUDIO_SUBCLASS_MIDI
//#define AUDIO_PROTOCOL_UNDEFINED                      0x00
//#define AUDIO_STREAMING_GENERAL                       0x01
//#define AUDIO_STREAMING_FORMAT_TYPE                   0x02

/* Audio Descriptor Types */
#define AUDIO_DESCRIPTOR_TYPE_UNDEFINED					0x20
#define AUDIO_DEVICE_DESCRIPTOR_TYPE					0x21
#define AUDIO_CONFIGURATION_DESCRIPTOR_TYPE				0x22
#define AUDIO_STRING_DESCRIPTOR_TYPE					0x23
//#define AUDIO_INTERFACE_DESCRIPTOR_TYPE               0x24
//#define AUDIO_ENDPOINT_DESCRIPTOR_TYPE                0x25

/* Audio Control Interface Descriptor Subtypes */
//#define AUDIO_CONTROL_HEADER                          0x01
//#define AUDIO_CONTROL_INPUT_TERMINAL                  0x02
//#define AUDIO_CONTROL_OUTPUT_TERMINAL                 0x03
#define AUDIO_CONTROL_MIXER_UNIT						0x04
#define AUDIO_CONTROL_SELECTOR_UNIT						0x05
//#define AUDIO_CONTROL_FEATURE_UNIT                    0x06
#define AUDIO_CONTROL_PROCESSING_UNIT					0x07
#define AUDIO_CONTROL_EXTENSION_UNIT					0x08

//#define AUDIO_INPUT_TERMINAL_DESC_SIZE                0x0C
//#define AUDIO_OUTPUT_TERMINAL_DESC_SIZE               0x09
//#define AUDIO_STREAMING_INTERFACE_DESC_SIZE           0x07
//
//#define AUDIO_CONTROL_MUTE                            0x0001
//
//#define AUDIO_FORMAT_TYPE_I                           0x01
//#define AUDIO_FORMAT_TYPE_III                         0x03
//
//#define AUDIO_ENDPOINT_GENERAL                        0x01
//
//#define AUDIO_REQ_GET_CUR                             0x81
//#define AUDIO_REQ_SET_CUR                             0x01
#define AUDIO_REQ_GET_MIN                             0x82
#define AUDIO_REQ_SET_MIN                             0x02
#define AUDIO_REQ_GET_MAX                             0x83
#define AUDIO_REQ_SET_MAX                             0x03
#define AUDIO_REQ_GET_RES                             0x84
#define AUDIO_REQ_SET_RES                             0x04
#define AUDIO_REQ_GET_MEM                             0x85
#define AUDIO_REQ_SET_MEM                             0x05
#define AUDIO_REQ_GET_STAT                            0xFF

//#define AUDIO_OUT_STREAMING_CTRL                      0x02

#define MIDI_OUT_PACKET_SIZE	0x40
#define MIDI_IN_PACKET_SIZE		0x40

#define MIDI_TOTAL_BUF_SIZE    	0x40

/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */

extern USBD_ClassTypeDef  USBD_MIDI;
#define USBD_MIDI_CLASS    &USBD_MIDI
/**
  * @}
  */




 typedef struct
 {
   __IO uint32_t            alt_setting;
   uint8_t                  buffer[MIDI_TOTAL_BUF_SIZE];
   //MIDI_OffsetTypeDef       offset;
   uint8_t                  rd_enable;
   uint16_t                 rd_ptr;
   uint16_t                 wr_ptr;
 }
 USBD_MIDI_HandleTypeDef;


/** @defgroup USB_CORE_Exported_Functions
  * @{
  */
//uint8_t  USBD_MIDI_RegisterInterface  (USBD_HandleTypeDef   *pdev,
//                                        USBD_AUDIO_ItfTypeDef *fops);
//
//void  USBD_AUDIO_Sync (USBD_HandleTypeDef *pdev, AUDIO_OffsetTypeDef offset);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif  /* __USB_MIDI_H */
/**
  * @}
  */

/**
  * @}
  */
