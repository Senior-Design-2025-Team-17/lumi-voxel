/**
 * @file run.cpp
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2025
 */

#include "main.h"

#include "errors.hpp"
#include "high_precision_counter.hpp"
#include "lp5890.hpp"
#include "lp5899.hpp"
#include "scheduler.hpp"
#include "syscall_retarget.hpp"
#include "app_bluenrg_2.h"

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
HighPrecisionCounter hpCounter(TIM7, 10000);

Lp5899 if1 __attribute__((section(".dtcmram"))) (&hspi2, GpioPin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin));
// Lp5899 if2 __attribute__((section(".dtcmram"))) (&hspi3);

Lp5890::Driver<numLeds / 2> ledDriver1 __attribute__((section(".dtcmram"))) (if1, brightness);
// Lp5890::Driver<numLeds / 2> ledDriver2 __attribute__((section(".dtcmram"))) (if2, brightness);

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

	// Initialize the high precision counter
	if (hpCounter.Init())
		puts("High precision counter initialized successfully");
	else
	{
		ErrorMessage::PrintMessage();
		Error_Handler();
	}

	// Enable the 3.8V and 2.8V regulators
	REG_EN_GPIO_Port->BSRR = REG_EN_Pin;

	if (if1.Init(hpCounter))
		puts("LP5899 1 initialized successfully");
	else
	{
		ErrorMessage::PrintMessage();
		Error_Handler();
	}

	// Initialize the LP5890 LED driver 1
	if (ledDriver1.Init(hpCounter))
		puts("LED driver 1 initialized successfully");
	else
	{
		ErrorMessage::PrintMessage();
		Error_Handler();
	}

	//Bluetooth
	MX_BlueNRG_2_Init();

	// Turn on the green LED
//	LED_GREEN_GPIO_Port->BSRR = LED_GREEN_Pin;
}

extern "C" void run()
{
	setup();

	while (true)
	{
		// __WFI();
		MX_BlueNRG_2_Process();
	}
}

extern "C" void TIM6_DAC_IRQHandler(void)
{
	if (TIM6->SR & TIM_SR_UIF)
	{
		TIM6->SR &= ~TIM_SR_UIF; // Clear the update interrupt flag
		scheduler.Update();
	}
}

extern "C" void TIM7_IRQHandler(void)
{
	uint32_t sr = TIM7->SR;
	if (TIM7->SR & TIM_SR_UIF)
	{
		hpCounter.Update(sr);
		sr &= ~TIM_SR_UIF; // Clear the update interrupt flag
	}

	TIM7->SR = sr;
}

