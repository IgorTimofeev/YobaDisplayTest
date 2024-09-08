#pragma once

#include "paletteBuffer.h"

class EightBitsPaletteBuffer : public PaletteBuffer {
	public:
		explicit EightBitsPaletteBuffer() : PaletteBuffer(_govnoPalette, ColorDepth::Eight) {

		}

		void flush() override;

		void renderPixel(int32_t x, int32_t y, size_t paletteIndex) override;

	private:
		uint16_t _govnoPalette[256] {};
};