#pragma once

#include "paletteBuffer.h"
#include "buffer.h"

class EightBitsPaletteBuffer : public PaletteBuffer<uint8_t> {
	public:
		EightBitsPaletteBuffer(Driver *driver, const Size &resolution);

		void allocate() override;
		void flush() override;

	protected:
		void renderPixelUnchecked(const Point &point, uint8_t paletteIndex) override;
		void renderHorizontalLineUnchecked(const Point &point, uint16_t width, uint8_t paletteIndex) override;
		void renderFilledRectangleUnchecked(const Bounds& bounds, uint8_t paletteIndex) override;

	private:
		uint16_t _govnoPalette[256] {};
};

