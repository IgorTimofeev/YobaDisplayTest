#pragma once

#include <cstdlib>
#include "../point.h"
#include "../size.h"

class Display;

class Buffer {
	public:
		virtual void setDisplay(Display* display);
		virtual void allocate() = 0;
		virtual void flush() = 0;

	protected:
		Display* _display = nullptr;

		uint8_t* _buffer = nullptr;
		size_t _bufferLength = 0;

	private:

};

template<typename TValue>
class TypedBuffer : public Buffer {
	public:
		virtual void clear(TValue value) = 0;
		virtual void renderPixel(const Point &point, TValue value) = 0;
		virtual void renderHorizontalLine(int32_t x, uint16_t width, TValue value) = 0;
		virtual void renderFilledRectangle(const Point &point, const Size& size, TValue value) = 0;

	private:

};