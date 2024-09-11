#include "eightBitsPaletteDisplay.h"
#include "display/bounds.h"

EightBitsPaletteDisplay::EightBitsPaletteDisplay(Driver *driver, const Size &size) : PaletteDisplay<uint8_t>(driver, size, _govnoPalette) {

}

void EightBitsPaletteDisplay::allocate() {
	_bufferLength = getSize().getWidth() * getSize().getHeight();
	_buffer = new uint8_t[_bufferLength];
}

void EightBitsPaletteDisplay::flush() {
	const size_t pixelCount = getSize().getWidth() * getDriver()->getTransactionScanlines();
	size_t bufferIndex = 0;

	for (uint16_t y = 0; y < getSize().getHeight(); y += getDriver()->getTransactionScanlines()) {
		for (size_t i = 0; i < pixelCount; i++)
			getDriver()->getTransactionBuffer()[i] = _palette[_buffer[bufferIndex++]];

		getDriver()->flushTransactionBuffer(this, y);
	}
}

void EightBitsPaletteDisplay::renderPixelUnchecked(const Point &point, uint8_t paletteIndex) {
	_buffer[getIndex(point)] = paletteIndex;
}

void EightBitsPaletteDisplay::renderHorizontalLineUnchecked(const Point &point, uint16_t width, uint8_t paletteIndex) {
	memset(_buffer + getIndex(point), paletteIndex, width);
}

void EightBitsPaletteDisplay::renderFilledRectangleUnchecked(const Bounds& bounds, uint8_t paletteIndex) {
	uint16_t y2 = bounds.getY2();

	for (uint16_t y = bounds.getY(); y < y2; y++)
		renderHorizontalLineUnchecked(Point(bounds.getX(), y), bounds.getWidth(), paletteIndex);
}
