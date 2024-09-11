#include "eightBitsPaletteBuffer.h"
#include "display/bounds.h"

EightBitsPaletteBuffer::EightBitsPaletteBuffer() : PaletteBuffer<uint8_t>(_palette) {

}

void EightBitsPaletteBuffer::allocate() {
	_bufferLength = _display->getSize().getWidth() * _display->getSize().getHeight();
	_buffer = new uint8_t[_bufferLength];
}

void EightBitsPaletteBuffer::flush() {
	const size_t pixelCount = _display->getSize().getWidth() * _display->getDriver()->getTransactionScanlines();
	size_t bufferIndex = 0;

	for (uint16_t y = 0; y < _display->getSize().getHeight(); y += _display->getDriver()->getTransactionScanlines()) {
		for (size_t i = 0; i < pixelCount; i++)
			_display->getDriver()->getTransactionBuffer()[i] = _palette[_buffer[bufferIndex++]];

		_display->getDriver()->flushTransactionBuffer(y);
	}
}

void EightBitsPaletteBuffer::renderPixelUnchecked(const Point &point, uint8_t paletteIndex) {
	_buffer[getIndex(point)] = paletteIndex;
}

void EightBitsPaletteBuffer::renderHorizontalLineUnchecked(const Point &point, uint16_t width, uint8_t paletteIndex) {
	memset(_buffer + getIndex(point), paletteIndex, width);
}

void EightBitsPaletteBuffer::renderFilledRectangleUnchecked(const Bounds& bounds, uint8_t paletteIndex) {
	uint16_t y2 = bounds.getY2();

	for (uint16_t y = bounds.getY(); y < y2; y++)
		renderHorizontalLineUnchecked(Point(bounds.getX(), y), bounds.getWidth(), paletteIndex);
}