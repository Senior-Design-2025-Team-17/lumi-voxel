# Common Lib

Includes some frequently used pieces of code for STM32 microcontrollers.

## C headers
- bit_operations.h - Useful extended bitwise operations (set, extract, rotate, reverse endianness, etc.)
- delay.h - Simplified microsecond delay functions
- stm32_includer.h - Simplifies generation of STM32 specific includes
- timer_helpers.h - Helper functions for manipulating and get information from timers

## C++ headers
- errors.hpp - Manages creating and printing nested error messages  
- gpio_pin.hpp - Wrapper class for easily manipulating GPIO pins
- high_precision_counter.hpp - Microsecond counter for measuring time over long periods
- interrupt_queue.hpp - Queue to allow generating callbacks during interrupts that get run in a non-interrupt context
- memory_operations.hpp - Simplified methods for reading and writing from byte arrays
- scheduler.hpp - Class to run tasks at regular intervals
