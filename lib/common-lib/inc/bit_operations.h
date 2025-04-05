/**
 * @file bit_operations.h
 * @author Purdue Solar Racing (Aidan Orr)
 * @brief Implementation of common bit-level operations for all integer types
 * @version 0.3
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __BIT_OPERATIONS_H
#define __BIT_OPERATIONS_H

#include <climits>
#include <cstdint>
#include <type_traits>

// Assert that types are integral and not pointers to make warnings and errors more helpful.
#define __ASSERT_INTEGRAL(T) static_assert(std::is_integral<T>::value && !std::is_pointer<T>::value, #T " must be integral.")

namespace LumiVoxel
{

/**
 * @brief Extract a single bit at an index from a value
 *
 * @tparam T the integer type of the value
 * @param value the value to extract bits from
 * @param bit the bit index to extract
 * @return int the extracted bit (1 or 0)
 */
template <typename T>
constexpr int bitExtract(T value, int bit)
{
	__ASSERT_INTEGRAL(T);
	return (int)((value >> bit) & 1);
}

/**
 * @brief Extract a range of bits from a value
 *
 * @tparam T the integer type of the value
 * @param value the value to extract bits from
 * @param index the start index of the bits to extract
 * @param count the number of bits to extract
 * @return T the extracted bits
 */
template <typename T>
constexpr T bitExtract(T value, int index, int count)
{
	__ASSERT_INTEGRAL(T);

	return (value >> index) & ((1ull << count) - 1);
}

/**
 * @brief Tests if a bit specified by an index is set in an integer
 *
 * @tparam T the integer type of the value
 * @param value the value to test
 * @param index the index of the bit to test
 * @return bool whether the bit at the specified index is set
 */
template <typename T>
constexpr bool isBitSet(T value, int index)
{
	__ASSERT_INTEGRAL(T);
	return ((value >> index) & 1) == 1;
}

/**
 * @brief Set the bit at a specified index to 1
 *
 * @tparam T the integer type of the value
 * @param value the value to change
 * @param index the index of the bit to set
 * @return T the modified value
 */
template <typename T>
constexpr T setBit(T value, int index)
{
	__ASSERT_INTEGRAL(T);
	return value | ((T)1 << index);
}

/**
 * @brief Set the bit at a specified index to 0
 *
 * @tparam T the integer type of the value
 * @param value the value to change
 * @param index the index of the bit to clear
 * @return T the modified value
 */
template <typename T>
constexpr T clearBit(T value, int index)
{
	__ASSERT_INTEGRAL(T);
	return value & ~((T)1 << index);
}

/**
 * @brief Perform a circular shift left by a specified number of bits
 *
 * @tparam T the integer type of the value
 * @param value the value to rotate
 * @param amount the number of bits to rotate by
 * @return T the rotated value
 */
template <typename T>
constexpr T rotateLeft(T value, int amount)
{
	__ASSERT_INTEGRAL(T);
	using uT           = typename std::make_unsigned<T>::type;
	constexpr int bits = CHAR_BIT * sizeof(uT);
	uT uv              = (uT)value;

	return (T)((uv << amount) | (uv >> (bits - amount)));
}

/**
 * @brief Perform a circular shift right by a specified number of bits
 *
 * @tparam T the integer type of the value
 * @param value the value to rotate
 * @param amount the number of bits to rotate by
 * @return T the rotated value
 */
template <typename T>
constexpr T rotateRight(T value, int amount)
{
	__ASSERT_INTEGRAL(T);
	using uT           = typename std::make_unsigned<T>::type;
	constexpr int bits = CHAR_BIT * sizeof(uT);
	uT uv              = (uT)value;

	return (T)((uv >> amount) | (uv << (bits - amount)));
}

/**
 * @brief Reverses the order of bytes in the input
 *
 * @tparam T The integer type of the value
 * @param value The value to be reversed
 * @return T The value with byte order reversed
 */
template <typename T>
#if !(defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_AVR)) // Arduino does not like constexpr here
constexpr
#endif
	T
	reverseEndianness(T value)
{
	__ASSERT_INTEGRAL(T);
	using uT           = typename std::make_unsigned<T>::type;
	constexpr int bits = CHAR_BIT * sizeof(uT);
	uT uv              = (uT)value;

	// Most compilers will replace this block with a processor-specific reverse instruction
	uT output = 0;
#pragma GCC unroll 8
	for (unsigned int i = 0; i < sizeof(uT) * CHAR_BIT; i += CHAR_BIT)
	{
		output |= ((uv >> i) & 0xFF) << (bits - CHAR_BIT - i);
	}

	return output;
}

/**
 * @brief Count the number of leading zeros in an integer
 * 
 * @tparam T the integer type of the value
 * @param value the value to count leading zeros in
 * @return int the number of leading zeros
 */
template <typename T>
constexpr int countLeadingZeros(T value)
{
	__ASSERT_INTEGRAL(T);
    return __builtin_clz(value);
}

/**
 * @brief Count the number of trailing zeros in an integer
 * 
 * @tparam T the integer type of the value
 * @param value the value to count trailing zeros in
 * @return int the number of trailing zeros
 */
template <typename T>
constexpr int countTrailingZeros(T value)
{
	__ASSERT_INTEGRAL(T);
    return __builtin_ctz(value);
}

/**
 * @brief Check if a flag is set in an enum
 * 
 * @tparam T the enum type
 * @param value the value to check
 * @param flag the flag to check
 * @return true if the flag is set
 */
template <typename T>
constexpr bool hasFlag(T value, T flag)
{
	static_assert(std::is_enum<T>::value, "T must be an enum type.");

    using uT = typename std::underlying_type<T>::type;

	uT v = static_cast<uT>(value);
	uT f = static_cast<uT>(flag);

	return (v & f) == f;
}

} // namespace LumiVoxel

#endif // End of include guard for bit_opeartions.h
