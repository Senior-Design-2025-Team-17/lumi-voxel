/**
 * @file run.cpp
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2025
 */

#include "main.h"

#include "app_bluenrg_2.h"
#include "errors.hpp"
#include "high_precision_counter.hpp"
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
HighPrecisionCounter hpCounter(TIM7, 10000);

Lp5890::FC0 fc0 __attribute__((section(".dtcmram"))) = [] () {
	Lp5890::FC0 fc0 = Lp5890::FC0::Default();
	fc0.ChipNumber = 0;
	fc0.PreDischargeEnable = 0;
	fc0.PowerSavingEnable = 0;
	fc0.PowerSavingPlusMode = 0;
	fc0.LedOpenLoadRemovalEnable = 0;
	fc0.ScanLineNumber = 16 - 1;
	fc0.FrequencyMode = 0;
	fc0.FrequencyMultiplier = 12 - 1;
	fc0.RedGroupDelay = 0;
	fc0.GreenGroupDelay = 0;
	fc0.BlueGroupDelay = 0;
	return fc0;
}();
Lp5890::FC1 fc1 __attribute__((section(".dtcmram"))) = Lp5890::FC1::Default();
Lp5890::FC2 fc2 __attribute__((section(".dtcmram"))) = Lp5890::FC2::Default();
Lp5890::FC3 fc3 __attribute__((section(".dtcmram"))) = Lp5890::FC3::Default();
Lp5890::FC4 fc4 __attribute__((section(".dtcmram"))) = Lp5890::FC4::Default();

Lp5899 if1 __attribute__((section(".dtcmram"))) (&hspi2, GpioPin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin));
Lp5899 if2 __attribute__((section(".dtcmram"))) (&hspi3, GpioPin(SPI3_NSS_GPIO_Port, SPI3_NSS_Pin));

Lp5890::Driver ledDriver1 __attribute__((section(".dtcmram"))) (if1, brightness, fc0, fc1, fc2, fc3, fc4);
// Lp5890::Driver<numLeds / 2> ledDriver2 __attribute__((section(".dtcmram"))) (if2, brightness);

void setup()
{
	NVIC_DisableIRQ(BLE_EXTI_EXTI_IRQn); // Disable to prevent some weird behavior during startup

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

	puts("\nInitializing LP5899 1...");
	if (if1.Init(hpCounter))
		puts("LP5899 1 initialized successfully");
	else
	{
		puts("LP5899 1 initialization failed");
		ErrorMessage::PrintMessage();
		Error_Handler();
	}

	// Add a scheduler task to read the status register at 5Hz
	size_t if1StatusIndex = scheduler.AddTask([]() {
		// puts("LP5899 1 - Reading global status register...");

		Lp5899::GlobalStatus status;
		if (!if1.TryReadGlobalStatus(status, false) || status.GlobalErrorFlag != 0)
		{
			printf("LP5899 1 - Status: %04x\n", status.Value);

			if (status.SpiFlag != 0 || status.CcsiErrorFlag != 0)
			{
				Lp5899::InterfaceStatus interfaceStatus;
				if (if1.TryReadInterfaceStatus(interfaceStatus, true))
					printf("LP5899 1 - Interface status: %04x\n", interfaceStatus.Value);
				else
				{
					ErrorMessage::WrapMessage("LP5899 1 - Failed to read interface status register");
					ErrorMessage::PrintMessage();
				}
			}

			if (status.GlobalErrorFlag == 0)
			{
				ErrorMessage::WrapMessage("LP5899 1 - Failed to read global status register");
				ErrorMessage::PrintMessage();
			}

			if1.TryWriteDeviceControl(Lp5899::DeviceControl{ .ExitFailSafe = 1 }, true);
			if1.TryClearGlobalStatus(true);
		}
	},
	                                          0.2f, 0.0f, true);

	if (if1StatusIndex == std::numeric_limits<size_t>::max())
	{
		puts("Failed to add LP5899 1 status task to scheduler");
		ErrorMessage::PrintMessage();
		Error_Handler();
	}

	puts("LP5899 1 - Status task added to scheduler");

	// puts("\nInitializing LP5899 2...");
	// if (if2.Init(hpCounter))
	// 	puts("LP5899 2 initialized successfully");
	// else
	// {
	// 	puts("LP5899 2 initialization failed");
	// 	ErrorMessage::PrintMessage();
	// 	Error_Handler();
	// }

	// Initialize the LP5890 LED driver 1
	ErrorMessage::ClearMessage();
	puts("\nInitializing LED driver 1...");
	if (ledDriver1.Init(hpCounter))
		puts("LED driver 1 initialized successfully");
	else
	{
		puts("LED driver 1 initialization failed");
		ErrorMessage::PrintMessage();
		Error_Handler();
	}

	NVIC_EnableIRQ(BLE_EXTI_EXTI_IRQn);

	// Bluetooth
	//  MX_BlueNRG_2_Init();

	// Turn on the green LED
	GPIOC->MODER &= ~(0b11 << (14 * 2)); // Clear mode bits for pin 14
	GPIOC->MODER |= (0b01 << (14 * 2));  // Set pin 14 to output mode
	GPIOC->BSRR = 1 << 14;
}

extern "C" void run()
{
	setup();

	while (true)
	{
		InterruptQueue::HandleQueue();
		// MX_BlueNRG_2_Process();
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
