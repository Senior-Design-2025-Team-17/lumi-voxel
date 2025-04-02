#include "errors.hpp"

using namespace PSR;

using Error = ErrorMessage::Error;

std::shared_ptr<Error> ErrorMessage::error = nullptr;

void ErrorMessage::ClearMessage()
{
	if (error != nullptr)
		error = nullptr;
}

size_t ErrorMessage::GetRequiredMessageSize(const std::shared_ptr<Error>& error, size_t depth)
{
	size_t size = depth + strlen(error->Message.get()) + 1; // +depth for tabs, +1 for newline

	if (error->InnerError != nullptr)
		size += GetRequiredMessageSize(error->InnerError, depth + 1);

	return size + 1; // +1 for null terminator
}

static char* WriteInnerErrorsInternal(const std::shared_ptr<Error>& error, char* buffer, size_t depth = 0)
{
	size_t messageLen = strlen(error->Message.get());
	for (size_t i = 0; i < depth; i++)
		*(buffer++) = '\t';

	memcpy(buffer, error->Message.get(), messageLen);
	buffer += messageLen;
	*(buffer++) = '\n';

	if (error->InnerError != nullptr)
		buffer = WriteInnerErrorsInternal(error->InnerError, buffer, depth + 1);
	else
		*(buffer++) = '\0';

	return buffer;
}

std::shared_ptr<char[]> ErrorMessage::WriteInnerErrors(const std::shared_ptr<Error>& error)
{
	if (error == nullptr)
		return nullptr;

	size_t size = GetRequiredMessageSize(error);
	std::shared_ptr<char[]> message(new char[size]);

	WriteInnerErrorsInternal(error, message.get());

	return message;
}

static void PrintInternal(const std::shared_ptr<Error>& error, size_t depth = 0)
{
	constexpr const char* tabString = "\t\t\t\t\t\t\t\tMore inner errors...";
	constexpr size_t tabChars       = 8;
	constexpr size_t takeChars      = 1;

	if (depth >= tabChars / takeChars)
	{
		puts(tabString);
		return;
	}

	printf("%.*s%s\n", takeChars * depth, tabString, error->Message.get());

	if (error->InnerError != nullptr)
		PrintInternal(error->InnerError, depth + 1);
}

void ErrorMessage::PrintMessage()
{
	if (error == nullptr)
		return;

	PrintInternal(error);
	ClearMessage();
}