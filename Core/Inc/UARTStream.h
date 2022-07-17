#ifndef _UART_STREAM_H_
#define _UART_STREAM_H_

#include "stm32f4xx.h"
#include "InputStream.h"
#include "OutputStream.h"

typedef struct {
	UART_HandleTypeDef* 		HUART;
	DMA_HandleTypeDef*			RxDMA;
	DMA_HandleTypeDef*			TxDMA;
	IStream						Input;
	OStream						Output;
} UARTStream;

void UARTStream_init(UARTStream* stream, UART_HandleTypeDef* huart, DMA_HandleTypeDef* rxDMA, DMA_HandleTypeDef* txDMA, uint8_t* rxBuff, Stream_LenType rxBuffSize, uint8_t* txBuff, Stream_LenType txBuffSize);

void UARTStream_rxHandle(UARTStream* stream);
void UARTStream_txHandle(UARTStream* stream);

void UARTStream_stopReceiveDMA(UARTStream* stream);
void UARTStream_stopTransmitDMA(UARTStream* stream);

Stream_LenType UARTStream_checkReceivedBytes(IStream* stream);
Stream_LenType UARTStream_checkTransmitedBytes(OStream* stream);
void UARTStream_receive(IStream* stream, uint8_t* buff, Stream_LenType len);
void UARTStream_transmit(OStream* stream, uint8_t* buff, Stream_LenType len);

#endif // _UART_STREAM_H_

