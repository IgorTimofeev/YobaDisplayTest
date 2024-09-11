#include "display.h"

Display::Display(
	Driver* driver,
	Buffer* buffer,
	const Size& size
) :
	_buffer(buffer),
	_driver(driver),
	_size(size)
{
	_driver->setDisplay(this);
	_buffer->setDisplay(this);
}

void Display::begin() {
	Serial.printf("size in begin: %d, %d\n", _size.getWidth(), _size.getHeight());

	_driver->begin();
	_buffer->allocate();
}

Buffer* Display::getBuffer() const {
	return _buffer;
}

Driver* Display::getDriver() const {
	return _driver;
}

const Size& Display::getSize() const {
	return _size;
}
