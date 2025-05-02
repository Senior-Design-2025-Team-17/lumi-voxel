/**
 * @file run.cpp
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2025
 */

#include "main.h"

#include "TriangleMesh.hpp"
#include "app_bluenrg_2.h"

#include "adc_handle.hpp"
#include "adc_ref.hpp"
#include "droplet_animation.hpp"
#include "errors.hpp"
#include "high_precision_counter.hpp"
#include "lp5890.hpp"
#include "lp5890/mappings.hpp"
#include "lp5899.hpp"
#include "rubiks_cube_animation.hpp"
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
extern "C" ADC_HandleTypeDef hadc1;

constexpr size_t xSize = 8;
constexpr size_t ySize = 8;
constexpr size_t zSize = 8;

constexpr size_t numLeds = xSize * ySize * zSize;

std::array<float, numLeds> red __attribute__((section(".dtcmram")));
std::array<float, numLeds> green __attribute__((section(".dtcmram")));
std::array<float, numLeds> blue __attribute__((section(".dtcmram")));
float brightness        = 1.0f;
uint32_t animationIndex = 0;
extern "C"
{
	uint8_t bleConnected    = false;
	uint8_t bleHasConnected = false;
}

TriangleMesh<256> triangleMesh __attribute__((section(".dtcmram")));
DropletAnimation dropletAnimation __attribute__((section(".dtcmram"))) (0.05f, 0.1f, 1.0f, 2.0f, 3, 5, 1, 3);
RubiksCubeAnimation rubiksCubeAnimation __attribute__((section(".dtcmram"))) (0.15f, 0.25f);

std::array<std::reference_wrapper<Animator>, 3> animations = {
	std::ref(triangleMesh),
	std::ref(dropletAnimation),
	std::ref(rubiksCubeAnimation)
};

Scheduler scheduler(TIM6, 500, 10000, 500 * 60);
HighPrecisionCounter hpCounter(TIM7, 10000);

uint32_t brightnessAdcValue;
AdcRef adcRef(&hadc1, scheduler, 0.1f, 1.0f, &brightnessAdcValue, 1, 0xFFFF);
AdcHandle adcHandle(adcRef, brightnessAdcValue);

Lp5890::FC0 fc0 = []() {
	Lp5890::FC0 fc0              = Lp5890::FC0::Default();
	fc0.ChipNumber               = 0;
	fc0.PreDischargeEnable       = 1;
	fc0.PowerSavingEnable        = 0;
	fc0.PowerSavingPlusMode      = 0;
	fc0.LedOpenLoadRemovalEnable = 0;
	fc0.ScanLineNumber           = 16 - 1;
	fc0.SubPeriodNumber          = 0b11;
	fc0.FrequencyMode            = 1;
	fc0.FrequencyMultiplier      = 15 - 1;
	fc0.RedGroupDelay            = 0;
	fc0.GreenGroupDelay          = 0;
	fc0.BlueGroupDelay           = 0;
	fc0.ModuleSize               = 0b01;
	return fc0;
}();
Lp5890::FC1 fc1 = []() {
	Lp5890::FC1 fc1          = Lp5890::FC1::Default();
	fc1.SegmentLength        = 1023;
	fc1.BlackFieldAdjustment = 0;
	return fc1;
}();
Lp5890::FC2 fc2 = []() {
	Lp5890::FC2 fc2              = Lp5890::FC2::Default();
	fc2.RedPreDischargeVoltage   = 0b0110;
	fc2.GreenPreDischargeVoltage = 0b0110;
	fc2.BluePreDischargeVoltage  = 0b0110;
	fc2.RedBrighnessCompensation = 2;
	return fc2;
}();
Lp5890::FC3 fc3 = []() {
	Lp5890::FC3 fc3          = Lp5890::FC3::Default();
	fc3.RedColorBrightness   = 255;
	fc3.GreenColorBrightness = 160;
	fc3.BlueColorBrightness  = 90;
	fc3.GlobalBrightness     = 7;
	// fc3.LedShortRemovalLevel = 15;
	// fc3.RedLedWeakThreshold  = 7;
	// fc3.GreenLedWeakThreshold = 7;
	// fc3.BlueLedWeakThreshold  = 7;
	return fc3;
}();
Lp5890::FC4 fc4 = []() {
	Lp5890::FC4 fc4 = Lp5890::FC4::Default();
	fc4.MaxCurrent  = 1;
	return fc4;
}();

Lp5899 if1 __attribute__((section(".dtcmram"))) (&hspi2, GpioPin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin));
Lp5899 if2 __attribute__((section(".dtcmram"))) (&hspi3, GpioPin(SPI3_NSS_GPIO_Port, SPI3_NSS_Pin));

Lp5890::Driver ledDriver1 __attribute__((section(".dtcmram"))) (if1, brightness, fc0, fc1, fc2, fc3, fc4);
Lp5890::Driver ledDriver2 __attribute__((section(".dtcmram"))) (if2, brightness, fc0, fc1, fc2, fc3, fc4);

std::array<Lp5890::DriverMapping, 512> ledMappings __attribute__((section(".dtcmram"))) = Lp5890::CreateDriverMappings(ledDriver2, ledDriver1);

void UpdateDisplay()
{
	for (size_t i = 0; i < numLeds; ++i)
	{
		Lp5890::DriverMapping& mapping = ledMappings[i];
		mapping.LedDriver.get().SetColor(mapping.Index, red[i], green[i], blue[i]);
	}

	ledDriver1.TryWriteColors();
	ledDriver2.TryWriteColors();

	ledDriver1.TrySendVsync();
	ledDriver2.TrySendVsync();
}

void InitializeCubeAnimation()
{
	constexpr uint64_t delay = 500000;
	red.fill(1.0f);
	green.fill(0.0f);
	blue.fill(0.0f);

	uint64_t count = hpCounter.GetCount();
	while ((hpCounter.GetCount() - count) < delay)
	{
		// wait
		UpdateDisplay();
	}

	red.fill(0.0f);
	green.fill(1.0f);
	blue.fill(0.0f);

	count = hpCounter.GetCount();
	while ((hpCounter.GetCount() - count) < delay)
	{
		// wait
		UpdateDisplay();
	}

	red.fill(0.0f);
	green.fill(0.0f);
	blue.fill(1.0f);

	count = hpCounter.GetCount();
	while ((hpCounter.GetCount() - count) < delay)
	{
		// wait
		UpdateDisplay();
	}

	red.fill(1.0f);
	green.fill(1.0f);
	blue.fill(1.0f);

	count = hpCounter.GetCount();
	while ((hpCounter.GetCount() - count) < delay)
	{
		// wait
		UpdateDisplay();
	}

	red.fill(0.0f);
	green.fill(0.0f);
	blue.fill(0.0f);

	for (size_t i = 0; i < numLeds; ++i)
	{
		red[i]   = 1.0f;
		green[i] = 1.0f;
		blue[i]  = 1.0f;

		UpdateDisplay();
	}

	red.fill(0.0f);
	green.fill(0.0f);
	blue.fill(0.0f);

	UpdateDisplay();
}

extern "C" void BluetoothConnectAnimation()
{
	constexpr uint64_t delay = 500000;

	if (bleHasConnected)
		return;

	red.fill(0.0f);
	green.fill(0.0f);

	uint64_t count = hpCounter.GetCount();
	while ((hpCounter.GetCount() - count) < delay)
	{
		// wait
		blue.fill(1 - (std::cos(3.14 * (hpCounter.GetCount() - count) / 500000.0f) / 2.0f));

		UpdateDisplay();
	}

	blue.fill(0.0f);
	UpdateDisplay();
}

extern "C" void BluetoothConnectionSuccessAnimation()
{
	// constexpr uint64_t delay = 1000000;

	red.fill(0.0f);
	blue.fill(0.0f);
	green.fill(1.0f);
	UpdateDisplay();

	bleConnected   = true;
	bleHasConnected = true;
	animationIndex = 0;
}

extern "C" void SetRainbowPresetColors()
{
	std::array<float, 4 * triangleMesh.MaxVertexCount()> colors;

	std::array<float, 4 * 4> presetColors = {
		1.0f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,
		1.0f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
	};

	for (size_t i = 0; i < triangleMesh.MaxVertexCount(); ++i)
	{
		colors[i * 4 + 0] = presetColors[4 * (i % 4) + 0];
		colors[i * 4 + 1] = presetColors[4 * (i % 4) + 1];
		colors[i * 4 + 2] = presetColors[4 * (i % 4) + 2];
		colors[i * 4 + 3] = presetColors[4 * (i % 4) + 3];
	}

	triangleMesh.AllocateColors(colors);
}

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

	if (!adcRef.Init())
	{
		ErrorMessage::PrintMessage();
		Error_Handler();
	}

	if (!adcHandle.Init())
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

	puts("\nInitializing LP5899 2...");
	if (if2.Init(hpCounter))
		puts("LP5899 2 initialized successfully");
	else
	{
		puts("LP5899 2 initialization failed");
		ErrorMessage::PrintMessage();
		Error_Handler();
	}

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

	// Initialize the LP5890 LED driver 2
	ErrorMessage::ClearMessage();
	puts("\nInitializing LED driver 2...");
	if (ledDriver2.Init(hpCounter))
		puts("LED driver 2 initialized successfully");
	else
	{
		puts("LED driver 2 initialization failed");
		ErrorMessage::PrintMessage();
		Error_Handler();
	}

	InitializeCubeAnimation();

	// Initialize the triangle mesh
	triangleMesh.SetFillColor(Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	SetRainbowPresetColors();
	triangleMesh.SetDrawOptions(DrawOptions::DrawVerticies | DrawOptions::DrawEdges | DrawOptions::ProjectToUnitCube | DrawOptions::Clamp);

	Eigen::Matrix4f transform = Eigen::Matrix4f::Identity();

	triangleMesh.Transform(transform);

	triangleMesh.Rasterize<8, 8, 8>(red, green, blue);

	__HAL_GPIO_EXTI_CLEAR_IT(BLE_EXTI_Pin); // Clear the interrupt flag
	NVIC_EnableIRQ(BLE_EXTI_EXTI_IRQn);

	// Bluetooth
	MX_BlueNRG_2_Init();

	// Turn on the green LED
	GPIOC->MODER &= ~(0b11 << (14 * 2)); // Clear mode bits for pin 14
	GPIOC->MODER |= (0b01 << (14 * 2));  // Set pin 14 to output mode
	GPIOC->BSRR = 1 << 14;
}

extern "C" void run()
{
	setup();

	for (auto& animation : animations)
	{
		if (!animation.get().Init(hpCounter))
		{
			ErrorMessage::PrintMessage();
			Error_Handler();
		}
	}

	while (true)
	{
		InterruptQueue::HandleQueue();

		brightness = adcHandle.GetVoltage() / 3.3f;

		if (animationIndex >= animations.size())
			animationIndex = 0;

		animations[animationIndex].get().Update(hpCounter, red, green, blue);

		UpdateDisplay();

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
