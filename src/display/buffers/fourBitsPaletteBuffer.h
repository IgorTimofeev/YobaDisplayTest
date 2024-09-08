#pragma once

#include "paletteBuffer.h"

class FourBitsPaletteBuffer : public PaletteBuffer {
	public:
		explicit FourBitsPaletteBuffer() : PaletteBuffer(_palette, ColorDepth::Four) {

		}

		void flush() override;

		void setPixel(int32_t x, int32_t y, size_t paletteIndex);

	private:
		const uint8_t _colorDepth = 4;
		uint16_t _palette[16] {};
};