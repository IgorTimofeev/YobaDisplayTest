#pragma once

#include "paletteBuffer.h"
#include "../display.h"

class EightBitsPaletteBuffer : public PaletteBuffer<uint8_t> {
	public:
		explicit EightBitsPaletteBuffer();

		void flush() override;

		void renderPixel(int32_t x, int32_t y, uint8_t paletteIndex) override;

	protected:
		size_t calculateBufferLength() override;

	private:
		uint16_t _palette[256] {};
};

EightBitsPaletteBuffer::EightBitsPaletteBuffer() : PaletteBuffer<uint8_t>(_palette) {

}

void EightBitsPaletteBuffer::renderPixel(int32_t x, int32_t y, uint8_t paletteIndex) {
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

size_t EightBitsPaletteBuffer::calculateBufferLength() {
	return _display->getWidth() * _display->getHeight();
}
