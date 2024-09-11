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
	_buffer->setDisplay(this);
	_driver->setDisplay(this);

	resetViewport();
}

void Display::begin() {
	_buffer->allocate();
	_driver->begin();
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

Bounds &Display::getViewport() {
	return _viewport;
}

void Display::resetViewport() {
	_viewport.setX(0);
	_viewport.setY(0);
	_viewport.setWidth(_size.getWidth());
	_viewport.setHeight(_size.getHeight());
}
