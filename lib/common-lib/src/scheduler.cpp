/**
 * @file scheduler.cpp
 * @author Purdue Solar Racing
 * @brief Contains logic for scheduling tasks on a fixed interval
 * @version 0.1
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "scheduler.hpp"
#include "errors.hpp"

using namespace LumiVoxel;

bool Scheduler::Init()
{
	if (isInitialized)
		return true;

	if (tim == nullptr || frequency == 0 || frequency > HAL_RCC_GetSysClockFreq())
	{
		ErrorMessage::SetMessage("Scheduler: Invalid timer or frequency\n");
		return false;
	}

	tim->CR1 = 0;
	if (!SetTimerFrequency(tim, frequency, timerPrecision))
	{
		ErrorMessage::SetMessage("Scheduler: Required timer precision is too high\n");
		return false;
	}
	tim->DIER |= TIM_DIER_UIE;
	tim->CNT = -1;
	tim->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE;

	tasks.fill(nullptr);
	intervals.fill(0);
	nextUpdates.fill(0);
	enabledTasks.reset();

	isInitialized = true;

	return true;
}

void Scheduler::Update()
{
	if (!isInitialized || paused)
		return;

	if (++counter >= timerRollOver)
		counter = 0;

	// Uses highest task index to avoid iterating through all tasks
	for (size_t i = 0; i < highestTaskIndex; i++)
	{
		std::function<void()>& task = tasks[i];
		if (task == nullptr || !enabledTasks[i])
			continue;

		int32_t diff = counter - nextUpdates[i];

		if (diff >= 0 && diff < (timerPrecision / 2))
		{
			// If the interrupt queue is full, try again next time
			if (!InterruptQueue::AddInterrupt(task))
				continue;
			
			if (intervals[i] == 0)
			{
				task            = nullptr;
				intervals[i]    = 0;
				nextUpdates[i]  = 0;
				enabledTasks[i] = false;
			}
			else
			{
				nextUpdates[i] = GetNextUpdate(counter, timerRollOver, intervals[i]);
			}
		}
	}
}

size_t Scheduler::AddTask(const std::function<void()>& task, uint32_t interval, uint32_t startOffset, bool enabled)
{
	if (startOffset >= timerRollOver || interval >= timerRollOver)
		return std::numeric_limits<size_t>::max();

	for (size_t i = 0; i < MaxTasks; i++)
	{
		if (tasks[i] == nullptr)
		{
			tasks[i]        = task;
			intervals[i]    = interval;
			nextUpdates[i]  = GetFirstUpdate(counter, interval, startOffset);
			enabledTasks[i] = enabled;

			if (i >= highestTaskIndex)
				highestTaskIndex = i + 1; // Update highest task index to the empty slot above

			return i;
		}
	}

	return std::numeric_limits<size_t>::max();
}

bool Scheduler::RemoveTask(size_t index)
{
	if (index >= MaxTasks)
		return false;

	tasks[index]        = nullptr;
	intervals[index]    = 0;
	nextUpdates[index]  = 0;
	enabledTasks[index] = false;

	if (index == highestTaskIndex - 1)
	{
		for (size_t i = highestTaskIndex - 1; i > 0; i--)
		{
			if (tasks[i] != nullptr)
			{
				highestTaskIndex = i + 1;
				break;
			}
		}
	}

	return true;
}