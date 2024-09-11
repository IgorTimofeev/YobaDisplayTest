#include "eightBitsPaletteBuffer.h"

EightBitsPaletteBuffer::EightBitsPaletteBuffer() : PaletteBuffer<uint8_t>(_palette) {

}

void EightBitsPaletteBuffer::allocate() {
	_bufferLength =  _display->getSize().getWidth() * _display->getSize().getHeight();
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

void EightBitsPaletteBuffer::renderPixel(const Point& point, uint8_t paletteIndex) {

	size_t byteIndex = point.getX() * _display->getSize().getWidth() + point.getY();

	_buffer[byteIndex] = paletteIndex;
}

void EightBitsPaletteBuffer::renderHorizontalLine(int32_t x, uint16_t width, uint8_t value) {

}

void EightBitsPaletteBuffer::renderFilledRectangle(const Point &point, const Size &size, uint8_t value) {

}
