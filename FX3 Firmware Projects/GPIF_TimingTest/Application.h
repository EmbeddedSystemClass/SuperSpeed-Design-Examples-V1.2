
/* This file contains the externals used by the Keyboard application. */

#ifndef _INCLUDED_KEYBOARD_H_
#define _INCLUDED_KEYBOARD_H_

#include "cyu3types.h"
#include "cyu3system.h"
#include "cyu3pib.h"
#include "cyu3gpif.h"
#include "cyu3os.h"
#include "cyu3dma.h"
#include "cyu3error.h"
#include "cyu3usbconst.h"
#include "cyu3lpp.h"
#include "cyu3gpio.h"
#include "cyu3usb.h"
#include "cyu3uart.h"
#include "cyu3externcstart.h"


#define DebugPrint	CyU3PDebugPrint

#define UART_CTS	(54)
#define CPLD_RESET	(27)

// Define Event Flag values
// First 22 Events are defined in cyu3usb.h at "typedef enum CyU3PUsbEventType_t"
#define USB_EVENTS				(0x7FFFFF)
#define USER_COMMAND_AVAILABLE	(23)
#define ANY_EVENT				(USB_EVENTS + (1<<USER_COMMAND_AVAILABLE))

// Define constants for blinking Error LED
#define PWM_PERIOD 			(20000000)	// Approximately 10Hz
#define PWM_THRESHOLD		( 5000000)	// On for 25% of the time

#define STANDARD_REQUEST	(0)		// My values are not shifted
#define CLASS_REQUEST		(1)
#define VENDOR_REQUEST		(2)

#define CY_FX_GPIFTOUSB_DMA_TX_SIZE	(0)       /* DMA transfer size is set to infinite */
#define APPLICATION_THREAD_STACK	(0x1000)
#define APPLICATION_THREAD_PRIORITY	(8)

#define CONSUMER_ENDPOINT			(0x81)    /* EP 1 IN */
#define CONSUMER_ENDPOINT_SOCKET	(CY_U3P_UIB_SOCKET_CONS_1)
#define CPU_PRODUCER_SOCKET			(CY_U3P_CPU_SOCKET_PROD)
#define GPIF_PRODUCER_SOCKET		(CY_U3P_PIB_SOCKET_0)
#define PING_PRODUCER_SOCKET		(CY_U3P_PIB_SOCKET_0)
#define PONG_PRODUCER_SOCKET		(CY_U3P_PIB_SOCKET_1)

/* Burst length in 1 KB packets. Only applicable to USB 3.0. */
#define ENDPOINT_BURST_LENGTH	(16)

/* DMA buffers used by the application. */
#define DMA_BUFFER_SIZE			(16384)
#define DMA_BUFFER_COUNT		(4)

#include "cyu3externcend.h"

#endif /* _INCLUDED_KEYBOARD_H_ */

