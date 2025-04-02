#pragma once

#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_tim.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_rcc.h)

#include <array>
#include <cstdint>
#include <functional>

namespace PSR
{

class HighPrecisionCounter
{
  private:
	TIM_TypeDef* const tim;
	const uint32_t timerPrecision;
	uint64_t upperCount;

	struct DelayedCallback
	{
		uint64_t DelayUntil;
		std::function<void()> Callback;

		DelayedCallback()
			: DelayUntil(0), Callback(nullptr)
		{}
	};

	static constexpr size_t MaxCallbacks = 32;
	std::array<DelayedCallback, MaxCallbacks> delayedCallbacks;
	
	/// @brief The highest index of a delayed callback
	size_t highestCallbackIndex = 0;

	bool isInitialized = false;

	void HandleDelayCallbacks();
	void ClearCallbacks();

  public:
	static constexpr uint32_t MillesecondsToMicroseconds = 1000;

	/**
	 * @brief Construct a new High Precision Counter object
	 *
	 * @param tim The timer peripheral to use for the counter
	 * @param timerPrecision The number of microseconds before the counter rolls over
	 */
	HighPrecisionCounter(TIM_TypeDef* const tim, uint32_t timerPrecision)
		: tim(tim), timerPrecision(timerPrecision), upperCount(0), delayedCallbacks()
	{}

	/**
	 * @brief Initialize the counter
	 *
	 * @return `bool` Whether the counter was initialized successfully, or is already initialized
	 */
	bool Init();

	/**
	 * @brief Update the counter
	 * @param statusRegister The timer status register when the interrupt was triggered
	 * @param suppressCallbacks Whether to suppress the delayed callbacks
	 * @remark This function should be called in the timer interrupt
	 */
	void Update(uint32_t statusRegister, bool suppressCallbacks = false) __attribute__((section(".RamFunc")));

	/**
	 * @brief Get the current count of the timer
	 *
	 * @return `uint64_t` The current count in microseconds
	 */
	uint64_t GetCount() const
	{
		return this->upperCount + this->tim->CNT;
	}

	uint64_t GetUpperCount() const
	{
		return this->upperCount;
	}

	uint32_t GetLowerCount() const
	{
		return this->tim->CNT;
	}

	uint32_t GetPrecision() const
	{
		return this->timerPrecision;
	}

	TIM_TypeDef* GetTimer() const
	{
		return this->tim;
	}

	/**
	 * @brief Reset the counter and clear all callbacks
	 */
	void Reset()
	{
		upperCount = 0;
		tim->CNT   = 0;

		ClearCallbacks();
	}

	/**
	 * @brief Delay for a number of microseconds
	 *
	 * @remark This function will block execution for the specified number of microseconds
	 *
	 * @param microseconds The number of microseconds to delay
	 */
	void Delay(int32_t microseconds)
	{
		if (microseconds < 0)
			return;

		uint64_t start = GetCount();
		uint64_t end   = start + microseconds;
		while (GetCount() < end) {
		}
	}

	/**
	 * @brief Add a callback to be called after a delay
	 *
	 * @remark The callback will be called in a non-interrupt context, after all other interrupts have been handled
	 *
	 * @param delay The delay in ms
	 * @param callback The callback to execute
	 * @return `bool` Whether the callback was added
	 */
	bool AddDelayedCallback(uint32_t delay, const std::function<void()>& callback);
};

} // namespace PSR
