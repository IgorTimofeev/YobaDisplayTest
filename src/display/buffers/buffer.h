#pragma once

#include "cstdint"

class Display;

class Buffer {
	public:
		virtual void flush();

		virtual void setDisplay(Display* display);

	protected:
		uint8_t* _buffer = nullptr;
		Display* _display = nullptr;

	private:

};