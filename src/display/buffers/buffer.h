#pragma once

#include <cstdlib>

class Display;

class Buffer {
	public:
		virtual void flush() {

		}

		virtual void setDisplay(Display* display) {
			_display = display;

			_bufferLength = calculateBufferLength();
			_buffer = new uint8_t[_bufferLength] {};
		}

	protected:
		Display* _display = nullptr;

		uint8_t* _buffer = nullptr;
		size_t _bufferLength = 0;

		virtual size_t calculateBufferLength() {
			return 0;
		}

	private:

};

template<typename TColor>
class TypedBuffer : public Buffer {
	public:
		virtual void clear(TColor value);
		virtual void renderPixel(int32_t x, int32_t y, TColor value);

	private:

};

template<typename TColor>
void TypedBuffer<TColor>::clear(TColor value) {

}

template<typename TColor>
void TypedBuffer<TColor>::renderPixel(int32_t x, int32_t y, TColor value) {

}
