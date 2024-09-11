#pragma once

#include "display/drivers/driver.h"
#include "display/buffers/buffer.h"
#include <driver/spi_master.h>
#include "Arduino.h"
#include "color.h"
#include "size.h"

class Display {
	public:
		Display(
			Driver* driver,
			Buffer* buffer,
			const Size& size
		);

		void begin();

		Buffer* getBuffer() const;
		Driver* getDriver() const;
		const Size &getSize() const;

	private:
		Size _size;

		Buffer* _buffer;
		Driver* _driver;
};