#include "display.h"
#include "display.h"

Display::Display(
	Driver* driver,
	const Size& resolution
) :
	_driver(driver),
	_resolution(resolution)
{
	resetViewport();
}

void Display::begin() {
	Serial.println("allocate()");
	allocate();

	Serial.println("_driver begin()");

	_driver->begin(this);
}

Driver* Display::getDriver() const {
	return _driver;
}

const Size& Display::getResolution() const {
	return _resolution;
}

Bounds &Display::getViewport() {
	return _viewport;
}

void Display::resetViewport() {
	_viewport.setX(0);
	_viewport.setY(0);
	_viewport.setWidth(_resolution.getWidth());
	_viewport.setHeight(_resolution.getHeight());
}

size_t Display::getIndex(uint16_t x, uint16_t y) const {
	return y * getResolution().getWidth() + x;
}

size_t Display::getIndex(const Point &point) {
	return getIndex(point.getX(), point.getY());
}


