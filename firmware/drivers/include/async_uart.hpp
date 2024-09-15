#pragma once

#include "errno.hpp"
#include "gpio.hpp"
#include "stm32l552xx.h"
#include "system.hpp"

#include <cstdint>
#include <functional>

class AsyncUartInterface {
public:
	virtual ~AsyncUartInterface() {}

	virtual void RxCallback(uint8_t* data, uint32_t size)      = 0;
	virtual void RxErrorCallback(uint8_t* data, uint32_t size) = 0;
	virtual void TxCallback(uint8_t* data, uint32_t size)      = 0;
	virtual void TxErrorCallback(uint8_t* data, uint32_t size) = 0;
};

class AsyncUart {
public:
	enum Error {
		ERROR_BUSY = 1,
		ERROR_TIMEOUT,
		ERROR_INVALID_ARGUMENT,
	};

	using IrqHandler = void (AsyncUart::*)(uint32_t);

	AsyncUart(USART_TypeDef* uart, GPIO_TypeDef* txPort, uint32_t txPin, GPIO_TypeDef* rxPort, uint32_t rxPin, AsyncUartInterface* interface);
	~AsyncUart();

	/**
	 * @brief Transmit data over the UART asynchronously.
	 *
	 * @param[in] data Data to transmit.
	 * @param[in] size Size of the data to transmit.
	 *
	 * @return error_code_t ERROR_NONE if successful. Otherwise, an error
	 *                      code of AsyncUart::Error.
	 */
	error_code_t Transmit(uint8_t* data, uint32_t size);

	/**
	 * @brief Start receiving data over the UART asynchronously.
	 *
	 * @return error_code_t ERROR_NONE if successful. Otherwise, an error
	 *                      code of AsyncUart::Error.
	 */
	error_code_t Start();

private:
	Gpio                    _txPin;
	Gpio                    _rxPin;
	USART_TypeDef*          _uart                        = nullptr;
	AsyncUartInterface*     _interface                   = nullptr;
	DMA_Channel_TypeDef*    _dmaChannel                  = nullptr;
	DMAMUX_Channel_TypeDef* _dmaMux                      = nullptr;
	uint8_t*                _txData                      = nullptr;
	bool volatile _txStarted                             = false;
	uint32_t volatile _txSent                            = 0UL;
	uint32_t volatile _txRemaining                       = 0UL;
	bool volatile _rxStarted                             = false;
	static constexpr uint32_t _RX_BUFFER_SIZE            = 1'024UL;
	uint8_t                   _rxBuffer[_RX_BUFFER_SIZE] = {};
	uint32_t volatile _rxHead                            = 0UL;
	uint32_t volatile _txErrors                          = 0UL;
	uint32_t volatile _rxErrors                          = 0UL;

	void _DisableIrq() {
		if (_uart == USART1) {
			__NVIC_DisableIRQ(USART1_IRQn);
			__NVIC_DisableIRQ(DMA1_Channel1_IRQn);
		} else if (_uart == USART2) {
			__NVIC_DisableIRQ(USART2_IRQn);
			__NVIC_DisableIRQ(DMA1_Channel2_IRQn);
		}
	}

	void _EnableIrq() {
		if (_uart == USART1) {
			__NVIC_EnableIRQ(USART1_IRQn);
			__NVIC_EnableIRQ(DMA1_Channel1_IRQn);
		} else if (_uart == USART2) {
			__NVIC_EnableIRQ(USART2_IRQn);
			__NVIC_EnableIRQ(DMA1_Channel2_IRQn);
		}
	}

public:
	/**
	 * @brief IRQ handler for the UART instance.
	 *
	 * @note DO NOT CALL THIS YOURSELF! This is called internally
	 *       and has to be a public method to be callable by an IRQ
	 *       handler.
	 */
	void _IrqHandler(uint32_t isr);

	/**
	 * @brief Handler for processing new rx data.
	 *
	 * @note DO NOT CALL THIS YOURSELF! This is called internally
	 *       and has to be a public method to be callable by an IRQ
	 *       handler.
	 */
	void _RxHandler();
};
