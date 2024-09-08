#pragma once

#include <cstdlib>
#include "buffer.h"

enum class ColorDepth : uint8_t {
	One = 1,
	Two = 2,
	Four = 4,
	Eight = 8,
	Sixteen = 16,
	ThirtyTwo = 32
};

class PaletteBuffer : public Buffer {
	public:
		explicit PaletteBuffer(uint16_t *palette, ColorDepth colorDepth);

		void setPaletteColor(size_t index, uint16_t value);

		void setDisplay(Display *display) override;

	protected:
		uint16_t* _palette;
		ColorDepth _colorDepth;
};