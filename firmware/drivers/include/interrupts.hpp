#pragma once

#include "stm32l552xx.h"

#include <cstdint>

typedef void (*IrqHandler_t)(void);

/**
 * @brief Register an IRQ handler.
 *
 * @param[in] irqn    Index of the IRQ.
 * @param[in] handler Handler for the IRQ.
 */
void RegisterIrqHandler(IRQn_Type irqn, IrqHandler_t handler);

/**
 * @brief Deregister an IRQ handler.
 *
 * @param[in] irqn    Index of the IRQ.
 */
void DeregisterIrqHandler(IRQn_Type irqn);
