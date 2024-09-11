#include "eightBitsPaletteDisplay.h"
#include "display/bounds.h"

EightBitsPaletteDisplay::EightBitsPaletteDisplay(Driver *driver, const Size& resolution) : PaletteDisplay<uint8_t>(driver, resolution, _govnoPalette) {

}

void EightBitsPaletteDisplay::allocate() {
	_bufferLength = getResolution().getWidth() * getResolution().getHeight();
	_buffer = new uint8_t[_bufferLength];
}

void EightBitsPaletteDisplay::flush() {
	const size_t pixelCount = getResolution().getWidth() * _driver->getTransactionBufferHeight();
	size_t bufferIndex = 0;

	for (uint16_t y = 0; y < getResolution().getHeight(); y += _driver->getTransactionBufferHeight()) {
		for (size_t i = 0; i < pixelCount; i++)
			_driver->getTransactionBuffer()[i] = _palette[_buffer[bufferIndex++]];

		_driver->flushTransactionBuffer(this, y);
	}
}

void EightBitsPaletteDisplay::renderPixelUnchecked(const Point &point, uint8_t paletteIndex) {
	_buffer[getIndex(point)] = paletteIndex;
}

void EightBitsPaletteDisplay::renderHorizontalLineUnchecked(const Point &point, uint16_t width, uint8_t paletteIndex) {
	memset(_buffer + getIndex(point), paletteIndex, width);
}

void EightBitsPaletteDisplay::renderFilledRectangleUnchecked(const Bounds& bounds, uint8_t paletteIndex) {
	uint8_t* bufferPtr = _buffer + getIndex(bounds.getTopLeft());
	uint16_t y2 = bounds.getY2();

	for (uint16_t y = bounds.getY(); y < y2; y++)
		memset(bufferPtr += getResolution().getWidth(), paletteIndex, bounds.getWidth());
}
