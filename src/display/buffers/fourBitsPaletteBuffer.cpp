#include "fourBitsPaletteBuffer.h"

FourBitsPaletteBuffer::FourBitsPaletteBuffer() : PaletteBuffer<uint8_t>(_palette) {

}

void FourBitsPaletteBuffer::allocate() {
	_bufferLength = _display->getWidth() * _display->getHeight() / 2;
	_buffer = new uint8_t[_bufferLength];
}

// 0000-0000 0000-0000 0000-0000 0000-0000
// 0000-AABB 0000-0000 0000-0000 0000-0000
void FourBitsPaletteBuffer::renderPixel(int32_t x, int32_t y, uint8_t paletteIndex) {
	size_t bitIndex = y * _display->getWidth() * 4 + x * 4;
	size_t byteIndex = bitIndex / 8;

	auto& bufferByte = _buffer[byteIndex];

	bufferByte =
		bitIndex % 8 == 0
		? (paletteIndex << 4) | (bufferByte & 0b1111)
		: (bufferByte & 0b11110000) | paletteIndex;
}

void FourBitsPaletteBuffer::flush() {
	const size_t pixelCount = _display->getWidth() * _display->getDriver()->getTransactionScanlines() / 2;
	size_t bufferIndex = 0;
	uint8_t bufferByte;
	size_t transactionBufferIndex;

	for (uint16_t y = 0; y < _display->getHeight(); y += _display->getDriver()->getTransactionScanlines()) {
		transactionBufferIndex = 0;

		for (size_t i = 0; i < pixelCount; i++) {
			bufferByte = _buffer[bufferIndex++];

			_display->getDriver()->getTransactionBuffer()[transactionBufferIndex++] = _palette[bufferByte >> 4];
			_display->getDriver()->getTransactionBuffer()[transactionBufferIndex++] = _palette[bufferByte & 0b1111];
		}

		_display->getDriver()->flushTransactionBuffer(y);
	}
}