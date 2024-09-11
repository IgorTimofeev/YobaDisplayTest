#pragma once

#include <cstdint>
#include "driver.h"

class ILI9341DriverSettings : public DriverSettings {
	public:
		ILI9341DriverSettings(uint8_t chipSelectPin, uint8_t dataCommandPin, int8_t resetPin);
};

class ILI9341Driver : public Driver {
	public:
		explicit ILI9341Driver(const ILI9341DriverSettings& settings);

		void writeInitializationCommands() override;

	private:
};