#include "display.h"

Display::Display(
	Driver* driver,
	Buffer* buffer,

	uint16_t width,
	uint16_t height
) :
	_buffer(buffer),
	_driver(driver),

	_width(width),
	_height(height)
{
	_driver->setDisplay(this);
	_buffer->setDisplay(this);
}

void Display::begin() {
	_driver->begin();
	_buffer->allocate();
}

uint16_t Display::getWidth() const {
	return _width;
}

uint16_t Display::getHeight() const {
	return _height;
}

Buffer* Display::getBuffer() const {
	return _buffer;
}

Driver* Display::getDriver() const {
	return _driver;
}