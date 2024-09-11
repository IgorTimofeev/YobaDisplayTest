#pragma once

#include <cstdlib>
#include "../point.h"
#include "../size.h"
#include "display/bounds.h"

class Display;

class Buffer {
	public:
		virtual void setDisplay(Display* display);
		virtual void allocate() = 0;
		virtual void flush() = 0;

		size_t getIndex(uint16_t x, uint16_t y);
		size_t getIndex(const Point& point);

	protected:
		Display* _display = nullptr;

		uint8_t* _buffer = nullptr;
		size_t _bufferLength = 0;

	private:

};