#include "buffer.h"
#include "../display.h"

void Buffer::setDisplay(Display *display) {
	_display = display;
}

size_t Buffer::getIndex(uint16_t x, uint16_t y) {
	return y * _display->getSize().getWidth() + x;
}

size_t Buffer::getIndex(const Point &point) {
	return getIndex(point.getX(), point.getY());
}


