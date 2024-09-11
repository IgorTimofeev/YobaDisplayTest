#pragma once

#include "paletteBuffer.h"
#include "display/display.h"

class EightBitsPaletteBuffer : public PaletteBuffer<uint8_t> {
	public:
		explicit EightBitsPaletteBuffer();

		void allocate() override;
		void flush() override;

	protected:
		void renderPixelUnchecked(const Point &point, uint8_t paletteIndex) override;
		void renderHorizontalLineUnchecked(const Point &point, uint16_t width, uint8_t paletteIndex) override;
		void renderFilledRectangleUnchecked(const Bounds& bounds, uint8_t paletteIndex) override;

	private:
		uint16_t _palette[256] {};
};

