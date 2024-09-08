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



