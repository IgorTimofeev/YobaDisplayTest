#pragma once

#include "paletteBuffer.h"
#include "../display.h"

class FourBitsPaletteBuffer : public PaletteBuffer<uint8_t>{
	public:
		explicit FourBitsPaletteBuffer();

		void flush() override;
		void renderPixel(int32_t x, int32_t y, uint8_t paletteIndex) override;

	protected:
		size_t calculateBufferLength() override;

	private:
		uint16_t _palette[16] {};
};

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
	size_t bytesToCopy = _display->getWidth() * _display->getDriver()->getTransactionScanlines() / 2;
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

size_t FourBitsPaletteBuffer::calculateBufferLength() {
	return _display->getWidth() * _display->getHeight() / 2;
}

FourBitsPaletteBuffer::FourBitsPaletteBuffer() : PaletteBuffer<uint8_t>(_palette) {

}
