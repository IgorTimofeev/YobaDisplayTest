#include "fourBitsPaletteBuffer.h"
#include "../display.h"

// 8x2 screen
// 1x1 pixel
// 0000-0000 0000-0000 0000-0000 0000-0000
// 0000-AABB 0000-0000 0000-0000 0000-0000
void FourBitsPaletteBuffer::setPixel(int32_t x, int32_t y, size_t paletteIndex) {
	size_t bitIndex = y * _display->getWidth() * (uint8_t) _colorDepth + x * (uint8_t) _colorDepth;
	size_t byteIndex = bitIndex / 8;

	auto bufferByte = _buffer[byteIndex];

	_buffer[byteIndex] =
		bitIndex % 8 == 0
		? (paletteIndex << 4) | (bufferByte & 0b1111)
		: (bufferByte & 0b11110000) | paletteIndex;
}

void FourBitsPaletteBuffer::flush() {

//			for (int i = 0; i < _transactionBufferSize; i++) {
//				_transactionBuffer[i] = to16Bit(RgbColor(color, 0, color));
//			}
//
//			color += 1;
//
	// Sending
	size_t bufferLength = _display->getWidth() * _display->getTransactionScanlines() * (uint8_t) _colorDepth / 8;
	size_t transactionBufferIndex;
	uint8_t bufferByte;
	size_t bufferIndex = 0;

	for (int transactionY = 0; transactionY < _display->getHeight(); transactionY += _display->getTransactionScanlines()) {
		// Copying screen buffer to transaction buffer

		transactionBufferIndex = 0;

		for (size_t i = 0; i < bufferLength; i++) {
			bufferByte = _buffer[bufferIndex++];

			_display->getTransactionBuffer()[transactionBufferIndex++] = _palette[bufferByte >> 4];
			_display->getTransactionBuffer()[transactionBufferIndex++] = _palette[bufferByte & 0b1111];
		}


		// Showing data on display
		_display->pushTransactions(transactionY);
	}
}
