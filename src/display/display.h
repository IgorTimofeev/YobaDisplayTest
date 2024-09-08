#pragma once

#include "display/drivers/driver.h"
#include "display/buffers/buffer.h"
#include <driver/spi_master.h>
#include "Arduino.h"

class RgbColor {
	public:
		RgbColor() : r(0), g(0), b(0) {}
		RgbColor(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

		uint8_t r;
		uint8_t g;
		uint8_t b;
};

static uint16_t to16Bit(const RgbColor& color) {
	return (color.r >> 3) << 3 | (color.g >> 5) | ((color.g >> 2) << 13) | ((color.b >> 3) << 8);
}

class Display {
	public:
		Display(
			Driver* driver,
			Buffer* buffer,

			uint16_t width,
			uint16_t height
		);

		void begin();

		uint16_t getWidth() const;
		uint16_t getHeight() const;
		Buffer* getBuffer() const;
		Driver* getDriver() const;

	private:
		uint16_t _width;
		uint16_t _height;


		Buffer* _buffer;
		Driver* _driver;
};