#include "lp5890.hpp"

#include "errors.hpp"
#include "lp5890/registers.hpp"

using namespace LumiVoxel;
using namespace LumiVoxel::Lp5890;

bool Driver::Init(HighPrecisionCounter& hpc)
{
	if (initialized)
		return true;

	// Initialize the LP5899 driver
	if (!interface.Init(hpc))
	{
		ErrorMessage::WrapMessage("LP5890 - Initialization failed: LP5899 Interface initialization failed");
		return false;
	}

	Lp5899::CcsiControl ccsiControl{
		.CcsiDataRate       = CcsiClock,
		.CcsiSpreadSpectrum = 0
	};

	puts("LP5890 - Setting LP5899 CCSI control register...");
	if (!interface.TryWriteCcsiControl(ccsiControl, true))
	{
		ErrorMessage::WrapMessage("LP5890 - Initialization failed: LP5899 CCSI control write failed");
		return false;
	}

	Lp5899::TxFifoControl txFifoControl{
		.TxFifoLevel = 0x1FF, // 512 word
		.TxFifoClear = 0
	};

	puts("LP5890 - Setting LP5899 TX FIFO control register...");
	if (!interface.TryWriteTxFifoControl(txFifoControl, true))
	{
		ErrorMessage::WrapMessage("LP5890 - Initialization failed: LP5899 TX FIFO control write failed");
		return false;
	}

	Lp5899::RxFifoControl rxFifoControl{
		.RxFifoLevel = 0x1FF, // 512 words
		.RxFifoClear = 0,
	};

	puts("LP5890 - Setting LP5899 RX FIFO control register...");
	if (!interface.TryWriteRxFifoControl(rxFifoControl, true))
	{
		ErrorMessage::WrapMessage("LP5890 - Initialization failed: LP5899 RX FIFO control write failed");
		return false;
	}

	if (!interface.TryClearGlobalStatus(true))
	{
		ErrorMessage::WrapMessage("LP5899 - Soft Reset failed: Failed to clear status register");
		return false;
	}

	HAL_Delay(10);

	puts("LP5890 - Fowarding Soft Reset command...");
	std::array<uint16_t, 1> softReset = { static_cast<uint16_t>(Command::SOFT_RESET) };
	if (!interface.TryForwardWriteData(softReset, false, true))
	{
		ErrorMessage::WrapMessage("LP5890 - Initialization failed: LP5899 Soft reset command failed");
		return false;
	}

	puts("LP5890 - Fowarding Chip Index command...");
	std::array<uint16_t, 1> chipIndex = { static_cast<uint16_t>(Command::CHIP_INDEX_WRITE) };
	if (!interface.TryForwardWriteData(chipIndex, true, true))
	{
		ErrorMessage::WrapMessage("LP5890 - Initialization failed: LP5899 Chip Index command failed");
		return false;
	}

	puts("LP5890 - Writing LED driver configuration registers...");

	puts("LP5890 - Forwarding FC0 register...");
	std::array<uint16_t, 4> fc0 = { static_cast<uint16_t>(Command::FC0_WRITE), fc0Config.Value[2], fc0Config.Value[1], fc0Config.Value[0] };
	if (!interface.TryForwardWriteData(fc0, true, true))
	{
		ErrorMessage::WrapMessage("LP5890 - Initialization failed: LP5890 FC0 register write failed");
		return false;
	}

	puts("LP5890 - Forwarding FC1 register...");
	std::array<uint16_t, 4> fc1 = { static_cast<uint16_t>(Command::FC1_WRITE), fc1Config.Value[2], fc1Config.Value[1], fc1Config.Value[0] };
	if (!interface.TryForwardWriteData(fc1, true, true))
	{
		ErrorMessage::WrapMessage("LP5890 - Initialization failed: LP5890 FC1 register write failed");
		return false;
	}

	puts("LP5890 - Forwarding FC2 register...");
	std::array<uint16_t, 4> fc2 = { static_cast<uint16_t>(Command::FC2_WRITE), fc2Config.Value[2], fc2Config.Value[1], fc2Config.Value[0] };
	if (!interface.TryForwardWriteData(fc2, true, true))
	{
		ErrorMessage::WrapMessage("LP5890 - Initialization failed: LP5890 FC2 register write failed");
		return false;
	}

	puts("LP5890 - Forwarding FC3 register...");
	std::array<uint16_t, 4> fc3 = { static_cast<uint16_t>(Command::FC3_WRITE), fc3Config.Value[2], fc3Config.Value[1], fc3Config.Value[0] };
	if (!interface.TryForwardWriteData(fc3, true, true))
	{
		ErrorMessage::WrapMessage("LP5890 - Initialization failed: LP5890 FC3 register write failed");
		return false;
	}

	puts("LP5890 - Forwarding FC4 register...");
	std::array<uint16_t, 4> fc4 = { static_cast<uint16_t>(Command::FC4_WRITE), fc4Config.Value[2], fc4Config.Value[1], fc4Config.Value[0] };
	if (!interface.TryForwardWriteData(fc4, true, true))
	{
		ErrorMessage::WrapMessage("LP5890 - Initialization failed: LP5890 FC4 register write failed");
		return false;
	}

	return true;
}