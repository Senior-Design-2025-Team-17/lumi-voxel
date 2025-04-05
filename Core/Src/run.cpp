/**
 * @file run.cpp
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2025
 */

#include "main.h"

#include "errors.hpp"
#include "lp5890.hpp"
#include "lp5899.hpp"
#include "scheduler.hpp"
#include "syscall_retarget.hpp"

#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_def.h"
#include "stm32h7xx_hal_spi.h"
#include "stm32h7xx_hal_uart.h"

#include <cstdio>

using namespace LumiVoxel;

extern "C" UART_HandleTypeDef huart1;
extern "C" SPI_HandleTypeDef hspi1;
extern "C" SPI_HandleTypeDef hspi2;
extern "C" SPI_HandleTypeDef hspi3;

constexpr size_t xSize = 8;
constexpr size_t ySize = 8;
constexpr size_t zSize = 8;

constexpr size_t numLeds = xSize * ySize * zSize;

std::array<float, numLeds> red __attribute__((section(".dtcmram")));
std::array<float, numLeds> green __attribute__((section(".dtcmram")));
std::array<float, numLeds> blue __attribute__((section(".dtcmram")));
float brightness;

Scheduler scheduler(TIM6, 500, 32, 500 * 60);

Lp5899 if1 __attribute__((section(".dtcmram"))) (&hspi1);
Lp5899 if2 __attribute__((section(".dtcmram"))) (&hspi1);

Lp5890::Driver<numLeds / 2> ledDriver1 __attribute__((section(".dtcmram"))) (if1, brightness);
Lp5890::Driver<numLeds / 2> ledDriver2 __attribute__((section(".dtcmram"))) (if2, brightness);

void setup()
{
	SyscallUARTRetarget(&huart1, 100, nullptr, nullptr);

	puts(CLEAR_BUFFER CLEAR_SCREEN CURSOR_HOME "Hello World!\n");

	red.fill(0);
	green.fill(0);
	blue.fill(0);

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
