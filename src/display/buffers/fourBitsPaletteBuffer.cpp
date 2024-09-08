#include "fourBitsPaletteBuffer.h"
#include "../display.h"

// 8x2 screen
// 1x1 pixel
// 0000-0000 0000-0000 0000-0000 0000-0000
// 0000-AABB 0000-0000 0000-0000 0000-0000
void FourBitsPaletteBuffer::renderPixel(int32_t x, int32_t y, size_t paletteIndex) {
	size_t bitIndex = y * _display->getWidth() * (uint8_t) _colorDepth + x * (uint8_t) _colorDepth;
	size_t byteIndex = bitIndex / 8;

	auto& bufferByte = _buffer[byteIndex];

	bufferByte =
		bitIndex % 8 == 0
		? (paletteIndex << 4) | (bufferByte & 0b1111)
		: (bufferByte & 0b11110000) | paletteIndex;
}

void FourBitsPaletteBuffer::flush() {
	size_t bytesToCopy = _display->getWidth() * _display->getDriver()->getTransactionScanlines() * (uint8_t) _colorDepth / 8;
	uint8_t bufferByte;
	size_t bufferIndex = 0;

	size_t transactionBufferIndex;
	auto transactionBuffer = _display->getDriver()->getTransactionBuffer();

	_display->getDriver()->pushTransactions([&]() {
		transactionBufferIndex = 0;

		for (size_t i = 0; i < bytesToCopy; i++) {
			bufferByte = _buffer[bufferIndex++];

			transactionBuffer[transactionBufferIndex++] = _palette[bufferByte >> 4];
			transactionBuffer[transactionBufferIndex++] = _palette[bufferByte & 0b1111];
		}
	});
}
