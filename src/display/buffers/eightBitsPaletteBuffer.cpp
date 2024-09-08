#include "EightBitsPaletteBuffer.h"
#include "../display.h"

void EightBitsPaletteBuffer::renderPixel(int32_t x, int32_t y, size_t paletteIndex) {
	size_t byteIndex = y * _display->getWidth() + x;

	_buffer[byteIndex] = paletteIndex;
}

void EightBitsPaletteBuffer::flush() {
	size_t pixelsToCopy = _display->getWidth() * _display->getDriver()->getTransactionScanlines();
	size_t bufferIndex = 0;

	auto transactionBuffer = _display->getDriver()->getTransactionBuffer();

	_display->getDriver()->pushTransactions([&]() {
		for (size_t i = 0; i < pixelsToCopy; i++) {
			transactionBuffer[i] = _palette[_buffer[bufferIndex++]];
		}
	});
}
