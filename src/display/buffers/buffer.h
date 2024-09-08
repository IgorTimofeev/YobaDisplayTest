#pragma once

#include <cstdlib>

class Display;

class Buffer {
	public:
		virtual void flush();

		virtual void setDisplay(Display* display);

	protected:
		uint8_t* _buffer = nullptr;
		size_t _bufferLength = 0;

		Display* _display = nullptr;

	private:

};