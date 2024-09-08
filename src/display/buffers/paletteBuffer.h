#pragma once

#include <cstdlib>
#include <cstring>
#include "buffer.h"

enum class ColorDepth : uint8_t {
	One = 1,
	Two = 2,
	Four = 4,
	Eight = 8,
	Sixteen = 16,
	ThirtyTwo = 32
};

template<typename TColor>
class PaletteBuffer : public TypedBuffer<TColor> {
	public:
		explicit PaletteBuffer(uint16_t *palette);

		void setPaletteColor(size_t index, uint16_t value);

		void clear(TColor paletteIndex) override;
		void renderPixel(int32_t x, int32_t y, TColor value) override;

	protected:
		uint16_t* _palette;
};

template<typename TColor>
PaletteBuffer<TColor>::PaletteBuffer(uint16_t *palette) : _palette(palette)  {

}

template<typename TColor>
void PaletteBuffer<TColor>::setPaletteColor(size_t index, uint16_t value) {
	_palette[index] = value;
}

template<typename TColor>
void PaletteBuffer<TColor>::clear(TColor paletteIndex) {
	memset((uint16_t*) this->_buffer, (int) paletteIndex, this->_bufferLength);
}

template<typename TColor>
void PaletteBuffer<TColor>::renderPixel(int32_t x, int32_t y, TColor value) {

}
