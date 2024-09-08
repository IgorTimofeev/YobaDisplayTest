#pragma once

#include <cstdint>
#include "driver.h"

class ILI9341Driver : public Driver {
	public:
		ILI9341Driver(uint8_t chipSelectPin, uint8_t dataCommandPin, uint8_t resetPin);

		void writeInitializationCommands() override;

	private:
};