/**
 * @file stm32_includer.h
 * @author Purdue Solar Racing (Aidan Orr)
 * @brief Generates STM32 board specific includes
 * @version 0.3
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __STM32_INCLUDER_H
#define __STM32_INCLUDER_H

#define STRINGIFY(x)  _STRINGIFY(x)
#define _STRINGIFY(x) #x

#define CAT(x, y)        CAT2(x, y)
#define CAT2(x, y)       x##y
#define CAT3(x, y, z)    CAT(x, CAT2(y, z))
#define CAT4(x, y, z, w) CAT(x, CAT3(y, z, w))

#define INCLUDE_FILE_2(a, b)       STRINGIFY(CAT(a, b))
#define INCLUDE_FILE_3(a, b, c)    STRINGIFY(CAT3(a, b, c))
#define INCLUDE_FILE_4(a, b, c, d) STRINGIFY(CAT4(a, b, c, d))

#define STM32_INCLUDE(__board_name, __header_name) INCLUDE_FILE_4(stm32, __board_name, xx_, __header_name)

#endif // end of include guard for stm32_include.h