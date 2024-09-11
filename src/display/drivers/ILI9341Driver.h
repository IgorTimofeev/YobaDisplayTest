#pragma once

#include <cstdint>
#include "driver.h"

class ILI9341Driver : public Driver {
	public:
		ILI9341Driver(
			uint8_t chipSelectPin,
			uint8_t dataCommandPin,
			uint8_t resetPin,

			int32_t SPIFrequency = SPI_MASTER_FREQ_26M,
			uint8_t transactionBufferHeight = 40
		);

		void writeInitializationCommands() override;

	private:
};