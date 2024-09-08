#pragma once

#include "paletteBuffer.h"

class FourBitsPaletteBuffer : public PaletteBuffer {
	public:
		explicit FourBitsPaletteBuffer() : PaletteBuffer(_palette, ColorDepth::Four) {

		}

		void flush() override;

		void renderPixel(int32_t x, int32_t y, size_t paletteIndex) override;

	private:
		uint16_t _palette[16] {};
};