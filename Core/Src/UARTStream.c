#include "UARTStream.h"

void UARTStream_init(UARTStream* stream, UART_HandleTypeDef* huart, DMA_HandleTypeDef* rxDMA, DMA_HandleTypeDef* txDMA, uint8_t* rxBuff, Stream_LenType rxBuffSize, uint8_t* txBuff, Stream_LenType txBuffSize) {
    stream->HUART = huart;
    stream->RxDMA = rxDMA;
    stream->TxDMA = txDMA;
    
    IStream_init(&stream->Input, UARTStream_receive, rxBuff, rxBuffSize);
    IStream_setCheckReceive(&stream->Input, UARTStream_checkReceivedBytes);
    IStream_setArgs(&stream->Input, stream);
    
    OStream_init(&stream->Output, UARTStream_transmit, txBuff, txBuffSize);
    OStream_setCheckTransmit(&stream->Output, UARTStream_checkTransmitedBytes);
    OStream_setArgs(&stream->Output, stream);
}

void UARTStream_rxHandle(UARTStream* stream) {
    IStream_handle(&stream->Input, IStream_incomingBytes(&stream->Input));
}
void UARTStream_txHandle(UARTStream* stream) {
    OStream_handle(&stream->Output, OStream_outgoingBytes(&stream->Output));
}

void UARTStream_stopReceiveDMA(UARTStream* stream) {
	UART_HandleTypeDef* huart = stream->HUART;
	uint32_t dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR);
  if ((huart->RxState == HAL_UART_STATE_BUSY_RX) && dmarequest)
  {
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

    /* Abort the UART DMA Rx stream */
    if (huart->hdmarx != NULL)
    {
      HAL_DMA_Abort(huart->hdmarx);
    }
    /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
		CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
		CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

		/* At end of Rx process, restore huart->RxState to Ready */
		huart->RxState = HAL_UART_STATE_READY;
  }
}
void UARTStream_stopTransmitDMA(UARTStream* stream) {
	UART_HandleTypeDef* huart = stream->HUART;
	uint32_t dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT);
  if ((huart->gState == HAL_UART_STATE_BUSY_TX) && dmarequest)
  {
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

    /* Abort the UART DMA Tx stream */
    if (huart->hdmatx != NULL)
    {
      HAL_DMA_Abort(huart->hdmatx);
    }
    /* Disable TXEIE and TCIE interrupts */
		CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));

		/* At end of Tx process, restore huart->gState to Ready */
		huart->gState = HAL_UART_STATE_READY;
  }
}
Stream_LenType UARTStream_checkReceivedBytes(IStream* stream) {
	UARTStream* uartStream = (UARTStream*) IStream_getArgs(stream);
	return IStream_incomingBytes(stream) - uartStream->RxDMA->Instance->NDTR;
}
Stream_LenType UARTStream_checkTransmitedBytes(OStream* stream) {
    UARTStream* uartStream = (UARTStream*) OStream_getArgs(stream);
	return OStream_outgoingBytes(stream) - uartStream->TxDMA->Instance->NDTR;
}
Stream_Result UARTStream_receive(IStream* stream, uint8_t* buff, Stream_LenType len) {
	UARTStream* uartStream = (UARTStream*) IStream_getArgs(stream);
	UARTStream_stopReceiveDMA(uartStream);
	HAL_UART_Receive_DMA(uartStream->HUART, buff, len);
  return Stream_Ok;
}
Stream_Result UARTStream_transmit(OStream* stream, uint8_t* buff, Stream_LenType len) {
	UARTStream* uartStream = (UARTStream*) OStream_getArgs(stream);
	//UARTStream_stopTransmitDMA(uartStream);
  //while (__HAL_UART_GET_FLAG(uartStream->HUART, UART_FLAG_TXE) == 0);
	HAL_UART_Transmit_DMA(uartStream->HUART, buff, len);
  return Stream_Ok;
}
