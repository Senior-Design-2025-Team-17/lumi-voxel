#pragma once

#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)

#include <array>
#include <functional>

namespace PSR
{

class InterruptQueue
{
	static constexpr size_t MaxDepth = 32;
	static std::array<std::function<void()>, MaxDepth> Queue;
	static volatile size_t InterruptsPending;

  public:
	static bool AddInterrupt(const std::function<void()>& callback) __attribute__((section(".RamFunc")));

	static void HandleQueue() __attribute__((section(".RamFunc")));
};

} // namespace PSR
