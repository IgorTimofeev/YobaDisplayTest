#pragma once

#include <cstdlib>
#include "point.h"
#include "size.h"
#include "bounds.h"
#include "drivers/driver.h"

class Display {
	public:
		Display(
			Driver* driver,
			const Size& resolution
		);

		void begin();

		Driver* getDriver() const;

		const Size &getResolution() const;

		Bounds& getViewport();
		void resetViewport();

		virtual void allocate() = 0;
		virtual void flush() = 0;

		size_t getIndex(uint16_t x, uint16_t y) const;
		size_t getIndex(const Point& point);

	protected:
		Driver* _driver;

		uint8_t* _buffer = nullptr;
		size_t _bufferLength = 0;

		const Size _resolution;
		Bounds _viewport = Bounds();
};