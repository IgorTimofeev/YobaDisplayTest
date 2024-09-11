#include "eightBitsPaletteBuffer.h"
#include "yoba/bounds.h"

EightBitsPaletteBuffer::EightBitsPaletteBuffer(Driver *driver, const Size& resolution) : PaletteBuffer<uint8_t>(driver, resolution, _govnoPalette) {

}

void EightBitsPaletteBuffer::allocate() {
	_bufferLength = getSize().getWidth() * getSize().getHeight();
	_buffer = new uint8_t[_bufferLength];
}

void EightBitsPaletteBuffer::flush() {
	const size_t pixelCount = getSize().getWidth() * _driver->getSettings().getTransactionBufferHeight();
	size_t bufferIndex = 0;

	for (uint16_t y = 0; y < getSize().getHeight(); y += _driver->getSettings().getTransactionBufferHeight()) {
		for (size_t i = 0; i < pixelCount; i++)
			_driver->getTransactionBuffer()[i] = _palette[_buffer[bufferIndex++]];

		_driver->flushTransactionBuffer(this, y);
	}
}

void EightBitsPaletteBuffer::renderPixelUnchecked(const Point &point, uint8_t paletteIndex) {
	_buffer[getIndex(point)] = paletteIndex;
}

void EightBitsPaletteBuffer::renderHorizontalLineUnchecked(const Point &point, uint16_t width, uint8_t paletteIndex) {
	memset(_buffer + getIndex(point), paletteIndex, width);
}

void EightBitsPaletteBuffer::renderFilledRectangleUnchecked(const Bounds& bounds, uint8_t paletteIndex) {
	uint8_t* bufferPtr = _buffer + getIndex(bounds.getTopLeft());
	uint16_t y2 = bounds.getY2();

	for (uint16_t y = bounds.getY(); y < y2; y++)
		memset(bufferPtr += getSize().getWidth(), paletteIndex, bounds.getWidth());
}
