/**
 * @file errors.hpp
 * @author Purdue Solar Racing
 * @brief Contains error messages
 * @version 0.1
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <cstring>
#include <memory>

namespace LumiVoxel
{

/// @brief Contains logic for creating and displaying nested error messages
class ErrorMessage
{
  public:
	/// @brief Structure for a single nested error message
	struct Error
	{
		std::shared_ptr<char[]> Message;
		std::shared_ptr<Error> InnerError;

		static constexpr const char* EmptyError = "";

		static void EmptyDeleter(char* message) {}

		Error(const char* message, const std::shared_ptr<Error>& innerError)
			: Message((char*)message, EmptyDeleter), InnerError(innerError)
		{ }

		Error(const std::shared_ptr<char[]> message, const std::shared_ptr<Error>& innerError)
			: Message(message), InnerError(innerError) {}

		Error(const char* message)
			: Error(message, nullptr) {}

		Error(const std::shared_ptr<char[]> message)
			: Error(message, nullptr) {}
	};

  private:
	static std::shared_ptr<Error> error;

	static size_t GetRequiredMessageSize(const std::shared_ptr<Error>& error, size_t depth = 0);
	static std::shared_ptr<char[]> WriteInnerErrors(const std::shared_ptr<Error>& error);

  public:
	/**
	 * @brief Clear the current error messages
	 */
	static void ClearMessage();

	static constexpr const char* GenericErrorMessage = "An Error Has Occurred:";

	/**
	 * @brief Get the current error message
	 * @return const char* The current error message
	 */
	static const char* GetMessage()
	{
		if (error == nullptr)
			return Error::EmptyError; // Return empty error message

		std::shared_ptr<char[]> message = WriteInnerErrors(error);
		if (message == nullptr)
			return Error::EmptyError; // Return empty error message
		
		return message.get();
	}

	/**
	 * @brief Print the current error messages to the serial port, will clear all messages after printing
	 */
	static void PrintMessage();

	/// @brief Set the current error message
	/// @param message A message to set the error. @remark DO NOT POINT TO A STACK ALLOCATED BUFFER.
	static void SetMessage(const char* message)
	{
		error = std::shared_ptr<Error>(new Error(message));
	}
	
	/// @brief Set the current error message
	/// @param message A message to set the error.
	static void SetMessage(const std::shared_ptr<char[]>& message)
	{
		error = std::shared_ptr<Error>(new Error(message));
	}

	/// @brief Wrap the current error message with a new message
	/// @param message A message to wrap the inner error with. @remark DO NOT POINT TO A STACK ALLOCATED BUFFER.
	static void WrapMessage(const char* message)
	{
		error = std::shared_ptr<Error>(new Error(message, error));
	}

	/// @brief Wrap the current error message with a new message
	/// @param message A message to wrap the inner error with.
	static void WrapMessage(const std::shared_ptr<char[]>& message)
	{
		error = std::shared_ptr<Error>(new Error(message, error));
	}
};

} // namespace LumiVoxel