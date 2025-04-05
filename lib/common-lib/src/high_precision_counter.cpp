#include "high_precision_counter.hpp"
#include "interrupt_queue.hpp"
#include "timer_helpers.h"

using namespace LumiVoxel;

void HighPrecisionCounter::Update(uint32_t statusRegister, bool suppressCallbacks)
{
	[[unlikely]] if (!this->isInitialized)
		return;

	if ((statusRegister & TIM_SR_UIF) != 0)
		this->upperCount += this->timerPrecision;

	if (!suppressCallbacks)
		this->HandleDelayCallbacks();
}

bool HighPrecisionCounter::Init()
{
	if (isInitialized)
		return true;

	upperCount         = 0;
	uint32_t clockFreq = GetTimerInputFrequency(tim);

	tim->CR1  = 0;
	tim->DIER = TIM_DIER_UIE;
	tim->PSC  = clockFreq / 1000000 - 1;
	tim->ARR  = timerPrecision - 1;
	tim->CNT  = 0xFFFFFFFF;
	tim->CR1 |= TIM_CR1_CEN | TIM_CR1_ARPE;

	delayedCallbacks.fill(DelayedCallback());

	isInitialized = true;

	return true;
}

void HighPrecisionCounter::HandleDelayCallbacks()
{
	uint64_t count = GetCount();
	for (size_t i = 0; i < highestCallbackIndex; i++)
	{
		DelayedCallback& delayedCallback = delayedCallbacks[i];
		if (delayedCallback.Callback != nullptr && delayedCallback.DelayUntil != 0 && count >= delayedCallback.DelayUntil)
		{
			// If the interrupt queue is full, try again next time
			if (!InterruptQueue::AddInterrupt(delayedCallback.Callback))
				continue;

			delayedCallback.DelayUntil = 0;
			delayedCallback.Callback   = nullptr;

			if (i + 1 >= highestCallbackIndex)
				highestCallbackIndex--;
		}
	}
}

void HighPrecisionCounter::ClearCallbacks()
{
	for (size_t i = 0; i < delayedCallbacks.size(); i++)
	{
		delayedCallbacks[i].DelayUntil = 0;
		delayedCallbacks[i].Callback   = nullptr;
	}
}

bool HighPrecisionCounter::AddDelayedCallback(uint32_t delay, const std::function<void()>& callback)
{
	if (delay == 0 || callback == nullptr)
		return false;

	uint64_t delayUntil = GetCount() + delay * MillesecondsToMicroseconds;

	for (size_t i = 0; i < delayedCallbacks.size(); i++)
	{
		// Search for an empty slot
		DelayedCallback& delayedCallback = delayedCallbacks[i];
		if (delayedCallback.Callback == nullptr || delayedCallback.DelayUntil == 0)
		{
			delayedCallback.DelayUntil = delayUntil;
			delayedCallback.Callback   = callback;

			if (i >= highestCallbackIndex)
				highestCallbackIndex = i + 1;

			return true;
		}
	}

	// No empty slots
	return false;
}