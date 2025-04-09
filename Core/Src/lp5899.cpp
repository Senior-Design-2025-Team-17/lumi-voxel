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

using namespace LumiVoxel;

bool Lp5899::Init(HighPrecisionCounter& hpc)
{
	if (initialized)
		return true;

	// Initialize the SPI interface
	HAL_StatusTypeDef status = HAL_SPI_Init(spi);
	if (status != HAL_OK)
	{
		ErrorMessage::SetMessage("LP5899 - Initialization failed: HAL SPI init failed");
		return false;
	}

	constexpr uint32_t maxRetryTime = 200 * HighPrecisionCounter::MillesecondsToMicroseconds;

	// Wait for the device to be ready
	bool ready = false;
	uint64_t maxTime = hpc.GetCount() + maxRetryTime;
	do
	{
		GlobalStatus statusRegister;

		if (!TryReadRegisterInit(RegisterAddr::STATUS, statusRegister.Value))
			continue;

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

		if (statusRegister.GlobalErrorFlag != 0)
		{
			ErrorMessage::SetMessage("LP5899 - Initialization failed: Global error flag set");
			return false;
		}

	} while (!ready && hpc.GetCount() < maxTime);

	if (!ready)
	{
		if (hpc.GetCount() < maxTime)
			ErrorMessage::SetMessage("LP5899 - Initialization failed: Device entered failsafe mode");
		else
			ErrorMessage::SetMessage("LP5899 - Initialization failed: Device not responding");

		return false;
	}

	ErrorMessage::ClearMessage();

	uint16_t deviceId;
	if (!TryReadDeviceId(deviceId, false) && deviceId != DEVICE_ID)
	{
		ErrorMessage::WrapMessage("LP5899 - Initialization failed: Device ID mismatch");
		return false;
	}

	initialized = true;
	return true;
}

bool Lp5899::TryReadRegisterInit(Lp5899::RegisterAddr reg, uint16_t& value)
{
	uint16_t addr = static_cast<uint16_t>(reg) << 6;
	uint16_t command = static_cast<uint16_t>(CommandType::REG_RD) | addr;

	std::array<uint16_t, 4> sendData = { command, 0xFF, 0, 0 };
	uint16_t crc0 = CalculateCrc(std::span(sendData.data(), 1));
	sendData[1] = crc0;

	std::array<uint16_t, 4> recvData = { 0, 0, 0xFF, 0xFF };

	HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(spi, reinterpret_cast<uint8_t*>(sendData.data()), reinterpret_cast<uint8_t*>(recvData.data()), sizeof(sendData), 100);

	if (status != HAL_OK)
	{
		ErrorMessage::SetMessage("LP5899 - Read Register failed: HAL SPI transmit/receive failed");
		return false;
	}
	
	value = recvData[2];
	return true;
}

bool Lp5899::TryReadRegister(Lp5899::RegisterAddr reg, uint16_t& value, bool checkCrc)
{
	if (!initialized)
	{
		ErrorMessage::SetMessage("LP5899 - Read Register failed: Not initialized");
		return false;
	}

	uint16_t addr = static_cast<uint16_t>(reg) << 6;
	uint16_t command = static_cast<uint16_t>(checkCrc ? CommandType::REG_RD_CRC : CommandType::REG_RD) | addr;

	std::array<uint16_t, 4> sendData = { command, 0xFF, 0, 0 };
	uint16_t crc0 = CalculateCrc(std::span(sendData.data(), 1));
	sendData[2] = crc0;

	std::array<uint16_t, 4> recvData = { 0, 0, 0xFF, 0xFF };

	HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(spi, reinterpret_cast<uint8_t*>(sendData.data()), reinterpret_cast<uint8_t*>(recvData.data()), sizeof(sendData), HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		ErrorMessage::SetMessage("LP5899 - Read Register failed: HAL SPI transmit/receive failed");
		return false;
	}

	if (checkCrc)
	{
		uint16_t calculatedCrc1 = CalculateCrc(std::span(&recvData[2], 1));

		if (calculatedCrc1 != recvData[3])
		{
			ErrorMessage::SetMessage("LP5899 - Read Register failed: CRC mismatch");
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

	uint16_t addr = static_cast<uint16_t>(reg) << 6;
	uint16_t command = static_cast<uint16_t>(checkCrc ? CommandType::REG_WR_CRC : CommandType::REG_WR) | addr;

	
	std::array<uint16_t, 5> sendData = { command, value, 0xFF, 0, 0 };
	uint16_t crc0 = CalculateCrc(std::span(sendData.data(), 2));
	sendData[2] = crc0;

	std::array<uint16_t, 5> recvData = { 0, 0, 0, 0xFF, 0xFF };

	HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(spi, reinterpret_cast<uint8_t*>(sendData.data()), reinterpret_cast<uint8_t*>(recvData.data()), sizeof(sendData), HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		ErrorMessage::SetMessage("LP5899 - Write Register failed: HAL SPI transmit/receive failed");
		return false;
	}

	if (checkCrc)
	{
		uint16_t calculatedCrc1 = CalculateCrc(std::span(&recvData[3], 1));
		
		if (calculatedCrc1 != recvData[4])
		{
			ErrorMessage::SetMessage("LP5899 - Write Register failed: CRC mismatch");
			return false;
		}
	}

	return true;
}