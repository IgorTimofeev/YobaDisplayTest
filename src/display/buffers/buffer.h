#pragma once

#include <cstdlib>

class Display;

class Buffer {
	public:
		virtual void flush();
		virtual void setDisplay(Display* display);

	protected:
		Display* _display = nullptr;

		uint8_t* _buffer = nullptr;
		size_t _bufferLength = 0;

	private:

};

template<typename TColor>
class TypedBuffer : public Buffer {
	public:
		virtual void clear(TColor value);
		virtual void renderPixel(int32_t x, int32_t y, TColor value);

	private:

};
