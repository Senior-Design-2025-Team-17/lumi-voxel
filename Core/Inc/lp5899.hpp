/**
 * @file lp5899.hpp
 * @author Aidan Orr
 * @brief Interface for the LP5899 inteface IC
 * @version 0.1
 * 
 * @details References: https://www.ti.com/lit/ds/symlink/lp5899.pdf
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#include "high_precision_counter.hpp"

#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_spi.h)

#include <span>

namespace LumiVoxel
{

/**
 * @brief LP5899 LED driver class, assumes SPI uses hardware CS pin and word size of 16 bits
 */
class Lp5899
{
	enum struct CommandType : uint16_t
	{
		FWD_WR_CRC   = 0x2 << 12, ///< @brief Forward write CRC
		FWD_WR         = 0x3 << 12, ///< @brief Forward write
		FWD_WR_END_CRC = 0x4 << 12, ///< @brief Forward write CRC with END bytes command
		FWD_WR_END     = 0x5 << 12, ///< @brief Forward write with END bytes command
		FWD_RD_END_CRC = 0x6 << 12, ///< @brief Forward read CRC with END bytes command
		FWD_RD_END     = 0x7 << 12, ///< @brief Forward read with END bytes command
		DATA_RD_CRC    = 0x8 << 12, ///< @brief Data read CRC command
		DATA_RD        = 0x9 << 12, ///< @brief Data read command
		REG_WR_CRC     = 0xA << 12, ///< @brief Register write CRC command
		REG_WR         = 0xB << 12, ///< @brief Register write command
		REG_RD_CRC     = 0xC << 12, ///< @brief Register read CRC command
		REG_RD         = 0xD << 12, ///< @brief Register read command
		SOFTRESET_CRC  = 0xE << 12  ///< @brief Soft reset CRC command
	};

	enum struct RegisterAddr : uint8_t
	{
		DEVID    = 0x0, ///< @brief Device ID register
		SPICTRL  = 0x1, ///< @brief SPI control register
		CCSICTRL = 0x2, ///< @brief CCSI control register
		TXFFLVL  = 0x3, ///< @brief Transmission FIFO level control register
		RXFFLVL  = 0x4, ///< @brief Reception FIFO level control register
		DEVCTRL  = 0x5, ///< @brief Device control register
		DIAGMASK = 0x6, ///< @brief Diagnostic mask register
		STATUS   = 0x7, ///< @brief Global device status register
		IFST     = 0x8, ///< @brief Detail interface status
		TXFFST   = 0x9, ///< @brief Detail transmission FIFO status
		RXFFST   = 0xA, ///< @brief Detail reception FIFO status
	};

	SPI_HandleTypeDef* const spi;

	bool initialized = false;

	static uint16_t CalculateCrc(std::span<uint16_t> data)
	{
		return 0; // TODO: Implement CRC calculation
	}

	bool TryReadRegisterInit(RegisterAddr reg, uint16_t& value);

  public:
	union SpiControl
	{
		struct
		{
			uint16_t DisableSpiAutoReply : 1; ///< @brief Disable SPI auto reply (0 - auto reply enabled, 1 - auto reply disabled)
			uint16_t DisableSpiSdo       : 1; ///< @brief Disable SPI SDO (0 - SDO is driven when CS is low, 1 - SDO is high impedance)
			uint16_t CrcAlgorithm        : 1; ///< @brief CRC algorithm (0 - CCITT-FALSE, 1 - CRC-16/XMODEM)
			uint16_t Reserved3           : 1; ///< @brief Reserved bit 3
			uint16_t SpiResetTimeout     : 4; ///< @brief Watchdog on SCLK to reset SPI after timeout (0 - Disabled, 1 - 500us, 2 - 1ms, 3 - 2ms, 4 - 3ms, 5 - 4ms, 6 - 5ms, 7 - 10ms, 8 - 15ms, 9 - 20ms, 10 - 30ms, 11 - 40ms, 12 - 50ms, 13 - 85ms, 14 - 100ms, 15 - 200ms)
			uint16_t Reserved8           : 4; ///< @brief Reserved bits 11-8
			uint16_t SpiWatchdogFailsafe : 2; ///< @brief Watchdog on SPI into FAILSAFE state (0 - 40ms, 1 - 20ms, 2 - 10ms, 3 - Disabled)
			uint16_t Reserved14          : 2; ///< @brief Reserved bits 15-14
		};

		uint16_t Value;
	};

	union CcsiControl
	{
		struct
		{
			uint16_t CcsiDataRate       : 4; ///< @brief Data rate for CCSI (0 - 1Mbps, 1 - 1.25Mbps, 2 - 1.43Mbps, 3 - 1.67Mbps, 4 - 2Mbps, 5 - 2.22Mbps, 6 - 2.5Mbps, 7 - 2.86Mbps, 8 - 3.33Mbps, 9 - 4Mbps, 10 - 5Mbps, 11 - 6.67Mbps, 12 - 8Mbps, 13 - 10Mbps, 14 - 13.33Mbps, 15 - 20Mpbs)
			uint16_t Reserved4          : 4; ///< @brief Reserved bits 7-4
			uint16_t CcsiSpreadSpectrum : 2; ///< @brief CCSI spread spectrum (0 - Disabled, 1 - 2%, 2 - 4%, 3 - 8%)
			uint16_t Reserved10         : 6; ///< @brief Reserved bits 15-10
		};

		uint16_t Value;
	};

	union TxFifoControl
	{
		struct
		{
			uint16_t TxFifoLevel : 9; ///< @brief Transmission FIFO level for start of transmission on CCSI in words, with 0x0 meaning 1 word
			uint16_t Reserved10  : 6; ///< @brief Reserved bits 14-10
			uint16_t TxFifoClear : 1; ///< @brief Write 1 to clear the transmission FIFO (0 - No clear, 1 - Clear)
		};

		uint16_t Value;
	};

	union RxFifoControl
	{
		struct
		{
			uint16_t RxFifoLevel : 9; ///< @brief Reception FIFO level to pull down the DRDY pin when number of words is exceeded, with 0x0 meaning 1 word
			uint16_t Reserved10  : 6; ///< @brief Reserved bits 14-10
			uint16_t RxFifoClear : 1; ///< @brief Write 1 to clear the reception FIFO (0 - No clear, 1 - Clear)
		};

		uint16_t Value;
	};

	union DeviceControl
	{
		struct
		{
			uint16_t ExitFailSafe  : 1;  ///< @brief Write 1 to exit FAILSAFE state (0 - No exit, 1 - Exit)
			uint16_t Reserved1     : 1;  ///< @brief Reserved bit 1
			uint16_t ForceFailSafe : 1;  ///< @brief Write 1 to force FAILSAFE state (0 - No force, 1 - Force)
			uint16_t Reserved3     : 13; ///< @brief Reserved bits 15-3
		};

		uint16_t Value;
	};

	union DiagnosticMask
	{
		struct
		{
			uint16_t MaskSpiCrc       : 1; ///< @brief Mask SPI CRC fault (0 - Fault enabled, 1 - Fault masked)
			uint16_t Reserved1        : 7; ///< @brief Reserved bits 7-1
			uint16_t MaskCcsiSin      : 1; ///< @brief Mask CCSI SIN stuck-at fault (0 - Fault enabled, 1 - Fault masked)
			uint16_t MaskCcsiCrc      : 1; ///< @brief Mask CCSI CRC fault (0 - Fault enabled, 1 - Fault masked)
			uint16_t Reserved10       : 1; ///< @brief Reserved bit 10
			uint16_t MaskCcsiCheckBit : 1; ///< @brief Mask CCSI check bit fault (0 - Fault enabled, 1 - Fault masked)
			uint16_t Reserved12       : 3; ///< @brief Reserved bits 15-12
		};

		uint16_t Value;
	};

	union GlobalStatus
	{
		struct
		{
			uint16_t GlobalErrorFlag       : 1; ///< @brief Global error flag (0 - No error, 1 - one or more errors)
			uint16_t PowerOnResetFlag      : 1; ///< @brief Power on reset flag (0 - No POR triggered, 1 - POR triggered)
			uint16_t OscillatorFlag        : 1; ///< @brief Oscillator out of range flag (0 - No oscillator error, 1 - Oscillator error)
			uint16_t OtpCrcFlag            : 1; ///< @brief OTP CRC error flag (0 - No OTP CRC error, 1 - OTP CRC error)
			uint16_t DeviceState           : 2; ///< @brief Device state (0 - Normal, 1 - Init, 2 - Init, 3 - Failsafe)
			uint16_t SpiCrcFlag            : 1; ///< @brief SPI CRC error flag (0 - No SPI CRC error, 1 - SPI CRC error)
			uint16_t SpiRegisterWriteFlag  : 1; ///< @brief Unsuccessful SPI register write command. CCSICTRL cannot be written while CCSI is transmitting or receiving (0 - No error, 1 - Error)
			uint16_t SpiFlag               : 1; ///< @brief SPI error flag (0 - No error, 1 - Error)
			uint16_t SoftResetFlag         : 1; ///< @brief Unsuccessful Soft Reset command. Soft reset cannot be executed while CCSI is transmitting (0 - No error, 1 - Error)
			uint16_t DataReadyFlag         : 1; ///< @brief Data ready flag (0 - Data ready is logic low, 1 - Data ready is logic high)
			uint16_t ReceiveFifoErrorFlag  : 1; ///< @brief Reception FIFO error flag (0 - No error, 1 - Error, check TXFFST for more details)
			uint16_t TransmitFifoErrorFlag : 1; ///< @brief Transmission FIFO error flag (0 - No error, 1 - Error, check RXFFST for more details)
			uint16_t Reserved13            : 1; ///< @brief Reserved bit 13
			uint16_t CcsiErrorFlag         : 1; ///< @brief CCSI error flag (0 - No error, 1 - Error, check IFST for more details)
			uint16_t ClearFlags            : 1; ///< @brief Write 1 to clear all flags (0 - No clear, 1 - Clear)
		};

		uint16_t Value;
	};

	union InterfaceStatus
	{
		struct
		{
			uint16_t CcsiSinFlag                  : 1; ///< @brief Missing toggling on SIN pin (0 - No error, 1 - Error)
			uint16_t CcsiCrcFlag                  : 1; ///< @brief CRC error detected for CCSI data (0 - No error, 1 - Error)
			uint16_t Reserved2                    : 1; ///< @brief Reserved bit 2
			uint16_t CcsiCheckBitFlag             : 1; ///< @brief CCSI check bit error has been detected for CCSI received data (0 - No error, 1 - Error)
			uint16_t CcsiCommandQueueOverflowFlag : 1; ///< @brief CCSI command queue overflow (0 - No error, 1 - Error)
			uint16_t Reserved5                    : 3; ///< @brief Reserved bits 7-5
			uint16_t SpiTimeoutFlag               : 1; ///< @brief SPI timeout error, only set when SPI_RST_TIMEOUT_CFG is enabled (0 - No error, 1 - Error)
			uint16_t SpiCsFlag                    : 1; ///< @brief SPI chip select was pulled high in the middle of reception command (0 - No error, 1 - Error)
			uint16_t Reserved10                   : 6; ///< @brief Reserved bits 15-10
		};

		uint16_t Value;
	};

	union TransmissionFifoStatus
	{
		struct
		{
			uint16_t TxFifoLevel     : 9; ///< @brief Transmission FIFO level in words (0x0 - empty, 0x1 - 1 word, 0x2 - 2 words, ..., 0x1FE - 510 words, 0x1FF - 511 or 512 words)
			uint16_t Reserved9       : 4; ///< @brief Reserved bits 12-9
			uint16_t SingleErrorFlag : 1; ///< @brief Single error detection on transmission FIFO (0 - No error, 1 - Error)
			uint16_t UnderflowFlag   : 1; ///< @brief Underflow error on transmission FIFO (0 - No error, 1 - Error)
			uint16_t OverflowFlag    : 1; ///< @brief Overflow error on transmission FIFO (0 - No error, 1 - Error)
		};

		uint16_t Value;
	};

	union ReceptionFifoStatus
	{
		struct
		{
			uint16_t RxFifoLevel     : 8; ///< @brief Reception FIFO level in words (0x0 - empty, 0x1 - 1 word, 0x2 - 2 words, ..., 0xFE - 254 words, 0xFF - 255 or 256 words)
			uint16_t Reserved8       : 5; ///< @brief Reserved bits 12-8
			uint16_t SingleErrorFlag : 1; ///< @brief Single error detection on reception FIFO (0 - No error, 1 - Error)
			uint16_t UnderflowFlag   : 1; ///< @brief Underflow error on reception FIFO (0 - No error, 1 - Error)
			uint16_t OverflowFlag    : 1; ///< @brief Overflow error on reception FIFO (0 - No error, 1 - Error)
		};

		uint16_t Value;
	};

	static constexpr uint16_t DEVICE_ID = 0xED99; ///< @brief Device ID for LP5899

	/**
	 * @brief Construct a new Lp5899 object
	 *
	 * @param spi A pointer to the SPI handle
	 */
	Lp5899(SPI_HandleTypeDef* spi) : spi(spi)
	{}

	/**
	 * @brief Initialize the LP5899 driver
	 * @param hpc A reference to a high precision counter object
	 *
	 * @return bool true if initialization was successful or already initialized, false otherwise
	 */
	bool Init(HighPrecisionCounter& hpc);

	bool TrySendCommand(uint16_t command, std::span<uint8_t> data);

	bool TryReadRegister(RegisterAddr reg, uint16_t& value, bool crc = false);
	bool TryReadRegisterMultiple(RegisterAddr startAddr, size_t count, std::span<uint16_t> values, bool crc = false);
	bool TryWriteRegister(RegisterAddr reg, uint16_t value, bool crc = false);
	bool TryWriteRegisterMultiple(RegisterAddr startAddr, size_t count, std::span<uint16_t> values, bool crc = false);

	bool TryReadDeviceId(uint16_t& deviceId, bool crc = false) { return TryReadRegister(RegisterAddr::DEVID, deviceId, crc); }

	bool TryReadSpiControl(SpiControl& spiControl, bool crc = false) { return TryReadRegister(RegisterAddr::SPICTRL, spiControl.Value, crc); }
	bool TryWriteSpiControl(SpiControl spiControl, bool crc = false) { return TryWriteRegister(RegisterAddr::SPICTRL, spiControl.Value, crc); }

	bool TryReadCcsiControl(CcsiControl& ccsiControl, bool crc = false) { return TryReadRegister(RegisterAddr::CCSICTRL, ccsiControl.Value, crc); }
	bool TryWriteCcsiControl(CcsiControl ccsiControl, bool crc = false) { return TryWriteRegister(RegisterAddr::CCSICTRL, ccsiControl.Value, crc); }

	bool TryReadTxFifoControl(TxFifoControl& txFifoControl, bool crc = false) { return TryReadRegister(RegisterAddr::TXFFLVL, txFifoControl.Value, crc); }
	bool TryWriteTxFifoControl(TxFifoControl txFifoControl, bool crc = false) { return TryWriteRegister(RegisterAddr::TXFFLVL, txFifoControl.Value, crc); }

	bool TryReadRxFifoControl(RxFifoControl& rxFifoControl, bool crc = false) { return TryReadRegister(RegisterAddr::RXFFLVL, rxFifoControl.Value, crc); }
	bool TryWriteRxFifoControl(RxFifoControl rxFifoControl, bool crc = false) { return TryWriteRegister(RegisterAddr::RXFFLVL, rxFifoControl.Value, crc); }

	bool TryReadDeviceControl(DeviceControl& deviceControl, bool crc = false) { return TryReadRegister(RegisterAddr::DEVCTRL, deviceControl.Value, crc); }
	bool TryWriteDeviceControl(DeviceControl deviceControl, bool crc = false) { return TryWriteRegister(RegisterAddr::DEVCTRL, deviceControl.Value, crc); }

	bool TryReadDiagnosticMask(DiagnosticMask& diagnosticMask, bool crc = false) { return TryReadRegister(RegisterAddr::DIAGMASK, diagnosticMask.Value, crc); }
	bool TryWriteDiagnosticMask(DiagnosticMask diagnosticMask, bool crc = false) { return TryWriteRegister(RegisterAddr::DIAGMASK, diagnosticMask.Value, crc); }

	bool TryReadGlobalStatus(GlobalStatus& globalStatus, bool crc = false) { return TryReadRegister(RegisterAddr::STATUS, globalStatus.Value, crc); }
	bool TryClearGlobalStatus(bool crc = false) { return TryWriteRegister(RegisterAddr::STATUS, (GlobalStatus){ .ClearFlags = 1 }.Value, crc); }

	bool TryReadInterfaceStatus(InterfaceStatus& interfaceStatus, bool crc = false) { return TryReadRegister(RegisterAddr::IFST, interfaceStatus.Value, crc); }
	bool TryReadTransmissionFifoStatus(TransmissionFifoStatus& transmissionFifoStatus, bool crc = false) { return TryReadRegister(RegisterAddr::TXFFST, transmissionFifoStatus.Value, crc); }
	bool TryReadReceptionFifoStatus(ReceptionFifoStatus& receptionFifoStatus, bool crc = false) { return TryReadRegister(RegisterAddr::RXFFST, receptionFifoStatus.Value, crc); }

}; // class Lp5899

} // namespace LumiVoxel
