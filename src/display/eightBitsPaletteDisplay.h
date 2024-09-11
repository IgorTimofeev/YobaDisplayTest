#pragma once

#include "paletteDisplay.h"
#include "display/display.h"

class EightBitsPaletteDisplay : public PaletteDisplay<uint8_t> {
	public:
		EightBitsPaletteDisplay(Driver *driver, const Size &resolution);

		void allocate() override;
		void flush() override;

	protected:
		void renderPixelUnchecked(const Point &point, uint8_t paletteIndex) override;
		void renderHorizontalLineUnchecked(const Point &point, uint16_t width, uint8_t paletteIndex) override;
		void renderFilledRectangleUnchecked(const Bounds& bounds, uint8_t paletteIndex) override;

	private:
		uint16_t _govnoPalette[256] {};
};

