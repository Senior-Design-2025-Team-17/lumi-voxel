/**
 * @file run.cpp
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2025
 */

#include "main.h"

#include "errors.hpp"
#include "scheduler.hpp"
#include "syscall_retarget.hpp"

#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_def.h"
#include "stm32h7xx_hal_spi.h"
#include "stm32h7xx_hal_uart.h"

#include <cstdio>

using namespace PSR;

extern "C" UART_HandleTypeDef huart1;
extern "C" SPI_HandleTypeDef hspi1;
extern "C" SPI_HandleTypeDef hspi2;
extern "C" SPI_HandleTypeDef hspi3;

Scheduler scheduler(TIM6, 500, 32, 500 * 60);

void setup()
{
	SyscallUARTRetarget(&huart1, 100, nullptr, nullptr);

	puts(CLEAR_BUFFER CLEAR_SCREEN CURSOR_HOME "Hello World!\n");

	// Initialize the scheduler
	if (scheduler.Init())
		puts("Scheduler initialized successfully");
	else
	{
		ErrorMessage::PrintMessage();
		Error_Handler();
	}

	// Enable the 3.8V and 2.8V regulators
	REG_EN_GPIO_Port->BSRR = REG_EN_Pin;
	
	// Turn on the green LED
	LED_GREEN_GPIO_Port->BSRR = LED_GREEN_Pin;
}

extern "C" void run()
{
	setup();

	while (true)
		__WFI();
}