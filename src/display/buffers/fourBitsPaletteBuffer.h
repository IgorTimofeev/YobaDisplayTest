#pragma once

#include "paletteBuffer.h"
#include "display/display.h"

class FourBitsPaletteBuffer : public PaletteBuffer<uint8_t>{
	public:
		explicit FourBitsPaletteBuffer();

		void flush() override;
		void renderPixel(int32_t x, int32_t y, uint8_t paletteIndex) override;

		void allocate() override;

	private:
		uint16_t _palette[16] {};
};
