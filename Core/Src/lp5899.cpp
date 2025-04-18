/**
 * @file lp5899.cpp
 * @author Aidan Orr
 * @brief LP5899 LED driver implementation
 * @version 0.1
 *
 * @copyright Copyright (c) 2025
 */

#include "lp5899.hpp"

#include "errors.hpp"
#include "high_precision_counter.hpp"

#include <cstdio>
#include <span>

using namespace LumiVoxel;

static void CrcFailErrorMessage(const char* startMsg, std::span<uint16_t> data, uint16_t crc, uint16_t crc1)
{
	constexpr size_t msgSize        = 512;
	std::shared_ptr<char[]> message = std::shared_ptr<char[]>(new char[msgSize]);

	char dataMsg[128];

	size_t index = 0;
	for (size_t i = 0; i < data.size(); ++i)
		index += snprintf(dataMsg + index, sizeof(dataMsg) - index, "%04X ", data[i]);

	snprintf(message.get(), msgSize, "%s: CRC mismatch: %04X != %04X, Data: %s", startMsg, crc, crc1, dataMsg);

	ErrorMessage::SetMessage(message);
}

static void FlushSPI(SPI_HandleTypeDef* spi, size_t extraReads = 4)
{
	for (size_t i = 0; i < extraReads; ++i)
	{
		uint16_t dummyWrite = 0;
		uint16_t dummyRead  = 0;
		HAL_SPI_TransmitReceive(spi, (uint8_t*)&dummyWrite, (uint8_t*)&dummyRead, 1, HAL_MAX_DELAY);
	}

	while (HAL_SPIEx_FlushRxFifo(spi) != HAL_OK)
	{}
}

bool Lp5899::Init(HighPrecisionCounter& hpc)
{
	if (initialized)
		return true;

	csPin.Set(); // Set CS pin high

	ErrorMessage::ClearMessage();

	HAL_Delay(10);

	// Initialize the SPI interface
	HAL_StatusTypeDef status = HAL_SPI_Init(spi);
	if (status != HAL_OK)
	{
		ErrorMessage::SetMessage("LP5899 - Initialization failed: HAL SPI init failed");
		return false;
	}

	constexpr uint32_t maxRetryTime = 200 * HighPrecisionCounter::MillesecondsToMicroseconds;

	// Wait for the device to be ready
	bool ready       = false;
	uint64_t maxTime = hpc.GetCount() + maxRetryTime;
	int attempt      = 0;
	while (!ready && hpc.GetCount() < maxTime)
	{
		FlushSPI(spi, 32);

		printf("Attempt %d\n", ++attempt);
		HAL_Delay(1);
		csPin.Set(); // Set CS pin high

		if (!TrySoftReset())
		{
			ErrorMessage::WrapMessage("LP5899 - Initialization failed: Soft reset failed");
			ErrorMessage::PrintMessage();
			continue;
		}

		puts("Soft reset successful");

		uint16_t deviceId;
		if (!TryReadRegisterInit(RegisterAddr::DEVID, deviceId) || deviceId != DEVICE_ID)
		{
			ErrorMessage::WrapMessage("LP5899 - Initialization failed: Device ID mismatch");
			ErrorMessage::PrintMessage();
			continue;
		}

		puts("Read device ID successfully");

		GlobalStatus statusRegister;
		if (!TryReadRegisterInit(RegisterAddr::STATUS, statusRegister.Value))
		{
			ErrorMessage::WrapMessage("LP5899 - Initialization failed: Read status register failed");
			ErrorMessage::PrintMessage();
			continue;
		}

		if (statusRegister.GlobalErrorFlag != 0)
		{
			ErrorMessage::WrapMessage("LP5899 - Initialization failed: Global error flag set");
			ErrorMessage::PrintMessage();
			continue;
		}

		if (statusRegister.DeviceState == 0x0)
		{
			ready = true;
			break;
		}
		else if (statusRegister.DeviceState == 0x3)
		{
			ready = false;
			break;
		}
	}

	if (!ready)
	{
		if (hpc.GetCount() < maxTime)
			ErrorMessage::WrapMessage("LP5899 - Initialization failed: Device entered failsafe mode");
		else
			ErrorMessage::WrapMessage("LP5899 - Initialization failed: Device not responding");

		return false;
	}

	ErrorMessage::ClearMessage();

	initialized = true;
	return true;
}

bool Lp5899::TryReadRegisterInit(Lp5899::RegisterAddr reg, uint16_t& value, bool checkCrc)
{
	// FlushSPI(spi, 2);

	uint16_t addr    = static_cast<uint16_t>(reg) << 6;
	uint16_t command = static_cast<uint16_t>(CommandType::REG_RD) | addr;

	std::array<uint16_t, 2> sendData = { command, 0xFFFF };
	std::array<uint16_t, 4> recvData = { 0, 0, 0xFFFF, 0xFFFF };

	uint16_t crc0 = CalculateCrc(std::span((uint8_t*)sendData.data(), 2));
	sendData[1]   = crc0;

	csPin.Reset();

	HAL_StatusTypeDef status1 = HAL_SPI_Transmit(spi, reinterpret_cast<uint8_t*>(sendData.data()), sendData.size(), 100);
	HAL_StatusTypeDef status2 = HAL_SPI_Receive(spi, reinterpret_cast<uint8_t*>(recvData.data()), recvData.size(), 100);

	csPin.Set();

	if (status1 != HAL_OK || status2 != HAL_OK)
	{
		ErrorMessage::SetMessage("LP5899 - Read Register failed: HAL SPI transmit/receive failed");
		return false;
	}

	if (checkCrc)
	{
		uint16_t calculatedCrc1 = CalculateCrc(std::span((uint8_t*)&recvData[0], 2));
		uint16_t crc1           = recvData[1];

		if (calculatedCrc1 != crc1)
		{
			CrcFailErrorMessage("LP5899 - Read Register failed", recvData, calculatedCrc1, crc1);
			return false;
		}
	}

	value = recvData[0];
	return true;
}

bool Lp5899::TryReadRegister(Lp5899::RegisterAddr reg, uint16_t& value, bool checkCrc)
{
	if (!initialized)
	{
		ErrorMessage::SetMessage("LP5899 - Read Register failed: Not initialized");
		return false;
	}

	FlushSPI(spi);

	uint16_t addr    = static_cast<uint16_t>(reg) << 6;
	uint16_t command = static_cast<uint16_t>(checkCrc ? CommandType::REG_RD_CRC : CommandType::REG_RD) | addr;

	std::array<uint16_t, 2> sendData = {
		command,
		0xFFFF,
	};
	std::array<uint16_t, 4> recvData = { 0x0000, 0x0000, 0xFFFF, 0xFFFF };

	uint16_t crc0 = CalculateCrc(std::span((uint8_t*)sendData.data(), 2));
	sendData[2]   = crc0;

	csPin.Reset();

	HAL_StatusTypeDef status1 = HAL_SPI_Transmit(spi, reinterpret_cast<uint8_t*>(sendData.data()), sendData.size(), HAL_MAX_DELAY);
	HAL_StatusTypeDef status2 = HAL_SPI_Receive(spi, reinterpret_cast<uint8_t*>(recvData.data()), recvData.size(), HAL_MAX_DELAY);

	csPin.Set();

	if (status1 != HAL_OK || status2 != HAL_OK)
	{
		ErrorMessage::SetMessage("LP5899 - Read Register failed: HAL SPI transmit/receive failed");
		return false;
	}

	if (checkCrc)
	{
		uint16_t calculatedCrc1 = CalculateCrc(std::span((uint8_t*)&recvData[0], 2));
		uint16_t crc1           = recvData[1];

		if (calculatedCrc1 != crc1)
		{
			CrcFailErrorMessage("LP5899 - Read Register failed", recvData, calculatedCrc1, crc1);
			return false;
		}
	}

	value = recvData[2];
	return true;
}

bool Lp5899::TryWriteRegister(Lp5899::RegisterAddr reg, uint16_t value, bool checkCrc)
{
	if (!initialized)
	{
		ErrorMessage::SetMessage("LP5899 - Write Register failed: Not initialized");
		return false;
	}

	FlushSPI(spi);

	uint16_t addr    = static_cast<uint16_t>(reg) << 6;
	uint16_t command = static_cast<uint16_t>(checkCrc ? CommandType::REG_WR_CRC : CommandType::REG_WR) | addr;

	std::array<uint16_t, 3> sendData = { command, value, 0xFFFF };
	std::array<uint16_t, 4> recvData = { 0x0000, 0x0000, 0xFFFF, 0xFFFF };

	uint16_t crc0 = CalculateCrc(std::span((uint8_t*)sendData.data(), 4));
	sendData[2]   = crc0;

	csPin.Reset();

	HAL_StatusTypeDef status1 = HAL_SPI_Transmit(spi, reinterpret_cast<uint8_t*>(sendData.data()), sendData.size(), HAL_MAX_DELAY);
	HAL_StatusTypeDef status2 = HAL_SPI_Receive(spi, reinterpret_cast<uint8_t*>(recvData.data()), recvData.size(), HAL_MAX_DELAY);

	csPin.Set();

	if (status1 != HAL_OK || status2 != HAL_OK)
	{
		ErrorMessage::SetMessage("LP5899 - Write Register failed: HAL SPI transmit/receive failed");
		return false;
	}

	if (checkCrc)
	{
		uint16_t calculatedCrc1 = CalculateCrc(std::span((uint8_t*)&recvData[0], 2));
		uint16_t crc1           = recvData[1];

		if (calculatedCrc1 != crc1)
		{
			CrcFailErrorMessage("LP5899 - Write Register failed", recvData, calculatedCrc1, crc1);
			return false;
		}
	}

	return true;
}

bool Lp5899::TryWriteRegisterInit(Lp5899::RegisterAddr reg, uint16_t value, bool checkCrc)
{
	FlushSPI(spi);

	uint16_t addr    = static_cast<uint16_t>(reg) << 6;
	uint16_t command = static_cast<uint16_t>(CommandType::REG_WR) | addr;

	std::array<uint16_t, 3> sendData = { command, value, 0xFF };
	std::array<uint16_t, 4> recvData = { 0xFFFF, 0xFFFF };

	uint16_t crc0 = CalculateCrc(std::span((uint8_t*)sendData.data(), 4));
	sendData[2]   = crc0;

	csPin.Reset();

	HAL_StatusTypeDef status1 = HAL_SPI_Transmit(spi, reinterpret_cast<uint8_t*>(sendData.data()), sendData.size(), HAL_MAX_DELAY);
	HAL_StatusTypeDef status2 = HAL_SPI_Receive(spi, reinterpret_cast<uint8_t*>(recvData.data()), recvData.size(), HAL_MAX_DELAY);

	csPin.Set();

	if (status1 != HAL_OK || status2 != HAL_OK)
	{
		ErrorMessage::SetMessage("LP5899 - Write Register failed: HAL SPI transmit/receive failed");
		return false;
	}

	if (checkCrc)
	{
		uint16_t calculatedCrc1 = CalculateCrc(std::span((uint8_t*)&recvData[0], 2));
		uint16_t crc1           = recvData[1];

		if (calculatedCrc1 != crc1)
		{
			CrcFailErrorMessage("LP5899 - Write Register failed", recvData, calculatedCrc1, crc1);
			return false;
		}
	}

	GlobalStatus statusRegister;
	statusRegister.Value = recvData[0];

	return true;
}

bool Lp5899::TrySoftReset()
{
	puts("Soft Resetting LP5899...");

	std::array<uint16_t, 2> sendData = { 0xE1E1, 0xD383 };
	std::array<uint16_t, 4> recvData = { 0xAAAA, 0xAAAA, 0, 0 };

	FlushSPI(spi);

	csPin.Reset();
	// HAL_Delay(1000);
	HAL_StatusTypeDef status1 = HAL_SPI_Transmit(spi, reinterpret_cast<uint8_t*>(sendData.data()), sendData.size(), 100);
	HAL_StatusTypeDef status2 = HAL_SPI_Receive(spi, reinterpret_cast<uint8_t*>(recvData.data()), recvData.size(), 100);
	// HAL_Delay(1000);
	csPin.Set();

	if (status1 != HAL_OK || status2 != HAL_OK)
	{
		ErrorMessage::SetMessage("LP5899 - Soft Reset failed: HAL SPI transmit/receive failed");
		return false;
	}

	GlobalStatus statusRegister;
	statusRegister.Value = recvData[0];

	uint16_t crc  = CalculateCrc(std::span((uint8_t*)&recvData[0], 2));
	uint16_t crc1 = recvData[1];
	if (crc != crc1)
	{
		CrcFailErrorMessage("LP5899 - Soft Reset failed", recvData, crc, crc1);
		return false;
	}

	puts("Soft Reset - Status CRC Match");

	if (statusRegister.DeviceState == 0x3)
	{
		printf("Device state: %d\n", statusRegister.DeviceState);
		ErrorMessage::SetMessage("LP5899 - Soft Reset failed: Device not in normal state after reset");
		return false;
	}

	puts("Soft Reset - Device in normal state");

	statusRegister            = { .Value = 0 };
	statusRegister.ClearFlags = 1;

	if (!TryWriteRegisterInit(RegisterAddr::STATUS, statusRegister.Value, true))
	{
		ErrorMessage::WrapMessage("LP5899 - Soft Reset failed: Failed to clear status register");
		return false;
	}

	SpiControl spiControl          = { .Value = 0 };
	spiControl.SpiResetTimeout     = 0x0;
	spiControl.SpiWatchdogFailsafe = 0x3;
	spiControl.CrcAlgorithm        = 0;

	if (!TryWriteRegisterInit(RegisterAddr::SPICTRL, spiControl.Value, true))
	{
		ErrorMessage::WrapMessage("LP5899 - Soft Reset failed: Failed to write SPI control register");
		return false;
	}

	puts("Soft Reset - SPI control register written successfully");

	return true;
}
