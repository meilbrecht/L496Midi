/*
 * usb_midi.c
 *
 *  Created on: 03.05.2018
 *      Author: meilbrecht
 *      Info: This file implements USB audio and MIDI functionallity based on
 *      		the STM32_USB_Device_Libraries USB Audio Class Middleware
 */
#include <usbd_midi.h>
#if 0
#include "usbd_audio.h"
#include "usbd_desc.h"
#include "usbd_ctlreq.h"


/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_AUDIO
  * @brief usbd core module
  * @{
  */

/** @defgroup USBD_AUDIO_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_AUDIO_Private_Defines
  * @{
  */

/**
  * @}
  */

// todo - copied from usbd_audio.h
/** @defgroup USBD_AUDIO_Private_Macros
  * @{
  */
#define AUDIO_SAMPLE_FREQ(frq)      (uint8_t)(frq), (uint8_t)((frq >> 8)), (uint8_t)((frq >> 16))

#define AUDIO_PACKET_SZE(frq)          (uint8_t)(((frq * 2 * 2)/1000) & 0xFF), \
                                       (uint8_t)((((frq * 2 * 2)/1000) >> 8) & 0xFF)

/**
  * @}
  */




/** @defgroup USBD_AUDIO_Private_FunctionPrototypes
  * @{
  */


static uint8_t  USBD_MIDI_Init (USBD_HandleTypeDef *pdev,
                               uint8_t cfgidx);

static uint8_t  USBD_MIDI_DeInit (USBD_HandleTypeDef *pdev,
                                 uint8_t cfgidx);

static uint8_t  USBD_MIDI_Setup (USBD_HandleTypeDef *pdev,
                                USBD_SetupReqTypedef *req);

static uint8_t  *USBD_MIDI_GetCfgDesc (uint16_t *length);

static uint8_t  *USBD_MIDI_GetDeviceQualifierDesc (uint16_t *length);

static uint8_t  USBD_MIDI_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_MIDI_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_MIDI_EP0_RxReady (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_MIDI_EP0_TxReady (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_MIDI_SOF (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_MIDI_IsoINIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_MIDI_IsoOutIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum);

//static void MIDI_REQ_GetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
//
//static void MIDI_REQ_SetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

/**
  * @}
  */

/** @defgroup USBD_AUDIO_Private_Variables
  * @{
  */

USBD_ClassTypeDef  USBD_MIDI =
{
  USBD_MIDI_Init,
  USBD_MIDI_DeInit,
  USBD_MIDI_Setup,
  USBD_MIDI_EP0_TxReady,
  USBD_MIDI_EP0_RxReady,
  USBD_MIDI_DataIn,
  USBD_MIDI_DataOut,
  USBD_MIDI_SOF,
  USBD_MIDI_IsoINIncomplete,
  USBD_MIDI_IsoOutIncomplete,
  USBD_MIDI_GetCfgDesc,
  USBD_MIDI_GetCfgDesc,
  USBD_MIDI_GetCfgDesc,
  USBD_MIDI_GetDeviceQualifierDesc,
};



/* USB AUDIO device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_MIDI_CfgDesc[USB_AUDIO_CONFIG_DESC_SIZ] __ALIGN_END =
{
  /* Configuration 1 */
  0x09,                                 /* bLength */
  USB_DESC_TYPE_CONFIGURATION,          /* bDescriptorType */
  LOBYTE(101),							/* wTotalLength  109 bytes*/
  HIBYTE(101),
  0x02,                                 /* bNumInterfaces */
  0x01,                                 /* bConfigurationValue */
  0x00,                                 /* iConfiguration */
  0xC0,                                 /* bmAttributes  BUS Powred*/
  0x32,                                 /* bMaxPower = 100 mA*/
  /* 09 byte*/

  /* USB Audio Standard interface descriptor */
  AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
  USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
  0x00,                                 /* bInterfaceNumber */
  0x00,                                 /* bAlternateSetting */
  0x00,                                 /* bNumEndpoints */
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
  AUDIO_SUBCLASS_AUDIOCONTROL,          /* bInterfaceSubClass */
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
  0x00,                                 /* iInterface */
  /* 09 byte*/

  /* USB Audio Class-specific AC Interface Descriptor */
  AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_CONTROL_HEADER,                 /* bDescriptorSubtype */
  0x00,          /* 1.00 */             /* bcdADC */
  0x01,
  0x27,                                 /* wTotalLength = 39*/	// todo - adjust later!
  0x00,
  0x01,                                 /* bInCollection */
  0x01,                                 /* baInterfaceNr */
  /* 09 byte*/

  // todo - following parts have been taken from Akai LPK25 Descr. - go through and understand what happens!!

  /* USB Audio MIDI Interface */
  AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
  USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
  0x01,                                 /* bInterfaceNumber */
  0x00,                                 /* bAlternateSetting */
  0x02,                                 /* bNumEndpoints */
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
  AUDIO_SUBCLASS_MIDISTREAMING,          	/* bInterfaceSubClass */
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
  0x00,                                 /* iInterface */
  /* 09 byte*/

  /* MS Interface Header Descriptor */
  0x07,	/* bLength */
  0x24,	/* bDescriptorType */
  0x01,	/* bDescriptorSubtype */
  0x00,	/* bcdMSC */
  0x01,
  0x41,	/* wTotalLength   (65 bytes) */	// todo - check length field!
  0x00,
  /* 07 byte */

  /* MS MIDI IN Jack Descriptor */
  0x06, /*	bLength */
  0x24, /*	bDescriptorType */
  0x02, /*	bDescriptorSubtype */
  0x01, /*	bJackType */
  0x01, /*	bJackID */
  0x00, /*	iJack */
  /* 06 bytes */

  /* MS MIDI IN Jack Descriptor */
  0x06, /*	bLength */
  0x24, /*	bDescriptorType */
  0x02, /*	bDescriptorSubtype */
  0x02, /*	bJackType */
  0x02, /*	bJackID */
  0x00, /*	iJack */
  /* 06 bytes */

  /* MS MIDI OUT Jack Descriptor */
  0x09, /*	bLength */
  0x24, /*	bDescriptorType */
  0x03, /*	bDescriptorSubtype */
  0x01, /*	bJackType */
  0x03, /*	bJackID */
  0x01, /*	bNrInputPins */
  0x02, /*	baSourceID(1) */
  0x01, /*	baSourcePin(1) */
  0x00, /*	iJack */
  /* 09 bytes */

  /* MS MIDI OUT Jack Descriptor */
  0x09, /*	bLength */
  0x24, /*	bDescriptorType */
  0x03, /*	bDescriptorSubtype */
  0x02, /*	bJackType */
  0x04, /*	bJackID */
  0x01, /*	bNrInputPins */
  0x01, /*	baSourceID(1) */
  0x01, /*	baSourcePin(1) */
  0x00, /*	iJack */
  /* 09 bytes */

  /* Endpoint OUT Descriptor (Audio/MIDI 1.0) */
  0x09, /*	bLength */
  0x05, /*	bDescriptorType */
  MIDI_OUT_EP, /*	bEndpointAddress (OUT EP 1) */
  0x02, /*	bmAttributes (bulk, no sync, data) */
  0x40, /*	wMaxPacketSize (64 bytes) */
  0x00,
  0x00, /*	bInterval */
  0x00, /*	bRefresh */
  0x00, /*	bSynchAddress */
  /* 09 bytes */

  /* MS Bulk Data OUT Endpoint Descriptor */
  0x05, /*	bLength */
  0x25, /*	bDescriptorType */
  0x01, /*	bDescriptorSubtype */
  0x01, /*	bNumEmbMIDIJack */
  0x01, /*	baAssocJackID(1) */
  /* 05 bytes */

  /* Endpoint IN Descriptor (Audio/MIDI 1.0) */
  0x09, /*	bLength */
  0x05, /*	bDescriptorType */
  MIDI_IN_EP, /*	bEndpointAddress (IN EP 1) */
  0x02, /*	bmAttributes (bulk, no sync, data) */
  0x40, /*	wMaxPacketSize (64 bytes) */
  0x00,
  0x00, /*	bInterval */
  0x00, /*	bRefresh */
  0x00, /*	bSynchAddress */
  /* 09 bytes */

  /* MS Bulk Data IN Endpoint Descriptor */
  0x05, /*	bLength */
  0x25, /*	bDescriptorType */
  0x01, /*	bDescriptorSubtype */
  0x01, /*	bNumEmbMIDIJack */
  0x03, /*	baAssocJackID(1) */
  /* 05 bytes */
} ;

/* USB Standard Device Descriptor (compare to USB 2.0 specification chapter 9.6.2 (p. 262)  */
__ALIGN_BEGIN static uint8_t USBD_MIDI_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END=
{
  USB_LEN_DEV_QUALIFIER_DESC,		/* bLength */
  USB_DESC_TYPE_DEVICE_QUALIFIER,	/* bDescriptorType */
  0x00,								/* bcdUSB */
  0x02,
  0x00,								/* bDeviceClass */
  0x00,								/* bDeviceSubClass*/
  0x00,								/* bDeviceProtocol */
  0x40,								/* bMaxPacketSize0 */
  0x01,								/* bNumConfigurations */
  0x00,								/* bReserved */
};

/**
  * @}
  */

/** @defgroup USBD_AUDIO_Private_Functions
  * @{
  */

/**
  * @brief  USBD_MIDI_Init
  *         Initialize the MIDI interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_MIDI_Init (USBD_HandleTypeDef *pdev,
                               uint8_t cfgidx)
{
  USBD_MIDI_HandleTypeDef   *hmidiin;

#if 0
  USBD_MIDI_HandleTypeDef   *hmidiout;

  /* Open EP OUT */
  USBD_LL_OpenEP(pdev,
                 MIDI_OUT_EP,
				 USBD_EP_TYPE_BULK,
				 MIDI_OUT_PACKET_SIZE);

  /* Allocate MIDI OUT structure */
  // todo - how to add a second buffer for 2nd enpoint? use the same? create another device handler? ...
  pdev->pClassData = USBD_malloc(sizeof (USBD_MIDI_HandleTypeDef));

  if(pdev->pClassData == NULL)
  {
    return USBD_FAIL;
  }
  else
  {
    hmidiout = (USBD_MIDI_HandleTypeDef*) pdev->pClassData;
    hmidiout->alt_setting = 0;
    hmidiout->offset = AUDIO_OFFSET_UNKNOWN;
    hmidiout->wr_ptr = 0;
    hmidiout->rd_ptr = 0;
    hmidiout->rd_enable = 0;

    /* Initialize the Audio output Hardware layer */
//    if (((USBD_AUDIO_ItfTypeDef *)pdev->pUserData)->Init(USBD_AUDIO_FREQ, AUDIO_DEFAULT_VOLUME, 0) != USBD_OK)
//    {
//      return USBD_FAIL;
//    }

    /* Prepare Out endpoint to receive 1st packet */
    USBD_LL_PrepareReceive(pdev,
					 	 MIDI_OUT_EP,
						 hmidiout->buffer,
						 MIDI_OUT_PACKET);
  }
#endif

  /* Open EP IN */
  USBD_LL_OpenEP(pdev,
                 MIDI_IN_EP,
				 USBD_EP_TYPE_BULK,
				 MIDI_IN_PACKET_SIZE);

  /* Allocate MIDI IN structure */
  pdev->pClassData = USBD_malloc(sizeof (USBD_MIDI_HandleTypeDef));

  if(pdev->pClassData == NULL)
  {
      return USBD_FAIL;
  }
  else
  {
      hmidiin = (USBD_MIDI_HandleTypeDef*) pdev->pClassData;
      hmidiin->alt_setting = 0;
      //hmidiin->offset = MIDI_OFFSET_UNKNOWN;
      hmidiin->wr_ptr = 0;
      hmidiin->rd_ptr = 0;
      hmidiin->rd_enable = 0;

      /* Initialize the Audio output Hardware layer */
  //    if (((USBD_AUDIO_ItfTypeDef *)pdev->pUserData)->Init(USBD_AUDIO_FREQ, AUDIO_DEFAULT_VOLUME, 0) != USBD_OK)
  //    {
  //      return USBD_FAIL;
  //    }

      /* This is how to use the In endpoint to send data: */
      /**
        * @brief  Transmits data over an endpoint.
        * @param  pdev: Device handle
        * @param  ep_addr: Endpoint number
        * @param  pbuf: Pointer to data to be sent
        * @param  size: Data size
        * @retval USBD status
        */
      USBD_StatusTypeDef usb_status = USBD_LL_Transmit(pdev,
    		  MIDI_IN_EP,
			  hmidiin->buffer,
			  MIDI_IN_PACKET_SIZE);
      if(usb_status != USBD_OK) {
    	  // todo - handle error
      }
  }


  return USBD_OK;
}

/**
  * @brief  USBD_AUDIO_Init
  *         DeInitialize the AUDIO layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_MIDI_DeInit (USBD_HandleTypeDef *pdev,
                                 uint8_t cfgidx)
{

#if 0
  /* Close EP OUT */
  USBD_LL_CloseEP(pdev,
              AUDIO_OUT_EP);

  /* DeInit  physical Interface components */
  if(pdev->pClassData != NULL)
  {
   ((USBD_AUDIO_ItfTypeDef *)pdev->pUserData)->DeInit(0);
    USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }
#endif

  /* Close EP IN */
  USBD_LL_CloseEP(pdev,
              MIDI_IN_EP);

  /* DeInit  physical Interface components */
  if(pdev->pClassData != NULL)
  {
	  // ((USBD_MIDI_ItfTypeDef *)pdev->pUserData)->DeInit(0);
	  USBD_free(pdev->pClassData);
	  pdev->pClassData = NULL;
  }

  return USBD_OK;


}

/**
  * @brief  USBD_AUDIO_Setup
  *         Handle the AUDIO specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_MIDI_Setup (USBD_HandleTypeDef *pdev,
                                USBD_SetupReqTypedef *req)
{
	  uint8_t ret = USBD_OK;
#if 0
  USBD_AUDIO_HandleTypeDef   *haudio;
  uint16_t len;
  uint8_t *pbuf;
  haudio = (USBD_AUDIO_HandleTypeDef*) pdev->pClassData;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS :
    switch (req->bRequest)
    {
    case AUDIO_REQ_GET_CUR:
      AUDIO_REQ_GetCurrent(pdev, req);
      break;

    case AUDIO_REQ_SET_CUR:
      AUDIO_REQ_SetCurrent(pdev, req);
      break;

    default:
      USBD_CtlError (pdev, req);
      ret = USBD_FAIL;
    }
    break;

  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_DESCRIPTOR:
      if( (req->wValue >> 8) == AUDIO_DESCRIPTOR_TYPE)
      {
        pbuf = USBD_AUDIO_CfgDesc + 18;
        len = MIN(USB_AUDIO_DESC_SIZ , req->wLength);


        USBD_CtlSendData (pdev,
                          pbuf,
                          len);
      }
      break;

    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev,
                        (uint8_t *)&(haudio->alt_setting),
                        1);
      break;

    case USB_REQ_SET_INTERFACE :
      if ((uint8_t)(req->wValue) <= USBD_MAX_NUM_INTERFACES)
      {
        haudio->alt_setting = (uint8_t)(req->wValue);
      }
      else
      {
        /* Call the error management function (command will be nacked */
        USBD_CtlError (pdev, req);
      }
      break;

    default:
      USBD_CtlError (pdev, req);
      ret = USBD_FAIL;
    }
  }
#endif
  return ret;
}


/**
  * @brief  USBD_MIDI_GetCfgDesc
  *         return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_MIDI_GetCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_MIDI_CfgDesc);
  return USBD_MIDI_CfgDesc;
}

/**
  * @brief  USBD_MIDI_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_MIDI_DataIn (USBD_HandleTypeDef *pdev,
                              uint8_t epnum)
{

  /* Only OUT data are processed */
  return USBD_OK;
}

/**
  * @brief  USBD_AUDIO_EP0_RxReady
  *         handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_MIDI_EP0_RxReady (USBD_HandleTypeDef *pdev)
{
#if 0
  USBD_MIDI_HandleTypeDef   *haudio;
  haudio = (USBD_MIDI_HandleTypeDef*) pdev->pClassData;

  if (haudio->control.cmd == AUDIO_REQ_SET_CUR)
  {/* In this driver, to simplify code, only SET_CUR request is managed */

    if (haudio->control.unit == AUDIO_OUT_STREAMING_CTRL)
    {
     ((USBD_AUDIO_ItfTypeDef *)pdev->pUserData)->MuteCtl(haudio->control.data[0]);
      haudio->control.cmd = 0;
      haudio->control.len = 0;
    }
  }
#endif
  return USBD_OK;
}
/**
  * @brief  USBD_AUDIO_EP0_TxReady
  *         handle EP0 TRx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_MIDI_EP0_TxReady (USBD_HandleTypeDef *pdev)
{
  /* Only OUT control data are processed */
  return USBD_OK;
}

/**
  * @brief  USBD_AUDIO_SOF
  *         handle SOF event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_MIDI_SOF (USBD_HandleTypeDef *pdev)
{
  return USBD_OK;
}


#if 0
/**
  * @brief  USBD_AUDIO_SOF
  *         handle SOF event
  * @param  pdev: device instance
  * @retval status
  */
void  USBD_AUDIO_Sync (USBD_HandleTypeDef *pdev, AUDIO_OffsetTypeDef offset)
{

  int8_t shift = 0;
  USBD_AUDIO_HandleTypeDef   *haudio;
  haudio = (USBD_AUDIO_HandleTypeDef*) pdev->pClassData;

  haudio->offset =  offset;


  if(haudio->rd_enable == 1)
  {
    haudio->rd_ptr += AUDIO_TOTAL_BUF_SIZE/2;

    if (haudio->rd_ptr == AUDIO_TOTAL_BUF_SIZE)
    {
      /* roll back */
      haudio->rd_ptr = 0;
    }
  }

  if(haudio->rd_ptr > haudio->wr_ptr)
  {
    if((haudio->rd_ptr - haudio->wr_ptr) < AUDIO_OUT_PACKET)
    {
      shift = -4;
    }
    else if((haudio->rd_ptr - haudio->wr_ptr) > (AUDIO_TOTAL_BUF_SIZE - AUDIO_OUT_PACKET))
    {
      shift = 4;
    }

  }
  else
  {
    if((haudio->wr_ptr - haudio->rd_ptr) < AUDIO_OUT_PACKET)
    {
      shift = 4;
    }
    else if((haudio->wr_ptr - haudio->rd_ptr) > (AUDIO_TOTAL_BUF_SIZE - AUDIO_OUT_PACKET))
    {
      shift = -4;
    }
  }

  if(haudio->offset == AUDIO_OFFSET_FULL)
  {
    ((USBD_AUDIO_ItfTypeDef *)pdev->pUserData)->AudioCmd(&haudio->buffer[0],
                                                         AUDIO_TOTAL_BUF_SIZE/2 - shift,
                                                         AUDIO_CMD_PLAY);
      haudio->offset = AUDIO_OFFSET_NONE;
  }
}
#endif

/**
  * @brief  USBD_MIDI_IsoINIncomplete
  *         handle data ISO IN Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_MIDI_IsoINIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return USBD_OK;
}
/**
  * @brief  USBD_MIDI_IsoOutIncomplete
  *         handle data ISO OUT Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_MIDI_IsoOutIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return USBD_OK;
}
/**
  * @brief  USBD_MIDI_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_MIDI_DataOut (USBD_HandleTypeDef *pdev,
                              uint8_t epnum)
{

#if 0
  USBD_MIDI_HandleTypeDef   *haudio;
  haudio = (USBD_MIDI_HandleTypeDef*) pdev->pClassData;

  if (epnum == MIDI_OUT_EP)
  {
    /* Increment the Buffer pointer or roll it back when all buffers are full */

    haudio->wr_ptr += AUDIO_OUT_PACKET;

    if (haudio->wr_ptr == AUDIO_TOTAL_BUF_SIZE)
    {/* All buffers are full: roll back */
      haudio->wr_ptr = 0;

      if(haudio->offset == AUDIO_OFFSET_UNKNOWN)
      {
        ((USBD_AUDIO_ItfTypeDef *)pdev->pUserData)->AudioCmd(&haudio->buffer[0],
                                                             AUDIO_TOTAL_BUF_SIZE/2,
                                                             AUDIO_CMD_START);
          haudio->offset = AUDIO_OFFSET_NONE;
      }
    }

    if(haudio->rd_enable == 0)
    {
      if (haudio->wr_ptr == (AUDIO_TOTAL_BUF_SIZE / 2))
      {
        haudio->rd_enable = 1;
      }
    }

    /* Prepare Out endpoint to receive next audio packet */
    USBD_LL_PrepareReceive(pdev,
                           AUDIO_OUT_EP,
                           &haudio->buffer[haudio->wr_ptr],
                           AUDIO_OUT_PACKET);

  }
#endif
  return USBD_OK;
}

/**
  * @brief  MIDI_Req_GetCurrent
  *         Handles the GET_CUR Audio control request.
  * @param  pdev: instance
  * @param  req: setup class request
  * @retval status
  */
#if 0
static void MIDI_REQ_GetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
#if 0
  USBD_MIDI_HandleTypeDef   *haudio;
  haudio = (USBD_MIDI_HandleTypeDef*) pdev->pClassData;

  memset(haudio->control.data, 0, 64);
  /* Send the current mute state */
  USBD_CtlSendData (pdev,
                    haudio->control.data,
                    req->wLength);
#endif
}
#endif

/**
  * @brief  MIDIO_Req_SetCurrent
  *         Handles the SET_CUR Audio control request.
  * @param  pdev: instance
  * @param  req: setup class request
  * @retval status
  */
#if 0
static void MIDI_REQ_SetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
#if 0
  USBD_AUDIO_HandleTypeDef   *haudio;
  haudio = (USBD_AUDIO_HandleTypeDef*) pdev->pClassData;

  if (req->wLength)
  {
    /* Prepare the reception of the buffer over EP0 */
    USBD_CtlPrepareRx (pdev,
                       haudio->control.data,
                       req->wLength);

    haudio->control.cmd = AUDIO_REQ_SET_CUR;     /* Set the request value */
    haudio->control.len = req->wLength;          /* Set the request data length */
    haudio->control.unit = HIBYTE(req->wIndex);  /* Set the request target unit */
  }
#endif
}
#endif

/**
* @brief  DeviceQualifierDescriptor
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
static uint8_t  *USBD_MIDI_GetDeviceQualifierDesc (uint16_t *length)
{
  *length = sizeof (USBD_MIDI_DeviceQualifierDesc);
  return USBD_MIDI_DeviceQualifierDesc;
}

/**
* @brief  USBD_MIDI_RegisterInterface
* @param  fops: Midi interface callback
* @retval status
*/
uint8_t  USBD_MIDI_RegisterInterface  (USBD_HandleTypeDef   *pdev)
                                        //,USBD_MIDI_ItfTypeDef *fops)
{
//  if(fops != NULL)
//  {
//    pdev->pUserData= fops;
//  }
  return 0;
}
/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
#endif

