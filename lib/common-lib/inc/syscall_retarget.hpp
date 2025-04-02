#pragma once
#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)

#define CLEAR_SCREEN "\e[2J"
#define CLEAR_BUFFER "\e[3J"
#define CURSOR_HOME  "\e[H"
#define CLEAR_LINE   "\e[2K"
#define CURSOR_UP(n) "\e[" #n "A"
#define CURSOR_DOWN(n) "\e[" #n "B"

#ifdef __cplusplus
#include <functional>
#include <cstdio>

void SyscallUARTRetarget(UART_HandleTypeDef* huart, uint32_t timeout, std::function<void()> onTxStart, std::function<void()> onTxEnd);

// Debug print
#pragma GCC push_options
#pragma GCC optimize("O3")
template <typename... Args>
__attribute__((always_inline)) static inline void print_debug(const char* str, Args... args)
{
#ifdef PRINT_DEBUG
	printf(str, args...);
#endif
}
#pragma GCC pop_options

extern "C"
{
#endif

int IsRetargeted();

#ifdef __cplusplus
}
#endif
