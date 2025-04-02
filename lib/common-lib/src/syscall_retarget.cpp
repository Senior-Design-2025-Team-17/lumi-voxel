#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_uart.h)
#include "syscall_retarget.hpp"
#include <errno.h>
#include <functional>
#include <stdio.h>
#include <sys/unistd.h>

UART_HandleTypeDef* stdoutUart = nullptr;
uint32_t stdoutTimeout = 0;
std::function<void()> writeOnTxStart;
std::function<void()> writeOnTxEnd;

void SyscallUARTRetarget(UART_HandleTypeDef* huart, uint32_t timeout, std::function<void()> onTxStart, std::function<void()> onTxEnd)
{
	stdoutUart    = huart;
	stdoutTimeout = timeout;

	writeOnTxStart = onTxStart;
	writeOnTxEnd   = onTxEnd;
}

extern "C"
{
	int IsRetargeted()
	{
		return stdoutUart != nullptr;
	}

	int _write(int file, char* ptr, int len)
	{
		if (writeOnTxStart)
			writeOnTxStart();

		if (!(file == STDOUT_FILENO || file == STDERR_FILENO))
		{
			errno = EBADF;
			return -1;
		}

		HAL_StatusTypeDef status = HAL_UART_Transmit(stdoutUart, (uint8_t*)ptr, len, stdoutTimeout);

		if (writeOnTxEnd)
			writeOnTxEnd();
		return status == HAL_OK ? len : 0;
	}

	int _read(int file, char* ptr, int len)
	{
		if (!(file == STDIN_FILENO))
		{
			errno = EBADF;
			return -1;
		}

		HAL_StatusTypeDef status = HAL_UART_Receive(stdoutUart, (uint8_t*)ptr, len, stdoutTimeout);
		return status == HAL_OK ? len : 0;
	}
}
