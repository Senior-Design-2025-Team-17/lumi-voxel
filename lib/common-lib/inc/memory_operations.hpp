/**
 * @file memory_operations.hpp
 * @author Purdue Solar Racing (Aidan Orr)
 * @brief Memory operations for reading and writing data
 * @version 0.1
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once

#include <cstddef>
#include <cstdint>

/**
 * @brief Read a value from a byte array
 * 
 * @tparam T The type of the value to read
 * @param data The byte array to read from
 * @param offset The byte offset in the array to read from
 * @return T The value read from the array
 */
template <typename T>
static inline const T& read(const uint8_t* data, size_t offset = 0)
{
	return *reinterpret_cast<const T*>(data + offset);
}

/**
 * @brief Write a value to a byte array
 * 
 * @tparam T The type of the value to write
 * @param value The value to write
 * @param data The byte array to write to
 * @param offset The byte offset in the array to write to
 */
template <typename T>
static inline void write(T value, uint8_t* data, size_t offset = 0)
{
	*reinterpret_cast<T*>(data + offset) = value;
}
