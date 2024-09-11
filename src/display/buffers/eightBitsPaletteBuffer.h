#pragma once

#include "paletteBuffer.h"
#include "display/display.h"

class EightBitsPaletteBuffer : public PaletteBuffer<uint8_t> {
	public:
		explicit EightBitsPaletteBuffer();

		void allocate() override;
		void flush() override;
		void renderPixel(const Point& point, uint8_t paletteIndex) override;
		void renderHorizontalLine(int32_t x, uint16_t width, uint8_t value) override;
		void renderFilledRectangle(const Point &point, const Size &size, uint8_t value) override;

	private:
		uint16_t _palette[256] {};
};

