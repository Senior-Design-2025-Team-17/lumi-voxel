/**
 * @file scheduler.hpp
 * @author Purdue Solar Racing (Aidan Orr)
 * @brief Contains logic for scheduling tasks on a fixed interval
 * @version 0.1
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include "interrupt_queue.hpp"
#include "timer_helpers.h"

#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_tim.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_rcc.h)

#include <array>
#include <bitset>
#include <functional>

namespace PSR
{

/**
 * @brief Scheduler class
 */
class Scheduler
{
  private:
	static constexpr size_t MaxTasks = 32;

	/// @brief The timer peripheral to use for the scheduler
	TIM_TypeDef* const tim;
	/// @brief The tasks to run
	/// @remark `nullptr` indicates an empty slot
	std::array<std::function<void()>, MaxTasks> tasks = { nullptr };
	/// @brief The intervals at which to run the tasks
	std::array<uint32_t, MaxTasks> intervals = { 0 };
	/// @brief The offset from zero at which the tasks will run
	std::array<uint32_t, MaxTasks> startOffsets = { 0 };
	/// @brief The counter value when the tasks will run next
	std::array<uint32_t, MaxTasks> nextUpdates = { 0 };
	/// @brief The enabled tasks
	std::bitset<MaxTasks> enabledTasks;

	/// @brief The internal counter used to track the scheduler
	uint32_t counter = 0;

	/// @brief The tick frequency of the scheduler
	const uint32_t frequency;
	/// @brief The timer precision
	const uint32_t timerPrecision;
	/// @brief The number of ticks before the scheduler rolls over
	const uint32_t timerRollOver;

	/// @brief The highest index of a task in the scheduler
	size_t highestTaskIndex = 0;

	/// @brief Whether the scheduler is initialized
	bool isInitialized = false;
	/// @brief Whether the scheduler is paused
	bool paused = false;

	static constexpr uint32_t GetNextUpdate(uint32_t counter, uint32_t rollOver, uint32_t interval)
	{
		uint32_t nextUpdate = counter + interval;
		if (nextUpdate >= rollOver)
			nextUpdate -= rollOver;

		return nextUpdate;
	}

	static constexpr uint32_t GetFirstUpdate(uint32_t counter, uint32_t interval, uint32_t startOffset)
	{
		if (startOffset > counter)
			return startOffset;

		return counter + interval - (counter - startOffset) % interval;
	}

  public:
	/**
	 * @brief Construct a new Scheduler object
	 *
	 * @param tim The timer peripheral to use for the scheduler
	 * @param frequency The tick frequency of the scheduler
	 * @param precision The ARR precision for the timer
	 * @param rollOver The number of ticks before the scheduler rolls over
	 */
	Scheduler(
		TIM_TypeDef* tim,
		uint32_t frequency,
		uint32_t precision = 32,
		uint32_t rollOver  = std::numeric_limits<uint32_t>::max() / 2)
		: tim(tim), frequency(frequency), timerPrecision(precision), timerRollOver(rollOver)
	{}

	/**
	 * @brief Get the maximum number of tasks in the scheduler
	 *
	 * @return `size_t` The maximum number of tasks in the scheduler
	 */
	constexpr size_t Size() const { return MaxTasks; }

	/**
	 * @brief Get the frequency at which the scheduler runs
	 *
	 * @return `uint32_t` The frequency of the scheduler in Hertz
	 */
	uint32_t GetFrequency() const { return frequency; }

	/**
	 * @brief Get the value at which the internal counter will roll over to zero
	 *
	 * @return `uint32_t` The roll over value of the scheduler
	 */
	uint32_t GetRollOverValue() const { return timerRollOver; }

	/**
	 * @brief Get the current value of the internal counter
	 *
	 * @return `uint32_t` The current value of the internal counter
	 */
	uint32_t GetCounter() const { return counter; }

	/**
	 * @brief Initialize the scheduler
	 * @remark This function must be called before adding tasks
	 *
	 * @return `bool` Whether the scheduler was initialized successfully, or is already initialized
	 */
	bool Init();

	/**
	 * @brief Update the scheduler, adding tasks to the interrupt queue when they are due
	 * @remark This function should be called in the timer interrupt
	 */
	void Update() __attribute__((section(".RamFunc")));

	/**
	 * @brief Add a task to the scheduler
	 *
	 * @param task The function to call when the task is due
	 * @param interval The interval in ticks at which to run the task. Zero indicates a one-shot task
	 * @param startOffset The offset from zero at which the task will start to run
	 * @param enabled Whether the task is enabled
	 * @return `size_t` The index of the task in the scheduler, returns `std::numeric_limits<size_t>::max()` if the task could not be added
	 */
	size_t AddTask(const std::function<void()>& task, uint32_t interval, uint32_t startOffset = 0, bool enabled = true);

	/**
	 * @brief Add a task to the scheduler
	 *
	 * @param task The function to call when the task is due
	 * @param interval The interval in seconds at which to run the task. Zero indicates a one-shot task
	 * @param startOffset The offset from zero in seconds at which the task will start to run
	 * @param enabled Whether the task is enabled
	 * @return `size_t` The index of the task in the scheduler, returns `std::numeric_limits<size_t>::max()` if the task could not be added
	 */
	size_t AddTask(const std::function<void()>& task, float interval, float startOffset = 0, bool enabled = true)
	{
		return AddTask(task, static_cast<uint32_t>(interval * frequency), static_cast<uint32_t>(startOffset * frequency), enabled);
	}

	/**
	 * @brief Removes a task from the scheduler
	 *
	 * @param index The index of the task to remove
	 * @return `bool` Whether the task was removed successfully
	 */
	bool RemoveTask(size_t index);

	/**
	 * @brief Enable a task
	 * @remark Does nothing if the task is already enabled, or an empty slot
	 * @param index The index of the task to enable
	 */
	void EnableTask(size_t index)
	{
		if (index >= MaxTasks)
			return;

		enabledTasks[index] = true;
	}

	/**
	 * @brief Disable a task
	 * @remark Does nothing if the task is already disabled, or an empty slot
	 * @param index The index of the task to disable
	 */
	void DisableTask(size_t index)
	{
		if (index >= MaxTasks)
			return;

		enabledTasks[index] = false;
	}

	/**
	 * @brief Get whether a task is enabled
	 *
	 * @param index The index of the task
	 * @return `bool` Whether the task is enabled, returns false if the slot is empty
	 */
	bool GetTaskEnabled(size_t index) const
	{
		if (index >= MaxTasks)
			return false;

		return enabledTasks[index];
	}

	/**
	 * @brief Set the interval of a task
	 * @remark Does nothing if the task is an empty slot
	 * @param index The index of the task
	 * @param interval The new interval in ticks
	 */
	void SetInterval(size_t index, uint32_t interval)
	{
		if (index >= MaxTasks)
			return;

		intervals[index] = interval;
	}

	/**
	 * @brief Get the interval of a task
	 * @remark Returns 0 if the task is an empty slot
	 * @param index The index of the task
	 * @return `uint32_t` The interval in ticks
	 */
	uint32_t GetInterval(size_t index) const
	{
		if (index >= MaxTasks)
			return 0;

		return intervals[index];
	}

	/**
	 * @brief Get whether the scheduler is paused
	 *
	 * @return `bool` Whether the scheduler is paused
	 */
	bool IsPaused() const { return paused; }

	/**
	 * @brief Pause the scheduler
	 */
	void Pause() { paused = true; }

	/**
	 * @brief Resume the scheduler
	 */
	void Resume() { paused = false; }

	/**
	 * @brief Set the paused state of the scheduler
	 *
	 * @param paused Whether the scheduler should be paused
	 */
	void SetPaused(bool paused) { this->paused = paused; }
};

} // namespace PSR