#include "eightBitsPaletteBuffer.h"

EightBitsPaletteBuffer::EightBitsPaletteBuffer() : PaletteBuffer<uint8_t>(_palette) {

}

void EightBitsPaletteBuffer::renderPixel(int32_t x, int32_t y, uint8_t paletteIndex) {
	size_t byteIndex = y * _display->getWidth() + x;

	_buffer[byteIndex] = paletteIndex;
}

void EightBitsPaletteBuffer::flush() {
	const size_t pixelCount = _display->getWidth() * _display->getDriver()->getTransactionScanlines();
	size_t bufferIndex = 0;

	for (uint16_t y = 0; y < _display->getHeight(); y += _display->getDriver()->getTransactionScanlines()) {
		for (size_t i = 0; i < pixelCount; i++)
			_display->getDriver()->getTransactionBuffer()[i] = _palette[_buffer[bufferIndex++]];

		_display->getDriver()->flushTransactionBuffer(y);
	}
}

void EightBitsPaletteBuffer::allocate() {
	_bufferLength = _display->getWidth() * _display->getHeight();
	_buffer = new uint8_t[_bufferLength];
}