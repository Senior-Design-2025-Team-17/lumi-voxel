#include "interrupt_queue.hpp"

#include "critical_section.h"

#include <cstdio>

using namespace PSR;

std::array<std::function<void()>, InterruptQueue::MaxDepth> InterruptQueue::Queue;
volatile size_t InterruptQueue::InterruptsPending = 0;

bool InterruptQueue::AddInterrupt(const std::function<void()>& callback)
{
	if (InterruptQueue::InterruptsPending >= InterruptQueue::MaxDepth)
		return false;

	// Disable interrupts while modifying the queue
	uint32_t primask = EnterCriticalSection();

	Queue[InterruptQueue::InterruptsPending] = callback;
	InterruptQueue::InterruptsPending        = InterruptQueue::InterruptsPending + 1;

	// Re-enable interrupts
	ExitCriticalSection(primask);

	return true;
}

void InterruptQueue::HandleQueue()
{	
	if ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0)
		return;	// This should never be called from an interrupt, so if it is, return

	if (InterruptsPending > 0)
	{
		for (size_t i = 0; i < InterruptsPending; i++)
		{
			const std::function<void()>& interrupt = InterruptQueue::Queue[i];
			if (interrupt)
			{
				interrupt();

				uint32_t primask         = EnterCriticalSection();
				InterruptQueue::Queue[i] = nullptr;
				ExitCriticalSection(primask);
			}
		}

		InterruptsPending = 0;
	}
}
