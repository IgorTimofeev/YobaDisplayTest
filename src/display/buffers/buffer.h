#pragma once

#include <cstdlib>

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
		virtual void clear(TValue value);
		virtual void renderPixel(int32_t x, int32_t y, TValue value);

	private:

};

template<typename TValue>
void TypedBuffer<TValue>::clear(TValue value) {

}

template<typename TValue>
void TypedBuffer<TValue>::renderPixel(int32_t x, int32_t y, TValue value) {

}
