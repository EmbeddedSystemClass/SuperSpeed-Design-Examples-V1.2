/*
 * DebugConsole.c
 *
 * I2C_DebugConsole.c
 *
 *  This module implements the DebugPrint portion of cyu3debug.c for an I2C-based console
 *	The LOG function is not implemented which makes this code simpler
 */

#include "Application.h"

// Declare external functions
extern void CheckStatus(char* StringPtr, CyU3PReturnStatus_t Status);

CyBool_t glDebugTxEnabled = CyFalse;	// Set true once I can output messages to the Console
CyU3PDmaChannel glUARTtoCPU_Handle;		// Handle needed by Uart Callback routine
char glConsoleInBuffer[32];				// Buffer for user Console Input
uint32_t glConsoleInIndex;				// Index into ConsoleIn buffer

uint8_t ConsoleInBuffer[32] __attribute__ ((aligned (32)));		// Buffer for characters from I2C

void UartCallback(CyU3PUartEvt_t Event, CyU3PUartError_t Error)
// Handle characters typed in by the developer
// Later we will respond to commands terminated with a <CR>
{
	CyU3PDmaBuffer_t ConsoleInDmaBuffer;
	char InputChar;
	if (Event == CY_U3P_UART_EVENT_RX_DONE)
    {
		CyU3PDmaChannelSetWrapUp(&glUARTtoCPU_Handle);
		CyU3PDmaChannelGetBuffer(&glUARTtoCPU_Handle, &ConsoleInDmaBuffer, CYU3P_NO_WAIT);
		InputChar = (char)*ConsoleInDmaBuffer.buffer;
		CyU3PDebugPrint(4, "%c", InputChar);			// Echo the character
		if (InputChar == 0x0d)
		{
			CyU3PDebugPrint(4, "\r\n");
			if (strcmp("start", glConsoleInBuffer) == 0) CyU3PDebugPrint(4, "\r\nStart");
			else if (strcmp("reset", glConsoleInBuffer) == 0)
			{
				CyU3PDebugPrint(4, "\r\nRESETTING CPU\r\n");
				CyU3PThreadSleep(100);
				CyU3PDeviceReset(CyFalse);
			}
			else CyU3PDebugPrint(4, "\r\nUnknown Command: '%s'", &glConsoleInBuffer[0]);
		}
		else
		{
			glConsoleInBuffer[glConsoleInIndex] = InputChar;
			if (glConsoleInIndex++ < sizeof(glConsoleInBuffer)) glConsoleInBuffer[glConsoleInIndex] = 0;
			else glConsoleInIndex--;
		}
		CyU3PDmaChannelDiscardBuffer(&glUARTtoCPU_Handle);
		CyU3PUartRxSetBlockXfer(1);
    }
}


CyU3PReturnStatus_t InitializeDebugConsole(uint8_t TraceLevel)
{
    CyU3PUartConfig_t uartConfig;
    CyU3PDmaChannelConfig_t dmaConfig;
    CyU3PReturnStatus_t Status = CY_U3P_SUCCESS;

    Status = CyU3PUartInit();										// Start the UART driver
    CheckStatus("CyU3PUartInit", Status);

    CyU3PMemSet ((uint8_t *)&uartConfig, 0, sizeof (uartConfig));
	uartConfig.baudRate = CY_U3P_UART_BAUDRATE_115200;
	uartConfig.stopBit  = CY_U3P_UART_ONE_STOP_BIT;
//r	uartConfig.parity   = CY_U3P_UART_NO_PARITY;
	uartConfig.txEnable = CyTrue;
	uartConfig.rxEnable = CyTrue;
//r	uartConfig.flowCtrl = CyFalse;
	uartConfig.isDma    = CyTrue;
	Status = CyU3PUartSetConfig(&uartConfig, UartCallback);				// Configure the UART hardware
    CheckStatus("CyU3PUartSetConfig", Status);

    Status = CyU3PUartTxSetBlockXfer(0xFFFFFFFF);						// Send as much data as I need to
    CheckStatus("CyU3PUartTxSetBlockXfer", Status);

	Status = CyU3PDebugInit(CY_U3P_LPP_SOCKET_UART_CONS, TraceLevel);	// Attach the Debug driver above the UART driver
	if (Status == CY_U3P_SUCCESS) glDebugTxEnabled = CyTrue;
    CheckStatus("ConsoleOutEnabled", Status);
	CyU3PDebugPreamble(CyFalse);										// Skip preamble, debug info is targeted for a person

	// Now setup a DMA channel to receive characters from the Uart Rx
    Status = CyU3PUartRxSetBlockXfer(1);
    CheckStatus("CyU3PUartRxSetBlockXfer", Status);
	CyU3PMemSet((uint8_t *)&dmaConfig, 0, sizeof(dmaConfig));
	dmaConfig.size  		= 16;									// Minimum size allowed, I only need 1 byte
	dmaConfig.count 		= 1;									// I can't type faster than the Uart Callback routine!
	dmaConfig.prodSckId		= CY_U3P_LPP_SOCKET_UART_PROD;
	dmaConfig.consSckId 	= CY_U3P_CPU_SOCKET_CONS;
	dmaConfig.dmaMode 		= CY_U3P_DMA_MODE_BYTE;
	dmaConfig.notification	= CY_U3P_DMA_CB_PROD_EVENT;
	Status = CyU3PDmaChannelCreate(&glUARTtoCPU_Handle, CY_U3P_DMA_TYPE_MANUAL_IN, &dmaConfig);
    CheckStatus("CreateDebugRxDmaChannel", Status);
    if (Status != CY_U3P_SUCCESS) CyU3PDmaChannelDestroy(&glUARTtoCPU_Handle);
    else
    {
		Status = CyU3PDmaChannelSetXfer(&glUARTtoCPU_Handle, 0);
		CheckStatus("ConsoleInEnabled", Status);
    }
    return Status;
}

