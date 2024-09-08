#pragma once

#include <cstdint>
#include "driver.h"

/*
The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct.
*/
struct InitializationCommand{
	uint8_t cmd;
	uint8_t data[16];
	uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
};


class ILI9341Driver : public Driver {
	public:
		ILI9341Driver(uint8_t chipSelectPin, uint8_t dataCommandPin, uint8_t resetPin);

		void writeInitializationCommands() override;

	private:
		static const InitializationCommand initializationCommands[];
};